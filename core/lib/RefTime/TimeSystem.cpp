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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#include <cmath>
#include "TimeSystem.hpp"
#include "TimeConverters.hpp"
#include "Exception.hpp"

using namespace std;

namespace gpstk
{
   ostream& operator<<(ostream& os, const TimeSystem ts)
   {
      return os << StringUtils::asString(ts);
   }

   double getLeapSeconds(const int year,
                         const int month,
                         const double day)
   {
         // Leap second data
         // number of changes before leap seconds (1960-1971) - this
         // should never change.
      static const int NPRE=14;

         // epoch year, epoch month(1-12), delta t(sec), rate
         // (sec/day) for [1960,1972).
      static const struct {
         int year, month;
         double delt, rate;
      } preleap[NPRE] = {
         { 1960,  1,  1.4178180, 0.0012960 },
         { 1961,  1,  1.4228180, 0.0012960 },
         { 1961,  8,  1.3728180, 0.0012960 },
         { 1962,  1,  1.8458580, 0.0011232 },
         { 1963, 11,  1.9458580, 0.0011232 },
         { 1964,  1,  3.2401300, 0.0012960 },
         { 1964,  4,  3.3401300, 0.0012960 },
         { 1964,  9,  3.4401300, 0.0012960 },
         { 1965,  1,  3.5401300, 0.0012960 },
         { 1965,  3,  3.6401300, 0.0012960 },
         { 1965,  7,  3.7401300, 0.0012960 },
         { 1965,  9,  3.8401300, 0.0012960 },
         { 1966,  1,  4.3131700, 0.0025920 },
         { 1968,  2,  4.2131700, 0.0025920 }
      };

         // Leap seconds history
         // ***** This table must be updated for new leap seconds **************
      static const struct {
         int year, month, nleap;
      } leaps[] = {
         { 1972,  1, 10 },
         { 1972,  7, 11 },
         { 1973,  1, 12 },
         { 1974,  1, 13 },
         { 1975,  1, 14 },
         { 1976,  1, 15 },
         { 1977,  1, 16 },
         { 1978,  1, 17 },
         { 1979,  1, 18 },
         { 1980,  1, 19 },
         { 1981,  7, 20 },
         { 1982,  7, 21 },
         { 1983,  7, 22 },
         { 1985,  7, 23 },
         { 1988,  1, 24 },
         { 1990,  1, 25 },
         { 1991,  1, 26 },
         { 1992,  7, 27 },
         { 1993,  7, 28 },
         { 1994,  7, 29 },
         { 1996,  1, 30 },
         { 1997,  7, 31 },
         { 1999,  1, 32 },
         { 2006,  1, 33 },
         { 2009,  1, 34 },
         { 2012,  7, 35 },
         { 2015,  7, 36 }, 
         { 2017,  1, 37 }, // leave the last comma!
            // add new entry here, of the form:
            // { year, month(1-12), leap_sec }, // leave the last comma!
      };

         // the number of leaps (do not change this)
      static const int NLEAPS = sizeof(leaps)/sizeof(leaps[0]);

         // last year in leaps
         //static const int MAXYEAR = leaps[NLEAPS-1].year;

         // END static data

         // search for the input year, month
      if (year < 1960)
      {
            // pre-1960 no deltas
      }
      else if (month < 1 || month > 12)
      {
            // blunder, should never happen - throw?
      }
      else if (year < 1972)
      {
            // [1960-1972) pre-leap
         for (int i=NPRE-1; i>=0; i--)
         {
            if (preleap[i].year > year ||
                (preleap[i].year == year && preleap[i].month > month))
            {
               continue;
            }

               // found last record with < rec.year >= year and
               // rec.month >= month
               // watch out - cannot use CommonTime here
            int iday(static_cast<int>(day));
            double dday(static_cast<double>(iday-int(day)));
            if (iday == 0)
            {
               iday = 1;
               dday = 1.0-dday;
            }
            long JD0 = convertCalendarToJD(year,month,iday);
            long JD = convertCalendarToJD(preleap[i].year,preleap[i].month,1);
            return (preleap[i].delt + (double(JD0-JD)+dday)*preleap[i].rate);
         }
      }
      else
      {                                    // [1972- leap seconds
         for (int i=NLEAPS-1; i>=0; i--)
         {
            if (leaps[i].year > year ||
                (leaps[i].year == year && leaps[i].month > month))
            {
               continue;
            }
            return double(leaps[i].nleap);
         }
      }

      return 0.0;
   }


