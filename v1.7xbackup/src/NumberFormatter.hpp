#pragma ident "$Id$"

/**
 * @file NumberFormatter.hpp
 * Modified from Poco, Original copyright by Applied Informatics.
 */

#ifndef GPSTK_NUMBERFORMATTER_HPP
#define GPSTK_NUMBERFORMATTER_HPP

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

#include <string>

namespace gpstk
{

      /// The NumberFormatter class provides static methods
      /// for formatting numeric values into strings.
      ///
      /// There are two kind of static member functions:
      ///    * format* functions return a std::string containing
      ///      the formatted value.
      ///    * append* functions append the formatted value to
      ///      an existing string.
      ///
      /// Internally, std::sprintf() is used to do the actual
      /// formatting.
   class NumberFormatter
   {
   public:
      static std::string format(int value);

      static std::string format(int value, int width);

      static std::string format0(int value, int width);

      static std::string formatHex(int value);

      static std::string formatHex(int value, int width);

      static std::string format(unsigned value);

      static std::string format(unsigned value, int width);

      static std::string format0(unsigned int value, int width);

      static std::string formatHex(unsigned value);

      static std::string formatHex(unsigned value, int width);

      static std::string format(long value);

      static std::string format(long value, int width);

      static std::string format0(long value, int width);

      static std::string formatHex(long value);

      static std::string formatHex(long value, int width);

      static std::string format(unsigned long value);

      static std::string format(unsigned long value, int width);

      static std::string format0(unsigned long value, int width);

      static std::string formatHex(unsigned long value);

      static std::string formatHex(unsigned long value, int width);

      static std::string format(float value);

      static std::string format(double value);

      static std::string format(double value, int precision);

      static std::string format(double value, int width, int precision);


      static void append(std::string& str, int value);

      static void append(std::string& str, int value, int width);

      static void append0(std::string& str, int value, int width);

      static void appendHex(std::string& str, int value);

      static void appendHex(std::string& str, int value, int width);

      static void append(std::string& str, unsigned value);

      static void append(std::string& str, unsigned value, int width);

      static void append0(std::string& str, unsigned int value, int width);

      static void appendHex(std::string& str, unsigned value);

      static void appendHex(std::string& str, unsigned value, int width);

      static void append(std::string& str, long value);

      static void append(std::string& str, long value, int width);

      static void append0(std::string& str, long value, int width);

      static void appendHex(std::string& str, long value);

      static void appendHex(std::string& str, long value, int width);

      static void append(std::string& str, unsigned long value);

      static void append(std::string& str, unsigned long value, int width);

      static void append0(std::string& str, unsigned long value, int width);

      static void appendHex(std::string& str, unsigned long value);

      static void appendHex(std::string& str, unsigned long value, int width);
      
      static void append(std::string& str, float value);

      static void append(std::string& str, double value);

      static void append(std::string& str, double value, int precision);

      static void append(std::string& str, double value, int width, int precision);

   };


   //
   // inlines
   //
   inline std::string NumberFormatter::format(int value)
   {
      std::string result;
      append(result, value);
      return result;
   }


   inline std::string NumberFormatter::format(int value, int width)
   {
      std::string result;
      append(result, value, width);
      return result;
   }


   inline std::string NumberFormatter::format0(int value, int width)
   {
      std::string result;
      append0(result, value, width);
      return result;
   }


   inline std::string NumberFormatter::formatHex(int value)
   {
      std::string result;
      appendHex(result, value);
      return result;
   }


   inline std::string NumberFormatter::formatHex(int value, int width)
   {
      std::string result;
      appendHex(result, value, width);
      return result;
   }


   inline std::string NumberFormatter::format(unsigned value)
   {
      std::string result;
      append(result, value);
      return result;
   }


   inline std::string NumberFormatter::format(unsigned value, int width)
   {
      std::string result;
      append(result, value, width);
      return result;
   }


   inline std::string NumberFormatter::format0(unsigned int value, int width)
   {
      std::string result;
      append0(result, value, width);
      return result;
   }


   inline std::string NumberFormatter::formatHex(unsigned value)
   {
      std::string result;
      appendHex(result, value);
      return result;
   }


   inline std::string NumberFormatter::formatHex(unsigned value, int width)
   {
      std::string result;
      appendHex(result, value, width);
      return result;
   }


   inline std::string NumberFormatter::format(long value)
   {
      std::string result;
      append(result, value);
      return result;
   }


   inline std::string NumberFormatter::format(long value, int width)
   {
      std::string result;
      append(result, value, width);
      return result;
   }


   inline std::string NumberFormatter::format0(long value, int width)
   {
      std::string result;
      append0(result, value, width);
      return result;
   }


   inline std::string NumberFormatter::formatHex(long value)
   {
      std::string result;
      appendHex(result, value);
      return result;
   }


   inline std::string NumberFormatter::formatHex(long value, int width)
   {
      std::string result;
      appendHex(result, value, width);
      return result;
   }


   inline std::string NumberFormatter::format(unsigned long value)
   {
      std::string result;
      append(result, value);
      return result;
   }


   inline std::string NumberFormatter::format(unsigned long value, int width)
   {
      std::string result;
      append(result, value, width);
      return result;
   }


   inline std::string NumberFormatter::format0(unsigned long value, int width)
   {
      std::string result;
      append0(result, value, width);
      return result;
   }


   inline std::string NumberFormatter::formatHex(unsigned long value)
   {
      std::string result;
      appendHex(result, value);
      return result;
   }


   inline std::string NumberFormatter::formatHex(unsigned long value, int width)
   {
      std::string result;
      appendHex(result, value, width);
      return result;
   }

   inline std::string NumberFormatter::format(float value)
   {
      std::string result;
      append(result, value);
      return result;
   }


   inline std::string NumberFormatter::format(double value)
   {
      std::string result;
      append(result, value);
      return result;
   }


   inline std::string NumberFormatter::format(double value, int precision)
   {
      std::string result;
      append(result, value, precision);
      return result;
   }


   inline std::string NumberFormatter::format(double value, int width, int precision)
   {
      std::string result;
      append(result, value, width, precision);
      return result;
   }
   
}   // End of namespace gpstk


#endif  //GPSTK_NUMBERFORMATTER_HPP

