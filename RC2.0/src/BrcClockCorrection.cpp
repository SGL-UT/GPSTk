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

/**
 * @file BrcClockCorrection.cpp
 * Ephemeris data encapsulated in engineering terms
 */

#include "StringUtils.hpp"
#include "GNSSconstants.hpp"
#include "BrcClockCorrection.hpp"
#include <cmath>

namespace gpstk
{
   using namespace std;
   using namespace gpstk;

   BrcClockCorrection::BrcClockCorrection()
      throw()
   {
      dataLoaded = false;

      PRNID = weeknum = 0;

      satSys = "";

      healthy = false;     
 
      Toc = af0 = af1 = af2 = accuracy = 0.0;
   }

   BrcClockCorrection::BrcClockCorrection(const std::string satSysArg, const ObsID obsIDArg,
                                          const short PRNIDArg, const double TocArg,
                                          const short weeknumArg, const double accuracyArg,
                                          const bool healthyArg, const double af0Arg,
                                          const double af1Arg, const double af2Arg )
   {
      loadData(satSysArg, obsIDArg, PRNIDArg, TocArg, weeknumArg, accuracyArg, healthyArg,
		         af0Arg, af1Arg, af2Arg );
   }

		/// Legacy GPS Subframe 1-3  
   BrcClockCorrection::BrcClockCorrection(const ObsID obsIDArg, const short PRNID,
                                          const short fullweeknum, const long subframe1[10] )
   {
      loadData(obsIDArg, PRNID,fullweeknum,subframe1 );
   }

   void BrcClockCorrection::loadData(const std::string satSysArg, const ObsID obsIDArg,
                                     const short PRNIDArg, const double TocArg,
                                     const short weeknumArg, const double accuracyArg,
                                     const bool healthyArg, const double af0Arg,
                                     const double af1Arg, const double af2Arg )
   {
	   satSys      = satSysArg;
	   obsID       = obsIDArg;
	   PRNID       = PRNIDArg;
	   Toc         = TocArg;
	   weeknum     = weeknumArg;
	   accuracy    = accuracyArg;
	   healthy     = healthyArg;
	   af0         = af0Arg;
	   af1         = af1Arg;
	   af2         = af2Arg;
	   dataLoaded  = true;
   }

   void BrcClockCorrection::loadData(const ObsID obsIDArg, const short PRNIDArg,
                                     const short fullweeknum, const long subframe1[10] )
	   throw(InvalidParameter)
   {
      double ficked[60];

 	      //Load overhead members
  	   satSys = "G";
	   obsID = obsIDArg;
      PRNID = PRNIDArg;

         //Convert Subframe 1
      if (!subframeConvert(subframe1, fullweeknum, ficked))
      {
         InvalidParameter exc("Subframe 1 not valid.");
	      GPSTK_THROW(exc);
      }
      weeknum       = static_cast<short>( ficked[5] );
      short accFlag = static_cast<short>( ficked[7] );
      short health  = static_cast<short>( ficked[8] );
      Toc           = ficked[12];
      af2           = ficked[13];
      af1           = ficked[14];
      af0           = ficked[15];
         //Convert the accuracy flag to a value...
      accuracy = gpstk::ura2accuracy(accFlag);
      healthy = false;
      if (health == 0)
      healthy = true;
      dataLoaded = true;	 
      return;
   }
	     
   bool BrcClockCorrection::hasData() const
   {
      return(dataLoaded);
   }

   CommonTime BrcClockCorrection::getEpochTime() const
      throw(InvalidRequest)
   {
      CommonTime toReturn;
      if (satSys == "G" )
         toReturn = GPSWeekSecond(weeknum, Toc, TimeSystem::GPS);
      else if (satSys == "E" )
         toReturn = GPSWeekSecond(weeknum, Toc, TimeSystem::GAL);
      else
      {
         InvalidRequest exc("Invalid Time System in BrcClockCorrection::getEpochTime()");
         GPSTK_THROW(exc);
      }
      return toReturn;
   }

   double BrcClockCorrection::svClockBias(const CommonTime& t) const
      throw(gpstk::InvalidRequest)
   {
      double dtc,elaptc;
      elaptc = t - getEpochTime();
      dtc = af0 + elaptc * ( af1 + elaptc * af2 );
      return dtc;
   }

   double BrcClockCorrection::svClockBiasM(const CommonTime& t) const
      throw(gpstk::InvalidRequest)
   {
      double ret = svClockBias(t);
      ret = ret*C_MPS;
      return (ret);
   }

