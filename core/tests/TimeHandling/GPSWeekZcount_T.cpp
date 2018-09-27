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

#include "GPSWeekZcount.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace gpstk;
using namespace std;

class GPSWeekZcount_T
{
public:
      /// Default Constructor, set the precision value
   GPSWeekZcount_T(){eps = 1e-12;}
      /// Default Desructor
   ~GPSWeekZcount_T() {}

      /// initializationTest ensures the constructors set the values properly
   unsigned initializationTest()
   {
      TUDEF("GPSWeekZcount", "GPSWeekZcount(w,z,ts)");

      GPSWeekZcount compare(1300,13500.,TimeSystem(2)); //Initialize an object
         //---------------------------------------------------------------------
         //Were the attributes set to expectation with the explicit constructor?
         //---------------------------------------------------------------------
      TUASSERTE(int, 1300, compare.week);
      TUASSERTE(unsigned int, 13500, compare.zcount);
      TUASSERTE(TimeSystem, TimeSystem(2), compare.getTimeSystem());

      TUCSM("GPSWeekZcount(GPSWeekZcount)");
      GPSWeekZcount copy(compare); // Initialize with copy constructor

         //---------------------------------------------------------------------
         //Were the attributes set to expectation with the copy constructor?
         //---------------------------------------------------------------------
      TUASSERTE(int, 1300, copy.week);
      TUASSERTE(unsigned int, 13500, copy.zcount);
      TUASSERTE(TimeSystem, TimeSystem(2), copy.getTimeSystem());

      TUCSM("operator=");
      GPSWeekZcount assigned;
      assigned = compare;

         //---------------------------------------------------------------------
         //Were the attributes set to expectation with the Set operator?
         //---------------------------------------------------------------------
      TUASSERTE(int, 1300, assigned.week);
      TUASSERTE(unsigned int, 13500, assigned.zcount);
      TUASSERTE(TimeSystem, TimeSystem(2), assigned.getTimeSystem());

      TURETURN();
   }


      /** Test will check if GPSWeekZcount variable can be set from a map.
       * Test also implicity tests whether the != operator functions. */
   unsigned setFromInfoTest ()
   {
      TUDEF("GPSWeekZcount", "setFromInfo");

      GPSWeekZcount setFromInfo1;
      GPSWeekZcount setFromInfo2;
      GPSWeekZcount setFromInfo3;
      TimeTag::IdToValue id;
      id['F'] = "1300";
      id['z'] = "13500";
      id['P'] = "GPS";

      GPSWeekZcount compare(1300,13500.,TimeSystem(2)); //Initialize an object
         //---------------------------------------------------------------------
         //Does a proper setFromInfo work with all information provided?
         //---------------------------------------------------------------------
      TUASSERT(setFromInfo1.setFromInfo(id));
      TUASSERTE(GPSWeekZcount, compare, setFromInfo1);

      id.erase('z');
      id['w'] = "3";
      GPSWeekZcount compare2(1300,3*57600L,TimeSystem(2));
         //---------------------------------------------------------------------
         //Does a proper setFromInfo work with different data type?
         //---------------------------------------------------------------------
      TUASSERT(setFromInfo2.setFromInfo(id));
      TUASSERTE(GPSWeekZcount, compare2, setFromInfo2);

      id.erase('F');
      GPSWeekZcount compare3(0,3*57600L,TimeSystem(2));
         //---------------------------------------------------------------------
         //Does a proper setFromInfo work with missing information?
         //---------------------------------------------------------------------
      TUASSERT(setFromInfo3.setFromInfo(id));
      TUASSERTE(GPSWeekZcount, compare3, setFromInfo3);

      TURETURN();
   }


