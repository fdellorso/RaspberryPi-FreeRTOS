/**
 *	Quick and very Dirty PWM API.
 *
 **/

#include "pwm.h"

typedef struct {
	unsigned long	CM_PWMCTL;	  
	unsigned long	CM_PWMDIV;
} BCM2835_CM_PWM_REGS;

static volatile BCM2835_CM_PWM_REGS * const pClk = (BCM2835_CM_PWM_REGS *) (BCM2835_CM_PWM_BASE);


typedef struct {
	unsigned long	CTL;	  
	unsigned long	STA;
	unsigned long	DMAC;
    unsigned long	Reserved_1;
    unsigned long	RNG1;
    unsigned long	DAT1;
    unsigned long	FIF1;
    unsigned long	Reserved_2;
    unsigned long	RNG2;
    unsigned long	DAT2;
} BCM2835_PWM_REGS;

static volatile BCM2835_PWM_REGS * const pRegs = (BCM2835_PWM_REGS *) (BCM2835_PWM_BASE);


void PwmSetClock(uint32_t divisor) {
	// From Gerts code
	divisor &= 0xfff;

	// Stop PWM clock
    pClk->CM_PWMCTL = CM_PASSWORD | 0x01;
    DelaySysTimer(110);                                 // Prevents clock going slow

	// Wait for the clock to be not busy
    while ((pClk->CM_PWMCTL & 0x80) != 0)
        DelaySysTimer(1);
    
	// set the clock divider and enable PWM clock
    pClk->CM_PWMDIV = CM_PASSWORD | (divisor << 12);
    pClk->CM_PWMCTL = CM_PASSWORD | 0x11;               // Source=osc and enable
}

void PwmSetMode(uint8_t channel, uint8_t markspace, uint8_t enabled) {
	uint32_t control = pRegs->CTL;

	if (channel == 0) {
		if (markspace)
			control |= PWM0_MS_MODE;
		else
			control &= ~PWM0_MS_MODE;
		if (enabled)
			control |= PWM0_ENABLE;
		else
			control &= ~PWM0_ENABLE;
	}
	else if (channel == 1) {
		if (markspace)
			control |= PWM1_MS_MODE;
		else
			control &= ~PWM1_MS_MODE;
		if (enabled)
			control |= PWM1_ENABLE;
		else
			control &= ~PWM1_ENABLE;
	}

    pRegs->CTL = control;
}

void PwmSetRange(uint8_t channel, uint32_t range) {
	if (channel == 0)
        pRegs->RNG1 = range;
	else if (channel == 1)
        pRegs->RNG2 = range;
}

void PwmSetData(uint8_t channel, uint32_t data) {
	if (channel == 0)
        pRegs->DAT1 = data;
	else if (channel == 1)
        pRegs->DAT2 = data;
}
