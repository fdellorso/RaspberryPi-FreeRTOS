//
// btdevicemanager.h
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
#ifndef _rsta_bt_btdevicemanager_h
#define _rsta_bt_btdevicemanager_h

#include <rsta_bt/bluetooth.h>
#include <rsta_bt/bthcilayer.h>
#include <rsta_bt/btqueue.h>
#include <uspi/types.h>

typedef enum
{
	BTDeviceStateResetPending,
	BTDeviceStateWriteRAMPending,
	BTDeviceStateLaunchRAMPending,
	BTDeviceStateReadBDAddrPending,
	BTDeviceStateWriteClassOfDevicePending,
	BTDeviceStateWriteLocalNamePending,
	BTDeviceStateWriteScanEnabledPending,
	BTDeviceStateRunning,
	BTDeviceStateFailed,
	BTDeviceStateUnknown
} TBTDeviceState;

typedef struct TBTDeviceManager
{
	struct TBTHCILayer		*m_pHCILayer;
	TBTQueue		*m_pEventQueue;
	u32				m_nClassOfDevice;
	u8				m_LocalName[BT_NAME_SIZE];

	TBTDeviceState	m_State;

	u8				m_LocalBDAddr[BT_BD_ADDR_SIZE];

	u8				*m_pBuffer;

	unsigned		m_nFirmwareOffset;
}
TBTDeviceManager;

void BTDeviceManager (TBTDeviceManager *pThis, struct TBTHCILayer *pHCILayer, TBTQueue *pEventQueue, u32 nClassOfDevice, const char *pLocalName);
void _BTDeviceManager (TBTDeviceManager *pThis);

boolean BTDeviceManagerInitialize (TBTDeviceManager *pThis);

void BTDeviceManagerProcess (TBTDeviceManager *pThis);

boolean BTDeviceManagerDeviceIsRunning (TBTDeviceManager *pThis);

#endif
