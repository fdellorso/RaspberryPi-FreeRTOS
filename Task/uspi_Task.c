// uspi_Task.c
// authored by Francesco Dell'Orso
//
// tasks for StuFA Project

#include "stufa_Task.h"

extern SemaphoreHandle_t	xSemUSPiInit;
extern TimerHandle_t		xWatchDogTimer;
extern TaskHandle_t			xHandleUSPiInit;
extern TaskHandle_t			xHandleBLTiInit;

void prvTask_UspiInitialize(void *pParam) {
	int i = 0;

	/* Stop warnings. */
	( void ) pParam;

	// vSemaphoreCreateBinary(xSemUSPiInit);
	// if(xSemUSPiInit != NULL) xSemaphoreTake(xSemUSPiInit, 0);
	
	xTimerChangePeriod( xWatchDogTimer, initWATCHDOG_TIMER_PERIOD, 0 );

	prvFunc_Print("\nUSPi Initialize...\t\t     Started");
	prvFunc_Print("--------------------------------------------");

	if(USPiInitialize()) {
		// Give Semaphore back when USPi is initialized
		// if(xSemaphoreGive(xSemUSPiInit) == pdPASS)
		{
			prvFunc_Print("--------------------------------------------");
			prvFunc_Print("USPi Initialize...\t\t    Finished");

			xTimerChangePeriod( xWatchDogTimer, mainWATCHDOG_TIMER_PERIOD, 0 );

			vTaskResume(xHandleBLTiInit);
			vTaskSuspend(xHandleUSPiInit);
		}
	}

	while(1) {
		i++;

		taskYIELD();
	}
}
