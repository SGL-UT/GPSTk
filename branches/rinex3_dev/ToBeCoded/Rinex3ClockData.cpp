#pragma ident "$Id$"

/**
 * @file Rinex3ClockData.cpp
 * Encapsulate RINEX3 clock data file, including I/O
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
//  Octavian Andrei - FGI ( http://www.fgi.fi ). 2008
//
//============================================================================

//GPSTk
#include "Rinex3ClockData.hpp"
#include "Rinex3ClockStream.hpp"

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{
      // Debug output function.
   void Rinex3ClockData::dump(std::ostream& s) const
   {

      string line = leftJustify(type,3);
      line += leftJustify(name,5);
      line += time.printf("%04Y %02m %02d %02H %02M");
      line += rightJustify(asString(time.second(),6),10);
      line += rightJustify(asString(numVal), 3);
      line += string(3, ' ');
      s << line;
      //line += rightJustify( asString(data[0],12),20 );
      //line += rightJustify( asString(data[1],12),20 );

      s << scientific << setw(20) << setprecision(12)<< data[0];
      s << scientific << setw(20) << setprecision(12)<< data[1] << endl;

      if(numVal > 2)
      {
         line.clear();

         for(size_t iVal = 2; iVal < numVal; iVal++)
         {
            //line += rightJustify( asString(data[iVal],12),20 );
            s << scientific << setw(19) << setprecision(12)<< data[iVal] << " ";
         }
         s << endl;
      }

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
      throw(std::exception, FFStreamError, 
            StringUtils::StringException)
   {
      Rinex3ClockStream& strm = dynamic_cast<Rinex3ClockStream&>(ffs);

        // If the header hasn't been read, read it...
      if(!strm.headerRead)
      {
         strm >> strm.header;
      }

         // Clear out this object
      Rinex3ClockHeader& hdr = strm.header;

      Rinex3ClockData rcd;
      *this=rcd;

      string line;

      strm.formattedGetLine(line, true);
      //cout << line << endl;

         // clock data type
      type =  line.substr(0,2);
         // receiver or satellite id
      name = line.substr(3,4);
         // epoch
      time = parseTime(line.substr(8,26));
         // number of clock data values
      numVal = asInt(line.substr(34,3));

         // let's check if there is a continuation line
      string s = leftJustify(line.substr(40,40),40);
      if(numVal > 2)
      {
         strm.formattedGetLine(line, true);
         s.append(line);
      }

         // now let's grab data
      for(size_t iVal = 0; iVal < numVal; iVal++)
      {
         data[iVal] = asDouble( strip(s.substr(20*iVal,20)) );
      }

         /// now you can get the next record

      return;

   }  // End of method 'Rinex3ClockData::reallyGetRecord(FFStream& ffs)'



         /** This function constructs a DayTime object from the given parameters.
          * @param line       the encoded time string found in the 
          *                   RINEX clock data record.
          */
   DayTime Rinex3ClockData::parseTime(const string& line) const
   {
      int year, month, day, hour, min;
      double sec;
   
      year  = asInt(   line.substr( 0, 4 ));
      month = asInt(   line.substr( 4, 3 ));
      day   = asInt(   line.substr( 7, 3 ));
      hour  = asInt(   line.substr(10, 3 ));
      min   = asInt(   line.substr(13, 3 ));
      sec   = asDouble(line.substr(16, 10));

      return DayTime(year, month, day, hour, min, sec);

   }  // End of method 'Rinex3ClockData::parseTime()'



      /// Converts the daytime \a dt into a Rinex3 Clock time
      /// string for the header
   string Rinex3ClockData::writeTime(const DayTime& dt) const
   {
      string line;
      line  = rightJustify(asString<short>(dt.year()), 4);
      line += rightJustify(asString<short>(dt.month()), 3);
      line += rightJustify(asString<short>(dt.day()), 3);
      line += rightJustify(asString<short>(dt.hour()), 3);
      line += rightJustify(asString<short>(dt.minute()), 3);
      line += rightJustify(asString(dt.second(), 6), 10);

      return line;

   }  // End of method 'Rinex3ClockData::writeTime(const DayTime& dt)'


}  // End of namespace gpstk