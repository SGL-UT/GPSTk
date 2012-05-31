#pragma ident "$Id$"

/**
 * @file NumberParser.hpp
 * Modified from Poco, Original copyright by Applied Informatics.
 */

#ifndef GPSTK_NUMBERPARSER_HPP
#define GPSTK_NUMBERPARSER_HPP

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

#include <iostream>
#include <string>

namespace gpstk
{
   class NumberParser     
   {
   public:
      
      static int parse(const std::string& s);

      static bool tryParse(const std::string& s, int& value);

      static unsigned parseUnsigned(const std::string& s);

      static bool tryParseUnsigned(const std::string& s, unsigned& value);

      static unsigned parseHex(const std::string& s);

      static bool tryParseHex(const std::string& s, unsigned& value);

      static double parseFloat(const std::string& s);

      static bool tryParseFloat(const std::string& s, double& value);
      
   protected:
      NumberParser(){}
   };
   
}   // End of namespace gpstk


#endif  //GPSTK_NUMBERPARSER_HPP

