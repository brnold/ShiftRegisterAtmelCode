/*
 * GccApplication1.c
 *
 * Created: 6/11/2016 6:45:26 PM
 * Author : Benjamin Nold
 */ 

#define  F_CPU   16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "circularQueue.h"
#include "serial.h"
#include "ShiftRegister.h"





void sysexFinder(void);


struct cirQueue receivedData, transmitData;
struct shiftReg downReg, upReg;


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
	unsigned char dataA[] = {0xFF, 0x00, 0x00};
	unsigned char dataB[] = {0x00, 0xFF, 0x00};
	unsigned char dataC[] = {0x00, 0x00, 0xFF};
	unsigned char dataClear[] = {0x00, 0x00, 0x00};
	
	setupQueue(&receivedData);
	setupQueue(&transmitData);
	DDRA = 0xFF;
	DDRC = 0xFF;
	//PINA = 0xFF;
	//set up shiftregiers
	shiftReg_init(&downReg, (char* volatile) &PORTA, 2, 1, 4, 3, 0);
	
	shiftReg_init(&upReg, (char* volatile) &PORTC, 3, 0, 2, 5, 1);

	USART0_Init(BAUD_PRESCALE);
	sei();
	/* Replace with your application code */
	USART_Transmit('f'); //might not be needed anymore

	_delay_ms(500);
	_delay_ms(500);
	_delay_ms(500);
	_delay_ms(500);

	flipStops(&upReg, &downReg);
	
	//shiftReg_loadData(&testShift, 0xFF);
	//shiftReg_loadData3(&downReg, dataA);
	//_delay_ms(100);
	//shiftReg_loadData3(&downReg, dataClear);
	//_delay_ms(100);
	//shiftReg_loadData3(&downReg, dataB);
	//_delay_ms(100);
	//shiftReg_loadData3(&downReg, dataClear);
	//_delay_ms(100);
	//shiftReg_loadData3(&downReg, dataC);
	//_delay_ms(100);
	//shiftReg_loadData3(&downReg, dataClear);
	//shiftReg_loadData(&testShift, 0x00);
	//USART_Transmit('d');
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

