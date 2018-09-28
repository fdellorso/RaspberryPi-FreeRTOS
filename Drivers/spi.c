/**
 *	Quick and very Dirty SPI API.
 *
 **/

#include "spi.h"

#include "prv_types.h"

typedef struct {
	unsigned long	CS;	  
	unsigned long	FIFO;
	unsigned long	CLK;
    unsigned long	DLEN;
    unsigned long	LTOH;
    unsigned long	DC;
} BCM2835_SPI0_REGS;

static volatile BCM2835_SPI0_REGS * const pRegs = (BCM2835_SPI0_REGS *) (BCM2835_SPI0_BASE);

void bcm2835_spi_begin(void)
{
	// Set the SPI0 pins to the Alt 0 function to enable SPI0 access on them
	bcm2835_gpio_fsel(RPI_GPIO_P1_26, BCM2835_GPIO_FSEL_ALT0); // CE1
	bcm2835_gpio_fsel(RPI_GPIO_P1_24, BCM2835_GPIO_FSEL_ALT0); // CE0
	bcm2835_gpio_fsel(RPI_GPIO_P1_21, BCM2835_GPIO_FSEL_ALT0); // MISO
	bcm2835_gpio_fsel(RPI_GPIO_P1_19, BCM2835_GPIO_FSEL_ALT0); // MOSI
	bcm2835_gpio_fsel(RPI_GPIO_P1_23, BCM2835_GPIO_FSEL_ALT0); // CLK

	// Set the SPI CS register to the some sensible defaults
	volatile uint32_t* paddr = bcm2835_spi0 + BCM2835_SPI0_CS/4;
	bcm2835_peri_write(paddr, 0); // All 0s

	// Clear TX and RX fifos
	bcm2835_peri_write_nb(paddr, BCM2835_SPI0_CS_CLEAR);
}

void bcm2835_spi_end(void)
{
	// Set all the SPI0 pins back to input
	bcm2835_gpio_fsel(RPI_GPIO_P1_26, BCM2835_GPIO_FSEL_INPT); // CE1
	bcm2835_gpio_fsel(RPI_GPIO_P1_24, BCM2835_GPIO_FSEL_INPT); // CE0
	bcm2835_gpio_fsel(RPI_GPIO_P1_21, BCM2835_GPIO_FSEL_INPT); // MISO
	bcm2835_gpio_fsel(RPI_GPIO_P1_19, BCM2835_GPIO_FSEL_INPT); // MOSI
	bcm2835_gpio_fsel(RPI_GPIO_P1_23, BCM2835_GPIO_FSEL_INPT); // CLK
}

void bcm2835_spi_setBitOrder(uint8_t order)
{
	// BCM2835_SPI_BIT_ORDER_MSBFIRST is the only one suported by SPI0
}

// defaults to 0, which means a divider of 65536.
// The divisor must be a power of 2. Odd numbers
// rounded down. The maximum SPI clock rate is
// of the APB clock
void bcm2835_spi_setClockDivider(uint16_t divider)
{
	volatile uint32_t* paddr = bcm2835_spi0 + BCM2835_SPI0_CLK/4;
	bcm2835_peri_write(paddr, divider);
}

void bcm2835_spi_setDataMode(uint8_t mode)
{
	volatile uint32_t* paddr = bcm2835_spi0 + BCM2835_SPI0_CS/4;
	// Mask in the CPO and CPHA bits of CS
	bcm2835_peri_set_bits(paddr, mode << 2, BCM2835_SPI0_CS_CPOL | BCM2835_SPI0_CS_CPHA);
}

// Writes (and reads) a single byte to SPI
uint8_t bcm2835_spi_transfer(uint8_t value)
{
	volatile uint32_t* paddr = bcm2835_spi0 + BCM2835_SPI0_CS/4;
	volatile uint32_t* fifo = bcm2835_spi0 + BCM2835_SPI0_FIFO/4;

	// This is Polled transfer as per section 10.6.1
	// BUG ALERT: what happens if we get interupted in this section, and someone else
	// accesses a different peripheral? 
	// Clear TX and RX fifos
	bcm2835_peri_set_bits(paddr, BCM2835_SPI0_CS_CLEAR, BCM2835_SPI0_CS_CLEAR);

	// Set TA = 1
	bcm2835_peri_set_bits(paddr, BCM2835_SPI0_CS_TA, BCM2835_SPI0_CS_TA);

	// Maybe wait for TXD
	while (!(bcm2835_peri_read(paddr) & BCM2835_SPI0_CS_TXD))
		;

	// Write to FIFO, no barrier
	bcm2835_peri_write_nb(fifo, value);

	// Wait for DONE to be set
	while (!(bcm2835_peri_read_nb(paddr) & BCM2835_SPI0_CS_DONE))
		;

	// Read any byte that was sent back by the slave while we sere sending to it
	uint32_t ret = bcm2835_peri_read_nb(fifo);

	// Set TA = 0, and also set the barrier
	bcm2835_peri_set_bits(paddr, 0, BCM2835_SPI0_CS_TA);

	return ret;
}

