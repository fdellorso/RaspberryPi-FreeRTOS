/**
 *	Tiny Interrupt Manager
 *
 *	@author James Walmsley <james@fullfat-fs.co.uk>
 *	This code is licensed under the GNU GPLv3 license.
 **/

#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include <bcm2835_base.h>

#define BCM2835_INTC_TOTAL_IRQ		64 + 8

#define BCM2835_BASE_INTC			(BCM2835_PERI_BASE + 0xB200)
#define BCM2835_INTC_IRQ_BASIC		(BCM2835_BASE_INTC + 0x00)
#define BCM2835_IRQ_PENDING1		(BCM2835_BASE_INTC + 0x04)
#define BCM2835_IRQ_PENDING2		(BCM2835_BASE_INTC + 0x08)
#define BCM2835_IRQ_FIQ_CTRL		(BCM2835_BASE_INTC + 0x0C)
#define BCM2835_IRQ_ENABLE1			(BCM2835_BASE_INTC + 0x10)
#define BCM2835_IRQ_ENABLE2			(BCM2835_BASE_INTC + 0x14)
#define BCM2835_IRQ_ENABLE_BASIC	(BCM2835_BASE_INTC + 0x18)
#define BCM2835_IRQ_DISABLE1		(BCM2835_BASE_INTC + 0x1C)
#define BCM2835_IRQ_DISABLE2		(BCM2835_BASE_INTC + 0x20)
#define BCM2835_IRQ_DISABLE_BASIC	(BCM2835_BASE_INTC + 0x24)

#define BCM2835_IRQ_ID_ST_C0		0       // Reserved by VideoCore
#define BCM2835_IRQ_ID_ST_C1		1
#define BCM2835_IRQ_ID_ST_C2		2       // Reserved by VideoCore
#define BCM2835_IRQ_ID_ST_C3		3
#define BCM2835_IRQ_ID_USB			9
#define BCM2835_IRQ_ID_AUX			29
#define BCM2835_IRQ_ID_SPI_SLAVE 	43
#define BCM2835_IRQ_ID_PWA0			45
#define BCM2835_IRQ_ID_PWA1		   	46
#define BCM2835_IRQ_ID_SMI 			48
#define BCM2835_IRQ_ID_GPIO_0 		49
#define BCM2835_IRQ_ID_GPIO_1 		50
#define BCM2835_IRQ_ID_GPIO_2 		51
#define BCM2835_IRQ_ID_GPIO_3 		52
#define BCM2835_IRQ_ID_I2C 			53
#define BCM2835_IRQ_ID_SPI 			54
#define BCM2835_IRQ_ID_PCM 			55
#define BCM2835_IRQ_ID_UART 		57
#define BCM2835_IRQ_ID_SD    		62

#define BCM2835_IRQ_ID_TIMER_0 		64
#define BCM2835_IRQ_ID_MAILBOX_0	65
#define BCM2835_IRQ_ID_DOORBELL_0 	66
#define BCM2835_IRQ_ID_DOORBELL_1 	67
#define BCM2835_IRQ_ID_GPU0_HALTED 	68

typedef void (*FN_INTERRUPT_HANDLER)(int nIRQ, void *pParam);

typedef struct {
	FN_INTERRUPT_HANDLER 	pfnHandler;			// Function that handles this IRQn
	void 				   *pParam;				// A special parameter that the use can pass to the IRQ.
} INTERRUPT_VECTOR;

int InitInterruptController	();
int RegisterInterrupt		(int nIRQ, FN_INTERRUPT_HANDLER pfnHandler, void *pParam);
int EnableInterrupt			(int nIRQ);
int DisableInterrupt		(int nIRQ);
int EnableInterrupts		();
int DisableInterrupts		();

#endif
