/*
 * SN74HC165N.c
 *
 * Created: 7/30/2016 10:06:13 AM
 *  Author: Benjamin Nold
 */ 
  #define  F_CPU   16000000UL
  #include "avr/builtins.h"
  #include "SN74HC165N.h"
  #include <avr/io.h>
  #include <util/delay.h>

 void shiftInReg_init(struct shiftInReg *s, char volatile *port, char volatile *pin, char volatile *ddr, 
															char CLK, char Qh, char LOAD) 
 {
	 //Setup DDR
	 *ddr |= (1 << CLK) | (1<<LOAD); 
	 *ddr &= ~(1<<Qh);

	 //setup Struct
	 s->port = port;	
	 s->CLK = CLK;
	 s->LOAD = LOAD;
	 s->Qh = Qh;
	 s->pin = pin;
 
	*s->port |= (1<<LOAD);// load Hi, don't store data
	*s->port |= (1<<CLK);
 }

 unsigned char shiftInReg_SingleByte(struct shiftInReg *s)
 {
	unsigned char temp, data=0;
	char count = 0;

	//Load data into shift register
	*s->port &= ~(1<<s->LOAD);
	__builtin_avr_delay_cycles(2);
	*s->port |= (1<<s->LOAD);
	  

	for(unsigned char c = 1; c; c = c << 1){
		
		*s->port &= ~(1 << s->CLK);
		__builtin_avr_delay_cycles(1);
		
		//read the port
		temp = *s->pin;
		//get only the data I want
		temp = temp & (1<<s->Qh);
		//shift the data to the right
		temp = temp >> s->Qh;
		//put it in the right place in the array
		data |= temp << count;
	

		*s->port |= (1 << s->CLK);
		__builtin_avr_delay_cycles(1);	
		count++;	
	}
	return data;
 }
 void shiftInReg_MultiByte(struct shiftInReg *s, unsigned char data[], char NumRegisters)
 {
	unsigned char temp;
	char count = 0;

	//Load data into shift register
	*s->port &= ~(1<<s->LOAD);
	__builtin_avr_delay_cycles(2);
	*s->port |= (1<<s->LOAD);
	
	for(unsigned char regCount = 0; regCount < NumRegisters; regCount++)
	{
		data[regCount] = 0;
		count = 0;
		for(unsigned char c = 1; c; c = c << 1){
		
			*s->port &= ~(1 << s->CLK);
			__builtin_avr_delay_cycles(1);
		
			//read the port
			temp = *s->pin;
			//get only the data I want
			temp = temp & (1<<s->Qh);
			//shift the data to the right
			temp = temp >> s->Qh;
			//put it in the right place in the array
			data[regCount] |= temp << count;
		

			*s->port |= (1 << s->CLK);
			__builtin_avr_delay_cycles(1);
			count++;
		}
	}

 }