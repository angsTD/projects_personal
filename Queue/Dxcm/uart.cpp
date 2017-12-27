/**
 * Copyright:	Copyright © 2014 by Dexcom, Inc.  All rights reserved.
 *
 * Created by Alisha Roger: 9/24/2015
 * ----------------------------------------------------------------------------
 * HAL_UART.cpp
 * ------------------------------------------------------------------------- */
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "uart.h"

extern "C" {
#include "app_uart.h"
#include "nrf_uart.h"
#include "nrf_delay.h"
#include "nrf52_bitfields.h"
}


#include "dio.h"
#include "CgmController.h"
#include "CI.hpp"
#include "byte_buffer.h"
#include "dxcm_delay.h"

static bool _uart_drvr_started = false; /* Driver is started (or not) */

/* These are the buffers for the app_uart driver to use */
static uint8_t _rx_buf[256];
static uint8_t _tx_buf[64];
static const app_uart_buffers_t _buffer =
{
  /* .rx_buf =      */ _rx_buf,
  /* .rx_buf_size = */ sizeof(_rx_buf),
  /* .tx_buf =      */ _tx_buf,
  /* .tx_buf_size = */ sizeof(_tx_buf),
};

/* The configuration parameters for the app driver */
static const app_uart_comm_params_t _uart_comm_params =
{
  /* .rx_pin_no = */    UART_RX_PIN,
  /* .tx_pin_no = */    UART_TX_PIN,
  /* .rts_pin_no = */   (uint8_t)UART_PIN_DISCONNECTED,
  /* .cts_pin_no = */   (uint8_t)UART_PIN_DISCONNECTED,
  /* .flow_control = */ APP_UART_FLOW_CONTROL_DISABLED,
  /* .use_parity = */   false,
  /* .baud_rate = */    NRF_UART_BAUDRATE_115200
};

/* When we detect an error we set a flag and keep track of the number of
   transmit characters that we inhibit. We also record the error conditions
   so that it can be described later (as part of uart hardware reset) */
static bool     _error_pending = false;
static uint32_t _error_reason;

/* This buffer is used to capture data from the app driver before tearing it
   down and restarting. We also keep track of overflow data that did not
   fit into this buffer. */
static uint8_t       _rx_drain_buf[sizeof(_rx_buf)];
static queue_t _rx_drain;

/* This buffer is used to cache UART characters from interrupt context until
   they can be flushed from the main context */
static uint8_t       _tx_cache_buf[256]; /* Must be power of two */
static queue_t _tx_cache;

static void _send_bytes(const uint8_t* data, unsigned num, unsigned xlate);

/*----------------------------------------------------------------------------*/
static void _prtf(const char* format, ...)
{
  static uint8_t _buf[100];

  /* Format into our buffer */
  va_list args;
  va_start(args, format);
  int num = vsnprintf((char*)_buf, sizeof(_buf), format, args);
  va_end(args);

  /* Detect strings that would have overrun the buffer and make sure that
     likewise do not try to output more than what the buffer contains */
  if (num > sizeof(_buf)) num = sizeof(_buf);

  /* Send to the UART all that was produced */
  _send_bytes(_buf, num, 1);
}

/*----------------------------------------------------------------------------*/
static void _handle_errors(void)
{
  uint8_t byte;

  if (!_error_pending) return;

  /* Drain received buffer */
  while (NRF_SUCCESS == app_uart_get(&byte))
    queue_push(&_rx_drain, (void*)&byte);

  /* Let TX fully drain before reset */
  nrf_delay_us(87 * sizeof(_tx_buf)); /* ~87 usec per byte assuming 115200 */

  /* Capture error reason and reset the driver */
  uint32_t code = _error_reason;
  UART_stop();  /* Shut down UART */
  UART_start(); /* Restart UART */
  _error_pending = false;

  static bool _nested; /* Protect from endless recursion */
  if (!_nested)
  {
    _nested = true;

    /* Display message about the reset and indicate number of RX lost */
    unsigned ovf;
    queue_read_and_reset_overflow(&_rx_drain, &ovf);
    _prtf("\n!!! UART has been reset (code = 0x%8.8x %u)\n", code, ovf);
    if (code & UART_ERRORSRC_OVERRUN_Msk)
      _prtf("   reason includes OVERRUN\n");
    if (code & UART_ERRORSRC_PARITY_Msk)
      _prtf("   reason includes PARITY\n");
    if (code & UART_ERRORSRC_FRAMING_Msk)
      _prtf("   reason includes FRAMING\n");
    if (code & UART_ERRORSRC_BREAK_Msk)
      _prtf("   reason includes BREAK\n");
    _nested = false;
  }
}

