// shared_Task.c
// authored by Francesco Dell'Orso
//
// Shared Resources for StuFA Project

#include "stufa_Task.h"

#include <rpi_logger.h>
#include <uspi/synchronize.h>
#include <uspi/string.h>

extern QueueHandle_t		xQuePrintPool;
extern SemaphoreHandle_t	xMutexMuart;

void prvTask_PrintPool(void *pParam) 
{
	int i = 0;

	(void) pParam;

	char *pcMessageToPrint; 
 
    /* This is the only task that is allowed to write to standard out.  Any other  
    task wanting to write a string to the output does not access standard out  
    directly, but instead sends the string to this task.  As only this task accesses  
    standard out there are no mutual exclusion or serialization issues to consider  
    within the implementation of the task itself. */ 
    while(1) {
		i++;

        /* Wait for a message to arrive.  An indefinite block time is specified so  
        there is no need to check the return value – the function will only return  
        when a message has been successfully received. */ 
        xQueueReceive( xQuePrintPool, &pcMessageToPrint, portMAX_DELAY ); 
 
        /* Output the received string. */ 
        uspi_EnterCritical();
		println(pcMessageToPrint);
		uspi_LeaveCritical(); 
 
        /* Loop back to wait for the next message. */ 
    } 
} 

// Print Function
void prvFunc_Print(const char *pMessage, ...) {
	va_list args;

	va_start(args, pMessage);

	TString Message;
	String(&Message);
	StringFormatV(&Message, pMessage, args);

	xQueueSendToBackFromISR( xQuePrintPool, StringGet(&Message), NULL );

	_String(&Message);

	va_end (args);
}

char * prvFunc_Scan(char * pDest) {
	char * c = pDest;
	
	if(xSemaphoreTake(xMutexMuart, portMAX_DELAY) == pdPASS) {
		uspi_EnterCritical();
		gets((unsigned char *)c);
		uspi_LeaveCritical();
		xSemaphoreGive(xMutexMuart);
	}

	return c;
}
