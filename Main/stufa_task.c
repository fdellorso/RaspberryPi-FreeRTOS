//stufa_task.c
//authored by Francesco Dell'Orso
//
//tasks for StuFA Project

// TODO Resolve DRB8825 Interrupt
// TODO Conversion loops -> pulses
// TODO Tic Home Routine

// TODO Update FreeRTOS 10.0
// TODO Update USPi 2.00
// TODO Understand if Uart FIFO Flush works
// TODO Investigate Data Memory Barrier
// TODO Investigate benefit of Cache
// TODO Investigate benefit of Newlib implementation (Syscalls.c)


#include "stufa_task.h"

#include <shared_resources.h>
#include <tic_taskfunc.h>

#include <rpi_header.h>
#include <rpi_logger.h>
#include <prvlib/stdlib.h>

#include <uspi/assert.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include <uspi.h>
#include <tic.h>
#include <rsta_bt/btsubsystem.h>


// Tasks
xTaskHandle			xHandleWDog			= NULL;
xTaskHandle			xHandleUSPi			= NULL;
xTaskHandle			xHandleTicCtrl		= NULL;
xTaskHandle			xHandleTicCnsl		= NULL;
xTaskHandle			xHandle8825Ctrl		= NULL;
xTaskHandle			xHandleBluetooth	= NULL;

// Semaphores
xSemaphoreHandle	xSemUSPiInit	= NULL;
xSemaphoreHandle	xSemTicInit		= NULL;

// Queues
xQueueHandle		xQueTicHdl		= NULL;
xQueueHandle		xQueTicVar		= NULL;
xQueueHandle		xQueTicSet		= NULL;
xQueueHandle		xQueTicCmd		= NULL;

// Mutexs
xSemaphoreHandle	xMutexMuart		= NULL;
xSemaphoreHandle	xMutexTicVar	= NULL;
xSemaphoreHandle	xMutexEnergize	= NULL;
xSemaphoreHandle	xMutexRunning	= NULL;


// Internal function Declaration
void vStepISR(int nIRQ, void *pParam);


