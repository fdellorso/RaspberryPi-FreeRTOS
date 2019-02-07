//
// bthcilayer.h
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
#ifndef _rsta_bt_bthcilayer_h
#define _rsta_bt_bthcilayer_h

#include <rsta_bt/bttransportlayer.h>
#include <uspi/usbbluetooth.h>
#include <rsta_bt/btuarttransport.h>
#include <rsta_bt/bluetooth.h>
#include <rsta_bt/btdevicemanager.h>
#include <rsta_bt/btqueue.h>
#include <uspi/types.h>

typedef struct TBTHCILayer
{
	TUSBBluetoothDevice			*m_pHCITransportUSB;
	TBTUARTTransport			*m_pHCITransportUART;

	struct TBTDeviceManager			*m_pDeviceManager;

	TBTQueue					*m_pCommandQueue;
	TBTQueue					*m_pDeviceEventQueue;
	TBTQueue					*m_pLinkEventQueue;

	u8							*m_pEventBuffer;
	unsigned					m_nEventLength;
	unsigned					m_nEventFragmentOffset;

	u8							*m_pBuffer;

	unsigned					m_nCommandPackets;				// commands allowed to be sent

	struct TBTHCILayer			*s_pThis;
}
TBTHCILayer;

void BTHCILayer (TBTHCILayer *pThis, u32 nClassOfDevice, const char *pLocalName);
void _BTHCILayer (TBTHCILayer *pThis);

boolean BTHCILayerInitialize (TBTHCILayer *pThis);

void BTHCILayerProcess (TBTHCILayer *pThis);

void BTHCILayerSendCommand (TBTHCILayer *pThis, const void *pBuffer, unsigned nLength);

// pBuffer must have size BT_MAX_HCI_EVENT_SIZE
boolean BTHCILayerReceiveLinkEvent (TBTHCILayer *pThis, void *pBuffer, unsigned *pResultLength);

void BTHCILayerSetCommandPackets (TBTHCILayer *pThis, unsigned nCommandPackets);	// set commands allowed to be sent

TBTTransportType BTHCILayerGetTransportType (TBTHCILayer *pThis);

struct TBTDeviceManager *BTHCILayerGetDeviceManager (TBTHCILayer *pThis);

#endif
