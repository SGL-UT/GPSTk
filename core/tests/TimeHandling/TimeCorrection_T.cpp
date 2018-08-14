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
//  Copyright 2018, The University of Texas at Austin
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

#include "TimeCorrection.hpp"
#include "GPSWeekZcount.hpp"
#include "SystemTime.hpp"
#include "TestUtil.hpp"

using namespace std;
using namespace gpstk;

class TimeCorrection_T
{
public:
   unsigned testEpochRollover();
};


unsigned TimeCorrection_T :: testEpochRollover()
{
   TUDEF("TestCorrection", "timeAdjustWeekRollover");
   long refWeek = 123;
      // week   year  week   year
      //  123 = 1982   635 = 1992
      // 1147 = 2001  1659 = 2011
      // 2171 = 2021  2683 = 2031
      // 3195 = 2041
      // test same week with 10-bit weeks
   TUASSERTE(long, 123, timeAdjustWeekRollover(123, refWeek));
   TUASSERTE(long, 123, refWeek);
   refWeek = 128;
      // test slightly different week with 10-bit weeks
   TUASSERTE(long, 123, timeAdjustWeekRollover(123, refWeek));
   TUASSERTE(long, 128, refWeek);
      // test 10-bit week edge cases
   refWeek = 123 + 512;
   TUASSERTE(long, 123, timeAdjustWeekRollover(123, refWeek));
   TUASSERTE(long, 123 + 512, refWeek);
   refWeek++;
   TUASSERTE(long, 1147, timeAdjustWeekRollover(123, refWeek));
   TUASSERTE(long, 123 + 513, refWeek);
      // test 10-bit to-correct and full ref week
   refWeek = 2014;
   TUASSERTE(long, 2014, timeAdjustWeekRollover(990, refWeek));
   TUASSERTE(long, 2014, refWeek);
      // test 10-bit to-correct and full ref week, two epochs off
   refWeek = 3038;
   TUASSERTE(long, 3038, timeAdjustWeekRollover(990, refWeek));
   TUASSERTE(long, 3038, refWeek);
      // test full to-correct week and full ref week, one epoch off
   refWeek = 3038;
   TUASSERTE(long, 3038, timeAdjustWeekRollover(2014, refWeek));
   TUASSERTE(long, 3038, refWeek);
      // test future data
   refWeek = 2014;
   TUASSERTE(long, 2015, timeAdjustWeekRollover(2015, refWeek));
   TUASSERTE(long, 2014, refWeek);
      // test future data edge cases
   refWeek = 2014;
   TUASSERTE(long, refWeek + 512, timeAdjustWeekRollover(refWeek + 512, refWeek));
   TUASSERTE(long, 2014, refWeek);
   TUASSERTE(long, refWeek - 511, timeAdjustWeekRollover(refWeek + 513, refWeek));
   TUASSERTE(long, 2014, refWeek);
      // test real-time path
   refWeek = 0;
   GPSWeekZcount cwz = SystemTime().convertToCommonTime();
   long expected = 1659;
      // This code should be able to function as expected at least through 2031.
   if (cwz.week > 2683)
      expected = 3195;
   else if (cwz.week > 1659)
      expected = 2171;
   TUASSERTE(long, expected, timeAdjustWeekRollover(123, refWeek));
      // refWeek should have been set to the system clock's time's week
   TUASSERTE(long, cwz.week, refWeek);
   TURETURN();
}


int main()
{
   TimeCorrection_T testClass;
   unsigned errorTotal = 0;
   errorTotal += testClass.testEpochRollover();
   cout << "Total Errors for " << __FILE__ <<": "<< errorTotal << endl;
   return errorTotal;
}
