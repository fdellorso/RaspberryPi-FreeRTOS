/**
 *	Quick and very Dirty GPIO API.
 *
 **/

#include "gpio.h"

typedef struct {
	unsigned long	GPFSEL[6];	///< Function selection registers.
	unsigned long	Reserved_1;
	unsigned long	GPSET[2];
	unsigned long	Reserved_2;
	unsigned long	GPCLR[2];
	unsigned long	Reserved_3;
	unsigned long	GPLEV[2];
	unsigned long	Reserved_4;
	unsigned long	GPEDS[2];
	unsigned long	Reserved_5;
	unsigned long	GPREN[2];
	unsigned long	Reserved_6;
	unsigned long	GPFEN[2];
	unsigned long	Reserved_7;
	unsigned long	GPHEN[2];
	unsigned long	Reserved_8;
	unsigned long	GPLEN[2];
	unsigned long	Reserved_9;
	unsigned long	GPAREN[2];
	unsigned long	Reserved_A;
	unsigned long	GPAFEN[2];
	unsigned long	Reserved_B;
	unsigned long	GPPUD[1];
	unsigned long	GPPUDCLK[2];
	//Ignoring the reserved and test bytes
} BCM2835_GPIO_REGS;

static volatile BCM2835_GPIO_REGS * const pRegs = (BCM2835_GPIO_REGS *) (BCM2835_GPIO_BASE);

void SetGpioFunction(unsigned int pinNum, enum GPIO_FUNC funcNum) {
	unsigned long offset = pinNum/10;
	unsigned long item 	 = pinNum % 10;
	unsigned long mask = 7UL << (item * 3);

	// Read in the original register value.
	unsigned long val	 = pRegs->GPFSEL[offset];

	// Mask out the bits for this pin.
	// val &= ~(0x7 << (item * 3));
	val &= ~mask;

	// Insert new bits for this pin.
	// val |= ((funcNum & 0x7) << (item * 3));
	val |= (unsigned long)funcNum << (item * 3);

	// Store back.
	pRegs->GPFSEL[offset] = val;
}

void SetGpioDirection(unsigned int pinNum, enum GPIO_DIR dir) {
	SetGpioFunction(pinNum,dir);
}

void SetGpio(unsigned int pinNum, unsigned int pinVal) {
	unsigned long offset = pinNum/32;
	unsigned long mask	 = (1<<(pinNum%32));

	if(pinVal) pRegs->GPSET[offset]|=mask;
	else 	   pRegs->GPCLR[offset]|=mask;
}

int ReadGpio(unsigned int pinNum) {
	return ((pRegs->GPLEV[pinNum/32])>>(pinNum%32))&1;
}

int PudGpio(unsigned int pinNum, enum PULL_STATE state) {
	unsigned long offset = pinNum/32;
	unsigned long mask	 = (1<<(pinNum%32));

	// Setup direction register:
	pRegs->GPPUD[1] = state;

	// Wait 150 cycles to set up the control signal:
	wait_cycles(150);

	// Activate clock signal for this pin:
	pRegs->GPPUD[offset] |= mask;

	// Wait 150 cycles to hold the control signal:
	wait_cycles(150);

	// Remove control signal:
	pRegs->GPPUD[1] = 0;

	// Wait 150 cycles to hold the control signal:
	wait_cycles(150);

	// Remove clock signal:
	pRegs->GPPUD[offset] = 0;
}

void EnableGpioDetect(unsigned int pinNum, enum DETECT_TYPE type) {
	unsigned long offset = pinNum/32;
	unsigned long mask	 = (1<<pinNum);
	
	switch(type) {
	case DETECT_RISING:
		pRegs->GPREN[offset]|=mask;
		break;
	case DETECT_FALLING:
		pRegs->GPFEN[offset]|=mask;
		break;
	case DETECT_HIGH:
		pRegs->GPHEN[offset]|=mask;
		break;
	case DETECT_LOW:
		pRegs->GPLEN[offset]|=mask;
		break;
	case DETECT_RISING_ASYNC:
		pRegs->GPAREN[offset]|=mask;
		break;
	case DETECT_FALLING_ASYNC:
		pRegs->GPAFEN[offset]|=mask;
		break;
	case DETECT_NONE:
		break;
	}
}

void DisableGpioDetect(unsigned int pinNum, enum DETECT_TYPE type) {
	unsigned long offset = pinNum/32;
	unsigned long mask	 = ~(1<<(pinNum%32));
	
	switch(type) {
	case DETECT_RISING:
		pRegs->GPREN[offset]&=mask;
		break;
	case DETECT_FALLING:
		pRegs->GPFEN[offset]&=mask;
		break;
	case DETECT_HIGH:
		pRegs->GPHEN[offset]&=mask;
		break;
	case DETECT_LOW:
		pRegs->GPLEN[offset]&=mask;
		break;
	case DETECT_RISING_ASYNC:
		pRegs->GPAREN[offset]&=mask;
		break;
	case DETECT_FALLING_ASYNC:
		pRegs->GPAFEN[offset]&=mask;
		break;
	case DETECT_NONE:
		break;
	}
}

void ClearGpioInterrupt(unsigned int pinNum) {
	unsigned long offset = pinNum/32;
	unsigned long mask	 = (1<<(pinNum%32));

	pRegs->GPEDS[offset]=mask;
}
