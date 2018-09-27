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

/// @file EOPPrediction.cpp
/// Implementation of class EOPPrediction: formulas to predict EOPs from data in
///  NGA 'EOPP###.txt' files.

//------------------------------------------------------------------------------------
#include "GNSSconstants.hpp"     // for TWO_PI
#include "TimeConverters.hpp"
#include "EOPPrediction.hpp"

//------------------------------------------------------------------------------------
using namespace std;

namespace gpstk
{
   // load the EOPPrediction in the given file
   // return  0 if ok, -1 if error reading file
   int EOPPrediction::loadFile(string filename)
      throw(FileMissingException)
   {
      bool ok;
      int n;
      string line,word;
      ifstream inpf(filename.c_str());
      if(!inpf) {
         FileMissingException fme("Could not open EOPP file " + filename);
         GPSTK_THROW(fme);
      }

      ok = true;
      n = 0;         // n is line number
      while(!inpf.eof() && inpf.good()) {
         getline(inpf,line);
         StringUtils::stripTrailing(line,'\r');
         if(inpf.bad()) break;
         if(line.size() > 80) { ok=false; break; }
         switch(n) {
            case 0:
               if(line.size() < 76) { ok=false; break; }
               word = line.substr( 0,10); ta = StringUtils::asDouble(word);
               word = line.substr(10,10);  A = StringUtils::asDouble(word);
               word = line.substr(20,10);  B = StringUtils::asDouble(word);
               word = line.substr(30,10); C1 = StringUtils::asDouble(word);
               word = line.substr(40,10); C2 = StringUtils::asDouble(word);
               word = line.substr(50,10); D1 = StringUtils::asDouble(word);
               word = line.substr(60,10); D2 = StringUtils::asDouble(word);
               word = line.substr(70, 6); P1 = StringUtils::asDouble(word);
               break;
            case 1:
               if(line.size() < 78) { ok=false; break; }
               word = line.substr( 0, 6); P2 = StringUtils::asDouble(word);
               word = line.substr( 6,10);  E = StringUtils::asDouble(word);
               word = line.substr(16,10);  F = StringUtils::asDouble(word);
               word = line.substr(26,10); G1 = StringUtils::asDouble(word);
               word = line.substr(36,10); G2 = StringUtils::asDouble(word);
               word = line.substr(46,10); H1 = StringUtils::asDouble(word);
               word = line.substr(56,10); H2 = StringUtils::asDouble(word);
               word = line.substr(66, 6); Q1 = StringUtils::asDouble(word);
               word = line.substr(72, 6); Q2 = StringUtils::asDouble(word);
               break;
            case 2:
               if(line.size() < 70) { ok=false; break; }
               word = line.substr( 0,10); tb = StringUtils::asDouble(word);
               word = line.substr(10,10);  I = StringUtils::asDouble(word);
               word = line.substr(20,10);  J = StringUtils::asDouble(word);
               word = line.substr(30,10); K1 = StringUtils::asDouble(word);
               word = line.substr(40,10); K2 = StringUtils::asDouble(word);
               word = line.substr(50,10); K3 = StringUtils::asDouble(word);
               word = line.substr(60,10); K4 = StringUtils::asDouble(word);
               break;
            case 3:
               if(line.size() < 76) { ok=false; break; }
               word = line.substr( 0,10); L1 = StringUtils::asDouble(word);
               word = line.substr(10,10); L2 = StringUtils::asDouble(word);
               word = line.substr(20,10); L3 = StringUtils::asDouble(word);
               word = line.substr(30,10); L4 = StringUtils::asDouble(word);
               word = line.substr(40, 9); R1 = StringUtils::asDouble(word);
               word = line.substr(49, 9); R2 = StringUtils::asDouble(word);
               word = line.substr(58, 9); R3 = StringUtils::asDouble(word);
               word = line.substr(67, 9); R4 = StringUtils::asDouble(word);
               break;
            case 4:
               if(line.size() < 16) { ok=false; break; }
               word = line.substr( 0, 4);  TAIUTC = StringUtils::asInt(word);
               word = line.substr( 4, 5);  SerialNo = StringUtils::asInt(word);
               // this actually integer : mjd of begin valid period
               word = line.substr( 9, 7);  tv = StringUtils::asDouble(word);
               Info = line.substr(16,19);
               break;
         }     // end switch on n=line number
         if(!ok) break;
         n++;
      };
      inpf.close();
      if(!ok) {
         FileMissingException fme("EOPP File " + filename
                                          + " is corrupted or wrong format");
         GPSTK_THROW(fme);
      }
      if(inpf.bad()) return -1;
      return 0;
   }

