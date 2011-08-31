#pragma ident "$Id$"

/**
 * @file RinexGloNavHeader.cpp
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


#include "StringUtils.hpp"
#include "CivilTime.hpp"
#include "SystemTime.hpp"
#include "RinexGloNavHeader.hpp"
#include "RinexGloNavStream.hpp"

#include <iostream>

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{

      // Definition of header strings
   const string RinexGloNavHeader::endOfHeader = "END OF HEADER";
   const string RinexGloNavHeader::leapSecondsString = "LEAP SECONDS";
   const string RinexGloNavHeader::corrToSystemTime = "CORR TO SYSTEM TIME";
   const string RinexGloNavHeader::commentString = "COMMENT";
   const string RinexGloNavHeader::runByString = "PGM / RUN BY / DATE";
   const string RinexGloNavHeader::versionString = "RINEX VERSION / TYPE";


      // Writes a correctly formatted record from this data to stream ffs.
   void RinexGloNavHeader::reallyPutRecord(FFStream& ffs) const
      throw(std::exception, FFStreamError, StringException)
   {
      RinexGloNavStream& strm = dynamic_cast<RinexGloNavStream&>(ffs);
      
      strm.header = (*this);
      
      unsigned long allValid;
      if (version == 2.01)       allValid = allValid20;
      else if (version == 2.1)   allValid = allValid21;
      else if (version == 2.11)  allValid = allValid211;
      else
      {
         FFStreamError err(  "Unknown RINEX GLONASS version: "
                           + asString(version,3) );
         err.addText("Make sure to set the version correctly.");
         GPSTK_THROW(err);
      }
      
      if ((valid & allValid) != allValid)
      {
         FFStreamError err("Incomplete or invalid header.");
         err.addText("Make sure you set all header valid bits for all of the available data.");
         GPSTK_THROW(err);
      }
      
      string line;
      
      if (valid & versionValid)
      {
         line  = rightJustify(asString(version,2), 9);
         line += string(11, ' ');
         line += string("G"); //leftJustify(fileType, 20);
         line += string(39, ' ');
         line += versionString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & runByValid) 
      {
         line  = leftJustify(fileProgram,20);
         line += leftJustify(fileAgency,20);
         SystemTime sysTime;
         CivilTime dt(sysTime);
         string dat = dt.printf("%02d-%0b-%02y %02H:%02M");
         line += leftJustify(dat, 20);
         line += runByString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & commentValid)
      {
         vector<string>::const_iterator itr = commentList.begin();
         while (itr != commentList.end())
         {
            line  = leftJustify((*itr), 60);
            line += commentString;
            strm << line << endl;
            strm.lineNumber++;
            itr++;
         }
      }
      if (valid & corrToSystemTimeValid)
      {
         line  = rightJustify(asString(yearRefTime),6);
         line += rightJustify(asString(monthRefTime),6);
         line += rightJustify(asString(dayRefTime),6);
         line += string(3, ' ');
         line += doub2for(minusTauC, 19, 2);
         line += string(20,' ');
         line += corrToSystemTime;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & leapSecondsValid)
      {
         line  = rightJustify(asString(leapSeconds), 6);
         line += string(54, ' ');
         line += leapSecondsString;
         strm << line << endl;
         strm.lineNumber++;
      }
      if (valid & endValid)
      {
         line  = string(60,' ');
         line += endOfHeader;
         strm << line << endl;
         strm.lineNumber++;
      }
      
   }  // End of method 'RinexGloNavHeader::reallyPutRecord(FFStream& ffs)
   

      /*
       * This function reads the RINEX GLONASS NAV header from the given
       * FFStream. If an error is encountered in reading from the stream,
       * the stream is reset to its original position and fail-bit is set.
       *
       * @throws StringException when a StringUtils function fails
       * @throws FFStreamError when exceptions(failbit) is set and
       *  a read or formatting error occurs.  This also resets the
       *  stream to its pre-read position.
       */
   void RinexGloNavHeader::reallyGetRecord(FFStream& ffs)
      throw(std::exception, FFStreamError, StringException)
   {
      RinexGloNavStream& strm = dynamic_cast<RinexGloNavStream&>(ffs);
      
         // if already read, just return
      if (strm.headerRead == true)
         return;
      
      valid = 0;
      
         // clear out anything that was unsuccessfully read the first
      commentList.clear();
      
      while (! (valid & endValid))
      {
         string line;
         strm.formattedGetLine(line);
         StringUtils::stripTrailing(line);

         if (line.length()==0) continue;
         else if (line.length()<60 || line.length()>80)
         {
            FFStreamError e("Invalid line length");
            GPSTK_THROW(e);
         }
         
         string thisLabel(line, 60, 20);
         
         if (thisLabel == versionString)
         {
            version = asDouble(line.substr(0,20));
            fileType = strip(line.substr(20,20));
            if ( (fileType[0] != 'G') &&
                 (fileType[0] != 'g'))
            {
               FFStreamError e("This isn't a Rinex GLONASS Nav file");
               GPSTK_THROW(e);
            }
            valid |= versionValid;
         }
         else if (thisLabel == runByString)
         {
            fileProgram = strip(line.substr(0,20));
            fileAgency = strip(line.substr(20,20));
            date = strip(line.substr(40,20));
            valid |= runByValid;
         }
         else if (thisLabel == commentString)
         {
            commentList.push_back(strip(line.substr(0,60)));
            valid |= commentValid;
         }
         else if (thisLabel == corrToSystemTime)
         {
            yearRefTime  = asInt(line.substr(0,6));
            monthRefTime = asInt(line.substr(6,6));
            dayRefTime   = asInt(line.substr(12,6));
            minusTauC = gpstk::StringUtils::for2doub(line.substr(21,19));
            valid |= corrToSystemTimeValid;
         }
         else if (thisLabel == leapSecondsString)
         {
            leapSeconds = asInt(line.substr(0,6));
            valid |= leapSecondsValid;
         }
         else if (thisLabel == endOfHeader)
         {
            valid |= endValid;
         }
         else
         {
            throw(FFStreamError("Unknown header label at line " + 
                                asString<size_t>(strm.lineNumber)));
         }
      }
      
      unsigned long allValid;
      if      (version == 2.01)     allValid = allValid20;
      else if (version == 2.1)      allValid = allValid21;
      else if (version == 2.11)     allValid = allValid211;
      else
      {
         FFStreamError e( "Unknown or unsupported RINEX GLONASS sversion "
                         + asString(version) );
         GPSTK_THROW(e);
      }
      
      if ( (allValid & valid) != allValid)
      {
         FFStreamError e("Incomplete or invalid header");
         GPSTK_THROW(e);               
      }            
      
         // we got here, so something must be right...
      strm.header = *this;
      strm.headerRead = true;

   }  // End of method 'RinexGloNavHeader::reallyGetRecord(FFStream& ffs)'


      // This function dumps the contents of the header.
   void RinexGloNavHeader::dump(ostream& s) const
   {

      int i;
       s << "---------------------------------- REQUIRED ----------------------------------\n";
      s << "Rinex Version " << fixed << setw(5) << setprecision(2) << version
         << ",  File type " << fileType << ".\n";
      s << "Prgm: " << fileProgram << ",  Run: " << date << ",  By: " << fileAgency << endl;

      s << "(This header is ";
      if((valid & allValid211) == allValid211) s << "VALID 2.11";
      else if((valid & allValid21) == allValid21) s << "VALID 2.1";
      else if((valid & allValid20) == allValid20) s << "VALID 2.0";
      else s << "NOT VALID";
      s << " Rinex.)\n";

      if(!(valid & versionValid)) s << " Version is NOT valid\n";
      if(!(valid & runByValid)) s << " Run by is NOT valid\n";
      if(!(valid & endValid)) s << " End is NOT valid\n";

      s << "---------------------------------- OPTIONAL ----------------------------------\n";
      if(valid & corrToSystemTimeValid) s << "Correction to System Time: Year="
         << yearRefTime << ", Month=" << monthRefTime << ", Day=" << dayRefTime
         << ", -TauC=" << scientific << setprecision(12) << minusTauC << "\n";
      else s << " Correction to System Time is NOT valid\n";
      if(valid & leapSecondsValid) s << "Leap seconds: " << leapSeconds << endl;
      else s << " Leap seconds is NOT valid\n";
      if(commentList.size() > 0) {
         s << "Comments (" << commentList.size() << ") :\n";
         for(int i=0; i<commentList.size(); i++)
            s << commentList[i] << endl;
      }
      s << "-------------------------------- END OF HEADER -------------------------------\n";

   }  // End of method 'RinexGloNavHeader::dump(ostream& s)'


}  // End of namespace gpstk
