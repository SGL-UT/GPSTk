#pragma ident "$Id$"

/**
 * @file NumberFormatter.cpp
 * Modified from Poco, Original copyright by Applied Informatics.
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

#include "NumberFormatter.hpp"
#include <cstdio>
#include "DebugUtils.hpp"

namespace gpstk
{
   void NumberFormatter::append(std::string& str, int value)
   {
      char buffer[64];
      std::sprintf(buffer, "%d", value);
      str.append(buffer);
   }


   void NumberFormatter::append(std::string& str, int value, int width)
   {
      GPSTK_ASSERT (width > 0 && width < 64);

      char buffer[64];
      std::sprintf(buffer, "%*d", width, value);
      str.append(buffer);
   }


   void NumberFormatter::append0(std::string& str, int value, int width)
   {
      GPSTK_ASSERT (width > 0 && width < 64);

      char buffer[64];
      std::sprintf(buffer, "%0*d", width, value);
      str.append(buffer);
   }


   void NumberFormatter::appendHex(std::string& str, int value)
   {
      char buffer[64];
      std::sprintf(buffer, "%X", value);
      str.append(buffer);
   }


   void NumberFormatter::appendHex(std::string& str, int value, int width)
   {
      GPSTK_ASSERT (width > 0 && width < 64);

      char buffer[64];
      std::sprintf(buffer, "%0*X", width, value);
      str.append(buffer);
   }


   void NumberFormatter::append(std::string& str, unsigned value)
   {
      char buffer[64];
      std::sprintf(buffer, "%u", value);
      str.append(buffer);
   }


   void NumberFormatter::append(std::string& str, unsigned value, int width)
   {
      GPSTK_ASSERT (width > 0 && width < 64);

      char buffer[64];
      std::sprintf(buffer, "%*u", width, value);
      str.append(buffer);
   }


   void NumberFormatter::append0(std::string& str, unsigned int value, int width)
   {
      GPSTK_ASSERT (width > 0 && width < 64);

      char buffer[64];
      std::sprintf(buffer, "%0*u", width, value);
      str.append(buffer);
   }


   void NumberFormatter::appendHex(std::string& str, unsigned value)
   {
      char buffer[64];
      std::sprintf(buffer, "%X", value);
      str.append(buffer);
   }


   void NumberFormatter::appendHex(std::string& str, unsigned value, int width)
   {
      GPSTK_ASSERT (width > 0 && width < 64);

      char buffer[64];
      std::sprintf(buffer, "%0*X", width, value);
      str.append(buffer);
   }


   void NumberFormatter::append(std::string& str, long value)
   {
      char buffer[64];
      std::sprintf(buffer, "%ld", value);
      str.append(buffer);
   }


   void NumberFormatter::append(std::string& str, long value, int width)
   {
      GPSTK_ASSERT (width > 0 && width < 64);

      char buffer[64];
      std::sprintf(buffer, "%*ld", width, value);
      str.append(buffer);
   }


   void NumberFormatter::append0(std::string& str, long value, int width)
   {
      GPSTK_ASSERT (width > 0 && width < 64);

      char buffer[64];
      std::sprintf(buffer, "%0*ld", width, value);
      str.append(buffer);
   }


   void NumberFormatter::appendHex(std::string& str, long value)
   {
      char buffer[64];
      std::sprintf(buffer, "%lX", value);
      str.append(buffer);
   }


   void NumberFormatter::appendHex(std::string& str, long value, int width)
   {
      GPSTK_ASSERT (width > 0 && width < 64);

      char buffer[64];
      std::sprintf(buffer, "%0*lX", width, value);
      str.append(buffer);
   }


   void NumberFormatter::append(std::string& str, unsigned long value)
   {
      char buffer[64];
      std::sprintf(buffer, "%lu", value);
      str.append(buffer);
   }


   void NumberFormatter::append(std::string& str, unsigned long value, int width)
   {
      GPSTK_ASSERT (width > 0 && width < 64);

      char buffer[64];
      std::sprintf(buffer, "%*lu", width, value);
      str.append(buffer);
   }


   void NumberFormatter::append0(std::string& str, unsigned long value, int width)
   {
      GPSTK_ASSERT (width > 0 && width < 64);

      char buffer[64];
      std::sprintf(buffer, "%0*lu", width, value);
      str.append(buffer);
   }


   void NumberFormatter::appendHex(std::string& str, unsigned long value)
   {
      char buffer[64];
      std::sprintf(buffer, "%lX", value);
      str.append(buffer);
   }


   void NumberFormatter::appendHex(std::string& str, unsigned long value, int width)
   {
      GPSTK_ASSERT (width > 0 && width < 64);

      char buffer[64];
      std::sprintf(buffer, "%0*lX", width, value);
      str.append(buffer);
   }

   void NumberFormatter::append(std::string& str, double value)
   {
      char buffer[64];
      std::sprintf(buffer, "%.*g", 16, value);
      str.append(buffer);
   }


   void NumberFormatter::append(std::string& str, double value, int precision)
   {
      GPSTK_ASSERT (precision >= 0 && precision < 32);

      char buffer[64];
      std::sprintf(buffer, "%.*f", precision, value);
      str.append(buffer);
   }


   void NumberFormatter::append(std::string& str, double value, int width, int precision)
   {
      GPSTK_ASSERT (width > 0 && width < 64 && precision >= 0 && precision < width);

      char buffer[64];
      std::sprintf(buffer, "%*.*f", width, precision, value);
      str.append(buffer);
   }
   
}   // End of namespace gpstk

