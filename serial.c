/*
 * serial.c
 *
 * Created: 7/6/2016 11:29:03 AM
 *  Author: Benjamin Nold
 */ 

 #include <avr/io.h>

 void USART0_Init(unsigned int baud)
 {
	 //baud rate
	 UBRR0H = (unsigned char)(baud>>8);
	 UBRR0L = (unsigned char)baud;
	 //enable receiver and tx, emable rx interrupt and Data Register Empty interrupt
	 UCSR0B = (1 <<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<UDRIE0);
	 //set frame format: 8data, 2stop bit
	 //UCSR0C = (1 << USBS0)|(3<<UCSZ00);
	 UCSR0C = (3<<UCSZ00); //one stop bit
 }

 //transmitter
 void USART0_Transmit( unsigned char data)
 {
	 //wait for empty transmit buffer
	 while ( !( UCSR0A & (1<<UDRE0)));
	 //put the data in the buffer
	 UDR0 = data;

 }

 unsigned char USART0_Receive(void)
 {

	 while(!(UCSR0A & (1<<RXC0)));
	 
	 return UDR0;
 }

 void USART1_Init(unsigned int baud)
 {
	 //baud rate
	 UBRR1H = (unsigned char)(baud>>8);
	 UBRR1L = (unsigned char)baud;
	 //enable receiver and tx, emable rx interrupt and Data Register Empty interrupt
	 UCSR1B = (1 <<RXEN1)|(1<<TXEN1)|(1<<RXCIE1)|(1<<UDRIE1);
	 //set frame format: 8data, 2stop bit
	 //UCSR0C = (1 << USBS0)|(3<<UCSZ00);
	 UCSR1C = (3<<UCSZ01); //one stop bit
 }

 //transmitter
 void USART1_Transmit( unsigned char data)
 {
	 //wait for empty transmit buffer
	 while ( !( UCSR1A & (1<<UDRE1)));
	 //put the data in the buffer
	 UDR1 = data;

 }

 unsigned char USART1_Receive(void)
 {

	 while(!(UCSR1A & (1<<RXC1)));
	 
	 return UDR1;
 }