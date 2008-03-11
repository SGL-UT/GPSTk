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
 * @file YumaData.cpp
 * Encapsulate Yuma almanac file data, including I/O
 */

#include "StringUtils.hpp"
#include "icd_200_constants.hpp"

#include "YumaData.hpp"
#include "YumaStream.hpp"


using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
   const std::string YumaData::sID   = "ID:                         ";
   const std::string YumaData::sHlth = "Health:                     ";
   const std::string YumaData::sEcc  = "Eccentricity:              ";
   const std::string YumaData::sTOA  = "Time of Applicability(s):  ";
   const std::string YumaData::sOrbI = "Orbital Inclination(rad):  ";
   const std::string YumaData::sRRA  = "Rate of Right Ascen(r/s):  ";
   const std::string YumaData::sSqrA = "SQRT(A)  (m 1/2):          ";
   const std::string YumaData::sRtAs = "Right Ascen at Week(rad):  ";
   const std::string YumaData::sArgP = "Argument of Perigee(rad):  ";
   const std::string YumaData::sMnAn = "Mean Anom(rad):            ";
   const std::string YumaData::sAf0  = "Af0(s):                    ";
   const std::string YumaData::sAf1  = "Af1(s/s):                  ";
   const std::string YumaData::sweek = "week:                      ";

   void YumaData::reallyPutRecord(FFStream& ffs) const 
      throw(std::exception, FFStreamError, 
               gpstk::StringUtils::StringException)  
   {
      string line;

      YumaStream& strm = dynamic_cast<YumaStream&>(ffs);
      
      // first the epoch line to 'line'
      line  = "******** Week";
      int epochWeek = week % 1024;
      line += rightJustify(asString<short>(epochWeek), 5);
      line += " almanac for PRN-";
      line += rightJustify(asString<short>(PRN), 2, '0');
      line += " ********";

      // write the header line
      strm << line << endl;
      line.erase();

      // Write the ID line
      line = sID;
      line += rightJustify(asString<short>(PRN),2,'0');
      strm << line << endl;
      line.erase();
      
      // Write the Health line
      line = sHlth;
      line += rightJustify(asString<short>(SV_health),3,'0');  // should be hex
      strm << line << endl;
      line.erase();
      
      // Write the Ecc line
      line = sEcc;
      line += leftJustify(asString(doub2for(ecc,17,3,false)),18);  
      strm << line << endl;
      line.erase();
      
      // Write the Toa line
      line = sTOA;
      line += leftJustify(asString((double)Toa,4),11);  
      strm << line << endl;
      line.erase();
      
      // Write the Orbital Inc line
      line = sOrbI;
      double i_total = i_offset + 54.0 * (gpstk::PI / 180.0 );
      if (i_total >=0) line += " ";
      line += leftJustify(asString(i_total,10),17);  
      strm << line << endl;
      line.erase();
      
      // Write the Rate of Right Ascen line
      line = sRRA;
      line += leftJustify(asString(doub2for(OMEGAdot,17,3,false)),18);
      strm << line << endl;
      line.erase();
      
      // Write the SqrtA line
      line = sSqrA;
      line += " ";
      line += leftJustify(asString(Ahalf,6),18); 
      strm << line << endl;
      line.erase();
      
      // Write the Right Ascen at Week line
      line = sRtAs;
      line += leftJustify(asString(doub2for(OMEGA0,17,3,false)),18); 
      strm << line << endl;
      line.erase();
      
      // Write the Argument of Perigee line
      line = sArgP;
      if (w>=0) line += " ";
      line += leftJustify(asString(w,9),18); 
      strm << line << endl;
      line.erase();
      
      // Write Mean Anomaly line
      line = sMnAn;
      line += leftJustify(asString(doub2for(M0,17,3,false)),18);  
      strm << line << endl;
      line.erase();
      
      // Write the Af0 line
      line = sAf0;
      line += leftJustify(asString(doub2for(AF0,17,3,false)),18);
      strm << line << endl;
      line.erase();
      
      // Write the AF1 line
      line = sAf1;
      line += leftJustify(asString(doub2for(AF1,17,3,false)),18);  // should be hex
      strm << line << endl;
      line.erase();
      
      // Write the week line
      line = sweek;
      line += rightJustify(asString<short>(epochWeek),5);  // should be hex
      strm << line << endl;
      line.erase();
      strm << endl;
      
   }   // end YumaData::reallyPutRecord


   void YumaData::reallyGetRecord(FFStream& ffs) 
      throw(std::exception, FFStreamError, 
               gpstk::StringUtils::StringException)  
   {
      YumaStream& strm = dynamic_cast<YumaStream&>(ffs);
            
      string line;
 
      // We don't need first line as we will get all the information from the others
      strm.formattedGetLine(line, true);
      
      //Second Line - PRN
      strm.formattedGetLine(line, true);
      stripLeading( line, sID );
      PRN = asInt(line);

      //Third Line - Satellite Health
      strm.formattedGetLine(line, true);
      stripLeading( line, sHlth ); 
      SV_health = asInt(line);
      
      //Fourth Line - Eccentricity
      strm.formattedGetLine(line, true);
      stripLeading( line, sEcc ); 
      ecc = asDouble(line);

      //Fifth Line - Time of Applicability
      strm.formattedGetLine(line, true);
      stripLeading( line, sTOA ); 
      double dToa = asDouble(line);
      Toa = (long) dToa;

      //Sixth Line - Orbital Inclination
      strm.formattedGetLine(line, true);
      stripLeading( line, sOrbI ); 
      double i_total = asDouble(line);
      i_offset = i_total - 54.0 * (gpstk::PI / 180.0);
      
      //Seventh Line - Rate of Right Ascen
      strm.formattedGetLine(line, true);
      stripLeading( line, sRRA ); 
      OMEGAdot = asDouble(line);
      
      //Eigth Line - SqrtA
      strm.formattedGetLine(line, true);
      stripLeading( line, sSqrA ); 
      Ahalf = asDouble(line);
      
      //Ninth Line - Right Ascen at Week
      strm.formattedGetLine(line, true);
      stripLeading( line, sRtAs ); 
      OMEGA0 = asDouble(line);
      
      //Tenth Line - Argument of Perigee
      strm.formattedGetLine(line, true);
      stripLeading( line, sArgP ); 
      w = asDouble(line);
      
      //Eleventh Line - Mean Anomaly
      strm.formattedGetLine(line, true);
      stripLeading( line, sMnAn ); 
      M0 = asDouble(line);
      
      //Twelfth Line - Af0
      strm.formattedGetLine(line, true);
      stripLeading( line, sAf0 ); 
      AF0 = asDouble(line);
      
      //Thirteenth Line - Af1
      strm.formattedGetLine(line, true);
      stripLeading( line, sAf1 ); 
      AF1 = asDouble(line);
      
      //Fourteenth Line - week
      strm.formattedGetLine(line, true);
      stripLeading( line, sweek ); 
      int epoch_week = asInt(line);
      week = epoch_week + 1024;                    // Need a way to set epoch     Do we??
      
      xmit_time = 0;
      strm.formattedGetLine(line,true);
      
   } // end of reallyGetRecord()

   void YumaData::dump(ostream& s) const 
   {      
      cout << "PRN = " << PRN << endl;
      cout << "week = " << week << endl;
      cout << "SV_health = " << SV_health << endl;
      cout << "ecc = " << ecc << endl;
      cout << "Toa = " << Toa << endl;
      cout << "i_offset = " << i_offset << endl;
      cout << "OMEGAdot = " << OMEGAdot << endl;
      cout << "Ahalf = " << Ahalf << endl;
      cout << "OMEGA0 = " << OMEGA0 << endl;
      cout << "w = " << w << endl;
      cout << "M0 = " << M0 << endl;
      cout << "AF0 = " << AF0 << endl;
      cout << "AF1 = " << AF1 << endl;
      cout << "xmit_time = " << xmit_time << endl;
   
   } // end of dump()
   
   YumaData::operator AlmOrbit() const
   {
      AlmOrbit ao(PRN, ecc,i_offset, OMEGAdot, Ahalf, OMEGA0,
                   w, M0, AF0, AF1, Toa, xmit_time, week, SV_health);
      
      return ao;
   
   } // end of AlmOrbit()
} // namespace
