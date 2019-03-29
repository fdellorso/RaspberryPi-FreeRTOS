#ifndef _SYS_TIMER_H_
#define _SYS_TIMER_H_

#include <rpi_header.h>

#include <uspi/types.h>

// Base Physical Address of the System Timer registers
#define BCM2835_SYS_TIMER_BASE			(BCM2835_PERI_BASE + 0x3000)

#define ST_ONESEC                       0x000F4240
#define ST_TICK                         (0x000003E8 - 1)
#define ST_STEP_MIN                     (0x000186A0 / 20)

void prvSystemTimerTickClear(uint32_t tickPeriod);
void prvSystemTimerTickSetup(uint32_t tickPeriod);

void prvSystemTimerStepClear(void);
void prvSystemTimerStepSetup(void);

uint64_t ReadSysTimer(void);
void DelaySysTimer(uint64_t micros);
void DelayMilliSysTimer(uint64_t millis);

#endif
