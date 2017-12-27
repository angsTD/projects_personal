/*
 ============================================================================
 Name        : Queue.c
 Author      : Angelo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "stdint.h"
#include "dxcm_ptr_q.h"

int main(void) {

	uint8_t mempool[Q_REQUIRED_MEM_SIZE(10)];

	q_t current = q_init(mempool,Q_REQUIRED_MEM_SIZE(10));
	uint32_t ptrArray[15] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	for(int32_t index = 0; index<5; index++)
	{
		q_enqueue(current,ptrArray+index);
	}

	for(int32_t index = 0; index<5; index ++)
	{
		uint32_t * ptr = 0;
		q_dequeue(current,ptr);
		printf("dequeued element is: %d \n",*ptr);
	}
	//Q_REQUIRED_MEM_SIZE(10);
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
