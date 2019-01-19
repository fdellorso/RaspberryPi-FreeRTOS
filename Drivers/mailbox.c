//mailbox.c
//authored by Jared Hull
//
//These functions are used for communications between the CPU and GPU

#include "mailbox.h"

//direct memory get and set
// extern void PUT32(int dest, int src);
// extern int GET32(int src);

typedef struct {
	unsigned long	READ;
	unsigned long	Reserved_1;
	unsigned long	Reserved_2;
	unsigned long	Reserved_3;
	unsigned long	PEAK;
	unsigned long	SENDER;
	unsigned long	STATUS;
	unsigned long	CONFIG;
	unsigned long	WRITE;
} BCM2835_MAIL0_REGS;

static volatile BCM2835_MAIL0_REGS * const pRegs = (BCM2835_MAIL0_REGS *) (BCM2835_MAIL0_BASE);

//Docuentation on the mailbox functions
//mailbuffer should probably be 16 byte aligned (for gpu at least):
//unsigned int mailbuffer[22] __attribute__((aligned (16)));
//https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface
void mailboxWrite(int data_addr, int channel){
	while(1){
		if((pRegs->STATUS & MAIL_FULL) == 0) break;
	}

	pRegs->WRITE = data_addr + channel;
	return;
}

int mailboxRead(int channel){
	int mail;

	while(1){
		while(1){
			if((pRegs->STATUS & MAIL_EMPTY) == 0) break;
		}
		mail = pRegs->READ;
		if((mail & 0xF) == channel) break;
	}
	
	return(mail);
}