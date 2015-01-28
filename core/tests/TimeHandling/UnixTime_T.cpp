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

#include "UnixTime.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

class UnixTime_T
{
	public:
/* Test to ensure the values in the constructor go to their intended locations */
	int initializationTest (void)
	{
		TestUtil testFramework( "UnixTime", "Constructor", __FILE__, __LINE__ );
		testFramework.init();

		UnixTime Compare(1350000,1,TimeSystem(2)); //Initialize an object

//--------------UnixTime_initializationTest_1 - Was the sec value set to expectation?
		testFramework.assert(1350000 == Compare.tv.tv_sec);
		testFramework.next();

//--------------UnixTime_initializationTest_2 - Was the usec value set to expectation?
		testFramework.assert(1 == Compare.tv.tv_sec);
		testFramework.next();

//--------------UnixTime_initializationTest_3 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("Constructor(UnixTime)");
		UnixTime Copy(Compare); //Initialize the copy constructor

//--------------UnixTime_initializationTest_4 - Was the sec value set to expectation?
		testFramework.assert(1350000 == Copy.tv.tv_sec);
		testFramework.next();

//--------------UnixTime_initializationTest_5 - Was the usec value set to expectation?
		testFramework.assert(1 == Copy.tv.tv_usec);
		testFramework.next();

//--------------UnixTime_initializationTest_6 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Copy.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("= Operator");
		UnixTime Assigned;
		Assigned = Compare;

//--------------UnixTime_initializationTest_7 - Was the sec value set to expectation?
		testFramework.assert(1350000 == Assigned.tv.tv_sec);
		testFramework.next();

//--------------UnixTime_initializationTest_8 - Was the usec value set to expectation?
		testFramework.assert(1 == Assigned.tv.tv_usec);
		testFramework.next();

//--------------UnixTime_initializationTest_9 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Assigned.getTimeSystem());

		return testFramework.countFails();
	}
	/* Test will check if UnixTime variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */
	int setFromInfoTest (void)
	{
		TestUtil testFramework( "UnixTime", "setFromInfo", __FILE__, __LINE__ );
		testFramework.init();

		UnixTime setFromInfo1;
		UnixTime setFromInfo2;
		UnixTime Compare(1350000,1,TimeSystem(2)),Compare2(0,1,TimeSystem(2));
		
		TimeTag::IdToValue Id;
		Id['U'] = "1350000";
		Id['u'] = "1";
		Id['P'] = "GPS";


//--------------UnixTime_setFromInfoTest_1 - Does a proper setFromInfo work with all information provided?
		testFramework.assert(setFromInfo1.setFromInfo(Id));
		testFramework.next();

//--------------UnixTime_setFromInfoTest_2 - Did the setFromInfo set the proper values?
		testFramework.assert(Compare == setFromInfo1);
		testFramework.next();

		Id.erase('U');

//--------------UnixTime_setFromInfoTest_3 - Does a proper setFromInfo work with missing information?
		testFramework.assert(setFromInfo2.setFromInfo(Id));
		testFramework.next();

//--------------UnixTime_setFromInfoTest_4 - Did the previous setFromInfo set the proper values?
		testFramework.assert(Compare2 == setFromInfo2);		

		return testFramework.countFails();

	}

