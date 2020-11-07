/*
 * IncFile1.h
 *
 * Created: 7/6/2016 11:29:11 AM
 *  Author: Benjamin Nold
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_

#define USART_BAUDRATE 57600
#define BAUD_PRESCALE (((16000000/(USART_BAUDRATE*16UL)))-1)


#define TX0_INTERRUPT_ON (UCSR0B |=(1<<UDRIE0))
#define TX0_INTERRUPT_OFF (UCSR0B = UCSR0B & ~(1<<UDRIE0))

#define TX1_INTERRUPT_ON (UCSR1B |=(1<<UDRIE1))
#define TX1_INTERRUPT_OFF (UCSR1B = UCSR1B & ~(1<<UDRIE1))

void USART0_Init(unsigned int baud);
void USART0_Transmit( unsigned char data);
unsigned char USART0_Receive(void);

void USART1_Init(unsigned int baud);
void USART1_Transmit( unsigned char data);
unsigned char USART1_Receive(void);

#endif /* INCFILE1_H_ */