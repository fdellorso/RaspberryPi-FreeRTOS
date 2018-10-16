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
#include <uspios.h>
#include <uspi/stdarg.h>
#include <uspi/string.h>
#include <FreeRTOS.h>
#include <task.h>

#include "sys_timer.h"
#include "mailbox.h"
#include "mem.h"
#include "video.h"
#include "ili9340.h"

__attribute__((no_instrument_function))
void MsDelay(unsigned nMilliSeconds) {
	/* volatile int* timeStamp = (int*)0x20003004;
	int stop = *timeStamp + nMilliSeconds * 1000;
	while (*timeStamp < stop) __asm__("nop"); */
	//vTaskDelay(nMilliSeconds);

	DelaySysTimer(nMilliSeconds * 1000);
}

__attribute__((no_instrument_function))
void usDelay(unsigned nMicroSeconds) {
	/* volatile int* timeStamp = (int*)0x20003004;
	int stop = *timeStamp + nMicroSeconds;
	while (*timeStamp < stop) __asm__("nop"); */

	DelaySysTimer(nMicroSeconds);
}

unsigned StartKernelTimer(unsigned nDelay, TKernelTimerHandler *pHandler, void *pParam, void *pContext) {
	//TimerStartKernelTimer (TimerGet (), nDelay, pHandler, pParam, pContext);
	println("StartKernelTimer", 0xFFFFFFFF);
	ili9340_println("StartKernelTimer", ILI9340_WHITE);
	return 1;
}

void CancelKernelTimer(unsigned hTimer) {
	//TimerCancelKernelTimer (TimerGet (), hTimer);
	println("CancelKernelTimer", 0xFFFFFFFF);
	ili9340_println("CancelKernelTimer", ILI9340_WHITE);
}

//void ConnectInterrupt (unsigned nIRQ, TInterruptHandler *pfnHandler, void *pParam){
void ConnectInterrupt(unsigned nIRQ, FN_INTERRUPT_HANDLER pfnHandler, void *pParam) {
	//DisableInterrupts();
	RegisterInterrupt(nIRQ, pfnHandler, pParam);
	EnableInterrupt(nIRQ);
	//EnableInterrupts();
}

int SetPowerStateOn(unsigned nDeviceId) {
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
	unsigned int mailbuffer[7] __attribute__((aligned (16)));

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

__attribute__((no_instrument_function))
void LogWrite(const char *pSource, unsigned Severity, const char *pMessage, ...) {
	va_list var;
	va_start (var, pMessage);

	TString Message;
	String (&Message);
	StringFormatV (&Message, pMessage, var);

	// LoggerWriteV (LoggerGet (), pSource, (TLogSeverity) Severity, pMessage, var);
	println(StringGet (&Message), 0xFFFFFFFF);
	ili9340_println(StringGet (&Message), ILI9340_WHITE);

	va_end (var);
	// println(pMessage, 0xFFFFFFFF);
	// ili9340_println(pMessage, ILI9340_WHITE);
}

#ifndef NDEBUG

void uspi_assertion_failed(const char *pExpr, const char *pFile, unsigned nLine) {	
	println(pExpr, 0xFFFFFFFF);
	println(pFile, 0xFFFFFFFF);
	printHex("Line ", nLine, 0xFFFFFFFF);
	ili9340_println(pExpr, ILI9340_WHITE);
	ili9340_println(pFile, ILI9340_WHITE);
	ili9340_printHex("Line ", nLine, ILI9340_WHITE);
	while(1){;} //system failure
}

void DebugHexdump (const void *pBuffer, unsigned nBufLen, const char *pSource) {
	//debug_hexdump (pBuffer, nBufLen, pSource);
	println("DebugHexdump", 0xFFFFFFFF);
	ili9340_println("DebugHexdump", ILI9340_WHITE);
}

#endif

void* malloc(unsigned nSize) {
	uspi_EnterCritical();
	//if(loaded == 2) println("malloc", 0xFFFFFFFF);
	void* temp = pvPortMalloc(nSize);
	uspi_LeaveCritical();
	return temp;

}

void free(void* pBlock) {
	uspi_EnterCritical();
	vPortFree(pBlock);
	uspi_LeaveCritical();
}
