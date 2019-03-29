/*
 *	Raspberry Pi Porting layer for FreeRTOS.
 */

#include "FreeRTOS.h"
#include "task.h"
#include "interrupts.h"
#include "sys_timer.h"

/* Constants required to setup the task context. */
#define portINITIAL_SPSR				( ( StackType_t ) 0x1f ) /* System mode, ARM mode, interrupts enabled. */
#define portTHUMB_MODE_BIT				( ( StackType_t ) 0x20 )
#define portINSTRUCTION_SIZE			( ( StackType_t ) 4 )

/* Constants required to setup the tick ISR. */
// #define portTIMER_PRESCALE 				( ( StackType_t ) 0xF9 )
// #define portPRESCALE_VALUE				0x00

/* Constants required to handle critical sections. */
#define portNO_CRITICAL_NESTING		( ( uint32_t ) 0 )
volatile uint32_t ulCriticalNesting = 9999UL;

/*-----------------------------------------------------------*/

/* Setup the timer to generate the tick interrupts. */
static void prvSetupTimerInterrupt( void );
void vTickISR(int nIRQ, void *pParam );

/* 
 * The scheduler can only be started from ARM mode, so 
 * vPortISRStartFirstSTask() is defined in portISR.c. 
 */
// extern void vPortISRStartFirstTask( void );

/*-----------------------------------------------------------*/

void ConnectInterrupt(int nIRQ, FN_INTERRUPT_HANDLER pfnHandler, void *pParamIRQ,
					  FN_INTERRUPT_SETUP pfnSetup, uint32_t pParamSetup, int bTask);


/* 
 * Initialise the stack of a task to look exactly as if a call to 
 * portSAVE_CONTEXT had been called.
 *
 * See header file for description. 
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
StackType_t *pxOriginalTOS;

	pxOriginalTOS = pxTopOfStack;

	/* To ensure asserts in tasks.c don't fail, although in this case the assert
	is not really required. */
	pxTopOfStack--;

	/* Setup the initial stack of the task.  The stack is set exactly as 
	expected by the portRESTORE_CONTEXT() macro. */

	/* First on the stack is the return address - which in this case is the
	start of the task.  The offset is added to make the return address appear
	as it would within an IRQ ISR. */
	*pxTopOfStack = ( StackType_t ) pxCode + portINSTRUCTION_SIZE;		
	pxTopOfStack--;

	*pxTopOfStack = ( StackType_t ) 0xaaaaaaaa;	/* R14 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) pxOriginalTOS; /* Stack used when task starts goes in R13. */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x12121212;	/* R12 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x11111111;	/* R11 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x10101010;	/* R10 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x09090909;	/* R9 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x08080808;	/* R8 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x07070707;	/* R7 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x06060606;	/* R6 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x05050505;	/* R5 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x00000000;	/* R4  Must be zero see the align 8 issue in portSaveContext */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x03030303;	/* R3 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x02020202;	/* R2 */
	pxTopOfStack--;	
	*pxTopOfStack = ( StackType_t ) 0x01010101;	/* R1 */
	pxTopOfStack--;	

	/* When the task starts is will expect to find the function parameter in
	R0. */
	*pxTopOfStack = ( StackType_t ) pvParameters; /* R0 */
	pxTopOfStack--;

	/* The last thing onto the stack is the status register, which is set for
	system mode, with interrupts enabled. */
	*pxTopOfStack = ( StackType_t ) portINITIAL_SPSR;

	if( ( ( uint32_t ) pxCode & 0x01UL ) != 0x00 )
	{
		/* We want the task to start in thumb mode. */
		*pxTopOfStack |= portTHUMB_MODE_BIT;
	}

	pxTopOfStack--;

	/* Some optimisation levels use the stack differently to others.  This 
	means the interrupt flags cannot always be stored on the stack and will
	instead be stored in a variable, which is then saved as part of the
	tasks context. */
	*pxTopOfStack = portNO_CRITICAL_NESTING;

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

extern void restore_context (void);
BaseType_t xPortStartScheduler( void )
{
	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	prvSetupTimerInterrupt();

	/* Start the first task. */
	// vPortISRStartFirstTask();
	restore_context();

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the ARM port will require this function as there
	is nothing to return to.  */
}
/*-----------------------------------------------------------*/