   //---------------------------------------------------------------------------------
   // generate serial number (NGA files are named EOPP<sn>.txt) from epoch
   // SN = Year (1 digit) + week of year
   int EOPPrediction::getSerialNumber(int imjd) throw(Exception)
   {
      int wk((imjd-GPS_EPOCH_MJD)/7);        // current week
      int w2 = wk-1;                         // the previous week
      if(w2 < 0) { GPSTK_THROW(
         Exception("Invalid week in EOPP file: "+StringUtils::asString<short>(w2))); }

      int yr,w1;
      try {
         long mjdSun = GPS_EPOCH_MJD + w2*7; // Sunday of prev week
         double mjdFri = mjdSun + 5.5;       // Friday noon of previous week

         int mm,dd,dow;
         long jday = MJD_JDAY+mjdSun+5;      // Friday prev week
         convertJDtoCalendar(jday,yr,mm,dd); // get the year
         jday = convertCalendarToJD(yr,1,1); // first day of that year
         jday -= MJD_JDAY + GPS_EPOCH_MJD;   // days since GPS epoch (1 Jan 1980)
         w1 = jday / 7;                      // GPS week of first day of year
         dow = jday - 7*w1;                  // day of week of first day of year
         if(dow == 6) w1++;                  // week of first Friday of year
         yr = yr % 10;                       // last digit of the year
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }

      return (100*yr + w2-w1+1);             // SN = Year (1 digit) + week of year
   }

   //---------------------------------------------------------------------------------
   //                      2                           2
   // xp(t)= A + B(t-ta) + SUM(Cj sin[2pi(t-ta)/Pj]) + SUM(Dj cos[2pi(t-ta)/Pj]) 
   //                     j=1                         j=1
   //
   //                      2                           2
   // yp(t)= E + F(t-ta) + SUM(Gk sin[2pi(t-ta)/Qk]) + SUM(Hk cos[2pi(t-ta)/Qk]) 
   //                     k=1                         k=1
   //
   //                          4                           4
   // UT1-UTC(t)= I+J(t-tb) + SUM(Km sin[2pi(t-tb)/Rm]) + SUM(Lm cos[2pi(t-tb)/Rm])
   //                         m=1                         m=1
   //---------------------------------------------------------------------------------
   EarthOrientation EOPPrediction::computeEOP(double& mjd) const
      throw()
   {
      double dt,arg;
      EarthOrientation eo;

      double t = mjd;
      //t = ep.MJD() + ep.secOfDay()/86400.0;
      //if(t < tv || t > tv+7) // TD warn - outside valid range
      //
      dt = t - ta;
      arg = TWO_PI*dt;
      eo.xp = A + B*dt + C1 * ::sin(arg/P1) + D1 * ::cos(arg/P1)
                       + C2 * ::sin(arg/P2) + D2 * ::cos(arg/P2);
      eo.yp = E + F*dt + G1 * ::sin(arg/Q1) + H1 * ::cos(arg/Q1)
                       + G2 * ::sin(arg/Q2) + H2 * ::cos(arg/Q2);

      dt = t - tb;
      arg = TWO_PI*dt;
      eo.UT1mUTC = I + J*dt
         + K1 * ::sin(arg/R1) + L1 * ::cos(arg/R1)
         + K2 * ::sin(arg/R2) + L2 * ::cos(arg/R2)
         + K3 * ::sin(arg/R3) + L3 * ::cos(arg/R3)
         + K4 * ::sin(arg/R4) + L4 * ::cos(arg/R4);

      return eo;
   }

   //---------------------------------------------------------------------------------
   // straight from the doc
   ostream& operator<<(ostream& os, const EOPPrediction& eopp)
   {
      os << fixed
         << setw(10) << setprecision(2) << eopp.ta
         << setw(10) << setprecision(6) << eopp.A
         << setw(10) << setprecision(6) << eopp.B
         << setw(10) << setprecision(6) << eopp.C1
         << setw(10) << setprecision(6) << eopp.C2
         << setw(10) << setprecision(6) << eopp.D1
         << setw(10) << setprecision(6) << eopp.D2
         << setw( 6) << setprecision(2) << eopp.P1
         << "    " << endl;
      os << setw( 6) << setprecision(2) << eopp.P2
         << setw(10) << setprecision(6) << eopp.E
         << setw(10) << setprecision(6) << eopp.F
         << setw(10) << setprecision(6) << eopp.G1
         << setw(10) << setprecision(6) << eopp.G2
         << setw(10) << setprecision(6) << eopp.H1
         << setw(10) << setprecision(6) << eopp.H2
         << setw( 6) << setprecision(2) << eopp.Q1
         << setw( 6) << setprecision(2) << eopp.Q2
         << "  " << endl;
      os << setw(10) << setprecision(2) << eopp.tb
         << setw(10) << setprecision(6) << eopp.I
         << setw(10) << setprecision(6) << eopp.J
         << setw(10) << setprecision(6) << eopp.K1
         << setw(10) << setprecision(6) << eopp.K2
         << setw(10) << setprecision(6) << eopp.K3
         << setw(10) << setprecision(6) << eopp.K4
         << "          " << endl;
      os << setw(10) << setprecision(6) << eopp.L1
         << setw(10) << setprecision(6) << eopp.L2
         << setw(10) << setprecision(6) << eopp.L3
         << setw(10) << setprecision(6) << eopp.L4
         << setw( 9) << setprecision(4) << eopp.R1
         << setw( 9) << setprecision(4) << eopp.R2
         << setw( 9) << setprecision(4) << eopp.R3
         << setw( 9) << setprecision(4) << eopp.R4
         << "    " << endl;
      os << setw(4) << eopp.TAIUTC
         << setw(5) << eopp.SerialNo
         << setw(6) << int(eopp.tv+0.5)
         << " " << eopp.Info
         << "                    "
         << "                    "
         << "      ";
      return os;
   }

} // end namespace gpstk

