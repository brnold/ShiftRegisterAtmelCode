/*
 * circularQueue.c
 *
 * Created: 7/2/2016 12:54:39 PM
 *  Author: Benjamin Nold
 */ 
 #include "circularQueue.h"
 #include <util/delay.h>
 
 void setupQueue(struct cirQueue *q){
	q->qFront = &q->theQueue[0];
	q->qRear = &q->theQueue[0];
	q->queueDepth = 0;
 }

 void enqueue(struct cirQueue *q, char c)
 {
	q->queueDepth++;
	*q->qFront = c;

	if(q->qFront >= &q->theQueue[100])
		q->qFront = &q->theQueue[0];
	else
		q->qFront++;
 }

 char dequeue(struct cirQueue *q)
 {
	//it's up to the user to make sure that the queue is not empty before calling
	char temp;
	//check if you can deqeue
	
	if(q->queueDepth == 0)
		return 0;
	
	q->queueDepth--;

	temp = *q->qRear;
	if(q->qRear >= &q->theQueue[100])
		q->qRear = & q->theQueue[0];
	else
		q->qRear++;

	return temp;
 }

 char isEmpty(struct cirQueue *q){
	if(q->qFront == q->qRear){
		return 1;
	}
	else{
		return 0;
	}
 }
 
 char queueDepth(struct cirQueue *q){
	 
	 return q->queueDepth;
 }