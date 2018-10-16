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

void USBTicT834Device (TUSBTicT834Device *pThis, TUSBDevice *pDevice)
{
	assert (pThis != 0);

	USBDeviceCopy (&pThis->m_USBDevice, pDevice);
	pThis->m_USBDevice.Configure = USBTicT834DeviceConfigure;

	pThis->m_pEndpointIn = 0;
    pThis->m_pEndpointOut = 0;

    pThis->serial_number= 0;
	pThis->os_id = 0;
	pThis->firmware_version = 0;
	pThis->product = 0;
    pThis->cached_firmware_version_string = 0;
}

void _USBTicT834Device (TUSBTicT834Device *pThis)
{
	assert (pThis != 0);

	if (pThis->m_pEndpointIn != 0)
	{
		_USBEndpoint (pThis->m_pEndpointIn);
		free (pThis->m_pEndpointIn);
		pThis->m_pEndpointIn = 0;
	}

    if (pThis->m_pEndpointOut != 0)
    {
        _USBEndpoint (pThis->m_pEndpointOut);
        free (pThis->m_pEndpointOut);
        pThis->m_pEndpointOut = 0;
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

    // LogWrite (FromTicT834, LOG_NOTICE, "bLength %u", pDeviceDesc->bLength);
    // LogWrite (FromTicT834, LOG_NOTICE, "bDescriptorType %u", pDeviceDesc->bDescriptorType);
    // LogWrite (FromTicT834, LOG_NOTICE, "bcdUSB %u", pDeviceDesc->bcdUSB);
    // LogWrite (FromTicT834, LOG_NOTICE, "bDeviceClass %u", pDeviceDesc->bDeviceClass);
    // LogWrite (FromTicT834, LOG_NOTICE, "bDeviceSubClass %u", pDeviceDesc->bDeviceSubClass);
    // LogWrite (FromTicT834, LOG_NOTICE, "bDeviceProtocol %u", pDeviceDesc->bDeviceProtocol);
    // LogWrite (FromTicT834, LOG_NOTICE, "bMaxPacketSize0 %u", pDeviceDesc->bMaxPacketSize0);
    // LogWrite (FromTicT834, LOG_NOTICE, "idVendor %u", pDeviceDesc->idVendor);
    // LogWrite (FromTicT834, LOG_NOTICE, "idProduct %u", pDeviceDesc->idProduct);
    // LogWrite (FromTicT834, LOG_NOTICE, "bcdDevice %u", pDeviceDesc->bcdDevice);
    // LogWrite (FromTicT834, LOG_NOTICE, "iManufacturer %u", pDeviceDesc->iManufacturer);
    // LogWrite (FromTicT834, LOG_NOTICE, "iProduct %u", pDeviceDesc->iProduct);
    // LogWrite (FromTicT834, LOG_NOTICE, "iSerialNumber %u", pDeviceDesc->iSerialNumber);
    // LogWrite (FromTicT834, LOG_NOTICE, "bNumConfigurations %u", pDeviceDesc->bNumConfigurations);

	// if (   pDeviceDesc->bDeviceClass       != USB_DEVICE_CLASS_HUB
	//     || pDeviceDesc->bDeviceSubClass    != 0
	//     || pDeviceDesc->bDeviceProtocol    != 2		// hub with multiple TTs
	//     || pDeviceDesc->bNumConfigurations != 1)
	// {
	// 	LogWrite (FromHub, LOG_ERROR, "Unsupported hub (proto %u)",
	// 		     (unsigned) pDeviceDesc->bDeviceProtocol);

	// 	return FALSE;
	// }

    // Configurator Descriptor
	TUSBConfigurationDescriptor *pConfDesc =
		(TUSBConfigurationDescriptor *) USBDeviceGetDescriptor (&pThis->m_USBDevice, DESCRIPTOR_CONFIGURATION);
	assert (pConfDesc != 0);

    // LogWrite (FromTicT834, LOG_NOTICE, "bLength %u", pConfDesc->bLength);
    // LogWrite (FromTicT834, LOG_NOTICE, "bDescriptorType %u", pConfDesc->bDescriptorType);
    // LogWrite (FromTicT834, LOG_NOTICE, "wTotalLength %u", pConfDesc->wTotalLength);
    // LogWrite (FromTicT834, LOG_NOTICE, "bNumInterfaces %u", pConfDesc->bNumInterfaces);
    // LogWrite (FromTicT834, LOG_NOTICE, "bConfigurationValue %u", pConfDesc->bConfigurationValue);
    // LogWrite (FromTicT834, LOG_NOTICE, "iConfiguration %u", pConfDesc->iConfiguration);
    // LogWrite (FromTicT834, LOG_NOTICE, "bmAttributes %u", pConfDesc->bmAttributes);
    // LogWrite (FromTicT834, LOG_NOTICE, "bMaxPower %u", pConfDesc->bMaxPower);

    // if (   pConfDesc == 0
	//     || pConfDesc->bNumInterfaces <  1)
	// {
	// 	USBDeviceConfigurationError (&pThis->m_USBDevice, FromTicT834);

	// 	return FALSE;
	// }

    // Interface Descriptor
    TUSBInterfaceDescriptor *pInterfaceDesc =
        (TUSBInterfaceDescriptor *) USBDeviceGetDescriptor (&pThis->m_USBDevice, DESCRIPTOR_INTERFACE);
    assert (pInterfaceDesc != 0);

    // LogWrite (FromTicT834, LOG_NOTICE, "bLength %u", pInterfaceDesc->bLength);
    // LogWrite (FromTicT834, LOG_NOTICE, "bDescriptorType %u", pInterfaceDesc->bDescriptorType);
    // LogWrite (FromTicT834, LOG_NOTICE, "bInterfaceNumber %u", pInterfaceDesc->bInterfaceNumber);
    // LogWrite (FromTicT834, LOG_NOTICE, "bAlternateSetting %u", pInterfaceDesc->bAlternateSetting);
    // LogWrite (FromTicT834, LOG_NOTICE, "bNumEndpoints %u", pInterfaceDesc->bNumEndpoints);
    // LogWrite (FromTicT834, LOG_NOTICE, "bInterfaceClass %u", pInterfaceDesc->bInterfaceClass);
    // LogWrite (FromTicT834, LOG_NOTICE, "bInterfaceSubClass %u", pInterfaceDesc->bInterfaceSubClass);
    // LogWrite (FromTicT834, LOG_NOTICE, "bInterfaceProtocol %u", pInterfaceDesc->bInterfaceProtocol);
    // LogWrite (FromTicT834, LOG_NOTICE, "iInterface %u", pInterfaceDesc->iInterface);

    // Endpoint Descriptor
    TUSBEndpointDescriptor *pEndpointDesc =
        (TUSBEndpointDescriptor *) USBDeviceGetDescriptor (&pThis->m_USBDevice, DESCRIPTOR_ENDPOINT);
    // assert (pEndpointDesc != 0);

    // LogWrite (FromTicT834, LOG_NOTICE, "bLength %u", pEndpointDesc->bLength);
    // LogWrite (FromTicT834, LOG_NOTICE, "bDescriptorType %u", pEndpointDesc->bDescriptorType);
    // LogWrite (FromTicT834, LOG_NOTICE, "bEndpointAddress %u", pEndpointDesc->bEndpointAddress);
    // LogWrite (FromTicT834, LOG_NOTICE, "bmAttributes %u", pEndpointDesc->bmAttributes);
    // LogWrite (FromTicT834, LOG_NOTICE, "wMaxPacketSize %u", pEndpointDesc->wMaxPacketSize);
    // LogWrite (FromTicT834, LOG_NOTICE, "bInterval %u", pEndpointDesc->bInterval);


    // if (   pInterfaceDesc == 0
    //     || pInterfaceDesc->bNumEndpoints      <  1
    //     || pInterfaceDesc->bInterfaceClass    != 0x03   // HID Class
    //     || pInterfaceDesc->bInterfaceSubClass != 0x00   // Boot Interface Subclass
    //     || pInterfaceDesc->bInterfaceProtocol != 0x00)  // GamePad
    // {
    //     USBDeviceConfigurationError (&pThis->m_USBDevice, FromTicT834);

    //     return FALSE;
    // }

    // pThis->m_ucInterfaceNumber  = pInterfaceDesc->bInterfaceNumber;
    // pThis->m_ucAlternateSetting = pInterfaceDesc->bAlternateSetting;

    // // HID Descriptor
    // TUSBHIDDescriptor *pHIDDesc = (TUSBHIDDescriptor *) USBDeviceGetDescriptor (&pThis->m_USBDevice, DESCRIPTOR_HID);
    // if (   pHIDDesc == 0
    //     || pHIDDesc->wReportDescriptorLength == 0)
    // {
    //     USBDeviceConfigurationError (&pThis->m_USBDevice, FromTicT834);

    //     return FALSE;
    // }

    // Endpoint Descriptor
    // const TUSBEndpointDescriptor *pEndpointDesc;
    // while ((pEndpointDesc = (TUSBEndpointDescriptor *) USBDeviceGetDescriptor (&pThis->m_USBDevice, DESCRIPTOR_ENDPOINT)) != 0)
    // {
    //     if ((pEndpointDesc->bmAttributes & 0x3F) == 0x03)       // Interrupt
    //     {
    //         if ((pEndpointDesc->bEndpointAddress & 0x80) == 0x80)   // Input
    //         {
    //             if (pThis->m_pEndpointIn != 0)
    //             {
    //                 USBDeviceConfigurationError (&pThis->m_USBDevice, FromTicT834);

    //                 return FALSE;
    //             }

    //             pThis->m_pEndpointIn = (TUSBEndpoint *) malloc (sizeof (TUSBEndpoint));
    //             assert (pThis->m_pEndpointIn != 0);
    //             USBEndpoint2 (pThis->m_pEndpointIn, &pThis->m_USBDevice, pEndpointDesc);
    //         }
    //         else                            // Output
    //         {
    //             if (pThis->m_pEndpointOut != 0)
    //             {
    //                 USBDeviceConfigurationError (&pThis->m_USBDevice, FromTicT834);

    //                 return FALSE;
    //             }

    //             pThis->m_pEndpointOut = (TUSBEndpoint *) malloc (sizeof (TUSBEndpoint));
    //             assert (pThis->m_pEndpointOut != 0);
    //             USBEndpoint2 (pThis->m_pEndpointOut, &pThis->m_USBDevice, pEndpointDesc);
    //         }
    //     }
    // }

    // Endpoint Check
    // if (   pThis->m_pEndpointIn  == 0
	//     || pThis->m_pEndpointOut == 0)
	// {
	// 	USBDeviceConfigurationError (&pThis->m_USBDevice, FromTicT834);

	// 	return FALSE;
	// }

    // Configuration Check
    if (!USBDeviceConfigure (&pThis->m_USBDevice))
    {
        LogWrite (FromTicT834, LOG_ERROR, "Cannot set configuration");

        return FALSE;
    }

    // pThis->m_usReportDescriptorLength = pHIDDesc->wReportDescriptorLength;
    // pThis->m_pHIDReportDescriptor = (unsigned char *) malloc(pHIDDesc->wReportDescriptorLength);
    // assert (pThis->m_pHIDReportDescriptor != 0);

    // if (DWHCIDeviceGetDescriptor (USBDeviceGetHost (&pThis->m_USBDevice),
    //                 USBDeviceGetEndpoint0 (&pThis->m_USBDevice),
    //                 pHIDDesc->bReportDescriptorType, DESCRIPTOR_INDEX_DEFAULT,
    //                 pThis->m_pHIDReportDescriptor, pHIDDesc->wReportDescriptorLength, REQUEST_IN)
    //     != pHIDDesc->wReportDescriptorLength)
    // {
    //     LogWrite (FromTicT834, LOG_ERROR, "Cannot get HID report descriptor");

    //     return FALSE;
    // }
    // //DebugHexdump (pThis->m_pHIDReportDescriptor, pHIDDesc->wReportDescriptorLength, "hid");

    // pThis->m_pReportBuffer[0] = 0;
    // USBGamePadDeviceDecodeReport (pThis);

    // if (pThis->m_ucAlternateSetting != 0)
    // {
    //     if (DWHCIDeviceControlMessage (USBDeviceGetHost (&pThis->m_USBDevice),
    //                     USBDeviceGetEndpoint0 (&pThis->m_USBDevice),
    //                     REQUEST_OUT | REQUEST_TO_INTERFACE, SET_INTERFACE,
    //                     pThis->m_ucAlternateSetting,
    //                     pThis->m_ucInterfaceNumber, 0, 0) < 0)
    //     {
    //         LogWrite (FromTicT834, LOG_ERROR, "Cannot set interface");

    //         return FALSE;
    //     }
    // }

    //DNS ADD
	TString DeviceName;
	String (&DeviceName);
	StringFormat (&DeviceName, "tic%u", s_nDeviceNumber++);
	DeviceNameServiceAddDevice (DeviceNameServiceGet (), StringGet (&DeviceName), pThis, FALSE);

	_String (&DeviceName);

	return TRUE;
}

// boolean USBTicT834DeviceSendFrame (TUSBTicT834Device *pThis, const void *pBuffer, unsigned nLength)
// {
// 	assert (pThis != 0);

// 	if (nLength >= FRAME_BUFFER_SIZE-8)
// 	{
// 		return FALSE;
// 	}

// 	assert (pThis->m_pTxBuffer != 0);
// 	assert (pBuffer != 0);
// 	memcpy2 (pThis->m_pTxBuffer+8, pBuffer, nLength);
	
// 	*(u32 *) &pThis->m_pTxBuffer[0] = TX_CMD_A_FIRST_SEG | TX_CMD_A_LAST_SEG | nLength;
// 	*(u32 *) &pThis->m_pTxBuffer[4] = nLength;
	
// 	assert (pThis->m_pEndpointBulkOut != 0);

// 	return DWHCIDeviceTransfer (USBDeviceGetHost (&pThis->m_USBDevice), pThis->m_pEndpointBulkOut, pThis->m_pTxBuffer, nLength+8) >= 0;
// }

// boolean USBTicT834DeviceReceiveFrame (TUSBTicT834Device *pThis, void *pBuffer, unsigned *pResultLength)
// {
// 	assert (pThis != 0);

// 	assert (pThis->m_pEndpointBulkIn != 0);
// 	assert (pBuffer != 0);
// 	TUSBRequest URB;
// 	USBRequest (&URB, pThis->m_pEndpointBulkIn, pBuffer, FRAME_BUFFER_SIZE, 0);

// 	if (!DWHCIDeviceSubmitBlockingRequest (USBDeviceGetHost (&pThis->m_USBDevice), &URB))
// 	{
// 		_USBRequest (&URB);

// 		return FALSE;
// 	}

// 	u32 nResultLength = USBRequestGetResultLength (&URB);
// 	if (nResultLength < 4)				// should not happen with HW_CFG_BIR set
// 	{
// 		_USBRequest (&URB);

// 		return FALSE;
// 	}

// 	u32 nRxStatus = *(u32 *) pBuffer;
// 	if (nRxStatus & RX_STS_ERROR)
// 	{
// 		LogWrite (FromSMSC951x, LOG_WARNING, "RX error (status 0x%X)", nRxStatus);

// 		_USBRequest (&URB);

// 		return FALSE;
// 	}
	
// 	u32 nFrameLength = RX_STS_FRAMELEN (nRxStatus);
// 	assert (nFrameLength == nResultLength-4);
// 	assert (nFrameLength > 4);
// 	if (nFrameLength <= 4)
// 	{
// 		_USBRequest (&URB);

// 		return FALSE;
// 	}
// 	nFrameLength -= 4;	// ignore CRC

// 	//LogWrite (FromSMSC951x, LOG_DEBUG, "Frame received (status 0x%X)", nRxStatus);

// 	memcpy2 (pBuffer, (u8 *) pBuffer + 4, nFrameLength);	// overwrite RX status

// 	assert (pResultLength != 0);
// 	*pResultLength = nFrameLength;
	
// 	_USBRequest (&URB);

// 	return TRUE;
// }

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

// boolean USBTicT834DeviceReadReg (TUSBTicT834Device *pThis, u32 nIndex, u32 *pValue)
// {
// 	assert (pThis != 0);

// 	return DWHCIDeviceControlMessage (USBDeviceGetHost (&pThis->m_USBDevice),
// 					  USBDeviceGetEndpoint0 (&pThis->m_USBDevice),
// 					  REQUEST_IN | REQUEST_VENDOR, READ_REGISTER,
// 					  0, nIndex, pValue, sizeof *pValue) == (int) sizeof *pValue;
// }
