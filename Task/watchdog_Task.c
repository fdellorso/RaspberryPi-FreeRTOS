// watchdog_Task.c
// authored by Francesco Dell'Orso
//
// tasks for StuFA Project

#include "stufa_Task.h"

// Tasks
extern TaskHandle_t			xHandleUSPi;
extern TaskHandle_t			xHandleTicCtrl;
extern TaskHandle_t			xHandleTicCnsl;
extern TaskHandle_t			xHandle8825Ctrl;
extern TaskHandle_t			xHandleBltInit;

// Semaphores
extern SemaphoreHandle_t	xSemUSPiInit;
extern SemaphoreHandle_t	xSemTicInit;

// Queues
extern QueueHandle_t		xQueTicHdl;

// Mutexs
extern const TickType_t 	xBlockTime;
extern SemaphoreHandle_t	xMutexMuart;
extern SemaphoreHandle_t	xMutexTicVar;
extern SemaphoreHandle_t	xMutexEnergize;

// WatchDog boot other tasks and 
void prvTask_WatchDog(void *pParam) {
	int i = 0;

	/* Stop warnings. */
	( void ) pParam;

	tic_handle * ticHandle = NULL;

	static const portTickType xPeriod = 500 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();

	prvFunc_Print("\nWatchDog...\t\t\t     Started");

	vTaskResume(xHandleUSPi);
	// taskYIELD();

	while(1) {
		i++;

		vTaskDelayUntil(&xLastWakeTime, xPeriod);

		// WhatcDog Activity Led
		SetGpio(47, i%2);

		// IF Tic is energized send Reset Timeout
		if(ticHandle != NULL && xMutexEnergize != NULL) {
			if(xSemaphoreTake(xMutexTicVar, xBlockTime) == pdPASS) {
				if(xSemaphoreTake(xMutexEnergize, xBlockTime) == pdPASS) {
					tic_reset_command_timeout(ticHandle);
					xSemaphoreGive(xMutexEnergize);
				}

				xSemaphoreGive(xMutexTicVar);
			}
		}

		// IF USPi is initialized start TicControl
		// if(xTaskIsTaskSuspended(xHandleUSPi) == pdFALSE && xSemUSPiInit != NULL) {
		if(eTaskGetState(xHandleUSPi) == eBlocked && xSemUSPiInit != NULL) {
			if(xSemaphoreTake(xSemUSPiInit, xBlockTime) == pdPASS) {
				vSemaphoreDelete(xSemUSPiInit);
				xSemUSPiInit = NULL;
				vTaskSuspend(xHandleUSPi);

				if(USPiTicAvailable()) {
					vTaskResume(xHandleTicCtrl);
					// taskYIELD();
				}

				if(0) {
					vTaskResume(xHandle8825Ctrl);
					// taskYIELD();
				}

				if(USPiBltAvailable()) {
					vTaskResume(xHandleBltInit);
					// taskYIELD();
				}
			}
		}

		// Receiving TicHandle to manage Tic Command Timeout
		if(xQueTicHdl != NULL && ticHandle == NULL) {
			if(xQueueReceive(xQueTicHdl, &ticHandle, xBlockTime) == pdPASS) {
				vQueueDelete(xQueTicHdl);
				prvFunc_Print("\nWatchDog...\n%cTicHandle...\t\t\t    Received", 0x3e);
			}
		}

		// IF TicControl is initialized start TicConsole
		// if(xTaskIsTaskSuspended(xHandleTicCnsl) == pdTRUE && xSemTicInit != NULL) {
		if(eTaskGetState(xHandleTicCnsl) == eBlocked && xSemTicInit != NULL) {
			if(xSemaphoreTake(xSemTicInit, xBlockTime) == pdPASS) {
				vSemaphoreDelete(xSemTicInit);
				vTaskResume(xHandleTicCnsl);
			}
		}
	}
}
