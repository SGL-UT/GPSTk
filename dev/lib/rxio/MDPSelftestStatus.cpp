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

#include <sstream>

#include "StringUtils.hpp"
#include "BinUtils.hpp"

#include "MDPSelftestStatus.hpp"
#include "MDPStream.hpp"

using gpstk::StringUtils::asString;
using gpstk::BinUtils::encodeVar;
using gpstk::BinUtils::decodeVar;
using namespace std;

namespace gpstk
{
   //---------------------------------------------------------------------------
   MDPSelftestStatus::MDPSelftestStatus()
      throw() :
      selfTestTime(gpstk::DayTime::BEGINNING_OF_TIME),
      firstPVTTime(gpstk::DayTime::BEGINNING_OF_TIME),
      antennaTemp(0), receiverTemp(0), status(0xffffffff),
      cpuLoad(0), extFreqStatus(0)
   {
      id = myId;
   } // MDPSelftestStatus::MDPSelftestStatus()


   //---------------------------------------------------------------------------
   string MDPSelftestStatus::encode() const
      throw()
   {
      string str;
      str += encodeVar( (float)    antennaTemp);
      str += encodeVar( (float)    receiverTemp);
      str += encodeVar( (uint32_t) status);
      str += encodeVar( (float)    cpuLoad);
      str += encodeVar( (uint32_t) 100*selfTestTime.GPSsecond());
      str += encodeVar( (uint16_t) selfTestTime.GPSfullweek());
      str += encodeVar( (uint16_t) firstPVTTime.GPSfullweek());
      str += encodeVar( (uint32_t) 100*firstPVTTime.GPSsecond());
      str += encodeVar( (uint16_t) extFreqStatus);
      str += encodeVar( (uint16_t) saasmStatusWord);
      return str;
   } // MDPSelftestStatus::encode()

      
   //---------------------------------------------------------------------------
   void MDPSelftestStatus::decode(string str)
      throw()
   {
      if (str.length() != myLength)
         return;

      clearstate(lenbit);
      
      
      long sow100;
      int week;

      antennaTemp   = decodeVar<float>(str);
      receiverTemp  = decodeVar<float>(str);
      status        = decodeVar<uint32_t>(str);
      cpuLoad       = decodeVar<float>(str);
      sow100        = decodeVar<uint32_t>(str);
      week          = decodeVar<uint16_t>(str);
      selfTestTime.setGPSfullweek(week, double(sow100)*0.01);
      week          = decodeVar<uint16_t>(str);
      sow100        = decodeVar<uint32_t>(str);
      firstPVTTime.setGPSfullweek(week, double(sow100)*0.01);
      extFreqStatus = decodeVar<uint16_t>(str);
      saasmStatusWord  = decodeVar<uint16_t>(str);
      
      clearstate(fmtbit);
   } // MDPSelftestStatus::decode()


   //---------------------------------------------------------------------------
   void MDPSelftestStatus::dump(ostream& out) const
      throw()
   {
      ostringstream oss;
      using gpstk::StringUtils::asString;
      using gpstk::StringUtils::leftJustify;

      MDPHeader::dump(oss);
      oss << getName() << "1:"
          << " Tst:" << selfTestTime.printf("%4F/%9.2g")
          << " Tpvt:" << firstPVTTime.printf("%4F/%9.2g")
          << " Ant. Temp:" << antennaTemp
          << " Rx. Temp:" << receiverTemp
          << " status:" << hex << status << dec
          << " cpuLoad:" << cpuLoad
          << " extFreq:" << hex << extFreqStatus << dec
          << " ssw:" << hex << saasmStatusWord << dec
          << endl;
      out << oss.str() << flush;
   } // MDPSelftestStatus::dump()

}
