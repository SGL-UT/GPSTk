#pragma ident "$Id$"

/**
 * @file SysInfo.cpp
 * 
 */
 
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
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================

#include "SysInfo.hpp"

//
// Platform Identification
//
#define GPSTK_OS_FREE_BSD      0x0001
#define GPSTK_OS_AIX           0x0002
#define GPSTK_OS_HPUX          0x0003
#define GPSTK_OS_TRU64         0x0004
#define GPSTK_OS_LINUX         0x0005
#define GPSTK_OS_MAC_OS_X      0x0006
#define GPSTK_OS_NET_BSD       0x0007
#define GPSTK_OS_OPEN_BSD      0x0008
#define GPSTK_OS_IRIX          0x0009
#define GPSTK_OS_SOLARIS       0x000a
#define GPSTK_OS_QNX           0x000b
#define GPSTK_OS_VXWORKS       0x000c
#define GPSTK_OS_CYGWIN        0x000d
#define GPSTK_OS_UNKNOWN_UNIX  0x00ff
#define GPSTK_OS_WINDOWS_NT    0x1001
#define GPSTK_OS_WINDOWS_CE    0x1011
#define GPSTK_OS_VMS           0x2001


#if defined(__FreeBSD__)
#define GPSTK_OS_FAMILY_UNIX 1
#define GPSTK_OS_FAMILY_BSD 1
#define GPSTK_OS GPSTK_OS_FREE_BSD
#elif defined(_AIX) || defined(__TOS_AIX__)
#define GPSTK_OS_FAMILY_UNIX 1
#define GPSTK_OS GPSTK_OS_AIX
#elif defined(hpux) || defined(_hpux)
#define GPSTK_OS_FAMILY_UNIX 1
#define GPSTK_OS GPSTK_OS_HPUX
#elif defined(__digital__) || defined(__osf__)
#define GPSTK_OS_FAMILY_UNIX 1
#define GPSTK_OS GPSTK_OS_TRU64
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__TOS_LINUX__)
#define GPSTK_OS_FAMILY_UNIX 1
#define GPSTK_OS GPSTK_OS_LINUX
#elif defined(__APPLE__) || defined(__TOS_MACOS__)
#define GPSTK_OS_FAMILY_UNIX 1
#define GPSTK_OS_FAMILY_BSD 1
#define GPSTK_OS GPSTK_OS_MAC_OS_X
#elif defined(__NetBSD__)
#define GPSTK_OS_FAMILY_UNIX 1
#define GPSTK_OS_FAMILY_BSD 1
#define GPSTK_OS GPSTK_OS_NET_BSD
#elif defined(__OpenBSD__)
#define GPSTK_OS_FAMILY_UNIX 1
#define GPSTK_OS_FAMILY_BSD 1
#define GPSTK_OS GPSTK_OS_OPEN_BSD
#elif defined(sgi) || defined(__sgi)
#define GPSTK_OS_FAMILY_UNIX 1
#define GPSTK_OS GPSTK_OS_IRIX
#elif defined(sun) || defined(__sun)
#define GPSTK_OS_FAMILY_UNIX 1
#define GPSTK_OS GPSTK_OS_SOLARIS
#elif defined(__QNX__)
#define GPSTK_OS_FAMILY_UNIX 1
#define GPSTK_OS GPSTK_OS_QNX
#elif defined(unix) || defined(__unix) || defined(__unix__)
#define GPSTK_OS_FAMILY_UNIX 1
#define GPSTK_OS GPSTK_OS_UNKNOWN_UNIX
#elif defined(_WIN32_WCE)
#define GPSTK_OS_FAMILY_WINDOWS 1
#define GPSTK_OS GPSTK_OS_WINDOWS_CE
#elif defined(_WIN32) || defined(_WIN64)
#define GPSTK_OS_FAMILY_WINDOWS 1
#define GPSTK_OS GPSTK_OS_WINDOWS_NT
#elif defined(__CYGWIN__)
#define GPSTK_OS_FAMILY_UNIX 1
#define GPSTK_OS GPSTK_OS_CYGWIN
#elif defined(__VMS)
#define GPSTK_OS_FAMILY_VMS 1
#define GPSTK_OS GPSTK_OS_VMS
#elif defined(GPSTK_VXWORKS)
#define GPSTK_OS_FAMILY_UNIX 1
#define GPSTK_OS GPSTK_OS_VXWORKS
#endif


//
// Hardware Architecture and Byte Order
//
#define GPSTK_ARCH_ALPHA   0x01
#define GPSTK_ARCH_IA32    0x02
#define GPSTK_ARCH_IA64    0x03
#define GPSTK_ARCH_MIPS    0x04
#define GPSTK_ARCH_HPPA    0x05
#define GPSTK_ARCH_PPC     0x06
#define GPSTK_ARCH_POWER   0x07
#define GPSTK_ARCH_SPARC   0x08
#define GPSTK_ARCH_AMD64   0x09
#define GPSTK_ARCH_ARM     0x0a
#define GPSTK_ARCH_M68K    0x0b
#define GPSTK_ARCH_S390    0x0c
#define GPSTK_ARCH_SH      0x0d
#define GPSTK_ARCH_NIOS2   0x0e


