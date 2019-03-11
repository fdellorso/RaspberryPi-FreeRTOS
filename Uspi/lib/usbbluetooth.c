//
// usbbluetooth.cpp
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
#include <uspi/usbbluetooth.h>
#include <uspi/usbhostcontroller.h>
#include <uspi/devicenameservice.h>
#include <uspi/assert.h>
#include <uspi/util.h>
#include <uspios.h>

static const char FromBluetooth[] = "btusb";

static unsigned s_nDeviceNumber = 1;

static boolean USBBluetoothDeviceStartRequest (TUSBBluetoothDevice *pThis);
static void USBBluetoothDeviceCompletionRoutine (TUSBRequest *pURB, void *pParam, void *pContext);

void USBBluetoothDevice (TUSBBluetoothDevice *pThis, TUSBFunction *pFunction)
{
	assert (pThis != 0);

	USBFunctionCopy (&pThis->m_USBFunction, pFunction);
	pThis->m_USBFunction.Configure = USBBluetoothDeviceConfigure;

	pThis->m_pEndpointInterrupt	= 0;
	pThis->m_pEndpointBulkIn	= 0;
	pThis->m_pEndpointBulkOut	= 0;
	pThis->m_pURB				= 0;
	pThis->m_pEventBuffer		= 0;
	pThis->m_pEventHandler		= 0;
}

