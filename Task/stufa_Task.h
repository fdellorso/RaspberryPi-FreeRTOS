#ifndef _STUFA_TASK_H_
#define _STUFA_TASK_H_

#include <uspi/stdarg.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include <uspi.h>
#include <tic.h>
#include <rsta_bt/btsubsystem.h>


tic_error * prvFunc_TicMotorInit(tic_handle * ticHandle);

char * tic_code_shifter(char * pDest, uint16_t code);	// Explode error code
void prvFunc_TicMenu(tic_variables * ticVariables, tic_settings * ticSettings);

// Struct to pass command from Console to Control
typedef struct tic_command {
	char command;
	char command_old;
	int value;
} tic_command;

// Command function
int prvFunc_TicCommandInit(tic_command **ticCommand);
void prvFunc_TicCommandScan(tic_command * ticCommand, tic_variables * ticVariables);
tic_error * prvFunc_TicCommandExec(tic_command * ticCommand, tic_settings * ticSettings,
	tic_handle * ticHandle);

// UI function
void prvFunc_Print(const char *pMessage, ...);      // Print everywhere
char * prvFunc_Scan(char * pDest);					// Scan only from UART

// Tasks
void prvTask_WatchDog(void *pParam);
void prvTask_UspiInitialize(void *pParam);
void prvTask_TicControl(void *pParam);
void prvTask_TicConsole(void *pParam);
void prvTask_Drv8825Control(void *pParam);
void prvTask_BluetoothInitialize(void *pParam);
void prvTask_BluetoothProcess(void *pParam);

#endif
