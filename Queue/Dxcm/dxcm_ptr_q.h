#ifndef DXCM_PTR_Q
#define DXCM_PTR_Q

#include "stddef.h"
#include "stdint.h"

/* q_t is a pointer to a structure that we know nothing about */
typedef struct q_private_t* q_t;

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