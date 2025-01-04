/*
 * GccApplication1.c
 *
 * Created: 6/11/2016 6:45:26 PM
 * Author : Benjamin Nold
 */ 

#define  F_CPU   16000000UL


#define _NOTEON 0x90
#define _NOTEOFF 0x80
#define _DEFAULTVELOCITY 0x69
#define _STOP_MIDI_CHANNEL 0
#define _PISTON_MIDI_CHANNEL 1


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "circularQueue.h"
#include "serial.h"
#include "ShiftRegister.h"
#include "SN74HC165N.h"
#include "SAMs_Micro_Function.h"
#include "globalDefines.h"

void sysexFinder(void);
void scanPistons(void);
void sendOutMidiStops(void);
void midiCommandOut(char channel, char cmd, char pitch);
void sendOutMidiPistons(void);

void tempSendOutQueue(void);

//for the pistonScanner
unsigned char previousPiston[] = {0, 0, 0, 0};
unsigned char currentPiston[] = {0, 0, 0, 0};
unsigned char pistonDiff;


//for the stopScanner
unsigned char previousStop[NUM_SHIFTIN_REG];
unsigned char updatedSAMs[NUM_SHIFTIN_REG];
unsigned char toClearSAMs[NUM_SHIFTIN_REG];
unsigned char toSetSAMs[NUM_SHIFTIN_REG];

unsigned char decodeSAMs(void);

struct cirQueue receivedData, transmitData;
struct shiftOutReg downReg, upReg;
struct shiftInReg stopReg;

//this is to count things

//This is the receive ISR
 ISR(USART0_RX_vect)
{
	//do something here
	enqueue(&receivedData, UDR0);
}

//This is the transmitter ISR
ISR(USART1_UDRE_vect)
{
	//do something here

	if(isEmpty(&transmitData)==1){
	//data is empty, so stop the interrupt
		TX1_INTERRUPT_OFF; //will this work?
		
	}else
		UDR1 = dequeue(&transmitData);
}


int main(void)
{	

	unsigned char dataA[] = {0xFF, 0x00, 0x00};
	unsigned char dataB[] = {0x00, 0xFF, 0x00};
	unsigned char dataC[] = {0x00, 0x00, 0xFF};
	unsigned char dataClear[] = {0x00, 0x00, 0x00};
	unsigned char newSAMS = 0;
	
	unsigned char tCount = 0, tCount2 =0;
	setupQueue(&receivedData);
	setupQueue(&transmitData);
	DDRA = 0xFF;
	DDRC = 0xFF;

	//For the KeyReader
	DDRL = 0x00; //L is input
	DDRD = 0x80; // D7 is in an output
	DDRA |= 0b10100000; //5-7 is output to control the matrix
	DDRA &= 0b10111111;
	DDRG &= 0b11111101; //PORTG is 40, is input

	
	//set up shiftregiers
	shiftReg_output_init(&downReg, (char* volatile) &PORTA, (char* volatile)&PINA, 2, 1, 4, 3, 0);
	shiftReg_output_init(&upReg, (char* volatile) &PORTC, (char* volatile)&PINC, 3, 0, 2, 5, 1);
	shiftInReg_init(&stopReg, (char* volatile)&PORTC, (char* volatile)&PINC, (char* volatile)&DDRC, 4, 7, 6);

	USART0_Init(BAUD_PRESCALE);
	sei();
	/* Replace with your application code */
	

	//_delay_ms(500);
	//_delay_ms(500);
	//_delay_ms(500);
	//_delay_ms(500);
//
	
	//testStubbornStop(&upReg, &downReg);
	//flipStops(&upReg, &downReg);
	//USART0_Transmit('b'); //test
	
	//Clear the flip tabs
	clearStops(&upReg, &downReg);
	
	//test out the ability to flip da stops correctly
// 	for (tCount2 = 0; tCount2<NUM_SHIFTIN_REG; tCount2++)
// 	{
// 		for( tCount=0; tCount<8; tCount++)
// 		{
// 			updatedSAMs[tCount2] = updatedSAMs[tCount2] | ( 1 << tCount);
// 			updateStops(&upReg, &downReg,updatedSAMs);
// 		}
// 
// 	}
	
	while (1) 
	{
	//scan the shift in registers

	sendOutMidiStops();
	scanPistons();
	sendOutMidiPistons();
	newSAMS = decodeSAMs();
	if(newSAMS == 1)
		updateStops(&upReg, &downReg,toSetSAMs, toClearSAMs);
	newSAMS=0;
	//_delay_us(5);
	

	
			
		
	}
}

