#ifndef _SPI_H_
#define _SPI_H_

#include <rpi_header.h>
#include <prvlib/stdint.h>

// Base Physical Address of the SPI0 registers
#define BCM2835_SPI0_BASE    			(BCM2835_PERI_BASE + 0x204000)

// SPI0 GPIO Pins
#define SPI0_CLK                    11
#define SPI0_MOSI                   10
#define SPI0_MISO                   9
#define SPI0_CE0                    8
#define SPI0_CE1                    7

#define SPI0_CS_CLK_PHASE           1 << 2
#define SPI0_CS_CLK_POLARITY        1 << 3
#define SPI0_CS_CLEAR_FIFO_TX       1 << 4
#define SPI0_CS_CLEAR_FIFO_RX       1 << 5
#define SPI0_CS_TRANSFER_ACTIVE     1 << 7
#define SPI0_CS_DONE                1 << 16
#define SPI0_CS_RXD                 1 << 17
#define SPI0_CS_TXD                 1 << 18
#define SPI0_CS_CHIP_SELECT0_POL    1 << 21
#define SPI0_CS_CHIP_SELECT1_POL    1 << 22
#define SPI0_CS_CHIP_SELECT2_POL    1 << 23

typedef enum {
	SPI0_MODE0 =                    0,      ///< CPOL = 0, CPHA = 0
	SPI0_MODE1 =                    1,      ///< CPOL = 0, CPHA = 1
	SPI0_MODE2 =                    2,      ///< CPOL = 1, CPHA = 0
	SPI0_MODE3 =                    3,      ///< CPOL = 1, CPHA = 1
} SPI0Mode;

typedef enum {
	SPI0_CS0     =                  0,      ///< Chip Select 0
	SPI0_CS1     =                  1,      ///< Chip Select 1
	SPI0_CS2     =                  2,      ///< Chip Select 2 (ie pins CS1 and CS2 are asserted)
	SPI0_CS_NONE =                  3,      ///< No CS, control it yourself
} SPI0ChipSelect;

typedef enum {
	SPI0_CLOCK_DIVIDER_65536 =      0,       ///< 65536 = 262.144us = 3.814697260kHz
	SPI0_CLOCK_DIVIDER_32768 =      32768,   ///< 32768 = 131.072us = 7.629394531kHz
	SPI0_CLOCK_DIVIDER_16384 =      16384,   ///< 16384 = 65.536us = 15.25878906kHz
	SPI0_CLOCK_DIVIDER_8192  =      8192,    ///< 8192 = 32.768us = 30/51757813kHz
	SPI0_CLOCK_DIVIDER_4096  =      4096,    ///< 4096 = 16.384us = 61.03515625kHz
	SPI0_CLOCK_DIVIDER_2048  =      2048,    ///< 2048 = 8.192us = 122.0703125kHz
	SPI0_CLOCK_DIVIDER_1024  =      1024,    ///< 1024 = 4.096us = 244.140625kHz
	SPI0_CLOCK_DIVIDER_512   =      512,     ///< 512 = 2.048us = 488.28125kHz
	SPI0_CLOCK_DIVIDER_256   =      256,     ///< 256 = 1.024us = 976.5625MHz
	SPI0_CLOCK_DIVIDER_128   =      128,     ///< 128 = 512ns = = 1.953125MHz
	SPI0_CLOCK_DIVIDER_64    =      64,      ///< 64 = 256ns = 3.90625MHz
	SPI0_CLOCK_DIVIDER_32    =      32,      ///< 32 = 128ns = 7.8125MHz
	SPI0_CLOCK_DIVIDER_16    =      16,      ///< 16 = 64ns = 15.625MHz
	SPI0_CLOCK_DIVIDER_8     =      8,       ///< 8 = 32ns = 31.25MHz
	SPI0_CLOCK_DIVIDER_4     =      4,       ///< 4 = 16ns = 62.5MHz
	SPI0_CLOCK_DIVIDER_2     =      2,       ///< 2 = 8ns = 125MHz, fastest you can get
	SPI0_CLOCK_DIVIDER_1     =      1,       ///< 1 = 262.144us = 3.814697260kHz, same as 0/65536
} SPI0ClockDivider;


void spi0_init(void);
void spi0_close(void);
void spi0_set_clock_divider(uint16_t divider);
void spi0_set_data_mode(uint8_t mode);
void spi0_set_chip_select_polarity(uint8_t cs, uint8_t active);
void spi0_chip_select(uint8_t cs);
uint8_t spi0_transfer(uint8_t value);
void spi0_transfernb(char* tbuf, char* rbuf, uint32_t len);
void spi0_writenb(char* tbuf, uint32_t len);
void spi0_transfern(char* buf, uint32_t len);

#endif
