#ifndef _SYS_TIMER_H_
#define _SYS_TIMER_H_

#include <rpi_header.h>
#include <prvlib/stdint.h>

// Base Physical Address of the System Timer registers
#define BCM2835_SYS_TIMER_BASE			(BCM2835_PERI_BASE + 0x3000)

#define ST_ONESEC                       0x000F4240
#define ST_TICK                         0x000003E8 - 1

void prvSystemTimerIrqClear(void);
void prvSystemTimerSetup(void);

uint64_t ReadSysTimer(void);
void DelaySysTimer(uint64_t micros);
void DelayMilliSysTimer(uint64_t millis);

#endif
