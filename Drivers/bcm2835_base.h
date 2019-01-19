#ifndef _BCM2835_BASE_H_
#define _BCM2835_BASE_H_

#include <rpi_header.h>

// This means pin HIGH, true, 3.3volts on a pin.
#define HIGH 0x1
// This means pin LOW, false, 0volts on a pin.
#define LOW  0x0

// Speed of the core clock core_clk
#define BCM2835_CORE_CLK_HZ				250000000	// 250 MHz

// Base Physical Address of the BCM 2835 peripheral registers
#define BCM2835_PERI_BASE               0x20000000


void wait_cycles(unsigned char n);

#endif
