//
// btqueue.h
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
#ifndef _rsta_bt_btqueue_h
#define _rsta_bt_btqueue_h

// #include <circle/spinlock.h>
#include <rsta_bt/bluetooth.h>
#include <uspi/types.h>

typedef struct TBTQueue
{
	struct TBTQueue *m_pFirst;
	struct TBTQueue *m_pLast;

	struct TBTQueue	*pPrev;
	struct TBTQueue	*pNext;
	unsigned		nLength;
	unsigned char	Buffer[BT_MAX_DATA_SIZE];
	void			*pParam;

	// CSpinLock m_SpinLock;
}
TBTQueue;

void BTQueue (TBTQueue *pThis);
void _BTQueue (TBTQueue *pThis);

boolean BTQueueIsEmpty (TBTQueue *pThis);

void BTQueueFlush (TBTQueue *pThis);

void BTQueueEnqueue (TBTQueue *pThis, const void *pBuffer, unsigned nLength, void *pParam);

// returns length (0 if queue is empty)
unsigned BTQueueDequeue (TBTQueue *pThis, void *pBuffer, void **ppParam);

#endif
