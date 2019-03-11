//
// types.h
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
#ifndef _uspi_types_h
#define _uspi_types_h

typedef unsigned char		    u8;
typedef unsigned short		    u16;
typedef unsigned int		    u32;
typedef unsigned long long	    u64;

typedef char                    s8;
typedef short                   s16;
typedef int                     s32;
typedef long long               s64;

typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long int  uint64_t;

typedef char                    int8_t;
typedef short int               int16_t;
typedef int                     int32_t;
typedef long long int           int64_t;

typedef int		                boolean;
#define FALSE		            0
#define TRUE		            1

typedef int		                bool;
#define false		            0
#define true		            1

typedef unsigned long	        size_t;
typedef long		            ssize_t;

#endif
