//
// ptrarray.h
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
#ifndef _rsta_bt_ptrarray_h
#define _rsta_bt_ptrarray_h

typedef struct TPtrArray									// dynamic array of pointers
{
	unsigned	m_nReservedSize;
	unsigned	m_nSizeIncrement;
	unsigned	m_nUsedCount;
	void		**m_ppArray;
}
TPtrArray;

void PtrArray (TPtrArray *pThis, unsigned nInitialSize, unsigned nSizeIncrement);
void _PtrArray (TPtrArray *pThis);

unsigned PtrArrayGetCount (TPtrArray *pThis);				// get number of elements in the array

void *PtrArrayGetElement (TPtrArray *pThis, unsigned nIndex);

unsigned PtrArrayAppend (TPtrArray *pThis, void *pPtr);		// append element to end of array

void PtrArrayRemoveLast (TPtrArray *pThis);					// remove last element

#endif
