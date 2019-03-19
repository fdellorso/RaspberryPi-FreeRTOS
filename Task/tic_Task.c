// tic_Task.c
// authored by Francesco Dell'Orso
//
// tasks for StuFA Project

#include "stufa_Task.h"

#include <uspi/types.h>

extern SemaphoreHandle_t	xSemTicInit;

extern QueueHandle_t		xQueTicHdl;
extern QueueHandle_t		xQueTicVar;
extern QueueHandle_t		xQueTicSet;
extern QueueHandle_t		xQueTicCmd;

extern const TickType_t 	xBlockTime;
extern SemaphoreHandle_t	xMutexTicVar;
extern SemaphoreHandle_t	xMutexEnergize;
extern SemaphoreHandle_t	xMutexRunning;

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

	// vSemaphoreCreateBinary(xSemTicInit);
	if(xSemTicInit != NULL) xSemaphoreTake(xSemTicInit, 0);
	// xMutexEnergize = xSemaphoreCreateMutex();
	if(xMutexEnergize != NULL) xSemaphoreTake(xMutexEnergize, 0);
	// xMutexRunning = xSemaphoreCreateMutex();
	if(xMutexRunning != NULL) xSemaphoreTake(xMutexRunning, 0);

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
