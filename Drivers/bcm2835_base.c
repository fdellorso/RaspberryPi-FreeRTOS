/**
 *	Quick and very Dirty BCM2835 BASE API.
 *
 **/

#include "bcm2835_base.h"

extern void dummy(unsigned int);

/**
 * Wait N CPU cycles (ARM CPU only)
 */
void wait_cycles(uint8_t n) {
    // if(n) while(n--) { asm volatile("nop"); }
	if(n) while(n--) { dummy(n); }
}
