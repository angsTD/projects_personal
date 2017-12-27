/*
 * dxcm_ptr_q.h
 *
 *  Created on: Feb 19, 2017
 *      Author: Angelo
 */

#ifndef DXCM_PTR_Q
#define DXCM_PTR_Q

#include "stddef.h"
#include "stdint.h"

typedef struct q_private_t
{
  unsigned tail; /* Position at which to insert the next q element */
  unsigned head; /* Position at which element can be dequeued */
  unsigned capacity; /* Maximum number of elements which can be stored */
  unsigned size; /* Number of elements in the queue right now */
  void*    ptrs[1]; /* array of pointers follows the header information.
                       Array must contain at least 1 element but we will
                       index past this if additional memory is provided */
} q_obj_t;

#define Q_REQUIRED_MEM_SIZE(num) (sizeof(q_obj_t) + num*sizeof(void*))

/* q_t is a pointer to a structure that we know nothing about */
typedef struct q_private_t* q_t;

q_t q_init(void* memory, unsigned size);

int q_enqueue(q_t q, void* ptr);

int q_dequeue(q_t q, void* ptr);
/*
typedef struct
{
    void** ptrs;
    uint32_t head,tail;
    uint32_t capacity,size;
    uint32_t addwrap, remwrap;
}obj_t;


obj_t* queue_init(void* mem,uint32_t size);

int enqueue(obj_t* Q,void *ptr);

int dequeue(obj_t* Q, void *ptr);
*/
#endif
