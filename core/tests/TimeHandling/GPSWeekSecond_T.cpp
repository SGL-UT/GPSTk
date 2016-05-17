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

#include "GPSWeekSecond.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include "StringUtils.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
using namespace gpstk;
using namespace std;


class GPSWeekSecond_T
{
public:
   GPSWeekSecond_T() {}
   ~GPSWeekSecond_T() {}

      // initializationTest ensures the constructors set the values properly
   unsigned initializationTest()
   {
      TUDEF("GPSWeekSecond", "Constructor");

      GPSWeekSecond compare(1300,13500.,TimeSystem(2)); //Initialize an object
         //--------------------------------------------------------------------
         //Were the attributes set to expectation with the explicit
         //constructor?
         //--------------------------------------------------------------------
      TUASSERTE(int, 1300, compare.week);
      TUASSERTFE(13500, compare.sow);
      TUASSERTE(TimeSystem, TimeSystem(2), compare.getTimeSystem());


      testFramework.changeSourceMethod("ConstructorCopy");
      GPSWeekSecond copy(compare); // Initialize with copy constructor
         //--------------------------------------------------------------------
         //Were the attributes set to expectation with the copy constructor?
         //--------------------------------------------------------------------
      TUASSERTE(int, 1300, copy.week);
      TUASSERTFE(13500, copy.sow);
      TUASSERTE(TimeSystem, TimeSystem(2), copy.getTimeSystem());

      testFramework.changeSourceMethod("operator=");
      GPSWeekSecond assigned;
      assigned = compare;
         //--------------------------------------------------------------------
         //Were the attributes set to expectation with the Set operator?
         //--------------------------------------------------------------------
      TUASSERTE(int, 1300, assigned.week);
      TUASSERTFE(13500, assigned.sow);
      TUASSERTE(TimeSystem, TimeSystem(2), assigned.getTimeSystem());

      TURETURN();
   }


      // Test will check if GPSWeekSecond variable can be set from a map.
      // Test also implicity tests whether the != operator functions.
   unsigned setFromInfoTest()
   {
      TUDEF("GPSWeekSecond", "setFromInfo");

      GPSWeekSecond setFromInfo1;
      GPSWeekSecond setFromInfo2;
      GPSWeekSecond setFromInfo3;
      TimeTag::IdToValue id;
      id['F'] = "1300";
      id['g'] = "13500";
      id['P'] = "GPS";
      GPSWeekSecond compare(1300,13500.,TimeSystem(2)); //Initialize an object
         //--------------------------------------------------------------------
         //Does a proper setFromInfo work with all information provided?
         //--------------------------------------------------------------------
      TUASSERT(setFromInfo1.setFromInfo(id));
      TUASSERTE(GPSWeekSecond, compare, setFromInfo1);

      id.erase('F');
      GPSWeekSecond compare2(0,13500.,TimeSystem(2));
         //--------------------------------------------------------------------
         //Does a proper setFromInfo work with missing data?
         //--------------------------------------------------------------------
      TUASSERT(setFromInfo2.setFromInfo(id));
      TUASSERTE(GPSWeekSecond, compare2, setFromInfo2);

      TURETURN();
   }


