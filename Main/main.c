//main.c
//authored by Francesco Dell'Orso
//

// TODO Resolve DRB8825 Interrupt
// TODO Conversion loops -> pulses
// TODO Tic Home Routine

// TODO Update FreeRTOS 10.0
// TODO Update USPi 2.00
// TODO Understand if Uart FIFO Flush works
// TODO Investigate Data Memory Barrier
// TODO Investigate benefit of Cache
// TODO Investigate benefit of Newlib implementation (Syscalls.c)

#include "stufa_Task.h"

#include <rpi_header.h>
#include <rpi_logger.h>
#include <prvlib/null.h>


// Tasks
xTaskHandle			xHandleWDog		= NULL;
xTaskHandle			xHandleUSPi		= NULL;
xTaskHandle			xHandleTicCtrl	= NULL;
xTaskHandle			xHandleTicCnsl	= NULL;
xTaskHandle			xHandle8825Ctrl	= NULL;
xTaskHandle			xHandleBltInit	= NULL;
xTaskHandle			xHandleBltProc	= NULL;

// Semaphores
xSemaphoreHandle	xSemUSPiInit	= NULL;
xSemaphoreHandle	xSemTicInit		= NULL;

// Mutexs
xSemaphoreHandle	xMutexMuart		= NULL;
xSemaphoreHandle	xMutexTicVar	= NULL;
xSemaphoreHandle	xMutexEnergize	= NULL;
xSemaphoreHandle	xMutexRunning	= NULL;

// Queues
xQueueHandle		xQueTicHdl		= NULL;
xQueueHandle		xQueTicVar		= NULL;
xQueueHandle		xQueTicSet		= NULL;
xQueueHandle		xQueTicCmd		= NULL;
xQueueHandle		xQueBltProc		= NULL;

const portTickType xBlockTime = 100 / portTICK_RATE_MS;


int main(void) {

	DisableInterrupts();
	InitInterruptController();

	logger_init();
	#ifdef ILI9340
		bcm2835_init();
		ili9340_set_rotation(1);
	#endif

	vSemaphoreCreateBinary(xSemUSPiInit);
	vSemaphoreCreateBinary(xSemTicInit);

	xMutexMuart = xSemaphoreCreateMutex();
	xMutexTicVar = xSemaphoreCreateMutex();
	xMutexEnergize = xSemaphoreCreateMutex();
	xMutexRunning = xSemaphoreCreateMutex();

	xQueTicHdl = xQueueCreate(1, sizeof(tic_handle *));
	xQueTicVar = xQueueCreate(1, sizeof(tic_variables *));
	xQueTicSet = xQueueCreate(1, sizeof(tic_settings *));
	xQueTicCmd = xQueueCreate(1, sizeof(tic_command *));
	xQueBltProc = xQueueCreate(1, sizeof(TBTSubSystem *));


	if(xTaskCreate(prvTask_WatchDog, (signed char *) "WatchDog",
		configMINIMAL_STACK_SIZE, NULL, 1, &xHandleWDog) == pdPASS) {
		// if(uxTaskPriorityGet(xHandleWDog) < configMAX_CO_ROUTINE_PRIORITIES) {
		// 	vTaskSuspend(xHandleWDog);
		// }
	}

	if(xTaskCreate(prvTask_UspiInitialize, (signed char *) "UspiInit",
		2 * configMINIMAL_STACK_SIZE, NULL, 0, &xHandleUSPi) == pdPASS) {
		if(uxTaskPriorityGet(xHandleUSPi) < configMAX_CO_ROUTINE_PRIORITIES) {
			vTaskSuspend(xHandleUSPi);
		}
	}

	// if(xTaskCreate(prvTask_TicControl, (signed char *) "TicControl",
	// 	configMINIMAL_STACK_SIZE, NULL, configMAX_CO_ROUTINE_PRIORITIES,
	// 	&xHandleTicCtrl) == pdPASS) {
	// 	if(uxTaskPriorityGet(xHandleTicCtrl) <= configMAX_CO_ROUTINE_PRIORITIES) {
	// 		vTaskSuspend(xHandleTicCtrl);
	// 	}
	// }

	// if(xTaskCreate(prvTask_TicConsole, (signed char *) "TicConsole",
	// 	configMINIMAL_STACK_SIZE, NULL, 0, &xHandleTicCnsl) == pdPASS) {
	// 	if(uxTaskPriorityGet(xHandleTicCnsl) <= configMAX_CO_ROUTINE_PRIORITIES) {
	// 		vTaskSuspend(xHandleTicCnsl);
	// 	}
	// }

	// if(xTaskCreate(prvTask_Drv8825Control, (signed char *) "8825Control",
	// 	configMINIMAL_STACK_SIZE, NULL, 0, &xHandle8825Ctrl) == pdPASS) {
	// 	if(uxTaskPriorityGet(xHandle8825Ctrl) <= configMAX_CO_ROUTINE_PRIORITIES) {
	// 		vTaskSuspend(xHandle8825Ctrl);
	// 	}
	// }

	if(xTaskCreate(prvTask_BluetoothInitialize, (signed char *) "BluetoothInitialize",
		configMINIMAL_STACK_SIZE, NULL, 0, &xHandleBltInit) == pdPASS) {
		if(uxTaskPriorityGet(xHandleBltInit) < configMAX_CO_ROUTINE_PRIORITIES) {
			vTaskSuspend(xHandleBltInit);
		}
	}

	if(xTaskCreate(prvTask_BluetoothProcess, (signed char *) "BluetoothProcess",
		configMINIMAL_STACK_SIZE, NULL, 0, &xHandleBltProc) == pdPASS) {
		if(uxTaskPriorityGet(xHandleBltProc) < configMAX_CO_ROUTINE_PRIORITIES) {
			vTaskSuspend(xHandleBltProc);
		}
	}

	// if(USPiInitialize()) {
	// 	// Give Semaphore back when USPi is initialized
	// 	printf("--------------------------------------------");
	// 	printf("USPi Initialize...\t\t    Finished");
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
