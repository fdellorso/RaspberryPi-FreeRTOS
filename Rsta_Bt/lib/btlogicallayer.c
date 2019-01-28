//
// btlogicallayer.c
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
#include <rsta_bt/btlogicallayer.h>
#include <rsta_bt/bluetooth.h>
#include <uspi/util.h>
#include <uspi/assert.h>

void BTLogicalLayer (TBTLogicalLayer *pThis, TBTHCILayer *pHCILayer)
{
	pThis->m_pHCILayer 			= pHCILayer;
	pThis->m_pInquiryResults	= 0;
	pThis->m_pBuffer			= 0;
}

void _BTLogicalLayer (TBTLogicalLayer *pThis)
{
	assert (pThis->m_pInquiryResults == 0);

	free (pThis->m_pBuffer;
	pThis->m_pBuffer = 0;

	pThis->m_pHCILayer = 0;
}

boolean BTLogicalLayerInitialize (TBTLogicalLayer *pThis)
{
	pThis->m_pBuffer = (u8) malloc (sizeof(u8) * BT_MAX_DATA_SIZE);
	assert (pThis->m_pBuffer != 0);

	return TRUE;
}

void BTLogicalLayerProcess (TBTLogicalLayer *pThis)
{
	assert (pThis->m_pHCILayer != 0);
	assert (pThis->m_pBuffer != 0);

	unsigned nLength;
	while (BTHCILayerReceiveLinkEvent(pThis->m_pHCILayer, pThis->m_pBuffer, &nLength))
	{
		assert (nLength >= sizeof (TBTHCIEventHeader));
		TBTHCIEventHeader *pHeader = (TBTHCIEventHeader *) pThis->m_pBuffer;

		switch (pHeader->EventCode)
		{
			case EVENT_CODE_INQUIRY_RESULT: {
				assert (nLength >= sizeof (TBTHCIEventInquiryResult));
				TBTHCIEventInquiryResult *pEvent = (TBTHCIEventInquiryResult *) pHeader;
				assert (nLength >= sizeof (TBTHCIEventInquiryResult)
						+ pEvent->NumResponses * INQUIRY_RESP_SIZE);

				assert (pThis->m_pInquiryResults != 0);
				BTInquiryResultsAddInquiryResult(pThis->m_pInquiryResults, pEvent);
			} break;

			case EVENT_CODE_INQUIRY_COMPLETE: {
				assert (nLength >= sizeof (TBTHCIEventInquiryComplete));
				TBTHCIEventInquiryComplete *pEvent = (TBTHCIEventInquiryComplete *) pHeader;

				if (pEvent->Status != BT_STATUS_SUCCESS)
				{
					free (pThis->m_pInquiryResults);
					pThis->m_pInquiryResults = 0;

					m_Event.Set ();		// TODO

					break;
				}

				pThis->m_nNameRequestsPending = BTInquiryResultsGetCount(pThis->m_pInquiryResults);
				if (pThis->m_nNameRequestsPending == 0)
				{
					m_Event.Set ();		// TODO

					break;
				}

				assert (pThis->m_pInquiryResults != 0);
				unsigned nResponse;
				for (nResponse = 0; nResponse < BTInquiryResultsGetCount(pThis->m_pInquiryResults); nResponse++)
				{
					TBTHCIRemoteNameRequestCommand Cmd;
					Cmd.Header.OpCode = OP_CODE_REMOTE_NAME_REQUEST;
					Cmd.Header.ParameterTotalLength = PARM_TOTAL_LEN (Cmd);
					memcpy (Cmd.BDAddr, BTInquiryResultsGetBDAddress(pThis->m_pInquiryResults, nResponse), BT_BD_ADDR_SIZE);
					Cmd.PageScanRepetitionMode = BTInquiryResultsGetPageScanRepetitionMode(pThis->m_pInquiryResults, nResponse);
					Cmd.Reserved = 0;
					Cmd.ClockOffset = CLOCK_OFFSET_INVALID;
					BTHCILayerSendCommand(pThis->m_pHCILayer, &Cmd, sizeof Cmd);
				}
			} break;

			case EVENT_CODE_REMOTE_NAME_REQUEST_COMPLETE: {
				assert (nLength >= sizeof (TBTHCIEventRemoteNameRequestComplete));
				TBTHCIEventRemoteNameRequestComplete *pEvent = (TBTHCIEventRemoteNameRequestComplete *) pHeader;

				if (pEvent->Status == BT_STATUS_SUCCESS)
				{
					assert (pThis->m_pInquiryResults != 0);
					BTInquiryResultsSetRemoteName(pThis->m_pInquiryResults, pEvent);
				}

				if (--pThis->m_nNameRequestsPending == 0)
				{
					m_Event.Set ();		// TODO
				}
			} break;

			default:
				break;
		}
	}
}

TBTInquiryResults *BTLogicalLayerInquiry (TBTLogicalLayer *pThis, unsigned nSeconds)
{
	assert (1 <= nSeconds && nSeconds <= 61);
	assert (pThis->m_pHCILayer != 0);

	assert (pThis->m_pInquiryResults == 0);
	pThis->m_pInquiryResults = (TBTInquiryResults *) malloc (sizeof(TBTInquiryResults));
	assert (pThis->m_pInquiryResults != 0);

	m_Event.Clear ();					// TODO

	TBTHCIInquiryCommand Cmd;
	Cmd.Header.OpCode = OP_CODE_INQUIRY;
	Cmd.Header.ParameterTotalLength = PARM_TOTAL_LEN (Cmd);
	Cmd.LAP[0] = INQUIRY_LAP_GIAC       & 0xFF;
	Cmd.LAP[1] = INQUIRY_LAP_GIAC >> 8  & 0xFF;
	Cmd.LAP[2] = INQUIRY_LAP_GIAC >> 16 & 0xFF;
	Cmd.InquiryLength = INQUIRY_LENGTH (nSeconds);
	Cmd.NumResponses = INQUIRY_NUM_RESPONSES_UNLIMITED;
	BTHCILayerSendCommand(pThis->m_pHCILayer, &Cmd, sizeof Cmd);

	m_Event.Wait ();					// TODO

	TBTInquiryResults *pResult = pThis->m_pInquiryResults;
	pThis->m_pInquiryResults = 0;
	
	return pResult;
}
