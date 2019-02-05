//
// btuarttransport.h
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
#ifndef _rsta_bt_btuarttransport_h
#define _rsta_bt_btuarttransport_h

// #include <circle/device.h>
#include <rsta_bt/bttransportlayer.h>
#include <rsta_bt/bluetooth.h>
// #include <circle/interrupt.h>
#include <interrupts.h>
// #include <circle/gpiopin.h>
#include <uspi/types.h>

typedef struct TBTUARTTransport
{
	// CGPIOPin			m_GPIO14;
	// CGPIOPin			m_GPIO15;
	// CGPIOPin			m_TxDPin;
	// CGPIOPin			m_RxDPin;

	CInterruptSystem	*m_pInterruptSystem;
	boolean				m_bIRQConnected;

	TBTHCIEventHandler	*m_pEventHandler;

	u8					m_RxBuffer[BT_MAX_HCI_EVENT_SIZE];
	unsigned			m_nRxState;
	unsigned			m_nRxParamLength;
	unsigned			m_nRxInPtr;
}
TBTUARTTransport;

void BTUARTTransport (TBTUARTTransport *pThis, CInterruptSystem *pInterruptSystem);
void _BTUARTTransport (TBTUARTTransport *pThis);

boolean BTUARTTransportInitialize (TBTUARTTransport *pThis, unsigned nBaudrate);

boolean BTUARTTransportSendHCICommand (TBTUARTTransport *pThis, const void *pBuffer, unsigned nLength);

void BTUARTTransportRegisterHCIEventHandler (TBTUARTTransport *pThis, TBTHCIEventHandler *pHandler);

#endif
