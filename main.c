/*
 * GccApplication1.c
 *
 * Created: 6/11/2016 6:45:26 PM
 * Author : Benjamin Nold
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "circularQueue.h"
#include "serial.h"
#include "ShiftRegister.h"

#define  F_CPU   16000000UL



void sysexFinder(void);


struct cirQueue receivedData, transmitData;
struct shiftReg testShift;


//This is the receive ISR
 ISR(USART0_RX_vect)
{
	//do something here
	//USART_Transmit('i'); //for debug
	enqueue(&receivedData, UDR0);

}

//This is the transmitter ISR
ISR(USART0_UDRE_vect)
{
	//do something herec
	//UDR0='Y';
	if(isEmpty(&transmitData)==0){
	//data is empty, so stop the interrupt
		TX_INTERRUPT_OFF; //will this work?
		
	}else
		UDR0 = dequeue(&transmitData);


}




int main(void)
{	
	unsigned char c;
	
	setupQueue(&receivedData);
	setupQueue(&transmitData);
	
	//set up shiftregiers
	shiftReg_init(&testShift,0, 1, 2, 3, 5);

	USART0_Init(BAUD_PRESCALE);
	sei();
	/* Replace with your application code */
	USART_Transmit('f'); //might not be needed anymore
	testIdea(&testShift);
	while (1) 
	{
		
	
		
		//if(isEmpty(&receivedData)==1){
			//enqueue(&transmitData, dequeue(&receivedData));
			//TX_INTERRUPT_ON;
			////USART_Transmit('x');
			////I think it's a transmission issue
		//}else{
			////USART_Transmit('e');
		//}
		
		//sysexFinder();


		
	}
}

void sysexFinder(void){

	const unsigned char bitVal[] = {2, 3, 0, 1, 5, 6, 1, 3,
									 6, 2, 1, 4, 0, 1, 0, 3, 4, 5, 0, 1, 4,
										6, 2, 5, 3, 4, 2, 4, 6, 0, 3, 5, 6, 6, 
											4, 3};
	const unsigned char offsetVal[] = {0,0,1,1,1,1,2,2,2,3,4,8,8,8,
										     9,9,9,9,10,10,10,11,11,11,21,21,
										          22,22,22,22,24,25,25,26,1,4,31};
	#define LENGTH_STOPS_LIST 35
	#define SYSEX_LIST_OFFSET 5
	#define LENGTH_CONDENSED_STOPS 4
	unsigned char first, second, temp;
	unsigned char arrayCounter, condencedArrayCounter, posCount;
	unsigned char sysexMessage[45]; //only needs to be 43, but who cares?
	unsigned char currentStops[LENGTH_CONDENSED_STOPS], previousStops[LENGTH_CONDENSED_STOPS];
	
	//USART_Transmit('1');
	
	first = dequeue(&receivedData);

	if(first == 0xF0)
	{
		second = dequeue(&receivedData);

		if(second==0x41) //this is most "likely" my data
		{
			//enqueue(&transmitData, 'a');
			//enqueue(&transmitData, '\n');
			//TX_INTERRUPT_ON;
			temp = dequeue(&receivedData);
			arrayCounter = 0;	

			while(temp != 0xF7){
			//fill my array
			sysexMessage[arrayCounter] = temp;
			temp = dequeue(&receivedData); //what to do with the last byte? I don't think it matters, it's garbage
			arrayCounter++;
			}

			////debug, see what I got in my array
			//for(unsigned char c =0; c<arrayCounter; c++)
			//{
				//enqueue(&transmitData, sysexMessage[c]);
			//}
			//TX_INTERRUPT_ON;

			condencedArrayCounter = 0;
			//got my data, now parse it out!
			for(unsigned char c = 0; c<(LENGTH_STOPS_LIST-SYSEX_LIST_OFFSET); c++)
			{
				temp = sysexMessage[offsetVal[c]+ SYSEX_LIST_OFFSET] & (1<<bitVal[c]);
				//now I have the bit I need, save it to a condensed array
				//enqueue(&transmitData, temp);
				temp = (temp>>bitVal[c]); //that that bit shift it back right
				currentStops[c/8] |= (temp<<(condencedArrayCounter%8)); //shift it back to sit in the correct stop
				condencedArrayCounter++;
				
			}
			//temp = sysexMessage[offsetVal[23]+ SYSEX_LIST_OFFSET] & (1<<bitVal[23]);
			//enqueue(&transmitData, temp);
			//temp = sysexMessage[offsetVal[25]+ SYSEX_LIST_OFFSET] & (1<<bitVal[25]);
			//enqueue(&transmitData, temp);
			//TX_INTERRUPT_ON;

			enqueue(&transmitData, 0xFF);
			enqueue(&transmitData, currentStops[0]);
			enqueue(&transmitData, currentStops[1]);
			enqueue(&transmitData, currentStops[2]);
			enqueue(&transmitData, currentStops[3]);
			enqueue(&transmitData, currentStops[4]);
			TX_INTERRUPT_ON;
			//Now in theory, I have a condensed Array.
			posCount = 0;
			for(unsigned char c = 0; c<LENGTH_CONDENSED_STOPS; c++)
			{
				temp = previousStops[c]^currentStops[c];
				for(unsigned char cOther = 1; cOther; cOther <<= 1){
			
					if((cOther&temp) == cOther)
					{
					//put it on the stack, the value will be posCount+(c*8);
					//enqueue(&transmitData, posCount);
					//TX_INTERRUPT_ON;
					}
					posCount++;
				}
			}
		
		}else
		{
		//put first AND second on the stack
		enqueue(&transmitData, first);
		enqueue(&transmitData, second);
		TX_INTERRUPT_ON;
		}

	}else//else transmit the data away, bye!
	{
	//put first on the stack
	enqueue(&transmitData, first);
	TX_INTERRUPT_ON;
	}
}