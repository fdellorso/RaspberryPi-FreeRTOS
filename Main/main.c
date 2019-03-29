//main.c
//authored by Francesco Dell'Orso
//

// TODO Implement resource control
// TODO Resolve DRB8825 Interrupt
// TODO Conversion loops -> pulses
// TODO Tic Home Routine
// TODO Understand if Uart FIFO Flush works
// TODO Investigate Data Memory Barrier
// TODO Investigate benefit of Cache
// TODO Investigate benefit of Newlib implementation (Syscalls.c)

#include <stufa_Task.h>

#include <rpi_header.h>
#include <rpi_logger.h>
#include <prvlib/null.h>

// #include <uspi/dwhcidevice.h>

// Semaphores
SemaphoreHandle_t	xSemUSPiInit	= NULL;
SemaphoreHandle_t	xSemTicInit		= NULL;

// Mutexs
const TickType_t xBlockTime = pdMS_TO_TICKS( 20 );
SemaphoreHandle_t	xMutexMuart		= NULL;
SemaphoreHandle_t	xMutexTicVar	= NULL;
SemaphoreHandle_t	xMutexEnergize	= NULL;
SemaphoreHandle_t	xMutexRunning	= NULL;

// Queues
QueueHandle_t		xQuePrintPool	= NULL;
QueueHandle_t		xQueTicHdl		= NULL;
QueueHandle_t		xQueTicVar		= NULL;
QueueHandle_t		xQueTicSet		= NULL;
QueueHandle_t		xQueTicCmd		= NULL;
QueueHandle_t		xQueueBLTiProc	= NULL;

// Event Groups
EventGroupHandle_t	xEventGroup		= NULL;

// Timers
TimerHandle_t		xWatchDogTimer	= NULL;
TimerHandle_t		xTicResetTimer	= NULL;
// TimerHandle_t		xDWHCITimer		= NULL;


// Tasks
TaskHandle_t		xHandlePrintPool= NULL;
TaskHandle_t		xHandleWDog		= NULL;
TaskHandle_t		xHandleUSPiInit	= NULL;
TaskHandle_t		xHandleTIC1Ctrl	= NULL;
TaskHandle_t		xHandleTIC1Cnsl	= NULL;
TaskHandle_t		xHandle8825Ctrl	= NULL;
TaskHandle_t		xHandleBLTiInit	= NULL;
TaskHandle_t		xHandleBLTiProc	= NULL;

static uint8_t ucWatchDogCounter = 0;
static void prvWatchDogTimerCallback( TimerHandle_t xTimer );
static void prvTicResetTimerCallback( TimerHandle_t xTimer );
// static void prvDWHCIDeviceTimerCallback( TimerHandle_t xTimer );

