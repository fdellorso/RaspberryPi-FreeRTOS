#ifndef _SHARED_RESOURCES_H_
#define _SHARED_RESOURCES_H_

// UI function
void prvFunc_Print(const char *pMessage, ...);      // Print everywhere
char * prvFunc_Scan(char * pDest);					// Scan only from UART

#endif