      /** Test will check if the ways to initialize and set an
       * GPSWeekZcount object.  Test also tests whether the comparison
       * operators and isValid method function. */
   unsigned operatorTest()
   {
      TUDEF("GPSWeekZCount", "operator==");

      GPSWeekZcount compare(1300,13500);
      GPSWeekZcount lessThanWeek(1299,13500);
      GPSWeekZcount lessThanZcount(1300,13400);
      GPSWeekZcount compareCopy(compare); // Initialize with copy constructor

         //---------------------------------------------------------------------
         //Does the == Operator function?
         //---------------------------------------------------------------------
      TUASSERT(compare == compareCopy);
      TUASSERT(!(compare == lessThanWeek));
      TUASSERT(!(compare == lessThanZcount));

      TUCSM("operator!=");
         //---------------------------------------------------------------------
         //Does the != Operator function?
         //---------------------------------------------------------------------
      TUASSERT(compare != lessThanWeek);
      TUASSERT(compare != lessThanZcount);
      TUASSERT(!(compare != compare));

      TUCSM("operator<");
         //---------------------------------------------------------------------
         //Does the < Operator function?
         //---------------------------------------------------------------------
      TUASSERT(lessThanWeek < compare);
      TUASSERT(lessThanZcount < compare);
      TUASSERT(!(compare < lessThanWeek));
      TUASSERT(!(compare < lessThanZcount));
      TUASSERT(!(compare < compareCopy));

      TUCSM("operator>");
         //---------------------------------------------------------------------
         //Does the > Operator function?
         //---------------------------------------------------------------------
      TUASSERT(!(lessThanWeek > compare));
      TUASSERT(!(lessThanZcount > compare));
      TUASSERT(compare > lessThanWeek);
      TUASSERT(compare > lessThanZcount);
      TUASSERT(!(compare > compareCopy));

      TUCSM("operator<=");
         //---------------------------------------------------------------------
         //Does the <= Operator function?
         //---------------------------------------------------------------------
      TUASSERT(lessThanWeek <= compare);
      TUASSERT(lessThanZcount <= compare);
      TUASSERT(!(compare <= lessThanWeek));
      TUASSERT(!(compare <= lessThanZcount));
      TUASSERT(compare <= compareCopy);

      TUCSM("operator>=");
         //---------------------------------------------------------------------
         //Does the >= Operator function?
         //---------------------------------------------------------------------
      TUASSERT(!(lessThanWeek >= compare));
      TUASSERT(!(lessThanZcount >= compare));
      TUASSERT(compare >= lessThanWeek);
      TUASSERT(compare >= lessThanZcount);
      TUASSERT(compare >= compareCopy);

      TURETURN();
   }


      /// Test will check the reset method.
   unsigned  resetTest()
   {
      TUDEF("GPSWeekZcount", "reset");

      GPSWeekZcount compare(1300,13500.,TimeSystem::GPS); //Initialize an object

      compare.reset(); // Reset it

         //---------------------------------------------------------------------
         //Were the attributes reset to expectation?
         //---------------------------------------------------------------------
      TUASSERTE(int, 0, compare.week);
      TUASSERTE(unsigned int, 0, compare.zcount);
      TUASSERTE(TimeSystem, TimeSystem(2), compare.getTimeSystem());

      TURETURN();
   }


      /// Test will check converting to/from CommonTime.
   unsigned  toFromCommonTimeTest()
   {
      TUDEF("GPSWeekZcount", "isValid");

      GPSWeekZcount compare(1300,13500.,TimeSystem(2)); //Initialize an object

         //---------------------------------------------------------------------
         //Is the time after the BEGINNING_OF_TIME?
         //---------------------------------------------------------------------
      TUASSERT(compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);

         //---------------------------------------------------------------------
         //Is the set object valid?
         //---------------------------------------------------------------------
      TUASSERT(compare.isValid());

      CommonTime test = compare.convertToCommonTime(); //Convert to

      GPSWeekZcount test2;
      test2.convertFromCommonTime(test); //Convert From

      TUCSM("CommonTimeConversion");
         //---------------------------------------------------------------------
         //Is the result of conversion the same?
         //---------------------------------------------------------------------
      TUASSERTE(TimeSystem, compare.getTimeSystem(), test2.getTimeSystem());
      TUASSERTE(int, compare.week, test2.week);
      TUASSERTE(unsigned int, compare.zcount, test2.zcount);

      TURETURN();
   }


