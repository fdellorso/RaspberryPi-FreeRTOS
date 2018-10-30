//main.c
//authored by Jared Hull
//
//tasks 1 and 2 blink the ACT LED
//main initialises the devices and IP tasks

#include <FreeRTOS.h>
#include <task.h>
#include <uspi.h>

#include "bcm2835_base.h"
#include "gpio.h"
#include "interrupts.h"

#ifdef VIDEO
	#include "video.h"
#endif
#ifdef ILI9340
	#include "ili9340.h"
#endif
#ifdef MUART
	#include "muart.h"
#endif

xTaskHandle xHandleUSPi;

void prvTask_WatchDog() {
	int i = 0;

	static const portTickType xPeriod = 500 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();

	muart_println("WatchDog");

	while(1) {
		i++;
		vTaskDelayUntil(&xLastWakeTime, xPeriod);

		// WhatcDog Activity Led
		SetGpio(47, i%2);
		// muart_puts("Nicoleta Amore Mio\r\n");
		
		// Tic834 Reset Command Timeout
		// USPiTicQuick(TIC_CMD_RESET_COMMAND_TIMEOUT);
	}
}

void prvTask_UspiInit() {
	int i = 0;

	int uspiInit = 0;
	
	muart_println("USPi Initialize");

	while(1) {
		i++;
		while(uspiInit == 0) uspiInit = USPiInitialize();
		vTaskDelete(xHandleUSPi);

		// xTaskCreate(task4, "TIC", 128, NULL, 0, NULL);
	}
}

void prvTask_Tic834Menu() {
	int i = 0;

	while(1) {
		i++;


	}
}

// void task4() {
// 	int i = 0;
// 	int result = 0;
// 	static const portTickType xDelayTime = 200 / portTICK_RATE_MS;

// 	uint32_t curr_position;

// 	ili9340_println("TIC TASK", ILI9340_WHITE);
	
// 	USPiTicBlockRead(TIC_CMD_GET_VARIABLE, TIC_VAR_CURRENT_POSITION, 0x04, &curr_position);
// 	ili9340_printHex("Get Current Position: ", curr_position, ILI9340_RED);

// 	USPiTicBlockRead(TIC_CMD_GET_VARIABLE, TIC_VAR_ERROR_STATUS, 0x02, &curr_position);
// 	ili9340_printHex("Get Error Status: ", curr_position, ILI9340_RED);
	
// 	result = USPiTicQuick(TIC_CMD_EXIT_SAFE_START);
// 	ili9340_printHex("Exit Safe Start Result: ", result, ILI9340_RED);
// 	vTaskDelay(xDelayTime);

// 	result = USPiTicQuick(TIC_CMD_ENERGIZE);
// 	ili9340_printHex("Energize Result: ", result, ILI9340_RED);

// 	result = USPiTic32BitWrite(TIC_CMD_SET_TARGET_POSITION, 0xFFFF, 0x7FFF);
// 	ili9340_printHex("Set Target Position Result: ", result, ILI9340_RED);

// 	while(1) {
// 		i++;
// 		USPiTicBlockRead(TIC_CMD_GET_VARIABLE, TIC_VAR_CURRENT_POSITION, 0x04, &curr_position);
// 		ili9340_printHex("Get Current Position: ", curr_position, ILI9340_RED);

// 		// ili9340_printHex("Reset Command Timeout Result: ", result, ILI9340_RED);

// 		// USPiTicReadBlock(0xA1, 0, 0x02, 0x02, &curr_position);
// 		// ili9340_printHex("Get Error Status: ", curr_position, ILI9340_RED);

// 		vTaskDelay(200);
// 	}
// }
