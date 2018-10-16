//
// usbtict834.h
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
#ifndef _usbtict834_h
#define _usbtict834_h

#include <uspi/usbdevice.h>
#include <uspi/usbendpoint.h>
#include <uspi/usbrequest.h>
#include <uspi/usbhid.h>
#include <uspi/types.h>
#include <uspi.h>

typedef struct TUSBTicT834Device
{
    TUSBDevice      m_USBDevice;

    TUSBEndpoint    *m_pEndpointIn;
    TUSBEndpoint    *m_pEndpointOut;

    s8	       		*serial_number;
    s8      	  	*os_id;
    u16		        firmware_version;
    u8		        product;

    s8              *cached_firmware_version_string;
}
TUSBTicT834Device;

void USBTicT834Device (TUSBTicT834Device *pThis, TUSBDevice *pDevice);
void _USBTicT834Device (TUSBTicT834Device *pThis);

boolean USBTicT834DeviceConfigure (TUSBDevice *pUSBDevice);
boolean USBTicT834DeviceWriteReg (TUSBTicT834Device *pThis, u8 nCommand, u16 nValue, u32 nIndex, u16 nLength, u32 *nData);
boolean USBTicT834DeviceReadReg (TUSBTicT834Device *pThis, u8 nCommand, u16 nValue, u32 nIndex, u16 nLength, u32 *nData);

#endif
