//main.c
//authored by Francesco Dell'Orso
//

#include "stufa_task.h"

#include <rpi_header.h>
#include <rpi_logger.h>

#include <FreeRTOS.h>
#include <task.h>

#include <uspi.h>


extern xTaskHandle xHandleWDog;
extern xTaskHandle xHandleUSPi;
extern xTaskHandle xHandleTicCtrl;
extern xTaskHandle xHandleTicCnsl;
extern xTaskHandle xHandle8825Ctrl;
extern xTaskHandle xHandleBluetooth;

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
		8 * configMINIMAL_STACK_SIZE, NULL, configMAX_CO_ROUTINE_PRIORITIES -1,
		&xHandleUSPi) == pdPASS) {
		if(uxTaskPriorityGet(xHandleUSPi) < configMAX_CO_ROUTINE_PRIORITIES) {
			vTaskSuspend(xHandleUSPi);
		}
	}

	if(xTaskCreate(prvTask_TicControl, (signed char *) "TicControl",
		8 * configMINIMAL_STACK_SIZE, NULL, configMAX_CO_ROUTINE_PRIORITIES -1,
		&xHandleTicCtrl) == pdPASS) {
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

	if(xTaskCreate(prvTask_8825Control, (signed char *) "8825Control",
		configMINIMAL_STACK_SIZE, NULL, 0, &xHandle8825Ctrl) == pdPASS) {
		if(uxTaskPriorityGet(xHandle8825Ctrl) < configMAX_CO_ROUTINE_PRIORITIES) {
			vTaskSuspend(xHandle8825Ctrl);
		}
	}

	if(xTaskCreate(prvTask_Bluetooth, (signed char *) "Bluetooth",
		8 * configMINIMAL_STACK_SIZE, NULL, 0, &xHandleBluetooth) == pdPASS) {
		if(uxTaskPriorityGet(xHandleBluetooth) < configMAX_CO_ROUTINE_PRIORITIES) {
			vTaskSuspend(xHandleBluetooth);
		}
	}

	// logger_init();
	// #ifdef ILI9340
	// 	bcm2835_init();
	// 	ili9340_set_rotation(1);
	// #endif

	// if(!USPiInitialize()) {
	// 	printf("USPi failed to initialize");
	// }

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}
