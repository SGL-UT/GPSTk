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
//  Copyright 2015, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
// This software developed by Applied Research Laboratories at the
// University of Texas at Austin, under contract to an agency or
// agencies within the U.S.  Department of Defense. The
// U.S. Government retains all rights to use, duplicate, distribute,
// disclose, or release this software.
//
// Pursuant to DoD Directive 523024
//
// DISTRIBUTION STATEMENT A: This software has been approved for public
//                           release, distribution is unlimited.
//
//=============================================================================

#include "RinexClockBase.hpp"

using namespace std;

namespace gpstk
{
   const RinexClockBase::RinexClkType 
   RinexClockBase::UN("UN", "Unknown or Invalid");
   const RinexClockBase::RinexClkType 
   RinexClockBase::AR("AR", "analysis data for receiver clocks");
   const RinexClockBase::RinexClkType 
   RinexClockBase::AS("AS", "analysis data for satellite clocks");
   const RinexClockBase::RinexClkType 
   RinexClockBase::CR("CR", "calibration data");
   const RinexClockBase::RinexClkType 
   RinexClockBase::DR("DR", "discontinuity data");
   const RinexClockBase::RinexClkType 
   RinexClockBase::MS("MS", "monitor data");
   
   
   string RinexClockBase::writeTime(const CivilTime& dt) const
   {
      if (dt == CommonTime::BEGINNING_OF_TIME)
      {
         return std::string(26, ' ');
      }
      
      std::string line;
      line  = StringUtils::rightJustify(
         StringUtils::asString<short>(dt.year), 4);
      line += StringUtils::rightJustify(
         StringUtils::asString<short>(dt.month), 3);
      line += StringUtils::rightJustify(
         StringUtils::asString<short>(dt.day), 3);
      line += StringUtils::rightJustify(
         StringUtils::asString<short>(dt.hour), 3);
      line += StringUtils::rightJustify(
         StringUtils::asString<short>(dt.minute), 3);
      line += StringUtils::rightJustify(
         StringUtils::asString(dt.second, 6), 10);
      
      return line;
   }  // writeTime

   
   CivilTime RinexClockBase::parseTime(const string& line) const
      throw(FFStreamError)
   {
      if ( line.size() != 26 )
      {
         FFStreamError e("Invalid time string length: " + 
                         StringUtils::asString(line.size()));
         GPSTK_THROW(e);
      }
      
      if ( line == std::string(26, ' ') )
      {
         return CommonTime::BEGINNING_OF_TIME;
      }
      
      int year, month, day, hour, min;
      double sec;
      
      year = StringUtils::asInt( line.substr(0, 4) );
      month = StringUtils::asInt( line.substr(4, 3) );
      day = StringUtils::asInt( line.substr(7, 3) );
      hour = StringUtils::asInt( line.substr(10, 3) );
      min = StringUtils::asInt( line.substr(13, 3) );
      sec = StringUtils::asDouble( line.substr(16, 10) );
      
      return CivilTime(year, month, day, hour, min, sec);

   }  // parseTime
   
   
}  // namespace

