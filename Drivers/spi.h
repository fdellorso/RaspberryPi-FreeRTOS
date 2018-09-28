#ifndef _SYS_TIMER_H_
#define _SYS_TIMER_H_

#include "bcm2835_base.h"
#include "prv_types.h"

// Base Physical Address of the SPI0 registers
#define BCM2835_SPI0_BASE    			(BCM2835_PERI_BASE + 0x204000)

// SPI0 GPIO Pins
#define SPI0_CLK        11
#define SPI0_MOSI       10
#define SPI0_MISO       9
#define SPI0_CE0        8
#define SPI0_CE1        7


#endif
