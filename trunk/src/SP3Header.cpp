#pragma ident "$Id: //depot/sgl/gpstk/dev/src/SP3Header.cpp#1 $"

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

namespace gpstk
{
   using namespace gpstk::StringUtils;
   using namespace std;

      /// @todo Implement this function.
   void SP3Header::reallyPutRecord(FFStream& ffs) const
      throw(exception, FFStreamError, StringException)
   {
      SP3Stream& strm = dynamic_cast<SP3Stream&>(ffs);
      
      FFStreamError err("Writing is not supported for this file format");
      GPSTK_THROW(err);
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
      
      s << " List of satellite PRN/acc (" << svList.size() << " total) :\n";
      int i=0;
      std::map<short,short>::const_iterator it=svList.begin();
      while(it != svList.end()) {
         s << "  " << setw(2) << it->first << "/" << it->second;
         if(!(++i % 8)) s << endl;
         it++;
      }
      if(++i % 8) s << endl;

      s << " Comments:\n";
      for(size_t j=0; j<comments.size(); j++) s << "    " << comments[j] << endl;

      s << "End of SP3 header" << endl;
   }

   void SP3Header::reallyGetRecord(FFStream& ffs)
      throw(exception, FFStreamError, gpstk::StringUtils::StringException)
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
         time = DayTime(year, month, dom, hour, minute, second);
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
      vector<short> svsAsWritten;

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
                  short sv = asInt(line.substr(index, 3));
                  svsAsWritten[readSVs] = sv;
                  svList[sv] = 0;
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
                  svList[svsAsWritten[readSVs]] = asInt(line.substr(index,3));
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
   }
}  // namespace
