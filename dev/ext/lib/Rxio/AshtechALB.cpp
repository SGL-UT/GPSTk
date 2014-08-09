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

#include "StringUtils.hpp"
#include "BinUtils.hpp"

#include "AshtechALB.hpp"
#include "AshtechStream.hpp"

using namespace std;

namespace gpstk
 {
   const char* AshtechALB::myId = "ALB";

   //---------------------------------------------------------------------------
   void AshtechALB::reallyGetRecord(FFStream& ffs)
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
   void AshtechALB::decode(const std::string& data)
      throw(std::exception, FFStreamError)
   {
      using BinUtils::decodeVar;

      string str(data);
      if (debugLevel>1)
         cout << "ALB " << str.length() << " " << endl;
      if (str.length() == 138)
      {
         ascii=false;
         header      = str.substr(0,11); str.erase(0,11);
         svid         = decodeVar<uint16_t>(str);
         str.erase(0,1);

         for (int w=0; w<10; w++)
            word[w] = decodeVar<uint32_t>(str);

         (void)decodeVar<uint16_t>(str);   // ignore checksum
         clear(ios_base::goodbit);
      }
   }

   //---------------------------------------------------------------------------
   void AshtechALB::dump(ostream& out) const throw()
   {
      ostringstream oss;
      using gpstk::StringUtils::asString;
      using gpstk::StringUtils::leftJustify;

      AshtechData::dump(out);
      oss << getName() << "1:"
          << " svid:" << svid
          << " S0W0: ..."
          << endl;
      out << oss.str() << flush;
   }

} // namespace gpstk
