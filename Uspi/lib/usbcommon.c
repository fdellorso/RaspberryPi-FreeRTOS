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
#include <uspi/usbcommon.h>
#include <uspi/usbhostcontroller.h>
#include <uspi/devicenameservice.h>
#include <uspi/assert.h>
#include <uspi/util.h>
#include <uspios.h>

void USBDescriptorPrinter(TUSBDevice *pThis, const char *FromUsb)
{
    const TUSBDeviceDescriptor *pDeviceDesc = 
        USBDeviceGetDeviceDescriptor (pThis);
	assert (pDeviceDesc != 0);

    LogWrite (FromUsb, LOG_NOTICE, "\r\n");
    LogWrite (FromUsb, LOG_NOTICE, "Device Descriptor");
    LogWrite (FromUsb, LOG_NOTICE, "------------------------------");
    LogWrite (FromUsb, LOG_NOTICE, "bLength\t\t\t%u", pDeviceDesc->bLength);
    LogWrite (FromUsb, LOG_NOTICE, "bDescriptorType\t\t%u", pDeviceDesc->bDescriptorType);
    LogWrite (FromUsb, LOG_NOTICE, "bcdUSB\t\t\t0x%x", pDeviceDesc->bcdUSB);
    LogWrite (FromUsb, LOG_NOTICE, "bDeviceClass\t\t0x%x", pDeviceDesc->bDeviceClass);
    LogWrite (FromUsb, LOG_NOTICE, "bDeviceSubClass\t\t0x%x", pDeviceDesc->bDeviceSubClass);
    LogWrite (FromUsb, LOG_NOTICE, "bDeviceProtocol\t\t0x%x", pDeviceDesc->bDeviceProtocol);
    LogWrite (FromUsb, LOG_NOTICE, "bMaxPacketSize0\t\t%u", pDeviceDesc->bMaxPacketSize0);
    LogWrite (FromUsb, LOG_NOTICE, "idVendor\t\t0x%x", pDeviceDesc->idVendor);
    LogWrite (FromUsb, LOG_NOTICE, "idProduct\t\t0x%x", pDeviceDesc->idProduct);
    LogWrite (FromUsb, LOG_NOTICE, "bcdDevice\t\t0x%x", pDeviceDesc->bcdDevice);
    LogWrite (FromUsb, LOG_NOTICE, "iManufacturer\t\t%u", pDeviceDesc->iManufacturer);
    LogWrite (FromUsb, LOG_NOTICE, "iProduct\t\t%u", pDeviceDesc->iProduct);
    LogWrite (FromUsb, LOG_NOTICE, "iSerialNumber\t\t%u", pDeviceDesc->iSerialNumber);
    LogWrite (FromUsb, LOG_NOTICE, "bNumConfigurations\t%u", pDeviceDesc->bNumConfigurations);
    LogWrite (FromUsb, LOG_NOTICE, "------------------------------");


    // Configurator Descriptor
	const TUSBConfigurationDescriptor *pConfDesc =
		USBDeviceGetConfigurationDescriptor (pThis);
	assert (pConfDesc != 0);

    LogWrite (FromUsb, LOG_NOTICE, "\r\n");
    LogWrite (FromUsb, LOG_NOTICE, "Configuration Descriptor");
    LogWrite (FromUsb, LOG_NOTICE, "------------------------------");
    LogWrite (FromUsb, LOG_NOTICE, "bLength\t\t\t%u", pConfDesc->bLength);
    LogWrite (FromUsb, LOG_NOTICE, "bDescriptorType\t\t%u", pConfDesc->bDescriptorType);
    LogWrite (FromUsb, LOG_NOTICE, "wTotalLength\t\t%u", pConfDesc->wTotalLength);
    LogWrite (FromUsb, LOG_NOTICE, "bNumInterfaces\t\t%u", pConfDesc->bNumInterfaces);
    LogWrite (FromUsb, LOG_NOTICE, "bConfigurationValue\t%u", pConfDesc->bConfigurationValue);
    LogWrite (FromUsb, LOG_NOTICE, "iConfiguration\t\t%u", pConfDesc->iConfiguration);
    LogWrite (FromUsb, LOG_NOTICE, "bmAttributes\t\t0x%x", pConfDesc->bmAttributes);
    LogWrite (FromUsb, LOG_NOTICE, "bMaxPower\t\t%u", pConfDesc->bMaxPower);
    LogWrite (FromUsb, LOG_NOTICE, "------------------------------");


    // Interface Descriptor
    const TUSBInterfaceDescriptor *pInterfaceDesc; // = NULL;
        // (TUSBInterfaceDescriptor *) USBDeviceGetDescriptor (pThis, DESCRIPTOR_INTERFACE);
    // assert (pInterfaceDesc != 0);
    while ((pInterfaceDesc = (TUSBInterfaceDescriptor *) USBDeviceGetDescriptor (pThis, DESCRIPTOR_INTERFACE)) != 0) {
        LogWrite (FromUsb, LOG_NOTICE, "\r\n");
        LogWrite (FromUsb, LOG_NOTICE, "Interface Descriptor");
        LogWrite (FromUsb, LOG_NOTICE, "------------------------------");
        LogWrite (FromUsb, LOG_NOTICE, "bLength\t\t\t%u", pInterfaceDesc->bLength);
        LogWrite (FromUsb, LOG_NOTICE, "bDescriptorType\t\t%u", pInterfaceDesc->bDescriptorType);
        LogWrite (FromUsb, LOG_NOTICE, "bInterfaceNumber\t%u", pInterfaceDesc->bInterfaceNumber);
        LogWrite (FromUsb, LOG_NOTICE, "bAlternateSetting\t%u", pInterfaceDesc->bAlternateSetting);
        LogWrite (FromUsb, LOG_NOTICE, "bNumEndpoints\t\t%u", pInterfaceDesc->bNumEndpoints);
        LogWrite (FromUsb, LOG_NOTICE, "bInterfaceClass\t\t0x%x", pInterfaceDesc->bInterfaceClass);
        LogWrite (FromUsb, LOG_NOTICE, "bInterfaceSubClass\t0x%x", pInterfaceDesc->bInterfaceSubClass);
        LogWrite (FromUsb, LOG_NOTICE, "bInterfaceProtocol\t0x%x", pInterfaceDesc->bInterfaceProtocol);
        LogWrite (FromUsb, LOG_NOTICE, "iInterface\t\t%u", pInterfaceDesc->iInterface);
        LogWrite (FromUsb, LOG_NOTICE, "------------------------------");

        // Endpoint Descriptor
        if (pInterfaceDesc->bNumEndpoints > 0)
        {
            const TUSBEndpointDescriptor *pEndpointDesc; // = NULL;
                // (TUSBEndpointDescriptor *) USBDeviceGetDescriptor (pThis, DESCRIPTOR_ENDPOINT);
            // assert (pEndpointDesc != 0);
            while ((pEndpointDesc = (TUSBEndpointDescriptor *) USBDeviceGetDescriptor (pThis, DESCRIPTOR_ENDPOINT)) != 0) {
                LogWrite (FromUsb, LOG_NOTICE, "\r\n");
                LogWrite (FromUsb, LOG_NOTICE, "Endpoint Descriptor");
                LogWrite (FromUsb, LOG_NOTICE, "------------------------------");
                LogWrite (FromUsb, LOG_NOTICE, "bLength\t\t\t%u", pEndpointDesc->bLength);
                LogWrite (FromUsb, LOG_NOTICE, "bDescriptorType\t\t%u", pEndpointDesc->bDescriptorType);
                LogWrite (FromUsb, LOG_NOTICE, "bEndpointAddress\t0x%x", pEndpointDesc->bEndpointAddress);
                LogWrite (FromUsb, LOG_NOTICE, "bmAttributes\t\t0x%x", pEndpointDesc->bmAttributes);
                LogWrite (FromUsb, LOG_NOTICE, "wMaxPacketSize\t\t0x%x", pEndpointDesc->wMaxPacketSize);
                LogWrite (FromUsb, LOG_NOTICE, "bInterval\t\t0x%x", pEndpointDesc->bInterval);
                LogWrite (FromUsb, LOG_NOTICE, "------------------------------");
            }  
        }
    }


    // String Descriptor
    TUSBStringDescriptor *pStringDesc =
        (TUSBStringDescriptor *) USBDeviceGetDescriptor (pThis, DESCRIPTOR_STRING);
    // assert (pEndpointDesc != 0);

    LogWrite (FromUsb, LOG_NOTICE, "\r\n");
    LogWrite (FromUsb, LOG_NOTICE, "String Descriptor");
    LogWrite (FromUsb, LOG_NOTICE, "------------------------------");
    LogWrite (FromUsb, LOG_NOTICE, "bLength\t\t\t%u", pStringDesc->bLength);
    LogWrite (FromUsb, LOG_NOTICE, "bDescriptorType\t\t%u", pStringDesc->bDescriptorType);
    LogWrite (FromUsb, LOG_NOTICE, "bString\t\t\t%u", pStringDesc->bString);
    LogWrite (FromUsb, LOG_NOTICE, "------------------------------");

    LogWrite (FromUsb, LOG_NOTICE, "\r\n");
}