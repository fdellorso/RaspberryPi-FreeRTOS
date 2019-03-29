//
// uspibind.cpp
//
// USPi - An USB driver for Raspberry Pi written in C
// Copyright (C) 2014  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <uspi/stdarg.h>
#include <uspi/string.h>
#include <uspi/synchronize.h>
#include <prvlib/stdlib.h>
#include <uspios.h>	

#include <rpi_header.h>
#include <rpi_logger.h>
#include <stufa_Task.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

void StartKernelTimer(unsigned int nDelay, PendedFunction_t pHandler, void *pContext, unsigned int nChannel);

void MsDelay(unsigned nMilliSeconds)
{
	DelaySysTimer(nMilliSeconds * 1000);
}

void usDelay(unsigned nMicroSeconds)
{
	DelaySysTimer(nMicroSeconds);
}

// extern TimerHandle_t		xDWHCITimer;
void StartKernelTimer(unsigned int nDelay, PendedFunction_t pHandler, void *pContext, unsigned int nChannel)
{
	(void) nDelay;		// FIXME Wunused

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	xTimerPendFunctionCallFromISR(pHandler, pContext, nChannel, &xHigherPriorityTaskWoken);
	
	// (void)pHandler;	// FIXME Wunused
	// (void)pContext;	// FIXME Wunused
	// (void)nChannel;	// FIXME Wunused

	// println("StartKernelTimer");
	// xTimerChangePeriodFromISR( xDWHCITimer, nDelay, NULL );
	// xTimerStartFromISR( xDWHCITimer, NULL );
}

void CancelKernelTimer(unsigned hTimer)
{
	(void)hTimer;	// FIXME Wunused

	// println("CancelKernelTimer");
}

// void ConnectInterrupt(unsigned nIRQ, FN_INTERRUPT_HANDLER pfnHandler, void *pParam)
// {
// 	DisableInterrupts();
// 	RegisterInterrupt(nIRQ, pfnHandler, pParam);
// 	EnableInterrupt(nIRQ);
// 	EnableInterrupts();
// }

int SetPowerStateOn(unsigned nDeviceId)
{
	unsigned int mailbuffer[8] __attribute__((aligned (16)));

	//set power state
	mailbuffer[0] = 8 * 4;		//mailbuffer size
	mailbuffer[1] = 0;			//response code
	mailbuffer[2] = 0x00028001;	//set power state
	mailbuffer[3] = 8;			//value buffer size
	mailbuffer[4] = 8;			//Req. + value length (bytes)
	mailbuffer[5] = nDeviceId;	//device id 3
	mailbuffer[6] = 3;			//state
	mailbuffer[7] = 0;			//terminate buffer

	//spam mail until the response code is ok
	while(mailbuffer[1] != 0x80000000){
		mailboxWrite((int)mailbuffer, 8);
		mailboxRead(8);
	}

	if(!(mailbuffer[6] & 1) || (mailbuffer[6] & 2)) return 0;
	return 1;
}

int GetMACAddress(unsigned char Buffer[6]) {
	unsigned int mailbuffer[8] __attribute__((aligned (16)));

	//get MAC
	mailbuffer[0] = 8 * 4;		//mailbuffer size
	mailbuffer[1] = 0;			//response code
	mailbuffer[2] = 0x00010003;	//get mac
	mailbuffer[3] = 6;			//value buffer size
	mailbuffer[4] = 6;			//Req. + value length (bytes)
	mailbuffer[5] = 0;			//12 34 56 AB
	mailbuffer[6] = 0;			//12 34 00 00
	mailbuffer[7] = 0;			//terminate buffer

	//spam mail until the response code is ok
	while(mailbuffer[1] != 0x80000000){
		mailboxWrite((int)mailbuffer, 8);
		mailboxRead(8);
	}

	//memcpy2(Buffer, *(&mailbuffer + 24), 6);
	Buffer[0] = (char)(mailbuffer[5] >> 0);
	Buffer[1] = (char)(mailbuffer[5] >> 8);
	Buffer[2] = (char)(mailbuffer[5] >> 16);
	Buffer[3] = (char)(mailbuffer[5] >> 24);
	Buffer[4] = (char)(mailbuffer[6] >> 0);
	Buffer[5] = (char)(mailbuffer[6] >> 8);

	return 1;
}

void LogWrite(const char *pSource, unsigned Severity, const char *pMessage, ...)
{
	(void)pSource;	// FIXME Wunused
	(void)Severity;	// FIXME Wunused

	va_list var;
	va_start (var, pMessage);

	TString Message;
	String (&Message);
	StringFormatV (&Message, pMessage, var);

	// LoggerWriteV (LoggerGet (), pSource, (TLogSeverity) Severity, pMessage, var);
	// println(StringGet (&Message));
	prvFunc_Print(StringGet (&Message));

	va_end (var);
}

#ifndef NDEBUG

void uspi_assertion_failed(const char *pExpr, const char *pFile, unsigned nLine)
{
	// println(pExpr);
	// println(pFile);
	// printHex("Line ", nLine);
	prvFunc_Print("%s %s Line 0x%x", pExpr, pFile, nLine);

	while(1){;} //system failure
}

void DebugHexdump (const void *pBuffer, unsigned nBufLen, const char *pSource)
{
	//debug_hexdump (pBuffer, nBufLen, pSource);

	(void)pBuffer;	// FIXME Wunused
	(void)nBufLen;	// FIXME Wunused
	(void)pSource;	// FIXME Wunused

	println("DebugHexdump");
}

#endif

void* malloc(unsigned nSize)
{
	uspi_EnterCritical();
	void* temp = pvPortMalloc(nSize);
	uspi_LeaveCritical();
	return temp;
}

void free(void* pBlock)
{
	uspi_EnterCritical();
	vPortFree(pBlock);
	uspi_LeaveCritical();
}
