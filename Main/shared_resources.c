//shared_resources.c
//authored by Francesco Dell'Orso
//
//Shared Resources for StuFA Project

#include "shared_resources.h"
#include <rpi_logger.h>

#include <uspi/stdarg.h>

#include <FreeRTOS.h>
#include <semphr.h>

extern xSemaphoreHandle	xMutexMuart;

// UI Function
void prvFunc_Print(const char *pMessage, ...) {
	va_list args;

	va_start(args, pMessage);

	if(xSemaphoreTake(xMutexMuart, portMAX_DELAY) == pdPASS) {
		vprintf(pMessage, args);
		xSemaphoreGive(xMutexMuart);
	}

	va_end (args);
}

char * prvFunc_Scan(char * pDest) {
	char * c = pDest;
	
	if(xSemaphoreTake(xMutexMuart, portMAX_DELAY) == pdPASS) {
		gets((unsigned char *)c);
		xSemaphoreGive(xMutexMuart);
	}

	return c;
}
