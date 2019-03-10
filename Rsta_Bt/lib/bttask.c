//
// bttask.c
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2015  R. Stange <rsta2@o2online.de>
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
#include <rsta_bt/bttask.h>

// #include <circle/sched/scheduler.h>
#include <FreeRTOS.h>
#include <task.h>

#include <uspi/assert.h>

void BTTask (TBTTask *pThis, TBTSubSystem *pBTSubSystem)
{
	pThis->m_pBTSubSystem = pBTSubSystem;
}

void _BTTask (TBTTask *pThis)
{
	pThis->m_pBTSubSystem = 0;
}

void BTTaskRun (TBTTask *pThis)
{
	while (1)
	{	
		assert (pThis->m_pBTSubSystem != 0);
		BTSubSystemProcess(pThis->m_pBTSubSystem);

		// CScheduler::Get ()->Yield ();
		taskYIELD();
	}
}
