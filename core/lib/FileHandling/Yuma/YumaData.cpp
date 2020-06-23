//==============================================================================
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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file YumaData.cpp
 * Encapsulate Yuma almanac file data, including I/O
 */

#include "StringUtils.hpp"
#include "GNSSconstants.hpp"

#include "YumaData.hpp"
#include "YumaStream.hpp"


using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
   short YumaData::nearFullWeek = 0;

   const std::string YumaData::sID   = "ID:";
   const std::string YumaData::sHlth = "Health:";
   const std::string YumaData::sEcc  = "Eccentricity:";
   const std::string YumaData::sTOA  = "Time of Applicability(s):";
   const std::string YumaData::sOrbI = "Orbital Inclination(rad):";
   const std::string YumaData::sRRA  = "Rate of Right Ascen(r/s):";
   const std::string YumaData::sSqrA = "SQRT(A)  (m 1/2):";
   const std::string YumaData::sRtAs = "Right Ascen at Week(rad):";
   const std::string YumaData::sArgP = "Argument of Perigee(rad):";
   const std::string YumaData::sMnAn = "Mean Anom(rad):";
   const std::string YumaData::sAf0  = "Af0(s):";
   const std::string YumaData::sAf1  = "Af1(s/s):";
   const std::string YumaData::sweek = "week:";

   // NOTE: It is impractical to EXACTLY produce the ICD-GPS-240
   // Yuma almanac format in C++.  The format includes scientific
   // format feature that work in FORTRAN but are not supported by
   // the C++ standard.  Specifically, 
   //  1.) three-digit exponents when they are not required, and 
   //  2.) the leading character is always zero 
   //      (i.e., the value is always beween -1 and +1).
   // The following will produce a something "very close" to the
   // Yuma format that will be successfully read by the reallyGetRecord
   // parser.
   void YumaData::reallyPutRecord(FFStream& ffs) const
   {
      YumaStream& strm = dynamic_cast<YumaStream&>(ffs);

      const int width=27;
      strm.setf(ios::fixed, ios::floatfield);
      strm.precision(0); 
      strm << right
           << "******** Week" << setw(5) << (week % 1024)
           << " almanac for PRN-" << setfill('0') << setw(2) << PRN << setfill(' ') 
           << " ********" << endl;

      strm << left
           << setw(width) << sID << "   "  
           << right
           << setfill('0') << setw(2) << PRN 
           << setfill(' ') << endl;

      strm << left 
           << setw(width) << sHlth << "   "
           << right 
           << setfill('0') << setw(3) << SV_health 
           << setfill(' ') << endl;

      strm.setf(ios::scientific, ios::floatfield);
      strm.setf(ios_base::uppercase);
      strm.precision(10);
      strm << left
           << setw(width) << sEcc  
           << right
           << setw(19) << ecc << endl;

      strm.setf(ios::fixed, ios::floatfield);
      strm.precision(6);
      strm << left
           << setw(width) << sTOA << "   "
           << setw(6) << Toa << endl;

      strm.setf(ios::scientific, ios::floatfield);
      strm.precision(10);
      strm << left << setw(width) << sOrbI << right << setw(19) << i_total << endl;
      strm << left << setw(width) << sRRA  << right << setw(19) << OMEGAdot << endl;

      strm.setf(ios::fixed, ios::floatfield);
      strm.precision(6);
      strm << left << setw(width) << sSqrA << "   " 
           << setw(11) << Ahalf << endl;

      strm.setf(ios::scientific, ios::floatfield);
      strm.precision(10);
      strm << left << setw(width) << sRtAs << right << setw(19) << OMEGA0 << endl;

      strm.setf(ios::fixed, ios::floatfield);
      strm.precision(9);
      strm << left << setw(width) << sArgP << "  "
           << right
           << setw(12) << w << endl;

      strm.setf(ios::scientific, ios::floatfield);
      strm.precision(10);
      strm << left << setw(width) << sMnAn << right << setw(19) << M0 << endl;
      strm << left << setw(width) << sAf0  << right << setw(19) << AF0 << endl;
      strm << left << setw(width) << sAf1  << right << setw(19) << AF1 << endl;

      strm.setf(ios::fixed, ios::floatfield);
      strm.precision(4);
      strm << left << setw(width) << sweek << "   "
           << setw(4) << week << endl;
      strm << endl;
   }   // end YumaData::reallyPutRecord


   string YumaData::lineParser(const string& line, const string& s) const
   {
      const int i = line.find_first_of(":");

      // Gotta have a colon or the format is wrong
      if (i == (int)string::npos)
         GPSTK_THROW(FFStreamError("Format error in YumaData"));

      // Only compare the first five characters since some files differ after that
      const int w = std::min(5, std::min(i, (int)s.size()));
      if (line.substr(0,w) != s.substr(0,w))
         GPSTK_THROW(FFStreamError("Format error in YumaData"));

      return stripLeading(line.substr(i+1), " ");
   }


   void YumaData::reallyGetRecord(FFStream& ffs)
   {
      YumaStream& strm = dynamic_cast<YumaStream&>(ffs);

      string line;

      xmit_time = 0;
      // Find next header line.
      // We don't need the header line as we will get all the information from the others
      bool found = false;
      unsigned lineCount = 0; 
      while (!found)
      {
          strm.formattedGetLine(line, true);
          if (line.substr(0,2).compare("**")==0) found = true;
          lineCount++;
             // If we don't find a header within 14 lines (which is the length
             // of a Yuma record) assume we will not find one.
          if (!found && lineCount>14)
          {
             FFStreamError exc("Could not find Yuma record.");
             GPSTK_THROW(exc);
          }  
      }

      //Second Line - PRN
      strm.formattedGetLine(line, true);
      PRN = asInt(lineParser(line, sID));

      //Third Line - Satellite Health
      strm.formattedGetLine(line, true);
      SV_health = asInt(lineParser(line, sHlth));

      //Fourth Line - Eccentricity
      strm.formattedGetLine(line, true);
      ecc = asDouble(lineParser(line, sEcc));

      //Fifth Line - Time of Applicability
      strm.formattedGetLine(line, true);
      Toa = (long) asDouble(lineParser(line, sTOA));

      //Sixth Line - Orbital Inclination
      strm.formattedGetLine(line, true);
      i_total = asDouble(lineParser(line, sOrbI));
      i_offset = i_total - 54.0 * (gpstk::PI / 180.0);

      //Seventh Line - Rate of Right Ascen
      strm.formattedGetLine(line, true);
      OMEGAdot = asDouble(lineParser(line, sRRA));

      //Eigth Line - SqrtA
      strm.formattedGetLine(line, true);
      Ahalf = asDouble(lineParser(line, sSqrA));

      //Ninth Line - Right Ascen at Week
      strm.formattedGetLine(line, true);
      OMEGA0 = asDouble(lineParser(line, sRtAs));

      //Tenth Line - Argument of Perigee
      strm.formattedGetLine(line, true);
      w = asDouble(lineParser(line, sArgP));

      //Eleventh Line - Mean Anomaly
      strm.formattedGetLine(line, true);
      M0 = asDouble(lineParser(line, sMnAn));

      //Twelfth Line - Af0
      strm.formattedGetLine(line, true);
      AF0 = asDouble(lineParser(line, sAf0));

      //Thirteenth Line - Af1
      strm.formattedGetLine(line, true);
      AF1 = asDouble(lineParser(line, sAf1));

      //Fourteenth Line - week
      // Its unclear whether this is a full week or week % 1024
      strm.formattedGetLine(line, true);
      week = asInt(lineParser(line, sweek));

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

   YumaData::operator OrbAlmGen() const
   {
     OrbAlmGen oag;

     oag.AHalf    = Ahalf; 
     oag.A        = Ahalf * Ahalf; 
     oag.af1      = AF1;
     oag.af0      = AF0;
     oag.OMEGA0   = OMEGA0; 
     oag.ecc      = ecc;
     oag.deltai   = i_offset;
     oag.i0       = i_total;
     oag.OMEGAdot = OMEGAdot;
     oag.w        = w;
     oag.M0       = M0;
     oag.toa      = Toa;
     oag.health   = SV_health; 
     
     // At this writing Yuma almanacs only exist for GPS
     oag.subjectSV = SatID(PRN, SatID::systemGPS); 

     // Unfortunately, we've NO IDEA which SV transmitted 
     // these data.
     oag.satID = SatID(0,SatID::systemGPS); 

     // 
     oag.ctToe = GPSWeekSecond(week,Toa,TimeSystem::GPS);

     // There is no transmit time in the Yuma alamanc format.  
     // Therefore, beginValid and endvalid are estimated.  The
     // estimate is based on IS-GPS-200 Table 20-XIII.  
     oag.beginValid = oag.ctToe - (70 * 3600.0);
     oag.endValid   = oag.beginValid + (144 * 3600.0);

     oag.dataLoadedFlag = true; 
     oag.setHealthy(false);
     if (oag.health==0) 
        oag.setHealthy(true);

        // It is assumed that the data were broadcast on
        // each of L1 C/A, L1 P(Y), and L2 P(Y).   We'll
        // load obsID with L1 C/A for the sake of completeness,
        // but this will probably never be examined.
     oag.obsID = ObsID(ObsID::otNavMsg,ObsID::cbL1,ObsID::tcCA);

     return oag; 
   }



} // namespace
