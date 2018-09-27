//main.c
//authored by Jared Hull
//
//tasks 1 and 2 blink the ACT LED
//main initialises the devices and IP tasks

#include <FreeRTOS.h>
#include <task.h>

#include "bcm2835_base.h"
#include "interrupts.h"
#include "gpio.h"
#include "video.h"
#include "ili9340.h"
#include <bcm2835.h>

// #include "FreeRTOS_IP.h"
// #include "FreeRTOS_Sockets.h"

#include <uspi.h>

//Only for debug, normally should not 
//include private header
// #include "FreeRTOS_IP_Private.h"

uint16_t width, height;

void task1() {
	int i = 0;
	while(1) {
		i++;
		SetGpio(47, 1);
		vTaskDelay(200);
	}
}

void task2() {
	int i = 0;
	while(1) {
		i++;
		vTaskDelay(100);
		SetGpio(47, 0);
		vTaskDelay(100);
	}
}

int main(void) {
	bcm2835_init();

	SetGpioFunction(47, 1);			// RDY led
	SetGpio(47, 1);

	// Inizializzazione Video per Debug
	// initFB();
	ili9340_init();
	ili9340_println("StuFA", ILI9340_WHITE);

	USPiInitialize();

	//videotest();

	// DisableInterrupts();
	// InitInterruptController();

	xTaskCreate(task1, "LED_0", 128, NULL, 0, NULL);
	xTaskCreate(task2, "LED_1", 128, NULL, 0, NULL);

	//set to 0 for no debug, 1 for debug, or 2 for GCC instrumentation (if enabled in config)
	loaded = 1;

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}
