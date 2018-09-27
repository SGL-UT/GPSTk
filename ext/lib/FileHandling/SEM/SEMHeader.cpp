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
 * @file SEMHeader.cpp
 * Encapsulate header of SEM file data, including I/O
 */

#include "StringUtils.hpp"
#include "GNSSconstants.hpp"

#include "SEMHeader.hpp"
#include "SEMStream.hpp"


using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
   short SEMHeader::nearFullWeek = 0;

   void SEMHeader::reallyPutRecord(FFStream& ffs) const
      throw(std::exception, FFStreamError,
               gpstk::StringUtils::StringException)
   {
      string line;

      SEMStream& strm = dynamic_cast<SEMStream&>(ffs);

      line = leftJustify(asString<short>(numRecords),2);
      line += " ";
      line += Title;
      strm << line << endl;
      line.erase();

      line = rightJustify(asString<short>(week),4);
      line += " ";
      line += asString<long>(Toa);
      strm << line << endl;
      line.erase();

   }   // end SEMAHeader::reallyPutRecord


   void SEMHeader::reallyGetRecord(FFStream& ffs)
      throw(std::exception, FFStreamError,
               gpstk::StringUtils::StringException)
   {
      string line;

      SEMStream& strm = dynamic_cast<SEMStream&>(ffs);
      
      //Grab the first line
      strm.formattedGetLine(line);
      if (line.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_ .-+") != std::string::npos ||
          line.length() < 4 || line.length() > 28)
      {
         FFStreamError fe("Invalid data");
         GPSTK_THROW(fe);
      }
                  
      numRecords = (short) asInt(line.substr(0,2));
      Title = line.substr(3,24);

      //Grab the second line
      strm.formattedGetLine(line);
      if (line.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_ .-+") != std::string::npos ||
          line.length() < 6 || line.length() > 12)
      {
         FFStreamError fe("Invalid data");
         GPSTK_THROW(fe);
      }
      week = (short) asInt(line.substr(0,4));
      Toa = asInt(line.substr(5,6));

      if (nearFullWeek > 0)
      {
            // In case a full week is provided.
         week %= 1024;
         week += (nearFullWeek / 1024) * 1024;
         short diff = nearFullWeek - week;
         if (diff > 512)
            week += 512;
         else if(diff < -512)
            week -= 512;
      }

      strm.header = *this;
      strm.headerRead = true;

   } // end of reallyGetRecord()

   void SEMHeader::dump(ostream& s) const
   {
      std::cout << "numRecords = " << numRecords << std::endl;
      std::cout << "Title = " << Title << std::endl;
      std::cout << "week = " << week << std::endl;
      std::cout << "Toa = " << Toa << std::endl;
   }

} // namespace
