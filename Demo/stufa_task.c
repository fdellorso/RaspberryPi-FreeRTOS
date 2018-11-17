//main.c
//authored by Jared Hull
//
//tasks 1 and 2 blink the ACT LED
//main initialises the devices and IP tasks

#include "stufa_task.h"

#include <rpi_header.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include <uspi.h>
#include <tic.h>


xTaskHandle xHandleWDog;
xTaskHandle xHandleUSPi;
xTaskHandle xHandleTicCtrl;
xTaskHandle xHandleTicCnsl;

xSemaphoreHandle xSemUSPiInit = NULL;
xSemaphoreHandle xSemTicInit = NULL;
xSemaphoreHandle xSemTicEner = NULL;
// xQueueHandle xQueTicHndl = NULL;
// xQueueHandle xQueTicComm = NULL;


void prvTask_WatchDog(void *pParam) {
	int i = 0;
	uint32_t ticEnergized = 0;

	/* Stop warnings. */
	( void ) pParam;

	static const portTickType xPeriod = 500 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();

	static const portTickType xBlockTime = 100 / portTICK_RATE_MS;

	muart_printf("WatchDog...\t\t\t     Started");

	#ifdef VIDEO
		// Inizializzazione Video per Debug
		initFB(1680, 1050);
	#endif
	#ifdef ILI9340
		bcm2835_init();
		ili9340_init();
		ili9340_set_rotation(1);
	#endif
	#ifdef MUART
		muart_init();
	#endif

	vTaskResume(xHandleUSPi);

	while(1) {
		i++;

		vTaskDelayUntil(&xLastWakeTime, xPeriod);

		// WhatcDog Activity Led
		SetGpio(47, i%2);
		// if(i%2) muart_puts("W\r");
		// else muart_puts(" \r");

		// IF Tic is energized send Reset Timeout
		if(ticEnergized) {
			// USPiTicQuick(TIC_CMD_RESET_COMMAND_TIMEOUT);
			// tic_reset_command_timeout(ticHandle);
		}

		// IF USPi is initialized start TicControl
		if(xTaskIsTaskSuspended(xHandleUSPi) == pdFALSE) {
			if(xSemaphoreTake(xSemUSPiInit, xBlockTime) == pdPASS) {
				vTaskSuspend(xHandleUSPi);
				vTaskResume(xHandleTicCtrl);
			}
		}

		// IF TicControl is initialized start TicConsole	// FIXME
		// if(xTaskIsTaskSuspended(xHandleTicCnsl) == pdTRUE) {
		// 	if(xSemaphoreTake(xSemTicInit, xBlockTime) == pdPASS) {
		// 		vTaskResume(xHandleTicCnsl);
		// 	}
		// }

		// if(uspiInited == 0) {
		// 	if(xSemaphoreTake(xSemUSPiInit, xBlockTime) == pdPASS) {
		// 		uspiInited = 1;
		// 		vTaskSuspend(xHandleUSPi);
		// 		vSemaphoreDelete(xSemUSPiInit);
		// 	}
		// }
		
		// if(uspiInited == 1) {
		// 	uspiInited = 2;
		// 	vTaskResume(xHandleTicCtrl);
		// }

		

		// if(xTaskIsTaskSuspended(xHandleUSPi) == pdFALSE) {
		// if(xQueueUSPiInit != NULL) {
		// 	if(xQueueReceive(xQueueUSPiInit, &uspiInited, xBlockTime) == pdPASS) {
		// 		vTaskSuspend(xHandleUSPi);
		// 		vQueueDelete(xQueueUSPiInit);
		// 	}
		// }

		// if(xQueueTicInit != NULL) {
		// 	if(xQueueReceive(xQueueTicInit, &ticInited, xBlockTime) == pdPASS) {
		// 		vQueueDelete(xQueueTicInit);
		// 	}
		// }
	}
}

void prvTask_UspiInit(void *pParam) {
	int i = 0;
	uint32_t uspiInited = 0;

	/* Stop warnings. */
	( void ) pParam;

	vSemaphoreCreateBinary(xSemUSPiInit);
	if(xSemUSPiInit != NULL) xSemaphoreTake(xSemUSPiInit, 0);
	
	muart_printf("USPi Initialize...\t\t     Started");
	muart_println("--------------------------------------------");

	while(1) {
		i++;

		while(uspiInited == 0) {
			uspiInited = (uint32_t) USPiInitialize();
		}

		// TODO Send message Queue
		if(uspiInited == 1) {
			if(xSemaphoreGive(xSemUSPiInit) == pdPASS) {
				uspiInited = 2;
				muart_println("--------------------------------------------");
				muart_printf("USPi Initialize...\t\t    Finished");
			}
		}

		// TODO Close USPi
	}
}

void prvTask_TicControl(void *pParam) {
	int i = 0;
	// uint32_t ticEnergized = 0;

	/* Stop warnings. */
	( void ) pParam;

	tic_error * error = NULL;

	tic_device * ticDevice;
	tic_handle * ticHandle;
	tic_variables * ticVariables;
	tic_settings * ticSettings;

	vSemaphoreCreateBinary(xSemTicInit);
	if(xSemTicInit != NULL) xSemaphoreTake(xSemTicInit, 0);

	muart_printf("Tic Control...\t\t\t     Started");

	if (error == NULL) error = tic_device_create(&ticDevice);
	if (error == NULL) error = tic_handle_create(ticDevice, &ticHandle);
	if (error == NULL) error = tic_get_variables(ticHandle, &ticVariables, false);
	if (error == NULL) error = tic_get_settings(ticHandle, &ticSettings);

	// muart_printf("%s", tic_get_firmware_version_string(ticHandle));	// FIXME

	if(xSemaphoreGive(xSemTicInit) == pdPASS) {
		;
	}

	while(1) {
		i++;

		// tic_set_target_position(ticHandle, 0x10000000);
		// tic_exit_safe_start(ticHandle);
		// tic_energize(ticHandle);
	}
}

void prvTask_TicConsole(void *pParam) {
	int i = 0;

	/* Stop warnings. */
	( void ) pParam;

	static const portTickType xBlockTime = 100 / portTICK_RATE_MS;

	tic_variables * ticVariables;
	tic_settings * ticSettings;

	muart_printf("Tic Console...\t\t\t     Started");

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
