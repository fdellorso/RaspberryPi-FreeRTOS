//
// btdevicemanager.c
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2015-2017  R. Stange <rsta2@o2online.de>
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
#include <rsta_bt/btdevicemanager.h>
#include <rsta_bt/bthcilayer.h>
#include <rsta_bt/bcmvendor.h>
// #include <circle/sched/scheduler.h>
#include <sys_timer.h>
// #include <circle/logger.h>
#include <uspios.h>
#include <uspi/util.h>
#include <uspi/assert.h>

static const u8 Firmware[] =
{
	#include "BCM43430A1.h"
};

static const char FromDeviceManager[] = "btdev";

void BTDeviceManager (TBTDeviceManager *pThis, TBTHCILayer *pHCILayer,
					  TBTQueue *pEventQueue, u32 nClassOfDevice, const char *pLocalName)
{
	pThis->m_pHCILayer		= pHCILayer;
	pThis->m_pEventQueue	= pEventQueue;
	pThis->m_nClassOfDevice	= nClassOfDevice;
	pThis->m_State			= BTDeviceStateUnknown;
	pThis->m_pBuffer		= 0;

	memset (pThis->m_LocalName, 0, sizeof pThis->m_LocalName);
	strncpy ((char *) pThis->m_LocalName, pLocalName, sizeof pThis->m_LocalName);
}

void _BTDeviceManager (TBTDeviceManager *pThis)
{
	free (pThis->m_pBuffer);
	pThis->m_pBuffer = 0;

	pThis->m_pHCILayer = 0;
	pThis->m_pEventQueue = 0;
}

boolean BTDeviceManagerInitialize (TBTDeviceManager *pThis)
{
	assert (pThis->m_pHCILayer != 0);

	pThis->m_pBuffer = (u8) malloc (sizeof(u8) * BT_MAX_HCI_EVENT_SIZE);
	assert (pThis->m_pBuffer != 0);

	TBTHCICommandHeader Cmd;
	Cmd.OpCode = OP_CODE_RESET;
	Cmd.ParameterTotalLength = PARM_TOTAL_LEN (Cmd);
	BTHCILayerSendCommand(pThis->m_pHCILayer, &Cmd, sizeof Cmd);

	pThis->m_State = BTDeviceStateResetPending;

	return TRUE;
}

