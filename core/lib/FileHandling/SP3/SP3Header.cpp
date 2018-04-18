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
 * @file SP3Header.cpp
 * Encapsulate header of SP3 file data, including I/O
 */

#include "StringUtils.hpp"
#include "SP3Stream.hpp"
#include "SP3Header.hpp"
#include "CivilTime.hpp"
#include "MJD.hpp"
#include "GPSWeekSecond.hpp"

#define debug false

namespace gpstk
{
   using namespace StringUtils;
   using namespace std;

   void SP3Header::reallyGetRecord(FFStream& ffs)
      throw(exception, FFStreamError, StringException)
   {
      SP3Stream& strm = dynamic_cast<SP3Stream&>(ffs);

      string line;
      strm.formattedGetLine(line);
      if(debug) std::cout << "SP3 Header Line 1 " << line << std::endl;

      if (line[0]=='#' && line[1]!='#')                  // line 1
      {
         // version character
         if(line[1] == 'a')      version = SP3a;
         else if(line[1] == 'b') version = SP3b;
         else if(line[1] == 'c') version = SP3c;
         else if(line[1] == 'd') version = SP3d;
         else {
            FFStreamError e("Unknown version of SP3: " + line.substr(0,3));
            GPSTK_THROW(e);
         }

         // are V records present?
         if(line[2] == 'V') containsVelocity = true;
         else               containsVelocity = false;

         // parse the rest of the line
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
         FFStreamError e("Unknown label in line 1: " + line.substr(0,2));
         GPSTK_THROW(e);
      }

      strm.formattedGetLine(line);
      if(debug) std::cout << "SP3 Header Line 2 " << line << std::endl;
      if (line[0]=='#' && line[1]=='#')                           // line 2
      {
         epochInterval = asDouble(line.substr(24,14));
      }
      else
      {
         FFStreamError e("Unknown label in line 2: " + line.substr(0,2));
         GPSTK_THROW(e);
      }

      int i, index;
      int numSVs(0), readSVs(0);

         // the map stores them sorted, so use svsAsWritten to determine
         // which SV each accuracy corresponds to.
      vector<SP3SatID> svsAsWritten;
      SP3SatID sat;

          //Count lines for use in SP3d
      int lineCount = 2;
      int svLineCount = 0;
      bool done = false;

          // read in the SV list
      while(!done)
      {
         strm.formattedGetLine(line);
         lineCount++;
         if(debug) std::cout << "SP3 Header Line " << lineCount << " " << line << std::endl;
         if (line[0]=='+' && line[1]=='+')
         {
             done=true;
             strm.lastLine=line;
         }
         else
         {
             if (line[0]=='+')
             {
                svLineCount++;
                   // get the total number of svs on line 3
                if (svLineCount == 1)
                {
                   numSVs = asInt(line.substr(3,3));
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
                FFStreamError e("Unknown 1st char in line " + asString(i) + ": "
                   + string(1, line[0]));
                GPSTK_THROW(e);
             }
         }
      }

      readSVs = 0;

         // read in the accuracy.
      for (i = 0; i < svLineCount; i++)                                    // lines 8-12
      {
         if (i==0)
         {
            line=strm.lastLine;
         }
         else
         {
            strm.formattedGetLine(line);
            lineCount++;
         }

         if(debug) std::cout << "SP3 Header Line " << lineCount << " " << line << std::endl;
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
            FFStreamError e("Unknown label in line " + asString(i) + ": "
               + line.substr(0,2));
            GPSTK_THROW(e);
         }
      }

      strm.formattedGetLine(line);
      lineCount++;
      if(debug) std::cout << "SP3 Header Line %c1 " << line << std::endl;
      if (version == SP3b || version == SP3c || version == SP3d) {
         if(line[0]=='%' && line[1]=='c')
         {
            // file system
            system.fromString(line.substr(3,2));

            // time system
            string ts = upperCase(line.substr(9,3));
            timeSystem.fromString(ts);
         }
         else
         {
            FFStreamError e("Unknown label in line %c1: " + line.substr(0,2));
            GPSTK_THROW(e);
         }
      }

      strm.formattedGetLine(line);
      lineCount++;
      if(debug) std::cout << "SP3 Header Line %c2 " << line << std::endl;

      strm.formattedGetLine(line);
      lineCount++;
      if(debug) std::cout << "SP3 Header Line \%f1 " << line << std::endl;
      if (version == SP3c || version == SP3d) {
         if (line[0]=='%' && line[1]=='f')
         {
            basePV = asDouble(line.substr(3,10));
            baseClk = asDouble(line.substr(14,12));
         }
         else
         {
            FFStreamError e("Unknown label in line \%f1: " + line.substr(0,2));
            GPSTK_THROW(e);
         }
      }

      strm.formattedGetLine(line);
      lineCount++;
      if(debug) std::cout << "SP3 Header Line \%f2 " << line << std::endl;

         // read in 2 unused %i lines                             // lines 17,18
      for(i = 0; i <= 1; i++) {
         strm.formattedGetLine(line);
         lineCount++;
         if(debug) std::cout << "SP3 Header Line \%i " << line << std::endl;
      }

         // read in comment lines
      comments.clear();
      done=false;
      while(!done)                                   // lines 19-22
      {
         strm.formattedGetLine(line);
         lineCount++;
         if(line[0]=='/' && line[1]=='*')
         {
             if(debug) std::cout << "SP3 Header Line " << i << " " << line << std::endl;
                // strip the first 3 characters
             line.erase(0, 3);
                // and add to the comment vector
             comments.push_back(line);
         }
         else
         {
             done=true;
             strm.lastLine=line;
         }
      }

      // save the header, for use later when reading SP3Data records
      strm.header = *this;

   }  // end SP3Header::reallyGetRecord()


