#pragma ident "$Id$"


/* This is where all platform specific includes, defines and crud should go.
   Unless, of course, it has to go somewhere else. :-)
*/

#ifndef GPSTK_PLATFORM_HPP
#define GPSTK_PLATFORM_HPP

#ifdef _MSC_VER
typedef __int8  int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#elif !defined(__SUNPRO_CC)
#include <stdint.h>
#endif

#endif
