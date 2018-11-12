#ifndef _GPIO_H_
#define _GPIO_H_

#include <rpi_header.h>

// Base Physical Address of the GPIO registers
#define BCM2835_GPIO_BASE               (BCM2835_PERI_BASE + 0x200000)

/* GPIO event detect types */
enum DETECT_TYPE {
	DETECT_NONE,
	DETECT_RISING,
	DETECT_FALLING,
	DETECT_HIGH,
	DETECT_LOW,
	DETECT_RISING_ASYNC,
	DETECT_FALLING_ASYNC
};

/* GPIO pull up or down states */
enum PULL_STATE {
	PULL_DISABLE,
	PULL_DOWN,
	PULL_UP,
	PULL_RESERVED
};

/* Pin data direction */
enum GPIO_DIR {
	GPIO_IN,
	GPIO_OUT
};

// Pin Alternate functions
enum GPIO_FUNC {
	GPIO_FUNC_INPUT		= 0,
	GPIO_FUNC_OUTPUT	= 1,
	GPIO_FUNC_ALT_0		= 4,	// Alternative function 0
	GPIO_FUNC_ALT_1		= 5,	// Alternative function 1
	GPIO_FUNC_ALT_2		= 6,	// Alternative function 2
	GPIO_FUNC_ALT_3		= 7,	// Alternative function 3
	GPIO_FUNC_ALT_4		= 3,	// Alternative function 4
	GPIO_FUNC_ALT_5		= 2		// Alternative function 5
};

/* GPIO pin setup */
// void SetGpioFunction	(unsigned int pinNum, unsigned int funcNum);
void SetGpioFunction	(unsigned int pinNum, enum GPIO_FUNC funcNum);
/* A simple wrapper around SetGpioFunction */
void SetGpioDirection	(unsigned int pinNum, enum GPIO_DIR dir);

/* Set GPIO output level */
void SetGpio			(unsigned int pinNum, unsigned int pinVal);

/* Read GPIO pin level */
int ReadGpio			(unsigned int pinNum);

/* GPIO pull up/down resistor control function (NOT YET IMPLEMENTED) */
void PudGpio				(unsigned int pinNum, enum PULL_STATE state);

/* Interrupt related functions */
void EnableGpioDetect	(unsigned int pinNum, enum DETECT_TYPE type);
void DisableGpioDetect	(unsigned int pinNum, enum DETECT_TYPE type);
void ClearGpioInterrupt	(unsigned int pinNum);

#endif
