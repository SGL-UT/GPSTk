#pragma ident "$Id$"

/**
 * @file Rinex3ClockHeader.cpp
 * Encapsulate header of RINEX3 clock file, including I/O
 * See more at: ftp://igscb.jpl.nasa.gov/pub/data/format/rinex_clock.txt
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

//system
#include<cmath>
//GPSTk
#include "CivilTime.hpp"
#include "Rinex3ClockHeader.hpp"
#include "Rinex3ClockStream.hpp"
#include "StringUtils.hpp"

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{
   const string Rinex3ClockHeader::versionString     =  "RINEX VERSION / TYPE";
   const string Rinex3ClockHeader::runByString       =  "PGM / RUN BY / DATE";
   const string Rinex3ClockHeader::commentString     =  "COMMENT";
   const string Rinex3ClockHeader::numObsString      =  "SYS / # / OBS TYPES";
   const string Rinex3ClockHeader::timeSystemString  =  "TIME SYSTEM ID";
   const string Rinex3ClockHeader::leapSecondsString =  "LEAP SECONDS";
   const string Rinex3ClockHeader::sysDCBString      =  "SYS / DCBS APPLIED";
   const string Rinex3ClockHeader::sysPCVString      =  "SYS / PCVS APPLIED";
   const string Rinex3ClockHeader::dataTypesString   =  "# / TYPES OF DATA";
   const string Rinex3ClockHeader::stationNameString =  "STATION NAME / NUM";
   const string Rinex3ClockHeader::calibrationClkString = "STATION CLK REF";
   const string Rinex3ClockHeader::acNameString      =   "ANALYSIS CENTER";
   const string Rinex3ClockHeader::numRefClkString   =  "# OF CLK REF";
   const string Rinex3ClockHeader::analysisClkRefString = "ANALYSIS CLK REF";
   const string Rinex3ClockHeader::numStationsString = "# OF SOLN STA / TRF";
   const string Rinex3ClockHeader::solnStaNameString =  "SOLN STA NAME / NUM";
   const string Rinex3ClockHeader::numSatsString     =  "# OF SOLN SATS";
   const string Rinex3ClockHeader::prnListString     =  "PRN LIST";
   const string Rinex3ClockHeader::endOfHeader       =  "END OF HEADER";


   const Rinex3ClockHeader::RinexClkType 
      Rinex3ClockHeader::UN("UN", "Unknown or Invalid");
   const Rinex3ClockHeader::RinexClkType 
      Rinex3ClockHeader::AR("AR", "analysis data for receiver clocks");
   const Rinex3ClockHeader::RinexClkType 
      Rinex3ClockHeader::AS("AS", "analysis data for satellite clocks");
   const Rinex3ClockHeader::RinexClkType 
      Rinex3ClockHeader::CR("CR", "calibration data");
   const Rinex3ClockHeader::RinexClkType 
      Rinex3ClockHeader::DR("DR", "discontinuity data");
   const Rinex3ClockHeader::RinexClkType 
      Rinex3ClockHeader::MS("MS", "monitor data");

      // Clear (empty out) header
   void Rinex3ClockHeader::clear(void)
   {

      cout << "WARNING: There is no implementation for "
           << "Rinex3ClockHeader::clear()"
           << endl;

      return;

   }  // End of method 'Rinex3ClockHeader::clear()'



   void Rinex3ClockHeader::reallyPutRecord(FFStream& ffs) const 
      throw(std::exception, FFStreamError, StringException)
   {
      cout << "WARNING: There is no implementation for "
           << "Rinex3ClockHeader::reallyPutRecord()"
           << endl;

      return;

   }  // End of method 'Rinex3ClockHeader::reallyPutRecord()'



      // This function parses the entire header from the given stream
   void Rinex3ClockHeader::reallyGetRecord(FFStream& ffs)
      throw(std::exception, FFStreamError, 
            StringUtils::StringException)
   {

      Rinex3ClockStream& strm = dynamic_cast<Rinex3ClockStream&>(ffs);

         // if already read, just return
      if (strm.headerRead == true)
         return;

         // since we're reading a new header, we need to reinitialize
         // all our list structures.  all the other objects should be ok.
         // this also applies if we threw an exception the first time we read
         // the header and are now re-reading it. some of these could be full
         // and we need to empty them.
      clear();

         // one file line
      string line;
      string label = "";

      while ( label != endOfHeader )
      {
         strm.formattedGetLine(line);
         StringUtils::stripTrailing(line);

         if ( line.length() == 0 )
         {
            FFStreamError ffse("No data read!");
            GPSTK_THROW(ffse);
         }
         else if ( line.length() < 60 || line.length() > 80 )
         {
            FFStreamError ffse("Invalid line length");
            GPSTK_THROW(ffse);
         }

         try
         {
            ParseHeaderRecord(line);
            label = line.substr(60, 20);
         }
         catch(FFStreamError& ffse)
         {
            GPSTK_RETHROW(ffse);
         }
         
      }   // end while(not end of header)


         // If we get here, we should have reached the end of header line
      strm.header = *this;
      strm.headerRead = true;

      return;

   }  // End of method 'Rinex3ClockHeader::reallyGetRecord(FFStream& ffs)'



      // this function parses a single header record
   void Rinex3ClockHeader::ParseHeaderRecord(string& line)
      throw(FFStreamError)
   {

      cout << "WARNING: There is no implementation for "
           << "Rinex3ClockHeader::ParseHeaderRecord()"
           << endl;

      return;

   }   // End of method 'Rinex3ClockHeader::ParseHeaderRecord(string& line)'



      /** This function sets the time for this header.
       * It looks at \a line to obtain the needed information.
       */
   CommonTime Rinex3ClockHeader::parseTime(const string& line) const
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

   }  // End of method 'Rinex3ClockHeader::parseTime(const string& line)'


      /// Converts the CommonTime \a dt into a Rinex3 Clock time
      /// string for the header
   string Rinex3ClockHeader::writeTime(const CommonTime& dt) const
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

   }  // End of method 'Rinex3ClockHeader::writeTime(const CommonTime& dt)'



      // Debug output function.
   void Rinex3ClockHeader::dump(ostream& s) const
   {
      cout << "WARNING: There is no implementation for "
           << "Rinex3ClockHeader::dump()"
           << endl;

   }  // End of method 'Rinex3ClockHeader::dump(ostream& s)'


}  // End of namespace gpstk