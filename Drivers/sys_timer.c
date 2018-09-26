/**
 *	Quick and very Dirty SYSTEM TIMER API.
 *
 **/

#include "sys_timer.h"

#include "prv_types.h"

typedef struct {
	unsigned long	CS;	  
	unsigned long	CLO;
	unsigned long	CHI;
	unsigned long	C0;
	unsigned long	C1;
	unsigned long	C2;
	unsigned long	C3;
} BCM2835_ST_REGS;

static volatile BCM2835_ST_REGS * const pRegs = (BCM2835_ST_REGS *) (BCM2835_ST_BASE);

// Read the System Timer Counter (64-bits)
uint64_t ReadSysTimer(void){
	return ((uint64_t)pRegs->CHI << 32) + (uint64_t)pRegs->CLO;
}
