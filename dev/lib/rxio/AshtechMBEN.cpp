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

#include "AshtechMBEN.hpp"
#include "AshtechStream.hpp"
#include "icd_200_constants.hpp"

using namespace std;

namespace gpstk
 {
   const char* AshtechMBEN::mpcId = "MPC";
   const char* AshtechMBEN::mcaId = "MCA";


   //---------------------------------------------------------------------------
   void AshtechMBEN::reallyGetRecord(FFStream& ffs)
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
   void AshtechMBEN::decode(const std::string& data)
      throw(std::exception, FFStreamError)
   {
      using gpstk::BinUtils::decodeVar;

      string str(data);
      if (debugLevel>2)
         cout << "MBEN " << str.length() << " " << endl;

      if (str.length() == 108 || str.length()==52)
      {
         ascii=false;
         header = str.substr(0,11); str.erase(0,11);

         seq    = decodeVar<uint16_t>(str);
         left   = decodeVar<uint8_t>(str);
         svprn  = decodeVar<uint8_t>(str);
         el     = decodeVar<uint8_t>(str);
         az     = decodeVar<uint8_t>(str);
         chid   = decodeVar<uint8_t>(str);

         ca.decodeBIN(str);

         if (id == mpcId)
         {
            p1.decodeBIN(str);
            p2.decodeBIN(str);
         }

         clear();
      }
      else
      {
         ascii=true;
         header = str.substr(0,11); str.erase(0,11);
         stringstream iss(str);
         char c; 
         iss >> seq >> c
             >> left >> c
             >> svprn >> c
             >> el >> c
             >> az >> c
             >> chid >> c;

         ca.decodeASCII(iss);

         if (id == mpcId)
         {
            p1.decodeASCII(iss);
            p2.decodeASCII(iss);
         }
         
         clear();
      }

      if (seq>36000)
         setstate(fmtbit);
   }


   //---------------------------------------------------------------------------
   void AshtechMBEN::code_block::decodeASCII(stringstream& str)
      throw(std::exception, FFStreamError)
   {
      char c;
      str >> warning >> c
          >> goodbad >> c
          >> polarity_known>> c
          >> ireg >> c
          >> qa_phase >> c
          >> full_phase >> c
          >> raw_range >> c
          >> doppler >> c
          >> smoothing >> c
          >> smooth_cnt >> c;

      // The ashtech docs say this field should be in 1e-4 Hz
      // The data sure doesn't look like it, however
      //doppler *= 1e-4;
      raw_range *= 1e-3; //convert ms to sec
   }


   //---------------------------------------------------------------------------
   void AshtechMBEN::code_block::decodeBIN(string& str)
      throw(std::exception, FFStreamError)
   {
      using gpstk::BinUtils::decodeVar;
      uint32_t smo;
      warning        = decodeVar<uint8_t>(str);
      goodbad        = decodeVar<uint8_t>(str);
      polarity_known = decodeVar<uint8_t>(str);
      ireg           = decodeVar<uint8_t>(str);
      qa_phase       = decodeVar<uint8_t>(str);
      full_phase     = decodeVar<double>(str);
      raw_range      = decodeVar<double>(str);
      doppler        = decodeVar<int32_t>(str);
      smo            = decodeVar<uint32_t>(str);

      doppler *= 1e-4;
      smoothing = (smo & 0x800000 ? -1e-3 : 1e-3) * (smo & 0x7fffff);
      smooth_cnt = (smo >> 24) & 0xff;
   }


   //---------------------------------------------------------------------------
   void AshtechMBEN::code_block::dump(ostream& out) const
   {
      using gpstk::StringUtils::asString;
      out << hex
          << "warn:" << (int)warning
          << " gb:" << (int)goodbad
          << " pol:" << (int)polarity_known
          << dec
          << " phase:" << asString(full_phase, 1)
          << " range:" << asString(raw_range*1e3, 3)
          << " doppler:" << doppler
          << " smo:" << smoothing;
   }


   //---------------------------------------------------------------------------
   float AshtechMBEN::code_block::snr(float chipRate) const throw()
   {
      const int   n = 20000;     // number of samples in 1 ms
      const float m = 4.14;      // magnitude of the carrier estimate;
      float bw = 0.9 * chipRate; // equivalent noise bandwidth (Hz)

      const float d = PI/(n*n*m*m*4.0);
      float snr=0;

      if (ireg)
      {
         snr = exp(((float)ireg)/25.0);
         snr = snr*snr*bw*d;
         snr = 10 * log10(snr);
      }

      return snr;
   }
   
   //---------------------------------------------------------------------------
   void AshtechMBEN::dump(ostream& out) const throw()
   {
      ostringstream oss;
      using gpstk::StringUtils::asString;

      AshtechData::dump(oss);
      oss << getName() << "1:"
          << " seq:" << 0.05 * seq
          << " left:" << (int)left
          << " prn:" << (int)svprn
          << " el:" << (int)el
          << " az:" << (int)az
          << " chid:" << (int)chid
          << " " << (ascii?"ascii":"bin")
          << endl;

      oss << getName() << "2: ca";
      ca.dump(oss);
      oss << endl;

      if (id == mpcId)
      {
         oss << getName() << "3: p1";
         p1.dump(oss);
         oss << endl;
         oss << getName() << "4: p2";
         p2.dump(oss);
         oss << endl;
      }
      out << oss.str() << flush;
   }
} // namespace gpstk
