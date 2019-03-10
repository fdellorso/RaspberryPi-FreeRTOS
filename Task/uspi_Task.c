// uspi_Task.c
// authored by Francesco Dell'Orso
//
// tasks for StuFA Project

#include "stufa_Task.h"

extern xSemaphoreHandle	xSemUSPiInit;

void prvTask_UspiInitialize(void *pParam) {
	int i = 0;
	// int uspiInited = 0;

	/* Stop warnings. */
	( void ) pParam;

	// vSemaphoreCreateBinary(xSemUSPiInit);
	if(xSemUSPiInit != NULL) xSemaphoreTake(xSemUSPiInit, 0);
	
	prvFunc_Print("\nUSPi Initialize...\t\t     Started");
	prvFunc_Print("--------------------------------------------");

	if(USPiInitialize()) {
		// Give Semaphore back when USPi is initialized
		if(xSemaphoreGive(xSemUSPiInit) == pdPASS) {
			prvFunc_Print("--------------------------------------------");
			prvFunc_Print("USPi Initialize...\t\t    Finished");
		}
	}

	while(1) {
		i++;

		// while(uspiInited == 0) {
		// 	uspiInited = USPiInitialize();
		// }

		// if(uspiInited == 1) {
		// 	// Give Semaphore back when USPi is initialized
		// 	if(xSemaphoreGive(xSemUSPiInit) == pdPASS) {
		// 		uspiInited = 2;
		// 		prvFunc_Print("--------------------------------------------");
		// 		prvFunc_Print("USPi Initialize...\t\t    Finished");
		// 	}
		// }

		// TODO Close USPi

		vTaskDelay(100);
	}
}
