/*
 * ShiftRegister.h
 *
 * Created: 7/6/2016 12:48:39 PM
 *  Author: Benjamin Nold
 */ 


#ifndef SHIFTREGISTER_H_
#define SHIFTREGISTER_H_

//#define SET_BIT(PORT_NR,BIT) (PORT_NR |= (1<<BIT))
//#define CLEAR_BIT(PORT_NR,BIT) (PORT_NR &= ~(1<<BIT))

struct shiftReg {
	unsigned char SRCLK, RCLK, SRCLR, OE, SER;
	char volatile *port;
};



void shiftReg_Clear(struct shiftReg *s);
void shiftReg_init(struct shiftReg *s, char volatile *port, char  SRCLK, char RCLK, char OE, char SRCLR, char SER);
void shiftReg_loadData(struct shiftReg *s, unsigned char data);
void testIdea(struct shiftReg *s);



#endif /* SHIFTREGISTER_H_ */