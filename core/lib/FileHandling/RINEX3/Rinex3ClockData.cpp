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

      s << " " << datatype;
      if(datatype == "AR") s << " " << site;
      else s << " " << sat.toString() << " ";
      s << " " << writeTime(time);
      s << scientific << setprecision(12)
          << " " << setw(19) << bias;
      if (sig_bias != 0.0) s << " " << setw(19) << sig_bias; else s << " 0.0";
      if (drift != 0.0) s << " " << setw(19) << sig_bias; else s << " 0.0";
      if (sig_drift != 0.0) s << " " << setw(19) << sig_bias; else s << " 0.0";
      if (accel != 0.0) s << " " << setw(19) << sig_bias; else s << " 0.0";
      if (sig_accel != 0.0) s << " " << setw(19) << sig_bias; else s << " 0.0";
      s << endl; // is this the only line ending?

      return;

   }  // End of method 'Rinex3ClockData::dump(std::ostream& s)'


  void Rinex3ClockData::reallyPutRecord(FFStream& ffs) const 
         throw(std::exception, FFStreamError,
               StringUtils::StringException)
   {
    convertTypes(); // updates all backwards compatibility members
    
    Rinex3ClockStream& strm = dynamic_cast<Rinex3ClockStream&>(ffs);

    string line;

    line = datatype;
    line += string(1,' ');

    if(datatype == "AR") line += rightJustify(site,4);
    else if(datatype == "AS") line += rightJustify(sat.toString(),4);
    else
    {
         FFStreamError e("Unknown data type: " + datatype);
         GPSTK_THROW(e);
    }

    line += string(1,' ');

    line += writeTime(time);

    // must count the data to output
    int n = 2;
    if(drift != 0.0) n=3;
    if(sig_drift != 0.0) n=4;
    if(accel != 0.0) n=5;
    if(sig_accel != 0.0) n=6;
    line += rightJustify(asString(n),3);
    line += string(3,' ');

    line += doubleToScientific(bias, 19, 12, 2);
    line += string(1,' ');
    line += doubleToScientific(sig_bias, 19, 12, 2);

    strm << line << endl;
    strm.lineNumber++;

    // continuation line
    if(n > 2) {
       line = doubleToScientific(drift, 19, 12, 2);
       line += string(1,' ');
       if(n > 3) {
          line += doubleToScientific(sig_drift, 19, 12, 2);
          line += string(1,' ');
       }
       if(n > 4) {
          line += doubleToScientific(accel, 19, 12, 2);
          line += string(1,' ');
       }
       if(n > 5) {
          line += doubleToScientific(sig_accel, 19, 12, 2);
          line += string(1,' ');
       }
       strm << line << endl;
       strm.lineNumber++;
    }


      return;

   }  // End of method 'Rinex3ClockData::reallyPutRecord()'



      // This function parses the entire header from the given stream
   void Rinex3ClockData::reallyGetRecord(FFStream& ffs)
      throw(std::exception, FFStreamError, 
            StringUtils::StringException)
   {
      Rinex3ClockStream& strm = dynamic_cast<Rinex3ClockStream&>(ffs);

      clear();

      string line;

      strm.formattedGetLine(line, true);
      stripTrailing(line);
      if(line.length() < 59) {
          FFStreamError e("Short line : " + line);
          GPSTK_THROW(e);
      }//expected line length is 79

      datatype = line.substr(0,2);
      site = line.substr(3,4);
      if(datatype == "AS") {
          strip(site);
          sat.fromString(site);
      }

      time = CivilTime(asInt(line.substr( 8,4)),
               asInt(line.substr(12,3)),
               asInt(line.substr(15,3)),
               asInt(line.substr(18,3)),
               asInt(line.substr(21,3)),
               asDouble(line.substr(24,10)),
               TimeSystem::Any);
      
      int n(asInt(line.substr(34,3)));            //Number of data values in this line
      bias = asDouble(line.substr(40,19));
      if(n > 1 && line.length() >= 59) sig_bias = asDouble(line.substr(60,19));

      if(n > 2) {
         strm.formattedGetLine(line,true);
         stripTrailing(line);
         if(int(line.length()) < (n-2)*20-1) {
            FFStreamError e("Short line : " + line);
            GPSTK_THROW(e);
         }
         drift =     asDouble(line.substr( 0,19));
         if(n > 3) sig_drift = asDouble(line.substr(20,19));
         if(n > 4) accel     = asDouble(line.substr(40,19));
         if(n > 5) sig_accel = asDouble(line.substr(60,19));
      }

      convertTypes(); //sets all the backwards compatibilty values

      return;

   }  // End of method 'Rinex3ClockData::reallyGetRecord(FFStream& ffs)'

   void Rinex3ClockData::convertTypes(void) throw()
   {
    //Only used to relate name in v.3 to site and sat in v.2
 
      if(name.empty() && site.empty() && sat.id != -1) // sat set
       {
          name = sat.toString();
          site = name = tempName = tempSite;
       }
      else if(name.empty() && !site.empty() && (sat.id == -1)) //site set
         name = tempName = site;
      else if(!name.empty() && site.empty() && (sat.id == -1)) //name set
      {
         try{sat.fromString(name); tempSat = sat;}
         catch(gpstk::Exception e) {site = tempSite = name;}
      }
 
      //sat modified
      if(sat != tempSat)
      {
       tempSat = sat;
       name = tempName = sat.toString();
      }
 
      //site modified
      if(site != tempSite)
      {
       name = tempName = tempSite = site;
       sat = RinexSatID(-1,RinexSatID::systemGPS);
      }
 
      //name modified
      if(name != tempName)
      {
         try{sat.fromString(name); tempSat = sat;}
         catch(gpstk::Exception e) {site = tempSite = name;}
         tempName = name;
      }

      tempName = name; tempSite = site; tempSat = sat;

      return;
     }  // End of method 'Rinex3ClockData::convertTypes'



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