/*----------------------------------------------------------------------------*/
static int _get_byte(uint8_t *data)
{
  /* First detect and correct UART errors (only if UART is initted) */
  if (_uart_drvr_started) _handle_errors();

  /* Get data from the rx drain first, then from the driver, and, if neither
     return an error. If either source provides a byte then store it in the
     caller provided memory and return 0, indicating success */
  uint8_t* pbyte;
  uint8_t byte;
    
  if(QUEUE_OK == queue_pop(&_rx_drain, (void**)&pbyte))
  {
      if (data) *data = *pbyte; /* Store in data if provided */
      return 0;               /* Indicate success to caller */
  }
  if(NRF_SUCCESS == app_uart_get(&byte))
  {
    if (data) *data = byte; /* Store in data if provided */
    return 0;               /* Indicate success to caller */
  }
  
  return -1;

  
}

/* ---------------------------------------------------------------------------*/
/* Returns a pointer to anbled command if it is ready to be executed */
static uint8_t* _parse_cmd(uint8_t data)
{
  #define _CMD_START      (0xaa) /* This is byte0 of a valid command */
  #define _CMD_LENGTH_IDX (1)    /* Byte1 of a command indicates data length */
  #define _CMD_CMD_IDX    (2)    /* Byte2 of a command is the opcode */
  static unsigned _cmd_len;  /* The length of the command */
  static unsigned _cmd_pos;  /* The current position of the assembled command */
  static uint8_t  _cmd[256]; /* A partially (or fully) built cmd_pkt */
  static bool    _cmd_in_progress = false; /* Command is being received */

  /* Detect the start of a command */
  if (!_cmd_in_progress && (data == _CMD_START))
  {
    _cmd_in_progress  = true;
    _cmd_len = 0;
    _cmd_pos = 0;
  }

#if INCLUDE_DEVELOPMENT_HACKS
  /* If we get here and command is not in progress than we can interpret
     the byte as a 1-byte special UART command. Do this here and then
     get out. */
  if (!_cmd_in_progress)
  {
    switch (data)
    {
      case _CMD_START:
      default:         break; /* Nothing to do */ 
 
      case '~': UART_prt("events : 0x%8.8x\n", App::gPendingEvents.u.reg);
      case '`': dxcm_delay_msec(20); break;
      case '!': UART_stop(); break; /* Shoot ourselves in the head */
      case '?': CI_print_cmd_tbl(1); break; /* print filtered cmd table */
      case '/': CI_print_cmd_tbl(0); break; /* print un-filtered cmd table */
    }
    return NULL;
  }
#endif

  /* If we are not in command then continue trying to find a command */
  if (!_cmd_in_progress) return NULL;

  /* The byte at location _CMD_LENGTH_IDX indicates the number
     of data bytes (the command is 2 bytes longer than this) */
  if (_cmd_pos == _CMD_LENGTH_IDX)
  {
    _cmd_len = data + 2;
    if (_cmd_len > sizeof(_cmd))
    {
      _cmd_in_progress = false;
      UART_prt("UART CMD cannot be stored in available buffer!!!\n");
      return NULL;
    }
  }

  /* Store bytes in the command buffer */
  _cmd[_cmd_pos++] = data;

  /* Detect incomplete command (thus far); and continue if necessary */
  if (_cmd_pos <= _CMD_CMD_IDX) return NULL;
  if (_cmd_pos != _cmd_len) return NULL;

  /* Command is complete and checksum is good */
  _cmd_in_progress = false;
  return _cmd;
}

/* ---------------------------------------------------------------------------*/
static void _uart_evt(app_uart_evt_t *evt)
{
  switch(evt->evt_type)
  {
    case APP_UART_COMMUNICATION_ERROR:
      /* If there are no other pending errors then log this one */
      if (!_error_pending)
      {
        _error_pending = true;
        _error_reason = evt->data.error_communication;
      }
      /* fallthrough is intentional */

    case APP_UART_DATA_READY:
      /* Ask main loop to service UART */
      App::gPendingEvents.u.b.MAIN_LOOP_UART_SVC = 1;
      break;

    case APP_UART_DATA:
    case APP_UART_TX_EMPTY:
    case APP_UART_FIFO_ERROR:
    default: break;
  }
}

/*----------------------------------------------------------------------------*/
void UART_start()
{
  if (_uart_drvr_started) return; /* Avoid reinitialize */

  queue_init(&_rx_drain, (void**)&_rx_drain_buf, sizeof(_rx_drain_buf), 1);
  queue_init(&_tx_cache, (void**)&_tx_cache_buf, sizeof(_tx_cache_buf), 1);
  uint32_t ecode = app_uart_init(&_uart_comm_params,
                                 (app_uart_buffers_t*)&_buffer,
                                 _uart_evt, APP_IRQ_PRIORITY_LOW);
  APP_ERROR_CHECK(ecode);
  _uart_drvr_started = true;
}

/*----------------------------------------------------------------------------*/
void UART_stop()
{
  if (!_uart_drvr_started) return; /* Avoid re-un-initialize */
  _uart_drvr_started = false;
  app_uart_close();
}