void BTDeviceManagerProcess (TBTDeviceManager *pThis)
{
	assert (pThis->m_pHCILayer != 0);
	assert (pThis->m_pEventQueue != 0);
	assert (pThis->m_pBuffer != 0);

	unsigned nLength;
	while ((nLength = BTQueueDequeue(pThis->m_pEventQueue, pThis->m_pBuffer, NULL)) > 0)
	{
		assert (nLength >= sizeof (TBTHCIEventHeader));
		TBTHCIEventHeader *pHeader = (TBTHCIEventHeader *) pThis->m_pBuffer;

		switch (pHeader->EventCode)
		{
			case EVENT_CODE_COMMAND_COMPLETE: {
				assert (nLength >= sizeof (TBTHCIEventCommandComplete));
				TBTHCIEventCommandComplete *pCommandComplete = (TBTHCIEventCommandComplete *) pHeader;

				BTHCILayerSetCommandPackets(pThis->m_pHCILayer, pCommandComplete->NumHCICommandPackets);

				if (pCommandComplete->Status != BT_STATUS_SUCCESS)
				{
					LogWrite (FromDeviceManager, LOG_ERROR,
								"Command 0x%X failed (status 0x%X)",
								(unsigned) pCommandComplete->CommandOpCode,
								(unsigned) pCommandComplete->Status);

					pThis->m_State = BTDeviceStateFailed;

					continue;
				}

				switch (pCommandComplete->CommandOpCode)
				{
					case OP_CODE_RESET: {
						assert (pThis->m_State == BTDeviceStateResetPending);

						if (BTHCILayerGetTransportType(pThis->m_pHCILayer) != BTTransportTypeUART)
						{
							goto NoFirmwareLoad;
						}

						TBTHCICommandHeader Cmd;
						Cmd.OpCode = OP_CODE_DOWNLOAD_MINIDRIVER;
						Cmd.ParameterTotalLength = PARM_TOTAL_LEN (Cmd);
						BTHCILayerSendCommand(pThis->m_pHCILayer, &Cmd, sizeof Cmd);
						// CScheduler::Get ()->MsSleep (50);
						DelayMilliSysTimer(50);

						pThis->m_nFirmwareOffset = 0;
						pThis->m_State = BTDeviceStateWriteRAMPending;
					} break;

					case OP_CODE_DOWNLOAD_MINIDRIVER:
					case OP_CODE_WRITE_RAM: {
						assert (pThis->m_State == BTDeviceStateWriteRAMPending);

						assert (pThis->m_nFirmwareOffset + 3 <= sizeof Firmware);
						(u16) nOpCode  = Firmware[pThis->m_nFirmwareOffset++];
							  nOpCode |= Firmware[pThis->m_nFirmwareOffset++] << 8;
						(u8)  nLength  = Firmware[pThis->m_nFirmwareOffset++];

						TBTHCIBcmVendorCommand Cmd;
						Cmd.Header.OpCode = nOpCode;
						Cmd.Header.ParameterTotalLength = nLength;

						unsigned i;
						for (i = 0; i < nLength; i++)
						{
							assert (pThis->m_nFirmwareOffset < sizeof Firmware);
							Cmd.Data[i] = Firmware[pThis->m_nFirmwareOffset++];
						}

						BTHCILayerSendCommand(pThis->m_pHCILayer, &Cmd, sizeof Cmd.Header + nLength);

						if (nOpCode == OP_CODE_LAUNCH_RAM)
						{
							pThis->m_State = BTDeviceStateLaunchRAMPending;
						}
					} break;

					case OP_CODE_LAUNCH_RAM: {
						assert (pThis->m_State == BTDeviceStateLaunchRAMPending);
						// CScheduler::Get ()->MsSleep (250);
						DelayMilliSysTimer(250);

					NoFirmwareLoad:
						TBTHCICommandHeader Cmd;
						Cmd.OpCode = OP_CODE_READ_BD_ADDR;
						Cmd.ParameterTotalLength = PARM_TOTAL_LEN (Cmd);
						BTHCILayerSendCommand(pThis->m_pHCILayer, &Cmd, sizeof Cmd);

						pThis->m_State = BTDeviceStateReadBDAddrPending;
					} break;

					case OP_CODE_READ_BD_ADDR: {
						assert (pThis->m_State == BTDeviceStateReadBDAddrPending);

						assert (nLength >= sizeof (TBTHCIEventReadBDAddrComplete));
						TBTHCIEventReadBDAddrComplete *pEvent = (TBTHCIEventReadBDAddrComplete *) pHeader;
						memcpy (pThis->m_LocalBDAddr, pEvent->BDAddr, BT_BD_ADDR_SIZE);

						LogWrite (FromDeviceManager, LOG_NOTICE,
									"Local BD address is %02X:%02X:%02X:%02X:%02X:%02X",
									(unsigned) pThis->m_LocalBDAddr[5],
									(unsigned) pThis->m_LocalBDAddr[4],
									(unsigned) pThis->m_LocalBDAddr[3],
									(unsigned) pThis->m_LocalBDAddr[2],
									(unsigned) pThis->m_LocalBDAddr[1],
									(unsigned) pThis->m_LocalBDAddr[0]);

						TBTHCIWriteClassOfDeviceCommand Cmd;
						Cmd.Header.OpCode = OP_CODE_WRITE_CLASS_OF_DEVICE;
						Cmd.Header.ParameterTotalLength = PARM_TOTAL_LEN (Cmd);
						Cmd.ClassOfDevice[0] = pThis->m_nClassOfDevice       & 0xFF;
						Cmd.ClassOfDevice[1] = pThis->m_nClassOfDevice >> 8  & 0xFF;
						Cmd.ClassOfDevice[2] = pThis->m_nClassOfDevice >> 16 & 0xFF;
						BTHCILayerSendCommand(pThis->m_pHCILayer, &Cmd, sizeof Cmd);

						pThis->m_State = BTDeviceStateWriteClassOfDevicePending;
					} break;

					case OP_CODE_WRITE_CLASS_OF_DEVICE: {
						assert (pThis->m_State == BTDeviceStateWriteClassOfDevicePending);

						TBTHCIWriteLocalNameCommand Cmd;
						Cmd.Header.OpCode = OP_CODE_WRITE_LOCAL_NAME;
						Cmd.Header.ParameterTotalLength = PARM_TOTAL_LEN (Cmd);
						memcpy (Cmd.LocalName, pThis->m_LocalName, sizeof Cmd.LocalName);
						BTHCILayerSendCommand(pThis->m_pHCILayer, &Cmd, sizeof Cmd);

						pThis->m_State = BTDeviceStateWriteLocalNamePending;
					} break;

					case OP_CODE_WRITE_LOCAL_NAME: {
						assert (pThis->m_State == BTDeviceStateWriteLocalNamePending);

						TBTHCIWriteScanEnableCommand Cmd;
						Cmd.Header.OpCode = OP_CODE_WRITE_SCAN_ENABLE;
						Cmd.Header.ParameterTotalLength = PARM_TOTAL_LEN (Cmd);
						Cmd.ScanEnable = SCAN_ENABLE_BOTH_ENABLED;
						BTHCILayerSendCommand(pThis->m_pHCILayer, &Cmd, sizeof Cmd);

						pThis->m_State = BTDeviceStateWriteScanEnabledPending;
					} break;

					case OP_CODE_WRITE_SCAN_ENABLE: {
						assert (pThis->m_State == BTDeviceStateWriteScanEnabledPending);
						pThis->m_State = BTDeviceStateRunning;
					} break;

					default:
						break;
				}
			} break;

			case EVENT_CODE_COMMAND_STATUS: {
				assert (nLength >= sizeof (TBTHCIEventCommandStatus));
				TBTHCIEventCommandStatus *pCommandStatus = (TBTHCIEventCommandStatus *) pHeader;

				BTHCILayerSetCommandPackets(pThis->m_pHCILayer, pCommandStatus->NumHCICommandPackets);
			} break;

			default:
				assert (0);
				break;
		}
	}
}

boolean BTDeviceManagerDeviceIsRunning (TBTDeviceManager *pThis)
{
	return pThis->m_State == BTDeviceStateRunning ? TRUE : FALSE;
}
