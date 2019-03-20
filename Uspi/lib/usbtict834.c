//
// usbtict834.c
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
#include <uspi/usbtict834.h>
#include <uspi/usbcommon.h>
#include <uspi/usbhostcontroller.h>
#include <uspi/devicenameservice.h>
#include <uspi/assert.h>
#include <uspi/util.h>
#include <uspios.h>

#include <prvlib/stdlib.h>

static const char FromTicT834[] = "tict834";

static unsigned s_nDeviceNumber = 1;

void USBTicT834Device (TUSBTicT834Device *pThis, TUSBFunction *pFunction)
{
	assert (pThis != 0);

	USBFunctionCopy (&pThis->m_USBFunction, pFunction);
	pThis->m_USBFunction.Configure = USBTicT834DeviceConfigure;

    pThis->serial_number = NULL;
    pThis->firmware_version = 0;
    pThis->vendor = 0;
    pThis->product = 0;
}

void _USBTicT834Device (TUSBTicT834Device *pThis)
{
	assert (pThis != 0);

    if (pThis->serial_number != 0)
	{
		free (pThis->serial_number);
		pThis->serial_number = 0;
	}

	_USBFunction (&pThis->m_USBFunction);
}

boolean USBTicT834DeviceConfigure (TUSBFunction *pUSBFunction)
{
	TUSBTicT834Device *pThis = (TUSBTicT834Device *) pUSBFunction;
	assert (pThis != 0);

    const TUSBDeviceDescriptor *pDeviceDesc = 
        USBDeviceGetDeviceDescriptor (USBFunctionGetDevice (&pThis->m_USBFunction));
	assert (pDeviceDesc != 0);

    pThis->firmware_version = pDeviceDesc->bcdDevice;
    pThis->vendor = pDeviceDesc->idVendor;
    pThis->product = pDeviceDesc->idProduct;

    // Get Serial Number from String Descriptor
    uint8_t *buffer = (uint8_t *) malloc (sizeof(uint8_t) * pDeviceDesc->bMaxPacketSize0);

    if(!DWHCIDeviceGetDescriptor (USBFunctionGetHost (&pThis->m_USBFunction),
                                  USBFunctionGetEndpoint0 (&pThis->m_USBFunction),
                                  DESCRIPTOR_STRING,
                                  3,
                                  buffer,
                                  sizeof(buffer),
                                  REQUEST_IN))
    {
        LogWrite (FromTicT834, LOG_ERROR, "Cannot get Serial Number");

		return FALSE;
    }

    uint8_t * new_string = (uint8_t*) malloc(sizeof(uint8_t) * ((buffer[0] - 2) /2 +1));
    assert (new_string != 0);

    int i;
    int index = 0;
    for(i = 2; i < buffer[0]; i += 2)
    {
        new_string[index++] = buffer[i];
    }

    new_string[index] = 0;

    pThis->serial_number = (s8 *) new_string;

    free(new_string);
    free(buffer);

    USBDescriptorPrinter(USBFunctionGetDevice (&pThis->m_USBFunction), FromTicT834);

    // Configuration Check
    if (!USBFunctionConfigure (&pThis->m_USBFunction))
    {
        LogWrite (FromTicT834, LOG_ERROR, "Cannot set configuration");

        return FALSE;
    }

    //DNS ADD
	TString DeviceName;
	String (&DeviceName);
	StringFormat (&DeviceName, "tic%u", s_nDeviceNumber++);
	DeviceNameServiceAddDevice (DeviceNameServiceGet (), StringGet (&DeviceName), pThis, FALSE);

	_String (&DeviceName);

    // LogWrite (FromTicT834, LOG_NOTICE, "Serial Number is\t%s", pThis->serial_number);
    // LogWrite (FromTicT834, LOG_NOTICE, "Firmware Version is\t%x", pThis->firmware_version);
    // LogWrite (FromTicT834, LOG_NOTICE, "Max Packet Size is\t%u", pDeviceDesc->bMaxPacketSize0);

	return TRUE;
}

boolean USBTicT834DeviceWriteReg (TUSBTicT834Device *pThis, u8 nCommand, u16 nValue, u32 nIndex, u16 nLength, u32 *nData)
{
	assert (pThis != 0);

	return DWHCIDeviceControlMessage (USBFunctionGetHost (&pThis->m_USBFunction),
			USBFunctionGetEndpoint0 (&pThis->m_USBFunction),
            REQUEST_OUT | REQUEST_VENDOR, nCommand,
            nValue, nIndex, nData, nLength) >= 0;
}

boolean USBTicT834DeviceReadReg (TUSBTicT834Device *pThis, u8 nCommand, u16 nValue, u32 nIndex, u16 nLength, u32 *nData)
{
	assert (pThis != 0);

	return DWHCIDeviceControlMessage (USBFunctionGetHost (&pThis->m_USBFunction),
            USBFunctionGetEndpoint0 (&pThis->m_USBFunction),
            REQUEST_IN | REQUEST_VENDOR, nCommand,
            nValue, nIndex, nData, nLength) >= 0;
}

int USBTicT834DeviceControl (TUSBTicT834Device *pThis, u8 nReqType, u8 nCommand, u16 nValue, u32 nIndex, u16 nLength, u8 *nData)
{
	assert (pThis != 0);

	return DWHCIDeviceControlMessage (USBFunctionGetHost (&pThis->m_USBFunction),
            USBFunctionGetEndpoint0 (&pThis->m_USBFunction),
            nReqType, nCommand, nValue, nIndex, nData, nLength);
}

boolean USBTicT834DeviceReadString (TUSBTicT834Device *pThis, u8 nString, u8 *nData)
{
	assert (pThis != 0);

	return DWHCIDeviceGetDescriptor (USBFunctionGetHost (&pThis->m_USBFunction),
            USBFunctionGetEndpoint0 (&pThis->m_USBFunction),
            DESCRIPTOR_STRING, nString, nData, sizeof(nData),
            REQUEST_IN) >= 0;
}

s8 * USBTicT834DeviceGetSerialNumber(TUSBTicT834Device *pThis)
{
  assert (pThis != 0);

  return pThis->serial_number;
}
