/*
 * ShiftRegister.c
 *
 * Created: 7/5/2016 5:25:43 AM
 *  Author: Benjamin Nold
 */ 

 #include "ShiftRegister.h"
 #include "avr/builtins.h"

 //fordebug
 void testIdea(struct shiftReg *s){
 *s->port = 0x00;
 *s->port = (s->SER << 1) | (s->OE << 1);
 }

 void shiftReg_Clear(struct shiftReg *s){
	*s->port &= (unsigned)~(s->SRCLR<<1);
	//no op
	__builtin_avr_delay_cycles(1);
	s->SRCLR = 1;
 }

 void shiftReg_init(struct shiftReg *s, int *port, char SRCLK, char RCLK, char OE, char SRCLR, char SER){
	s->port = &port;
	s->SRCLK = SRCLK;
	s->RCLK = RCLK;
	s->OE = OE;
	s->SRCLR = SRCLR;
	s->SER = SER;

	shiftReg_Clear(s); //Clear the shift reg
	s->OE = 0; //set as output;
	s->RCLK = 0;
	s->SRCLK = 0;	
 }

 void shiftReg_loadData(struct shiftReg *s, unsigned char data){
	
	for(unsigned char c = 1; c < 0b10000000; c <<= 1){
		s->SRCLK = 0;
		s->SER = (data&&c) ? 1 : 0; //this would be amazing if it works!
		s->SRCLK = 1;
	}
	s->SRCLK = 0;
	s->SER = (data&&0xF0) ? 1 : 0; //this would be amazing if it works!
	s->SRCLK = 1;
	s->RCLK = 1;
	__builtin_avr_delay_cycles(1);
	s->RCLK = 0;
	s->SRCLK = 0;
 } 


