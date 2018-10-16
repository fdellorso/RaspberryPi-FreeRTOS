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
xTaskHandle xHandleUSPi;

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

void task4() {
	int i = 0;
	int result = 0;
	static const portTickType xDelayTime = 200 / portTICK_RATE_MS;

	uint32_t curr_position;

	ili9340_println("TIC TASK", ILI9340_WHITE);
	
	USPiTicBlockRead(TIC_CMD_GET_VARIABLE, TIC_VAR_CURRENT_POSITION, 0x04, &curr_position);
	ili9340_printHex("Get Current Position: ", curr_position, ILI9340_RED);

	USPiTicBlockRead(TIC_CMD_GET_VARIABLE, TIC_VAR_ERROR_STATUS, 0x02, &curr_position);
	ili9340_printHex("Get Error Status: ", curr_position, ILI9340_RED);
	
	result = USPiTicQuick(TIC_CMD_EXIT_SAFE_START);
	ili9340_printHex("Exit Safe Start Result: ", result, ILI9340_RED);
	vTaskDelay(xDelayTime);

	result = USPiTicQuick(TIC_CMD_ENERGIZE);
	ili9340_printHex("Energize Result: ", result, ILI9340_RED);

	result = USPiTic32BitWrite(TIC_CMD_SET_TARGET_POSITION, 0xFFFF, 0x7FFF);
	ili9340_printHex("Set Target Position Result: ", result, ILI9340_RED);

	while(1) {
		i++;
		USPiTicBlockRead(TIC_CMD_GET_VARIABLE, TIC_VAR_CURRENT_POSITION, 0x04, &curr_position);
		ili9340_printHex("Get Current Position: ", curr_position, ILI9340_RED);

		result = USPiTicQuick(TIC_CMD_RESET_COMMAND_TIMEOUT);
		// ili9340_printHex("Reset Command Timeout Result: ", result, ILI9340_RED);

		// USPiTicReadBlock(0xA1, 0, 0x02, 0x02, &curr_position);
		// ili9340_printHex("Get Error Status: ", curr_position, ILI9340_RED);

		vTaskDelay(200);
	}
}

void task3() {
	int uspiInit = 0;
	int i = 0;
	ili9340_println("USPi Initialize", ILI9340_WHITE);
	while(1) {
		i++;
		while(uspiInit == 0) uspiInit = USPiInitialize();
		xTaskCreate(task4, "TIC", 128, NULL, 0, NULL);
		vTaskDelete(xHandleUSPi);
	}
}

int main(void) {
	bcm2835_init();

	SetGpioFunction(47, 1);			// RDY led
	SetGpio(47, 1);

	// Inizializzazione Video per Debug
	initFB(1680, 1050);
	ili9340_init();
	ili9340_set_rotation(1);

	// videotest();

	DisableInterrupts();
	InitInterruptController();

	xTaskCreate(task1, "LED_0", 128, NULL, 0, NULL);
	xTaskCreate(task2, "LED_1", 128, NULL, 0, NULL);
	xTaskCreate(task3, "USPI", 1024, NULL, 1, &xHandleUSPi);


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
