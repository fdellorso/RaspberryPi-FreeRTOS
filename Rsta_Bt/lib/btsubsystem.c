//
// btsubsystem.c
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
#include <rsta_bt/btsubsystem.h>
#include <rsta_bt/bttask.h>
// #include <circle/sched/scheduler.h>
#include <FreeRTOS.h>
#include <uspi/devicenameservice.h>
// #include <circle/machineinfo.h>
#include <uspi/assert.h>

void BTSubSystem (TBTSubSystem *pThis, CInterruptSystem *pInterruptSystem, u32 nClassOfDevice, const char *pLocalName)
{
	m_pInterruptSystem (pInterruptSystem);		// TODO
	pThis->m_pUARTTransport = 0;
	BTHCILayer(pThis->m_HCILayer, nClassOfDevice, pLocalName);
	BTLogicalLayer(pThis->m_LogicalLayer, &pThis->m_HCILayer);
}

void _BTSubSystem (TBTSubSystem *pThis)
{
	free (pThis->m_pUARTTransport);
	pThis->m_pUARTTransport = 0;
}

boolean BTSubSystemInitialize (TBTSubSystem *pThis)
{
	// if USB transport not available, UART still free and this is a RPi 3B or Zero W:
	//	use UART transport
	if (DeviceNameServiceGetDevice (DeviceNameServiceGet (), "ubt1", FALSE) == 0)
	{
		assert (pThis->m_pUARTTransport == 0);
		assert (pThis->m_pInterruptSystem != 0);
		pThis->m_pUARTTransport = new CBTUARTTransport (m_pInterruptSystem);	// TODO
		assert (pThis->m_pUARTTransport != 0);

		if (!BTUARTTransportInitialize(pThis->m_pUARTTransport)
		{
			return FALSE;
		}
	}

	if (!BTHCILayerInitialize(pThis->m_HCILayer)
	{
		return FALSE;
	}

	if (!BTLogicalLayerInitialize(pThis->m_LogicalLayer)
	{
		return FALSE;
	}

	TBTTask *pTask = (TBTTask *) malloc (sizeof(TBTTask));
	BTTask (pTask);

	while (!BTDeviceManagerDeviceIsRunning(BTHCILayerGetDeviceManager(pThis->m_HCILayer)))
	{
		// CScheduler::Get ()->Yield ();
		taskYIELD();
	}

	return TRUE;
}

void BTSubSystemProcess (TBTSubSystem *pThis)
{
	BTHCILayerProcess(pThis->m_HCILayer);

	BTLogicalLayerProcess(pThis->m_LogicalLayer);
}

TBTInquiryResults *BTSubSystemInquiry (TBTSubSystem *pThis, unsigned nSeconds)
{
	return BTLogicalLayerInquiry(pThis->m_LogicalLayer, nSeconds);
}