void tempSendOutQueue(void){
	while(~isEmpty(&receivedData)){
		USART0_Transmit(dequeue(&receivedData));
	}	
}

void sendOutMidiStops(void)
{
	unsigned char shiftInReg_data[NUM_SHIFTIN_REG];
	unsigned char stopDiff;
	unsigned char otherCounter;
	unsigned char s19Swap, s22Swap;
	//get the new data
	shiftInReg_MultiByte(&stopReg, shiftInReg_data, NUM_SHIFTIN_REG);
	
	//USART0_Transmit(shiftInReg_data[2]);
	//I have a hook up error with stop 19 and 23, let's fix that here
	if((shiftInReg_data[2] & 0b00000100) == 0) // e.g. the 4ft flute is down 
		s19Swap = 0;
	else
		s19Swap = 1;
		
	if((shiftInReg_data[2] & 0b01000000) == 0) // e.g. the 1ft flute is down
		s22Swap = 0;
	else
		s22Swap = 1;
	
	// now that I know the states, correct the shiftInReg
	if(s19Swap == 0)
		shiftInReg_data[2] = shiftInReg_data[2] & 0b10111111;
		//shiftInReg_data[2] =0;
	else
		shiftInReg_data[2] = shiftInReg_data[2] | 0b01000000;
	
	if(s22Swap == 0)
		shiftInReg_data[2] = shiftInReg_data[2] & 0b11111011;
	else
		shiftInReg_data[2] = shiftInReg_data[2] | 0b00000100;
	//USART0_Transmit(shiftInReg_data[2]);
	
	//compare to the previous sops + send out at will.
	for(int count = 0; count < NUM_SHIFTIN_REG; count++)
	{
		stopDiff = shiftInReg_data[count] ^ previousStop[count];
		if (stopDiff != 0x00){ // something has changed, short circuit
			otherCounter = 0;
			for(unsigned char c = 1; c; c = c << 1){
				otherCounter ++;
				if(c&stopDiff?'1':'0' == 1){ // if there is a difference
					if(c&shiftInReg_data[count]?'1':'0' == 1) {//stop on
					//sendout command
					midiCommandOut(_STOP_MIDI_CHANNEL, _NOTEOFF, count*8 + otherCounter);
					}else{ //stop off
					//sendout command
					midiCommandOut(_STOP_MIDI_CHANNEL, _NOTEON, count*8 + otherCounter);
					}
				}
			}

		}	
		
	}

	//set the prevoious stops to current
	for(int count = 0; count < NUM_SHIFTIN_REG; count++)
	{
		previousStop[count] = shiftInReg_data[count];
		updatedSAMs[count] = ~shiftInReg_data[count];
	}

}

void midiCommandOut(char channel, char cmd, char pitch){
	
	char chanelCommand = (cmd & 0xF0) | (channel & 0x0F);
	
	//push channel and command
	USART0_Transmit(chanelCommand);
	//push pitch
	USART0_Transmit(pitch);
	//push velocity
	USART0_Transmit(_DEFAULTVELOCITY);
}

