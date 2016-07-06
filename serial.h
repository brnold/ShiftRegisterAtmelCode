/*
 * IncFile1.h
 *
 * Created: 7/6/2016 11:29:11 AM
 *  Author: Benjamin Nold
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((16000000/(USART_BAUDRATE*16UL)))-1)


#define TX_INTERRUPT_ON (UCSR0B |=(1<<UDRIE0))
#define TX_INTERRUPT_OFF (UCSR0B = UCSR0B & ~(1<<UDRIE0))

void USART0_Init(unsigned int baud);
void USART_Transmit( unsigned char data);
unsigned char USART0_Receive(void);

#endif /* INCFILE1_H_ */