#ifndef _TIC_TASKFUNC_H_
#define _TIC_TASKFUNC_H_

#include <tic.h>

#ifndef TIC_SPEED
#define TIC_SPEED	700		// Pulses/s
#endif

// Struct to pass command from Console to Control
typedef struct tic_command {
	char command;
	char command_old;
	int value;
} tic_command;

tic_error * prvFunc_TicMotorInit(tic_handle * ticHandle);

void prvFunc_TicMenu(tic_variables * ticVariables, tic_settings * ticSettings);
char * tic_code_shifter(char * pDest, uint16_t code);	// Explode error code

// Command function
int prvFunc_TicCommandInit(tic_command **ticCommand);
void prvFunc_TicCommandScan(tic_command * ticCommand, tic_variables * ticVariables);
tic_error * prvFunc_TicCommandExec(tic_command * ticCommand, tic_settings * ticSettings,
	tic_handle * ticHandle);

#endif
