#ifndef _CLK_H_
#define _CLK_H_

#include <rpi_header.h>

// Base Physical Address of the Clock Manager registers
#define BCM2835_CM_BASE					(BCM2835_PERI_BASE + 0x101000)

// Password to enable setting CM
#define CM_PASSWORD     				(0x5A << 24)

#endif