unsigned char decodeSAMs(void){
	
	// Let's do a state machine
	// Three messages, the "first part"
	// the pitch
	// the default velocity
	// We always know what the "first part" will be
	
	// recall that previousStop has the current stop values.
	unsigned char message, onOrOff,stopDiff,note,c,count, go, shouldReturnOne=0;
	
	for(char i=0; i<NUM_SHIFTIN_REG; i++)
	{
		toSetSAMs[i] = 0x000;
		toClearSAMs[i] = 0X00;
	}
	

	//state machine
	//first check if I have a full message
	while(queueDepth(&receivedData) >=3){

        message = dequeue(&receivedData);

        //check if it's a on or off message
        go = 0;
		// check if it's a wired message
	    if((message&0xF0)==0x80){
            go = 1;
		} else if((message&0xF0)==0x90){
            go = 1;
		}
        if(go == 0){
            // then we know we didn't get the correct message
           
        }else{
            onOrOff = (message&0x10) >> 4;
            note = dequeue(&receivedData); //get the next byte
			note = note-1;
			if(note == 0)
				count = 0;
 			else if (note == 18)
 				note == 22;
			else if (note == 22)
 				note == 18;
				
			count = note/8; // this should work, int math
            //safety
            if(count > NUM_SHIFTIN_REG){
                count = NUM_SHIFTIN_REG;
            }else if(count < 0){
                count = 0;
            }
        
            note = note % 8;
            c = 1 << note;

            if(onOrOff == 1) {
            //stop on
            //update the array
                toSetSAMs[count] = toSetSAMs[count]|c;
            }else{
            //stop off
            //update the array
			
                toClearSAMs[count] = toClearSAMs[count]|c;
            }
            
            
            message = dequeue(&receivedData); // this is the velocity, we don't care
			shouldReturnOne =1;
            //_delay_us(1);
			}
			}
		
	
return shouldReturnOne;
}








 /************************************************************************/
 /* Scan the pistons and put the results in the outQueue                 */
 /************************************************************************/
 void scanPistons(void){
	unsigned char count = 0;
	//turn on A7
	PORTD |= 0b10000000; //finding something
	_delay_us(1);
	currentPiston[0] = PINL;
	PORTD &= 0b01111111;

	
	PORTA = PORTA | 0b00100000;
	_delay_us(1);
	currentPiston[1] = PINL;

	//Store the other 2 buttons
	if((PING&0x02) == 2) //one button for sure
	currentPiston[3] = currentPiston[3] |= 0x01;
	else
	currentPiston[3] = currentPiston[3] &= 0xFE;

	if( (PINA&0b01000000)== 0x40)
	currentPiston[3] = currentPiston[3] |= 0x02;
	else
	currentPiston[3] = currentPiston[3] &= 0b11111101;

	PORTA = PORTA & 0b11011111;

	PORTA |= 0b10000000;
	_delay_us(1);
	currentPiston[2] = PINL;
	PORTA &= 0b01111111;

	

	


 }

 void sendOutMidiPistons(void){
	unsigned char otherCounter;
	//compare to the previous pistons + send out at will.
	for(int count = 0; count < 4; count++)
	{
		pistonDiff = currentPiston[count] ^ previousPiston[count];
		if (pistonDiff != 0x00){ // something has changed, short circuit
			otherCounter = 0;
			for(unsigned char c = 1; c; c = c << 1){
				otherCounter ++;
				if(c&pistonDiff?'1':'0' == 1){ // if there is a difference
					if(c&currentPiston[count]?'1':'0' == 1) {//stop on
					//sendout command
					midiCommandOut(_PISTON_MIDI_CHANNEL, _NOTEON, count*8 + otherCounter);
					}else{ //stop off
					//sendout command
					midiCommandOut(_PISTON_MIDI_CHANNEL, _NOTEOFF, count*8 + otherCounter);
					}
				}
			}

		}
	}

	//set the prevoious stops to current
	for(int count = 0; count < 4; count++)
	{
		previousPiston[count] = currentPiston[count];
	}

 }


//This is for another project that I got side tracked on. Ignore 
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
			TX0_INTERRUPT_ON;
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
		TX0_INTERRUPT_ON;
		}

	}else//else transmit the data away, bye!
	{
	//put first on the stack
	enqueue(&transmitData, first);
	TX0_INTERRUPT_ON;
	}
}