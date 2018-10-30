//main.c
//authored by Jared Hull
//
//tasks 1 and 2 blink the ACT LED
//main initialises the devices and IP tasks

#include <FreeRTOS.h>
#include <task.h>

#include "bcm2835_base.h"
#include "gpio.h"
#include "interrupts.h"
#include "stufa_task.h"

#ifdef VIDEO
	#include "video.h"
#endif
#ifdef ILI9340
	#include "ili9340.h"
#endif
#ifdef MUART
	#include "muart.h"
#endif

#include <bcm2835.h>

extern xTaskHandle xHandleUSPi;

int main(void) {
	bcm2835_init();

	SetGpioFunction(47, 1);			// RDY led
	SetGpio(47, 1);

	
	// ili9340_println("StuFA",ILI9340_RED);

	#ifdef VIDEO
		// Inizializzazione Video per Debug
		initFB(1680, 1050);
	#endif
	#ifdef ILI9340
		ili9340_init();
		ili9340_set_rotation(1);
	#endif
	#ifdef MUART
		muart_init();
	#endif

	DisableInterrupts();
	InitInterruptController();

	xTaskCreate(prvTask_WatchDog, "WatchDog", 128, NULL, configMAX_PRIORITIES - 1, NULL);
	xTaskCreate(prvTask_UspiInit, "UspiInit", 1024, NULL, 0, &xHandleUSPi);
	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}
