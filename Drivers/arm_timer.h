#ifndef _ARM_TIMER_H_
#define _ARM_TIMER_H_

#include <rpi_header.h>

#include <uspi/types.h>

// Base Physical Address of the System Timer registers
#define BCM2835_ARM_TIMER_BASE			(BCM2835_PERI_BASE + 0xB400)

void prvArmTimerIrqClear(void);
void prvArmTimerIrqSetup(uint32_t portTimerPrescale);

#endif
