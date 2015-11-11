//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/** @file RinexDatum.cpp
 * Defines class methods for a single RINEX datum.
 */

#include "RinexDatum.hpp"
#include "Exception.hpp"
#include "StringUtils.hpp"

namespace gpstk
{
   RinexDatum ::
   RinexDatum()
         : data(0), lli(0), ssi(0),
           dataBlank(false), lliBlank(false), ssiBlank(false)
   {
         // blanks are false by default for backwards compatibility
   }


   RinexDatum ::
   RinexDatum(const std::string& str)
   {
      fromString(str);
   }


   void RinexDatum ::
   fromString(const std::string& str)
   {
      std::string tmpStr;
      GPSTK_ASSERT(str.length() == 16);
      tmpStr = str.substr(0, 14);
      if (tmpStr.find_last_not_of(" ") == std::string::npos)
      {
         data = 0.;
         dataBlank = true;
      }
      else
      {
         data = StringUtils::asDouble(tmpStr);
         dataBlank = false;
      }
      tmpStr = str.substr(14, 1);
      if (tmpStr == " ")
      {
         lli = 0.;
         lliBlank = true;
      }
      else
      {
         lli = StringUtils::asInt(tmpStr);
         lliBlank = false;
      }
      tmpStr = str.substr(15, 1);
      if (tmpStr == " ")
      {
         ssi = 0.;
         ssiBlank = true;
      }
      else
      {
         ssi = StringUtils::asInt(tmpStr);
         ssiBlank = false;
      }
   }


   std::string RinexDatum ::
   asString() const
   {
      std::string rv;
      using gpstk::StringUtils::rightJustify;

      if (!dataBlank)
      {
            // double 14.3
         rv += rightJustify(gpstk::StringUtils::asString(data, 3), 14);
      }
      else
      {
         rv += std::string(14, ' ');
      }
      if ((lli != 0) || !lliBlank)
      {
         rv += rightJustify(gpstk::StringUtils::asString<short>(lli),1);
      }
      else
      {
         rv += " ";
      }
      if ((ssi != 0) || !ssiBlank)
      {
         rv += rightJustify(gpstk::StringUtils::asString<short>(ssi),1);
      }
      else
      {
         rv += " ";
      }
      return rv;
   } // asString() const

} // namespace gpstk
