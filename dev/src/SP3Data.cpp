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
 * @file SP3Data.cpp
 * Encapsulate SP3 file data, including I/O
 */

#include "StringUtils.hpp"
#include "DayTime.hpp"
#include "SP3Stream.hpp"
#include "SP3Data.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
      /// @todo Implement this function.
   void SP3Data::reallyPutRecord(FFStream& ffs) const 
      throw(std::exception, FFStreamError, StringException)
   {
      SP3Stream& strm = dynamic_cast<SP3Stream&>(ffs);
      
      FFStreamError err("Writing is not supported for this file format");
      GPSTK_THROW(err);
   }

   void SP3Data::dump(ostream& s) const 
   {
      if (flag != '*')
      {
         s << flag
            << " " << setw(2) << id
            << " " << fixed << setw(13) << setprecision(6) << x[0]
            << " " << fixed << setw(13) << setprecision(6) << x[1]
            << " " << fixed << setw(13) << setprecision(6) << x[2]
            << " " << fixed << setw(13) << setprecision(6) << clk << endl;
      }
   };

   void SP3Data::reallyGetRecord(FFStream& ffs)
      throw(std::exception, FFStreamError, StringException)
   {
      SP3Stream& strm = dynamic_cast<SP3Stream&>(ffs);

      string line;
      strm.formattedGetLine(line);
      
      if (line[0]=='*')
      {
         if (line.size()<=30)
         {
            FFStreamError err("Invalid line length " + 
                              asString(line.size()));
            GPSTK_THROW(err);                  
         }
         int year = asInt(line.substr(3,4));
         int month = asInt(line.substr(8,2));
         int dom = asInt(line.substr(11,2));
         int hour = asInt(line.substr(14,2));
         int minute = asInt(line.substr(17,2));
         double second = asInt(line.substr(20,10));
         DayTime t(year, month, dom, hour, minute, second);
         strm.currentEpoch = t;
         
            // since we only got the time, go ahead and read another 
            // record
         strm.formattedGetLine(line);
      }

         /// set the internal record time
      time = strm.currentEpoch;
      
      if ((line[0]=='P' || line[0]=='V'))
      {
         if (line.size()<=59)
         {
            FFStreamError err("Invalid line length " + 
                              asString(line.size()));
            GPSTK_THROW(err);
         }
         flag = line[0];
         id = asInt(line.substr(1, 3));
         x[0] = asDouble(line.substr(5,13));
         x[1] = asDouble(line.substr(19,13));
         x[2] = asDouble(line.substr(33,13));
         clk = asDouble(line.substr(47,13));
      }
      else if (line.substr(0,3) == string("EOF"))
      {
            // this next read had better fail - if it does, 
            // an exception will be thrown and the next FFStreamError
            // won't get thrown
         strm.formattedGetLine(line, true);
         
         FFStreamError err("EOF text found but file didn't end");
         GPSTK_THROW(err);
      }
      else
      {
         FFStreamError err("Unknown line label " + string(1,line[0]));
         GPSTK_THROW(err);
      }
   }   // end reallyGetRecord()
} // namespace