      // Test will check if the ways to initialize and set an
      // GPSWeekSecond object.  Test also tests whether the comparison
      // operators and isValid method function.
   unsigned operatorTest()
   {
      TUDEF("GPSWeekSecond", "operator==");

      GPSWeekSecond compare(1300,13500.);
      GPSWeekSecond lessThanWeek(1299,13500.);
      GPSWeekSecond lessThanSecond(1300,13400.);
      GPSWeekSecond compareCopy(compare); // Initialize with copy constructor

         // TUASSERT is used below instead of TUASSERTE to make
         // certain the right operator is being tested.

         //--------------------------------------------------------------------
         //Does the == Operator function?
         //--------------------------------------------------------------------
      TUASSERT(  compare == compareCopy);
      TUASSERT(!(compare == lessThanWeek));
      TUASSERT(!(compare == lessThanSecond));

      testFramework.changeSourceMethod("operator!=");
         //--------------------------------------------------------------------
         //Does the != Operator function?
         //--------------------------------------------------------------------
      TUASSERT(  compare != lessThanWeek);
      TUASSERT(  compare != lessThanSecond);
      TUASSERT(!(compare != compare));

      testFramework.changeSourceMethod("operator<");
         //--------------------------------------------------------------------
         //Does the < Operator function?
         //--------------------------------------------------------------------
      TUASSERT(  lessThanWeek < compare);
      TUASSERT(  lessThanSecond < compare);
      TUASSERT(!(compare < lessThanWeek));
      TUASSERT(!(compare < lessThanSecond));
      TUASSERT(!(compare < compareCopy));

      testFramework.changeSourceMethod("operator>");
         //--------------------------------------------------------------------
         //Does the > Operator function?
         //--------------------------------------------------------------------
      TUASSERT(!(lessThanWeek > compare));
      TUASSERT(!(lessThanSecond > compare));
      TUASSERT(  compare > lessThanWeek);
      TUASSERT(  compare > lessThanSecond);
      TUASSERT(!(compare > compareCopy));

      testFramework.changeSourceMethod("operator<=");
         //--------------------------------------------------------------------
         //Does the <= Operator function?
         //--------------------------------------------------------------------
      TUASSERT(  lessThanWeek <= compare);
      TUASSERT(  lessThanSecond <= compare);
      TUASSERT(!(compare <= lessThanWeek));
      TUASSERT(!(compare <= lessThanSecond));
      TUASSERT(  compare <= compareCopy);

      testFramework.changeSourceMethod("operator>=");
         //--------------------------------------------------------------------
         //Does the >= Operator function?
         //--------------------------------------------------------------------
      TUASSERT(!(lessThanWeek >= compare));
      TUASSERT(!(lessThanSecond >= compare));
      TUASSERT(  compare >= lessThanWeek);
      TUASSERT(  compare >= lessThanSecond);
      TUASSERT(  compare >= compareCopy);

      TURETURN();
   }


      // Test the reset method.
   unsigned resetTest()
   {
      TUDEF("GPSWeekSecond", "reset");

      GPSWeekSecond compare(1300,13500.,TimeSystem::GPS); //Initialize an object
      compare.reset(); // Reset it

         //--------------------------------------------------------------------
         //Were the attributes reset to expectation?
         //--------------------------------------------------------------------
      TUASSERTE(int, 0, compare.week);
      TUASSERTFE(0, compare.sow);
      TUASSERTE(TimeSystem, TimeSystem(2), compare.getTimeSystem());

      TURETURN();
   }


      // Test will check converting to/from CommonTime.
   unsigned toFromCommonTimeTest()
   {
      TUDEF("GPSWeekSecond", "isValid");

      GPSWeekSecond compare(0,10.0,TimeSystem(2)); //Initialize an object
      CommonTime truth;
      long truthDay, truthSOD;
      double truthFSOD;
      truth.set(2444244, 43210,0.0,TimeSystem(2));

         //--------------------------------------------------------------------
         //Is the time after the BEGINNING_OF_TIME?
         //--------------------------------------------------------------------
      TUASSERT(compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);

         //--------------------------------------------------------------------
         //Is the set object valid?
         //--------------------------------------------------------------------
      TUASSERT(compare.isValid());

      CommonTime test = compare.convertToCommonTime(); //Convert to CommonTime
      long testDay, testSOD;
      double testFSOD;
      test.get(testDay, testSOD, testFSOD);
      truth.get(truthDay, truthSOD, truthFSOD);
      
      // Currently, GPSWeekSecond does not convert to proper CommonTime
      // These tests will be valid and will be uncommented once issue_248 has been 
      // resolved and merged into master.
      // TUASSERTE(long, truthDay, testDay); 
      // TUASSERTE(long, truthSOD, testSOD);
      // TUASSERTFE(truthFSOD, testFSOD);

      GPSWeekSecond test2;
      test2.convertFromCommonTime(test); //Convert From
      testFramework.changeSourceMethod("CommonTimeConversion");
         //--------------------------------------------------------------------
         //Is the result of conversion the same?
         //--------------------------------------------------------------------
      TUASSERTE(TimeSystem, compare.getTimeSystem(), test2.getTimeSystem());
      TUASSERTE(int, compare.week, test2.week);
      TUASSERTFE(compare.sow, test2.sow);

      TUASSERT(test2 == test);

      TURETURN();
   }


