/**
 *	Quick and very Dirty UART PL011 API.
 *
 **/

#include "uart0.h"

typedef struct {
	unsigned long	DR;				// 00	  
	unsigned long	RSRECR;			// 04
	unsigned long	Reserved_1[4];	// 08 0C 10 14
	unsigned long	FR;				// 18
	unsigned long	Reserved_2;		// 1C
	unsigned long	ILPR;			// 20
	unsigned long	IBRD;			// 24
	unsigned long	FBRD;			// 28
	unsigned long	LCRH;			// 2C
	unsigned long	CR;				// 30
	unsigned long	IFLS;			// 34
	unsigned long	IMSC;			// 38
	unsigned long	RIS;			// 3C
	unsigned long	MIS;			// 40
	unsigned long	ICR;			// 44
	unsigned long	DMACR;			// 48
	unsigned long	Reserved_3[13];	// 4C 50 54 58 5C 60 64 68 6C 70 74 78 7C
	unsigned long	ITCR;			// 80
	unsigned long	ITIP;			// 84
	unsigned long	ITOP;			// 88
	unsigned long	TDR;			// 8C
} BCM2835_UART0_REGS;

static volatile BCM2835_UART0_REGS * const pRegs = (BCM2835_UART0_REGS *) (BCM2835_UART0_BASE);

void BTUARTClose(void) {
	pRegs->IMSC = 0;
	pRegs->CR = 0;
}

void BTUARTInit(unsigned int nIntDiv, unsigned int nFractDiv) {
	pRegs->IMSC	= 0;
	pRegs->ICR	= 0x7FF;
	pRegs->IBRD	= nIntDiv;
	pRegs->FBRD	= nFractDiv;
	pRegs->IFLS	= IFLS_IFSEL_1_4 << IFLS_RXIFSEL_SHIFT;
	pRegs->LCRH	= LCRH_WLEN8_MASK | LCRH_FEN_MASK;				// 8N1
	pRegs->CR	= CR_UART_EN_MASK | CR_TXE_MASK | CR_RXE_MASK;
	pRegs->IMSC	= INT_RX | INT_RT | INT_OE;
}