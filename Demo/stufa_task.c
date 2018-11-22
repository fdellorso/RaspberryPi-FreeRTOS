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


typedef struct tic_command {
	bool change;
	unsigned int max_position;
	int target_position;
	int target_velocity;
	int max_speed;
	int max_acceleration;
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

// xSemaphoreHandle	xSemTicEner		= NULL;
// xQueueHandle		xQueTicHndl		= NULL;

xSemaphoreHandle	xMutexMuart		= NULL;
xSemaphoreHandle	xMutexFcall		= NULL;


static void prvFunc_Print(const char *pMessage, ...);
char * prvFunc_Scan(char * pDest);

void prvFunc_TicMenu(tic_variables * ticVariables, tic_settings * ticSettings);
char * tic_code_shifter(char * pDest, uint16_t code);

int prvFunc_TicCommandInit(tic_command **pDest, tic_variables *ticVariables, tic_settings * ticSettings);
tic_command * prvFunc_TicCommandScan(tic_command * pDest);
void prvFunc_TicCommandExec(tic_command * pDest);


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

	vSemaphoreCreateBinary(xSemTicInit);
	if(xSemTicInit != NULL) xSemaphoreTake(xSemTicInit, 0);

	prvFunc_Print("\nTic Control...\t\t\t     Started");

	if (error == NULL) error = tic_device_create(&ticDevice);
	if (error == NULL) error = tic_handle_create(ticDevice, &ticHandle);
	prvFunc_Print("%cFirmware Version...\t\t\t%s", 0x3e, tic_get_firmware_version_string(ticHandle));

	// TODO put also in loop with if
	if (error == NULL) error = tic_get_variables(ticHandle, &ticVariables, false);
	if (error == NULL) error = tic_get_settings(ticHandle, &ticSettings);

	if(xSemaphoreGive(xSemTicInit) == pdPASS) {
		prvFunc_Print("%cTic Control...\t\t\t      Inited", 0x3e);
	}

	xQueTicVar = xQueueCreate(1, sizeof(tic_variables *));
	xQueTicSet = xQueueCreate(1, sizeof(tic_settings *));

	if(xQueueSend(xQueTicVar, &ticVariables, 0) == pdPASS) {
		prvFunc_Print("%cTicVariables...\t\t\tSent", 0x3e);
	}

	if(xQueueSend(xQueTicSet, &ticSettings, 0) == pdPASS) {
		prvFunc_Print("%cTicSettings...\t\t\t\tSent", 0x3e);
	}

	// TODO put also in loop with if


	while(1) {
		i++;

		vTaskDelay(200);
	}
}

void prvTask_TicConsole(void *pParam) {
	int i = 0;

	/* Stop warnings. */
	( void ) pParam;

	tic_variables * ticVariables = NULL;
	tic_settings * ticSettings = NULL;

	prvFunc_Print("\nTic Console...\t\t\t     Started");

	// TODO put in loop with if
	if(xQueueReceive(xQueTicVar, &ticVariables, portMAX_DELAY) == pdPASS) {
		vQueueDelete(xQueTicVar);
		prvFunc_Print("%cTicVariables...\t\t    Received", 0x3e);
	}

	if(xQueueReceive(xQueTicSet, &ticSettings, portMAX_DELAY) == pdPASS) {
		vQueueDelete(xQueTicSet);
		prvFunc_Print("%cTicSettings...\t\t\t    Received", 0x3e);
	}
	// TODO put in loop with if

	while(1) {
		i++;

		prvFunc_TicMenu(ticVariables, ticSettings);


		vTaskDelay(200);
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
		tic_look_up_operation_state_name_ui(tic_variables_get_operation_state(ticVariables)),
		tic_look_up_device_reset_name_ui(tic_variables_get_device_reset(ticVariables)));
	prvFunc_Print("Planning mode:\t  %s\t\t\tInput state:\t  %s",
		tic_look_up_planning_mode_name_ui(tic_variables_get_planning_mode(ticVariables)),
		tic_look_up_input_state_name_ui(tic_variables_get_input_state(ticVariables)));
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

int prvFunc_TicCommandInit(tic_command **pDest, tic_variables *ticVariables, tic_settings * ticSettings) {
	tic_command * ticCommand = NULL;

	ticCommand = (tic_command *)calloc(1, sizeof(tic_command));
    if(ticCommand == NULL) return -1;

	ticCommand->change				= FALSE;
	ticCommand->current_limit		= ticVariables->current_limit_code;
	ticCommand->de_energize			= FALSE;
	ticCommand->energize			= FALSE;
	ticCommand->enter_safe_start	= FALSE;
	ticCommand->exit_safe_start		= FALSE;
	ticCommand->max_acceleration	= ticVariables->max_accel;
	ticCommand->max_position		= ticSettings->output_max;
	ticCommand->max_speed			= ticVariables->max_speed;
	ticCommand->step_mode			= ticVariables->step_mode;
	ticCommand->target_position		= ticVariables->target_position;
	ticCommand->target_velocity		= ticVariables->target_velocity;

	*pDest = ticCommand;

	// ticCommand = NULL;

	// free(ticCommand);

	return 0;
}

tic_command * prvFunc_TicCommandScan(tic_command * pDest) {
	tic_command * ticCommand = pDest;
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
				if(intCommand != ticCommand->max_acceleration) {
					ticCommand->change = TRUE;
					ticCommand->max_acceleration = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [100 to 2,147,483,647]:");
			break;
		case 'd':
		case 'D':
			if(ticCommand->de_energize != TRUE) {
				ticCommand->change = TRUE;
				ticCommand->de_energize = TRUE;
			}
			break;
		case 'e':
		case 'E':
			if(ticCommand->energize != TRUE) {
				ticCommand->change = TRUE;
				ticCommand->energize = TRUE;
			}
			break;
		case 'l':
		case 'L':
			// TODO Use current code
			prvFunc_Print("\nCurrent limit range [100 to 2,147,483,647]:");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= 0) && (intCommand <= 124)) {
				if(intCommand != ticCommand->current_limit) {
					ticCommand->change = TRUE;
					ticCommand->current_limit = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [100 to 2,147,483,647]:");
			break;
		case 'm':
		case 'M':
			prvFunc_Print("\nStep mode range [0 to 5]:");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= 0) && (intCommand <= 5)) {
				if(intCommand != ticCommand->step_mode) {
					ticCommand->change = TRUE;
					ticCommand->step_mode = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [0 to 5]:");
			break;
		case 'n':
		case 'N':
			if(ticCommand->enter_safe_start != TRUE) {
				ticCommand->change = TRUE;
				ticCommand->enter_safe_start = TRUE;
			}
			break;
		case 'p':
		case 'P':
			prvFunc_Print("\nTarget position range [-2,147,483,647 to 2,147,483,647]:");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= -(signed)0x7fffffff) && (intCommand <= (signed)0x7fffffff)) {
				if(intCommand != ticCommand->target_position) {
					ticCommand->change = TRUE;
					ticCommand->target_position = intCommand;
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
				if(intCommand != ticCommand->max_speed) {
					ticCommand->change = TRUE;
					ticCommand->max_speed = intCommand;
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
				if(intCommand != ticCommand->target_velocity) {
					ticCommand->change = TRUE;
					ticCommand->target_velocity = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [-500,000,000 to 500,000,000]:");
			break;
		case 'x':
		case 'X':
			if(ticCommand->exit_safe_start != TRUE) {
				ticCommand->change = TRUE;
				ticCommand->exit_safe_start = TRUE;
			}
			break;
		default:
			prvFunc_Print("\nInvalid command");
	}

	return ticCommand;
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