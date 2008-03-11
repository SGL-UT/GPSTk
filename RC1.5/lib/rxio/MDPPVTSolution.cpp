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

#include "MDPPVTSolution.hpp"
#include "MDPStream.hpp"

using gpstk::StringUtils::asString;
using gpstk::BinUtils::hostToNet;
using gpstk::BinUtils::netToHost;
using gpstk::BinUtils::encodeVar;
using gpstk::BinUtils::decodeVar;
using namespace std;

namespace gpstk
{
   //---------------------------------------------------------------------------
   MDPPVTSolution::MDPPVTSolution()
      throw()
   {
      id = myId;
      timep = gpstk::DayTime::BEGINNING_OF_TIME;
      x[0] = x[1] = x[2] = 0;
      v[0] = v[1] = v[2] = 0;
      dtime = ddtime = 0;
      numSVs = fom = pvtMode = corrections = 0;
   } // MDPPVTSolution::MDPPVTSolution()


   //---------------------------------------------------------------------------
   string MDPPVTSolution::encode() const
      throw()
   {
      string str;
      str += encodeVar<double>(x[0]);
      str += encodeVar<double>(x[1]);
      str += encodeVar<double>(x[2]);
      str += encodeVar<float>(v[0]);
      str += encodeVar<float>(v[1]);
      str += encodeVar<float>(v[2]);
      str += encodeVar<uint8_t>(numSVs);
      str += encodeVar<int8_t>(fom);
      str += encodeVar<uint16_t>(time.GPSfullweek());
      str += encodeVar<double>(time.GPSsecond());
      str += encodeVar<double>(dtime);
      str += encodeVar<double>(ddtime);
      str += encodeVar<uint8_t>(pvtMode);
      str += encodeVar<uint8_t>(corrections);
      return str;
   } // MDPPVTSolution::encode()

      
   //---------------------------------------------------------------------------
   void MDPPVTSolution::decode(string str)
      throw()
   {
      if (str.length() != myLength)
         return;

      clearstate(lenbit);
      
      x[0]        = decodeVar<double>(str);
      x[1]        = decodeVar<double>(str);
      x[2]        = decodeVar<double>(str);
      v[0]        = decodeVar<float>(str);
      v[1]        = decodeVar<float>(str);
      v[2]        = decodeVar<float>(str);
      numSVs      = decodeVar<uint8_t>(str);
      fom         = decodeVar<int8_t>(str);
      int week    = decodeVar<uint16_t>(str);
      double sow  = decodeVar<double>(str);
      dtime       = decodeVar<double>(str);
      ddtime      = decodeVar<double>(str);
      pvtMode     = decodeVar<uint8_t>(str);
      corrections = decodeVar<uint8_t>(str);

      if (week < 0 || week > 5000 || sow < 0 || sow > 604800)
         return;

      timep.setGPS(week, sow);

      clearstate(fmtbit);
   } // MDPPVTSolution::decode()


   //---------------------------------------------------------------------------
   void MDPPVTSolution::dump(ostream& out) const
      throw()
   {
      ostringstream oss;
      using gpstk::StringUtils::asString;
      using gpstk::StringUtils::leftJustify;

      MDPHeader::dump(oss);
      oss << getName() << "1:"
          << " #SV:" << (int)numSVs
          << " FoM:" << (int)fom
          << " ClkOff:" << asString(dtime*1e9, 3) 
          << " ClkDft:" << asString(ddtime*86400*1e6, 3)
          << " PVTMode:" << (int)pvtMode
          << " Corr:" << hex << (int)corrections << dec
          << endl
          << getName() << "2:"
          << " X:" << asString(x[0], 3)
          << " Y:" << asString(x[1], 3)
          << " Z:" << asString(x[2], 3)
          << " Vx:" << asString(v[0], 3)
          << " Vy:" << asString(v[1], 3)
          << " Vz:" << asString(v[2], 3)
          << endl;
      out << oss.str() << flush;
   } // MDPPVTSolution::dump()
}
