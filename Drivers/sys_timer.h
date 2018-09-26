#ifndef _SYS_TIMER_H_
#define _SYS_TIMER_H_

#include "bcm2835_base.h"
#include "prv_types.h"

// Base Physical Address of the System Timer registers
#define BCM2835_SYS_TIMER_BASE			(BCM2835_PERI_BASE + 0x3000)

uint64_t ReadSysTimer(void);
void DelaySysTimer(uint64_t micros);

#endif