   double getTimeSystemCorrection(const TimeSystem inTS,
                                  const TimeSystem outTS,
                                  const int year,
                                  const int month,
                                  const double day)
   {
      double dt(0.0);

         // identity
      if (inTS == outTS)
         return dt;

         // cannot convert unknowns
      if (inTS == TimeSystem::Unknown || outTS == TimeSystem::Unknown)
      {
         Exception e("Cannot compute correction for TimeSystem::Unknown");
         GPSTK_THROW(e);
      }

         // compute TT-TDB here; ref Astronomical Almanac B7
      double TDBmTT(0.0);
      if (inTS == TimeSystem::TDB || outTS == TimeSystem::TDB)
      {
         int iday = int(day);
         long jday = convertCalendarToJD(year, month, iday) ;
         double frac(day-iday);
         double TJ2000(jday-2451545.5+frac);     // t-J2000
            //       0.0001657 sec * sin(357.53 + 0.98560028 * TJ2000 deg)
         frac = ::fmod(0.017201969994578 * TJ2000, 6.2831853071796);
         TDBmTT = 0.0001657 * ::sin(6.240075674 + frac);
            //        0.000022 sec * sin(246.11 + 0.90251792 * TJ2000 deg)
         frac = ::fmod(0.015751909262251 * TJ2000, 6.2831853071796);
         TDBmTT += 0.000022  * ::sin(4.295429822 + frac);
      }

         // Time system conversions constants
      static const double TAI_minus_GPSGAL_EPOCH = 19.;
      static const double TAI_minus_BDT_EPOCH = 33.;
      static const double TAI_minus_TT_EPOCH = -32.184;
      
         // -----------------------------------------------------------
         // conversions: first convert inTS->TAI ...
         // TAI = GPS + 19s
         // TAI = UTC + getLeapSeconds()
         // TAI = TT - 32.184s
      if (inTS == TimeSystem::GPS ||       // GPS -> TAI
          inTS == TimeSystem::GAL ||       // GAL -> TAI
          inTS == TimeSystem::IRN )        // IRN -> TAI
      {
         dt = TAI_minus_GPSGAL_EPOCH;
      }
      else if (inTS == TimeSystem::UTC ||  // UTC -> TAI
               inTS == TimeSystem::GLO)    // GLO -> TAI
      {
         dt = getLeapSeconds(year, month, day);
      }
      else if (inTS == TimeSystem::BDT)    // BDT -> TAI
      {
         dt = TAI_minus_BDT_EPOCH;
      }
      else if (inTS == TimeSystem::TAI)    // TAI
      {
      }
      else if (inTS == TimeSystem::TT)     // TT -> TAI
      {
         dt = TAI_minus_TT_EPOCH;
      }
      else if (inTS == TimeSystem::TDB)    // TDB -> TAI
      {
         dt = TAI_minus_TT_EPOCH + TDBmTT;
      }
      else
      {                              // other
         Exception e("Invalid input TimeSystem " + StringUtils::asString(inTS));
         GPSTK_THROW(e);
      }

         // -----------------------------------------------------------
         // ... then convert TAI->outTS
         // GPS = TAI - 19s
         // UTC = TAI - getLeapSeconds()
         // TT = TAI + 32.184s
      if (outTS == TimeSystem::GPS ||      // TAI -> GPS
          outTS == TimeSystem::GAL ||      // TAI -> GAL
          outTS == TimeSystem::IRN )       // TAI -> IRN
      {
         dt -= TAI_minus_GPSGAL_EPOCH;
      }
      else if (outTS == TimeSystem::UTC || // TAI -> UTC
               outTS == TimeSystem::GLO)   // TAI -> GLO
      {
         dt -= getLeapSeconds(year, month, day);
      }
      else if (outTS == TimeSystem::BDT)   // TAI -> BDT
      {
         dt -= TAI_minus_BDT_EPOCH;
      }
      else if (outTS == TimeSystem::TAI)   // TAI
      {
      }
      else if (outTS == TimeSystem::TT)    // TAI -> TT
      {
         dt -= TAI_minus_TT_EPOCH;
      }
      else if (outTS == TimeSystem::TDB)   // TAI -> TDB
      {
         dt -= TAI_minus_TT_EPOCH + TDBmTT;
      }
      else
      {                              // other
         Exception e("Invalid output TimeSystem "+StringUtils::asString(outTS));
         GPSTK_THROW(e);
      }

      return dt;
   }


   namespace StringUtils
   {
      std::string asString(TimeSystem ts)
      {
         switch (ts)
         {
            case TimeSystem::Unknown: return "UNK";
            case TimeSystem::Any: return "Any";
            case TimeSystem::GPS: return "GPS";
            case TimeSystem::GLO: return "GLO";
            case TimeSystem::GAL: return "GAL";
            case TimeSystem::QZS: return "QZS";
            case TimeSystem::BDT: return "BDT";
            case TimeSystem::IRN: return "IRN";
            case TimeSystem::UTC: return "UTC";
            case TimeSystem::TAI: return "TAI";
            case TimeSystem::TT:  return "TT";
            case TimeSystem::TDB: return "TDB";
            default:              return "???";
         }
      }


      TimeSystem asTimeSystem(const std::string& s)
      {
         if (s == "UNK") return TimeSystem::Unknown;
         if (s == "Any") return TimeSystem::Any;
         if (s == "GPS") return TimeSystem::GPS;
         if (s == "GLO") return TimeSystem::GLO;
         if (s == "GAL") return TimeSystem::GAL;
         if (s == "QZS") return TimeSystem::QZS;
         if (s == "BDT") return TimeSystem::BDT;
         if (s == "IRN") return TimeSystem::IRN;
         if (s == "UTC") return TimeSystem::UTC;
         if (s == "TAI") return TimeSystem::TAI;
         if (s == "TT")  return TimeSystem::TT;
         if (s == "TDB") return TimeSystem::TDB;
         return TimeSystem::Unknown;
      }
   }

}   // end namespace
