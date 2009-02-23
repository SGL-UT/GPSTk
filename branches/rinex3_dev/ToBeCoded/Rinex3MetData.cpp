#pragma ident "$Id$"



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






/**
 * @file Rinex3MetData.cpp
 * Encapsulate RINEX3 meteorological file data, including I/O
 */

#include "StringUtils.hpp"
#include "CommonTime.hpp"
#include "Rinex3MetHeader.hpp"
#include "Rinex3MetData.hpp"
#include "Rinex3MetStream.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
   const int Rinex3MetData::maxObsPerLine = 8;
   const int Rinex3MetData::maxObsPerContinuationLine = 10;

   void Rinex3MetData::reallyPutRecord(FFStream& ffs) const
      throw(std::exception, FFStreamError, 
            gpstk::StringUtils::StringException)
   {
      const int maxObsPerOutputLine = 8;
      const int maxObsPerOutputContLine = 10;

      Rinex3MetStream& strm = dynamic_cast<Rinex3MetStream&>(ffs);
      string line;
      
         // write the first line
      line += " ";
      line += rightJustify(asString<short>(time.year()),2,'0');
      line += " ";
      line += rightJustify(asString<short>(time.month()),2);
      line += " ";
      line += rightJustify(asString<short>(time.day()),2);
      line += " ";
      line += rightJustify(asString<short>(time.hour()),2);
      line += " ";
      line += rightJustify(asString<short>(time.minute()),2);
      line += " ";
      line += rightJustify(asString<short>(short(time.second())),2);
      
      for (int i = 0; 
           (i < strm.header.obsTypeList.size()) &&
              (i < maxObsPerOutputLine);
           i++)
      {
         Rinex3MetHeader::Rinex3MetType thistype = strm.header.obsTypeList[i];
         Rinex3MetMap::const_iterator itr = data.find(thistype);
         if (itr == data.end())
         {
            FFStreamError err("Couldn't find data for " + 
                              Rinex3MetHeader::convertObsType(strm.header.obsTypeList[i]));
            GPSTK_THROW(err);
         }
         line += rightJustify(asString((*itr).second,1),7);
      }
      
         // do we need continuation lines?
      if (strm.header.obsTypeList.size() > maxObsPerOutputLine)
      {
         for (int i = maxObsPerOutputLine;
              i < strm.header.obsTypeList.size();
              i++)
         {
            
            if (((i - maxObsPerOutputLine) % maxObsPerOutputContLine) == 0)
            {
               ffs << line << endl;
               strm.lineNumber++;
               line.clear();
               line += string(4,' ');
            }
            Rinex3MetHeader::Rinex3MetType thistype = strm.header.obsTypeList[i];
            Rinex3MetMap::const_iterator itr = data.find(thistype);
            if (itr == data.end())
            {
               FFStreamError err("Couldn't find data for " + 
                  Rinex3MetHeader::convertObsType(strm.header.obsTypeList[i]));
               GPSTK_THROW(err);
            }
            line += rightJustify(asString((*itr).second,1),7);
         }
      }
      
      ffs << line << endl;
      strm.lineNumber++;
   }

   void Rinex3MetData::reallyGetRecord(FFStream& ffs) 
      throw(std::exception, FFStreamError, 
            gpstk::StringUtils::StringException)
   {
      Rinex3MetStream& strm = dynamic_cast<Rinex3MetStream&>(ffs);
      
      if(!strm.headerRead)
         strm >> strm.header;
      
      Rinex3MetHeader& hdr = strm.header;
      
      string line;
      data.clear();
      
         // this is to see whether or not we expect an EOF
         // when we read this next line
      if (hdr.obsTypeList.size() > maxObsPerLine)
         strm.formattedGetLine(line); 
      else
         strm.formattedGetLine(line, true); 

      processFirstLine(line, hdr);
      
      time = parseTime(line);
      
      while (data.size() < hdr.obsTypeList.size())
      {
         if (hdr.obsTypeList.size() - data.size() < maxObsPerContinuationLine)
            strm.formattedGetLine(line, true); 
         else
            strm.formattedGetLine(line);
	 processContinuationLine(line, hdr);
      }
      
      if (data.size() != hdr.obsTypeList.size())
      {
         FFStreamError e("Incorrect number of records");
         GPSTK_THROW(e);
      }
   } 

   void Rinex3MetData::processFirstLine(const string& line,
                                       const Rinex3MetHeader& hdr)
      throw(FFStreamError)
   {
      try
      {
          for (int i = 0; 
              (i < maxObsPerLine) && (i < hdr.obsTypeList.size());
              i++)
         {
            int currPos = i * 7 + 18;
	    data[hdr.obsTypeList[i]] = asDouble(line.substr(currPos,7));
         }
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void Rinex3MetData::processContinuationLine(const string& line,
                                              const Rinex3MetHeader& hdr)
      throw(FFStreamError)
   {
      try
      {
         int currentElements = data.size();
         for (int i = currentElements; 
              (i < (maxObsPerContinuationLine + currentElements)) && 
                 (i < hdr.obsTypeList.size());
              i++)
         {
	    int currPos = ((i - maxObsPerLine) % maxObsPerContinuationLine) * 7 
               + 4;
            data[hdr.obsTypeList[i]] = asDouble(line.substr(currPos,7));
         }
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }
   }

   CommonTime Rinex3MetData::parseTime(const string& line) const
      throw(FFStreamError)
   {
      try
      {
            // according to the RINEX3 spec, any 2 digit year 80 or greater
            // is a year in the 1900s (1980-1999), under 80 is 2000s
         const int YearRollover = 80;
         
            // check if the spaces are in the right place - an easy way to check
            // if there's corruption in the file
         if ( (line[0] != ' ') ||
              (line[3] != ' ') ||
              (line[6] != ' ') ||
              (line[9] != ' ') ||
              (line[12] != ' ') ||
              (line[15] != ' '))
         {
            FFStreamError e("Invalid time format");
            GPSTK_THROW(e);
         }
         
         int year, month, day, hour, min;
         double sec;
         
         year  = asInt(   line.substr(1,  2 ));
         month = asInt(   line.substr(3,  3 ));
         day   = asInt(   line.substr(6,  3 ));
         hour  = asInt(   line.substr(9,  3 ));
         min   = asInt(   line.substr(12, 3 ));
         sec   = asInt(   line.substr(15, 3 ));
         
         if (year < YearRollover)
         {
            year += 100;
         }
         year += 1900;
         
         CommonTime rv(year, month, day, hour, min, sec);
         return rv;
      }
      catch (std::exception &e)
      {
         FFStreamError err("std::exception: " + string(e.what()));
         GPSTK_THROW(err);
      }
   }

   void Rinex3MetData::dump(ostream& s) const
   {
      s << time << endl;

      Rinex3MetMap::const_iterator itr;
      for(itr = data.begin(); itr != data.end(); itr++)
      {
         s << Rinex3MetHeader::convertObsType((*itr).first) 
           << " " << (*itr).second << endl;
      }
   }



}  // end of namespace
