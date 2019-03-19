/**
 *	Quick and very Dirty BCM2835 BASE API.
 *
 **/

#include "bcm2835_base.h"

extern void NOOP(unsigned int);

/**
 * Wait N CPU cycles (ARM CPU only)
 */
void wait_cycles(unsigned char n) {
    // if(n) while(n--) { asm volatile("nop"); }
	if(n) while(n--) { NOOP(n); }
}
