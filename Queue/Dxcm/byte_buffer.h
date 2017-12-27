/**
 * Copyright:	Copyright © 2016 by Dexcom, Inc.  All rights reserved.
 *
 * Initially Created: DexCom/Jim Amidei 09/12/2016
 * ------------------------------------------------------------------------- */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define QUEUE_OK           ( 0)
#define QUEUE_INVALID_BB   (-1)
#define QUEUE_CORRUPT_BB   (-2)
#define QUEUE_INVALID_PARM (-3)
#define QUEUE_OVERFLOW     ( 1)
#define QUEUE_UNDERFLOW    ( 2)

typedef struct
{
  /* All data is considered private and should only be accessed by the
     functions defined below */
  unsigned magic;
  void** ppBuf;
  unsigned buf_len;
  unsigned inpos;
  unsigned outpos;
  unsigned overflow;
  unsigned obj_size;
  unsigned obj_count;
} queue_t;

/* Initialize a byte buffer with the provided memory.  */
int queue_init(queue_t* bb, void** ppbuf, unsigned buf_size, uint32_t data_size);

/* Push data into the buffer. Non-error return values include OK and OVERFLOW */
int queue_push(queue_t* bb, void* pdata);

/* Pop data out the buffer. Non-error return values include OK and UNDERFLOW.
   data is allowed to be NULL to allow pop'ing without keeping popped data */
int queue_pop(queue_t* bb, void **ppdata);

/* Read and reset the count of overflow push's that have occurred since the
   last invocation of this function */
int queue_read_and_reset_overflow(queue_t* bb, unsigned *ovr);

#ifdef __cplusplus
}
#endif
