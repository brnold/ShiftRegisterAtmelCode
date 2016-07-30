/*
 * ShiftRegister.h
 *
 * Created: 7/6/2016 12:48:39 PM
 *  Author: Benjamin Nold
 *
 * Use Case
 * //Make a shiftReg Struct
 * struct shiftReg example;

 * //Initialize the struct
 * setupQueue(&example);

 * //Initialize the shift register, The last 5 char's are the pin numbers 
 * //that you have the pins of the shift register connected to.
 * shiftReg_init(&example, (char* volatile) &PORTA, 2, 1, 4, 3, 0);
 * 
 */ 


#ifndef SHIFTREGISTER_H_
#define SHIFTREGISTER_H_

//#define SET_BIT(PORT_NR,BIT) (PORT_NR |= (1<<BIT))
//#define CLEAR_BIT(PORT_NR,BIT) (PORT_NR &= ~(1<<BIT))

struct shiftOutReg {
	unsigned char SRCLK, RCLK, SRCLR, OE, SER;
	char volatile *port;
};

#define SAM_WAIT_TIME 75



void shiftReg_Clear_Registers(struct shiftOutReg *s);
void shiftReg_Clear_Output(struct shiftOutReg *s);
void shiftReg_output_init(struct shiftOutReg *s, char volatile *port, char volatile *ddr, char  SRCLK, char RCLK, char OE, char SRCLR, char SER);
void shiftReg_input_init(struct shiftOutReg *s, char volatile *port, char volatile *ddr, char  SRCLK, char RCLK, char OE, char SRCLR, char SER);
void shiftReg_loadData(struct shiftOutReg *s, unsigned char data);
unsigned char shiftReg_readData(struct shiftOutReg *s);
void testIdea(struct shiftOutReg *s);
void shiftReg_loadData3(struct shiftOutReg *s, unsigned char data[]);
void shiftReg_loadStops(struct shiftOutReg *s, unsigned char data[]);
void flipStops(struct shiftOutReg *up, struct shiftOutReg *down);



#endif /* SHIFTREGISTER_H_ */