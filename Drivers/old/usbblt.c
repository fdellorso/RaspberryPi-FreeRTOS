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

	pThis->m_pEndpointBulkIn = 0;
	pThis->m_pEndpointBulkOut = 0;

	pThis->m_pReportEndpoint = 0;
	pThis->m_pReportBuffer = 0;

	pThis->m_pReportBuffer = malloc(16);
	assert (pThis->m_pReportBuffer != 0);

	pThis->m_pBd_addr = malloc(6);
	assert (pThis->m_pBd_addr != 0);
}

void _USBBltDevice (TUSBBltDevice *pThis)
{
	assert (pThis != 0);

	if (pThis->m_pReportBuffer != 0)
	{
		free (pThis->m_pReportBuffer);
		pThis->m_pReportBuffer = 0;
	}

	if (pThis->m_pReportEndpoint != 0)
	{
		_USBEndpoint (pThis->m_pReportEndpoint);
		free (pThis->m_pReportEndpoint);
		pThis->m_pReportEndpoint = 0;
	}

	if (pThis->m_pEndpointBulkOut != 0)
	{
		_USBEndpoint (pThis->m_pEndpointBulkOut);
		free (pThis->m_pEndpointBulkOut);
		pThis->m_pEndpointBulkOut = 0;
	}
	
	if (pThis->m_pEndpointBulkIn != 0)
	{
		_USBEndpoint (pThis->m_pEndpointBulkIn);
		free (pThis->m_pEndpointBulkIn);
		pThis->m_pEndpointBulkIn = 0;
	}

	_USBDevice (&pThis->m_USBDevice);
}

