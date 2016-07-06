/*
 * circularQueue.h
 *
 * Created: 7/2/2016 1:02:41 PM
 *  Author: Benjamin Nold
 *  This appears to be working on 7/3. I have only enqueued and dequeued
 */ 


#ifndef CIRCULARQUEUE_H_
#define CIRCULARQUEUE_H_

#define QUEUE_SIZE 100
struct cirQueue{
	unsigned char theQueue[QUEUE_SIZE];
	volatile char *qFront, *qRear;
};


void setupQueue(struct cirQueue *q);
void enqueue(struct cirQueue *q, char c);
char dequeue(struct cirQueue *q);
char isEmpty(struct cirQueue *q);





#endif /* CIRCULARQUEUE_H_ */