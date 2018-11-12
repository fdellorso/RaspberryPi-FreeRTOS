#ifndef _AUX_H_
#define _AUX_H_

#include <rpi_header.h>

// Base Physical Address of the AUX registers
#define BCM2835_AUX_BASE			(BCM2835_PERI_BASE + 0x215000)

/* GPIO pull up or down states */
enum AUXIRQ {
	Mini_UART_IRQ   = 1,
	SPI1_IRQ        = 2,
	SPI2_IRQ        = 4
};

enum AUXENB {
	Mini_UART_ENB   = 1,
	SPI1_ENB        = 2,
	SPI2_ENB        = 4
};

#endif