// WatchDog boot other tasks and 
void prvTask_WatchDog(void *pParam) {
	int i = 0;

	/* Stop warnings. */
	( void ) pParam;

	tic_handle * ticHandle = NULL;

	static const portTickType xPeriod = 500 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();

	static const portTickType xBlockTime = 100 / portTICK_RATE_MS;

	xMutexMuart = xSemaphoreCreateMutex();
	xMutexTicVar = xSemaphoreCreateMutex();

	logger_init();
	#ifdef ILI9340
		bcm2835_init();
		ili9340_set_rotation(1);
	#endif

	prvFunc_Print("\nWatchDog...\t\t\t     Started");

	vTaskResume(xHandleUSPi);

	// if(USPiTicAvailable())	vTaskResume(xHandleTicCtrl);
	// if(0) 					vTaskResume(xHandle8825Ctrl);
	// if(USPiBTAvailable())	vTaskResume(xHandleBluetooth);

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
		if(xTaskIsTaskSuspended(xHandleUSPi) == pdFALSE && xSemUSPiInit != NULL) {
			if(xSemaphoreTake(xSemUSPiInit, xBlockTime) == pdPASS) {
				vSemaphoreDelete(xSemUSPiInit);
				vTaskSuspend(xHandleUSPi);
				if(USPiTicAvailable())	vTaskResume(xHandleTicCtrl);
				if(0) 					vTaskResume(xHandle8825Ctrl);
				vTaskResume(xHandleBluetooth);
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
	int uspiInited = 0;

	/* Stop warnings. */
	( void ) pParam;

	vSemaphoreCreateBinary(xSemUSPiInit);
	if(xSemUSPiInit != NULL) xSemaphoreTake(xSemUSPiInit, 0);
	
	prvFunc_Print("\nUSPi Initialize...\t\t     Started");
	prvFunc_Print("--------------------------------------------");

	while(1) {
		i++;

		while(uspiInited == 0) {
			uspiInited = USPiInitialize();
		}

		if(uspiInited == 1) {
			// Give Semaphore back when USPi is initialized
			if(xSemaphoreGive(xSemUSPiInit) == pdPASS) {
				uspiInited = 2;
				prvFunc_Print("--------------------------------------------");
				prvFunc_Print("USPi Initialize...\t\t    Finished");
			}
		}

		// TODO Close USPi

		vTaskDelay(100);
	}
}


void prvTask_TicControl(void *pParam) {
	int i = 0;

	/* Stop warnings. */
	( void ) pParam;

	tic_error		* error = NULL;

	tic_device		* ticDevice = NULL;
	tic_handle		* ticHandle = NULL;
	tic_variables	* ticVariables = NULL;
	tic_settings	* ticSettings = NULL;

	tic_command		* ticCommand = NULL;

	static const portTickType xBlockTime = 100 / portTICK_RATE_MS;

	vSemaphoreCreateBinary(xSemTicInit);
	if(xSemTicInit != NULL) xSemaphoreTake(xSemTicInit, 0);
	xMutexEnergize = xSemaphoreCreateMutex();
	if(xMutexEnergize != NULL) xSemaphoreTake(xMutexEnergize, 0);
	xMutexRunning = xSemaphoreCreateMutex();
	if(xMutexRunning != NULL) xSemaphoreTake(xMutexRunning, 0);

	xQueTicHdl = xQueueCreate(1, sizeof(tic_handle *));
	xQueTicVar = xQueueCreate(1, sizeof(tic_variables *));
	xQueTicSet = xQueueCreate(1, sizeof(tic_settings *));
	xQueTicCmd = xQueueCreate(1, sizeof(tic_command *));

	prvFunc_Print("\nTic Control...\t\t\t     Started");

	if(xSemaphoreTake(xMutexTicVar, portMAX_DELAY) == pdPASS) {
		if (error == NULL) error = tic_device_create(&ticDevice);
		if (error == NULL) error = tic_handle_create(ticDevice, &ticHandle);
		prvFunc_Print("%cFirmware Version...\t\t\t%s", 0x3e,
			tic_get_firmware_version_string(ticHandle));

		if (error == NULL) error = prvFunc_TicMotorInit(ticHandle);

		if (error == NULL) error = tic_get_variables(ticHandle, &ticVariables, false);
		if (error == NULL) error = tic_get_settings(ticHandle, &ticSettings);

		if (error == NULL) error = tic_halt_and_set_position(ticHandle,
			tic_variables_get_target_position(ticVariables));

		prvFunc_TicCommandInit(&ticCommand);

		xSemaphoreGive(xMutexTicVar);
	}

	if(xQueueSend(xQueTicHdl, &ticHandle, 0) == pdPASS) {
		prvFunc_Print("%cTicHandle...\t\t\t\tSent", 0x3e);
	}

	if(xQueueSend(xQueTicVar, &ticVariables, 0) == pdPASS) {
		prvFunc_Print("%cTicVariables...\t\t\tSent", 0x3e);
	}

	if(xQueueSend(xQueTicSet, &ticSettings, 0) == pdPASS) {
		prvFunc_Print("%cTicSettings...\t\t\t\tSent", 0x3e);
	}

	if(xQueueSend(xQueTicCmd, &ticCommand, 0) == pdPASS) {
		prvFunc_Print("%cTicCommand...\t\t\t\tSent", 0x3e);
	}

	if(xSemaphoreGive(xSemTicInit) == pdPASS) {
		prvFunc_Print("%cTic Control...\t\t\t      Inited", 0x3e);
	}

	// FIXME Simulation
	// ticCommand->command = 'x';
	// FIXME Simulation

	while(1) {
		i++;

		if(xSemaphoreTake(xMutexTicVar, portMAX_DELAY) == pdPASS) {
			if(ticCommand->command != ticCommand->command_old) {
				if (error == NULL) 
					error = prvFunc_TicCommandExec(ticCommand, ticSettings, ticHandle);
			}

			if(ticCommand->command == 'n') {
				xSemaphoreTake(xMutexEnergize, xBlockTime);
				xSemaphoreGive(xMutexRunning);
			}

			if(ticCommand->command == 'x') {
				xSemaphoreGive(xMutexEnergize);
				xSemaphoreTake(xMutexRunning, xBlockTime);
			}

			// TODO find condition to update TicVar & TicSet
			{
				if (error == NULL)
					error = tic_get_variables(ticHandle, &ticVariables, false);
				// TODO Update TicSettings not needed
				// if (error == NULL)
				// 	error = tic_get_settings(ticHandle, &ticSettings);
			}

			if(abs(tic_variables_get_current_position(ticVariables) - 
				tic_variables_get_target_position(ticVariables)) <= 1) {
				// FIXME Simulation
				// if(i%2){
				// 	if (error == NULL)
				// 		error = tic_set_target_position(ticHandle, 1200);
				// }
				// else {
				// 	if (error == NULL)
				// 		error = tic_set_target_position(ticHandle, -1200);
				// }
				// FIXME Simulation

				ticCommand->command = 'n';
			}
						
			xSemaphoreGive(xMutexTicVar);
		}

		vTaskDelay(100);
	}
}


void prvTask_TicConsole(void *pParam) {
	int i = 0;

	/* Stop warnings. */
	( void ) pParam;

	tic_variables	* ticVariables = NULL;
	tic_settings	* ticSettings = NULL;
	tic_command		* ticCommand = NULL;

	static const portTickType xBlockTime = 100 / portTICK_RATE_MS;

	prvFunc_Print("\nTic Console...\t\t\t     Started");

	if(xQueueReceive(xQueTicVar, &ticVariables, portMAX_DELAY) == pdPASS) {
		vQueueDelete(xQueTicVar);
		prvFunc_Print("%cTicVariables...\t\t    Received", 0x3e);
	}

	if(xQueueReceive(xQueTicSet, &ticSettings, portMAX_DELAY) == pdPASS) {
		vQueueDelete(xQueTicSet);
		prvFunc_Print("%cTicSettings...\t\t\t    Received", 0x3e);
	}

	if(xQueueReceive(xQueTicCmd, &ticCommand, portMAX_DELAY) == pdPASS) {
		vQueueDelete(xQueTicCmd);
		prvFunc_Print("%cTicCommand...\t\t\t    Received", 0x3e);
	}

	prvFunc_Print("%cTic Console...\t\t\t      Inited", 0x3e);

	while(1) {
		i++;

		if(xSemaphoreTake(xMutexTicVar, portMAX_DELAY) == pdPASS) {
			prvFunc_TicMenu(ticVariables, ticSettings);

			// FIXME find better filter
			// if((tic_variables_get_error_status(ticVariables) & (1 << TIC_ERROR_SAFE_START_VIOLATION))) {
			if(xSemaphoreTake(xMutexRunning, xBlockTime) == pdPASS) {	
				prvFunc_TicCommandScan(ticCommand, ticVariables);
				xSemaphoreGive(xMutexRunning);
			}

			xSemaphoreGive(xMutexTicVar);
		}

		vTaskDelay(500);
	}
}


int step;

void prvTask_8825Control(void *pParam) {
	int i = 0;

	/* Stop warnings. */
	( void ) pParam;

	step = 0;

	SetGpioFunction(16, GPIO_FUNC_OUTPUT);
	SetGpioFunction(19, GPIO_FUNC_OUTPUT);		// Raw Test Stepping

	PudGpio(16, PULL_DISABLE);
	PudGpio(19, PULL_DISABLE);

	SetGpio(16, 0);
	SetGpio(19, step%2);


	taskENTER_CRITICAL();

	prvSystemTimerStepSetup();
	RegisterInterrupt(BCM2835_IRQ_ID_ST_C3, vStepISR, NULL);
	EnableInterrupt(BCM2835_IRQ_ID_ST_C3);

	taskEXIT_CRITICAL();


	prvFunc_Print("\nDrv8825 Control...\t\t     Started");

	while(1) {
		i++;

		// prvFunc_Print("Step nr. %u", step);

		vTaskDelay(100);
	}
}

void vStepISR(int nIRQ, void *pParam) {
	(void)nIRQ;		// FIXME Wunused
	(void)pParam;	// FIXME Wunused

	// if(dir == 0)	step += 1;
	// else			step -= 1;

	// if(step == 0) {
	// 	dir = 0;
	// 	SetGpio(16, dir);
	// }

	// if(step == 400 - 1) {
	// 	step = 0;
	// 	dir = 1;
	// 	SetGpio(16, dir);
	// }

	step += 1;

	if(step == 400 - 1) {
		step = -400;
		SetGpio(16, 1);							// Raw Test Stepping
	}

	if(step == 0) {
		SetGpio(16, 0);							// Raw Test Stepping
	}

	SetGpio(19, step%2);						// Raw Test Stepping

	prvSystemTimerStepClear();
}


#define INQUIRY_SECONDS		20

void prvTask_Bluetooth(void *pParam) {
	int i = 0;

	/* Stop warnings. */
	( void ) pParam;

	TBTSubSystem *m_Bluetooth = (TBTSubSystem *) malloc (sizeof(TBTSubSystem));
	BTSubSystem(m_Bluetooth, 0, NULL);
	BTSubSystemInitialize(m_Bluetooth);

	prvFunc_Print("Inquiry is running for %u seconds", INQUIRY_SECONDS);

	TBTInquiryResults *pInquiryResults = BTSubSystemInquiry(m_Bluetooth, INQUIRY_SECONDS);
	if (pInquiryResults == 0)
	{
		prvFunc_Print("Inquiry failed");
	}

	prvFunc_Print("Inquiry complete, %u device(s) found", BTInquiryResultsGetCount(pInquiryResults));

	if (BTInquiryResultsGetCount(pInquiryResults) > 0)
	{
		prvFunc_Print("# BD address        Class  Name");

		for (unsigned nDevice = 0; nDevice < BTInquiryResultsGetCount(pInquiryResults); nDevice++)
		{
			const u8 *pBDAddress = BTInquiryResultsGetBDAddress(pInquiryResults, nDevice);
			assert (pBDAddress != 0);
			
			const u8 *pClassOfDevice = BTInquiryResultsGetClassOfDevice(pInquiryResults, nDevice);
			assert (pClassOfDevice != 0);
			
			prvFunc_Print("%u %02X:%02X:%02X:%02X:%02X:%02X %02X%02X%02X %s",
					nDevice+1,
					(unsigned) pBDAddress[5],
					(unsigned) pBDAddress[4],
					(unsigned) pBDAddress[3],
					(unsigned) pBDAddress[2],
					(unsigned) pBDAddress[1],
					(unsigned) pBDAddress[0],
					(unsigned) pClassOfDevice[2],
					(unsigned) pClassOfDevice[1],
					(unsigned) pClassOfDevice[0],
					BTInquiryResultsGetRemoteName(pInquiryResults, nDevice));
		}
	}

	free(pInquiryResults);

	while(1) {
		i++;

		// prvFunc_Print("Step nr. %u", step);

		vTaskDelay(100);
	}
}

