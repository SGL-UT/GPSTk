#pragma ident "$Id$"

/**
 * @file DebugUtils.hpp
 * 
 */

#ifndef GPSTK_DEBUGUTILS_HPP
#define GPSTK_DEBUGUTILS_HPP

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================

#include <string>
#include "Exception.hpp"

namespace gpstk
{
      /// Assert failed
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(AssertFailedException, Exception);

      /** This class ...
       *
       */
   class DebugUtils   
   {
   public:
      static void assertion(const char* cond, const char* file, int line);

      static void nullPointer(const char* ptr, const char* file, int line);

   protected:
      static std::string what(const char* msg, const char* file, int line);
   
   private:
      DebugUtils(){}
         
   };   // End of class 'DebugUtils'
   

   //
   // useful macros (these automatically supply line number and file name)
   //
#if defined(_DEBUG)
#define GPSTK_ASSERT_DBG(cond) \
   if (!(cond)) gpstk::DebugUtils::assertion(#cond, __FILE__, __LINE__); else (void) 0
#else
#define GPSTK_ASSERT_DBG(cond)
#endif


#define GPSTK_ASSERT(cond) \
   if (!(cond)) gpstk::DebugUtils::assertion(#cond, __FILE__, __LINE__); else (void) 0


#define GPSTK_CHECK_PTR(ptr) \
   if (!(ptr)) gpstk::DebugUtils::nullPointer(#ptr, __FILE__, __LINE__); else (void) 0


#if defined(_DEBUG)
#	define GPSTK_STDOUT_DBG(outstr) \
   std::cout << __FILE__ << '(' << std::dec << __LINE__ << "):" << outstr << std::endl;
#else
#	define GPSTK_STDOUT_DBG(outstr)
#endif


#if defined(_DEBUG)
#	define GPSTK_STDERR_DBG(outstr) \
   std::cerr << __FILE__ << '(' << std::dec << __LINE__ << "):" << outstr << std::endl;
#else
#	define GPSTK_STDERR_DBG(outstr)
#endif


}   // End of namespace gpstk


#endif  //GPSTK_DEBUGUTILS_HPP

