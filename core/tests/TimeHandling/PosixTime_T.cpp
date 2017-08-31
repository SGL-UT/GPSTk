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
//  Copyright 2017, The University of Texas at Austin
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

#include "PosixTime.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

class PosixTime_T
{
public:
      /// initializationTest ensures the constructors set the values properly
   unsigned initializationTest (void)
   {
      TUDEF("PosixTime", "Constructor");


      PosixTime compare(1350000,1,TimeSystem(2)); // Initialize an object

         // Were the attributes set to expectation with the explicit
         // constructor?
      TUASSERTE(time_t, 1350000, compare.ts.tv_sec);
      TUASSERTE(long, 1, compare.ts.tv_nsec);
      TUASSERTE(TimeSystem, TimeSystem(2), compare.getTimeSystem());

      TUCSM("PosixTime(PosixTime)");
      PosixTime copy(compare); //  Initialize with copy constructor
         // Were the attributes set to expectation with the copy constructor?
      TUASSERTE(time_t, 1350000, copy.ts.tv_sec);
      TUASSERTE(long, 1, copy.ts.tv_nsec);
      TUASSERTE(TimeSystem, TimeSystem(2), copy.getTimeSystem());

      TUCSM("operator=");
      PosixTime assigned;
      assigned = compare;
         // Were the attributes set to expectation with the Set Operator?
      TUASSERTE(time_t, 1350000, assigned.ts.tv_sec);
      TUASSERTE(long, 1, assigned.ts.tv_nsec);
      TUASSERTE(TimeSystem, TimeSystem(2), assigned.getTimeSystem());

      TURETURN();
   }


      // Check if PosixTime variable can be set from a map.  Test also
      // implicity tests whether the != operator functions.
   unsigned setFromInfoTest (void)
   {
      TUDEF("PosixTime", "setFromInfo");

      PosixTime setFromInfo1;
      PosixTime setFromInfo2;
      PosixTime compare(1350000,1,TimeSystem(2)), compare2(0,1,TimeSystem(2));
      TimeTag::IdToValue id;

      id['W'] = "1350000";
      id['N'] = "1";
      id['P'] = "GPS";

         // Does a proper setFromInfo work with all information provided?
      TUASSERT(setFromInfo1.setFromInfo(id));
      TUASSERTE(PosixTime, compare, setFromInfo1);

      id.erase('W');
         // Does a proper setFromInfo work with missing information?
      TUASSERT(setFromInfo2.setFromInfo(id));
      TUASSERTE(PosixTime, compare2, setFromInfo2);

      TURETURN();
   }


      // Check if the ways to initialize and set an PosixTime object.
      // Test also tests whether the comparison operators and isValid
      // method function.
   unsigned operatorTest (void)
   {
      TUDEF("PosixTime", "moo");

      PosixTime compare(1350000,100); //  Initialize with value
      PosixTime lessThanSec(1340000, 100); // Initialize with fewer seconds
      PosixTime lessThanNanoSec(1350000,0); // Initialize with fewer nanoseconds
      PosixTime compareCopy(compare); //  Initialize with copy constructor

      TUCSM("operator==");
      TUASSERTE(PosixTime, compare, compareCopy);
      TUASSERT(!(compare == lessThanSec));
      TUASSERT(!(compare == lessThanNanoSec));

      TUCSM("operator!=");
      TUASSERT(!(compare != compareCopy));
      TUASSERT(compare != lessThanSec);
      TUASSERT(compare != lessThanNanoSec);

      TUCSM("operator<");
      TUASSERT(lessThanSec < compare);
      TUASSERT(lessThanNanoSec < compare);
      TUASSERT(!(compare < lessThanSec));
      TUASSERT(!(compare < lessThanNanoSec));
      TUASSERT(!(compare < compareCopy));

      TUCSM("operator>");
      TUASSERT(!(lessThanSec > compare));
      TUASSERT(!(lessThanNanoSec > compare));
      TUASSERT(compare > lessThanSec);
      TUASSERT(compare > lessThanNanoSec);
      TUASSERT(!(compare > compareCopy));

      TUCSM("operator<=");
      TUASSERT(lessThanSec <= compare);
      TUASSERT(lessThanNanoSec <= compare);
      TUASSERT(!(compare <= lessThanSec));
      TUASSERT(!(compare <= lessThanNanoSec));
      TUASSERT(compare <= compareCopy);

      TUCSM("operator>=");
      TUASSERT(!(lessThanSec >= compare));
      TUASSERT(!(lessThanNanoSec >= compare));
      TUASSERT(compare >= lessThanSec);
      TUASSERT(compare >= lessThanNanoSec);
      TUASSERT(compare >= compareCopy);

      TURETURN();
   }


