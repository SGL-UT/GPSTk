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

/**
 * @file SEMHeader.cpp
 * Encapsulate header of SEM file data, including I/O
 */

#include "StringUtils.hpp"
#include "icd_200_constants.hpp"

#include "SEMHeader.hpp"
#include "SEMStream.hpp"


using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
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
      
      numRecords = (short) asInt(line.substr(0,2));
      Title = line.substr(3,24);
      
      //Grab the second line
      strm.formattedGetLine(line);
      week = (short) asInt(line.substr(0,4));
      Toa = asInt(line.substr(5,6));
      
   } // end of reallyGetRecord()

   void SEMHeader::dump(ostream& s) const 
   {      
      std::cout << "numRecords = " << numRecords << std::endl;
      std::cout << "Title = " << Title << std::endl;
      std::cout << "week = " << week << std::endl;
      std::cout << "Toa = " << Toa << std::endl;
   }
   
} // namespace
