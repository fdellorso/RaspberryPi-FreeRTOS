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

void BTUARTWrite (unsigned char nChar) {
	while (pRegs->FR & FR_TXFF_MASK)
	{
		// do nothing
	}
		
	pRegs->DR = nChar;
}

unsigned int BTUARTReadReg (UART0_REG regs) {
	switch(regs)
	{
		case UART0_DR:
			return pRegs->DR;
    	case UART0_RSRECR:
			return pRegs->RSRECR;
    	case UART0_FR:
			return pRegs->FR;
    	case UART0_ILPR:
			return pRegs->ILPR;
    	case UART0_IBRD:
			return pRegs->IBRD;
    	case UART0_FBRD:
			return pRegs->FBRD;
    	case UART0_LCRH:
			return pRegs->LCRH;
    	case UART0_CR:
			return pRegs->CR;
    	case UART0_IFLS:
			return pRegs->IFLS;
    	case UART0_IMSC:
			return pRegs->IMSC;
    	case UART0_RIS:
			return pRegs->RIS;
    	case UART0_MIS:
			return pRegs->MIS;
    	case UART0_ICR:
			return pRegs->ICR;
    	case UART0_DMACR:
			return pRegs->DMACR;
    	case UART0_ITCR:
			return pRegs->ITCR;
    	case UART0_ITIP:
			return pRegs->ITIP;
    	case UART0_ITOP:
			return pRegs->ITOP;
    	case UART0_TDR:
			return pRegs->TDR;
		default:
			return -1;
	}
}

void BTUARTWriteReg (UART0_REG regs, unsigned int toWrite) {
	switch(regs)
	{
		case UART0_DR:
			pRegs->DR = toWrite;
			break;
    	case UART0_RSRECR:
			pRegs->RSRECR = toWrite;
			break;
    	case UART0_FR:
			pRegs->FR = toWrite;
			break;
    	case UART0_ILPR:
			pRegs->ILPR = toWrite;
			break;
    	case UART0_IBRD:
			pRegs->IBRD = toWrite;
			break;
    	case UART0_FBRD:
			pRegs->FBRD = toWrite;
			break;
    	case UART0_LCRH:
			pRegs->LCRH = toWrite;
			break;
    	case UART0_CR:
			pRegs->CR = toWrite;
			break;
    	case UART0_IFLS:
			pRegs->IFLS = toWrite;
			break;
    	case UART0_IMSC:
			pRegs->IMSC = toWrite;
			break;
    	case UART0_RIS:
			pRegs->RIS = toWrite;
			break;
    	case UART0_MIS:
			pRegs->MIS = toWrite;
			break;
    	case UART0_ICR:
			pRegs->ICR = toWrite;
			break;
    	case UART0_DMACR:
			pRegs->DMACR = toWrite;
			break;
    	case UART0_ITCR:
			pRegs->ITCR = toWrite;
			break;
    	case UART0_ITIP:
			pRegs->ITIP = toWrite;
			break;
    	case UART0_ITOP:
			pRegs->ITOP = toWrite;
			break;
    	case UART0_TDR:
			pRegs->TDR = toWrite;
			break;
		default:
			break;
	}
}