#if defined(__ALPHA) || defined(__alpha) || defined(__alpha__) || defined(_M_ALPHA)
#define GPSTK_ARCH GPSTK_ARCH_ALPHA
#define GPSTK_ARCH_LITTLE_ENDIAN 1
#elif defined(i386) || defined(__i386) || defined(__i386__) || defined(_M_IX86)
#define GPSTK_ARCH GPSTK_ARCH_IA32
#define GPSTK_ARCH_LITTLE_ENDIAN 1
#elif defined(_IA64) || defined(__IA64__) || defined(__ia64__) || defined(__ia64) || defined(_M_IA64)
#define GPSTK_ARCH GPSTK_ARCH_IA64
#if defined(hpux) || defined(_hpux)
#define GPSTK_ARCH_BIG_ENDIAN 1
#else
#define GPSTK_ARCH_LITTLE_ENDIAN 1
#endif
#elif defined(__x86_64__) || defined(_M_X64)
#define GPSTK_ARCH GPSTK_ARCH_AMD64
#define GPSTK_ARCH_LITTLE_ENDIAN 1
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__) || defined(_M_MRX000)
#define GPSTK_ARCH GPSTK_ARCH_MIPS
#define GPSTK_ARCH_BIG_ENDIAN 1
#elif defined(__hppa) || defined(__hppa__)
#define GPSTK_ARCH GPSTK_ARCH_HPPA
#define GPSTK_ARCH_BIG_ENDIAN 1
#elif defined(__PPC) || defined(__POWERPC__) || defined(__powerpc) || defined(__PPC__) || \
   defined(__powerpc__) || defined(__ppc__) || defined(__ppc) || defined(_ARCH_PPC) || defined(_M_PPC)
#define GPSTK_ARCH GPSTK_ARCH_PPC
#define GPSTK_ARCH_BIG_ENDIAN 1
#elif defined(_POWER) || defined(_ARCH_PWR) || defined(_ARCH_PWR2) || defined(_ARCH_PWR3) || \
   defined(_ARCH_PWR4) || defined(__THW_RS6000)
#define GPSTK_ARCH GPSTK_ARCH_POWER
#define GPSTK_ARCH_BIG_ENDIAN 1
#elif defined(__sparc__) || defined(__sparc) || defined(sparc)
#define GPSTK_ARCH GPSTK_ARCH_SPARC
#define GPSTK_ARCH_BIG_ENDIAN 1
#elif defined(__arm__) || defined(__arm) || defined(ARM) || defined(_ARM_) || defined(__ARM__) || defined(_M_ARM)
#define GPSTK_ARCH GPSTK_ARCH_ARM
#if defined(__ARMEB__)
#define GPSTK_ARCH_BIG_ENDIAN 1
#else
#define GPSTK_ARCH_LITTLE_ENDIAN 1
#endif
#elif defined(__m68k__)
#define GPSTK_ARCH GPSTK_ARCH_M68K
#define GPSTK_ARCH_BIG_ENDIAN 1
#elif defined(__s390__)
#define GPSTK_ARCH GPSTK_ARCH_S390
#define GPSTK_ARCH_BIG_ENDIAN 1
#elif defined(__sh__) || defined(__sh)
#define GPSTK_ARCH GPSTK_ARCH_SH
#if defined(__LITTLE_ENDIAN__)
#define GPSTK_ARCH_LITTLE_ENDIAN 1
#else
#define GPSTK_ARCH_BIG_ENDIAN 1
#endif
#elif defined (nios2) || defined(__nios2) || defined(__nios2__)
#define GPSTK_ARCH GPSTK_ARCH_NIOS2
#if defined(__nios2_little_endian) || defined(nios2_little_endian) || defined(__nios2_little_endian__)
#define GPSTK_ARCH_LITTLE_ENDIAN 1
#else
#define GPSTK_ARCH_BIG_ENDIAN 1
#endif

#endif


namespace gpstk
{

   std::string SysInfo::osName()
   {
      switch (GPSTK_OS)
      {
      case GPSTK_OS_FREE_BSD:    return "Free BSD";
      case GPSTK_OS_AIX:         return "AIX";
      case GPSTK_OS_HPUX:        return "HPUX";
      case GPSTK_OS_TRU64:       return "IRU64";
      case GPSTK_OS_LINUX:       return "Linux";
      case GPSTK_OS_MAC_OS_X:    return "Mac OS X";
      case GPSTK_OS_NET_BSD:     return "Net BSD";
      case GPSTK_OS_OPEN_BSD:    return "Open BSD";
      case GPSTK_OS_IRIX:        return "IRIX";
      case GPSTK_OS_SOLARIS:     return "Solaris";
      case GPSTK_OS_QNX:         return "QNX";
      case GPSTK_OS_VXWORKS:     return "Vxworks";
      case GPSTK_OS_CYGWIN:      return "Cygwin";
      case GPSTK_OS_UNKNOWN_UNIX:return "Unknown Unix";
      case GPSTK_OS_WINDOWS_NT:  return "Windows NT";
      case GPSTK_OS_WINDOWS_CE:  return "Windows CE";
      case GPSTK_OS_VMS:         return "VMS";
      default:                   return "Unknown";
      }
   }

   std::string SysInfo::osArchitecture()
   {
      switch (GPSTK_ARCH)
      {
      case GPSTK_ARCH_ALPHA:     return "ALPHA";
      case GPSTK_ARCH_IA32:      return "IA32";
      case GPSTK_ARCH_IA64:      return "IA64";
      case GPSTK_ARCH_MIPS:      return "MIPS";
      case GPSTK_ARCH_HPPA:      return "HPPA";
      case GPSTK_ARCH_PPC:       return "PPC";
      case GPSTK_ARCH_POWER:     return "POWER";
      case GPSTK_ARCH_SPARC:     return "SPARC";
      case GPSTK_ARCH_AMD64:     return "AMD64";
      case GPSTK_ARCH_ARM:       return "ARM";
      case GPSTK_ARCH_M68K:      return "M68K";
      case GPSTK_ARCH_S390:      return "S390";
      case GPSTK_ARCH_SH:        return "SH";
      case GPSTK_ARCH_NIOS2:     return "NIOS2";
      default:                   return "Unknown";
      }
   }
   
}   // End of namespace gpstk

