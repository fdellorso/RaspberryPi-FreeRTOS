//
// ptrarray.c
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
#include <rsta_bt/ptrarray.h>
#include <uspi/util.h>
#include <uspi/assert.h>

void PtrArray (TPtrArray *pThis, unsigned nInitialSize, unsigned nSizeIncrement)
{
	pThis->m_nReservedSize	= nInitialSize;
	pThis->m_nSizeIncrement	= nSizeIncrement;
	pThis->m_nUsedCount 	= 0;
	pThis->m_ppArray 		= 0;

	assert (pThis->m_nReservedSize > 0);
	assert (pThis->m_nSizeIncrement > 0);

	pThis->m_ppArray = (void *) malloc (pThis->m_nReservedSize);
	assert (pThis->m_ppArray != 0);
}

void _PtrArray (TPtrArray *pThis)
{
	pThis->m_nReservedSize = 0;
	pThis->m_nSizeIncrement = 0;

	free (pThis->m_ppArray);
	pThis->m_ppArray = 0;
}

unsigned PtrArrayGetCount (TPtrArray *pThis)
{
	return pThis->m_nUsedCount;
}

void *PtrArrayGetElement (TPtrArray *pThis, unsigned nIndex)
{
	assert (nIndex < pThis->m_nUsedCount);
	assert (pThis->m_nUsedCount <= pThis->m_nReservedSize);
	assert (pThis->m_ppArray != 0);

	return pThis->m_ppArray[nIndex];
}

unsigned PtrArrayAppend (TPtrArray *pThis, void *pPtr)
{
	assert (pThis->m_nReservedSize > 0);
	assert (pThis->m_ppArray != 0);

	assert (pThis->m_nUsedCount <= pThis->m_nReservedSize);
	if (pThis->m_nUsedCount == pThis->m_nReservedSize)
	{
		assert (pThis->m_nSizeIncrement > 0);
		void **ppNewArray = (void *) malloc (pThis->m_nReservedSize + pThis->m_nSizeIncrement);
		assert (ppNewArray != 0);

		memcpy (ppNewArray, pThis->m_ppArray, pThis->m_nReservedSize * sizeof (void *));

		free (pThis->m_ppArray);
		pThis->m_ppArray = ppNewArray;

		pThis->m_nReservedSize += pThis->m_nSizeIncrement;
	}

	pThis->m_ppArray[pThis->m_nUsedCount] = pPtr;

	return pThis->m_nUsedCount++;
}

void PtrArrayRemoveLast (TPtrArray *pThis)
{
	assert (pThis->m_nUsedCount > 0);
	pThis->m_nUsedCount--;
}
