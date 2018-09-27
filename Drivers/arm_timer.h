#ifndef _SYS_TIMER_H_
#define _SYS_TIMER_H_

#include "bcm2835_base.h"
#include "prv_types.h"

// Base Physical Address of the System Timer registers
#define BCM2835_ARM_TIMER_BASE			(BCM2835_PERI_BASE + 0xB400)
#define portTIMER_PRESCALE 				(unsigned long) 0xF9 

void TimerIrqClear();
void prvFreeRtosTimerSetup();

#endif