      /** Test will check the TimeSystem comparisons when using the
       * comparison operators. */
   unsigned  timeSystemTest()
   {
      TUDEF("GPSWeekZcount", "operator==");

      GPSWeekZcount gps1(1300,13500.,TimeSystem(2));
      GPSWeekZcount gps2(1200,13500.,TimeSystem(2));
      GPSWeekZcount utc1(1300,13500.,TimeSystem(5));
      GPSWeekZcount unknown(1300,13500.,TimeSystem(0));
      GPSWeekZcount any(1300,13500.,TimeSystem(1));
      GPSWeekZcount any2(1200,13500.,TimeSystem(1));

         //---------------------------------------------------------------------
         //Verify differing TimeSystem sets equivalence operator to false
         //Note that the operator test checks for == in ALL members
         //---------------------------------------------------------------------
      TUASSERT(!(gps1 == utc1));
      TUASSERT(gps1 == any);
      TUASSERT(utc1 == any);
      TUASSERT(unknown == any);

         //---------------------------------------------------------------------
         //Verify different Time System but same time inequality
         //---------------------------------------------------------------------
      TUASSERT(gps1 != utc1);
      TUASSERT(gps1 != unknown);
      TUASSERT(!(gps1 != any));

      TUCSM("operator<");
         //---------------------------------------------------------------------
         //Verify TimeSystem=ANY does not matter in other operator comparisons
         //---------------------------------------------------------------------
      TUASSERT(any2 < gps1);
      TUASSERT(gps2 < any);

      TUCSM("setTimeSystem");
      unknown.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem
         //---------------------------------------------------------------------
         //Ensure resetting a Time System changes it
         //---------------------------------------------------------------------
      TUASSERTE(TimeSystem, TimeSystem(2), unknown.getTimeSystem());

      TURETURN();
   }


