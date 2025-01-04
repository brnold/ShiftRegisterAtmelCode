/*
 * ShiftRegister.c
 *
 * Created: 7/5/2016 5:25:43 AM
 *  Author: Benjamin Nold
 */ 
 
 #define  F_CPU   16000000UL
 #include "ShiftRegister.h"
 #include "avr/builtins.h"
 #include <avr/io.h>
 #include <util/delay.h>
 #include "globalDefines.h"


 //fordebug, decrypt
 void testIdea(struct shiftOutReg *s){
 *s->port = 0x00;
 //*s->port |= (1 << s->SRCLK) | (1 << s->RCLK);
 //*s->port &= ~(1 << s->RCLK);
 //*s->port = 0x0F;
 *s->port = 0xFF;
 *s->port &= ~(1 << s->SRCLK);
 *s->port &= ~(1 << s->RCLK);

 }

 /************************************************************************/
 /* Clears the Shift Registers, but doesn't clear the output!            */
 /************************************************************************/
 void shiftReg_Clear_Registers(struct shiftOutReg *s){
	*s->port &= ~(1 << s->SRCLR);
	//no op
	__builtin_avr_delay_cycles(1);
	*s->port |= (1 << s->SRCLR);
 }

 /************************************************************************/
 /* Clears the Shift Registers and sets the outputs to 0			     */
 /************************************************************************/
 void shiftReg_Clear_Output(struct shiftOutReg *s)
 {
	*s->port &= ~(1 << s->SRCLR);
	//no op
	__builtin_avr_delay_cycles(1);
	*s->port |= (1 << s->SRCLR);
	*s->port |= (1 << s->RCLK);
	//s->RCLK = 1;
	__builtin_avr_delay_cycles(1);
	*s->port &= ~(1 << s->RCLK);
 }

 /************************************************************************/
 /* Sets up the DDR registers and shftReg Struct for an output Shift Reg.*/
 /* Takes in the address of the shift Reg Struct, port where the pins are*/ 
 /* located, then the pin numbers for the shift register pins            */
 /************************************************************************/
 void shiftReg_output_init(struct shiftOutReg *s, char volatile *port, char volatile *ddr, 
									char  SRCLK, char RCLK, char OE, char SRCLR, char SER)
{
    //Set up the DDR
	*ddr |= (1 << SRCLK);
	*ddr |= (1 << RCLK);
	*ddr |= (1 << OE);
	*ddr |= (1 << SRCLR);
	*ddr |= (1 << SER);

	//Setup struct
	s->port = port;
	s->SRCLK = SRCLK;
	s->RCLK = RCLK;
	s->OE = OE;
	s->SRCLR = SRCLR;
	s->SER = SER;

	shiftReg_Clear_Registers(s); //Clear the shift reg
	*s->port &= ~((1 << s->RCLK) | (1 << s->SRCLK));
	*s->port != (1<< OE); //set ouputenable high
 }

 /************************************************************************/
 /* Sets up the DDR registers and shftReg Struct for an input Shift Reg. */
 /* Takes in the address of the shift Reg Struct, port where the pins are*/
 /* located, then the pin numbers for the shift register pins            */                                                       
 /************************************************************************/

 void shiftReg_input_init(struct shiftOutReg *s, char volatile *port, char volatile *ddr,
									 char  SRCLK, char RCLK, char OE, char SRCLR, char SER)
{
	//Set up the DDR
	*ddr |= (1 << SRCLK);
	*ddr |= (1 << RCLK);
	*ddr |= (1 << OE);
	*ddr |= (1 << SRCLR);
	*ddr &= ~(1 << SER);

	//setup struct
	s->port = port;
	s->SRCLK = SRCLK;
	s->RCLK = RCLK;
	s->OE = OE;
	s->SRCLR = SRCLR;
	s->SER = SER;

	shiftReg_Clear_Registers(s); //Clear the shift reg
	*s->port &= ~((1 << s->OE) | (1 << s->RCLK) | (1 << s->SRCLK));
}


 /************************************************************************/
 /* Loads 8 bits into 1 shift register                          */
 /************************************************************************/
 void shiftReg_loadData(struct shiftOutReg *s, unsigned char data){
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

 /************************************************************************/
 /* Loads in 3*8 bits into 3 daisy chaned shift registers                */
 /************************************************************************/

 void shiftReg_loadData3(struct shiftOutReg *s, unsigned char data[]){
 	char temp;
 	//so fully load in the first 2 bytes
	for(unsigned char byteCount = 0; byteCount<2; byteCount++){
		for(unsigned char c = 1; c; c <<= 1){
	 		*s->port &= ~(1 << s->SRCLK);
	 		__builtin_avr_delay_cycles(1);
	 		temp = (data[byteCount]&c) ? 1 : 0; //this would be amazing if it works!
	 		if(temp == 1){
		 		*s->port |= (1 << s->SER);
		 		}else{
		 		*s->port &= ~(1 << s->SER);
	 		}
	 		*s->port |= (1 << s->SRCLK);
	 		__builtin_avr_delay_cycles(1);
 		}
	}
	
	for(unsigned char c = 1; c < 0b10000000; c <<= 1){
	 	*s->port &= ~(1 << s->SRCLK);
	 	__builtin_avr_delay_cycles(1);
	 	temp = (data[2]&c) ? 1 : 0; //this would be amazing if it works!
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
 	temp = (data[2]&0x80) ? 1 : 0; //this would be amazing if it works!
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

  /************************************************************************/
  /* Loads in 6*8 bits into 6 daisy chaned shift registers                */
  /************************************************************************/

 void shiftReg_loadShiftRegs(struct shiftOutReg *s, unsigned char data[]){
  	char temp;
  	//so fully load in the first 5 bytes
  	for(unsigned char byteCount = 5; byteCount>0; byteCount--){
	  	for(unsigned char c = 1; c; c <<= 1){
		  	*s->port &= ~(1 << s->SRCLK);
		  	__builtin_avr_delay_cycles(1);
		  	temp = (data[byteCount]&c) ? 1 : 0; //this would be amazing if it works!
		  	if(temp == 1){
			  	*s->port |= (1 << s->SER);
			  	}else{
			  	*s->port &= ~(1 << s->SER);
		  	}
		  	*s->port |= (1 << s->SRCLK);
		  	__builtin_avr_delay_cycles(1);
	  	}
  	}
  	
  	for(unsigned char c = 1; c < 0b10000000; c <<= 1){
	  	*s->port &= ~(1 << s->SRCLK);
	  	__builtin_avr_delay_cycles(1);
	  	temp = (data[0]&c) ? 1 : 0; //this would be amazing if it works!
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
  	temp = (data[0]&0x80) ? 1 : 0; //this would be amazing if it works! It does!
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

//deprecated on 10/31/2020
 void shiftReg_loadStops(struct shiftOutReg *s, unsigned char data[]){
	shiftReg_loadShiftRegs(s, data);
	_delay_ms(SAM_WAIT_TIME);
	shiftReg_Clear_Output(s);
 }
 /************************************
 - this should clear the stops
 ************************************/
 void clearStops(struct shiftOutReg *up, struct shiftOutReg *down){
	unsigned char numRegisters=6;
	unsigned char flipArray[numRegisters];
	unsigned char flipArrayCleared[numRegisters];
	//first clear out the array
	for(unsigned char c = 0; c<=numRegisters; c++)
	{
		flipArray[c] = 0x00;
		flipArrayCleared[c] = 0x00;
	}

	for(unsigned char regCount = 0; regCount<=numRegisters; regCount++){
		for(unsigned char c = 1; c; c <<= 1){
			flipArray[regCount] = c;
			shiftReg_loadStops(up, flipArray);
			//_delay_ms(SAM_WAIT_TIME);
		}
		flipArray[regCount] = 0x00;
	}

	shiftReg_loadStops(up, flipArrayCleared);

 }


 /************************************************************************/
 /* This updates the SAMs based on what's on, and what want's to be on    */
 /************************************************************************/
void updateStops(struct shiftOutReg *up, struct shiftOutReg *down, unsigned char toClearSAMs[], unsigned char toSetSAMs[]){
	unsigned char numRegisters=6;
	//unsigned char zeros[6] ={0,0,0,0,0,0,0};
	//unsigned char toClear[NUM_SHIFTIN_REG],toSet[NUM_SHIFTIN_REG];
	
	shiftReg_loadShiftRegs(down, toClearSAMs);
	shiftReg_loadShiftRegs(up, toSetSAMs);
	_delay_ms(SAM_WAIT_TIME);

	shiftReg_Clear_Output(up);
	shiftReg_Clear_Output(down);


	
}

 /************************************************************************/
 /* For Testing out my PCB.                                              */
 /************************************************************************/
 void flipStops(struct shiftOutReg *up, struct shiftOutReg *down){
	unsigned char numRegisters=6;
	unsigned char flipArray[numRegisters];
	unsigned char flipArrayCleared[numRegisters];
	//first clear out the array
	for(unsigned char c = 0; c<=numRegisters; c++)
	{
		flipArray[c] = 0x00;
		flipArrayCleared[c] = 0x00;
	}

	////Test that one stubborn stop


	for(unsigned char regCount = 0; regCount<numRegisters; regCount++){
		for(unsigned char c = 1; c; c <<= 1){
			flipArray[regCount] = c;
			shiftReg_loadStops(down, flipArray);
			_delay_ms(SAM_WAIT_TIME);
		}
		flipArray[regCount] = 0x00;
	}

	shiftReg_loadStops(down, flipArrayCleared);
	//shiftReg_Clear(down);

	for(unsigned char c = 0; c<numRegisters; c++)
	{
		flipArray[c] = 0x00;
		flipArrayCleared[c] = 0x00;
	}

	for(unsigned char regCount = 0; regCount<numRegisters; regCount++){
		for(unsigned char c = 1; c; c <<= 1){
			flipArray[regCount] = c;
			shiftReg_loadStops(up, flipArray);
			_delay_ms(SAM_WAIT_TIME);
		}
		flipArray[regCount] = 0x00;
	}

	shiftReg_loadStops(up, flipArrayCleared);

	
 }

 void testStubbornStop(struct shiftOutReg *up, struct shiftOutReg *down){
 	unsigned char numRegisters=6;
 	unsigned char flipArray[numRegisters];
 	unsigned char flipArrayCleared[numRegisters];

	for(unsigned char c = 0; c<=numRegisters; c++)
	{
		flipArray[c] = 0x00;
		flipArrayCleared[c] = 0x00;
	}
	
	shiftReg_Clear_Registers(up);
	shiftReg_Clear_Registers(down);
 	flipArray[5] = 0xFF;
 	shiftReg_loadStops(down, flipArray);
 	_delay_ms(SAM_WAIT_TIME);
 	shiftReg_Clear_Output(down);
 	
 	_delay_ms(500);
 	_delay_ms(500);
	_delay_ms(500);
	_delay_ms(500);

 	shiftReg_loadStops(up, flipArray);
 	_delay_ms(SAM_WAIT_TIME);
 	shiftReg_Clear_Output(up);

 	
 	shiftReg_Clear_Registers(up);
	shiftReg_Clear_Registers(down);


 }

 