//
// uspilibrary.c
//
// USPi - An USB driver for Raspberry Pi written in C
// Copyright (C) 2014  R. Stange <rsta2@o2online.de>
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
#include <uspi/uspilibrary.h>
#include <uspi.h>
#include <uspios.h>
#include <uspi/usbdevice.h>
#include <uspi/string.h>
#include <uspi/util.h>
#include <uspi/assert.h>

static const char FromUSPi[] = "uspi";

static TUSPiLibrary *s_pLibrary = 0;


int USPiTicQuick (unsigned char nCommand);
int USPiTic7BitWrite (unsigned char nCommand, unsigned short nValue);
int USPiTic32BitWrite (unsigned char nCommand, unsigned short nValue, unsigned int nIndex);
int USPiTicBlockRead (unsigned char nCommand, unsigned int nIndex, unsigned short nLength, unsigned int *nData);


int USPiInitialize (void)
{
	assert (s_pLibrary == 0);
	s_pLibrary = (TUSPiLibrary *) malloc (sizeof (TUSPiLibrary));
	assert (s_pLibrary != 0);

	DeviceNameService (&s_pLibrary->NameService);
	DWHCIDevice (&s_pLibrary->DWHCI);
	s_pLibrary->pEth0 = 0;

	if (!DWHCIDeviceInitialize (&s_pLibrary->DWHCI))
	{
		LogWrite (FromUSPi, LOG_ERROR, "Cannot initialize USB host controller interface");

		_DWHCIDevice (&s_pLibrary->DWHCI);
		_DeviceNameService (&s_pLibrary->NameService);
		free (s_pLibrary);
		s_pLibrary = 0;

		return 0;
	}

	s_pLibrary->pUKBD1 = (TUSBKeyboardDevice *) DeviceNameServiceGetDevice (DeviceNameServiceGet (), "ukbd1", FALSE);

	s_pLibrary->pUMouse1 = (TUSBMouseDevice *) DeviceNameServiceGetDevice (DeviceNameServiceGet (), "umouse1", FALSE);

	for (unsigned i = 0; i < MAX_DEVICES; i++)
	{
		TString DeviceName;
		String  (&DeviceName);
		StringFormat (&DeviceName, "umsd%u", i+1);

		s_pLibrary->pUMSD[i] = (TUSBBulkOnlyMassStorageDevice *)
			DeviceNameServiceGetDevice (DeviceNameServiceGet (), StringGet (&DeviceName), TRUE);

		_String  (&DeviceName);
	}

	s_pLibrary->pEth0 = (TSMSC951xDevice *) DeviceNameServiceGetDevice (DeviceNameServiceGet (), "eth0", FALSE);

	for (unsigned i = 0; i < MAX_DEVICES; i++)
	{
		TString DeviceName;
		String  (&DeviceName);
		StringFormat (&DeviceName, "upad%u", i+1);

		s_pLibrary->pUPAD[i] = (TUSBGamePadDevice *)
			DeviceNameServiceGetDevice (DeviceNameServiceGet (), StringGet (&DeviceName), FALSE);

		_String  (&DeviceName);
	}

	s_pLibrary->pTic1 = (TUSBTicT834Device *) DeviceNameServiceGetDevice (DeviceNameServiceGet (), "tic0", FALSE);

	s_pLibrary->pUbt1 = (TUSBBluetoothDevice *) DeviceNameServiceGetDevice (DeviceNameServiceGet (), "ubt1", FALSE);

	LogWrite (FromUSPi, LOG_DEBUG, "USPi library successfully initialized");

	return 1;
}

int USPiTicAvailable (void)
{
	assert (s_pLibrary != 0);
	return s_pLibrary->pTic1 != 0;
}

int USPiTicQuick (unsigned char nCommand)
{
	assert (s_pLibrary != 0);
	assert (s_pLibrary->pTic1 != 0);
	return USBTicT834DeviceWriteReg (s_pLibrary->pTic1, nCommand, 0, 0, 0, NULL) ? 1 : 0;
}

int USPiTic7BitWrite (unsigned char nCommand, unsigned short nValue)
{
	assert (s_pLibrary != 0);
	assert (s_pLibrary->pTic1 != 0);
	return USBTicT834DeviceWriteReg (s_pLibrary->pTic1, nCommand, nValue, 0, 0, NULL) ? 1 : 0;
}

int USPiTic32BitWrite (unsigned char nCommand, unsigned short nValue, unsigned int nIndex)
{
	assert (s_pLibrary != 0);
	assert (s_pLibrary->pTic1 != 0);
	return USBTicT834DeviceWriteReg (s_pLibrary->pTic1, nCommand, nValue, nIndex, 0, NULL) ? 1 : 0;
}

int USPiTicBlockRead (unsigned char nCommand, unsigned int nIndex, unsigned short nLength, unsigned int *nData)
{
	assert (s_pLibrary != 0);
	assert (s_pLibrary->pTic1 != 0);
	return USBTicT834DeviceReadReg (s_pLibrary->pTic1, nCommand, 0, nIndex, nLength, nData) ? 1 : 0;
}

