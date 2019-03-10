//
// btuarttransport.c
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2016  R. Stange <rsta2@o2online.de>
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
#include <rsta_bt/btuarttransport.h>
#include <uspi/devicenameservice.h>
#include <uspi/bcm2835.h>
#include <uspi/synchronize.h>
#include <uspi/string.h>
#include <uspi/assert.h>
// #include <circle/memio.h>
// #include <circle/machineinfo.h>
// #include <circle/logger.h>
#include <FreeRTOS.h>
#include <task.h>
#include <gpio.h>
#include <uart0.h>

enum TBTUARTRxState
{
	RxStateStart,
	RxStateCommand,
	RxStateLength,
	RxStateParam,
	RxStateUnknown
};

#define HCI_PACKET_COMMAND		0x01
#define HCI_PACKET_ACL_DATA		0x02
#define HCI_PACKET_SYNCH_DATA	0x03
#define HCI_PACKET_EVENT		0x04

static const char FromBTUART[] = "btuart";

static unsigned s_nDeviceNumber = 1;

void BTUARTTransportWrite (u8 nChar);
void BTUARTTransportIRQHandler (TBTUARTTransport *pThis);
static void BTUARTTransportIRQStub (int nIRQ, void *pParam);

void BTUARTTransport (TBTUARTTransport *pThis) //, CInterruptSystem *pInterruptSystem)
{
	// to be sure there is no collision with the UART GPIO interface
	SetGpioFunction(14, GPIO_FUNC_INPUT);
	SetGpioFunction(15, GPIO_FUNC_INPUT);
	SetGpioFunction(32, GPIO_FUNC_ALT_3);
	SetGpioFunction(33, GPIO_FUNC_ALT_3);
	// pThis->m_pInterruptSystem (pInterruptSystem);		// TODO
	pThis->m_bIRQConnected	= FALSE;
	pThis->m_pEventHandler	= 0;
	pThis->m_nRxState		= RxStateStart;
}

void _BTUARTTransport (TBTUARTTransport *pThis)
{
	DataSyncBarrier ();
	// write32 (ARM_UART0_IMSC, 0);
	// write32 (ARM_UART0_CR, 0);
	BTUARTClose();
	DataMemBarrier ();

	pThis->m_pEventHandler = 0;

	if (pThis->m_bIRQConnected)
	{
		// assert (pThis->m_pInterruptSystem != 0);
		// pThis->m_pInterruptSystem->DisconnectIRQ (ARM_IRQ_UART);	// TODO
		taskENTER_CRITICAL();
		DisableInterrupt(BCM2835_IRQ_ID_UART);
		taskEXIT_CRITICAL();
	}

	// pThis->m_pInterruptSystem = 0;
}

boolean BTUARTTransportInitialize (TBTUARTTransport *pThis, unsigned nBaudrate)
{
	// unsigned nClockRate = CMachineInfo::Get ()->GetClockRate (CLOCK_ID_UART);
	unsigned nClockRate = GetClockRate(0); // CLOCK_ID_UART)	// TODO
	assert (nClockRate > 0);

	if(nBaudrate == 0) nBaudrate = 115200;

	assert (300 <= nBaudrate && nBaudrate <= 3000000);
	unsigned nBaud16 = nBaudrate * 16;
	unsigned nIntDiv = nClockRate / nBaud16;
	assert (1 <= nIntDiv && nIntDiv <= 0xFFFF);
	unsigned nFractDiv2 = (nClockRate % nBaud16) * 8 / nBaudrate;
	unsigned nFractDiv = nFractDiv2 / 2 + nFractDiv2 % 2;
	assert (nFractDiv <= 0x3F);

	// assert (pThis->m_pInterruptSystem != 0);
	// pThis->m_pInterruptSystem->ConnectIRQ (ARM_IRQ_UART, IRQStub, this);			// TODO
	taskENTER_CRITICAL();		// FIXME Possible to move after Init
	RegisterInterrupt(BCM2835_IRQ_ID_UART, BTUARTTransportIRQStub, pThis);
	EnableInterrupt(BCM2835_IRQ_ID_UART);
	taskEXIT_CRITICAL();
	pThis->m_bIRQConnected = TRUE;

	DataSyncBarrier ();

	// write32 (ARM_UART0_IMSC, 0);
	// write32 (ARM_UART0_ICR,  0x7FF);
	// write32 (ARM_UART0_IBRD, nIntDiv);
	// write32 (ARM_UART0_FBRD, nFractDiv);
	// write32 (ARM_UART0_IFLS, IFLS_IFSEL_1_4 << IFLS_RXIFSEL_SHIFT);
	// write32 (ARM_UART0_LCRH, LCRH_WLEN8_MASK | LCRH_FEN_MASK);		// 8N1
	// write32 (ARM_UART0_CR,   CR_UART_EN_MASK | CR_TXE_MASK | CR_RXE_MASK);
	// write32 (ARM_UART0_IMSC, INT_RX | INT_RT | INT_OE);

	BTUARTInit(nIntDiv, nFractDiv);

	DataMemBarrier ();

	// CDeviceNameService::Get ()->AddDevice ("ttyBT1", this, FALSE);
	
	//DNS ADD
	TString DeviceName;
	String (&DeviceName);
	StringFormat (&DeviceName, "ttyBT%u", s_nDeviceNumber++);
	DeviceNameServiceAddDevice (DeviceNameServiceGet (), StringGet (&DeviceName), pThis, FALSE);

	_String (&DeviceName);

	return TRUE;
}

