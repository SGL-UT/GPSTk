#pragma ident "$Id$"

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

/**
 * @file Rinex3ClockData.cpp
 * Encapsulate RINEX3 clock data file, including I/O
 * See more at: ftp://igscb.jpl.nasa.gov/pub/data/format/rinex_clock.txt
 */

//GPSTk
#include "CivilTime.hpp"
#include "Rinex3ClockData.hpp"
#include "Rinex3ClockStream.hpp"

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{
      // Debug output function.
   void Rinex3ClockData::dump(std::ostream& s) const
   {

      cout << "WARNING: There is no implementation for "
           << "Rinex3ClockData::dump()"
           << endl;

      return;

   }  // End of method 'Rinex3ClockData::dump(std::ostream& s)'



   void Rinex3ClockData::reallyPutRecord(FFStream& s) const 
         throw(std::exception, FFStreamError,
               StringUtils::StringException)
   {
      cout << "WARNING: There is no implementation for "
           << "Rinex3ClockData::reallyPutRecord()"
           << endl;

      return;

   }  // End of method 'Rinex3ClockData::reallyPutRecord()'



      // This function parses the entire header from the given stream
   void Rinex3ClockData::reallyGetRecord(FFStream& ffs)
   {
      Rinex3ClockStream& strm = dynamic_cast<Rinex3ClockStream&>(ffs);

      string line;

      strm.formattedGetLine(line, true);
      //cout << line << endl;

      return;

   }  // End of method 'Rinex3ClockData::reallyGetRecord(FFStream& ffs)'



         /** This function constructs a CommonTime object from the given parameters.
          * @param line       the encoded time string found in the 
          *                   RINEX clock data record.
          */
   CommonTime Rinex3ClockData::parseTime(const string& line) const
   {

      int year, month, day, hour, min;
      double sec;

      year  = asInt(   line.substr( 0, 4 ));
      month = asInt(   line.substr( 4, 3 ));
      day   = asInt(   line.substr( 7, 3 ));
      hour  = asInt(   line.substr(10, 3 ));
      min   = asInt(   line.substr(13, 3 ));
      sec   = asDouble(line.substr(16, 10));

      return CivilTime(year, month, day, hour, min, sec).convertToCommonTime();

   }  // End of method 'Rinex3ClockData::parseTime()'



      /// Converts the CommonTime \a dt into a Rinex3 Clock time
      /// string for the header
   string Rinex3ClockData::writeTime(const CommonTime& dt) const
      throw(gpstk::StringUtils::StringException)
   {

      if (dt == CommonTime::BEGINNING_OF_TIME)
      {
         return string(36, ' ');
      }

      string line;
      CivilTime civTime(dt);
      line  = rightJustify(asString<short>(civTime.year), 4);
      line += rightJustify(asString<short>(civTime.month), 3);
      line += rightJustify(asString<short>(civTime.day), 3);
      line += rightJustify(asString<short>(civTime.hour), 3);
      line += rightJustify(asString<short>(civTime.minute), 3);
      line += rightJustify(asString(civTime.second, 6), 10);

      return line;

   }  // End of method 'Rinex3ClockData::writeTime(const CommonTime& dt)'


}  // End of namespace gpstk
