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

/**
 * @file EarthOrientation.cpp
 * Implementation of classes to handle earth orientation data:
 * class gpstk::EarthOrientation encapsulates Earth orientation parameters (EOPs)
 * class gpstk::EOPPrediction encapsulates formulas to predict EOPs; created from
 *  data obtained by reading NGA 'EOPP###.txt' files.
 * class gpstk::EOPStore encapsulates input, storage and retreval of EOPs.
 */

//------------------------------------------------------------------------------------
// system includes
#include <fstream>
#include <cmath>
// GPSTk
#include "GNSSconstants.hpp"    // for TWO_PI
#include "EarthOrientation.hpp"
#include "GPSWeekSecond.hpp"
#include "CivilTime.hpp"
#include "YDSTime.hpp"
#include "TimeString.hpp"

//------------------------------------------------------------------------------------
using namespace std;

namespace gpstk
{
   //---------------------------------------------------------------------------------
   ostream& operator<<(ostream& os, const EarthOrientation& eo)
   {
      os << " " << setw(17) << setprecision(6) << eo.xp
         << " " << setw(17) << setprecision(6) << eo.yp
         << " " << setw(17) << setprecision(7) << eo.UT1mUTC;
      return os;
   }

   //---------------------------------------------------------------------------------
   // class EOPPrediction
   //---------------------------------------------------------------------------------
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
   int EOPPrediction::getSerialNumber(CommonTime& t)
      throw(Exception)
   {
      int w2 = static_cast<GPSWeekSecond>(t).week-1;            // the previous week
      if(w2 < 0) {
         using namespace StringUtils;
	 Exception dte("Invalid week in EOPP file: "
               + asString<short>(w2));
         GPSTK_THROW(dte);
      }

      int yr,w1;
      try {
         CommonTime ht;
         ht=GPSWeekSecond(w2,475200.0);     // Friday (noon) of previous week
         yr = static_cast<YDSTime>(ht).year;                     // save the year for later
         ht=CivilTime(yr,1,1,0,0,0.0);       // first day of that year
         w1 = static_cast<GPSWeekSecond>(ht).week;
         if(static_cast<GPSWeekSecond>(ht).getDayOfWeek() == 6) w1++; // GPS week of first Friday in the year
         yr = yr % 10;                       // last digit of the year
      }
      catch(Exception& dte) {
         GPSTK_RETHROW(dte);
      }
      return (100*yr + w2-w1+1);             // SN = Year (1 digit) + week of year
   }

