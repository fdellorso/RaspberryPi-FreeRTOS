//stufa_task.c
//authored by Francesco Dell'Orso
//
//tasks for StuFA Project

#include "stufa_task.h"

#include <rpi_header.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include <uspi.h>
#include <tic.h>


xTaskHandle			xHandleWDog;
xTaskHandle			xHandleUSPi;
xTaskHandle			xHandleTicCtrl;
xTaskHandle			xHandleTicCnsl;

xSemaphoreHandle	xSemUSPiInit	= NULL;
xSemaphoreHandle	xSemTicInit		= NULL;
xQueueHandle		xQueTicVar		= NULL;
xQueueHandle		xQueTicSet		= NULL;

// xSemaphoreHandle	xSemTicEner		= NULL;
// xQueueHandle		xQueTicHndl		= NULL;


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

		// IF Tic is energized send Reset Timeout
		if(ticEnergized) {
			// USPiTicQuick(TIC_CMD_RESET_COMMAND_TIMEOUT);
			// tic_reset_command_timeout(ticHandle);
		}

		// IF USPi is initialized start TicControl
		if(xTaskIsTaskSuspended(xHandleUSPi) == pdFALSE && xSemUSPiInit != NULL) {
			if(xSemaphoreTake(xSemUSPiInit, xBlockTime) == pdPASS) {
				vSemaphoreDelete(xSemUSPiInit);
				vTaskSuspend(xHandleUSPi);
				vTaskResume(xHandleTicCtrl);
			}
		}

		// IF TicControl is initialized start TicConsole
		if(xTaskIsTaskSuspended(xHandleTicCnsl) == pdTRUE && xSemTicInit != NULL) {
			if(xSemaphoreTake(xSemTicInit, xBlockTime) == pdPASS) {
				vSemaphoreDelete(xSemTicInit);
				vTaskResume(xHandleTicCnsl);
			}
		}
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
	uint32_t setSent = 0;
	uint32_t varSent = 0;
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

	tic_get_firmware_version_string(ticHandle);

	if(xSemaphoreGive(xSemTicInit) == pdPASS) {
		;
	}

	while(1) {
		i++;

		if(varSent == 0 && xQueTicVar != NULL) {
			if(xQueueSend(xQueTicVar, &ticVariables, 0) == pdPASS) {
				varSent = 1;
			}
		}

		if(setSent == 0 && xQueTicSet != NULL) {
			if(xQueueSend(xQueTicSet, &ticSettings, 0) == pdPASS) {
				setSent = 1;
			}
		}

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

	xQueTicVar = xQueueCreate(1, sizeof(tic_variables *));
	xQueTicSet = xQueueCreate(1, sizeof(tic_settings *));

	while(1) {
		i++;

		if(xQueTicVar != NULL) {
			if(xQueueReceive(xQueTicVar, &ticVariables, xBlockTime) == pdPASS) {
				vQueueDelete(xQueTicVar);
			}
		}

		if(xQueTicSet != NULL) {
			if(xQueueReceive(xQueTicSet, &ticSettings, xBlockTime) == pdPASS) {
				vQueueDelete(xQueTicSet);
			}
		}
	}
}
