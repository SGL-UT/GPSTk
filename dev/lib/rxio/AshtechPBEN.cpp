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

#include "StringUtils.hpp"
#include "BinUtils.hpp"

#include "AshtechPBEN.hpp"
#include "AshtechStream.hpp"
#include "TimeConstants.hpp"

using namespace std;

namespace gpstk
 {
   const char* AshtechPBEN::myId = "PBN";

   //---------------------------------------------------------------------------
   void AshtechPBEN::reallyGetRecord(FFStream& ffs)
      throw(std::exception, FFStreamError, EndOfFile)
   {
      AshtechStream& stream=dynamic_cast<AshtechStream&>(ffs);

      // make sure the object is reset before starting the search
      clear(fmtbit | lenbit | crcbit);
      string& rawData = stream.rawData;

      // If this object doesn't have an id set yet, assume that the streams
      // most recent read id is what we need to be
      if (id == "" && rawData.size()>=11 && 
          rawData.substr(0,7) == preamble &&
          rawData[10]==',')
         id = rawData.substr(7,3);

      // If that didn't work, or this is object is not of the right type,
      // then give up.
      if (id == "" || !checkId(id))
         return;

      readBody(stream);
   }

   //---------------------------------------------------------------------------
   void AshtechPBEN::decode(const std::string& data)
      throw(std::exception, FFStreamError)
   {
      using gpstk::BinUtils::decodeVar;

      string str(data);
      if (debugLevel>3)
         StringUtils::hexDumpData(cout, data);
      if (str.length() == 69)
      {
         ascii=false;
         header      = str.substr(0,11); str.erase(0,11);
         sow         = 1e-3 * decodeVar<int32_t>(str);
         sitename    = str.substr(0,4); str.erase(0,4);
         navx        = decodeVar<double>(str);
         navy        = decodeVar<double>(str);
         navz        = decodeVar<double>(str);
         navt        = decodeVar<float>(str);
         navxdot     = decodeVar<float>(str);
         navydot     = decodeVar<float>(str);
         navzdot     = decodeVar<float>(str);
         navtdot     = decodeVar<float>(str);
         pdop        = decodeVar<uint16_t>(str);
         lat =  lon =  alt =  numSV =  hdop =  vdop =  tdop = 0;

         checksum = decodeVar<uint16_t>(str);
         clear();

         uint16_t csum=0;
         int len=data.size()-3-11;
         string body(data.substr(11, len));
         while (body.size()>1)
            csum += decodeVar<uint16_t>(body);

         if (csum != checksum)
         {
            setstate(crcbit);
            if (debugLevel)
               cout << "checksum error, computed:" << hex << csum
                    << " received:" << checksum << dec << endl;
         }

      }
      else
      {
         ascii=true;
         header = str.substr(0,11); str.erase(0,11);
         stringstream iss(str);
         double latMin,lonMin;
         char c;
         iss >> sow >> c
             >> navx>> c >> navy >> c >> navz >> c
             >> lat >> c >> latMin >> c >> lon >> c >> lonMin >> c >> alt >> c
             >> navxdot>> c  >> navydot>> c  >> navzdot >> c
             >> numSV >> c;
         getline(iss, sitename, ',');
         iss >> pdop>> c  >> hdop>> c  >> vdop>> c  >> tdop;

         lat += latMin / 60;
         lon += lonMin / 60;
         navt = navtdot = 0;
         if (iss)
            clear();
      }

      if (sow>FULLWEEK)
         setstate(fmtbit);
   }

   //---------------------------------------------------------------------------
   void AshtechPBEN::dump(ostream& out) const throw()
   {
      ostringstream oss;
      using gpstk::StringUtils::asString;
      using gpstk::StringUtils::leftJustify;

      AshtechData::dump(out);
      oss << getName() << "1:"
          << " SOW:" << asString(sow, 1)
          << " #SV:" << (int)numSV
          << " PDOP:" << (int)pdop
          << " ClkOff:" << asString(navt, 3) 
          << " ClkDft:" << asString(navtdot, 3)
          << " sitename:" << sitename
          << " " << (ascii?"ascii":"bin")
          << endl
          << getName() << "2:"
          << " X:" << asString(navx, 1)
          << " Y:" << asString(navy, 1)
          << " Z:" << asString(navz, 1)
          << " Vx:" << asString(navxdot, 3)
          << " Vy:" << asString(navydot, 3)
          << " Vz:" << asString(navzdot, 3)
          << endl;
      out << oss.str() << flush;
   }
} // namespace gpstk
