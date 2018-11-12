#ifndef _MAILBOX_H_
#define _MAILBOX_H_

#include <rpi_header.h>

// Base Physical Address of the mailbox and framebuffer registers
#define BCM2835_MAIL0_BASE				(BCM2835_PERI_BASE + 0xB880)

#define BCM2835_MAIL0_READ	            (BCM2835_MAIL0_BASE + 0x00)     // The read register for mailbox 0 at offset (the Linux source mentions something of "and the next 4 words", but I've found it sufficient to read only from this address)
#define BCM2835_MAIL0_PEAK	            (BCM2835_MAIL0_BASE + 0x10)     // Read from the mailbox without removing data from it
#define BCM2835_MAIL0_SENDER            (BCM2835_MAIL0_BASE + 0x14)     // Sender ID (bottom 2 bits only)
#define BCM2835_MAIL0_STATUS            (BCM2835_MAIL0_BASE + 0x18)	    // The status register for mailbox 0
#define BCM2835_MAIL0_CONFIG            (BCM2835_MAIL0_BASE + 0x1C)	    // The configuration register for mailbox 0
#define BCM2835_MAIL0_WRITE             (BCM2835_MAIL0_BASE + 0x20)     // The write register for mailbox 0 (this is actually the read register for mailbox 1)

#define MAIL_EMPTY  (1UL << 30)		// Set if the mailbox is empty, and thus no more data is available to be read from it.
#define MAIL_FULL   (1UL << 31)	    // Set if the mailbox is full, and thus no more data can be written to it.

// https://github.com/raspberrypi/firmware/wiki/Mailboxes
enum MAIL0_CHANNELS{
	BCM2835_MAIL0_POWER_MANAGEMENT	= 0,
	BCM2835_MAIL0_FRAMEBUFFER		= 1,
	BCM2835_MAIL0_VIRTUAL_UART		= 2,
	BCM2835_MAIL0_VCHIQ				= 3,
	BCM2835_MAIL0_LED				= 4,
	BCM2835_MAIL0_BUTTONS			= 5,
	BCM2835_MAIL0_TOUCH_SCREEN		= 6,
    BCM2835_MAIL0_UNKNOWN           = 7,
    BCM2835_MAIL0_ARM_VC            = 8,
    BCM2835_MAIL0_VC_ARM            = 9
};

void mailboxWrite(int data_addr, int channel);
int mailboxRead(int channel);

#endif
