//
// usbbluetooth.h
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
#ifndef _uspi_usbbluetooth_h
#define _uspi_usbbluetooth_h

#include <uspi/usbfunction.h>
#include <uspi/usbendpoint.h>
#include <uspi/usbrequest.h>
#include <uspi/types.h>
#include <uspi.h>
#include <rsta_bt/bttransportlayer.h>


typedef struct TUSBBluetoothDevice
{
	TUSBFunction 		m_USBFunction;

	TUSBEndpoint 		*m_pEndpointInterrupt;
	TUSBEndpoint 		*m_pEndpointBulkIn;
	TUSBEndpoint 		*m_pEndpointBulkOut;

	TUSBRequest 		*m_pURB;
	u8					*m_pEventBuffer;

	TBTHCIEventHandler	*m_pEventHandler;

	// unsigned      		s_nDeviceNumber;
}
TUSBBluetoothDevice;


void USBBluetoothDevice (TUSBBluetoothDevice *pThis, TUSBFunction *pFunction);
void _USBBluetoothDevice (TUSBBluetoothDevice *pThis);

boolean USBBluetoothDeviceConfigure (TUSBFunction *pUSBFunction);

boolean USBBluetoothDeviceSendHCICommand (TUSBBluetoothDevice *pThis, const void *pBuffer, unsigned nLength);

void USBBluetoothDeviceRegisterHCIEventHandler (TUSBBluetoothDevice *pThis, TBTHCIEventHandler *pHandler);

#endif
