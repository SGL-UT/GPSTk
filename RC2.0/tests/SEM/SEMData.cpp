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
 * @file SEMData.cpp
 * Encapsulate SEM almanac file data, including I/O
 */

#include "StringUtils.hpp"
#include "icd_200_constants.hpp"

#include "SEMData.hpp"
#include "SEMStream.hpp"


using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
   void SEMData::reallyPutRecord(FFStream& ffs) const 
      throw(std::exception, FFStreamError, 
               gpstk::StringUtils::StringException)  
   {
      string line;

      SEMStream& strm = dynamic_cast<SEMStream&>(ffs);
      
      //First output blank line to mark between records
      strm << std::endl;
      
      //PRN output
      strm << asString<short>(PRN) << endl;
      
      //SVNnum
      strm << asString<short>(SVNnum) << endl;
      
      //URAnum
      strm << asString<short>(URAnum) << endl;
      
      //Ecc, i_offset, OMEGAdot
      line += rightJustify(asString(doub2for(ecc,22,4,false)),23);
      line += rightJustify(asString(doub2for(i_offset/gpstk::PI,22,4,false)),24);
      line += rightJustify(asString(doub2for((OMEGAdot/gpstk::PI),22,4,false)),24);
      strm << line << endl;
      line.erase();
      
      //Ahalf, OMEGA0, w
      line += rightJustify(asString(doub2for(Ahalf,22,4,false)),23);
      line += rightJustify(asString(doub2for((OMEGA0/gpstk::PI),22,4,false)),24);
      line += rightJustify(asString(doub2for((w/gpstk::PI),22,4,false)),24);
      strm << line << endl;
      line.erase();      
      
      //M0, AF0, AF1
      line += rightJustify(asString(doub2for((M0/gpstk::PI),22,4,false)),23);
      line += rightJustify(asString(doub2for(AF0,22,4,false)),24);
      line += rightJustify(asString(doub2for(AF1,22,4,false)),24);
      strm << line << endl;
      line.erase(); 
      
      //SV_health
      strm << asString<short>(SV_health) << endl;
      
      //satConfig
      strm << asString<short>(satConfig) << endl;
      
            
   }   // end SEMData::reallyPutRecord


   void SEMData::reallyGetRecord(FFStream& ffs) 
      throw(std::exception, FFStreamError, 
               gpstk::StringUtils::StringException)  
   {
      string line;
      
      SEMStream& strm = dynamic_cast<SEMStream&>(ffs);
      
      //if(!strm.headerRead)
      //    strm >> strm.header;
       
      SEMHeader& hdr = strm.header;

      //Don't need first line - empty space
      strm.formattedGetLine(line, true);
      
      // Second line - PRN
      strm.formattedGetLine(line, true);
      PRN = asInt(line);

      // Third line - SVN Number
      // HACKHACKHACK This information might not be here??? Find out more info
      strm.formattedGetLine(line, true);
      SVNnum = (short) asInt(line);
      
      // Fourth line - Average URA Number as defined in ICD-GPS-200
      strm.formattedGetLine(line, true);
      URAnum = (short) asInt(line);

            
      // Fifth line - Eccentricity, Inclinatin Offset, and Rate of Right Ascension
      strm.formattedGetLine(line, true);
      ecc = asDouble(line.substr(0,23));
      i_offset = asDouble(line.substr(24,47));
      OMEGAdot = asDouble(line.substr(48,72));
      i_offset *= gpstk::PI;
      OMEGAdot *= gpstk::PI;


      // Sixth line - Sqrt of A, Omega0, and Arg of Perigee
      strm.formattedGetLine(line, true);
      Ahalf = asDouble(line.substr(0,23));
      OMEGA0 = asDouble(line.substr(24,47));
      OMEGA0 *= gpstk::PI;
      w = asDouble(line.substr(48,72));
      w *= gpstk::PI;
      
      // Seventh Line - M0, AF0, AF1
      strm.formattedGetLine(line, true);
      M0 = asDouble(line.substr(0,23));
      M0 *= gpstk::PI;
      AF0 = asDouble(line.substr(24,47));
      AF1 = asDouble(line.substr(48,72));
      
      // Eigth line - Satellite Health
      strm.formattedGetLine(line, true);
      SV_health = (short) asInt(line);
      
      // Ninth line - Satellite Config
      strm.formattedGetLine(line, true); 
      satConfig = (short) asInt(line);
            

      //week = strm.header.week + 1024;                    // Need a way to set epoch  Hmmm really?

      
      week = hdr.week;
      Toa = hdr.Toa;

      
      xmit_time = 0;
      
   } // end of reallyGetRecord()

   void SEMData::dump(ostream& s) const 
   {      
      std::cout << "PRN =" << PRN << std::endl;
      std::cout << "SVNnum =" << SVNnum << std::endl;
      std::cout << "URAnum =" << URAnum << std::endl;
      std::cout << "ecc =" << ecc << std::endl;
      std::cout << "i_offset =" << i_offset << std::endl;
      std::cout << "OMEGAdot =" << OMEGAdot << std::endl;
      std::cout << "Ahalf =" << Ahalf << std::endl;
      std::cout << "OMEGA0 =" << OMEGA0 << std::endl;
      std::cout << "w =" << w << std::endl;
      std::cout << "M0 =" << M0 << std::endl;
      std::cout << "AF0 =" << AF0 << std::endl;
      std::cout << "AF1 =" << AF1 << std::endl;
      std::cout << "SV_health =" << SV_health << std::endl;
      std::cout << "satConfig =" << satConfig << std::endl;
      std::cout << "xmit_time =" << xmit_time << std::endl;
      std::cout << "week" << week << std::endl;
      std::cout << "toa" << Toa << std::endl;
   }
   
   SEMData::operator AlmOrbit() const
   {
      
      AlmOrbit ao(PRN, ecc,i_offset, OMEGAdot, Ahalf, OMEGA0,
                   w, M0, AF0, AF1, Toa, xmit_time, week, SV_health);
		   
      return ao;
   
   }
   
} // namespace
