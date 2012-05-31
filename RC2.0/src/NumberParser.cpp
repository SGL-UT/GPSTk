#pragma ident "$Id$"

/**
 * @file NumberParser.cpp
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

#include <cstdio>  // For std::sscanf()
#include "NumberParser.hpp"
#include "Exception.hpp"

namespace gpstk
{
   int NumberParser::parse(const std::string& s)
   {
      int result;
      if (tryParse(s, result))
         return result;
      else
         throw Exception("Not a valid integer "+ s);
   }


   bool NumberParser::tryParse(const std::string& s, int& value)
   {
      char temp;
      return std::sscanf(s.c_str(), "%d%c", &value, &temp) == 1;
   }


   unsigned NumberParser::parseUnsigned(const std::string& s)
   {
      unsigned result;
      if (tryParseUnsigned(s, result))
         return result;
      else
         throw Exception("Not a valid unsigned integer "+s);
   }


   bool NumberParser::tryParseUnsigned(const std::string& s, unsigned& value)
   {
      char temp;
      return std::sscanf(s.c_str(), "%u%c", &value, &temp) == 1;
   }


   unsigned NumberParser::parseHex(const std::string& s)
   {
      unsigned result;
      if (tryParseHex(s, result))
         return result;
      else
         throw Exception("Not a valid hexadecimal integer " + s);
   }


   bool NumberParser::tryParseHex(const std::string& s, unsigned& value)
   {
      char temp;
      return std::sscanf(s.c_str(), "%x%c", &value, &temp) == 1;
   }


   double NumberParser::parseFloat(const std::string& s)
   {
      double result;
      if (tryParseFloat(s, result))
         return result;
      else
         throw Exception("Not a valid floating-point number" + s);
   }


   bool NumberParser::tryParseFloat(const std::string& s, double& value)
   {
      char temp;
      return std::sscanf(s.c_str(), "%lf%c", &value, &temp) == 1;
   }
   
}   // End of namespace gpstk

