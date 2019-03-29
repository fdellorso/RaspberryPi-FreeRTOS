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

int GetClockRate(unsigned nClockId)
{
	unsigned int mailbuffer[8] __attribute__((aligned (16)));

	//set power state
	mailbuffer[0] = 8 * 4;		//mailbuffer size
	mailbuffer[1] = 0;			//response code
	mailbuffer[2] = 0x00030002;	//set power state
	mailbuffer[3] = 8;			//value buffer size
	mailbuffer[4] = 8;			//Req. + value length (bytes)
	mailbuffer[5] = nClockId;	//device id 3
	mailbuffer[6] = 0;			//state
	mailbuffer[7] = 0;			//terminate buffer

	//spam mail until the response code is ok
	while(mailbuffer[1] != 0x80000000){
		mailboxWrite((int)mailbuffer, BCM2835_MAIL0_ARM_VC);
		mailboxRead(BCM2835_MAIL0_ARM_VC);
	}

	if(mailbuffer[5] != nClockId) return 0;

	return mailbuffer[6];
}
