#ifndef _SPI_H_
#define _SPI_H_

#include <rpi_header.h>

// Base Physical Address of the SPI0 registers
#define BCM2835_SPI0_BASE    			(BCM2835_PERI_BASE + 0x204000)

// SPI0 GPIO Pins
#define SPI0_CLK        11
#define SPI0_MOSI       10
#define SPI0_MISO       9
#define SPI0_CE0        8
#define SPI0_CE1        7


#endif