boolean BTUARTTransportSendHCICommand (TBTUARTTransport *pThis, const void *pBuffer, unsigned nLength)
{
	(void) pThis;
	
	DataSyncBarrier ();

	BTUARTTransportWrite (HCI_PACKET_COMMAND);

	u8 *pChar = (u8 *) pBuffer;
	assert (pChar != 0);
	
	while (nLength--)
	{
		BTUARTTransportWrite (*pChar++);
	}

	DataMemBarrier ();

	return TRUE;
}

void BTUARTTransportRegisterHCIEventHandler (TBTUARTTransport *pThis, TBTHCIEventHandler *pHandler)
{
	assert (pThis->m_pEventHandler == 0);
	pThis->m_pEventHandler = pHandler;
	assert (pThis->m_pEventHandler != 0);
}

void BTUARTTransportWrite (u8 nChar)
{
	// while (read32 (ARM_UART0_FR) & FR_TXFF_MASK)
	// {
	// 	// do nothing
	// }
		
	// write32 (ARM_UART0_DR, nChar);

	BTUARTWrite(nChar);
}

void BTUARTTransportIRQHandler (TBTUARTTransport *pThis)
{
	DataSyncBarrier ();

	u32 nMIS = BTUARTReadReg (UART0_MIS);
	if (nMIS & INT_OE)
	{
		LogWrite (FromBTUART, LOG_PANIC, "Overrun error");
	}

	BTUARTWriteReg (UART0_ICR, nMIS);

	while (1)		// FIXME Warning Maybe necessary convert IRQ in Task
	{
		if (BTUARTReadReg (UART0_FR) & FR_RXFE_MASK)
		{
			break;
		}

		u8 nData = BTUARTReadReg (UART0_DR) & 0xFF;

		switch (pThis->m_nRxState)
		{
			case RxStateStart:
				if (nData == HCI_PACKET_EVENT)
				{
					pThis->m_nRxInPtr = 0;
					pThis->m_nRxState = RxStateCommand;
				}
				break;

			case RxStateCommand:
				pThis->m_RxBuffer[pThis->m_nRxInPtr++] = nData;
				pThis->m_nRxState = RxStateLength;
				break;

			case RxStateLength:
				pThis->m_RxBuffer[pThis->m_nRxInPtr++] = nData;
				if (nData > 0)
				{
					pThis->m_nRxParamLength = nData;
					pThis->m_nRxState = RxStateParam;
				}
				else
				{
					if (pThis->m_pEventHandler != 0)
					{
						(*pThis->m_pEventHandler) (pThis->m_RxBuffer, pThis->m_nRxInPtr);		// TODO
					}

					pThis->m_nRxState = RxStateStart;
				}
				break;

			case RxStateParam:
				assert (pThis->m_nRxInPtr < BT_MAX_HCI_EVENT_SIZE);
				pThis->m_RxBuffer[pThis->m_nRxInPtr++] = nData;
				if (--pThis->m_nRxParamLength == 0)
				{
					if (pThis->m_pEventHandler != 0)
					{
						(*pThis->m_pEventHandler) (pThis->m_RxBuffer, pThis->m_nRxInPtr);		// TODO
					}

					pThis->m_nRxState = RxStateStart;
				}
				break;

			default:
				assert (0);
				break;
		}
	}

	DataMemBarrier ();
}

static void BTUARTTransportIRQStub (int nIRQ, void *pParam)
{
	(void) nIRQ;		// FIXME Wunused

	TBTUARTTransport *pThis = (TBTUARTTransport *) pParam;
	assert (pThis != 0);

	BTUARTTransportIRQHandler(pThis);
}
