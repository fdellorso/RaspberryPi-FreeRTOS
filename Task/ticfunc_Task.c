// tic_taskfunc.c
// authored by Francesco Dell'Orso
//
// Tic Function for Tasks for StuFA Project

#include "stufa_Task.h"

#include <prvlib/stdlib.h>
#include <uspi/util.h>

#ifndef TIC_SPEED
#define TIC_SPEED	700		// Pulses/s
#endif

extern const TickType_t		xBlockTime;
extern SemaphoreHandle_t	xMutexRunning;

tic_error * prvFunc_TicMotorInit(tic_handle * ticHandle) {
	tic_error * error = NULL;

	// if (error == NULL) error = tic_reinitialize(ticHandle);
	if (error == NULL) error = tic_reset(ticHandle);
	if (error == NULL) error = tic_clear_driver_error(ticHandle);
	if (error == NULL) error = tic_energize(ticHandle);
	if (error == NULL) error = tic_set_step_mode(ticHandle, TIC_STEP_MODE_FULL);
	if (error == NULL) error = tic_set_max_speed(ticHandle, TIC_SPEED * 10000);
	// if (error == NULL) error = tic_halt_and_set_position(ticHandle, -1200);
	if (error == NULL) error = tic_enter_safe_start(ticHandle);

	return error;
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

void prvFunc_TicMenu(tic_variables * ticVariables, tic_settings * ticSettings) {
	// char ticErrorName[180];				// used malloc to reduce stack usage
	// ticErrorName[0] = '\0';

	char * ticErrorName = (char *) malloc(sizeof(char) * 180);

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
	// prvFunc_Print("Acting target:\t  %d",
	// 	tic_variables_get_acting_target_position(ticVariables));
	prvFunc_Print("Current limit:\t  %u mA\t\tStep mode:\t  %s",
		tic_variables_get_current_limit(ticVariables),
		tic_look_up_step_mode_name_ui(tic_variables_get_step_mode(ticVariables)));
	prvFunc_Print("Max speed:\t  %u pulses/s\t\tMax acceleration: %u pulses/s",
		tic_variables_get_max_speed(ticVariables)/10000,
		tic_variables_get_max_accel(ticVariables)/100);
	prvFunc_Print("Target minimum:\t  %d\t\t\tTarget maximum:\t  %d",
		tic_settings_get_output_min(ticSettings),
		tic_settings_get_output_max(ticSettings));
	prvFunc_Print("Target position:  %d\t\t\tTarget velocity:  %d",
		tic_variables_get_target_position(ticVariables),
		tic_variables_get_target_velocity(ticVariables));
	prvFunc_Print("Current position: %d\t\t\tCurrent velocity: %d pulses/s",
		tic_variables_get_current_position(ticVariables),
		tic_variables_get_current_velocity(ticVariables)/10000);
	prvFunc_Print("Error status:\t  %s", ticErrorName);

	free(ticErrorName);
	
	// FIXME find better filter
	// if((tic_variables_get_error_status(ticVariables) & (1 << TIC_ERROR_SAFE_START_VIOLATION))) {
	if(xSemaphoreTake(xMutexRunning, xBlockTime) == pdPASS) {	
		prvFunc_Print("\n[Command]");
		prvFunc_Print("Set target [P]osition\t\t\tSet target [V]elocity");
		prvFunc_Print("Set max [S]peed\t\t\t\tSet max [A]cceleration");
		prvFunc_Print("Set step [M]ode\t\t\t\tSet current [L]imit");
		prvFunc_Print("E[X]it safe start\t\t\tE[N]ter safe start");
		prvFunc_Print("[E]nergize\t\t\t\t[D]e-energize");

		prvFunc_Print("\nChoose an available command [A,D,E,L,M,N,P,S,V,X]: ");
		
		xSemaphoreGive(xMutexRunning);	
	}
}

// Command function
int prvFunc_TicCommandInit(tic_command **ticCommand) {
	tic_command * new_ticCommand = NULL;

	new_ticCommand = (tic_command *) calloc(1, sizeof(tic_command));
    if(new_ticCommand == NULL) return -1;

	new_ticCommand->command		= '0';
	new_ticCommand->command_old	= '0';
	new_ticCommand->value		= 0;

	*ticCommand = new_ticCommand;

	new_ticCommand = NULL;

	// free(ticCommand);

	return 0;
}

void prvFunc_TicCommandScan(tic_command * ticCommand, tic_variables * ticVariables) {
	tic_command * new_ticCommand = ticCommand;
	char * charCommand = 0;
	int intCommand = 0;

	prvFunc_Scan(charCommand);

	switch(charCommand[0]) {
		case 'a':
		case 'A':
			prvFunc_Print("\nMax acceleration range [1 to 2,147,483]: ");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand) * 100;
			if((intCommand >= 0x64) && (intCommand <= 0x7fffffff)) {
				if((unsigned)intCommand != tic_variables_get_max_accel(ticVariables)) {
					new_ticCommand->command = 'a';
					new_ticCommand->value = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [1 to 2,147,483]");
			break;
		case 'd':
		case 'D':
			if(tic_variables_get_energized(ticVariables)) {
				new_ticCommand->command = 'd';
				new_ticCommand->value = 0;
			}
			else prvFunc_Print("\nAlready De-energized");
			break;
		case 'e':
		case 'E':
			if(!(tic_variables_get_energized(ticVariables))) {
				new_ticCommand->command = 'e';
				new_ticCommand->value = 0;
			}
			else prvFunc_Print("\nAlready Energized");
			break;
		case 'l':
		case 'L':
			prvFunc_Print("\nCurrent limit range [0 to %u]: ",
				TIC_MAX_ALLOWED_CURRENT_T834);
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= 0) && (intCommand <= 124)) {
				if((unsigned)intCommand != tic_variables_get_current_limit(ticVariables)) {
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
				if((unsigned)intCommand != tic_variables_get_step_mode(ticVariables)) {
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
		case 'o':
		case 'O':
			prvFunc_Print("\nTarget min/max range [0 to 2,147,483,647]: ");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= 0) && (intCommand <= 0x7fffffff)) {
				new_ticCommand->command = 'o';
				new_ticCommand->value = intCommand;
			}
			else prvFunc_Print("\nInvalid range [0 to 2,147,483,647]");
			break;
		case 'p':
		case 'P':
			prvFunc_Print("\nTarget position range [-2,147,483,647 to 2,147,483,647]: ");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand);
			if((intCommand >= -(signed)0x7fffffff) && 
				(intCommand <= (signed)0x7fffffff)) {
				if(intCommand != tic_variables_get_current_position(ticVariables)) {
					new_ticCommand->command = 'p';
					new_ticCommand->value = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [-2,147,483,647 to 2,147,483,647]");
			break;
		case 's':
		case 'S':
			prvFunc_Print("\nMax speed range [0 to 50,000]: ");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand) * 10000;
			if((intCommand >= 0) && (intCommand <= 500000000)) {
				if((unsigned)intCommand != tic_variables_get_max_speed(ticVariables)) {
					new_ticCommand->command = 's';
					new_ticCommand->value = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [0 to 50,000]");
			break;
		case 'v':
		case 'V':
			prvFunc_Print("\nTarget velocity range [-50,000 to 50,000]: ");
			prvFunc_Scan(charCommand);
			intCommand = atoi(charCommand) * 10000;
			if((intCommand >= -500000000) && (intCommand <= 500000000)) {
				if(intCommand != tic_variables_get_target_velocity(ticVariables)) {
					new_ticCommand->command = 'v';
					new_ticCommand->value = intCommand;
				}
			}
			else prvFunc_Print("\nInvalid range [-50,000 to 50,000]");
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