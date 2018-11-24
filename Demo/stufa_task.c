//stufa_task.c
//authored by Francesco Dell'Orso
//
//tasks for StuFA Project

// TODO Finish Tic Menu (Ideate way to pass command tot Tic_Control)
// TODO Protect Access to ticSettings & ticVariables
// TODO Implement SPI Library
// TODO Use SysTimer as FreeRTOS Tick
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

// TODO transform in 2 element struct command & value
typedef struct tic_command {
	bool change;
	int max_position;
	int target_position;
	int target_velocity;
	unsigned int max_speed;
	unsigned int max_acceleration;
	unsigned char step_mode;
	unsigned char current_limit;
	bool exit_safe_start;
	bool enter_safe_start;
	bool de_energize;
	bool energize;
} tic_command;


xTaskHandle			xHandleWDog		= NULL;
xTaskHandle			xHandleUSPi		= NULL;
xTaskHandle			xHandleTicCtrl	= NULL;
xTaskHandle			xHandleTicCnsl	= NULL;

xSemaphoreHandle	xSemUSPiInit	= NULL;
xSemaphoreHandle	xSemTicInit		= NULL;
xQueueHandle		xQueTicVar		= NULL;
xQueueHandle		xQueTicSet		= NULL;
xQueueHandle		xQueTicCom		= NULL;

// xSemaphoreHandle	xSemTicEner		= NULL;
// xQueueHandle		xQueTicHndl		= NULL;

xSemaphoreHandle	xMutexMuart		= NULL;
xSemaphoreHandle	xMutexFcall		= NULL;


static void prvFunc_Print(const char *pMessage, ...);
char * prvFunc_Scan(char * pDest);

void prvFunc_TicMenu(tic_variables * ticVariables, tic_settings * ticSettings);
char * tic_code_shifter(char * pDest, uint16_t code);

int prvFunc_TicCommandInit(tic_command **ticCommand, tic_variables *ticVariables,
	tic_settings * ticSettings);
tic_command * prvFunc_TicCommandScan(tic_command * ticCommand);
int prvFunc_TicCommandExec(tic_command * ticCommand, tic_variables *ticVariables,
	tic_settings * ticSettings, tic_handle * ticHandle);

