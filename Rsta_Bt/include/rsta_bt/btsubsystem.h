//
// btsubsystem.h
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2015-2016  R. Stange <rsta2@o2online.de>
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
#ifndef _rsta_bt_btsubsystem_h
#define _rsta_bt_btsubsystem_h

// #include <circle/interrupt.h>
#include <interrupts.h>
#include <rsta_bt/bluetooth.h>
#include <rsta_bt/btinquiryresults.h>
#include <rsta_bt/btuarttransport.h>
#include <rsta_bt/bthcilayer.h>
#include <rsta_bt/btlogicallayer.h>
#include <uspi/types.h>

typedef struct TBTSubSystem
{
	CInterruptSystem	*m_pInterruptSystem;	// TODO

	TBTUARTTransport	*m_pUARTTransport;

	TBTHCILayer			*m_pHCILayer;
	TBTLogicalLayer		*m_pLogicalLayer;
}
TBTSubSystem;

void BTSubSystem (TBTSubSystem *pThis, CInterruptSystem *pInterruptSystem,
				  u32 nClassOfDevice, const char *pLocalName);

void _BTSubSystem (TBTSubSystem *pThis);

boolean BTSubSystemInitialize (TBTSubSystem *pThis);

void BTSubSystemProcess (TBTSubSystem *pThis);

// returns 0 on failure, result must be deleted by caller otherwise
TBTInquiryResults *BTSubSystemInquiry (TBTSubSystem *pThis, unsigned nSeconds);		// 1 <= nSeconds <= 61

#endif
