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
#include <uspi/usbhostcontroller.h>
#include <uspi/devicenameservice.h>
#include <uspi/assert.h>
#include <uspi/util.h>
#include <uspios.h>

static const char FromTicT834[] = "tict834";

static unsigned s_nDeviceNumber = 0;

void USBDescriptorPrinter(TUSBTicT834Device *pThis);

void USBTicT834Device (TUSBTicT834Device *pThis, TUSBDevice *pDevice)
{
	assert (pThis != 0);

	USBDeviceCopy (&pThis->m_USBDevice, pDevice);
	pThis->m_USBDevice.Configure = USBTicT834DeviceConfigure;
}

void _USBTicT834Device (TUSBTicT834Device *pThis)
{
	assert (pThis != 0);

    if (pThis->serial_number != 0)
	{
		free (pThis->serial_number);
		pThis->serial_number = 0;
	}

	_USBDevice (&pThis->m_USBDevice);
}

boolean USBTicT834DeviceConfigure (TUSBDevice *pUSBDevice)
{
	TUSBTicT834Device *pThis = (TUSBTicT834Device *) pUSBDevice;
	assert (pThis != 0);

    const TUSBDeviceDescriptor *pDeviceDesc = 
        USBDeviceGetDeviceDescriptor (&pThis->m_USBDevice);
	assert (pDeviceDesc != 0);

    pThis->firmware_version = pDeviceDesc->bcdDevice;
    pThis->vendor = pDeviceDesc->idVendor;
    pThis->product = pDeviceDesc->idProduct;

    // Get Serial Number from String Descriptor
    unsigned char buffer[pDeviceDesc->bMaxPacketSize0];

    if(!DWHCIDeviceGetDescriptor (USBDeviceGetHost (&pThis->m_USBDevice),
                                  USBDeviceGetEndpoint0 (&pThis->m_USBDevice),
                                  DESCRIPTOR_STRING,
                                  3,
                                  buffer,
                                  sizeof(buffer),
                                  REQUEST_IN))
    {
        LogWrite (FromTicT834, LOG_ERROR, "Cannot get Serial Number");

		return FALSE;
    }

    char * new_string = malloc((buffer[0] - 2) /2 +1);
    assert (new_string != 0);

    int i;
    int index = 0;
    for(i = 2; i < buffer[0]; i += 2)
    {
        new_string[index++] = buffer[i];
    }

    new_string[index] = 0;

    pThis->serial_number = new_string;

    // USBDescriptorPrinter(pThis);

    // Configuration Check
    if (!USBDeviceConfigure (&pThis->m_USBDevice))
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

	return DWHCIDeviceControlMessage (USBDeviceGetHost (&pThis->m_USBDevice),
			USBDeviceGetEndpoint0 (&pThis->m_USBDevice),
            REQUEST_OUT | REQUEST_VENDOR, nCommand,
            nValue, nIndex, nData, nLength) >= 0;
}

boolean USBTicT834DeviceReadReg (TUSBTicT834Device *pThis, u8 nCommand, u16 nValue, u32 nIndex, u16 nLength, u32 *nData)
{
	assert (pThis != 0);

	return DWHCIDeviceControlMessage (USBDeviceGetHost (&pThis->m_USBDevice),
            USBDeviceGetEndpoint0 (&pThis->m_USBDevice),
            REQUEST_IN | REQUEST_VENDOR, nCommand,
            nValue, nIndex, nData, nLength) >= 0;
}

int USBTicT834DeviceControl (TUSBTicT834Device *pThis, u8 nReqType, u8 nCommand, u16 nValue, u32 nIndex, u16 nLength, u8 *nData)
{
	assert (pThis != 0);

	return DWHCIDeviceControlMessage (USBDeviceGetHost (&pThis->m_USBDevice),
            USBDeviceGetEndpoint0 (&pThis->m_USBDevice),
            nReqType, nCommand, nValue, nIndex, nData, nLength);
}

boolean USBTicT834DeviceReadString (TUSBTicT834Device *pThis, u8 nString, u8 *nData)
{
	assert (pThis != 0);

	return DWHCIDeviceGetDescriptor (USBDeviceGetHost (&pThis->m_USBDevice),
            USBDeviceGetEndpoint0 (&pThis->m_USBDevice),
            DESCRIPTOR_STRING, nString, nData, sizeof(nData),
            REQUEST_IN) >= 0;
}