void _USBBluetoothDevice (TUSBBluetoothDevice *pThis)
{
	assert (pThis != 0);

	if (pThis->m_pEventHandler != 0)
    {
        pThis->m_pEventHandler = 0;
    }

	if (pThis->m_pEventBuffer != 0)
	{
		free (pThis->m_pEventBuffer);
		pThis->m_pEventBuffer = 0;
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

	if (pThis->m_pEndpointInterrupt != 0)
    {
        _USBEndpoint (pThis->m_pEndpointInterrupt);
        free (pThis->m_pEndpointInterrupt);
        pThis->m_pEndpointInterrupt = 0;
    }	

	_USBFunction (&pThis->m_USBFunction);
}

boolean USBBluetoothDeviceConfigure (TUSBFunction *pUSBFunction)
{
	TUSBBluetoothDevice *pThis = (TUSBBluetoothDevice *) pUSBFunction;
	assert (pThis != 0);

	if(USBFunctionGetInterfaceNumber(&pThis->m_USBFunction) != 0)
	{
		LogWrite (FromBluetooth, LOG_WARNING, "Voice channels are not supported");

		return FALSE;
	}

	if(USBFunctionGetNumEndpoints(&pThis->m_USBFunction) != 3)
	{
		USBFunctionConfigurationError (&pThis->m_USBFunction, FromBluetooth);

		return FALSE;
	}

	const TUSBEndpointDescriptor *pEndpointDesc;
	while ((pEndpointDesc = (TUSBEndpointDescriptor *) USBFunctionGetDescriptor (&pThis->m_USBFunction, DESCRIPTOR_ENDPOINT)) != 0)
	{
		if ((pEndpointDesc->bmAttributes & 0x3F) == 0x02)			// Bulk
		{
			if ((pEndpointDesc->bEndpointAddress & 0x80) == 0x80)	// Input
			{
				if (pThis->m_pEndpointBulkIn != 0)
				{
					USBFunctionConfigurationError (&pThis->m_USBFunction, FromBluetooth);

					return FALSE;
				}

				pThis->m_pEndpointBulkIn = (TUSBEndpoint *) malloc (sizeof (TUSBEndpoint));
				assert (pThis->m_pEndpointBulkIn != 0);
				USBEndpoint2 (pThis->m_pEndpointBulkIn, USBFunctionGetDevice (&pThis->m_USBFunction), pEndpointDesc);
			}
			else													// Output
			{
				if (pThis->m_pEndpointBulkOut != 0)
				{
					USBFunctionConfigurationError (&pThis->m_USBFunction, FromBluetooth);

					return FALSE;
				}

				pThis->m_pEndpointBulkOut = (TUSBEndpoint *) malloc (sizeof (TUSBEndpoint));
				assert (pThis->m_pEndpointBulkOut != 0);
				USBEndpoint2 (pThis->m_pEndpointBulkOut, USBFunctionGetDevice (&pThis->m_USBFunction), pEndpointDesc);
			}
		}
		else if ((pEndpointDesc->bmAttributes & 0x3F) == 0x03)		// Interrupt
		{
			if (pThis->m_pEndpointInterrupt != 0)
			{
				USBFunctionConfigurationError (&pThis->m_USBFunction, FromBluetooth);

				return FALSE;
			}

			pThis->m_pEndpointInterrupt = (TUSBEndpoint *) malloc (sizeof (TUSBEndpoint));
			assert (pThis->m_pEndpointInterrupt != 0);
			USBEndpoint2 (pThis->m_pEndpointInterrupt, USBFunctionGetDevice (&pThis->m_USBFunction), pEndpointDesc);
		}
	}

	if (   pThis->m_pEndpointBulkIn    == 0
	    || pThis->m_pEndpointBulkOut   == 0
	    || pThis->m_pEndpointInterrupt == 0)
	{
		USBFunctionConfigurationError (&pThis->m_USBFunction, FromBluetooth);

		return FALSE;
	}

	// Configuration Check
    if (!USBFunctionConfigure (&pThis->m_USBFunction))
    {
        LogWrite (FromBluetooth, LOG_ERROR, "Cannot set configuration");

        return FALSE;
    }

	// if (pThis->m_ucAlternateSetting != 0)
	// {
	// 	if (DWHCIDeviceControlMessage (USBFunctionGetHost (&pThis->m_USBFunction),
	// 		USBFunctionGetEndpoint0 (&pThis->m_USBFunction), REQUEST_OUT | REQUEST_TO_INTERFACE, SET_INTERFACE,
	// 		pThis->m_ucAlternateSetting, pThis->m_ucInterfaceNumber, 0, 0) < 0)
	// 	{
	// 		LogWrite (FromBluetooth, LOG_ERROR, "Cannot set interface");

	// 		return FALSE;
	// 	}
	// }

	pThis->m_pEventBuffer = (u8 *) malloc (pThis->m_pEndpointInterrupt->m_nMaxPacketSize);
	assert (pThis->m_pEventBuffer != 0);

	//DNS ADD
	TString DeviceName;
	String (&DeviceName);
	StringFormat (&DeviceName, "ubt%u", s_nDeviceNumber++);
	DeviceNameServiceAddDevice (DeviceNameServiceGet (), StringGet (&DeviceName), pThis, FALSE);

	_String (&DeviceName);

	return TRUE;
}

boolean USBBluetoothDeviceSendHCICommand (TUSBBluetoothDevice *pThis, const void *pBuffer, unsigned nLength)
{
	if (DWHCIDeviceControlMessage (USBFunctionGetHost (&pThis->m_USBFunction),
		USBFunctionGetEndpoint0 (&pThis->m_USBFunction), REQUEST_OUT | REQUEST_CLASS | REQUEST_TO_DEVICE,
		0, 0, 0, (void *) pBuffer, nLength) < 0)
	{
		return FALSE;
	}

	return TRUE;
}

void USBBluetoothDeviceRegisterHCIEventHandler (TUSBBluetoothDevice *pThis, TBTHCIEventHandler *pHandler)
{
	pThis->m_pEventHandler = pHandler;
	assert (pThis->m_pEventHandler != 0);

	USBBluetoothDeviceStartRequest (pThis);
}

static boolean USBBluetoothDeviceStartRequest (TUSBBluetoothDevice *pThis)
{
	assert (pThis != 0);

	assert (pThis->m_pEndpointInterrupt  != 0);
	assert (pThis->m_pEventBuffer != 0);

	assert (pThis->m_pURB == 0);

	pThis->m_pURB = malloc (sizeof (TUSBRequest));
	assert (pThis->m_pURB != 0);
	
	USBRequest (pThis->m_pURB, pThis->m_pEndpointInterrupt, pThis->m_pEventBuffer, sizeof (pThis->m_pEventBuffer), 0);
	USBRequestSetCompletionRoutine (pThis->m_pURB, USBBluetoothDeviceCompletionRoutine, 0, pThis);

	return DWHCIDeviceSubmitAsyncRequest (USBFunctionGetHost (&pThis->m_USBFunction), pThis->m_pURB);
}

static void USBBluetoothDeviceCompletionRoutine (TUSBRequest *pURB, void *pParam, void *pContext)
{
	(void)pParam;   // FIXME Wunused


	TUSBBluetoothDevice *pThis = (TUSBBluetoothDevice *) pContext;
	assert (pThis != 0);

	assert (pURB != 0);
	assert (pThis->m_pURB == pURB);
	assert (pThis->m_pEventBuffer != 0);

	if (   USBRequestGetStatus (pURB) != 0
	    && USBRequestGetResultLength (pURB) > 0)
	{
		assert (pThis->m_pEventHandler != 0);
		// (*m_pEventHandler) (m_pEventBuffer, pURB->GetResultLength ());		// TODO Translate in C
	}
	else
	{
		LogWrite (FromBluetooth, LOG_ERROR, "Request failed");
	}

	_USBRequest (pThis->m_pURB);
	free (pThis->m_pURB);
	pThis->m_pURB = 0;

	if ( !USBBluetoothDeviceStartRequest (pThis))
	{
		LogWrite (FromBluetooth, LOG_ERROR, "Cannot restart request");
	}
}
