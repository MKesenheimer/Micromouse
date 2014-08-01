
#ifndef _CQUEUE_H_
#define _CQUEUE_H_

#include "common.h"

#define QUEUE_MAX 8

typedef struct cqueue {
	U8 data[QUEUE_MAX];
	U8 head;
	U8 tail; // front = rear when empty
	U8 size;
} CQueue;

void cq_init(CQueue*);
BOOL cq_queue(int, CQueue*);
U8 cq_dequeue(CQueue*);
void cq_print(const CQueue*);
#endif
