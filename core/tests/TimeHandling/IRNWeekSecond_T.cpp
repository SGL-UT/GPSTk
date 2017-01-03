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

#include "IRNWeekSecond.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include "StringUtils.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
using namespace gpstk;
using namespace std;


class IRNWeekSecond_T
{
public:
   IRNWeekSecond_T() {}
   ~IRNWeekSecond_T() {}

      // initializationTest ensures the constructors set the values properly
   unsigned initializationTest()
   {
      TUDEF("IRNWeekSecond", "Constructor");

      IRNWeekSecond compare(1300,13500.,TimeSystem::IRN); //Initialize an object
         //--------------------------------------------------------------------
         //Were the attributes set to expectation with the explicit
         //constructor?
         //--------------------------------------------------------------------
      TUASSERTE(int, 1300, compare.week);
      TUASSERTFE(13500, compare.sow);
      TUASSERTE(TimeSystem, TimeSystem::IRN, compare.getTimeSystem());


      testFramework.changeSourceMethod("ConstructorCopy");
      IRNWeekSecond copy(compare); // Initialize with copy constructor
         //--------------------------------------------------------------------
         //Were the attributes set to expectation with the copy constructor?
         //--------------------------------------------------------------------
      TUASSERTE(int, 1300, copy.week);
      TUASSERTFE(13500, copy.sow);
      TUASSERTE(TimeSystem, TimeSystem::IRN, copy.getTimeSystem());

      testFramework.changeSourceMethod("operator=");
      IRNWeekSecond assigned;
      assigned = compare;
         //--------------------------------------------------------------------
         //Were the attributes set to expectation with the Set operator?
         //--------------------------------------------------------------------
      TUASSERTE(int, 1300, assigned.week);
      TUASSERTFE(13500, assigned.sow);
      TUASSERTE(TimeSystem, TimeSystem::IRN, assigned.getTimeSystem());

      TURETURN();
   }


      // Test will check if IRNWeekSecond variable can be set from a map.
      // Test also implicity tests whether the != operator functions.
   unsigned setFromInfoTest()
   {
      TUDEF("IRNWeekSecond", "setFromInfo");

      IRNWeekSecond setFromInfo1;
      IRNWeekSecond setFromInfo2;
      IRNWeekSecond setFromInfo3;
      TimeTag::IdToValue id;
      id['O'] = "1300";
      id['g'] = "13500";
      id['P'] = "IRN";
      IRNWeekSecond compare(1300,13500.,TimeSystem::IRN); //Initialize an object
         //--------------------------------------------------------------------
         //Does a proper setFromInfo work with all information provided?
         //--------------------------------------------------------------------
      TUASSERT(setFromInfo1.setFromInfo(id));
      TUASSERTE(IRNWeekSecond, compare, setFromInfo1);

      id.erase('O');
      IRNWeekSecond compare2(0,13500.,TimeSystem::IRN);
         //--------------------------------------------------------------------
         //Does a proper setFromInfo work with missing data?
         //--------------------------------------------------------------------
      TUASSERT(setFromInfo2.setFromInfo(id));
      TUASSERTE(IRNWeekSecond, compare2, setFromInfo2);

      TURETURN();
   }


      // Test will check if the ways to initialize and set an
      // IRNWeekSecond object.  Test also tests whether the comparison
      // operators and isValid method function.
   unsigned operatorTest()
   {
      TUDEF("IRNWeekSecond", "operator==");

      IRNWeekSecond compare(1300,13500.);
      IRNWeekSecond lessThanWeek(1299,13500.);
      IRNWeekSecond lessThanSecond(1300,13400.);
      IRNWeekSecond compareCopy(compare); // Initialize with copy constructor

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
      TUDEF("IRNWeekSecond", "reset");

      IRNWeekSecond compare(1300,13500.,TimeSystem::IRN); //Initialize an object
      compare.reset(); // Reset it

         //--------------------------------------------------------------------
         //Were the attributes reset to expectation?
         //--------------------------------------------------------------------
      TUASSERTE(int, 0, compare.week);
      TUASSERTFE(0, compare.sow);
      TUASSERTE(TimeSystem, TimeSystem::IRN, compare.getTimeSystem());

      TURETURN();
   }


