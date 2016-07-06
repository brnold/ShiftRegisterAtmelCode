/*
 * ShiftRegister.h
 *
 * Created: 7/5/2016 5:25:52 AM
 *  Author: Benjamin Nold
 * ASSUMES THAT ALL PINS ARE ON THE SAME PORT!
 */ 


#ifndef SHIFTREGISTER_H_
#define SHIFTREGISTER_H_

struct shiftReg {
	unsigned char SRCLK, RCLK, SRCLR, OE, SER;
	unsigned int *port;
};



void shiftReg_Clear(struct shiftReg *s);
void shiftReg_init(struct shiftReg *s, int *port, int SRCLK, int RCLK, int OE, int SRCLR, int SER);
void shiftReg_loadData(struct shiftReg *s, unsigned char data);
void testIdea(struct shiftReg *s);

#endif /* INCFILE1_H_ */