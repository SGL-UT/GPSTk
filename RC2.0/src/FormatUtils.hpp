#pragma ident "$Id$"

/**
 * @file FormatUtils.hpp
 * Modified from Poco, Original copyright by Applied Informatics.
 */

#ifndef GPSTK_FORMATUTILS_HPP
#define GPSTK_FORMATUTILS_HPP

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

#include <vector>
#include <string>
#include "Any.hpp"

namespace gpstk
{
      /// This function implements sprintf-style formatting in a typesafe way.
      /// Various variants of the function are available, supporting a
      /// different number of arguments (up to six).
      ///
      /// The formatting is controlled by the format string in fmt.
      /// Format strings are quite similar to those of the std::printf() function, but
      /// there are some minor differences.
      ///
      /// The format string can consist of any sequence of characters; certain
      /// characters have a special meaning. Characters without a special meaning
      /// are copied verbatim to the result. A percent sign (%) marks the beginning
      /// of a format specification. Format specifications have the following syntax:
      ///
      ///   %[<index>][<flags>][<width>][.<precision>][<modifier>]<type>
      ///
      /// Index, flags, width, precision and prefix are optional. The only required part of
      /// the format specification, apart from the percent sign, is the type.
      ///
      /// The optional index argument has the format "[<n>]" and allows to
      /// address an argument by its zero-based position (see the example below).
      ///
      /// Following are valid type specifications and their meaning:
      ///
      ///   * b boolean (true = 1, false = 0)
      ///   * c character
      ///   * d signed decimal integer
      ///   * i signed decimal integer
      ///   * o unsigned octal integer
      ///   * u unsigned decimal integer
      ///   * x unsigned hexadecimal integer (lower case)
      ///   * X unsigned hexadecimal integer (upper case)
      ///   * e signed floating-point value in the form [-]d.dddde[<sign>]dd[d]
      ///   * E signed floating-point value in the form [-]d.ddddE[<sign>]dd[d]
      ///   * f signed floating-point value in the form [-]dddd.dddd
      ///   * s std::string
      ///   * z std::size_t
      ///
      /// The following flags are supported:
      ///
      ///   * - left align the result within the given field width
      ///   * + prefix the output value with a sign (+ or -) if the output value is of a signed type
      ///   * 0 if width is prefixed with 0, zeros are added until the minimum width is reached
      ///   * # For o, x, X, the # flag prefixes any nonzero output value with 0, 0x, or 0X, respectively; 
      ///     for e, E, f, the # flag forces the output value to contain a decimal point in all cases.
      ///
      /// The following modifiers are supported:
      ///
      ///   * (none) argument is char (c), int (d, i), unsigned (o, u, x, X) double (e, E, f, g, G) or string (s)
      ///   * l      argument is long (d, i), unsigned long (o, u, x, X) or long double (e, E, f, g, G)
      ///   * L      argument is long long (d, i), unsigned long long (o, u, x, X)
      ///   * h      argument is short (d, i), unsigned short (o, u, x, X) or float (e, E, f, g, G)
      ///   * ?      argument is any signed or unsigned int, short, long, or 64-bit integer (d, i, o, x, X)
      ///
      /// The width argument is a nonnegative decimal integer controlling the minimum number of characters printed.
      /// If the number of characters in the output value is less than the specified width, blanks or
      /// leading zeros are added, according to the specified flags (-, +, 0).
      ///
      /// Precision is a nonnegative decimal integer, preceded by a period (.), which specifies the number of characters 
      /// to be printed, the number of decimal places, or the number of significant digits.
      ///
      /// Throws a BadCastException if an argument does not correspond to the type of its format specification.
      /// Throws an InvalidArgumentException if an argument index is out of range.
      /// 
      /// If there are more format specifiers than values, the format specifiers without a corresponding value
      /// are copied verbatim to output.
      ///
      /// If there are more values than format specifiers, the superfluous values are ignored.
      ///
      /// Usage Examples:
      ///     std::string s1 = format("The answer to life, the universe, and everything is %d", 42);
      ///     std::string s2 = format("second: %[1]d, first: %[0]d", 1, 2);
      ///
   class FormatUtils
   {
   public:
      static std::string format(const std::string& fmt, const Any& value);
      static std::string format(const std::string& fmt, const Any& value1, const Any& value2);
      static std::string format(const std::string& fmt, const Any& value1, const Any& value2, const Any& value3);
      static std::string format(const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4);
      static std::string format(const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5);
      static std::string format(const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5, const Any& value6);

      
      static void format(std::string& result, const std::string& fmt, const Any& value);
      static void format(std::string& result, const std::string& fmt, const Any& value1, const Any& value2);
      static void format(std::string& result, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3);
      static void format(std::string& result, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4);
      static void format(std::string& result, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5);
      static void format(std::string& result, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5, const Any& value6);

      static void format(std::string& result, const std::string& fmt, const std::vector<Any>& values);
      
   protected:
      FormatUtils(){}

      static void parseFlags(std::ostream& str, std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt);
      static void parseWidth(std::ostream& str, std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt);
      static void parsePrec(std::ostream& str, std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt);
      static char parseMod(std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt);
      static std::size_t parseIndex(std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt);
      static void prepareFormat(std::ostream& str, char type);

      static void writeAnyInt(std::ostream& str, const Any& any);

      static void formatOne(std::string& result, std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt, std::vector<Any>::const_iterator& itVal);
   };


}   // End of namespace gpstk


#endif  //GPSTK_FORMATUTILS_HPP

