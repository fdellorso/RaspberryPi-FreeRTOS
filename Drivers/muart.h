#ifndef _MUART_H_
#define _MUART_H_

#include <rpi_header.h>

// Base Physical Address of the Mini UART registers
#define BCM2835_MUART_BASE			(BCM2835_PERI_BASE + 0x215040)

#define FIFO_ENABLES                0xC0
#define FIFO_CLEAR_RECEIVE          0x02
#define FIFO_CLEAR_TRANSMIT         0x04
#define FIFO_TRANSMITTER_IDLE       0x40
#define FIFO_TRANSMITTER_EMPTY      0x20
#define FIFO_DATA_READY             0x01

#define LINE_FEED                   0x0A
#define CARRIAGE_RETURN             0x0D

#define CLEAR_SCREEN                "\033[2J"

extern char loaded;

void muart_init(void);
void muart_putc(unsigned char c);
void muart_puts(const char *str);
void muart_println(const char *message);
void muart_printHex(const char *message, unsigned int hexi);
void muart_printf(const char *pMessage, ...);
unsigned char muart_getc(void);

#endif