      // Test will check converting to/from CommonTime.
   unsigned toFromCommonTimeTest()
   {
      TUDEF("IRNWeekSecond", "isValid");

      IRNWeekSecond compare(0,10.0,TimeSystem::IRN); //Initialize an object
      CommonTime truth;
      long truthDay, truthSOD;
      double truthFSOD;
      truth.set(2451412, 43210,0.0,TimeSystem::IRN);

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
      
      // Currently, IRNWeekSecond does not convert to proper CommonTime
      // These tests will be valid and will be uncommented once issue_248 has been 
      // resolved and merged into master.
      // TUASSERTE(long, truthDay, testDay); 
      // TUASSERTE(long, truthSOD, testSOD);
      // TUASSERTFE(truthFSOD, testFSOD);

      IRNWeekSecond test2;
      test2.convertFromCommonTime(test); //Convert From
      testFramework.changeSourceMethod("CommonTimeConversion");
         //--------------------------------------------------------------------
         //Is the result of conversion the same?
         //--------------------------------------------------------------------
      TUASSERTE(TimeSystem, compare.getTimeSystem(), test2.getTimeSystem());
      TUASSERTE(int, compare.week, test2.week);
      TUASSERTFE(compare.sow, test2.sow);

      //TUASSERT(test2 == test);

      TURETURN();
   }


      // Test will check the TimeSystem comparisons when using the
      // comparison operators.
   unsigned timeSystemTest()
   {
      TUDEF("IRNWeekSecond", "OperatorEquivalentWithDifferingTimeSystem");

      IRNWeekSecond IRN1(1300,13500.,TimeSystem::IRN);
      IRNWeekSecond IRN2(1200,13500.,TimeSystem::IRN);
      IRNWeekSecond UTC1(1300,13500.,TimeSystem::QZS);
      IRNWeekSecond UNKNOWN(1300,13500.,TimeSystem::Unknown);
      IRNWeekSecond ANY(1300,13500.,TimeSystem::Any);
      IRNWeekSecond ANY2(1200,13500.,TimeSystem::Any);

         //--------------------------------------------------------------------
         //Verify differing TimeSystem sets equivalence operator to false
         //Note that the operator test checks for == in ALL members
         //--------------------------------------------------------------------
      TUASSERT(!(IRN1 == UTC1));
      TUASSERT(IRN1 == ANY);
      TUASSERT(UTC1 == ANY);
      TUASSERT(UNKNOWN == ANY);

      testFramework.changeSourceMethod(
         "OperatorNotEquivalentWithDifferingTimeSystem");
         //--------------------------------------------------------------------
         //Verify different Time System but same time inequality
         //--------------------------------------------------------------------
      TUASSERT(IRN1 != UTC1);
      TUASSERT(IRN1 != UNKNOWN);
      TUASSERT(!(IRN1 != ANY));

      testFramework.changeSourceMethod(
         "OperatorLessThanWithDifferingTimeSystem");
         //--------------------------------------------------------------------
         //Verify TimeSystem=ANY does not matter in other operator comparisons
         //--------------------------------------------------------------------
      TUASSERT(ANY2 < IRN1);
      TUASSERT(IRN2 < ANY);

      testFramework.changeSourceMethod("setTimeSystem");
      UNKNOWN.setTimeSystem(TimeSystem::IRN); //Set the Unknown TimeSystem
         //--------------------------------------------------------------------
         //Ensure resetting a Time System changes it
         //--------------------------------------------------------------------
      TUASSERTE(TimeSystem, TimeSystem::IRN, UNKNOWN.getTimeSystem());

      TURETURN();
   }


      // Test for the formatted printing of IRNWeekSecond objects
   unsigned printfTest()
   {
      TUDEF("IRNWeekSecond", "printf");

      IRNWeekSecond IRN1(1300,13500.,TimeSystem::IRN);
      IRNWeekSecond UTC1(1300,13500.,TimeSystem::UTC);

         //--------------------------------------------------------------------
         //Verify printed output matches expectation
         //--------------------------------------------------------------------
      TUASSERTE(string, "1300 13500.000000 IRN", IRN1.printf("%04O %05g %02P"));
      TUASSERTE(string, "1300 13500.000000 UTC", UTC1.printf("%04O %05g %02P"));

      testFramework.changeSourceMethod("printError");

         //--------------------------------------------------------------------
         //Verify printed error message matches expectation
         //--------------------------------------------------------------------
      TUASSERTE(string, "BadIRNepoch BadIRNmweek BadIRNdow BadIRNfweek BadIRNsow BadIRNsys", IRN1.printError("%X %o %w %04O %05g %02P"));
      TUASSERTE(string, "BadIRNepoch BadIRNmweek BadIRNdow BadIRNfweek BadIRNsow BadIRNsys", UTC1.printError("%X %o %w %04O %05g %02P"));

      TURETURN();
   }
};


int main() //Main function to initialize and run all tests above
{
   unsigned errorTotal = 0;
   IRNWeekSecond_T testClass;

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
