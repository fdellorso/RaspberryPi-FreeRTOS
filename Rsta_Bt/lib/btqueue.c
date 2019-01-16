//
// btqueue.c
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
#include <rsta_bt/btqueue.h>
#include <rsta_bt/bluetooth.h>
#include <uspi/util.h>
#include <uspi/assert.h>

// struct TBTQueue
// {
// 	volatile TBTQueue	*pPrev;
// 	volatile TBTQueue	*pNext;
// 	unsigned		 	nLength;
// 	unsigned char		 Buffer[BT_MAX_DATA_SIZE];
// 	void				*pParam;
// };

void BTQueue (TBTQueue *pThis)
{
	pThis->m_pFirst	= 0;
	pThis->m_pLast	= 0;
}

void _BTQueue (TBTQueue *pThis)
{
	BTQueueFlush (pThis);
}

boolean BTQueueIsEmpty (TBTQueue *pThis)
{
	return pThis->m_pFirst == 0 ? TRUE : FALSE;
}

void BTQueueFlush (TBTQueue *pThis)
{
	while (pThis->m_pFirst != 0)
	{
		// m_SpinLock.Acquire ();

		TBTQueue *pEntry = pThis->m_pFirst;
		assert (pEntry != 0);

		pThis->m_pFirst = pEntry->pNext;
		if (pThis->m_pFirst != 0)
		{
			pThis->m_pFirst->pPrev = 0;
		}
		else
		{
			assert (pThis->m_pLast == pEntry);
			pThis->m_pLast = 0;
		}

		// m_SpinLock.Release ();

		free(pEntry);
	}
}
	
void BTQueueEnqueue (TBTQueue *pThis, const void *pBuffer, unsigned nLength, void *pParam)
{
	TBTQueue *pEntry = (TBTQueue *) malloc (sizeof(TBTQueue));
	assert (pEntry != 0);

	assert (nLength > 0);
	assert (nLength <= BT_MAX_DATA_SIZE);
	pEntry->nLength = nLength;

	assert (pBuffer != 0);
	memcpy (pEntry->Buffer, pBuffer, nLength);

	pEntry->pParam = pParam;

	// m_SpinLock.Acquire ();

	pEntry->pPrev = pThis->m_pLast;
	pEntry->pNext = 0;

	if (pThis->m_pFirst == 0)
	{
		pThis->m_pFirst = pEntry;
	}
	else
	{
		assert (pThis->m_pLast != 0);
		assert (pThis->m_pLast->pNext == 0);
		pThis->m_pLast->pNext = pEntry;
	}
	pThis->m_pLast = pEntry;

	free (pEntry);

	// m_SpinLock.Release ();
}

unsigned BTQueueDequeue (TBTQueue *pThis, void *pBuffer, void **ppParam)
{
	unsigned nResult = 0;
	
	if (pThis->m_pFirst != 0)
	{
		// m_SpinLock.Acquire ();

		TBTQueue *pEntry = (TBTQueue *) malloc (sizeof(TBTQueue));
		pEntry = pThis->m_pFirst;
		assert (pEntry != 0);

		pThis->m_pFirst = pEntry->pNext;
		if (pThis->m_pFirst != 0)
		{
			pThis->m_pFirst->pPrev = 0;
		}
		else
		{
			assert (pThis->m_pLast == pEntry);
			pThis->m_pLast = 0;
		}

		// m_SpinLock.Release ();

		nResult = pEntry->nLength;
		assert (nResult > 0);
		assert (nResult <= BT_MAX_DATA_SIZE);

		memcpy (pBuffer, (const void *) pEntry->Buffer, nResult);

		if (ppParam != 0)
		{
			*ppParam = pEntry->pParam;
		}

		free (pEntry);
	}

	return nResult;
}