	/* Test will check if the ways to initialize and set an UnixTime object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{
		TestUtil testFramework( "UnixTime", "== Operator", __FILE__, __LINE__ );
		testFramework.init();

		UnixTime Compare(1350000,100); // Initialize with value
		UnixTime LessThanSec(1340000, 100); //Initialize with fewer seconds
		UnixTime LessThanMicroSec(1350000,0); //Initialize with fewer microseconds
		UnixTime CompareCopy(Compare); // Initialize with copy constructor

//--------------UnixTime_operatorTest_1 - Are equivalent objects equivalent?
		testFramework.assert(Compare == CompareCopy);
		testFramework.next();

//--------------UnixTime_operatorTest_2 - Are equivalent objects equivalent?
		testFramework.assert(!(Compare == LessThanSec));
		testFramework.next();

		testFramework.changeSourceMethod("!= Operator");

//--------------UnixTime_operatorTest_3 - Are non-equivalent objects not equivalent?
		testFramework.assert(Compare != LessThanSec);
		testFramework.next();

//--------------UnixTime_operatorTest_4 - Are equivalent objects not equivalent?
		testFramework.assert(!(Compare != Compare));
		testFramework.next();

		testFramework.changeSourceMethod("< Operator");

//--------------UnixTime_operatorTest_5 - Does the < operator function when left_object < right_object by years?
		testFramework.assert(LessThanSec < Compare);
		testFramework.next();

//--------------UnixTime_operatorTest_6 - Does the < operator function when left_object > right_object by years?
		testFramework.assert(!(Compare < LessThanSec));
		testFramework.next();

//--------------UnixTime_operatorTest_7 - Does the < operator function when left_object < right_object by days?
		testFramework.assert(LessThanMicroSec < Compare);
		testFramework.next();

//--------------UnixTime_operatorTest_8 - Does the < operator function when left_object > right_object by days?		
		testFramework.assert(!(Compare < LessThanMicroSec));
		testFramework.next();


//--------------UnixTime_operatorTest_11 - Does the < operator function when left_object = right_object?
		testFramework.assert(!(Compare < CompareCopy));
		testFramework.next();

		//Greater than assertions
		testFramework.changeSourceMethod("> Operator");

//--------------UnixTime_operatorTest_12 - Does the > operator function when left_object > right_object by years?
		testFramework.assert(Compare > LessThanSec);
		testFramework.next();

//--------------UnixTime_operatorTest_13 - Does the > operator function when left_object < right_object by years?
		testFramework.assert(!(LessThanSec > Compare));
		testFramework.next();

//--------------UnixTime_operatorTest_14 - Does the > operator function when left_object > right_object by days?
		testFramework.assert(Compare > LessThanMicroSec);
		testFramework.next();

//--------------UnixTime_operatorTest_15 - Does the > operator function when left_object < right_object by days?		
		testFramework.assert(!(LessThanMicroSec > Compare));
		testFramework.next();

//--------------UnixTime_operatorTest_18 - Does the > operator function when left_object = right_object?
		testFramework.assert(!(Compare > CompareCopy));
		testFramework.next();	

		//Less than equals assertion
		testFramework.changeSourceMethod("<= Operator");

//--------------UnixTime_operatorTest_19 - Does the < operator function when left_object < right_object by years?
		testFramework.assert(LessThanSec <= Compare);
		testFramework.next();

//--------------UnixTime_operatorTest_20 - Does the <= operator function when left_object > right_object by years?
		testFramework.assert(!(Compare <= LessThanSec));
		testFramework.next();

//--------------UnixTime_operatorTest_21 - Does the <= operator function when left_object < right_object by days?
		testFramework.assert(LessThanMicroSec <= Compare);
		testFramework.next();

//--------------UnixTime_operatorTest_22 - Does the <= operator function when left_object > right_object by days?		
		testFramework.assert(!(Compare <= LessThanMicroSec));
		testFramework.next();


//--------------UnixTime_operatorTest_25 - Does the <= operator function when left_object = right_object?
		testFramework.assert(Compare <= CompareCopy);
		testFramework.next();

		//Greater than equals assertion
		testFramework.changeSourceMethod(">= Operator");

//--------------UnixTime_operatorTest_26 - Does the >= operator function when left_object > right_object by years?
		testFramework.assert(Compare >= LessThanSec);
		testFramework.next();

//--------------UnixTime_operatorTest_27 - Does the >= operator function when left_object < right_object by years?
		testFramework.assert(!(LessThanSec >= Compare));
		testFramework.next();

//--------------UnixTime_operatorTest_28 - Does the >= operator function when left_object > right_object by days?
		testFramework.assert(Compare >= LessThanMicroSec);
		testFramework.next();

//--------------UnixTime_operatorTest_29 - Does the >= operator function when left_object < right_object by days?		
		testFramework.assert(!(LessThanMicroSec >= Compare));
		testFramework.next();

//--------------UnixTime_operatorTest_32 - Does the > operator function when left_object = right_object?
		testFramework.assert(!(Compare < CompareCopy));
		testFramework.next();	

		//Validity check
		testFramework.changeSourceMethod("isValid Method");

//--------------UnixTime_operatorTest_33 - Does the isValid methods function properly?
		testFramework.assert(Compare.isValid());

		return testFramework.countFails();
	}

	/* Test will check the reset method. */
	int  resetTest (void)
	{
		TestUtil testFramework( "UnixTime", "reset" , __FILE__, __LINE__ );
		testFramework.init();

	  	UnixTime Compare(1350000,0,TimeSystem(2)); //Initialize an object

	  	Compare.reset(); // Reset it

//--------------UnixTime_operatorTest_1 - Was the time system reset to expectation?
	  	testFramework.assert(TimeSystem(0) == Compare.getTimeSystem());
	  	testFramework.next();

//--------------UnixTime_operatorTest_2 - Was the year value reset to expectation?	  	
		testFramework.assert(0 == (int)Compare.tv.tv_sec); 
		testFramework.next();

//--------------UnixTime_operatorTest_3 - Was the day usec reset to expectation?
		testFramework.assert(0 == (int)Compare.tv.tv_usec); 

		return testFramework.countFails();
	}


