//stufa_task.c
//authored by Francesco Dell'Orso
//
//tasks for StuFA Project

// TODO Finish Tic Menu (Ideate way to pass command tot Tic_Control)
// TODO Update FreeRTOS
// TODO Understand if Uart FIFO Flush works
// TODO Investigate Data Memory Barrier
// TODO Investigate benefit of Cache
// TODO Investigate benefit of Newlib implementation (Syscalls.c)


#include "stufa_task.h"
#include <uspi/stdarg.h>
#include <uspi/util.h>

#include <rpi_header.h>
#include <rpi_logger.h>
#include <prvlib/stdlib.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include <uspi.h>
#include <tic.h>


// Struct to pass command from Console to Control
typedef struct tic_command {
	char command;
	char command_old;
	int value;
} tic_command;

// Tasks
xTaskHandle			xHandleWDog		= NULL;
xTaskHandle			xHandleUSPi		= NULL;
xTaskHandle			xHandleTicCtrl	= NULL;
xTaskHandle			xHandleTicCnsl	= NULL;

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

// Internal function Declaration
// UI function
static void prvFunc_Print(const char *pMessage, ...);	// Print everywhere
char * prvFunc_Scan(char * pDest);						// Scan only from UART

void prvFunc_TicMenu(tic_variables * ticVariables, tic_settings * ticSettings);
char * tic_code_shifter(char * pDest, uint16_t code);	// Explode error code

// Command function
int prvFunc_TicCommandInit(tic_command **ticCommand);
void prvFunc_TicCommandScan(tic_command * ticCommand, tic_variables * ticVariables,
	tic_settings * ticSettings);
