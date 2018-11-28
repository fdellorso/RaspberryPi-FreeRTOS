#ifndef _ARM_TIMER_H_
#define _ARM_TIMER_H_

#include <rpi_header.h>

// Base Physical Address of the System Timer registers
#define BCM2835_ARM_TIMER_BASE			(BCM2835_PERI_BASE + 0xB400)
#define portTIMER_PRESCALE 				(unsigned long) 0xF9 

void prvArmTimerIrqClear(void);
void prvArmTimerSetup(void);

#endif