void prvTask_WatchDog(void *pParam) {
	int i = 0;
	uint32_t ticEnergized = 0;

	/* Stop warnings. */
	( void ) pParam;

	static const portTickType xPeriod = 500 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();

	static const portTickType xBlockTime = 100 / portTICK_RATE_MS;

	xMutexMuart = xSemaphoreCreateMutex();
	xMutexFcall = xSemaphoreCreateMutex();

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

		// IF Tic is energized send Reset Timeout
		if(ticEnergized) {
			// USPiTicQuick(TIC_CMD_RESET_COMMAND_TIMEOUT);
			// tic_reset_command_timeout(ticHandle);
		}

		// IF USPi is initialized start TicControl
		if(xTaskIsTaskSuspended(xHandleUSPi) == pdFALSE && xSemUSPiInit != NULL) {
			if(xSemaphoreTake(xSemUSPiInit, xBlockTime) == pdPASS) {
				vSemaphoreDelete(xSemUSPiInit);
				vTaskSuspend(xHandleUSPi);
				vTaskResume(xHandleTicCtrl);
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
	uint32_t uspiInited = 0;

	/* Stop warnings. */
	( void ) pParam;

	vSemaphoreCreateBinary(xSemUSPiInit);
	if(xSemUSPiInit != NULL) xSemaphoreTake(xSemUSPiInit, 0);
	
	prvFunc_Print("\nUSPi Initialize...\t\t     Started");
	prvFunc_Print("--------------------------------------------");

	while(1) {
		i++;

		while(uspiInited == 0) {
			uspiInited = (uint32_t) USPiInitialize();
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

		vTaskDelay(200);
	}
}

void prvTask_TicControl(void *pParam) {
	int i = 0;
	// uint32_t ticEnergized = 0;

	/* Stop warnings. */
	( void ) pParam;

	tic_error * error = NULL;

	tic_device * ticDevice = NULL;
	tic_handle * ticHandle = NULL;
	tic_variables * ticVariables = NULL;
	tic_settings * ticSettings = NULL;

	tic_command * ticCommand = NULL;

	vSemaphoreCreateBinary(xSemTicInit);
	if(xSemTicInit != NULL) xSemaphoreTake(xSemTicInit, 0);

	prvFunc_Print("\nTic Control...\t\t\t     Started");

	if(xSemaphoreTake(xMutexFcall,portMAX_DELAY) == pdPASS) {
		if (error == NULL) error = tic_device_create(&ticDevice);
		if (error == NULL) error = tic_handle_create(ticDevice, &ticHandle);
		prvFunc_Print("%cFirmware Version...\t\t\t%s", 0x3e,
			tic_get_firmware_version_string(ticHandle));

		// TODO put also in loop with if
		if (error == NULL) error = tic_get_variables(ticHandle, &ticVariables, false);
		if (error == NULL) error = tic_get_settings(ticHandle, &ticSettings);

		prvFunc_TicCommandInit(&ticCommand, ticVariables, ticSettings);
	}
	xSemaphoreGive(xMutexFcall);

	if(xSemaphoreGive(xSemTicInit) == pdPASS) {
		prvFunc_Print("%cTic Control...\t\t\t      Inited", 0x3e);
	}

	xQueTicVar = xQueueCreate(1, sizeof(tic_variables *));
	xQueTicSet = xQueueCreate(1, sizeof(tic_settings *));
	xQueTicCom = xQueueCreate(1, sizeof(tic_command *));

	if(xQueueSend(xQueTicVar, &ticVariables, 0) == pdPASS) {
		prvFunc_Print("%cTicVariables...\t\t\tSent", 0x3e);
	}

	if(xQueueSend(xQueTicSet, &ticSettings, 0) == pdPASS) {
		prvFunc_Print("%cTicSettings...\t\t\t\tSent", 0x3e);
	}

	if(xQueueSend(xQueTicCom, &ticCommand, 0) == pdPASS) {
		prvFunc_Print("%cTicCommand...\t\t\t\tSent", 0x3e);
	}

	// TODO put also in loop with if


	while(1) {
		i++;

		vTaskDelay(100);
	}
}

void prvTask_TicConsole(void *pParam) {
	int i = 0;

	/* Stop warnings. */
	( void ) pParam;

	tic_variables * ticVariables = NULL;
	tic_settings * ticSettings = NULL;
	tic_command * ticCommand = NULL;

	prvFunc_Print("\nTic Console...\t\t\t     Started");

	// TODO put in loop with if
	if(xQueueReceive(xQueTicVar, &ticVariables, portMAX_DELAY) == pdPASS) {
		// vQueueDelete(xQueTicVar);
		prvFunc_Print("%cTicVariables...\t\t    Received", 0x3e);
	}

	if(xQueueReceive(xQueTicSet, &ticSettings, portMAX_DELAY) == pdPASS) {
		// vQueueDelete(xQueTicSet);
		prvFunc_Print("%cTicSettings...\t\t\t    Received", 0x3e);
	}

	if(xQueueReceive(xQueTicSet, &ticSettings, portMAX_DELAY) == pdPASS) {
		// vQueueDelete(xQueTicSet);
		prvFunc_Print("%cTicCommand...\t\t\t    Received", 0x3e);
	}
	// TODO put in loop with if

	while(1) {
		i++;

		if(xSemaphoreTake(xMutexFcall,portMAX_DELAY) == pdPASS) {
			prvFunc_TicMenu(ticVariables, ticSettings);

			prvFunc_TicCommandScan(ticCommand);
		}
		xSemaphoreGive(xMutexFcall);


		vTaskDelay(100);
	}
}

static void prvFunc_Print(const char *pMessage, ...) {
	va_list args;

	va_start(args, pMessage);

	if(xSemaphoreTake(xMutexMuart,portMAX_DELAY) == pdPASS) {
		vprintf(pMessage, args);
	}

	xSemaphoreGive(xMutexMuart);

	va_end (args);
}

char * prvFunc_Scan(char * pDest) {
	char * c = pDest;
	
	if(xSemaphoreTake(xMutexMuart,portMAX_DELAY) == pdPASS) {
		// *c = getc();
		gets((unsigned char *)c);
	}

	xSemaphoreGive(xMutexMuart);

	// c++;
	// *c = '\0';

	return c;
}

void prvFunc_TicMenu(tic_variables * ticVariables, tic_settings * ticSettings) {
	char * ticErrorName = NULL;

	tic_code_shifter(ticErrorName, tic_variables_get_error_status(ticVariables));
	
	prvFunc_Print("\nTic Command Menu");
	prvFunc_Print("----------------");

	prvFunc_Print("\n[Status]");
	prvFunc_Print("Operation state:  %s\t\tDevice reset:\t  %s",
		tic_look_up_operation_state_name_ui(
			tic_variables_get_operation_state(ticVariables)),
		tic_look_up_device_reset_name_ui(
			tic_variables_get_device_reset(ticVariables)));
	prvFunc_Print("Planning mode:\t  %s\t\t\tInput state:\t  %s",
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
	
	prvFunc_Print("\n[Command]");
	prvFunc_Print("Set target [P]osition\t\t\tSet target [V]elocity");
	prvFunc_Print("Set max [S]peed\t\t\t\tSet max [A]cceleration");
	prvFunc_Print("Set step [M]ode\t\t\t\tSet current [L]imit");
	prvFunc_Print("E[X]it safe start\t\t\tE[N]ter safe start");
	prvFunc_Print("[E]nergize\t\t\t\t[D]e-energize");

	prvFunc_Print("\nChoose an available command [A,D,E,L,M,N,P,S,V,X]\n");
}

int prvFunc_TicCommandInit(tic_command **ticCommand, tic_variables *ticVariables,
	tic_settings * ticSettings) {
	tic_command * new_ticCommand = NULL;

	new_ticCommand = (tic_command *)calloc(1, sizeof(tic_command));
    if(new_ticCommand == NULL) return -1;

	new_ticCommand->change				= FALSE;
	new_ticCommand->current_limit		= ticSettings->current_limit;
	new_ticCommand->de_energize			= FALSE;
	new_ticCommand->energize			= FALSE;
	new_ticCommand->enter_safe_start	= FALSE;
	new_ticCommand->exit_safe_start		= FALSE;
	new_ticCommand->max_acceleration	= ticVariables->max_accel;
	new_ticCommand->max_position		= ticSettings->output_max;
	new_ticCommand->max_speed			= ticVariables->max_speed;
	new_ticCommand->step_mode			= ticVariables->step_mode;
	new_ticCommand->target_position		= ticVariables->target_position;
	new_ticCommand->target_velocity		= ticVariables->target_velocity;

	*ticCommand = new_ticCommand;

	new_ticCommand = NULL;

	// free(ticCommand);

	return 0;
}

tic_command * prvFunc_TicCommandScan(tic_command * ticCommand) {
	tic_command * new_ticCommand = ticCommand;
	char * charCommand = 0;
	int intCommand = 0;

	prvFunc_Scan(charCommand);

	switch(charCommand[0]) {
		case 'a':
		case 'A':
			prvFunc_Print("\nMax acceleration range [100 to 2,147,483,647]:");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= 0x64) && (intCommand <= 0x7fffffff)) {
				if((unsigned)intCommand != new_ticCommand->max_acceleration) {
					new_ticCommand->change = TRUE;
					new_ticCommand->max_acceleration = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [100 to 2,147,483,647]:");
			break;
		case 'd':
		case 'D':
			if(new_ticCommand->de_energize != TRUE) {
				new_ticCommand->change = TRUE;
				new_ticCommand->de_energize = TRUE;
			}
			break;
		case 'e':
		case 'E':
			if(new_ticCommand->energize != TRUE) {
				new_ticCommand->change = TRUE;
				new_ticCommand->energize = TRUE;
			}
			break;
		case 'l':
		case 'L':
			prvFunc_Print("\nCurrent limit range [0 to %u]:",
				TIC_MAX_ALLOWED_CURRENT_T834);
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= 0) && (intCommand <= 124)) {
				if(intCommand != new_ticCommand->current_limit) {
					new_ticCommand->change = TRUE;
					new_ticCommand->current_limit = intCommand;
				}
			}
			prvFunc_Print("\nInvalid range [0 to %u]:",
				TIC_MAX_ALLOWED_CURRENT_T834);
			break;
		case 'm':
		case 'M':
			prvFunc_Print("\nStep mode range [0 to 5]:");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= 0) && (intCommand <= 5)) {
				if(intCommand != new_ticCommand->step_mode) {
					new_ticCommand->change = TRUE;
					new_ticCommand->step_mode = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [0 to 5]:");
			break;
		case 'n':
		case 'N':
			if(new_ticCommand->enter_safe_start != TRUE) {
				new_ticCommand->change = TRUE;
				new_ticCommand->enter_safe_start = TRUE;
			}
			break;
		case 'p':
		case 'P':
			prvFunc_Print("\nTarget position range [-2,147,483,647 to 2,147,483,647]:");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= -(signed)0x7fffffff) && 
				(intCommand <= (signed)0x7fffffff)) {
				if(intCommand != new_ticCommand->target_position) {
					new_ticCommand->change = TRUE;
					new_ticCommand->target_position = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [-2,147,483,647 to 2,147,483,647]:");
			break;
		case 's':
		case 'S':
			prvFunc_Print("\nMax speed range [0 to 500,000,000]:");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= 0) && (intCommand <= 500000000)) {
				if((unsigned)intCommand != new_ticCommand->max_speed) {
					new_ticCommand->change = TRUE;
					new_ticCommand->max_speed = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [0 to 500,000,000]:");
			break;
		case 'v':
		case 'V':
			prvFunc_Print("\nTarget velocity range [-500,000,000 to 500,000,000]:");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= -500000000) && (intCommand <= 500000000)) {
				if(intCommand != new_ticCommand->target_velocity) {
					new_ticCommand->change = TRUE;
					new_ticCommand->target_velocity = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [-500,000,000 to 500,000,000]:");
			break;
		case 'x':
		case 'X':
			if(new_ticCommand->exit_safe_start != TRUE) {
				new_ticCommand->change = TRUE;
				new_ticCommand->exit_safe_start = TRUE;
			}
			break;
		default:
			prvFunc_Print("\nInvalid command");
	}

	return new_ticCommand;
}

int prvFunc_TicCommandExec(tic_command * ticCommand, tic_variables *ticVariables,
	tic_settings * ticSettings, tic_handle * ticHandle) {

	if(ticCommand->change == TRUE) {
		// TODO put a Switch
		if(ticCommand->current_limit != ticSettings->current_limit) {
			tic_set_current_limit(ticHandle, ticCommand->current_limit);
		}

		if(ticCommand->de_energize == TRUE) {
			tic_deenergize(ticHandle);
		}

		if(ticCommand->energize	== TRUE) {
			tic_energize(ticHandle);
		}

		if(ticCommand->enter_safe_start	== TRUE){
			tic_enter_safe_start(ticHandle);
		}

		if(ticCommand->exit_safe_start == TRUE) {
			tic_exit_safe_start(ticHandle);
		}

		if(ticCommand->max_acceleration != ticVariables->max_accel) {
			tic_set_max_accel(ticHandle, ticCommand->max_acceleration);
		}

		if(ticCommand->max_position != ticSettings->output_max) {
			tic_settings_set_output_max(ticSettings, ticCommand->max_position);
			tic_settings_set_output_min(ticSettings, -(ticCommand->max_position));
			tic_set_settings(ticHandle, ticSettings);
		}

		if(ticCommand->max_speed != ticVariables->max_speed) {
			tic_set_max_speed(ticHandle, ticCommand->max_speed);
		}

		if(ticCommand->step_mode != ticVariables->step_mode) {
			tic_set_step_mode(ticHandle, ticCommand->step_mode);
		}

		if(ticCommand->target_position != ticVariables->target_position){
			tic_set_target_position(ticHandle, ticCommand->target_position);
		}

		if(ticCommand->target_velocity != ticVariables->target_velocity) {
			tic_set_target_velocity(ticHandle, ticCommand->target_velocity);
		}

		return 1;
	}

	return 0;
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

// typedef tic_error * (FUNC_HANDLER)(void **pParam);

// tic_error * prvFunc_Caller(FUNC_HANDLER pfnHandler, void *pParam);

// tic_error * prvFunc_Caller(FUNC_HANDLER pfnHandler, void *pParam) {
// 	tic_error * error = NULL;
	
// 	if(xSemaphoreTake(xMutexFcall,portMAX_DELAY) == pdPASS) {
// 		error = pfnHandler(pParam);
// 	}

// 	xSemaphoreGive(xMutexFcall);

// 	return error;
// }