#ifndef _UART0_H_
#define _UART0_H_

#include <rpi_header.h>
#include <uspi/string.h>

// Base Physical Address of the Mini UART registers
#define BCM2835_UART0_BASE			(BCM2835_PERI_BASE + 0x201000)

// Definitions from Raspberry PI Remote Serial Protocol.
//     Copyright 2012 Jamie Iles, jamie@jamieiles.com.
//     Licensed under GPLv2
#define DR_OE_MASK			(1 << 11)
#define DR_BE_MASK			(1 << 10)
#define DR_PE_MASK			(1 << 9)
#define DR_FE_MASK			(1 << 8)

#define FR_TXFE_MASK		(1 << 7)
#define FR_RXFF_MASK		(1 << 6)
#define FR_TXFF_MASK		(1 << 5)
#define FR_RXFE_MASK		(1 << 4)
#define FR_BUSY_MASK		(1 << 3)

#define LCRH_SPS_MASK		(1 << 7)
#define LCRH_WLEN8_MASK		(3 << 5)
#define LCRH_WLEN7_MASK		(2 << 5)
#define LCRH_WLEN6_MASK		(1 << 5)
#define LCRH_WLEN5_MASK		(0 << 5)
#define LCRH_FEN_MASK		(1 << 4)
#define LCRH_STP2_MASK		(1 << 3)
#define LCRH_EPS_MASK		(1 << 2)
#define LCRH_PEN_MASK		(1 << 1)
#define LCRH_BRK_MASK		(1 << 0)

#define CR_CTSEN_MASK		(1 << 15)
#define CR_RTSEN_MASK		(1 << 14)
#define CR_OUT2_MASK		(1 << 13)
#define CR_OUT1_MASK		(1 << 12)
#define CR_RTS_MASK			(1 << 11)
#define CR_DTR_MASK			(1 << 10)
#define CR_RXE_MASK			(1 << 9)
#define CR_TXE_MASK			(1 << 8)
#define CR_LBE_MASK			(1 << 7)
#define CR_UART_EN_MASK		(1 << 0)

#define IFLS_RXIFSEL_SHIFT	3
#define IFLS_RXIFSEL_MASK	(7 << IFLS_RXIFSEL_SHIFT)
#define IFLS_TXIFSEL_SHIFT	0
#define IFLS_TXIFSEL_MASK	(7 << IFLS_TXIFSEL_SHIFT)
#define IFLS_IFSEL_1_8	    0
#define IFLS_IFSEL_1_4	    1
#define IFLS_IFSEL_1_2	    2
#define IFLS_IFSEL_3_4	    3
#define IFLS_IFSEL_7_8	    4

#define INT_OE				(1 << 10)
#define INT_BE				(1 << 9)
#define INT_PE				(1 << 8)
#define INT_FE				(1 << 7)
#define INT_RT				(1 << 6)
#define INT_TX				(1 << 5)
#define INT_RX				(1 << 4)
#define INT_DSRM			(1 << 3)
#define INT_DCDM			(1 << 2)
#define INT_CTSM			(1 << 1)


void BTUARTClose(void);

void BTUARTInit(unsigned int nIntDiv, unsigned int nFractDiv);

#endif
