#pragma ident "$Id: gpstkplatform.h 733 2007-09-12 13:54:22Z pben $"



//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================




/* This is where all platform specific includes, defines and crud should go.
   Unless, of course, it has to go somewhere else. :-)
*/

#ifndef GPSTK_PLATFORM_HPP
#define GPSTK_PLATFORM_HPP

#ifdef _MSC_VER

#include<stdlib.h>

#define HAVE_STRING_H 1
#define STDC_HEADERS  1

typedef __int8  int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#include <sys/types.h>
#include <sys/timeb.h>


#elif !defined(__SUNPRO_CC)
#include <stdint.h>
#endif

#endif