int USPiTicControlTransfer(void * handle,
		unsigned char bmRequestType, unsigned char bRequest,
    	unsigned short wValue, unsigned short wIndex, unsigned char * data,
		unsigned short wLength, unsigned char * transferred)
{
	(void)handle;	// FIXME Wunused

	int result;

	assert (s_pLibrary != 0);
	assert (s_pLibrary->pTic1 != 0);

	result = USBTicT834DeviceControl (s_pLibrary->pTic1, bmRequestType, bRequest, wValue, wIndex, wLength, data);
	
	if (result >= 0) {
		*transferred = result;
		return 0;
	}
	else {
		return -1;
	}
}

int USPiTicStringRead (unsigned char nString, unsigned char *nData)
{
	assert (s_pLibrary != 0);
	assert (s_pLibrary->pTic1 != 0);
	return USBTicT834DeviceReadString (s_pLibrary->pTic1, nString, nData) ? 1 : 0;
}

char * USPiTicGetSerialNumber(void)
{
	assert (s_pLibrary != 0);
	assert (s_pLibrary->pTic1 != 0);
	return USBTicT834DeviceGetSerialNumber(s_pLibrary->pTic1);
}

int USPiBltAvailable (void)
{
	assert (s_pLibrary != 0);
	return s_pLibrary->pUbt1 != 0;
}

int USPiKeyboardAvailable (void)
{
	assert (s_pLibrary != 0);
	return s_pLibrary->pUKBD1 != 0;
}

void USPiKeyboardRegisterKeyPressedHandler (TUSPiKeyPressedHandler *pKeyPressedHandler)
{
	assert (s_pLibrary != 0);
	assert (s_pLibrary->pUKBD1 != 0);
	USBKeyboardDeviceRegisterKeyPressedHandler (s_pLibrary->pUKBD1, pKeyPressedHandler);
}

void USPiKeyboardRegisterShutdownHandler (TUSPiShutdownHandler *pShutdownHandler)
{
	assert (s_pLibrary != 0);
	assert (s_pLibrary->pUKBD1 != 0);
	USBKeyboardDeviceRegisterShutdownHandler (s_pLibrary->pUKBD1, pShutdownHandler);
}

void USPiKeyboardRegisterKeyStatusHandlerRaw (TKeyStatusHandlerRaw *pKeyStatusHandlerRaw)
{
	assert (s_pLibrary != 0);
	assert (s_pLibrary->pUKBD1 != 0);
	USBKeyboardDeviceRegisterKeyStatusHandlerRaw (s_pLibrary->pUKBD1, pKeyStatusHandlerRaw);
}

int USPiMouseAvailable (void)
{
	assert (s_pLibrary != 0);
	return s_pLibrary->pUMouse1 != 0;
}

void USPiMouseRegisterStatusHandler (TUSPiMouseStatusHandler *pStatusHandler)
{
	assert (s_pLibrary != 0);
	assert (s_pLibrary->pUMouse1 != 0);
	USBMouseDeviceRegisterStatusHandler (s_pLibrary->pUMouse1, pStatusHandler);
}

int USPiMassStorageDeviceAvailable (void)
{
	assert (s_pLibrary != 0);

	unsigned i;
	for (i = 0; i < MAX_DEVICES; i++)
	{
		if (s_pLibrary->pUMSD[i] == 0)
		{
			break;
		}
	}

	return (int) i;
}

int USPiMassStorageDeviceRead (unsigned long long ullOffset, void *pBuffer, unsigned nCount, unsigned nDeviceIndex)
{
	assert (s_pLibrary != 0);

	if (   nDeviceIndex >= MAX_DEVICES
	    || s_pLibrary->pUMSD[nDeviceIndex] == 0)
	{
		return -1;
	}

	if (USBBulkOnlyMassStorageDeviceSeek (s_pLibrary->pUMSD[nDeviceIndex], ullOffset) != ullOffset)
	{
		return -1;
	}

	return USBBulkOnlyMassStorageDeviceRead (s_pLibrary->pUMSD[nDeviceIndex], pBuffer, nCount);
}

int USPiMassStorageDeviceWrite (unsigned long long ullOffset, const void *pBuffer, unsigned nCount, unsigned nDeviceIndex)
{
	assert (s_pLibrary != 0);

	if (   nDeviceIndex >= MAX_DEVICES
	    || s_pLibrary->pUMSD[nDeviceIndex] == 0)
	{
		return -1;
	}

	if (USBBulkOnlyMassStorageDeviceSeek (s_pLibrary->pUMSD[nDeviceIndex], ullOffset) != ullOffset)
	{
		return -1;
	}

	return USBBulkOnlyMassStorageDeviceWrite (s_pLibrary->pUMSD[nDeviceIndex], pBuffer, nCount);
}