	/* Test will check converting to/from CommonTime. */
	int  toFromCommonTimeTest (void)
	{
		TestUtil testFramework( "UnixTime", "isValid", __FILE__, __LINE__ );
		testFramework.init();

	  	UnixTime Compare(1350000,0,TimeSystem(2)); //Initialize an object
  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

//--------------UnixTime_toFromCommonTimeTest_1 - Is the time after the BEGINNING_OF_TIME?
  		testFramework.assert(Compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);
		testFramework.next();

//--------------UnixTime_toFromCommonTimeTest_2 - Is the set object valid?
		testFramework.assert(Compare.isValid());
		testFramework.next();

  		UnixTime Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

//--------------UnixTime_toFromCommonTimeTest_3 - Is the result of conversion the same?
		testFramework.assert(Test2 == Compare);
		testFramework.next();

//--------------UnixTime_toFromCommonTimeTest_4 - Is the time system after conversion what is expected?
		testFramework.assert(Compare.getTimeSystem()==TimeSystem(2));
		testFramework.next();

//--------------UnixTime_toFromCommonTimeTest_5 - Is the year after conversion what is expected?
		testFramework.assert(1350000 == (int)Compare.tv.tv_sec);
		testFramework.next();

//--------------UnixTime_toFromCommonTimeTest_6 - Is the day after conversion what is expected?
		testFramework.assert(0 == (int)Compare.tv.tv_usec);

		return testFramework.countFails();
	}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int  timeSystemTest (void)
	{
		TestUtil testFramework( "UnixTime", "Differing TimeSystem == Operator", __FILE__, __LINE__ );
		testFramework.init();

		UnixTime GPS1(1350000,0,TimeSystem::GPS);
		UnixTime GPS2(1340000,0,TimeSystem::GPS);
		UnixTime UTC1(1350000,0,TimeSystem::UTC);
		UnixTime UNKNOWN(1350000,0,TimeSystem::Unknown);
		UnixTime ANY(1350000,0,TimeSystem::Any);

//--------------UnixTime_timeSystemTest_1 - Verify same Time System but different time inequality
		testFramework.assert(!(GPS1 == GPS2));
		testFramework.next();

//--------------UnixTime_timeSystemTest_2 - Verify same Time System equality
		testFramework.assert(GPS1.getTimeSystem() == GPS2.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("Differing TimeSystem != Operator");
//--------------UnixTime_timeSystemTest_3 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UTC1);
		testFramework.next();

//--------------UnixTime_timeSystemTest_4 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UNKNOWN);
		testFramework.next();
		
		testFramework.changeSourceMethod("ANY TimeSystem == Operator");		
//--------------UnixTime_timeSystemTest_5 - Verify TimeSystem=ANY does not matter in TimeSystem=GPS comparisons 
		testFramework.assert(GPS1 == ANY);
		testFramework.next();

//--------------UnixTime_timeSystemTest_6 - Verify TimeSystem=ANY does not matter in TimeSystem=UTC comparisons 
		testFramework.assert(UTC1 == ANY);
		testFramework.next();

//--------------UnixTime_timeSystemTest_7 - Verify TimeSystem=ANY does not matter in TimeSystem=UNKOWN comparisons 
		testFramework.assert(UNKNOWN == ANY);
		testFramework.next();

		testFramework.changeSourceMethod("ANY TimeSystem < Operator");	
//--------------UnixTime_timeSystemTest_8 - Verify TimeSystem=ANY does not matter in other operator comparisons 
		testFramework.assert(!(GPS2 == ANY) && (GPS2 < ANY));
		testFramework.next();

		testFramework.changeSourceMethod("setTimeSystem");	
  		UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem

//--------------UnixTime_timeSystemTest_9 - Ensure resetting a Time System changes it
		testFramework.assert(UNKNOWN.getTimeSystem()==TimeSystem(2));

		return testFramework.countFails();
	}
	/* Test for the formatted printing of UnixTime objects */
	int  printfTest (void)
	{
		TestUtil testFramework( "UnixTime", "printf", __FILE__, __LINE__ );
		testFramework.init();

  		UnixTime GPS1(1350000,0,TimeSystem(2));
  		UnixTime UTC1(1350000,0,TimeSystem(7));
		
		//--------------UnixTime_printfTest_1 - Verify printed output matches expectation		
  		testFramework.assert(GPS1.printf("%07U %02u %02P") == (std::string)"1350000 00 GPS");
  		testFramework.next();

  		//--------------UnixTime_printfTest_2 - Verify printed output matches expectation
  		testFramework.assert(UTC1.printf("%07U %02u %02P") == (std::string)"1350000 00 UTC");
  		testFramework.next();

  		//--------------UnixTime_printfTest_3 - Verify printed error message matches expectation
  		testFramework.assert(GPS1.printError("%07U %02u %02P") == (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime"); 
  		testFramework.next();

  		//--------------UnixTime_printfTest_4 - Verify printed error message matches expectation
  		testFramework.assert(UTC1.printError("%07U %02u %02P") == (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime");
		
		return testFramework.countFails();
	}
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	UnixTime_T testClass;

	check = testClass.initializationTest();
	errorCounter += check;

	check = testClass.operatorTest();
    errorCounter += check;

	check = testClass.setFromInfoTest();
    errorCounter += check;

	check = testClass.resetTest();
    errorCounter += check;

	check = testClass.timeSystemTest();
    errorCounter += check;

	check = testClass.toFromCommonTimeTest();
    errorCounter += check;

	check = testClass.printfTest();
    errorCounter += check;
	
	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;


	return errorCounter; //Return the total number of errors
}
