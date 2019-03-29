// shared_Task.c
// authored by Francesco Dell'Orso
//
// Shared Resources for StuFA Project

#include "stufa_Task.h"

#include <rpi_logger.h>
#include <uspi/synchronize.h>

extern xSemaphoreHandle	xMutexMuart;

// UI Function
void prvFunc_Print(const char *pMessage, ...) {
	va_list args;

	va_start(args, pMessage);

	if(xSemaphoreTake(xMutexMuart, portMAX_DELAY) == pdPASS) {
		uspi_EnterCritical();
		vprintf(pMessage, args);
		uspi_LeaveCritical();
		xSemaphoreGive(xMutexMuart);
	}

	va_end (args);
}

#ifdef MUART
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
#else
char * prvFunc_Scan(char * pDest) {
	char * c = pDest;

	c = "NoMUART";

	return c;
}
#endif