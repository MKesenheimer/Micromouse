#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../inc/common.h"
#include "../inc/cqueue.h"

PRIVATE void inc_index(U8* index);

void cq_init(CQueue* cq) {
	cq->size = 0;
	cq->head = cq->tail = 0; // head=tail when empty or only 1 element
}

/**
 * push data to back of queue
 */
BOOL cq_queue(int data, CQueue* cq) {
	//assert(cq->size != QUEUE_MAX); // or do something else
	if (cq->size >= QUEUE_MAX)
		return FALSE;
	if (cq->size != 0) // if not 0 element, head=tail
		inc_index(&(cq->tail));
	cq->data[cq->tail] = data;
	cq->size++;
	return TRUE;
}

/**
 * pop and return value from front of queue
 */
U8 cq_dequeue(CQueue* cq)
{
	U8 data = cq->data[cq->head];
	//assert(cq->size != 0);
	if (cq->size != 1)
		inc_index(&(cq->head));
	cq->size--;
	return data;
}

void cq_print(const CQueue* cq) {
	U8 index = cq->head;
	if (cq->size == 0) return;
	if (cq->size == 1) {
		fprintf(stdout, "%d ", cq->data[index]);
		return;
	}
	while (index != cq->tail) {
		fprintf(stdout, "%d ", cq->data[index]);
		inc_index(&index);
	}
	fprintf(stdout, "%d ", cq->data[index]);
}

/**
 * increment index with wrap-around
 */
PRIVATE void inc_index(U8* index) {
	*index = (*index + 1 < QUEUE_MAX) ? *index+1 : 0;
}

/*
int main(void) {
	CQueue cq;
	cq_init(&cq);
	cq_push(1, &cq);
	cq_push(2, &cq);
	cq_push(3, &cq);
	cq_push(4, &cq);
	cq_push(5, &cq);
	cq_push(6, &cq);
	cq_push(7, &cq);
	cq_push(8, &cq);
	fprintf(stdout, "pop: %d\n", cq_pop(&cq));
	fprintf(stdout, "pop: %d\n", cq_pop(&cq));
	cq_push(9, &cq);
	cq_push(10, &cq);
	fprintf(stdout, "pop: %d\n", cq_pop(&cq));
	fprintf(stdout, "pop: %d\n", cq_pop(&cq));
	fprintf(stdout, "pop: %d\n", cq_pop(&cq));
	fprintf(stdout, "pop: %d\n", cq_pop(&cq));
	fprintf(stdout, "pop: %d\n", cq_pop(&cq));
	fprintf(stdout, "pop: %d\n", cq_pop(&cq));
	fprintf(stdout, "pop: %d\n", cq_pop(&cq));
	fprintf(stdout, "pop: %d\n", cq_pop(&cq));
	cq_push(0, &cq);
	cq_print(&cq);
	fprintf(stdout, "\n");

	return 0;
}
*/