char * USBTicT834DeviceGetSerialNumber(TUSBTicT834Device *pThis)
{
  assert (pThis != 0);

  return pThis->serial_number;
}

void USBDescriptorPrinter(TUSBTicT834Device *pThis)
{
    const TUSBDeviceDescriptor *pDeviceDesc = 
        USBDeviceGetDeviceDescriptor (&pThis->m_USBDevice);
	assert (pDeviceDesc != 0);

    LogWrite (FromTicT834, LOG_NOTICE, "\r\n");
    LogWrite (FromTicT834, LOG_NOTICE, "Device Descriptor");
    LogWrite (FromTicT834, LOG_NOTICE, "------------------------------");
    LogWrite (FromTicT834, LOG_NOTICE, "bLength\t\t\t%u", pDeviceDesc->bLength);
    LogWrite (FromTicT834, LOG_NOTICE, "bDescriptorType\t\t%u", pDeviceDesc->bDescriptorType);
    LogWrite (FromTicT834, LOG_NOTICE, "bcdUSB\t\t\t0x%x", pDeviceDesc->bcdUSB);
    LogWrite (FromTicT834, LOG_NOTICE, "bDeviceClass\t\t0x%x", pDeviceDesc->bDeviceClass);
    LogWrite (FromTicT834, LOG_NOTICE, "bDeviceSubClass\t\t0x%x", pDeviceDesc->bDeviceSubClass);
    LogWrite (FromTicT834, LOG_NOTICE, "bDeviceProtocol\t\t0x%x", pDeviceDesc->bDeviceProtocol);
    LogWrite (FromTicT834, LOG_NOTICE, "bMaxPacketSize0\t\t%u", pDeviceDesc->bMaxPacketSize0);
    LogWrite (FromTicT834, LOG_NOTICE, "idVendor\t\t0x%x", pDeviceDesc->idVendor);
    LogWrite (FromTicT834, LOG_NOTICE, "idProduct\t\t0x%x", pDeviceDesc->idProduct);
    LogWrite (FromTicT834, LOG_NOTICE, "bcdDevice\t\t0x%x", pDeviceDesc->bcdDevice);
    LogWrite (FromTicT834, LOG_NOTICE, "iManufacturer\t\t%u", pDeviceDesc->iManufacturer);
    LogWrite (FromTicT834, LOG_NOTICE, "iProduct\t\t%u", pDeviceDesc->iProduct);
    LogWrite (FromTicT834, LOG_NOTICE, "iSerialNumber\t\t%u", pDeviceDesc->iSerialNumber);
    LogWrite (FromTicT834, LOG_NOTICE, "bNumConfigurations\t%u", pDeviceDesc->bNumConfigurations);
    LogWrite (FromTicT834, LOG_NOTICE, "------------------------------");


    // Configurator Descriptor
	const TUSBConfigurationDescriptor *pConfDesc =
		USBDeviceGetConfigurationDescriptor (&pThis->m_USBDevice);
	assert (pConfDesc != 0);

    LogWrite (FromTicT834, LOG_NOTICE, "\r\n");
    LogWrite (FromTicT834, LOG_NOTICE, "Configuration Descriptor");
    LogWrite (FromTicT834, LOG_NOTICE, "------------------------------");
    LogWrite (FromTicT834, LOG_NOTICE, "bLength\t\t\t%u", pConfDesc->bLength);
    LogWrite (FromTicT834, LOG_NOTICE, "bDescriptorType\t\t%u", pConfDesc->bDescriptorType);
    LogWrite (FromTicT834, LOG_NOTICE, "wTotalLength\t\t%u", pConfDesc->wTotalLength);
    LogWrite (FromTicT834, LOG_NOTICE, "bNumInterfaces\t\t%u", pConfDesc->bNumInterfaces);
    LogWrite (FromTicT834, LOG_NOTICE, "bConfigurationValue\t%u", pConfDesc->bConfigurationValue);
    LogWrite (FromTicT834, LOG_NOTICE, "iConfiguration\t\t%u", pConfDesc->iConfiguration);
    LogWrite (FromTicT834, LOG_NOTICE, "bmAttributes\t\t0x%x", pConfDesc->bmAttributes);
    LogWrite (FromTicT834, LOG_NOTICE, "bMaxPower\t\t%u", pConfDesc->bMaxPower);
    LogWrite (FromTicT834, LOG_NOTICE, "------------------------------");


    // Interface Descriptor
    TUSBInterfaceDescriptor *pInterfaceDesc =
        (TUSBInterfaceDescriptor *) USBDeviceGetDescriptor (&pThis->m_USBDevice, DESCRIPTOR_INTERFACE);
    assert (pInterfaceDesc != 0);

    LogWrite (FromTicT834, LOG_NOTICE, "\r\n");
    LogWrite (FromTicT834, LOG_NOTICE, "Interface Descriptor");
    LogWrite (FromTicT834, LOG_NOTICE, "------------------------------");
    LogWrite (FromTicT834, LOG_NOTICE, "bLength\t\t\t%u", pInterfaceDesc->bLength);
    LogWrite (FromTicT834, LOG_NOTICE, "bDescriptorType\t\t%u", pInterfaceDesc->bDescriptorType);
    LogWrite (FromTicT834, LOG_NOTICE, "bInterfaceNumber\t%u", pInterfaceDesc->bInterfaceNumber);
    LogWrite (FromTicT834, LOG_NOTICE, "bAlternateSetting\t%u", pInterfaceDesc->bAlternateSetting);
    LogWrite (FromTicT834, LOG_NOTICE, "bNumEndpoints\t\t%u", pInterfaceDesc->bNumEndpoints);
    LogWrite (FromTicT834, LOG_NOTICE, "bInterfaceClass\t\t0x%x", pInterfaceDesc->bInterfaceClass);
    LogWrite (FromTicT834, LOG_NOTICE, "bInterfaceSubClass\t0x%x", pInterfaceDesc->bInterfaceSubClass);
    LogWrite (FromTicT834, LOG_NOTICE, "bInterfaceProtocol\t0x%x", pInterfaceDesc->bInterfaceProtocol);
    LogWrite (FromTicT834, LOG_NOTICE, "iInterface\t\t%u", pInterfaceDesc->iInterface);
    LogWrite (FromTicT834, LOG_NOTICE, "------------------------------");


    // Endpoint Descriptor
    if (pInterfaceDesc->bNumEndpoints > 0)
    {
        TUSBEndpointDescriptor *pEndpointDesc =
            (TUSBEndpointDescriptor *) USBDeviceGetDescriptor (&pThis->m_USBDevice, DESCRIPTOR_ENDPOINT);
        // assert (pEndpointDesc != 0);

        LogWrite (FromTicT834, LOG_NOTICE, "\r\n");
        LogWrite (FromTicT834, LOG_NOTICE, "Endpoint Descriptor");
        LogWrite (FromTicT834, LOG_NOTICE, "------------------------------");
        LogWrite (FromTicT834, LOG_NOTICE, "bLength\t\t\t%u", pEndpointDesc->bLength);
        LogWrite (FromTicT834, LOG_NOTICE, "bDescriptorType\t\t%u", pEndpointDesc->bDescriptorType);
        LogWrite (FromTicT834, LOG_NOTICE, "bEndpointAddress\t0x%x", pEndpointDesc->bEndpointAddress);
        LogWrite (FromTicT834, LOG_NOTICE, "bmAttributes\t\t0x%x", pEndpointDesc->bmAttributes);
        LogWrite (FromTicT834, LOG_NOTICE, "wMaxPacketSize\t\t0x%x", pEndpointDesc->wMaxPacketSize);
        LogWrite (FromTicT834, LOG_NOTICE, "bInterval\t\t0x%x", pEndpointDesc->bInterval);
        LogWrite (FromTicT834, LOG_NOTICE, "------------------------------");
    }


    // String Descriptor
    TUSBStringDescriptor *pStringDesc =
        (TUSBStringDescriptor *) USBDeviceGetDescriptor (&pThis->m_USBDevice, DESCRIPTOR_STRING);
    // assert (pEndpointDesc != 0);

    LogWrite (FromTicT834, LOG_NOTICE, "\r\n");
    LogWrite (FromTicT834, LOG_NOTICE, "String Descriptor");
    LogWrite (FromTicT834, LOG_NOTICE, "------------------------------");
    LogWrite (FromTicT834, LOG_NOTICE, "bLength\t\t\t%u", pStringDesc->bLength);
    LogWrite (FromTicT834, LOG_NOTICE, "bDescriptorType\t\t%u", pStringDesc->bDescriptorType);
    LogWrite (FromTicT834, LOG_NOTICE, "bString\t\t\t%u", pStringDesc->bString);
    LogWrite (FromTicT834, LOG_NOTICE, "------------------------------");

    LogWrite (FromTicT834, LOG_NOTICE, "\r\n");
}