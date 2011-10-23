#pragma ident "$Id$"

/**
* @file FormatUtils.cpp
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

#include "FormatUtils.hpp"
#include "Exception.hpp"
#include "Ascii.hpp"
#include <sstream>
#include <cstddef>

namespace gpstk
{
   using namespace std;


   void FormatUtils::parseFlags(std::ostream& str, std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt)
   {
      bool isFlag = true;
      while (isFlag && itFmt != endFmt)
      {
         switch (*itFmt)
         {
         case '-': str.setf(std::ios::left); ++itFmt; break;
         case '+': str.setf(std::ios::showpos); ++itFmt; break;
         case '0': str.fill('0'); ++itFmt; break;
         case '#': str.setf(std::ios::showpoint | std::ios_base::showbase); ++itFmt; break;
         default:  isFlag = false; break;
         }
      }
   }


   void FormatUtils::parseWidth(std::ostream& str, std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt)
   {
      int width = 0;
      while (itFmt != endFmt && Ascii::isDigit(*itFmt))
      {
         width = 10*width + *itFmt - '0';
         ++itFmt;
      }
      if (width != 0) str.width(width);
   }


   void FormatUtils::parsePrec(std::ostream& str, std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt)
   {
      if (itFmt != endFmt && *itFmt == '.')
      {
         ++itFmt;
         int prec = 0;
         while (itFmt != endFmt && Ascii::isDigit(*itFmt))
         {
            prec = 10*prec + *itFmt - '0';
            ++itFmt;
         }
         if (prec >= 0) str.precision(prec);
      }
   }


   char FormatUtils::parseMod(std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt)
   {
      char mod = 0;
      if (itFmt != endFmt)
      {
         switch (*itFmt)
         {
         case 'l':
         case 'h':
         case 'L': 
         case '?': mod = *itFmt++; break;
         }
      }
      return mod;
   }


   std::size_t FormatUtils::parseIndex(std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt)
   {
      int index = 0;
      while (itFmt != endFmt && Ascii::isDigit(*itFmt))
      {
         index = 10*index + *itFmt - '0';
         ++itFmt;
      }
      if (itFmt != endFmt && *itFmt == ']') ++itFmt;
      return index;
   }


   void FormatUtils::prepareFormat(std::ostream& str, char type)
   {
      switch (type)
      {
      case 'd':
      case 'i': str << std::dec; break;
      case 'o': str << std::oct; break;
      case 'x': str << std::hex; break;
      case 'X': str << std::hex << std::uppercase; break;
      case 'e': str << std::scientific; break;
      case 'E': str << std::scientific << std::uppercase; break;
      case 'f': str << std::fixed; break;
      }
   }


   void FormatUtils::writeAnyInt(std::ostream& str, const Any& any)
   {
      if (any.type() == typeid(char))
         str << static_cast<int>(AnyCast<char>(any));
      else if (any.type() == typeid(signed char))
         str << static_cast<int>(AnyCast<signed char>(any));
      else if (any.type() == typeid(unsigned char))
         str << static_cast<unsigned>(AnyCast<unsigned char>(any));
      else if (any.type() == typeid(short))
         str << AnyCast<short>(any);
      else if (any.type() == typeid(unsigned short))
         str << AnyCast<unsigned short>(any);
      else if (any.type() == typeid(int))
         str << AnyCast<int>(any);
      else if (any.type() == typeid(unsigned int))
         str << AnyCast<unsigned int>(any);
      else if (any.type() == typeid(long))
         str << AnyCast<long>(any);
      else if (any.type() == typeid(unsigned long))
         str << AnyCast<unsigned long>(any);
      else if (any.type() == typeid(Int64))
         str << AnyCast<Int64>(any);
      else if (any.type() == typeid(UInt64))
         str << AnyCast<UInt64>(any);
      else if (any.type() == typeid(bool))
         str << AnyCast<bool>(any);
   }


   void FormatUtils::formatOne(std::string& result, std::string::const_iterator& itFmt, const std::string::const_iterator& endFmt, std::vector<Any>::const_iterator& itVal)
   {
      std::ostringstream str;
      parseFlags(str, itFmt, endFmt);
      parseWidth(str, itFmt, endFmt);
      parsePrec(str, itFmt, endFmt);
      char mod = parseMod(itFmt, endFmt);
      if (itFmt != endFmt)
      {
         char type = *itFmt++;
         prepareFormat(str, type);
         switch (type)
         {
         case 'b':
            str << AnyCast<bool>(*itVal++);
            break;
         case 'c':
            str << AnyCast<char>(*itVal++);
            break;
         case 'd':
         case 'i':
            switch (mod)
            {
            case 'l': str << AnyCast<long>(*itVal++); break;
            case 'L': str << AnyCast<Int64>(*itVal++); break;
            case 'h': str << AnyCast<short>(*itVal++); break;
            case '?': writeAnyInt(str, *itVal++); break;
            default:  str << AnyCast<int>(*itVal++); break;
            }
            break;
         case 'o':
         case 'u':
         case 'x':
         case 'X':
            switch (mod)
            {
            case 'l': str << AnyCast<unsigned long>(*itVal++); break;
            case 'L': str << AnyCast<UInt64>(*itVal++); break;
            case 'h': str << AnyCast<unsigned short>(*itVal++); break;
            case '?': writeAnyInt(str, *itVal++); break;
            default:  str << AnyCast<unsigned>(*itVal++); break;
            }
            break;
         case 'e':
         case 'E':
         case 'f':
            switch (mod)
            {
            case 'l': str << AnyCast<long double>(*itVal++); break;
            case 'L': str << AnyCast<long double>(*itVal++); break;
            case 'h': str << AnyCast<float>(*itVal++); break;
            default:  str << AnyCast<double>(*itVal++); break;
            }
            break;
         case 's':
            str << RefAnyCast<std::string>(*itVal++);
            break;
         case 'z':
            str << AnyCast<std::size_t>(*itVal++); 
            break;
         case 'I':
         case 'D':
         default:
            str << type;
         }
      }
      result.append(str.str());
   }


   std::string FormatUtils::format(const std::string& fmt, const Any& value)
   {
      std::string result;
      format(result, fmt, value);
      return result;
   }


   std::string FormatUtils::format(const std::string& fmt, const Any& value1, const Any& value2)
   {
      std::string result;
      format(result, fmt, value1, value2);
      return result;
   }


   std::string FormatUtils::format(const std::string& fmt, const Any& value1, const Any& value2, const Any& value3)
   {
      std::string result;
      format(result, fmt, value1, value2, value3);
      return result;
   }


   std::string FormatUtils::format(const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4)
   {
      std::string result;
      format(result, fmt, value1, value2, value3, value4);
      return result;
   }


   std::string FormatUtils::format(const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5)
   {
      std::string result;
      format(result, fmt, value1, value2, value3, value4, value5);
      return result;
   }


   std::string FormatUtils::format(const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5, const Any& value6)
   {
      std::string result;
      format(result, fmt, value1, value2, value3, value4, value5, value6);
      return result;
   }


   void FormatUtils::format(std::string& result, const std::string& fmt, const Any& value)
   {
      std::vector<Any> args;
      args.push_back(value);
      format(result, fmt, args);
   }


   void FormatUtils::format(std::string& result, const std::string& fmt, const Any& value1, const Any& value2)
   {
      std::vector<Any> args;
      args.push_back(value1);
      args.push_back(value2);
      format(result, fmt, args);
   }


   void FormatUtils::format(std::string& result, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3)
   {
      std::vector<Any> args;
      args.push_back(value1);
      args.push_back(value2);
      args.push_back(value3);
      format(result, fmt, args);
   }


   void FormatUtils::format(std::string& result, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4)
   {
      std::vector<Any> args;
      args.push_back(value1);
      args.push_back(value2);
      args.push_back(value3);
      args.push_back(value4);
      format(result, fmt, args);
   }


   void FormatUtils::format(std::string& result, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5)
   {
      std::vector<Any> args;
      args.push_back(value1);
      args.push_back(value2);
      args.push_back(value3);
      args.push_back(value4);
      args.push_back(value5);
      format(result, fmt, args);
   }


   void FormatUtils::format(std::string& result, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5, const Any& value6)
   {
      std::vector<Any> args;
      args.push_back(value1);
      args.push_back(value2);
      args.push_back(value3);
      args.push_back(value4);
      args.push_back(value5);
      args.push_back(value6);
      format(result, fmt, args);
   }

   void FormatUtils::format(std::string& result, const std::string& fmt, const std::vector<Any>& values)
   {
      std::string::const_iterator itFmt  = fmt.begin();
      std::string::const_iterator endFmt = fmt.end();
      std::vector<Any>::const_iterator itVal  = values.begin();
      std::vector<Any>::const_iterator endVal = values.end(); 
      while (itFmt != endFmt)
      {
         switch (*itFmt)
         {
         case '%':
            ++itFmt;
            if (itFmt != endFmt && itVal != endVal)
            {
               if (*itFmt == '[')
               {
                  ++itFmt;
                  std::size_t index = parseIndex(itFmt, endFmt);
                  if (index < values.size())
                  {
                     std::vector<Any>::const_iterator it = values.begin() + index;
                     formatOne(result, itFmt, endFmt, it);
                  }
                  else throw Exception("format argument index out of range '" + fmt + "'.");
               }
               else
               {
                  formatOne(result, itFmt, endFmt, itVal);
               }
            }
            else if (itFmt != endFmt)
            {
               result += *itFmt++;
            }
            break;
         default:
            result += *itFmt;
            ++itFmt;
         }
      }
   }


}   // End of namespace gpstk

