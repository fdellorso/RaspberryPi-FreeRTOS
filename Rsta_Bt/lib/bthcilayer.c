//
// bthcilayer.c
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
#include <rsta_bt/bthcilayer.h>
#include <uspi/devicenameservice.h>
// #include <circle/logger.h>
#include <uspios.h>
#include <uspi/util.h>
#include <uspi/assert.h>

static const char FromHCILayer[] = "bthci";

TBTHCILayer *s_pThis = 0;

void BTHCILayerEventHandler (TBTHCILayer *pThis, const void *pBuffer, unsigned nLength);
static void BTHCILayerEventStub (const void *pBuffer, unsigned nLength);

void BTHCILayer (TBTHCILayer *pThis, u32 nClassOfDevice, const char *pLocalName)
{
	pThis->m_pHCITransportUSB		= 0;
	pThis->m_pHCITransportUART		= 0;
	BTDeviceManager(pThis->m_DeviceManager, pThis, &m_DeviceEventQueue, nClassOfDevice, pLocalName);
	pThis->m_pEventBuffer			= 0;
	pThis->m_nEventLength			= 0;
	pThis->m_nEventFragmentOffset	= 0;
	pThis->m_pBuffer				= 0;
	pThis->m_nCommandPackets		= 1;

	assert (s_pThis == 0);
	s_pThis = pThis;
}

void _BTHCILayer (TBTHCILayer *pThis)
{
	pThis->m_pHCITransportUSB = 0;
	pThis->m_pHCITransportUART = 0;

	free (pThis->m_pBuffer);
	pThis->m_pBuffer = 0;

	free (pThis->m_pEventBuffer);
	pThis->m_pEventBuffer = 0;

	s_pThis = 0;
}

boolean BTHCILayerInitialize (TBTHCILayer *pThis)
{
	pThis->m_pHCITransportUSB = (TUSBBluetoothDevice *) DeviceNameServiceGetDevice (DeviceNameServiceGet (), "ubt1", FALSE);												
	if (pThis->m_pHCITransportUSB == 0)
	{
		pThis->m_pHCITransportUART = (TBTUARTTransport *) DeviceNameServiceGetDevice (DeviceNameServiceGet (), "ttyBT1", FALSE);
		if (pThis->m_pHCITransportUART == 0)
		{
			LogWrite (FromHCILayer, LOG_ERROR, "Bluetooth controller not found");

			return FALSE;
		}
	}

	pThis->m_pEventBuffer = (u8) malloc (sizeof(u8) * BT_MAX_HCI_EVENT_SIZE);
	assert (pThis->m_pEventBuffer != 0);

	pThis->m_pBuffer = (u8) malloc (sizeof(u8) * BT_MAX_DATA_SIZE);
	assert (pThis->m_pBuffer != 0);

	if (pThis->m_pHCITransportUSB != 0)
	{
		USBBluetoothDeviceRegisterHCIEventHandler(pThis->m_pHCITransportUSB, BTHCILayerEventStub);
	}
	else
	{
		assert (pThis->m_pHCITransportUART != 0);
		BTUARTTransportRegisterHCIEventHandler(pThis->m_pHCITransportUART, BTHCILayerEventStub);
	}

	return BTDeviceManagerInitialize(pThis->m_DeviceManager);
}

TBTTransportType BTHCILayerGetTransportType (TBTHCILayer *pThis)
{
	if (pThis->m_pHCITransportUSB != 0)
	{
		return BTTransportTypeUSB;
	}

	if (pThis->m_pHCITransportUART != 0)
	{
		return BTTransportTypeUART;
	}

	return BTTransportTypeUnknown;
}

void BTHCILayerProcess (TBTHCILayer *pThis)
{
	assert (pThis->m_pHCITransportUSB != 0 || pThis->m_pHCITransportUART != 0);
	assert (pThis->m_pBuffer != 0);

	unsigned nLength;
	while (   pThis->m_nCommandPackets > 0
	       && (nLength = BTQueueDequeue(pThis->m_CommandQueue, pThis->m_pBuffer, NULL)) > 0)
	{
		if (  m_pHCITransportUSB != 0
		    ? !USBBluetoothDeviceSendHCICommand(pThis->m_pHCITransportUSB, m_pBuffer, nLength)
		    : !BTUARTTransportSendHCICommand(pThis->m_pHCITransportUART, m_pBuffer, nLength))
		{
			LogWrite (FromHCILayer, LOG_ERROR, "HCI command dropped");

			break;
		}

		pThis->m_nCommandPackets--;
	}

	BTDeviceManagerProcess(pThis->m_DeviceManager);
}

void BTHCILayerSendCommand (TBTHCILayer *pThis, const void *pBuffer, unsigned nLength)
{
	BTQueueEnqueue(pThis->m_CommandQueue, pBuffer, nLength, NULL);
}

boolean BTHCILayerReceiveLinkEvent (TBTHCILayer *pThis, void *pBuffer, unsigned *pResultLength)
{
	unsigned nLength = BTQueueDequeue(pThis->m_LinkEventQueue, pBuffer, NULL);
	if (nLength > 0)
	{
		assert (pResultLength != 0);
		*pResultLength = nLength;

		return TRUE;
	}

	return FALSE;
}

void BTHCILayerSetCommandPackets (TBTHCILayer *pThis, unsigned nCommandPackets)
{
	pThis->m_nCommandPackets = nCommandPackets;
}

TBTDeviceManager *BTHCILayerGetDeviceManager (TBTHCILayer *pThis)
{
	return &pThis->m_DeviceManager;
}

void BTHCILayerEventHandler (TBTHCILayer *pThis, const void *pBuffer, unsigned nLength)
{
	assert (pBuffer != 0);
	assert (nLength > 0);

	if (pThis->m_nEventFragmentOffset == 0)
	{
		if (nLength < sizeof (TBTHCIEventHeader))
		{
			LogWrite (FromHCILayer, LOG_WARNING, "Short event ignored");

			return;
		}

		TBTHCIEventHeader *pHeader = (TBTHCIEventHeader *) pBuffer;

		assert (pThis->m_nEventLength == 0);
		pThis->m_nEventLength = sizeof (TBTHCIEventHeader) + pHeader->ParameterTotalLength;
	}

	assert (pThis->m_pEventBuffer != 0);
	memcpy (pThis->m_pEventBuffer + pThis->m_nEventFragmentOffset, pBuffer, nLength);

	pThis->m_nEventFragmentOffset += nLength;
	if (pThis->m_nEventFragmentOffset < pThis->m_nEventLength)
	{
		return;
	}

	TBTHCIEventHeader *pHeader = (TBTHCIEventHeader *) pThis->m_pEventBuffer;
	switch (pHeader->EventCode)
	{
	case EVENT_CODE_COMMAND_COMPLETE:
	case EVENT_CODE_COMMAND_STATUS:
		BTQueueEnqueue(pThis->m_DeviceEventQueue, pThis->m_pEventBuffer, pThis->m_nEventLength, NULL);
		break;

	default:
		BTQueueEnqueue(pThis->m_LinkEventQueue, pThis->m_pEventBuffer, pThis->m_nEventLength, NULL);
		break;
	}

	pThis->m_nEventLength = 0;
	pThis->m_nEventFragmentOffset = 0;
}

void BTHCILayerEventStub (const void *pBuffer, unsigned nLength)
{
	assert (s_pThis != 0);
	BTHCILayerEventHandler(s_pThis, pBuffer, nLength);
}