int main(void) {

	DelayMilliSysTimer(100);

	SetGpio(41, 1);
	SetGpio(45, 1);

	DisableInterrupts();
	InitInterruptController();

	logger_init();
	#ifdef ILI9340
		ili9340_set_rotation(1);
	#endif

	println("StuFA Start");

	vSemaphoreCreateBinary(xSemUSPiInit);
	vSemaphoreCreateBinary(xSemTicInit);

	xMutexMuart = xSemaphoreCreateMutex();
	xMutexTicVar = xSemaphoreCreateMutex();
	xMutexEnergize = xSemaphoreCreateMutex();
	xMutexRunning = xSemaphoreCreateMutex();

	xQuePrintPool = xQueueCreate(30, sizeof(char *));
	xQueTicHdl = xQueueCreate(1, sizeof(tic_handle *));
	xQueTicVar = xQueueCreate(1, sizeof(tic_variables *));
	xQueTicSet = xQueueCreate(1, sizeof(tic_settings *));
	xQueTicCmd = xQueueCreate(1, sizeof(tic_command *));
	xQueueBLTiProc = xQueueCreate(1, sizeof(TBTSubSystem *));

	xEventGroup = xEventGroupCreate();

	xWatchDogTimer = xTimerCreate( (const char *) "LedTimer",
						mainWATCHDOG_TIMER_PERIOD, pdTRUE,
						0, prvWatchDogTimerCallback );

	xTicResetTimer = xTimerCreate( (const char *) "TicReset",
						TICRESET_TIMER_PERIOD, pdTRUE,
						0, prvTicResetTimerCallback );
	
	// xDWHCITimer    = xTimerCreate( (const char *) "DWHCITimer",
	// 					initWATCHDOG_TIMER_PERIOD, pdFALSE,
	// 					0, prvDWHCIDeviceTimerCallback );

	// if(xTaskCreate(prvTask_WatchDog, (const char *) "WatchDog",
	// 	configMINIMAL_STACK_SIZE, NULL, 1, &xHandleWDog) == pdPASS) {
	// 	if(uxTaskPriorityGet(xHandleWDog) < configMAX_CO_ROUTINE_PRIORITIES) {
	// 		vTaskSuspend(xHandleWDog);
	// 	}
	// }

	// if(xTaskCreate(prvTask_PrintPool, (const char *) "PrintPool",
	// 	configMINIMAL_STACK_SIZE, NULL, 1, &xHandlePrintPool) == pdPASS) {
	// 	// if(uxTaskPriorityGet(xHandleUSPi) < configMAX_CO_ROUTINE_PRIORITIES)
	// 	// {
	// 		// vTaskSuspend(xHandleUSPi);
	// 	// }
	// }

	if(xTaskCreate(prvTask_UspiInitialize, (const char *) "UspiInit",
		configMINIMAL_STACK_SIZE, NULL, 1, &xHandleUSPiInit) == pdPASS) {
		// if(uxTaskPriorityGet(xHandleUSPiInit) < configMAX_CO_ROUTINE_PRIORITIES)
		// {
		// 	vTaskSuspend(xHandleUSPiInit);
		// }
	}

	// if(xTaskCreate(prvTask_TicControl, (const char *) "TicControl",
	// 	configMINIMAL_STACK_SIZE, NULL, configMAX_CO_ROUTINE_PRIORITIES,
	// 	&xHandleTicCtrl) == pdPASS) {
	// 	if(uxTaskPriorityGet(xHandleTicCtrl) <= configMAX_CO_ROUTINE_PRIORITIES) {
	// 		vTaskSuspend(xHandleTicCtrl);
	// 	}
	// }

	// if(xTaskCreate(prvTask_TicConsole, (const char *) "TicConsole",
	// 	configMINIMAL_STACK_SIZE, NULL, 0, &xHandleTicCnsl) == pdPASS) {
	// 	if(uxTaskPriorityGet(xHandleTicCnsl) <= configMAX_CO_ROUTINE_PRIORITIES) {
	// 		vTaskSuspend(xHandleTicCnsl);
	// 	}
	// }

	// if(xTaskCreate(prvTask_Drv8825Control, (const char *) "8825Control",
	// 	configMINIMAL_STACK_SIZE, NULL, 0, &xHandle8825Ctrl) == pdPASS) {
	// 	if(uxTaskPriorityGet(xHandle8825Ctrl) <= configMAX_CO_ROUTINE_PRIORITIES) {
	// 		vTaskSuspend(xHandle8825Ctrl);
	// 	}
	// }

	if(xTaskCreate(prvTask_BluetoothInitialize, (const char *) "BluetoothInitialize",
		configMINIMAL_STACK_SIZE, NULL, 1, &xHandleBLTiInit) == pdPASS) {
		// if(uxTaskPriorityGet(xHandleBltInit) < configMAX_CO_ROUTINE_PRIORITIES)
		{
			vTaskSuspend(xHandleBLTiInit);
		}
	}

	if(xTaskCreate(prvTask_BluetoothProcess, (const char *) "BluetoothProcess",
		configMINIMAL_STACK_SIZE, NULL, 1, &xHandleBLTiProc) == pdPASS) {
		// if(uxTaskPriorityGet(xHandleBltProc) < configMAX_CO_ROUTINE_PRIORITIES)
		{
			vTaskSuspend(xHandleBLTiProc);
		}
	}

	if(xTimerStart(xWatchDogTimer, 0) == pdPASS) {
		vTaskStartScheduler();
	}

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}

static void prvWatchDogTimerCallback( TimerHandle_t xTimer ) 
{
	(void) xTimer;

	// WhatcDog Activity Led
	SetGpio(47, ucWatchDogCounter % 2);

	ucWatchDogCounter++;
}

static void prvTicResetTimerCallback( TimerHandle_t xTimer ) 
{
	(void) xTimer;


}

// extern TDWHCIDevice *e_pThis;
// extern int e_nChannel;
// extern void DWHCIDeviceTimerHandler (void *pContext, unsigned int nChannel);
// static void prvDWHCIDeviceTimerCallback( TimerHandle_t xTimer ) 
// {
// 	(void) xTimer;

// 	if(e_pThis != NULL && e_nChannel > -1) {
// 		DWHCIDeviceTimerHandler(e_pThis, (unsigned int) e_nChannel);
// 	}
// }
