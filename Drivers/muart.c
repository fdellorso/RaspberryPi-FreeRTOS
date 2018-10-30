/**
 *	Quick and very Dirty Mini UART API.
 *
 **/

#ifndef PRVLIB
	#include <string.h>
#else
	#include "prvlib/string.h"
#endif

#include "aux.h"
#include "muart.h"
#include "gpio.h"

typedef struct {
	unsigned long	AUXIRQ;	  
	unsigned long	AUXENB;
} BCM2835_AUX_REGS;

static volatile BCM2835_AUX_REGS * const pAux = (BCM2835_AUX_REGS *) (BCM2835_AUX_BASE);

typedef struct {
	unsigned long	AUX_MU_IO_REG;	  
	unsigned long	AUX_MU_IER_REG;
	unsigned long	AUX_MU_IIR_REG;
	unsigned long	AUX_MU_LCR_REG;
	unsigned long	AUX_MU_MCR_REG;
	unsigned long	AUX_MU_LSR_REG;
	unsigned long	AUX_MU_MSR_REG;
	unsigned long	AUX_MU_SCRATCH;
	unsigned long	AUX_MU_CNTL_REG;
	unsigned long	AUX_MU_STAT_REG;
	unsigned long	AUX_MU_BAUD_REG;
} BCM2835_MUART_REGS;

static volatile BCM2835_MUART_REGS * const pRegs = (BCM2835_MUART_REGS *) (BCM2835_MUART_BASE);

extern char loaded;

void muart_init() {
	pAux->AUXENB			= Mini_UART_ENB;	// Enable Mini UART

	pRegs->AUX_MU_IER_REG 	= 0;				// Disable Interrupt
	pRegs->AUX_MU_CNTL_REG	= 0;				// Disable Comunicatio
	pRegs->AUX_MU_LCR_REG	= 3;				// Data Size = 8-bit (wrong in documentation)
	pRegs->AUX_MU_MCR_REG	= 0;				// Write 0 to "Modem Signals"
	pRegs->AUX_MU_IER_REG	= 0;
	// FIFO Enable & CLear
	pRegs->AUX_MU_IIR_REG	= FIFO_ENABLES | 
							  FIFO_CLEAR_RECEIVE | 
							  FIFO_CLEAR_TRANSMIT;
	pRegs->AUX_MU_BAUD_REG	= 270;				// ((250,000,000/115200)/8)-1 = 270

	SetGpioFunction(14,GPIO_FUNC_ALT_5);
	SetGpioFunction(15,GPIO_FUNC_ALT_5);

	PudGpio(14,PULL_DISABLE);
	PudGpio(15,PULL_DISABLE);

	pRegs->AUX_MU_CNTL_REG	= 3;				// Enable Comunication

	loaded = 1;
}

void muart_putc(unsigned char c) {
	while(1) if(pRegs->AUX_MU_LSR_REG & FIFO_TRANSMITTER_EMPTY) break;
	pRegs->AUX_MU_IO_REG = c;
}

void muart_puts(const char *str) {
	while(*str) muart_putc(*str++);

}

void muart_println(const char *message) {
	muart_puts(message);
	muart_puts("\r\n");

}

void muart_printHex(const char *message, uint32_t hexi) {
	if(loaded == 0) return; //if video isn't loaded don't bother

	// TODO disabilitata perche usa memcpy della stdlib
	// char hex[16] = {'0','1','2','3','4','5','6','7',
	// 				'8','9','A','B','C','D','E','F'};

	char *hex;
	memcpy(hex, "0123456789ABCDEF", sizeof("0123456789ABCDEF"));
	
	char m[200];
	int i = 0;
	while (*message){
		m[i] = *message++;
		i++;
	}
	//overwrite the null terminator
	m[i + 0] = hex[(hexi >> 28)&0xF];
	m[i + 1] = hex[(hexi >> 24)&0xF];
	m[i + 2] = hex[(hexi >> 20)&0xF];
	m[i + 3] = hex[(hexi >> 16)&0xF];
	m[i + 4] = hex[(hexi >> 12)&0xF];
	m[i + 5] = hex[(hexi >> 8)&0xF];
	m[i + 6] = hex[(hexi >> 4)&0xF];
	m[i + 7] = hex[(hexi >> 0)&0xF];
	m[i + 8] = 0; //null termination

	muart_println(m);
}

unsigned char muart_getc() {
	while(1) if(pRegs->AUX_MU_LSR_REG & FIFO_DATA_READY) break;
	return (unsigned char) pRegs->AUX_MU_IO_REG;
}
