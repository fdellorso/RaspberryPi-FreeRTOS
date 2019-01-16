//
// btinquiryresults.c
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
#include <rsta_bt/btinquiryresults.h>
#include <uspi/util.h>
#include <uspi/assert.h>

typedef struct TBTInquiryResponse
{
	u8	BDAddress[BT_BD_ADDR_SIZE];
	u8	ClassOfDevice[BT_CLASS_SIZE];
	u8	PageScanRepetitionMode;
	u8	RemoteName[BT_NAME_SIZE];
}
TBTInquiryResponse;

void BTInquiryResults (TBTInquiryResults *pThis)
{
	(void) pThis;		// FIXME Wunused
}

void _BTInquiryResults (TBTInquiryResults *pThis)
{
	for (unsigned nResponse = 0; nResponse < BTInquiryResultsGetCount(pThis); nResponse++)
	{
		TBTInquiryResponse *pResponse = (TBTInquiryResponse *) PtrArrayGetElement(pThis->m_Responses, nResponse);
		assert (pResponse != 0);

		free (pResponse);
	}
}

void BTInquiryResultsAddInquiryResult (TBTInquiryResults *pThis, TBTHCIEventInquiryResult *pEvent)
{
	assert (pEvent != 0);

	for (unsigned i = 0; i < pEvent->NumResponses; i++)
	{
		TBTInquiryResponse *pResponse = (TBTInquiryResponse *) malloc(sizeof(TBTInquiryResponse));
		assert (pResponse != 0);

		memcpy (pResponse->BDAddress, INQUIRY_RESP_BD_ADDR (pEvent, i), BT_BD_ADDR_SIZE);
		memcpy (pResponse->ClassOfDevice, INQUIRY_RESP_CLASS_OF_DEVICE (pEvent, i), BT_CLASS_SIZE);
		pResponse->PageScanRepetitionMode = INQUIRY_RESP_PAGE_SCAN_REP_MODE (pEvent, i);
		strcpy ((char *) pResponse->RemoteName, "Unknown");

		PtrArrayAppend(pThis->m_Responses, pResponse);
	}
}

boolean BTInquiryResultsSetRemoteName (TBTInquiryResults *pThis, TBTHCIEventRemoteNameRequestComplete *pEvent)
{
	assert (pEvent != 0);

	for (unsigned nResponse = 0; nResponse < BTInquiryResultsGetCount(pThis); nResponse++)
	{
		TBTInquiryResponse *pResponse = (TBTInquiryResponse *) PtrArrayGetElement(pThis->m_Responses, nResponse);
		assert (pResponse != 0);

		if (memcmp (pResponse->BDAddress, pEvent->BDAddr, BT_BD_ADDR_SIZE) == 0)
		{
			memcpy (pResponse->RemoteName, pEvent->RemoteName, BT_NAME_SIZE);

			return TRUE;
		}
	}

	return FALSE;
}

unsigned BTInquiryResultsGetCount (TBTInquiryResults *pThis)
{
	return PtrArrayGetCount(pThis->m_Responses);
}

const u8 *BTInquiryResultsGetBDAddress (TBTInquiryResults *pThis, unsigned nResponse)
{
	TBTInquiryResponse *pResponse = (TBTInquiryResponse *) PtrArrayGetElement(pThis->m_Responses, nResponse);
	assert (pResponse != 0);

	return pResponse->BDAddress;
}

const u8 *BTInquiryResultsGetClassOfDevice (TBTInquiryResults *pThis, unsigned nResponse)
{
	TBTInquiryResponse *pResponse = (TBTInquiryResponse *) PtrArrayGetElement(pThis->m_Responses, nResponse);
	assert (pResponse != 0);

	return pResponse->ClassOfDevice;
}

const u8 *BTInquiryResultsGetRemoteName (TBTInquiryResults *pThis, unsigned nResponse)
{
	TBTInquiryResponse *pResponse = (TBTInquiryResponse *) PtrArrayGetElement(pThis->m_Responses, nResponse);
	assert (pResponse != 0);

	return pResponse->RemoteName;
}

u8 BTInquiryResultsGetPageScanRepetitionMode (TBTInquiryResults *pThis, unsigned nResponse)
{
	TBTInquiryResponse *pResponse = (TBTInquiryResponse *) PtrArrayGetElement(pThis->m_Responses, nResponse);
	assert (pResponse != 0);

	return pResponse->PageScanRepetitionMode;
}
