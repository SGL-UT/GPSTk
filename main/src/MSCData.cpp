#pragma ident "$Id: //depot/sgl/gpstk/dev/src/MSCData.cpp#1 $"

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
 * @file MSCData.cpp
 * Monitor station coordinate file data
 */

#include <math.h>
#include "MSCData.hpp"
#include "MSCStream.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{

   static const unsigned long SEC_YEAR = (unsigned long)(365.25 * gpstk::DayTime::SEC_DAY);

   void MSCData::reallyPutRecord(gpstk::FFStream & ffs) const
      throw(std::exception, gpstk::FFStreamError, StringException)
   {
      MSCStream& strm = dynamic_cast<MSCStream&>(ffs);

      string line;
      
      line += rightJustify(asString<short>(time.DOYyear()), 4);
      line += rightJustify(asString<short>(time.DOYday()), 3 , '0');
      line += rightJustify(asString<long>(station), 5);
      line += leftJustify(mnemonic, 7);
      double repoch = refepoch.DOYyear() 
         + (refepoch.DOYday() * gpstk::DayTime::SEC_DAY
            + refepoch.DOYsecond()) / SEC_YEAR;
      line += rightJustify(asString(repoch, 6), 7);
      double eepoch = effepoch.DOYyear() 
         + (effepoch.DOYday() * gpstk::DayTime::SEC_DAY
            + effepoch.DOYsecond()) / SEC_YEAR;
      line += rightJustify(asString(eepoch, 6), 7);
      line += rightJustify(asString(coordinates[0], 10), 12);
      line += rightJustify(asString(coordinates[1], 10), 12);
      line += rightJustify(asString(coordinates[2], 10), 12);
      line += rightJustify(asString(velocities[0], 5), 7);            
      line += rightJustify(asString(velocities[1], 5), 7);
      line += rightJustify(asString(velocities[2], 5), 7);
      
      ffs << line << endl;
      strm.lineNumber++;
   }

   void MSCData::reallyGetRecord(gpstk::FFStream& ffs)
      throw(std::exception, gpstk::FFStreamError,
            gpstk::StringUtils::StringException)
   {
      MSCStream& strm = dynamic_cast<MSCStream&>(ffs);

      string currentLine;
      
      strm.formattedGetLine(currentLine, true);
      int len = currentLine.length();
      
      short year = asInt(currentLine.substr(0, 4));
      short day =  asInt(currentLine.substr(4, 3));
      time.setYDoySod(year, day, 0.0);
      
      station = asInt(currentLine.substr(7, 5));
      mnemonic = currentLine.substr(12, 7);
      
      double epoch, intg, frac, sod;
      short doy;
      
         // can't have DOY 0, so use doy + 1 when generating times
      epoch = asDouble(currentLine.substr(19, 7));
      frac = modf(epoch, &intg);
      doy = (short)(frac * SEC_YEAR / gpstk::DayTime::SEC_DAY);
      sod = (short)((frac * gpstk::DayTime::SEC_DAY) - (doy * gpstk::DayTime::SEC_DAY));
      refepoch = gpstk::DayTime((short)intg, doy+1, sod);
      
      epoch = asDouble(currentLine.substr(26, 7));
      frac = modf(epoch, &intg);
      doy = (short)(frac * SEC_YEAR / gpstk::DayTime::SEC_DAY);
      sod = (frac * gpstk::DayTime::SEC_DAY) - (doy * gpstk::DayTime::SEC_DAY);
      effepoch = gpstk::DayTime((short)intg, doy+1, sod);
      
      coordinates[0] = asDouble(currentLine.substr(33, 12));
      coordinates[1] = asDouble(currentLine.substr(45, 12));
      coordinates[2] = asDouble(currentLine.substr(57, 12));
      
      velocities[0] = asDouble(currentLine.substr(69, 7));
      velocities[1] = asDouble(currentLine.substr(76, 7));
      velocities[2] = asDouble(currentLine.substr(83, 7));            
   }
}