boolean USBBltDeviceConfigure (TUSBDevice *pUSBDevice)
{
	TUSBBltDevice *pThis = (TUSBBltDevice *) pUSBDevice;
	assert (pThis != 0);

    // USBDescriptorPrinter(&pThis->m_USBDevice, FromBlt);

    // const TUSBDeviceDescriptor *pDeviceDesc = 
    //     USBDeviceGetDeviceDescriptor (&pThis->m_USBDevice);
	// assert (pDeviceDesc != 0);

	TUSBConfigurationDescriptor *pConfDesc =
		(TUSBConfigurationDescriptor *) USBDeviceGetDescriptor (&pThis->m_USBDevice, DESCRIPTOR_CONFIGURATION);
	if (   pConfDesc == 0
	    || pConfDesc->bNumInterfaces <  1)
	{
		USBDeviceConfigurationError (&pThis->m_USBDevice, FromBlt);

		return FALSE;
	}

	TUSBInterfaceDescriptor *pInterfaceDesc;
	while ((pInterfaceDesc = (TUSBInterfaceDescriptor *) USBDeviceGetDescriptor (&pThis->m_USBDevice, DESCRIPTOR_INTERFACE)) != 0)
	{
		if (pInterfaceDesc->bInterfaceNumber > 0)
		{
			break;
		}

		pThis->m_ucInterfaceNumber  = pInterfaceDesc->bInterfaceNumber;
		pThis->m_ucAlternateSetting = pInterfaceDesc->bAlternateSetting;

		const TUSBEndpointDescriptor *pEndpointDesc;
		while ((pEndpointDesc = (TUSBEndpointDescriptor *) USBDeviceGetDescriptor (&pThis->m_USBDevice, DESCRIPTOR_ENDPOINT)) != 0)
		{
			if ((pEndpointDesc->bmAttributes & 0x3F) == 0x03)			// Interrupt
			{
				if (pThis->m_pReportEndpoint != 0)
				{
					USBDeviceConfigurationError (&pThis->m_USBDevice, FromBlt);

					return FALSE;
				}

				pThis->m_pReportEndpoint = (TUSBEndpoint *) malloc (sizeof (TUSBEndpoint));
				assert (pThis->m_pReportEndpoint != 0);
				USBEndpoint2 (pThis->m_pReportEndpoint, &pThis->m_USBDevice, pEndpointDesc);
			}

			if ((pEndpointDesc->bmAttributes & 0x3F) == 0x02)			// Bulk
			{
				if ((pEndpointDesc->bEndpointAddress & 0x80) == 0x80)	// Input
				{
					if (pThis->m_pEndpointBulkIn != 0)
					{
						USBDeviceConfigurationError (&pThis->m_USBDevice, FromBlt);

						return FALSE;
					}

					pThis->m_pEndpointBulkIn = (TUSBEndpoint *) malloc (sizeof (TUSBEndpoint));
					assert (pThis->m_pEndpointBulkIn != 0);
					USBEndpoint2 (pThis->m_pEndpointBulkIn, &pThis->m_USBDevice, pEndpointDesc);
				}
				else													// Output
				{
					if (pThis->m_pEndpointBulkOut != 0)
					{
						USBDeviceConfigurationError (&pThis->m_USBDevice, FromBlt);

						return FALSE;
					}

					pThis->m_pEndpointBulkOut = (TUSBEndpoint *) malloc (sizeof (TUSBEndpoint));
					assert (pThis->m_pEndpointBulkOut != 0);
					USBEndpoint2 (pThis->m_pEndpointBulkOut, &pThis->m_USBDevice, pEndpointDesc);
				}
			}
		}
	}

	if (pThis->m_pReportEndpoint == 0)
	{
		USBDeviceConfigurationError (&pThis->m_USBDevice, FromBlt);

		return FALSE;
	}

	if (   pThis->m_pEndpointBulkIn  == 0
	    || pThis->m_pEndpointBulkOut == 0)
	{
		USBDeviceConfigurationError (&pThis->m_USBDevice, FromBlt);

		return FALSE;
	}

    // Configuration Check
    if (!USBDeviceConfigure (&pThis->m_USBDevice))
    {
        LogWrite (FromBlt, LOG_ERROR, "Cannot set configuration");

        return FALSE;
    }

	if (pThis->m_ucAlternateSetting != 0)
	{
		if (DWHCIDeviceControlMessage (USBDeviceGetHost (&pThis->m_USBDevice),
					USBDeviceGetEndpoint0 (&pThis->m_USBDevice),
					REQUEST_OUT | REQUEST_TO_INTERFACE, SET_INTERFACE,
					pThis->m_ucAlternateSetting,
					pThis->m_ucInterfaceNumber, 0, 0) < 0)
		{
			LogWrite (FromBlt, LOG_ERROR, "Cannot set interface");

			return FALSE;
		}
	}

	int opcode = OGF_INFO_CMD | (OCF_HCI_READ_BD_ADDR << 2);

	if (DWHCIDeviceControlMessage (USBDeviceGetHost (&pThis->m_USBDevice),
		USBDeviceGetEndpoint0 (&pThis->m_USBDevice),
		REQUEST_OUT | REQUEST_CLASS,
		0,
		0,
		0,
		&opcode,
		0) < 0)
	{
		LogWrite (FromBlt, LOG_ERROR, "Cannot Transfer Opcode Command");

		return FALSE;
	}

	if (DWHCIDeviceControlMessage (USBDeviceGetHost (&pThis->m_USBDevice),
		USBDeviceGetEndpoint0 (&pThis->m_USBDevice), //pThis->m_pReportEndpoint,
		REQUEST_IN | REQUEST_CLASS,
		0,
		0,
		0,
		pThis->m_pBd_addr,
		sizeof(pThis->m_pBd_addr)) < 0)
	{
		LogWrite (FromBlt, LOG_ERROR, "Cannot Receive BD_ADDR");

		return FALSE;
	}

	LogWrite (FromBlt, LOG_NOTICE, "BD_ADDR\t\t\t0x%x", pThis->m_pBd_addr);

    //DNS ADD
	TString DeviceName;
	String (&DeviceName);
	StringFormat (&DeviceName, "blt%u", s_nDeviceNumber++);
	DeviceNameServiceAddDevice (DeviceNameServiceGet (), StringGet (&DeviceName), pThis, FALSE);

	_String (&DeviceName);

	return TRUE;
}
