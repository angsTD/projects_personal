/**
 * Copyright:	Copyright © 2016 by Dexcom, Inc.  All rights reserved.
 *
 * Initially Created: DexCom/Jim Amidei 09/12/2016
 * ------------------------------------------------------------------------- */

#include <string.h>
#include "byte_buffer.h"
#include "app_util_platform.h"

#define _MAGIC (0x43f00231) /* Random stuff */

/*----------------------------------------------------------------------------*/
int queue_init(queue_t* bb, void** ppbuf, unsigned buf_size, uint32_t data_size)
{
  if (!bb)                  return QUEUE_INVALID_BB;
  if (!ppbuf)               return QUEUE_INVALID_PARM;
  if (!buf_size)            return QUEUE_INVALID_PARM;
  if (!data_size)           return QUEUE_INVALID_PARM;

  if (bb->magic != _MAGIC)
  {
    memset(bb, 0, sizeof(*bb));
    bb->ppBuf = ppbuf;
    bb->buf_len = buf_size*data_size;
    bb->obj_size = data_size;
    bb->magic = _MAGIC;
    bb->obj_count = buf_size;
  }
  else
  {
    /* Do not allow re-init attempts with different buffer or size */
    if (bb->ppBuf            != ppbuf)      return QUEUE_INVALID_PARM;
    if (bb->buf_len != buf_size) return QUEUE_INVALID_PARM;
  }
  return QUEUE_OK;
}

/*----------------------------------------------------------------------------*/
int queue_push(queue_t* bb, void* pdata)
{
  if (!bb)                 return QUEUE_INVALID_BB;
  if (bb->magic != _MAGIC) return QUEUE_CORRUPT_BB;

  int result = QUEUE_OK;

  CRITICAL_REGION_ENTER();
  unsigned used = bb->inpos - bb->outpos;
  if (used <= (bb->obj_count-1))
  {
    unsigned indx = (bb->inpos & (bb->obj_count-1));
    memcpy((void*)((unsigned)bb->ppBuf + indx*bb->obj_size),pdata,sizeof(uint8_t)*bb->obj_size);
    bb->inpos++;
  }
  else
  {
    UART_prt("used:%d, bb->inpos: %d bb->outpos: %d\n",used,bb->inpos,bb->outpos);
    bb->overflow += bb->obj_size;
    result = QUEUE_OVERFLOW;
  }
  CRITICAL_REGION_EXIT();
  return result;
}

/*----------------------------------------------------------------------------*/
int queue_pop(queue_t* bb, void** ppdata)
{
  if (!bb)                 return QUEUE_INVALID_BB;
  if (bb->magic != _MAGIC) return QUEUE_CORRUPT_BB;

  int result = QUEUE_UNDERFLOW;

  CRITICAL_REGION_ENTER();
  unsigned avail = bb->inpos - bb->outpos;
  if (avail)
  {
    unsigned indx = (bb->outpos & (bb->obj_count-1));
    *ppdata = (void*)(((unsigned)bb->ppBuf) + indx*bb->obj_size);
    bb->outpos++;
    result = QUEUE_OK;
  }
  else
      *ppdata = NULL;
  CRITICAL_REGION_EXIT();
  return result;
}

/*----------------------------------------------------------------------------*/
int queue_read_and_reset_overflow(queue_t* bb, unsigned *ovr)
{
  if (!bb)                 return QUEUE_INVALID_BB;
  if (bb->magic != _MAGIC) return QUEUE_CORRUPT_BB;
  CRITICAL_REGION_ENTER();
  if (ovr) *ovr = bb->overflow;
  bb->overflow = 0;
  CRITICAL_REGION_EXIT();
  return QUEUE_OK;
}