   double BrcClockCorrection::svClockDrift(const CommonTime& t) const
      throw(gpstk::InvalidRequest)
   {
      double drift,elaptc;
      elaptc = t - getEpochTime();
      drift = af1 + elaptc * af2;
      return drift;
   }

   short BrcClockCorrection::getPRNID() const
      throw(InvalidRequest)
   {
      if(!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return PRNID;
   }
     
   short BrcClockCorrection::getFullWeek()  const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return weeknum;
   }
     
   double BrcClockCorrection::getAccuracy()  const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return accuracy;
   }   

   double BrcClockCorrection::getToc() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return Toc;
   }

   double BrcClockCorrection::getAf0() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return af0;
   }

   double BrcClockCorrection::getAf1() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return af1;
   }

   double BrcClockCorrection::getAf2() const
      throw(InvalidRequest)
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GPSTK_THROW(exc);
      }
      return af2;
   }

   static void timeDisplay( ostream & os, const CommonTime& t )
   {
         // Convert to CommonTime struct from GPS wk,SOW to M/D/Y, H:M:S.
      GPSWeekSecond dummyTime;
      dummyTime = GPSWeekSecond(t);
      os << setw(4) << dummyTime.week << "(";
      os << setw(4) << (dummyTime.week & 0x03FF) << ")  ";
      os << setw(6) << setfill(' ') << dummyTime.sow << "   ";

      switch (dummyTime.getDayOfWeek())
      {
         case 0: os << "Sun-0"; break;
         case 1: os << "Mon-1"; break;
         case 2: os << "Tue-2"; break;
         case 3: os << "Wed-3"; break;
         case 4: os << "Thu-4"; break;
         case 5: os << "Fri-5"; break;
         case 6: os << "Sat-6"; break;
         default: break;
      }
      os << "   " << (static_cast<YDSTime>(t)).printf("%3j   %5.0s  ") 
         << (static_cast<CivilTime>(t)).printf("%02m/%02d/%04Y   %02H:%02M:%02S");
   }

   static void shortcut(ostream & os, const long HOW )
   {
      short DOW, hour, min, sec;
      long SOD, SOW;
      short SOH;
      
      SOW = static_cast<long>( HOW );
      DOW = static_cast<short>( SOW / SEC_PER_DAY );
      SOD = SOW - static_cast<long>( DOW * SEC_PER_DAY );
      hour = static_cast<short>( SOD/3600 );

      SOH = static_cast<short>( SOD - (hour*3600) );
      min = SOH/60;

      sec = SOH - min * 60;
      switch (DOW)
      {
         case 0: os << "Sun-0"; break;
         case 1: os << "Mon-1"; break;
         case 2: os << "Tue-2"; break;
         case 3: os << "Wed-3"; break;
         case 4: os << "Thu-4"; break;
         case 5: os << "Fri-5"; break;
         case 6: os << "Sat-6"; break;
         default: break;
      }

      os << ":" << setfill('0')
         << setw(2) << hour
         << ":" << setw(2) << min
         << ":" << setw(2) << sec
         << setfill(' ');
   }

   void BrcClockCorrection::dump(ostream& s) const
      throw()
   {
      ios::fmtflags oldFlags = s.flags();
   
      s.setf(ios::fixed, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.setf(ios::uppercase);
      s.precision(0);
      s.fill(' ');
      
      s << "****************************************************************"
        << "************" << endl
        << "Broadcast Ephemeris (Engineering Units)" << endl
        << endl
        << "PRN : " << setw(2) << PRNID << endl
        << endl;
  
      s << "              Week(10bt)     SOW     DOW   UTD     SOD"
        << "  MM/DD/YYYY   HH:MM:SS\n";
      s << "Clock Epoch:  ";

      timeDisplay(s, getEpochTime());
      s << endl;
        
      s.setf(ios::scientific, ios::floatfield);
      s.precision(11);
      
      s << endl
        << "           CLOCK"
        << endl
        << endl
        << "Bias T0:     " << setw(18) << af0 << " sec" << endl
        << "Drift:       " << setw(18) << af1 << " sec/sec" << endl
        << "Drift rate:  " << setw(18) << af2 << " sec/(sec**2)" << endl;

      s << "****************************************************************"
        << "************" << endl;                         
   }
   
   ostream& operator<<(ostream& s, const BrcClockCorrection& eph)
   {
      eph.dump(s);
      return s;
   } // end of operator<<             
       
} // namespace