   //---------------------------------------------------------------------------------
   // Compute the Earth orientation parameters at the given epoch.
   // TD how to warn if input is outside limits of validity?
   EarthOrientation EOPPrediction::computeEOP(int& mjd) const
      throw(Exception)
   {
      CommonTime t;
      try { t=MJD(double(mjd)); }
      catch(Exception& dte) { GPSTK_RETHROW(dte); }
      return computeEOP(t);
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
   EarthOrientation EOPPrediction::computeEOP(CommonTime& ep) const
      throw()
   {
      double t,dt,arg;
      EarthOrientation eo;

      t = static_cast<MJD>(ep).mjd + static_cast<YDSTime>(ep).sod/86400.0;
      //if(t < tv || t > tv+7) // TD warn - outside valid range
      //
      dt = t - ta;
      arg = TWO_PI*dt;
      eo.xp = A + B*dt + C1*sin(arg/P1) + D1*cos(arg/P1)
                       + C2*sin(arg/P2) + D2*cos(arg/P2);
      eo.yp = E + F*dt + G1*sin(arg/Q1) + H1*cos(arg/Q1)
                       + G2*sin(arg/Q2) + H2*cos(arg/Q2);

      dt = t - tb;
      arg = TWO_PI*dt;
      eo.UT1mUTC = I + J*dt
         + K1*sin(arg/R1) + L1*cos(arg/R1)
         + K2*sin(arg/R2) + L2*cos(arg/R2)
         + K3*sin(arg/R3) + L3*cos(arg/R3)
         + K4*sin(arg/R4) + L4*cos(arg/R4);

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

   //---------------------------------------------------------------------------------
   // class EOPStore
   //---------------------------------------------------------------------------------
   // Add to the store directly -- not recommended,
   // use the form that takes EOPPrediction
   void EOPStore::addEOP(int mjd, EarthOrientation& eop)
      throw()
   {
      mapMJD_EOP[mjd] = eop;

      if(begMJD == -1 || endMJD == -1) {
         begMJD = endMJD = mjd;
      }
      else if(mjd < begMJD) {
         begMJD = mjd;
      }
      else if(mjd > endMJD) {
         endMJD = mjd;
      }
   }

   //---------------------------------------------------------------------------------
   // Add to the store by computing using an EOPPrediction,
   // this is the usual way.
   // @param MJD integer MJD at which to add EOPs
   // @return non-0 if MJD is outside range
   int EOPStore::addEOP(int mjd, EOPPrediction& eopp)
      throw(Exception)
   {
      EarthOrientation eo;
      try {
         eo = eopp.computeEOP(mjd);
      }
      catch(Exception& dte)
      {
         GPSTK_RETHROW(dte);
      }

      addEOP(mjd,eo);

      return 0;
   }

   //---------------------------------------------------------------------------------
   // Add EOPs to the store via an inpu file: either an EOPP file
   // or a flat file produced by USNO (see http://maia.usno.navy.mil/
   // and get either file 'finals.data' or finals2000A.data').
   // @param filename Name of file to read, including path.
   void EOPStore::addFile(const string& filename)
      throw(FileMissingException)
   {
      try {
         addEOPPFile(filename);
      }
      catch(FileMissingException& fme)
      {
         if(StringUtils::matches(fme.getText(),string("wrong format")).empty()) {
            GPSTK_RETHROW(fme);
         }

         // try other format
         try {
            addIERSFile(filename);
         }
         catch(FileMissingException& fme)
         {
            GPSTK_RETHROW(fme);
         }
      }
   }

   //---------------------------------------------------------------------------------
   // Add EOPs to the store via an EOPP file:
   // read the EOPPrediction from the file and then compute EOPs
   // for all days within the valid range.
   // @param filename Name of file to read, including path.
   void EOPStore::addEOPPFile(const string& filename)
      throw(FileMissingException)
   {
      // read the file into an EOPPrediction
      EOPPrediction eopp;
      try {
         eopp.loadFile(filename);
      }
      catch(FileMissingException& fme)
      {
         GPSTK_RETHROW(fme);
      }

      // pull out the beginning of the valid time range
      int mjd;
      mjd = eopp.getValidTime();
      // add all 7 days
      for(int i=0; i<7; i++) {
         EarthOrientation eo;
         eo = eopp.computeEOP(mjd);
         addEOP(mjd,eo);
         mjd++;
      }
   }

   //---------------------------------------------------------------------------------
   // see http://maia.usno.navy.mil/readme.finals
   void EOPStore::addIERSFile(const string& filename)
      throw(FileMissingException)
   {
      bool ok;
      int mjd;
      string line,word;

      ifstream inpf(filename.c_str());
      if(!inpf) {
         FileMissingException fme("Could not open IERS file " + filename);
         GPSTK_THROW(fme);
      }

      ok = true;
      while(!inpf.eof() && inpf.good()) {
         getline(inpf,line);
         StringUtils::stripTrailing(line,'\r');
         if(inpf.eof()) break;
            // line length is actually 187
         if(inpf.bad() || line.size() < 70) { ok = false; break; }
         EarthOrientation eo;
         mjd = StringUtils::asInt(line.substr(7,5));
         eo.xp = StringUtils::asDouble(line.substr(18,9));       // arcseconds
         eo.yp = StringUtils::asDouble(line.substr(37,9));       // arcseconds
         eo.UT1mUTC = StringUtils::asDouble(line.substr(58,10)); // seconds

         addEOP(mjd,eo);
      };
      inpf.close();

      if(!ok) {
         FileMissingException fme("IERS File " + filename
            + " is corrupted or wrong format");
         GPSTK_THROW(fme);
      }
   }

   //---------------------------------------------------------------------------------
   // Edit the store by deleting all entries before(after)
   //  the given min(max) MJDs. If mjdmin is later than mjdmax,
   //  the two times are switched.
   //  @param mjdmin integer MJD desired earliest store time.
   //  @param mjdmax integer MJD desired latest store time.
   void EOPStore::edit(int mjdmin, int mjdmax)
      throw()
   {
      if(mjdmin > mjdmax) {
         int m=mjdmin;
         mjdmin = mjdmax;
         mjdmax = m;
      }

      if(mjdmin > endMJD) return;
      if(mjdmax < begMJD) return;

      map<int,EarthOrientation>::iterator it;
      it = mapMJD_EOP.lower_bound(mjdmin);
      if(it != mapMJD_EOP.begin())
         mapMJD_EOP.erase(mapMJD_EOP.begin(), it);

      it = mapMJD_EOP.upper_bound(mjdmax);
      if(it != mapMJD_EOP.end())
         mapMJD_EOP.erase(it, mapMJD_EOP.end());

      it = mapMJD_EOP.begin();
      if(it == mapMJD_EOP.end())
         begMJD = -1;
      else
         begMJD = it->first;

      it = mapMJD_EOP.end();
      if(--it == mapMJD_EOP.end())
         endMJD = -1;
      else
         endMJD = it->first;
   }

   //---------------------------------------------------------------------------------
   // Dump the store to cout.
   // @param detail determines how much detail to include in the output
   //   0 start and stop times (MJD), and number of EOPs.
   //   1 list of all times and EOPs.
   void EOPStore::dump(short detail, ostream& os) const
      throw()
   {
      CommonTime tt;
      os << "EOPStore dump (" << mapMJD_EOP.size() << " entries):\n";
      os << " Time limits: [MJD " << begMJD << " - " << endMJD << "]";
      tt=MJD(double(begMJD));
      os << " = [m/d/y " << printTime(tt,"%m/%d/%Y");
      tt=MJD(double(endMJD));
      os << " - " << printTime(tt,"%m/%d/%Y") << "]" << endl;
      if(detail > 0) {
         int lastmjd=-1;
         map<int,EarthOrientation>::const_iterator it;
         for(it=mapMJD_EOP.begin(); it != mapMJD_EOP.end(); it++) {
            if(lastmjd != -1 && it->first - lastmjd > 1)
               os << " ....." << endl;
            os << " " << it->first << " " << it->second
               << "     (" << setfill('0') << setw(3)
               << EOPPrediction::getSerialNumber(it->first) << setfill(' ') << ")"
               << endl;
            lastmjd = it->first;
         }
      }
   }

   //---------------------------------------------------------------------------------
   // Get the EOP at the given epoch and return it.
   // @param t CommonTime at which to compute the EOPs.
   // @return EarthOrientation EOPs at time t.
   // @throw InvalidRequest if the (int) MJDs on either side of t
   // cannot be found in the map.
   EarthOrientation EOPStore::getEOP(CommonTime& t) const
      throw(InvalidRequest)
   {
         // find the EOs before and after epoch
      int loMJD = int(static_cast<MJD>(t).mjd);
      int hiMJD = loMJD + 1;
         // find these EOPs
      map<int,EarthOrientation>::const_iterator hit,lit;
      lit = mapMJD_EOP.find(loMJD);
      hit = mapMJD_EOP.find(hiMJD);
      if(lit == mapMJD_EOP.end() || hit == mapMJD_EOP.end()) {
         InvalidRequest ire(string("Time tag (MJD=")
         + (lit == mapMJD_EOP.end() ?
               StringUtils::asString(loMJD) : StringUtils::asString(hiMJD))
         + string(") not found within the EOP store - EOPP files are out-of-date"));
         GPSTK_THROW(ire);
      }
         // linearly interpolate to get EOP at the desired time
      EarthOrientation eo;
      double dt=static_cast<MJD>(t).mjd-double(loMJD);
      eo.xp = (1.0-dt) * lit->second.xp + dt * hit->second.xp;
      eo.yp = (1.0-dt) * lit->second.yp + dt * hit->second.yp;
      eo.UT1mUTC = (1.0-dt) * lit->second.UT1mUTC + dt * hit->second.UT1mUTC;

      return eo;
   }

} // end namespace gpstk
//------------------------------------------------------------------------------------
