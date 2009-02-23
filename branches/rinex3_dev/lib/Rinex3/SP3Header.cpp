#pragma ident "$Id$"

/**
 * @file SP3Header.cpp
 * Encapsulate header of SP3 file data, including I/O
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

#include "StringUtils.hpp"
#include "SP3Stream.hpp"
#include "SP3Header.hpp"
#include "CivilTime.hpp"
#include "MJD.hpp"
#include "GPSWeekSecond.hpp"

using namespace gpstk;

namespace Rinex3
{
   using namespace StringUtils;
   using namespace std;

   void SP3Header::reallyPutRecord(FFStream& ffs) const
      throw(exception, FFStreamError, StringException)
   {
      SP3Stream& strm = dynamic_cast<SP3Stream&>(ffs);
      int i,j,k;
      string line;
      SatID SVid;

      if(version != 'a' && version != 'c') {
         Exception e(string("SP3 version must be 'a' or 'c' : ") + version);
         GPSTK_THROW(e);
      }
      if(pvFlag != 'P' && pvFlag != 'V') {
         Exception e(string("SP3 pv flag must be 'P' or 'V' : ") + pvFlag);
         GPSTK_THROW(e);
      }

      // line 1
      CivilTime civTime(time);
      line = "#";
      line += version;
      line += pvFlag;
      line += civTime.printf("%4Y %2m %2d %2H %2M");
      line += " " + rightJustify(civTime.printf("%f"),11);
      line += " " + rightJustify(asString(numberOfEpochs),7);
      line += " " + rightJustify(dataUsed,5);
      line += " " + rightJustify(coordSystem,5);
      line += " " + rightJustify(orbitType,3);
      line += " " + rightJustify(agency,4);
      strm << line << endl;

      // line 2
      GPSWeekSecond gpsWS(time);
      line = "##";
      line += rightJustify(gpsWS.printf("%F"),5);
      line += rightJustify(gpsWS.printf("%g"),16);
      line += " " + rightJustify(asString(epochInterval,8),14);
      line += " " + (static_cast<MJD>(time)).printf("%5.0Q");
      line += " " + rightJustify(asString(time.getSecondOfDay()/86400.,13),15);
      strm << line << endl;

      // lines 3-7 and 8-12
      //Map<SV,accuracy flag> (all SVs in data)
      std::map<SatID, short>::const_iterator it;
      for(i=3; i<=12; i++) {                 // loop over the lines
         if(i==3) line = "+   " + rightJustify(asString(satList.size()),2) + "   ";
         else if(i < 8) line = "+        ";
         else           line = "++       ";
         k = 0;
         if(i == 3 || i == 8)                // start the iteration
            it = satList.begin();
         while(k < 17) {                     // there are 17 per line
            if(it != satList.end()) {
               if(i < 8) {                   // lines 3-7 - sat id
                  if(version == 'c') {
                     // a satellite in version c -> let j be -1 to mark it
                     SVid = it->first;
                     j = -1;
                  }
                  else j = it->first.id;
               }
               else                          // lines 8-12 - accuracy
                  j = it->second;
               it++;
            }
            else j=0;            // no more

            if(j == -1)          // sat version c
               try {
                  line += rightJustify(SP3SatID(SVid).toString(),3);
               }
               catch (Exception& e)
               {
                  FFStreamError ffse(e);
                  GPSTK_THROW(ffse);
               }
            else                 // sat version a, accuracy, or 0
               line += rightJustify(asString(j),3);
            k++;
         }
         strm << line << endl;
      }

      // line 13
      string ft(" cc");
      if (version == 'c')
      {
         ft[1] = system.systemChar(); ft[2] = ' '; }
      strm << "%c" << ft << " cc"
           << " " << (version == 'c' ? timeSystemString() : "ccc")
           << " ccc cccc cccc cccc cccc ccccc ccccc ccccc ccccc" << endl;
      // line 14
      strm << "%c cc cc ccc ccc cccc cccc cccc cccc ccccc ccccc ccccc ccccc" << endl;
      // line 15
      strm << "%f "
           << (version == 'c' ? rightJustify(asString(basePV,7),10) : " 0.0000000")
           << " "
           << (version == 'c' ? rightJustify(asString(baseClk,9),12) : " 0.000000000")
           << "  0.00000000000  0.000000000000000" << endl;
      // lines 16-18
      strm << "%f  0.0000000  0.000000000  0.00000000000  0.000000000000000" << endl;
      strm << "%i    0    0    0    0      0      0      0      0         0" << endl;
      strm << "%i    0    0    0    0      0      0      0      0         0" << endl;

      // lines 19-22
      //std::vector<std::string> comments; ///< vector of 4 comment lines
      for(j=0,i=19; i<=22; i++) {
         line = "/* ";
         if(j < comments.size()) line += leftJustify(comments[j++],57);
         else line += string(57,'C');
         strm << line << endl;
      }
   }

   void SP3Header::dump(ostream& s) const
   {
      s << "SP3 Header: version '" << version << "' containing ";
      if(pvFlag=='P') s << "positions.";
      if(pvFlag=='V') s << "positions and velocities.";
      s << endl;
      s << " Time tag in header is " << time << endl;
      s << " Timespacing is " << epochInterval
         << " sec, and the number of epochs is " << numberOfEpochs << endl;
      s << " Data used as input : " << dataUsed << endl;
      s << " Coordinate system : " << coordSystem << endl;
      s << " Orbit estimate type : " << orbitType << endl;
      s << " Agency : " << agency << endl;
      if(version == 'c') {
         s << " File type: '" << system.systemChar() << "' which is "
           << system.systemString() << endl;
         s << " Time System: " << timeSystemString() << endl;
         s << " Base for Pos/Vel =" << fixed << setw(10) << setprecision(7)
           << basePV << endl;
         s << " Base for Clk/Rate =" << setw(12) << setprecision(9)
           << baseClk << endl;
      }
      
      s << " List of satellite PRN/acc (" << satList.size() << " total) :\n";
      int i=0;
      std::map<SatID,short>::const_iterator it=satList.begin();
      while(it != satList.end()) {
         s << " " << it->first << "/" << it->second;
         if(!(++i % 8)) s << endl;
         it++;
      }
      if(++i % 8) s << endl;

      s << " Comments:\n";
      for(size_t j=0; j<comments.size(); j++) s << "    " << comments[j] << endl;

      s << "End of SP3 header" << endl;

   }  // end SP3Header::reallyPutRecord()

   void SP3Header::reallyGetRecord(FFStream& ffs)
      throw(exception, FFStreamError, StringException)
   {
      SP3Stream& strm = dynamic_cast<SP3Stream&>(ffs);
      
      string line;
      strm.formattedGetLine(line);

      if (line[0]=='#' && line[1]!='#')
      {
         version=line[1];
         pvFlag=line[2];
         int year = asInt(line.substr(3,4));
         int month = asInt(line.substr(8,2));
         int dom = asInt(line.substr(11,2));
         int hour = asInt(line.substr(14,2));
         int minute = asInt(line.substr(17,2));
         double second = asInt(line.substr(20,10));
         try {
         time = CivilTime(year, month, dom, hour, minute, second);
         }
         catch (gpstk::Exception& e) {
            FFStreamError fe("Invalid time:" + string(1, line[0]));
            GPSTK_THROW(fe);
         }
         numberOfEpochs = asInt(line.substr(32,7));
         dataUsed = line.substr(40,5);
         coordSystem = line.substr(46,5);
         orbitType = line.substr(52,3);
         agency = line.substr(56,4);
      }
      else
      {
         FFStreamError e("Unknown label " + string(1, line[0]));
         GPSTK_THROW(e);
      }
      
      strm.formattedGetLine(line);
      if (line[0]=='#' && line[1]=='#')
      {
         epochInterval = asDouble(line.substr(24,14));
      }
      else
      {
         FFStreamError e("Unknown label " + string(1, line[0]));
         GPSTK_THROW(e);
      }

      int i, index;
      int numSVs(0), readSVs(0);

         // the map stores them sorted, so use svsAsWritten to determine
         // which SV each accuracy corresponds to.
      vector<SatID> svsAsWritten;
      SatID sat;

            // read in the SV list
      for(i = 3; i <= 7; i++)
      {
         strm.formattedGetLine(line);
         if (line[0]=='+')
         {
               // get the total number of svs on line 3
            if (i == 3)
            {
               numSVs = asInt(line.substr(4,2));
               svsAsWritten.resize(numSVs);
            }
            for(index = 9; index < 60; index += 3)
            {
               if (readSVs < numSVs)
               {
                  try { 
                     sat = SP3SatID(line.substr(index,3));
                  }
                  catch (Exception& e) {
                     FFStreamError ffse(e);
                     GPSTK_THROW(ffse);
                  }
                  svsAsWritten[readSVs] = sat;
                  satList[sat] = 0;
                  readSVs++;
               }
            }
         }
         else
         {
            FFStreamError e("Unknown label " + string(1, line[0]));
            GPSTK_THROW(e);
         }
      }
      
      readSVs = 0;

         // read in the accuracy.
      for(i = 8; i <= 12; i++)
      {
         strm.formattedGetLine(line);
         if ((line[0]=='+') && (line[1]=='+'))
         {
            for(index = 9; index < 60; index += 3)
            {
               if (readSVs < numSVs)
               {
                  satList[svsAsWritten[readSVs]] = asInt(line.substr(index,3));
                  readSVs++;
               }
            }
         }
         else
         {
            FFStreamError e("Unknown label " + string(1, line[0]));
            GPSTK_THROW(e);
         }
      }

         // read in 6 unused lines
      for(i = 13; i <= 18; i++)
         strm.formattedGetLine(line);
      
         // read in 4 comment lines
      for(i = 19; i <= 22; i++)
      {
         strm.formattedGetLine(line);
            // strip the first 3 characters
         line.erase(0, 3);
            // and add to the comment vector
         comments.push_back(line);
      }
   }  // end SP3Header::reallyGetRecord()

}  // namespace
