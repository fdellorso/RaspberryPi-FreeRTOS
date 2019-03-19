// drv8825_Task.c
// authored by Francesco Dell'Orso
//
// tasks for StuFA Project

#include "stufa_Task.h"

#include <gpio.h>
#include <interrupts.h>

// Internal function Declaration
void vStepISR(int nIRQ, void *pParam);

int step;

void prvTask_Drv8825Control(void *pParam) {
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

	// taskENTER_CRITICAL();
	// prvSystemTimerStepSetup();
	// RegisterInterrupt(BCM2835_IRQ_ID_ST_C3, vStepISR, NULL);
	// EnableInterrupt(BCM2835_IRQ_ID_ST_C3);
	// taskEXIT_CRITICAL();

	prvFunc_Print("\nDrv8825 Control...\t\t     Started");

	while(1) {
		i++;

		// prvFunc_Print("Step nr. %u", step);
		// prvFunc_Print("Drv8825 nr. %d", i);

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
