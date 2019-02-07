//
// btlogicallayer.h
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2015  R. Stange <rsta2@o2online.de>
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
#ifndef _rsta_bt_btlogicallayer_h
#define _rsta_bt_btlogicallayer_h

#include <rsta_bt/bthcilayer.h>
#include <rsta_bt/btinquiryresults.h>
// #include <circle/sched/synchronizationevent.h>		// TODO
#include <uspi/types.h>

typedef struct TBTLogicalLayer
{
	TBTHCILayer				*m_pHCILayer;

	TBTInquiryResults		*m_pInquiryResults;

	unsigned				m_nNameRequestsPending;
	// CSynchronizationEvent	m_Event;				// TODO

	u8 						*m_pBuffer;
}
TBTLogicalLayer;

void BTLogicalLayer (TBTLogicalLayer *pThis, TBTHCILayer *pHCILayer);
void _BTLogicalLayer (TBTLogicalLayer *pThis);

boolean BTLogicalLayerInitialize (TBTLogicalLayer *pThis);

void BTLogicalLayerProcess (TBTLogicalLayer *pThis);

// returns 0 on failure, result must be deleted by caller otherwise
TBTInquiryResults *BTLogicalLayerInquiry (TBTLogicalLayer *pThis, unsigned nSeconds);		// 1 <= nSeconds <= 61

#endif
