#pragma ident "$Id"

/**
 * @file RinexGloNavData.cpp
 * Encapsulate header of Rinex GLONASSs navigation file
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2011
//
//============================================================================


#include "RinexGloNavData.hpp"
#include "RinexGloNavStream.hpp"
#include "CivilTime.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{


      // Outputs the record to the FFStream \a s.
   void RinexGloNavData::reallyPutRecord( FFStream& ffs ) const
      throw( exception, FFStreamError, StringException )
   {

      RinexGloNavStream& strm = dynamic_cast<RinexGloNavStream&>(ffs);

      strm << putPRNEpoch() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit1() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit2() << endl;
      strm.lineNumber++;
      strm << putBroadcastOrbit3() << endl;
      strm.lineNumber++;

   }  // End of method 'RinexGloNavData::reallyPutRecord( FFStream& ffs )'


      /* This function retrieves a RINEX 3 NAV record from the given
       *  FFStream. If an error is encountered in reading from the stream,
       *  the stream is returned to its original position and its fail-bit
       *  is set.
       *
       * @throws StringException when a StringUtils function fails.
       * @throws FFStreamError when exceptions(failbit) is set and
       *    a read or formatting error occurs.  This also resets the
       *    stream to its pre-read position.
       */
   void RinexGloNavData::reallyGetRecord( FFStream& ffs )
      throw( exception, FFStreamError, StringException )
   {

      RinexGloNavStream& strm = dynamic_cast<RinexGloNavStream&>(ffs);

         // If the header hasn't been read, read it...
      if( !strm.headerRead ) strm >> strm.header;

      string line;

      strm.formattedGetLine(line, true);
      getPRNEpoch(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit1(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit2(line);

      strm.formattedGetLine(line);
      getBroadcastOrbit3(line);

   }  // End of method 'RinexGloNavData::reallyGetRecord( FFStream& ffs )'


      /* A debug output function.
       *  Prints the PRN id and the IODC for this record.
       */
   void RinexGloNavData::dump( ostream& s ) const
   {

      s << "Sat: " << sat
        << " TOE: "  << time
        << " TauN: " << TauN << " "
        << " GammaN: " << GammaN << " "
        << " MFtime: " << MFtime
        << " Health: " << fixed << setw(2) << health
        << " Freq. Number: " << fixed << setw(3) << freqNum
        << " Age of oper. information: "  << fixed << setw(5) << ageOfInfo
        << endl;

   }  // End of method 'RinexGloNavData::dump( ostream& s )'


      // Generates a line to be output to a file for the PRN/epoch line
   string RinexGloNavData::putPRNEpoch(void) const
      throw( StringException )
   {

      string line;
      CivilTime civtime(time);

      std::string tempString = asString(PRNID);
      if ( PRNID < 10 )
        {
          tempString = "0" + tempString;
        }
      line += rightJustify(tempString, 2);
      line += string(1, ' ');
      line += rightJustify(asString<short>((civtime.year%100)), 2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(civtime.month), 2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(civtime.day), 2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(civtime.hour), 2);
      line += string(1, ' ');
      line += rightJustify(asString<short>(civtime.minute), 2);

      line += rightJustify( asString( civtime.second, 1 ), 5 );

      double minusTauN( (-1.0)*TauN );
      line += doub2for(minusTauN, 19, 2);
      line += doub2for(GammaN, 19, 2);
      line += doub2for((double)MFtime, 19, 2);

       return line;

   }  // End of method 'RinexGloNavData::putPRNEpoch(void)'


      // Writes line 1 of the Nav Data record
   string RinexGloNavData::putBroadcastOrbit1(void) const
      throw( StringException )
   {

      string line;

      line += string(3, ' ');
      line += doub2for(px, 19, 2);
      line += doub2for(vx, 19, 2);
      line += doub2for(ax, 19, 2);
      line += doub2for((double)health, 19, 2);

      return line;

   }  // End of method 'RinexGloNavData::putBroadcastOrbit1(void)'


      // Writes line 2 of the Nav Data record
   string RinexGloNavData::putBroadcastOrbit2(void) const
      throw( StringException )
   {

      string line;

      line += string(3, ' ');
      line += doub2for(py, 19, 2);
      line += doub2for(vy, 19, 2);
      line += doub2for(ay, 19, 2);
      line += doub2for((double)freqNum, 19, 2);

      return line;

   }  // End of method 'RinexGloNavData::putBroadcastOrbit2(void)'


      // Writes line 3 of the Nav Data record
   string RinexGloNavData::putBroadcastOrbit3(void) const
      throw(StringException)
   {

      string line;

      line += string(3, ' ');
      line += doub2for(pz, 19, 2);
      line += doub2for(vz, 19, 2);
      line += doub2for(az, 19, 2);
      line += doub2for(ageOfInfo, 19, 2);

      return line;

   }  // End of method 'RinexGloNavData::putBroadcastOrbit3(void)'


      // Parses string \a currentLine to obtain PRN id and epoch.
   void RinexGloNavData::getPRNEpoch(const string& currentLine)
      throw( StringException, FFStreamError )
   {

      try
      {

            // Check for spaces in the right spots...
         for (int i = 2; i <= 17; i += 3)
            if (currentLine[i] != ' ')
               throw( FFStreamError("Badly formatted line") );

         PRNID = asInt(currentLine.substr(0,2));
         
         sat = SatID( PRNID, SatID::systemGlonass );

         short yr  = asInt(currentLine.substr( 2,3));
         short mo  = asInt(currentLine.substr( 5,3));
         short day = asInt(currentLine.substr( 8,3));
         short hr  = asInt(currentLine.substr(11,3));
         short min = asInt(currentLine.substr(14,3));
         double sec = asDouble(currentLine.substr(17,5));

            // years 80-99 represent 1980-1999
         const int rolloverYear = 80;
         if (yr < rolloverYear)
            yr += 100;
         yr += 1900;

            // Real RINEX 2 had epochs 'yy mm dd hr 59 60.0' surprisingly often.
            // Keep this in place (as Int) to be cautious.
         double ds=0;
         if(sec >= 60.) { ds=sec; sec=0.0; }
         CivilTime civtime( yr, mo, day, hr, min, sec );
         civtime.setTimeSystem(TimeSystem::GLO);
         time = civtime.convertToCommonTime();
         if(ds != 0) time += ds;

         TauN = StringUtils::for2doub(currentLine.substr(23,19));

            // The Rinex file provides -TauN
         TauN = (-1.0)*TauN;
         
         GammaN = StringUtils::for2doub(currentLine.substr(42,19));
         MFtime = (short)StringUtils::for2doub(currentLine.substr(61,19));

      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }

   }  // End of method 'RinexGloNavData::getPRNEpoch(const string& currentLine)'


      // Reads line 1 of the Nav Data record
   void RinexGloNavData::getBroadcastOrbit1(const string& currentLine)
      throw( StringException, FFStreamError )
   {

      try
      {
         px     =        StringUtils::for2doub(currentLine.substr( 3,19));
         vx     =        StringUtils::for2doub(currentLine.substr(22,19));
         ax     =        StringUtils::for2doub(currentLine.substr(41,19));
         health = (short)StringUtils::for2doub(currentLine.substr(60,19));
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }

   }  // End of method 'RinexGloNavData::getBroadcastOrbit1(...)'


      // Reads line 2 of the Nav Data record
   void RinexGloNavData::getBroadcastOrbit2(const string& currentLine)
      throw( StringException, FFStreamError )
   {

      try
      {
         py      =        StringUtils::for2doub(currentLine.substr( 3,19));
         vy      =        StringUtils::for2doub(currentLine.substr(22,19));
         ay      =        StringUtils::for2doub(currentLine.substr(41,19));
         freqNum = (short)StringUtils::for2doub(currentLine.substr(60,19));
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }

   }  // End of method 'RinexGloNavData::getBroadcastOrbit2(...)'


      // Reads line 3 of the Nav Data record
   void RinexGloNavData::getBroadcastOrbit3(const string& currentLine)
      throw( StringException, FFStreamError )
   {

      try
      {
         pz        = StringUtils::for2doub(currentLine.substr( 4,19));
         vz        = StringUtils::for2doub(currentLine.substr(23,19));
         az        = StringUtils::for2doub(currentLine.substr(42,19));
         ageOfInfo = StringUtils::for2doub(currentLine.substr(61,19));
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }

   }  // End of method 'RinexGloNavData::getBroadcastOrbit3(...)'

}  // End of namespace gpstk
