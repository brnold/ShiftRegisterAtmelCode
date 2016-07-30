/*
 * SN74HC165N.h
 *
 * Created: 7/30/2016 10:06:35 AM
 *  Author: Benjamin Nold
 */ 


#ifndef SN74HC165N_H_
#define SN74HC165N_H_

struct shiftInReg {
	unsigned char CLK, Qh, LOAD;
	char volatile *port;
	char volatile *pin;
};


void shiftInReg_init(struct shiftInReg *s, char volatile *port, char volatile *pin, char volatile *ddr, char CLK, char Qh, char LOAD);
unsigned char shiftInReg_SingleByte(struct shiftInReg *s);
void shiftInReg_MultiByte(struct shiftInReg *s, unsigned char data[], char NumRegisters);



#endif /* SN74HC165N_H_ */