      /// Test for the formatted printing of GPSWeekZcount objects
   unsigned  printfTest()
   {
      TUDEF("GPSWeekZCount", "printf");

      GPSWeekZcount gps1(1300,13500.,TimeSystem::GPS);
      GPSWeekZcount utc1(1300,13500.,TimeSystem::UTC);

         //---------------------------------------------------------------------
         //Verify printed output matches expectation
         //---------------------------------------------------------------------
      TUASSERTE(std::string, (std::string)"1300 13500 GPS",
                gps1.printf("%04F %05z %02P"));
      TUASSERTE(std::string, (std::string)"1300 13500 UTC",
                utc1.printf("%04F %05z %02P"));

      TUCSM("printError");
         //---------------------------------------------------------------------
         //Verify printed error message matches expectation
         //---------------------------------------------------------------------
      TUASSERTE(std::string,
                (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime",
                gps1.printError("%04F %05z %02P"));
      TUASSERTE(std::string,
                (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime",
                utc1.printError("%04F %05z %02P"));

      TURETURN();
   }


      /// Check the various math methods
   unsigned mathTest()
   {
      TUDEF("GPSWeekZcount", "getTotalZcounts");
      GPSWeekZcount orig(1024, 0), copy, diff1(1024,1), diff2(1023,403199),
         diff3(1025,0);
      long expDiff1 = 1, expDiff2(-1), expDiff3(403200);

      TUASSERTE(unsigned long, 412876800, orig.getTotalZcounts());

      TUCSM("addWeeks");
      try
      {
         copy = orig;
         GPSWeekZcount &ref1 = copy.addWeeks(1);
            // ref1 and copy should both have the updated week number, and
            // ref1 should be a reference to copy
         TUASSERTE(int, 1025, copy.week);
         TUASSERTE(int, 1025, ref1.week);
         TUASSERTE(GPSWeekZcount*, &copy, &ref1);
      }
      catch (...)
      {
         TUFAIL("Caught an exception");
      }

      try
      {
         copy = orig;
         GPSWeekZcount &ref2 = copy.addWeeks(-1);
            // ref2 and copy should both have the updated week number, and
            // ref2 should be a reference to copy
         TUASSERTE(int, 1023, copy.week);
         TUASSERTE(int, 1023, ref2.week);
         TUASSERTE(GPSWeekZcount*, &copy, &ref2);
      }
      catch (...)
      {
         TUFAIL("Caught an exception");
      }
         // invalid week
      try
      {
         copy = orig;
         copy.addWeeks(-1025);
         TUFAIL("addWeeks(invalid)");
      }
      catch (gpstk::InvalidRequest)
      {
         TUPASS("addWeeks(invalid)");
      }
      catch (...)
      {
         TUFAIL("addWeeks(invalid)");
      }
      

      TUCSM("addZcounts");
         // simple add
      copy = orig;
      TUCATCH(copy.addZcounts(27));
      TUASSERTE(int, 1024, copy.week);
      TUASSERTE(unsigned int, 27, copy.zcount);
         // simple subtract
      TUCATCH(copy.addZcounts(-27));
      TUASSERTE(int, 1024, copy.week);
      TUASSERTE(unsigned int, 0, copy.zcount);
         // subtract with week roll-over
      TUCATCH(copy.addZcounts(-43));
      TUASSERTE(int, 1023, copy.week);
      TUASSERTE(unsigned int, 403157, copy.zcount);
         // add with week roll-over
      TUCATCH(copy.addZcounts(71));
      TUASSERTE(int, 1024, copy.week);
      TUASSERTE(unsigned int, 28, copy.zcount);
         // invalid week
      try
      {
         copy = orig;
         copy.addZcounts(-413280000);
         TUFAIL("addZcounts(invalid)");
      }
      catch (gpstk::InvalidRequest)
      {
         TUPASS("addZcounts(invalid)");
      }
      catch (...)
      {
         TUFAIL("addZcounts(invalid)");
      }

      TUCSM("operator++ (postfix)");
      copy = orig;
         // zcount should not be modified until AFTER this statement
      TUASSERTE(unsigned int, 0, (copy++).zcount);
      TUASSERTE(unsigned int, 1, copy.zcount);
      TUASSERTE(int, 1024, copy.week);

      TUCSM("operator++ (prefix)");
      copy = orig;
         // zcount should be modified at the start of this statement
      TUASSERTE(unsigned int, 1, (++copy).zcount);
      TUASSERTE(unsigned int, 1, copy.zcount);
      TUASSERTE(int, 1024, copy.week);
         // prefix should return reference to copy
      TUASSERTE(GPSWeekZcount*, &copy, &(++copy));

      TUCSM("operator-- (postfix)");
      copy = orig;
         // zcount should not be modified until AFTER this statement
      TUASSERTE(unsigned int, 0, (copy--).zcount);
      TUASSERTE(unsigned int, 403199, copy.zcount);
      TUASSERTE(int, 1023, copy.week);

      TUCSM("operator-- (prefix)");
      copy = orig;
         // zcount should be modified at the start of this statement
      TUASSERTE(unsigned int, 403199, (--copy).zcount);
      TUASSERTE(unsigned int, 403199, copy.zcount);
      TUASSERTE(int, 1023, copy.week);
         // prefix should return reference to copy
      TUASSERTE(GPSWeekZcount*, &copy, &(--copy));

      TUCSM("operator+");
      try
      {
         copy = orig;
         GPSWeekZcount added = copy + 1;
            // copy should not have changed
         TUASSERTE(GPSWeekZcount, orig, copy);
         TUASSERTE(int, 1024, added.week);
         TUASSERTE(unsigned int, 1, added.zcount);
      }
      catch (...)
      {
         TUFAIL("Caught an exception");
      }
         // add a negative
      try
      {
         copy = orig;
         long counts = -1;
         GPSWeekZcount added = copy + counts;
            // copy should not have changed
         TUASSERTE(GPSWeekZcount, orig, copy);
         TUASSERTE(int, 1023, added.week);
         TUASSERTE(unsigned int, 403199, added.zcount);
      }
      catch (...)
      {
         TUFAIL("Caught an exception");
      }
         // invalid week
      try
      {
         copy = orig;
         long counts = -413280000;
         GPSWeekZcount added = copy + counts;
         TUFAIL("operator+(invalid)");
      }
      catch (gpstk::InvalidRequest)
      {
         TUPASS("operator+(invalid)");
      }
      catch (...)
      {
         TUFAIL("operator+(invalid)");
      }

      TUCSM("operator-(long)");
      try
      {
         copy = orig;
         GPSWeekZcount added = copy - 1;
            // copy should not have changed
         TUASSERTE(GPSWeekZcount, orig, copy);
         TUASSERTE(int, 1023, added.week);
         TUASSERTE(unsigned int, 403199, added.zcount);
      }
      catch (...)
      {
         TUFAIL("Caught an exception");
      }
         // add a negative
      try
      {
         copy = orig;
         long counts = -1;
         GPSWeekZcount added = copy - counts;
            // copy should not have changed
         TUASSERTE(GPSWeekZcount, orig, copy);
         TUASSERTE(int, 1024, added.week);
         TUASSERTE(unsigned int, 1, added.zcount);
      }
      catch (...)
      {
         TUFAIL("Caught an exception");
      }
         // invalid week
      try
      {
         copy = orig;
         long counts = 413280000;
         GPSWeekZcount added = copy - counts;
         TUFAIL("operator-(invalid)");
      }
      catch (gpstk::InvalidRequest)
      {
         TUPASS("operator-(invalid)");
      }
      catch (...)
      {
         TUFAIL("operator-(invalid)");
      }

         // difference two GPSWeekZcount objects
      TUCSM("operator-(GPSWeekZcount)");
      copy = orig;
      TUASSERTE(long, 0, (orig - copy));
      TUASSERTE(long, expDiff1, diff1-copy);
      TUASSERTE(long, expDiff2, diff2-copy);
      TUASSERTE(long, expDiff3, diff3-copy);

      TUCSM("operator+=");
      copy = orig;
      TUASSERTE(unsigned int, 27, (copy += 27).zcount);
      TUASSERTE(unsigned int, 27, copy.zcount);
      TUASSERTE(int, 1024, copy.week);
         // += should return reference to copy
      TUASSERTE(GPSWeekZcount*, &copy, &(copy += 99));
         // add a negative
      try
      {
         copy = orig;
         long counts = -1;
         TUASSERTE(unsigned int, 403199, (copy += counts).zcount);
         TUASSERTE(int, 1023, copy.week);
      }
      catch (...)
      {
         TUFAIL("Caught an exception");
      }
         // invalid week
      try
      {
         copy = orig;
         long counts = -413280000;
         copy += counts;
         TUFAIL("operator+(invalid)");
      }
      catch (gpstk::InvalidRequest)
      {
         TUPASS("operator+(invalid)");
      }
      catch (...)
      {
         TUFAIL("operator+(invalid)");
      }

      TUCSM("operator-=");
      copy = orig;
      TUASSERTE(unsigned int, 403199, (copy -= 1).zcount);
      TUASSERTE(unsigned int, 403199, copy.zcount);
      TUASSERTE(int, 1023, copy.week);
         // -= should return reference to copy
      TUASSERTE(GPSWeekZcount*, &copy, &(copy -= 99));
         // subtract a negative
      try
      {
         copy = orig;
         long counts = -1;
         TUASSERTE(unsigned int, 1, (copy -= counts).zcount);
         TUASSERTE(int, 1024, copy.week);
      }
      catch (...)
      {
         TUFAIL("Caught an exception");
      }
         // invalid week
      try
      {
         copy = orig;
         long counts = 413280000;
         copy -= counts;
         TUFAIL("operator-(invalid)");
      }
      catch (gpstk::InvalidRequest)
      {
         TUPASS("operator-(invalid)");
      }
      catch (...)
      {
         TUFAIL("operator-(invalid)");
      }

      TURETURN();
   }


   unsigned testTimeBlock()
   {
      TUDEF("GPSWeekZcount", "inSameTimeBlock");
      GPSWeekZcount
         t0(1024, 10),
         sameMinute(1024,39),
         sameHour(1024, 2399),
         sameWeek(1024,403199),
         sameWeekOffset(1025, 9),
         notSameWeek(1023,403199);

      TUASSERT(t0.inSameTimeBlock(sameMinute, ZCOUNT_PER_MINUTE));
      TUASSERT(t0.inSameTimeBlock(sameMinute, ZCOUNT_PER_HOUR));
      TUASSERT(t0.inSameTimeBlock(sameMinute, ZCOUNT_PER_WEEK));

      TUASSERT(!t0.inSameTimeBlock(sameHour, ZCOUNT_PER_MINUTE));
      TUASSERT(t0.inSameTimeBlock(sameHour, ZCOUNT_PER_HOUR));
      TUASSERT(t0.inSameTimeBlock(sameHour, ZCOUNT_PER_WEEK));

      TUASSERT(!t0.inSameTimeBlock(sameWeek, ZCOUNT_PER_MINUTE));
      TUASSERT(!t0.inSameTimeBlock(sameWeek, ZCOUNT_PER_HOUR));
      TUASSERT(t0.inSameTimeBlock(sameWeek, ZCOUNT_PER_WEEK));

      TUASSERT(!t0.inSameTimeBlock(sameWeekOffset, ZCOUNT_PER_MINUTE));
      TUASSERT(!t0.inSameTimeBlock(sameWeekOffset, ZCOUNT_PER_HOUR));
      TUASSERT(!t0.inSameTimeBlock(sameWeekOffset, ZCOUNT_PER_WEEK));

      TUASSERT(!t0.inSameTimeBlock(notSameWeek, ZCOUNT_PER_MINUTE));
      TUASSERT(!t0.inSameTimeBlock(notSameWeek, ZCOUNT_PER_HOUR));
      TUASSERT(!t0.inSameTimeBlock(notSameWeek, ZCOUNT_PER_WEEK));

      TUASSERT(!t0.inSameTimeBlock(notSameWeek, ZCOUNT_PER_MINUTE, 10));
      TUASSERT(!t0.inSameTimeBlock(notSameWeek, ZCOUNT_PER_HOUR, 10));
      TUASSERT(!t0.inSameTimeBlock(notSameWeek, ZCOUNT_PER_WEEK, 10));

      TUASSERT(!t0.inSameTimeBlock(sameWeekOffset, ZCOUNT_PER_MINUTE, 10));
      TUASSERT(!t0.inSameTimeBlock(sameWeekOffset, ZCOUNT_PER_HOUR, 10));
      TUASSERT(t0.inSameTimeBlock(sameWeekOffset, ZCOUNT_PER_WEEK, 10));

      TURETURN();
   }


private:
   double eps;
};


int main() //Main function to initialize and run all tests above
{
   unsigned errorCounter = 0;
   GPSWeekZcount_T testClass;

   errorCounter += testClass.initializationTest();
   errorCounter += testClass.operatorTest();
   errorCounter += testClass.setFromInfoTest();
   errorCounter += testClass.resetTest();
   errorCounter += testClass.timeSystemTest();
   errorCounter += testClass.toFromCommonTimeTest();
   errorCounter += testClass.printfTest();
   errorCounter += testClass.mathTest();
   errorCounter += testClass.testTimeBlock();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter
             << std::endl;

   return errorCounter; //Return the total number of errors
}
