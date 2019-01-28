//
// btinquiryresults.h
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
#ifndef _rsta_bt_btinquiryresults_h
#define _rsta_bt_btinquiryresults_h

#include <rsta_bt/bluetooth.h>
#include <rsta_bt/ptrarray.h>
#include <uspi/types.h>

typedef struct TBTInquiryResults
{
	TPtrArray *m_Responses;
}
TBTInquiryResults;

typedef struct TBTInquiryResponse
{
	u8	BDAddress[BT_BD_ADDR_SIZE];
	u8	ClassOfDevice[BT_CLASS_SIZE];
	u8	PageScanRepetitionMode;
	u8	RemoteName[BT_NAME_SIZE];
}
TBTInquiryResponse;

void BTInquiryResults (TBTInquiryResults *pThis);
void _BTInquiryResults (TBTInquiryResults *pThis);

void BTInquiryResultsAddInquiryResult (TBTInquiryResults *pThis, TBTHCIEventInquiryResult *pEvent);
boolean BTInquiryResultsSetRemoteName (TBTInquiryResults *pThis, TBTHCIEventRemoteNameRequestComplete *pEvent);

unsigned BTInquiryResultsGetCount (TBTInquiryResults *pThis);

const u8 *BTInquiryResultsGetBDAddress (TBTInquiryResults *pThis, unsigned nResponse);
const u8 *BTInquiryResultsGetClassOfDevice (TBTInquiryResults *pThis, unsigned nResponse);
const u8 *BTInquiryResultsGetRemoteName (TBTInquiryResults *pThis, unsigned nResponse);
u8 BTInquiryResultsGetPageScanRepetitionMode (TBTInquiryResults *pThis, unsigned nResponse);

#endif