   void SP3Header::reallyPutRecord(FFStream& ffs) const
      throw(exception, FFStreamError, StringException)
   {
   try {
      SP3Stream& strm = dynamic_cast<SP3Stream&>(ffs);
      int i,k;
      long j;
      string line;
      SP3SatID SVid;
      bool isVerA  = (version == SP3a);
      bool isVerB  = (version == SP3b);
      bool isVerC  = (version == SP3c);
      bool isVerD  = (version == SP3d);

      // line 1
      CivilTime civTime(time);
      line = "#";
      line += versionChar();
      line += (containsVelocity ? "V" : "P");
      line += civTime.printf("%4Y %2m %2d %2H %2M");
      line += " " + rightJustify(civTime.printf("%.8f"),11);
      line += " " + rightJustify(asString(numberOfEpochs),7);
      line += " " + rightJustify(dataUsed,5);
      line += " " + rightJustify(coordSystem,5);
      line += " " + rightJustify(orbitType,3);
      line += " " + rightJustify(agency,4);
      strm << line << endl;
      strm.lineNumber++;

      // line 2
      GPSWeekSecond gpsWS(time);
      line = "##";
      line += rightJustify(gpsWS.printf("%F"),5);
      line += rightJustify(gpsWS.printf("%15.8g"),16);
      line += " " + rightJustify(asString(epochInterval,8),14);
      line += " " + (static_cast<MJD>(time)).printf("%5.0Q");
      line += " " + rightJustify(asString(time.getSecondOfDay()/86400.,13),15);
      strm << line << endl;
      strm.lineNumber++;

      //SV lines and SV Accuracy lines
      //Map<SV,accuracy flag> (all SVs in data)
      std::map<SP3SatID, short>::const_iterator it;
      if (isVerD)
      {
          int totalLines = satList.size()/17;
          if (satList.size()%17 != 0) totalLines++;
          int numberOfLines = 2*totalLines;
          if (numberOfLines < 10){
              numberOfLines = 10;
          }
          int linesInSec = numberOfLines/2;
          for(i=1;i<=numberOfLines;i++){
              if (i==1) line = "+  " + rightJustify(asString(satList.size()),3) + "   ";
              else if ( i < linesInSec+1) line = "+        ";
              else                        line = "++       ";
              k=0;
              if(i==1 || i==linesInSec+1)
              {
                it = satList.begin();
              }
              while(k<17)
              {
                if (it != satList.end())
                {
                    if (i < linesInSec+1)
                    {
                        SVid = it->first;
                        j=-1;
                    }
                    else
                    {
                        j = it->second;
                    }
                    it++;
                }
                else
                {
                    j=0;
                }
                if(j==-1)
                {
                    try{
                        line += rightJustify(SP3SatID(SVid).toString(),3);
                    }
                    catch(Exception& e)
                    {
                        FFStreamError ffse(e);
                        GPSTK_THROW(ffse);
                    }
                }
                else
                {
                    line += rightJustify(asString(j),3);
                }
                k++;
              }
              strm << line << endl;
              strm.lineNumber++;
          }
      }
      else
      {
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
                      if(!isVerA) {
                         // a satellite in version b or c -> let j be -1 to mark it
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
    
                if(j == -1)          // sat version b or c
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
             strm.lineNumber++;
          }
      }

      // line 13
      string ft("cc");
      if(!isVerA) {
         ft[0] = system.systemChar();
         ft[1] = ' ';
      }
      if(isVerB) {
         TimeSystem::Systems tsys = timeSystem.getTimeSystem();
         if(tsys != TimeSystem::GPS && tsys != TimeSystem::UTC) {
            FFStreamError ffse("Time system must be GPS or UTC");
            GPSTK_THROW(ffse);
         }
      }
      if(isVerC || isVerD) {
         TimeSystem::Systems tsys = timeSystem.getTimeSystem();
         if(   tsys != TimeSystem::GPS && tsys != TimeSystem::GLO
            && tsys != TimeSystem::GAL && tsys != TimeSystem::TAI
            && tsys != TimeSystem::UTC && tsys != TimeSystem::QZS ) {
            FFStreamError ffse("Time system must be GPS, GLO, GAL, TAI, UTC, or QZS");
            GPSTK_THROW(ffse);
         }
      }
      strm << "%c " << ft << " cc"
           << " " << (isVerA ? "ccc" : timeSystemString())
           << " ccc cccc cccc cccc cccc ccccc ccccc ccccc ccccc" << endl;
      strm.lineNumber++;

      // line 14
      strm << "%c cc cc ccc ccc cccc cccc cccc cccc ccccc ccccc ccccc ccccc" << endl;

      // line 15
      strm << "%f "
           << ((isVerC || isVerD) ? rightJustify(asString(basePV,7),10) : " 0.0000000")
           << " "
           << ((isVerC || isVerD) ? rightJustify(asString(baseClk,9),12) : " 0.000000000")
           << "  0.00000000000  0.000000000000000" << endl;
      strm.lineNumber++;

      // lines 16-18
      strm << "%f  0.0000000  0.000000000  0.00000000000  0.000000000000000" << endl;
      strm.lineNumber++;
      strm << "%i    0    0    0    0      0      0      0      0         0" << endl;
      strm.lineNumber++;
      strm << "%i    0    0    0    0      0      0      0      0         0" << endl;
      strm.lineNumber++;

      // Comment lines
      //std::vector<std::string> comments; ///< vector of 4 comment lines
      bool done=false;
      j=0;
      int linesOfComments = 4;
      if ((int)comments.size() > linesOfComments){
          linesOfComments = (int)comments.size();
      }
      while (!done) {
         line = "/* ";
         if(isVerD){
            if(j < (int)comments.size()) line += leftJustify(comments[j],77);
            else line += string(77,'C');
         }
         else{
            if(j < (int)comments.size()) line += leftJustify(comments[j],57);
            else line += string(57,'C');
         }
         j++;
         if(j==linesOfComments){
             done=true;
         }
         strm << line << endl;
         strm.lineNumber++;
      }

      // save header for use with SP3Data::reallyPut
      strm.header = *this;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(std::exception& e) { Exception g(e.what()); GPSTK_THROW(g); }
   }


   void SP3Header::dump(ostream& s) const throw()
   {
      s << "SP3 Header: version " << versionString() << " containing ";
      if(containsVelocity) s << "positions and velocities.";
      else s << "positions only.";
      CivilTime ct(time);
      s << endl;
      s << " Time tag : " << ct.printf("%4Y/%02m/%02d %2H:%02M:%02S") << endl;
      s << " Timespacing is " << epochInterval
         << " sec, and the number of epochs is " << numberOfEpochs << endl;
      s << " Data used as input : " << dataUsed << endl;
      s << " Coordinate system : " << coordSystem << endl;
      s << " Orbit estimate type : " << orbitType << endl;
      s << " Agency : " << agency << endl;
      if(version == SP3c) {
         s << " File type: '" << system.systemChar() << "' which is "
           << system.systemString() << endl;
         s << " Time System: " << timeSystemString() << endl;
         s << " Base for Pos/Vel =" << fixed << setw(10) << setprecision(7)
           << basePV << endl;
         s << " Base for Clk/Rate =" << setw(12) << setprecision(9)
           << baseClk << endl;
      }

      s << " List of satellite PRN/accuracy (" << satList.size() << " total) :\n";
      int i=0;
      std::map<SP3SatID,short>::const_iterator it=satList.begin();
      while(it != satList.end()) {
         s << " " << it->first << "/" << it->second;
         if(!(++i % 8)) s << endl;
         it++;
      }
      if(i % 8) s << endl;

      s << " Comments:\n";
      for(size_t j=0; j<comments.size(); j++) s << "    " << comments[j] << endl;

      s << "End of SP3 header" << endl;

   }  // end SP3Header::dump()

}  // namespace