tic_error * prvFunc_TicCommandExec(tic_command * ticCommand, tic_settings * ticSettings,
	tic_handle * ticHandle);


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

	while(1) {
		i++;

		vTaskDelayUntil(&xLastWakeTime, xPeriod);

		// WhatcDog Activity Led
		SetGpio(47, i%2);

		// prvFunc_Print("%cHeap Free Meamory...\t\t      %u", 0x3e, xPortGetFreeHeapSize());

		// IF Tic is energized send Reset Timeout
		if(ticHandle != NULL && xMutexEnergize != NULL) {
			if(xSemaphoreTake(xMutexEnergize, xBlockTime) == pdPASS) {
				tic_reset_command_timeout(ticHandle);
				xSemaphoreGive(xMutexEnergize);
			}
		}

		

		// IF USPi is initialized start TicControl
		if(xTaskIsTaskSuspended(xHandleUSPi) == pdFALSE && xSemUSPiInit != NULL) {
			if(xSemaphoreTake(xSemUSPiInit, xBlockTime) == pdPASS) {
				vSemaphoreDelete(xSemUSPiInit);
				vTaskSuspend(xHandleUSPi);
				if(USPiTicAvailable()) vTaskResume(xHandleTicCtrl);
			}
		}

		// IF TicControl is initialized start TicConsole
		if(xTaskIsTaskSuspended(xHandleTicCnsl) == pdTRUE && xSemTicInit != NULL) {
			if(xSemaphoreTake(xSemTicInit, xBlockTime) == pdPASS) {
				vSemaphoreDelete(xSemTicInit);
				vTaskResume(xHandleTicCnsl);
			}
		}

		// Receiving TicHandle to manage Tic Command Timeout
		if(xQueTicHdl != NULL) {
			if(xQueueReceive(xQueTicHdl, &ticHandle, xBlockTime) == pdPASS) {
				vQueueDelete(xQueTicHdl);
				prvFunc_Print("\nWatchDog...\n%cTicHandle...\t\t\t    Received", 0x3e);
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

	vSemaphoreCreateBinary(xSemTicInit);
	if(xSemTicInit != NULL) xSemaphoreTake(xSemTicInit, 0);
	xMutexEnergize = xSemaphoreCreateMutex();
	if(xMutexEnergize != NULL) xSemaphoreTake(xMutexEnergize,0);

	xQueTicHdl = xQueueCreate(1, sizeof(tic_handle *));
	xQueTicVar = xQueueCreate(1, sizeof(tic_variables *));
	xQueTicSet = xQueueCreate(1, sizeof(tic_settings *));
	xQueTicCmd = xQueueCreate(1, sizeof(tic_command *));

	prvFunc_Print("\nTic Control...\t\t\t     Started");

	if(xSemaphoreTake(xMutexTicVar,portMAX_DELAY) == pdPASS) {
		if (error == NULL) error = tic_device_create(&ticDevice);
		if (error == NULL) error = tic_handle_create(ticDevice, &ticHandle);
		prvFunc_Print("%cFirmware Version...\t\t\t%s", 0x3e,
			tic_get_firmware_version_string(ticHandle));
		
		// FIXME Simulation
		if (error == NULL) error = tic_exit_safe_start(ticHandle);
		if (error == NULL) error = tic_energize(ticHandle);
		// FIXME Simulation

		if (error == NULL) error = tic_get_variables(ticHandle, &ticVariables, false);
		if (error == NULL) error = tic_get_settings(ticHandle, &ticSettings);

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

	while(1) {
		i++;

		// FIXME Receive TicCommand and execute
		if(xSemaphoreTake(xMutexTicVar, portMAX_DELAY) == pdPASS) {
			if(ticCommand->command != ticCommand->command_old) {
				if (error == NULL) 
					error = prvFunc_TicCommandExec(ticCommand, ticSettings, ticHandle);
			}

			// TODO Check acting variable
			// if(tic_variables_get_acting_target_position(ticVariables)) {
				
			if(tic_variables_get_current_position(ticVariables) == 
				tic_variables_get_target_position) {
				// FIXME Simulation
				if(i%2){
					if (error == NULL)
						error = tic_set_target_position(ticHandle, 200);
				}
				else {
					if (error == NULL)
						error = tic_set_target_position(ticHandle, -200);
				}
				// FIXME Simulation
			}

			xSemaphoreTake(xMutexEnergize,portMAX_DELAY);
			if(tic_variables_get_energized(ticVariables))
				xSemaphoreGive(xMutexEnergize);

			// TODO find condition to update TicVar & TicSet
			{
				if (error == NULL)
					error = tic_get_variables(ticHandle, &ticVariables, false);
				if (error == NULL)
					error = tic_get_settings(ticHandle, &ticSettings);
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

			if(tic_variables_get_current_position(ticVariables) == 
				tic_variables_get_target_position) {
				prvFunc_TicCommandScan(ticCommand, ticVariables, ticSettings);
			}

			xSemaphoreGive(xMutexTicVar);
		}

		vTaskDelay(100);
	}
}

// UI Function
static void prvFunc_Print(const char *pMessage, ...) {
	va_list args;

	va_start(args, pMessage);

	if(xSemaphoreTake(xMutexMuart, portMAX_DELAY) == pdPASS) {
		vprintf(pMessage, args);
		xSemaphoreGive(xMutexMuart);
	}

	va_end (args);
}

char * prvFunc_Scan(char * pDest) {
	char * c = pDest;
	
	if(xSemaphoreTake(xMutexMuart, portMAX_DELAY) == pdPASS) {
		gets((unsigned char *)c);
		xSemaphoreGive(xMutexMuart);
	}

	return c;
}

void prvFunc_TicMenu(tic_variables * ticVariables, tic_settings * ticSettings) {
	char ticErrorName[180];
	ticErrorName[0] = '\0';

	tic_code_shifter(ticErrorName, tic_variables_get_error_status(ticVariables));
	
	prvFunc_Print("\nTic Command Menu");
	prvFunc_Print("----------------");

	prvFunc_Print("\n[Status]");
	prvFunc_Print("Operation state:  %s\t\tDevice reset:\t  %s",
		tic_look_up_operation_state_name_ui(
			tic_variables_get_operation_state(ticVariables)),
		tic_look_up_device_reset_name_ui(
			tic_variables_get_device_reset(ticVariables)));
	prvFunc_Print("Planning mode:\t  %s\tInput state:\t  %s",
		tic_look_up_planning_mode_name_ui(
			tic_variables_get_planning_mode(ticVariables)),
		tic_look_up_input_state_name_ui(
			tic_variables_get_input_state(ticVariables)));
	prvFunc_Print("Current limit:\t  %u mA\t\tStep mode:\t  %s",
		tic_settings_get_current_limit(ticSettings),
		tic_look_up_step_mode_name_ui(tic_settings_get_step_mode(ticSettings)));
	prvFunc_Print("Max speed:\t  %u pulses/s\tMax acceleration: %u pulses/s",
		tic_settings_get_max_speed(ticSettings)/10000,
		tic_settings_get_max_accel(ticSettings)/100);
	prvFunc_Print("Target minimum:\t  %d\t\t\tTarget maximum:\t  %d",
		tic_settings_get_output_min(ticSettings),
		tic_settings_get_output_max(ticSettings));
	prvFunc_Print("Target position:  %d\t\t\tTarget velocity:  %d",
		tic_variables_get_target_position(ticVariables),
		tic_variables_get_target_velocity(ticVariables));
	prvFunc_Print("Current position: %d\t\t\tCurrent velocity: %d",
		tic_variables_get_current_position(ticVariables),
		tic_variables_get_current_velocity(ticVariables));
	prvFunc_Print("Error status:\t  %s", ticErrorName);
	
	if(!tic_variables_get_energized(ticVariables)) {
		prvFunc_Print("\n[Command]");
		prvFunc_Print("Set target [P]osition\t\t\tSet target [V]elocity");
		prvFunc_Print("Set max [S]peed\t\t\t\tSet max [A]cceleration");
		prvFunc_Print("Set step [M]ode\t\t\t\tSet current [L]imit");
		prvFunc_Print("E[X]it safe start\t\t\tE[N]ter safe start");
		prvFunc_Print("[E]nergize\t\t\t\t[D]e-energize");

		prvFunc_Print("\nChoose an available command [A,D,E,L,M,N,P,S,V,X]: ");
	}
}

char * tic_code_shifter(char * pDest, uint16_t code) {
	int i = 0;
	char * pCode = pDest;

	for(;i < 8; i++) {
		if(code & (1<<i)) {
			strcat(pCode, tic_look_up_error_name_ui(code & (1<<i)));
			strcat(pCode, "\n\t\t  ");
		}
	}

	pCode[strlen(pCode)-5] = '\0';

	return pCode;
}

// Command function
int prvFunc_TicCommandInit(tic_command **ticCommand) {
	tic_command * new_ticCommand = NULL;

	new_ticCommand = (tic_command *)calloc(1, sizeof(tic_command));
    if(new_ticCommand == NULL) return -1;

	new_ticCommand->command		= '0';
	new_ticCommand->command_old	= '0';
	new_ticCommand->value		= 0;

	*ticCommand = new_ticCommand;

	new_ticCommand = NULL;

	// free(ticCommand);

	return 0;
}

void prvFunc_TicCommandScan(tic_command * ticCommand, tic_variables * ticVariables,
	tic_settings * ticSettings) {

	tic_command * new_ticCommand = ticCommand;
	char * charCommand = 0;
	int intCommand = 0;

	prvFunc_Scan(charCommand);

	// TODO use tic_get instead direct access
	switch(charCommand[0]) {
		case 'a':
		case 'A':
			prvFunc_Print("\nMax acceleration range [100 to 2,147,483,647]: ");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= 0x64) && (intCommand <= 0x7fffffff)) {
				if((unsigned)intCommand != ticVariables->max_accel) {
					new_ticCommand->command = 'a';
					new_ticCommand->value = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [100 to 2,147,483,647]");
			break;
		case 'd':
		case 'D':
			new_ticCommand->command = 'd';
			new_ticCommand->value = 0;
			break;
		case 'e':
		case 'E':
			new_ticCommand->command = 'e';
			new_ticCommand->value = 0;
			break;
		case 'l':
		case 'L':
			prvFunc_Print("\nCurrent limit range [0 to %u]: ",
				TIC_MAX_ALLOWED_CURRENT_T834);
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= 0) && (intCommand <= 124)) {
				if(intCommand != (signed)ticSettings->current_limit) {
					new_ticCommand->command = 'l';
					new_ticCommand->value = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [0 to %u]",
				TIC_MAX_ALLOWED_CURRENT_T834);
			break;
		case 'm':
		case 'M':
			prvFunc_Print("\nStep mode range [0 to 5]: ");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= 0) && (intCommand <= 5)) {
				if(intCommand != ticVariables->step_mode) {
					new_ticCommand->command = 'm';
					new_ticCommand->value = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [0 to 5]");
			break;
		case 'n':
		case 'N':
			new_ticCommand->command = 'n';
			new_ticCommand->value = 0;
			break;
		case 'p':
		case 'P':
			prvFunc_Print("\nTarget position range [-2,147,483,647 to 2,147,483,647]: ");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= -(signed)0x7fffffff) && 
				(intCommand <= (signed)0x7fffffff)) {
				if(intCommand != ticVariables->target_position) {
					new_ticCommand->command = 'p';
					new_ticCommand->value = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [-2,147,483,647 to 2,147,483,647]");
			break;
		case 's':
		case 'S':
			prvFunc_Print("\nMax speed range [0 to 500,000,000]: ");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= 0) && (intCommand <= 500000000)) {
				if((unsigned)intCommand != ticVariables->max_speed) {
					new_ticCommand->command = 's';
					new_ticCommand->value = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [0 to 500,000,000]");
			break;
		case 'v':
		case 'V':
			prvFunc_Print("\nTarget velocity range [-500,000,000 to 500,000,000]: ");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= -500000000) && (intCommand <= 500000000)) {
				if(intCommand != ticVariables->target_velocity) {
					new_ticCommand->command = 'v';
					new_ticCommand->value = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [-500,000,000 to 500,000,000]");
			break;
		case 'x':
		case 'X':
			new_ticCommand->command = 'x';
			new_ticCommand->value = 0;
			break;
		default:
			prvFunc_Print("\nInvalid command");
	}
}

tic_error * prvFunc_TicCommandExec(tic_command * ticCommand, tic_settings * ticSettings,
	tic_handle * ticHandle) {
	tic_error * error = NULL;

	switch(ticCommand->command) {
		case 'a':
		case 'A':
			error = tic_set_max_accel(ticHandle, ticCommand->value);
			break;
		case 'd':
		case 'D':
			error = tic_deenergize(ticHandle);
			break;
		case 'e':
		case 'E':
			error = tic_energize(ticHandle);
			break;
		case 'l':
		case 'L':
			error = tic_set_current_limit(ticHandle, ticCommand->value);
			break;
		case 'm':
		case 'M':
			error = tic_set_step_mode(ticHandle, ticCommand->value);
			break;
		case 'n':
		case 'N':
			error = tic_enter_safe_start(ticHandle);
			break;
		case 'o':
		case 'O':
			tic_settings_set_output_max(ticSettings, ticCommand->value);
			tic_settings_set_output_min(ticSettings, -(ticCommand->value));
			error = tic_set_settings(ticHandle, ticSettings);
			break;
		case 'p':
		case 'P':
			error = tic_set_target_position(ticHandle, ticCommand->value);
			break;
		case 's':
		case 'S':
			error = tic_set_max_speed(ticHandle, ticCommand->value);
			break;
		case 'v':
		case 'V':
			error = tic_set_target_velocity(ticHandle, ticCommand->value);
			break;
		case 'x':
		case 'X':
			error = tic_exit_safe_start(ticHandle);
			break;
		default:
			prvFunc_Print("\nInvalid command");
	}

	ticCommand->command_old = ticCommand->command;

	return error;
}

// typedef tic_error * (FUNC_HANDLER)(void **pParam);

// tic_error * prvFunc_Caller(FUNC_HANDLER pfnHandler, void *pParam);

// tic_error * prvFunc_Caller(FUNC_HANDLER pfnHandler, void *pParam) {
// 	tic_error * error = NULL;
	
// 	if(xSemaphoreTake(xMutexTicVar,portMAX_DELAY) == pdPASS) {
// 		error = pfnHandler(pParam);
// 	}

// 	xSemaphoreGive(xMutexTicVar);

// 	return error;
// }
