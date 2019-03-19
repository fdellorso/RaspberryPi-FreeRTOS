/**
 *	Quick and very Dirty SYSTEM TIMER API.
 *
 **/

#include "arm_timer.h"

typedef struct _BCM2835_ARM_TIMER_REGS {
	unsigned long LOD;
	unsigned long VAL;
	unsigned long CTL;
	unsigned long CLI;
	unsigned long RIS;
	unsigned long MIS;
	unsigned long RLD;
	unsigned long DIV;
	unsigned long CNT;
} BCM2835_ARM_TIMER_REGS;

static volatile BCM2835_ARM_TIMER_REGS * const pRegs = (BCM2835_ARM_TIMER_REGS *) (BCM2835_ARM_TIMER_BASE);

void prvArmTimerIrqClear(void) {
	pRegs->CLI = 0;
}

void prvArmTimerIrqSetup(uint32_t portTimerPrescale) {
	pRegs->CTL = 0x003E0000;
	pRegs->LOD = 1000 - 1;
	pRegs->RLD = 1000 - 1;
	pRegs->DIV = portTimerPrescale;
	pRegs->CLI = 0;
	pRegs->CTL = 0x003E00A2;
}

