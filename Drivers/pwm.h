#ifndef _PWM_H_
#define _PWM_H_

#include <rpi_header.h>

// Base Physical Address of the PWM registers
#define BCM2835_PWM_BASE    			(BCM2835_PERI_BASE + 0x20C000)

// Defines for PWM Clock, word offsets (ie 4 byte multiples)
#define BCM2835_CM_PWM_BASE    			(BCM2835_CM_BASE + 0xA0)

#define PWM_CLEAR_FIFO  		1 << 6  // Clear FIFO

#define PWM1_MS_MODE    		1 << 15	// Run in Mark/Space mode
#define PWM1_USEFIFO    		1 << 13	// Data from FIFO
#define PWM1_REVPOLAR   		1 << 12	// Reverse polarity
#define PWM1_OFFSTATE   		1 << 11	// Ouput Off state
#define PWM1_REPEATFF   		1 << 10	// Repeat last value if FIFO empty
#define PWM1_SERIAL     		1 << 9  // Run in serial mode
#define PWM1_ENABLE     		1 << 8  // Channel Enable

#define PWM0_MS_MODE    		1 << 7	// Run in Mark/Space mode
#define PWM0_USEFIFO    		1 << 5  // Data from FIFO
#define PWM0_REVPOLAR   		1 << 4  // Reverse polarity
#define PWM0_OFFSTATE   		1 << 3  // Ouput Off state
#define PWM0_REPEATFF   		1 << 2  // Repeat last value if FIFO empty
#define PWM0_SERIAL     		1 << 1  // Run in serial mode
#define PWM0_ENABLE     		1 << 0  // Channel Enable

typedef enum {
	PWM_CLOCK_DIVIDER_32768 = 	32768,	// 32768	= 585Hz
	PWM_CLOCK_DIVIDER_16384 = 	16384,	// 16384	= 1171.8Hz
	PWM_CLOCK_DIVIDER_8192  = 	8192, 	// 8192		= 2.34375kHz
	PWM_CLOCK_DIVIDER_4096  = 	4096, 	// 4096		= 4.6875kHz
	PWM_CLOCK_DIVIDER_2048  = 	2048, 	// 2048		= 9.375kHz
	PWM_CLOCK_DIVIDER_1024  = 	1024, 	// 1024		= 18.75kHz
	PWM_CLOCK_DIVIDER_512   = 	512,  	// 512		= 37.5kHz
	PWM_CLOCK_DIVIDER_256   = 	256,  	// 256		= 75kHz
	PWM_CLOCK_DIVIDER_128   = 	128,  	// 128		= 150kHz
	PWM_CLOCK_DIVIDER_64    = 	64,   	// 64		= 300kHz
	PWM_CLOCK_DIVIDER_32    = 	32,   	// 32		= 600.0kHz
	PWM_CLOCK_DIVIDER_16    = 	16,   	// 16		= 1.2MHz
	PWM_CLOCK_DIVIDER_8     = 	8,    	// 8		= 2.4MHz
	PWM_CLOCK_DIVIDER_4     = 	4,    	// 4		= 4.8MHz
	PWM_CLOCK_DIVIDER_2     = 	2,    	// 2		= 9.6MHz, fastest you can get
	PWM_CLOCK_DIVIDER_1     = 	1,    	// 1		= 4.6875kHz, same as divider 4096
} PWMClockDivider;


void PwmSetClock(uint32_t divisor);
void PwmSetMode(uint8_t channel, uint8_t markspace, uint8_t enabled);
void PwmSetRange(uint8_t channel, uint32_t range);
void PwmSetData(uint8_t channel, uint32_t data);

#endif