      // Check the reset method.
   unsigned  resetTest (void)
   {
      TUDEF("PosixTime", "reset" );

      PosixTime compare(1350000,0,TimeSystem(2)); // Initialize an object

      compare.reset(); //  Reset it

         // Were the attributes reset to expectation?
      TUASSERTE(TimeSystem, TimeSystem(0), compare.getTimeSystem());
      TUASSERTE(time_t, 0, compare.ts.tv_sec);
      TUASSERTE(long, 0, compare.ts.tv_nsec);

      TURETURN();
   }


      // Check converting to/from CommonTime.
   unsigned  toFromCommonTimeTest (void)
   {
      TUDEF("PosixTime", "isValid");

      PosixTime compare(1350000,0,TimeSystem(2)); // Initialize an object
      CommonTime test = compare.convertToCommonTime(); // Convert to

         // Is the time after the BEGINNING_OF_TIME?
         // Is this even a useful test?
      TUASSERT(compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);

         // Is the set object valid?
      TUASSERT(compare.isValid());

      PosixTime test2;
      test2.convertFromCommonTime(test); // Convert From
      TUCSM("convertFromCommonTime");
         // Is the result of conversion the same?
      TUASSERTE(TimeSystem, compare.getTimeSystem(), test2.getTimeSystem());
      TUASSERTE(time_t, compare.ts.tv_sec, test2.ts.tv_sec);
      TUASSERTE(long, compare.ts.tv_nsec, test2.ts.tv_nsec);

      TURETURN();
   }


      // Check the TimeSystem comparisons when using the comparison
      // operators.
   unsigned  timeSystemTest (void)
   {
      TUDEF("PosixTime", "OperatorEquivalentWithDifferingTimeSystem");

      PosixTime GPS1(1350000,0,TimeSystem::GPS);
      PosixTime GPS2(1340000,0,TimeSystem::GPS);
      PosixTime UTC1(1350000,0,TimeSystem::UTC);
      PosixTime UNKNOWN(1350000,0,TimeSystem::Unknown);
      PosixTime ANY(1350000,0,TimeSystem::Any);
      PosixTime ANY2(1340000,0,TimeSystem::Any);

         // Verify differing TimeSystem sets equivalence operator to false
         // Note that the operator test checks for == in ALL members
      TUASSERT(!(GPS1 == UTC1));
      TUASSERT(GPS1 == ANY);
      TUASSERT(UTC1 == ANY);
      TUASSERT(UNKNOWN == ANY);

      TUCSM("OperatorNotEquivalentWithDifferingTimeSystem");
         // Verify different Time System but same time inequality
      TUASSERT(GPS1 != UTC1);
      TUASSERT(GPS1 != UNKNOWN);
      TUASSERT(!(GPS1 != ANY));

      TUCSM("OperatorLessThanWithDifferingTimeSystem");
         // Verify TimeSystem=ANY does not matter in other operator comparisons
      TUASSERT(ANY2 < GPS1);
      TUASSERT(GPS2 < ANY);

      TUCSM("setTimeSystem");
      UNKNOWN.setTimeSystem(TimeSystem(2)); // Set the Unknown TimeSystem
         // Ensure resetting a Time System changes it
      TUASSERTE(TimeSystem, TimeSystem(2), UNKNOWN.getTimeSystem());

      TURETURN();
   }


      // Test for the formatted printing of PosixTime objects
   unsigned  printfTest (void)
   {
      TUDEF("PosixTime", "printf");


      PosixTime GPS1(1350000,0,TimeSystem::GPS);
      PosixTime UTC1(1350000,0,TimeSystem::UTC);

         // Verify printed output matches expectation
      TUASSERTE(string, "1350000 00 GPS", GPS1.printf("%07W %02N %02P"));
      TUASSERTE(string, "1350000 00 UTC", UTC1.printf("%07W %02N %02P"));

         // Verify printed error message matches expectation
      TUASSERTE(string, "ErrorBadTime ErrorBadTime ErrorBadTime",
                GPS1.printError("%07W %02N %02P"));
      TUASSERTE(string, "ErrorBadTime ErrorBadTime ErrorBadTime",
                UTC1.printError("%07W %02N %02P"));

      TURETURN();
   }
};

int main() // Main function to initialize and run all tests above
{
   int errorTotal = 0;
   PosixTime_T testClass;

   errorTotal += testClass.initializationTest();
   errorTotal += testClass.operatorTest();
   errorTotal += testClass.setFromInfoTest();
   errorTotal += testClass.resetTest();
   errorTotal += testClass.timeSystemTest();
   errorTotal += testClass.toFromCommonTimeTest();
   errorTotal += testClass.printfTest();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal
             << std::endl;

   return errorTotal; // Return the total number of errors
}