/*
 *	This is the TICK interrupt service routine, note. no SAVE/RESTORE_CONTEXT here
 *	as thats done in the bottom-half of the ISR.
 *
 *	See bt_interrupts.c in the RaspberryPi Drivers folder.
 */
void vTickISR(int nIRQ, void *pParam)
{
	(void)nIRQ;		// FIXME Wunused
	(void)pParam;	// FIXME Wunused

	xTaskIncrementTick();
	#if configUSE_PREEMPTION == 1
	vTaskSwitchContext();
	#endif
	prvSystemTimerTickClear(configTICK_RATE_HZ);
}

/*
 * Setup the timer 0 to generate the tick interrupts at the required frequency.
 */
static void prvSetupTimerInterrupt( void )
{
	// DisableInterrupts();
	// prvSystemTimerTickSetup(configTICK_RATE_HZ);
	// RegisterInterrupt(BCM2835_IRQ_ID_ST_C1, vTickISR, NULL);
	// EnableInterrupt(BCM2835_IRQ_ID_ST_C1);
	// EnableInterrupts();

	ConnectInterrupt(BCM2835_IRQ_ID_ST_C1, vTickISR, NULL,
					 prvSystemTimerTickSetup, configTICK_RATE_HZ, 0);
}
/*-----------------------------------------------------------*/

// static volatile uint32_t s_bWereEnabled;		// BUG Smette di funzionare il timer se ON
/* The code generated by the GCC compiler uses the stack in different ways at
different optimisation levels.  The interrupt flags can therefore not always
be saved to the stack.  Instead the critical section nesting level is stored
in a variable, which is then saved as part of the stack context. */
void vPortEnterCritical( void )
{
	// int nFlags;
	// __asm volatile ("mrs %0, cpsr" : "=r" (nFlags));
	// s_bWereEnabled = nFlags & 0x80 ? 0 : 1; 
	// if(!s_bWereEnabled) return;

	/* Disable interrupts as per portDISABLE_INTERRUPTS(); 							*/
	__asm volatile ( 
		"STMDB	SP!, {R0}			\n\t"	/* Push R0.							*/
		"MRS	R0, CPSR			\n\t"	/* Get CPSR.						*/
		"ORR	R0, R0, #0xC0		\n\t"	/* Disable IRQ, FIQ.				*/
		"MSR	CPSR, R0			\n\t"	/* Write back modified value.		*/
		"LDMIA	SP!, {R0}" );				/* Pop R0.							*/

	/* Now interrupts are disabled ulCriticalNesting can be accessed 
	directly.  Increment ulCriticalNesting to keep a count of how many times
	portENTER_CRITICAL() has been called. */
	ulCriticalNesting++;
}

void vPortExitCritical( void )
{
	// if(s_bWereEnabled) return;

	if( ulCriticalNesting > portNO_CRITICAL_NESTING )
	{
		/* Decrement the nesting count as we are leaving a critical section.	*/
		ulCriticalNesting--;

		/* If the nesting level has reached zero then interrupts should be
		re-enabled. */
		if( ulCriticalNesting == portNO_CRITICAL_NESTING )
		{
			/* Enable interrupts as per portEXIT_CRITICAL().					*/
			__asm volatile ( 
				"STMDB	SP!, {R0}		\n\t"	/* Push R0.						*/	
				"MRS	R0, CPSR		\n\t"	/* Get CPSR.					*/	
				"BIC	R0, R0, #0xC0	\n\t"	/* Enable IRQ, FIQ.				*/	
				"MSR	CPSR, R0		\n\t"	/* Write back modified value.	*/	
				"LDMIA	SP!, {R0}" );			/* Pop R0.						*/
		}
	}
}

void ConnectInterrupt(int nIRQ, FN_INTERRUPT_HANDLER pfnHandler, void *pParamIRQ,
					  FN_INTERRUPT_SETUP pfnSetup, uint32_t pParamSetup, int bTask)
{
	(void) bTask;
	
	// if (bTask) vPortEnterCritical();
	// else DisableInterrupts();
	DisableInterrupts();

	if (pfnSetup != voidSetupFN) pfnSetup(pParamSetup);

	RegisterInterrupt(nIRQ, pfnHandler, pParamIRQ);
	EnableInterrupt(nIRQ);

	// if (bTask) vPortExitCritical();
	// else EnableInterrupts();
	EnableInterrupts();
}
