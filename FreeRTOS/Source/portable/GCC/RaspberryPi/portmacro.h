#ifndef PORTMACRO_H
#define PORTMACRO_H

#include <uspi/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.  
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	uint32_t
#define portBASE_TYPE	portLONG

typedef portSTACK_TYPE StackType_t;
typedef portLONG BaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
#error "The Raspberry Pi port does not support 16 bit timer ticks"
#else
typedef uint32_t UBaseType_t;
typedef uint32_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#define portBYTE_ALIGNMENT			8

/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
not need to be guarded with a critical section. */
#define portTICK_TYPE_IS_ATOMIC 1
#endif
/*-----------------------------------------------------------*/	

/* Architecture specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portNOP()					__asm volatile ( "NOP" );
/*-----------------------------------------------------------*/	

// extern void vTaskSwitchContext( void );
// #define portYIELD_FROM_ISR()		vTaskSwitchContext()
#define portYIELD()					__asm volatile ( "SWI 0" )
/*-----------------------------------------------------------*/

/* Critical section management. */

/*
 * The interrupt management utilities can only be called from ARM mode.  When
 * THUMB_INTERWORK is defined the utilities are defined as functions in 
 * portISR.c to ensure a switch to ARM mode.  When THUMB_INTERWORK is not 
 * defined then the utilities are defined as macros here - as per other ports.
 */

#ifdef THUMB_INTERWORK

	extern void vPortDisableInterruptsFromThumb( void ) __attribute__ ((naked));
	extern void vPortEnableInterruptsFromThumb( void ) __attribute__ ((naked));

	#define portDISABLE_INTERRUPTS()	vPortDisableInterruptsFromThumb()
	#define portENABLE_INTERRUPTS()		vPortEnableInterruptsFromThumb()
	
#else

	#define portDISABLE_INTERRUPTS()												\
		__asm volatile (															\
			"STMDB	SP!, {R0}		\n\t"		/* Push R0.						*/	\
			"MRS	R0, CPSR		\n\t"		/* Get CPSR.					*/	\
			"ORR	R0, R0, #0xC0	\n\t"		/* Disable IRQ, FIQ.			*/	\
			"MSR	CPSR, R0		\n\t"		/* Write back modified value.	*/	\
			"LDMIA	SP!, {R0}			" )		/* Pop R0.						*/
			
	#define portENABLE_INTERRUPTS()													\
		__asm volatile (															\
			"STMDB	SP!, {R0}		\n\t"		/* Push R0.						*/	\
			"MRS	R0, CPSR		\n\t"		/* Get CPSR.					*/	\
			"BIC	R0, R0, #0xC0	\n\t"		/* Enable IRQ, FIQ.				*/	\
			"MSR	CPSR, R0		\n\t"		/* Write back modified value.	*/	\
			"LDMIA	SP!, {R0}			" )		/* Pop R0.						*/

#endif /* THUMB_INTERWORK */

extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );

#define portENTER_CRITICAL()	vPortEnterCritical();
#define portEXIT_CRITICAL()		vPortExitCritical();
/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters )	void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters )		void vFunction( void *pvParameters )

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
