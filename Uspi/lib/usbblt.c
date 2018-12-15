//
// usbblt.c
//
// USPi - An USB driver for Raspberry Pi written in C
// Copyright (C) 2014  R. Stange <rsta2@o2online.de>
// Copyright (C) 2014  M. Maccaferri <macca@maccasoft.com>
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
#include <uspi/usbblt.h>
#include <uspi/usbcommon.h>
#include <uspi/usbhostcontroller.h>
#include <uspi/devicenameservice.h>
#include <uspi/assert.h>
#include <uspi/util.h>
#include <uspios.h>

static const char FromBlt[] = "blt";

static unsigned s_nDeviceNumber = 0;

void USBBltDevice (TUSBBltDevice *pThis, TUSBDevice *pDevice)
{
	assert (pThis != 0);

	USBDeviceCopy (&pThis->m_USBDevice, pDevice);
	pThis->m_USBDevice.Configure = USBBltDeviceConfigure;
}

void _USBBltDevice (TUSBBltDevice *pThis)
{
	assert (pThis != 0);

	_USBDevice (&pThis->m_USBDevice);
}

boolean USBBltDeviceConfigure (TUSBDevice *pUSBDevice)
{
	TUSBBltDevice *pThis = (TUSBBltDevice *) pUSBDevice;
	assert (pThis != 0);

    USBDescriptorPrinter(&pThis->m_USBDevice, FromBlt);

    const TUSBDeviceDescriptor *pDeviceDesc = 
        USBDeviceGetDeviceDescriptor (&pThis->m_USBDevice);
	assert (pDeviceDesc != 0);


    // Configuration Check
    if (!USBDeviceConfigure (&pThis->m_USBDevice))
    {
        LogWrite (FromBlt, LOG_ERROR, "Cannot set configuration");

        return FALSE;
    }

    //DNS ADD
	TString DeviceName;
	String (&DeviceName);
	StringFormat (&DeviceName, "blt%u", s_nDeviceNumber++);
	DeviceNameServiceAddDevice (DeviceNameServiceGet (), StringGet (&DeviceName), pThis, FALSE);

	_String (&DeviceName);

	return TRUE;
}
