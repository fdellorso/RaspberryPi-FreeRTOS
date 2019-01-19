//
// usbblt.h
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
#ifndef _usbblt_h
#define _usbblt_h

#include <uspi/usbdevice.h>
#include <uspi/usbendpoint.h>
#include <uspi/usbrequest.h>
#include <uspi/types.h>
#include <uspi.h>

#define OGF_INFO_CMD            0x04

#define OCF_HCI_READ_BD_ADDR    0x0009

typedef struct TUSBBltDevice
{
    TUSBDevice      m_USBDevice;

    u8              m_ucInterfaceNumber;
	u8              m_ucAlternateSetting;

    TUSBEndpoint    *m_pEndpointBulkIn;
	TUSBEndpoint    *m_pEndpointBulkOut;
    TUSBEndpoint    *m_pReportEndpoint;

    u8              *m_pReportBuffer;
    u8              *m_pBd_addr;
}
TUSBBltDevice;

void USBBltDevice (TUSBBltDevice *pThis, TUSBDevice *pDevice);
void _USBBltDevice (TUSBBltDevice *pThis);

boolean USBBltDeviceConfigure (TUSBDevice *pUSBDevice);

#endif