/*
 * ShiftRegister.c
 *
 * Created: 7/5/2016 5:25:43 AM
 *  Author: Benjamin Nold
 */ 

 #include "ShiftRegister.h"
 #include "avr/builtins.h"
 #include <avr/io.h>

 //fordebug
 void testIdea(struct shiftReg *s){
 *s->port = 0x00;
 //*s->port |= (1 << s->SRCLK) | (1 << s->RCLK);
 //*s->port &= ~(1 << s->RCLK);
 //*s->port = 0x0F;
 *s->port = 0xFF;
 *s->port &= ~(1 << s->SRCLK);
 *s->port &= ~(1 << s->RCLK);

 }

 void shiftReg_Clear(struct shiftReg *s){
	*s->port &= ~(1 << s->SRCLR);
	//no op
	__builtin_avr_delay_cycles(2);
	*s->port |= (1 << s->SRCLR);
 }

 void shiftReg_init(struct shiftReg *s, char volatile *port, char SRCLK, char RCLK, char OE, char SRCLR, char SER){
    s->port = port;
	s->SRCLK = SRCLK;
	s->RCLK = RCLK;
	s->OE = OE;
	s->SRCLR = SRCLR;
	s->SER = SER;

	shiftReg_Clear(s); //Clear the shift reg
	*s->port &= ~((1 << s->OE) | (1 << s->RCLK) | (1 << s->SRCLK));
	//s->OE = 0; //set as output;
	//s->RCLK = 0;
	//s->SRCLK = 0;	
 }

 void shiftReg_loadData(struct shiftReg *s, unsigned char data){
	char temp;
	for(unsigned char c = 1; c < 0b10000000; c <<= 1){
		*s->port &= ~(1 << s->SRCLK);
		__builtin_avr_delay_cycles(1);
		temp = (data&c) ? 1 : 0; //this would be amazing if it works!
		if(temp == 1){
			*s->port |= (1 << s->SER);
		}else{
			*s->port &= ~(1 << s->SER);
		}
		*s->port |= (1 << s->SRCLK);
		__builtin_avr_delay_cycles(1);
	}
	*s->port &= ~(1 << s->SRCLK);
	__builtin_avr_delay_cycles(1);
	temp = (data&0x80) ? 1 : 0; //this would be amazing if it works!
	if(temp == 1){
		*s->port |= (1 << s->SER);
		}else{
		*s->port &= ~(1 << s->SER);
	}
	*s->port |= (1 << s->SRCLK);
	*s->port |= (1 << s->RCLK);
	//s->RCLK = 1;
	__builtin_avr_delay_cycles(1);
	*s->port &= ~((1 << s->SRCLK) | (1 << s->RCLK));
	//s->RCLK = 0;
	//s->SRCLK = 0;
 } 