/*----------------------------------------------------------------------------*/
void UART_tx_drain(void)
{
  if (!_uart_drvr_started) return; /* No need to drain if we are stopped */
  dxcm_delay_usec(90 * sizeof(_tx_buf)); /* ~86.8 usec/byte = 11520 Byte/sec */
}

/*----------------------------------------------------------------------------*/
static void _transmit_from_non_isr(uint8_t c)
{
  _handle_errors();
  while (NRF_ERROR_NO_MEM == app_uart_put(c)) ;
}

/*----------------------------------------------------------------------------*/
static void _transmit(uint8_t c)
{
  if (!_uart_drvr_started) return;

  if (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)
  {
    queue_push(&_tx_cache, (void*)&c);
    App::gPendingEvents.u.b.MAIN_LOOP_UART_SVC = 1;
  }
  else
  {
    _transmit_from_non_isr(c);
  }
}

/*----------------------------------------------------------------------------*/
void _send_bytes(const uint8_t* bytes, unsigned length, unsigned xlate)
{
  if (!_uart_drvr_started) return;
  if (!xlate) /* Binary mode */
  {
    while (length--)
      _transmit(*bytes++);
  }
  else /* Text mode - remove control characters and do LF to CRLF translation */
  {
    while (length--)
    {
      uint8_t byte = *bytes++;
      if (isprint(byte))
      {
        _transmit(byte);
      }
      else if (byte == '\n')
      {
        _transmit('\r');
        _transmit(byte);
      }
      else if ((byte >= 017) && (byte <= 037))
      {
        _transmit('\033');
        _transmit('[');
        if (byte == 017) _transmit('0');
        else
        {
          if (byte <= 027) _transmit('3');
          else             _transmit('4');
          _transmit('0' + (byte & 7));
        }
        _transmit('m');
      }
    }
  }
}

/*----------------------------------------------------------------------------*/
void _send_1byte_msg(uint8_t byte)
{
  uint8_t msg[4];
  msg[0] = 0xaa;
  msg[1] = 0x02;
  msg[2] = byte;
  msg[3] = byte ^ 2;
  _send_bytes(msg, 4, 0);
}

/*----------------------------------------------------------------------------*/
void UART_send_ready(void)
{
  _send_1byte_msg(0xbb);
}

/*----------------------------------------------------------------------------*/
void UART_send_exit_storage_mode(void)
{
  _send_1byte_msg(0xcc);
}

/*----------------------------------------------------------------------------*/
static void _stream_data(const void* data, unsigned bytes)
{
  const uint8_t* d8 = (const uint8_t*)data;
  while (bytes)
  {
    unsigned num = bytes;
    if (num > 48) num = 48;

    uint8_t hdr[2];
    hdr[0] = 0xee;
    hdr[1] = num;
    _send_bytes(hdr, 2, 0);
    _send_bytes(d8, num, 0);
    bytes -= num;
    d8 += num;
  }
}

/*----------------------------------------------------------------------------*/
void UART_prt(const char* format, ...)
{
  static uint8_t _buf[256];

  if (!_uart_drvr_started) return;

  /* Format into our buffer */
  va_list args;
  va_start(args, format);
  int num = vsnprintf((char*)_buf, sizeof(_buf), format, args);
  va_end(args);

  /* Detect strings that would have overrun the buffer and make sure that
     likewise do not try to output more than what the buffer contains */
  if (num > sizeof(_buf)) num = sizeof(_buf);

  /* Send to the UART all that was produced */
  _send_bytes(_buf, num, 1);
}

/*----------------------------------------------------------------------------*/
void UART_service(void)
{
  App::gPendingEvents.u.b.MAIN_LOOP_UART_SVC = 0;

  /* Transmit any TX bytes that were held in buffer from ISR printfs */
  uint8_t* pbyte;
  while (QUEUE_OK == queue_pop(&_tx_cache, (void**)&pbyte))
    _transmit_from_non_isr(*pbyte);

  /* Notice of any bytes due to TX from ISR overflow */
  unsigned ovf;
  queue_read_and_reset_overflow(&_tx_cache, &ovf);
  if (ovf) UART_prt("\n%u bytes of UART TX have been lost\n", ovf);


  uint8_t byte;
  /* Handle any incoming data and send to CI when ready */
  while (1)
  {
    /* Receive a byte of data - when stream dries up we exit the loop */
    if (_get_byte(&byte)) break;

    /* Pass byte into command parser and try to assemble a command */
    uint8_t *cmd = _parse_cmd(byte);
    if (cmd)
    {
      static uint8_t _rsp[256];
      if (CI_cmd_result_protocol == CI_cmd(cmd, _rsp, _stream_data, true))
      {
        /* We get here if protocol CRC mismatch happens */
        UART_msg(red, blk, "UART command has chksum error (0x%2.2x)", cmd[2]);

        /* Send something back so that JUnit knows that we got something */
        _send_1byte_msg(0xdd);
      }
      else
        _send_bytes(_rsp, _rsp[1]+2, 0);
    }
  }
}