// Writes (and reads) an number of bytes to SPI
void bcm2835_spi_transfernb(char* tbuf, char* rbuf, uint32_t len)
{
	volatile uint32_t* paddr = bcm2835_spi0 + BCM2835_SPI0_CS/4;
	volatile uint32_t* fifo = bcm2835_spi0 + BCM2835_SPI0_FIFO/4;
	uint32_t TXCnt=0;
	uint32_t RXCnt=0;

	// This is Polled transfer as per section 10.6.1
	// BUG ALERT: what happens if we get interupted in this section, and someone else
	// accesses a different peripheral? 

	// Clear TX and RX fifos
	bcm2835_peri_set_bits(paddr, BCM2835_SPI0_CS_CLEAR, BCM2835_SPI0_CS_CLEAR);

	// Set TA = 1
	bcm2835_peri_set_bits(paddr, BCM2835_SPI0_CS_TA, BCM2835_SPI0_CS_TA);

	// Use the FIFO's to reduce the interbyte times
	while((TXCnt < len)||(RXCnt < len))
	{
		// TX fifo not full, so add some more bytes
		while(((bcm2835_peri_read(paddr) & BCM2835_SPI0_CS_TXD))&&(TXCnt < len ))
		{
			bcm2835_peri_write_nb(fifo, tbuf[TXCnt]);
			TXCnt++;
		}
		//Rx fifo not empty, so get the next received bytes
		while(((bcm2835_peri_read(paddr) & BCM2835_SPI0_CS_RXD))&&( RXCnt < len ))
		{
			rbuf[RXCnt] = bcm2835_peri_read_nb(fifo);
			RXCnt++;
		}
	}
	// Wait for DONE to be set
	while (!(bcm2835_peri_read_nb(paddr) & BCM2835_SPI0_CS_DONE))
		;

	// Set TA = 0, and also set the barrier
	bcm2835_peri_set_bits(paddr, 0, BCM2835_SPI0_CS_TA);
}

// Writes an number of bytes to SPI
void bcm2835_spi_writenb(char* tbuf, uint32_t len)
{
	volatile uint32_t* paddr = bcm2835_spi0 + BCM2835_SPI0_CS/4;
	volatile uint32_t* fifo = bcm2835_spi0 + BCM2835_SPI0_FIFO/4;

	// This is Polled transfer as per section 10.6.1
	// BUG ALERT: what happens if we get interupted in this section, and someone else
	// accesses a different peripheral?

	// Clear TX and RX fifos
	bcm2835_peri_set_bits(paddr, BCM2835_SPI0_CS_CLEAR, BCM2835_SPI0_CS_CLEAR);

	// Set TA = 1
	bcm2835_peri_set_bits(paddr, BCM2835_SPI0_CS_TA, BCM2835_SPI0_CS_TA);

	uint32_t i;
	for (i = 0; i < len; i++)
	{
		// Maybe wait for TXD
		while (!(bcm2835_peri_read(paddr) & BCM2835_SPI0_CS_TXD))
			;

		// Write to FIFO, no barrier
		bcm2835_peri_write_nb(fifo, tbuf[i]);

		// Read from FIFO to prevent stalling
		while (bcm2835_peri_read(paddr) & BCM2835_SPI0_CS_RXD)
			(void) bcm2835_peri_read_nb(fifo);
	}

	// Wait for DONE to be set
	while (!(bcm2835_peri_read_nb(paddr) & BCM2835_SPI0_CS_DONE)) {
		while (bcm2835_peri_read(paddr) & BCM2835_SPI0_CS_RXD)
			(void) bcm2835_peri_read_nb(fifo);
	};

	// Set TA = 0, and also set the barrier
	bcm2835_peri_set_bits(paddr, 0, BCM2835_SPI0_CS_TA);
}

// Writes (and reads) an number of bytes to SPI
// Read bytes are copied over onto the transmit buffer
void bcm2835_spi_transfern(char* buf, uint32_t len)
{
	bcm2835_spi_transfernb(buf, buf, len);
}

void bcm2835_spi_chipSelect(uint8_t cs)
{
	volatile uint32_t* paddr = bcm2835_spi0 + BCM2835_SPI0_CS/4;
	// Mask in the CS bits of CS
	bcm2835_peri_set_bits(paddr, cs, BCM2835_SPI0_CS_CS);
}

void bcm2835_spi_setChipSelectPolarity(uint8_t cs, uint8_t active)
{
	volatile uint32_t* paddr = bcm2835_spi0 + BCM2835_SPI0_CS/4;
	uint8_t shift = 21 + cs;
	// Mask in the appropriate CSPOLn bit
	bcm2835_peri_set_bits(paddr, active << shift, 1 << shift);
}