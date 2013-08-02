/// TimeSystem.cpp

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

#include "TimeSystem.hpp"
#include "TimeConverters.hpp"

using namespace std;

namespace gpstk
{
   // Static initialization of const std::strings for asString().
   // Must parallel enum Systems in TimeSystem.hpp.
   // NB: DO NOT use std::map here; on some systems initialization fails.
   const string TimeSystem::Strings[count] =
     {
       string("UNK"),
       string("Any"),
       string("GPS"),
       string("GLO"),
       string("GAL"),
       string("QZS"),
       string("BDS"),
       string("UTC"),
       string("TAI"),
       string("TRT"),
     };

   void TimeSystem::setTimeSystem(const Systems& sys)
   {
      if(sys < 0 || sys >= count)
         system = Unknown;
      else
         system = sys;
   }

   void TimeSystem::fromString(const string str)

   {
      system = Unknown;
      for(int i=0; i<count; i++) {
         if(Strings[i] == str) {
            system = static_cast<Systems>(i);
            break;
         }
      }
   }

   ostream& operator<<(ostream os, const TimeSystem& ts)
   {
      return os << ts.asString();
   }

   // NB. The table 'leaps' must be modified when a new leap second is announced.
   // Return the number of leap seconds between UTC and TAI, that is the
   // difference in time scales UTC-TAI at an epoch defined by (year, month, day).
   // NB. Input day in a floating quantity and thus any epoch may be represented;
   // this is relevant the period 1960 to 1972, when UTC-TAI was not integral.
   // NB. GPS = TAI - 19sec and so GPS-UTC = getLeapSeconds()-19.
   double TimeSystem::getLeapSeconds(const int& year,
                                     const int& month,
                                     const double& day)
   {
      // Leap second data --------------------------------------------------------
      // number of changes before leap seconds (1960-1971) - this should never change.
      static const int NPRE=14;

      // epoch year, epoch month(1-12), delta t(sec), rate (sec/day) for [1960,1972).
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
         unsigned int year, month;
         int nleap;
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
         { 2012,  7, 35 }, // leave the last comma!
         // add new entry here, of the form:
         // { year, month(1-12), leap_sec }, // leave the last comma!
      };

      // the number of leaps (do not change this)
      static const int NLEAPS = sizeof(leaps)/sizeof(leaps[0]);

      // last year in leaps
      static const int MAXYEAR = leaps[NLEAPS-1].year;

      // END static data -----------------------------------------------------

      // search for the input year, month
      if(year < 1960)                        // pre-1960 no deltas
         ;
      else if(month < 1 || month > 12)       // blunder, should never happen - throw?
         ;
      else if(year < 1972) {                 // [1960-1972) pre-leap
         for(int i=NPRE-1; i>=0; i--) {
            if(preleap[i].year > year ||
               (preleap[i].year == year && preleap[i].month > month)) continue;

            // found last record with < rec.year >= year and rec.month >= month
            // watch out - cannot use CommonTime here
            int iday(day);
            double dday(iday-int(day));
            if(iday == 0) { iday = 1; dday = 1.0-dday; }
            long JD0 = convertCalendarToJD(year,month,iday);
            long JD = convertCalendarToJD(preleap[i].year,preleap[i].month,1);
            return (preleap[i].delt + (double(JD0-JD)+dday)*preleap[i].rate);
         }
      }
      else {                                    // [1972- leap seconds
         for(int i=NLEAPS-1; i>=0; i--) {
            if(leaps[i].year > year ||
               (leaps[i].year == year && leaps[i].month > month)) continue;
            return double(leaps[i].nleap);
         }
      }

      return 0.0;
   }

}   // end namespace
