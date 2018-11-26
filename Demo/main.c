//main.c
//authored by Francesco Dell'Orso
//

#include "stufa_task.h"

#include <rpi_header.h>

#include <FreeRTOS.h>
#include <task.h>


extern xTaskHandle xHandleWDog;
extern xTaskHandle xHandleUSPi;
extern xTaskHandle xHandleTicCtrl;
extern xTaskHandle xHandleTicCnsl;


int main(void) {

	DisableInterrupts();
	InitInterruptController();

	if(xTaskCreate(prvTask_WatchDog, (signed char *) "WatchDog",
		8 * configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1,
		&xHandleWDog) == pdPASS) {
		if(uxTaskPriorityGet(xHandleWDog) < configMAX_CO_ROUTINE_PRIORITIES) {
			vTaskSuspend(xHandleWDog);
		}
	}

	if(xTaskCreate(prvTask_UspiInit, (signed char *) "UspiInit",
		8 * configMINIMAL_STACK_SIZE, NULL, 0, &xHandleUSPi) == pdPASS) {
		if(uxTaskPriorityGet(xHandleUSPi) < configMAX_CO_ROUTINE_PRIORITIES) {
			vTaskSuspend(xHandleUSPi);
		}
	}

	if(xTaskCreate(prvTask_TicControl, (signed char *) "TicControl",
		8 * configMINIMAL_STACK_SIZE, NULL, 0, &xHandleTicCtrl) == pdPASS) {
		if(uxTaskPriorityGet(xHandleTicCtrl) < configMAX_CO_ROUTINE_PRIORITIES) {
			vTaskSuspend(xHandleTicCtrl);
		}
	}

	if(xTaskCreate(prvTask_TicConsole, (signed char *) "TicConsole",
		8 * configMINIMAL_STACK_SIZE, NULL, 0, &xHandleTicCnsl) == pdPASS) {
		if(uxTaskPriorityGet(xHandleTicCnsl) < configMAX_CO_ROUTINE_PRIORITIES) {
			vTaskSuspend(xHandleTicCnsl);
		}
	}

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}