unsigned USPiMassStorageDeviceGetCapacity (unsigned nDeviceIndex)
{
	assert (s_pLibrary != 0);

	if (   nDeviceIndex >= MAX_DEVICES
	    || s_pLibrary->pUMSD[nDeviceIndex] == 0)
	{
		return 0;
	}

	return USBBulkOnlyMassStorageDeviceGetCapacity (s_pLibrary->pUMSD[nDeviceIndex]);
}

int USPiEthernetAvailable (void)
{
	assert (s_pLibrary != 0);
	return s_pLibrary->pEth0 != 0;
}

void USPiGetMACAddress (unsigned char Buffer[6])
{
	assert (s_pLibrary != 0);
	assert (s_pLibrary->pEth0 != 0);
	TMACAddress *pMACAddress = SMSC951xDeviceGetMACAddress (s_pLibrary->pEth0);

	assert (Buffer != 0);
	MACAddressCopyTo (pMACAddress, Buffer);
}

int USPiSendFrame (const void *pBuffer, unsigned nLength)
{
	assert (s_pLibrary != 0);
	assert (s_pLibrary->pEth0 != 0);
	return SMSC951xDeviceSendFrame (s_pLibrary->pEth0, pBuffer, nLength) ? 1 : 0;
}

int USPiReceiveFrame (void *pBuffer, unsigned *pResultLength)
{
	assert (s_pLibrary != 0);
	assert (s_pLibrary->pEth0 != 0);
	return SMSC951xDeviceReceiveFrame (s_pLibrary->pEth0, pBuffer, pResultLength) ? 1 : 0;
}

int USPiGamePadAvailable (void)
{
	assert (s_pLibrary != 0);

	unsigned i;
	for (i = 0; i < MAX_DEVICES; i++)
	{
		if (s_pLibrary->pUPAD[i] == 0)
		{
			break;
		}
	}

	return (int) i;
}

void USPiGamePadRegisterStatusHandler (TGamePadStatusHandler *pStatusHandler)
{
	assert (s_pLibrary != 0);

    unsigned i;
    for (i = 0; i < MAX_DEVICES; i++)
    {
        if (s_pLibrary->pUPAD[i] != 0)
        {
            USBGamePadDeviceRegisterStatusHandler (s_pLibrary->pUPAD[i], pStatusHandler);
        }
    }
}

const USPiGamePadState *USPiGamePadGetStatus (unsigned nDeviceIndex)
{
	assert (s_pLibrary != 0);

	if (   nDeviceIndex >= MAX_DEVICES
	    || s_pLibrary->pUPAD[nDeviceIndex] == 0)
	{
		return 0;
	}

	USBGamePadDeviceGetReport (s_pLibrary->pUPAD[nDeviceIndex]);

	return &s_pLibrary->pUPAD[nDeviceIndex]->m_State;
}

int USPiDeviceGetInformation (unsigned nClass, unsigned nDeviceIndex, TUSPiDeviceInformation *pInfo)
{
	assert (s_pLibrary != 0);

	TUSBDevice *pUSBDevice = 0;

	switch (nClass)
	{
	case KEYBOARD_CLASS:
		if (nDeviceIndex == 0)
		{
			pUSBDevice = (TUSBDevice *) s_pLibrary->pUKBD1;
		}
		break;

	case MOUSE_CLASS:
		if (nDeviceIndex == 0)
		{
			pUSBDevice = (TUSBDevice *) s_pLibrary->pUMouse1;
		}
		break;

	case STORAGE_CLASS:
		if (nDeviceIndex < MAX_DEVICES)
		{
			pUSBDevice = (TUSBDevice *) s_pLibrary->pUMSD[nDeviceIndex];
		}
		break;

	case ETHERNET_CLASS:
		if (nDeviceIndex == 0)
		{
			pUSBDevice = (TUSBDevice *) s_pLibrary->pEth0;
		}
		break;

	case GAMEPAD_CLASS:
		if (nDeviceIndex < MAX_DEVICES)
		{
			pUSBDevice = (TUSBDevice *) s_pLibrary->pUPAD[nDeviceIndex];
		}
		break;

	case TICT834_CLASS:
		if (nDeviceIndex == 0)
		{
			pUSBDevice = (TUSBDevice *) s_pLibrary->pTic1;
		}
		break;

	case BLT_CLASS:
		if (nDeviceIndex == 0)
		{
			pUSBDevice = (TUSBDevice *) s_pLibrary->pUbt1;
		}
		break;

	default:
		break;
	}

	if (pUSBDevice == 0)
	{
		return 0;
	}

	const TUSBDeviceDescriptor *pDesc = USBDeviceGetDeviceDescriptor (pUSBDevice);
	assert (pDesc != 0);

	assert (pInfo != 0);
	pInfo->idVendor  = pDesc->idVendor;
	pInfo->idProduct = pDesc->idProduct;
	pInfo->bcdDevice = pDesc->bcdDevice;

	pInfo->pManufacturer = USBStringGet (&pUSBDevice->m_ManufacturerString);
	pInfo->pProduct      = USBStringGet (&pUSBDevice->m_ProductString);

	return 1;
}