      // Test will check the TimeSystem comparisons when using the
      // comparison operators.
   unsigned timeSystemTest()
   {
      TUDEF("GPSWeekSecond", "OperatorEquivalentWithDifferingTimeSystem");

      GPSWeekSecond GPS1(1300,13500.,TimeSystem(2));
      GPSWeekSecond GPS2(1200,13500.,TimeSystem(2));
      GPSWeekSecond UTC1(1300,13500.,TimeSystem(5));
      GPSWeekSecond UNKNOWN(1300,13500.,TimeSystem(0));
      GPSWeekSecond ANY(1300,13500.,TimeSystem(1));
      GPSWeekSecond ANY2(1200,13500.,TimeSystem(1));

         //--------------------------------------------------------------------
         //Verify differing TimeSystem sets equivalence operator to false
         //Note that the operator test checks for == in ALL members
         //--------------------------------------------------------------------
      TUASSERT(!(GPS1 == UTC1));
      TUASSERT(GPS1 == ANY);
      TUASSERT(UTC1 == ANY);
      TUASSERT(UNKNOWN == ANY);

      testFramework.changeSourceMethod(
         "OperatorNotEquivalentWithDifferingTimeSystem");
         //--------------------------------------------------------------------
         //Verify different Time System but same time inequality
         //--------------------------------------------------------------------
      TUASSERT(GPS1 != UTC1);
      TUASSERT(GPS1 != UNKNOWN);
      TUASSERT(!(GPS1 != ANY));

      testFramework.changeSourceMethod(
         "OperatorLessThanWithDifferingTimeSystem");
         //--------------------------------------------------------------------
         //Verify TimeSystem=ANY does not matter in other operator comparisons
         //--------------------------------------------------------------------
      TUASSERT(ANY2 < GPS1);
      TUASSERT(GPS2 < ANY);

      testFramework.changeSourceMethod("setTimeSystem");
      UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem
         //--------------------------------------------------------------------
         //Ensure resetting a Time System changes it
         //--------------------------------------------------------------------
      TUASSERTE(TimeSystem, TimeSystem(2), UNKNOWN.getTimeSystem());

      TURETURN();
   }


      // Test for the formatted printing of GPSWeekSecond objects
   unsigned printfTest()
   {
      TUDEF("GPSWeekSecond", "printf");

      GPSWeekSecond GPS1(1300,13500.,TimeSystem(2));
      GPSWeekSecond UTC1(1300,13500.,TimeSystem(7));

         //--------------------------------------------------------------------
         //Verify printed output matches expectation
         //--------------------------------------------------------------------
      TUASSERTE(string, "1300 13500.000000 GPS", GPS1.printf("%04F %05g %02P"));
      TUASSERTE(string, "1300 13500.000000 UTC", UTC1.printf("%04F %05g %02P"));

      testFramework.changeSourceMethod("printError");

         //--------------------------------------------------------------------
         //Verify printed error message matches expectation
         //--------------------------------------------------------------------
      TUASSERTE(string, "BadGPSepoch BadGPSmweek BadGPSdow BadGPSfweek BadGPSsow BadGPSsys", GPS1.printError("%E %G %w %04F %05g %02P"));
      TUASSERTE(string, "BadGPSepoch BadGPSmweek BadGPSdow BadGPSfweek BadGPSsow BadGPSsys", UTC1.printError("%E %G %w %04F %05g %02P"));

      TURETURN();
   }
};


int main() //Main function to initialize and run all tests above
{
   unsigned errorTotal = 0;
   GPSWeekSecond_T testClass;

   errorTotal += testClass.initializationTest();
   errorTotal += testClass.operatorTest();
   errorTotal += testClass.setFromInfoTest();
   errorTotal += testClass.resetTest();
   errorTotal += testClass.timeSystemTest();
   errorTotal += testClass.toFromCommonTimeTest();
   errorTotal += testClass.printfTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; //Return the total number of errors
}
