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
 }

 void enqueue(struct cirQueue *q, char c)
 {
	*q->qFront = c;

	if(q->qFront >= &q->theQueue[100])
		q->qFront = &q->theQueue[0];
	else
		q->qFront++;
 }

 char dequeue(struct cirQueue *q)
 {
	char temp;
	//check if you can deqeue
	while(isEmpty(q)==0){
		_delay_us(1); //#fixme, will not seem to work with out the delay, can I do somthing shorter like nop?
	}

	temp = *q->qRear;
	if(q->qRear >= &q->theQueue[100])
		q->qRear = & q->theQueue[0];
	else
		q->qRear++;

	return temp;
 }

 char isEmpty(struct cirQueue *q){
	if(q->qFront == q->qRear){
		return 0;
	}
	else{
		return 1;
	}
 }