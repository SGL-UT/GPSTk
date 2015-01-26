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

#include "MJD.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

class MJD_T
{
	public:

	int  initializationTest (void)
	{
		TestUtil testFramework( "MJD", "Constructor", __FILE__, __LINE__ );
		testFramework.init();

	  	MJD Compare(135000.0,TimeSystem(2)); //Initialize an object

//--------------MJD_initializationTest_1 - Was the jd value set to expectation?
		testFramework.assert(135000.0 == Compare.mjd);
		testFramework.next();

//--------------MJD_initializationTest_2 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("Constructor(MJD)");
		MJD Copy(Compare); // Initialize with copy constructor

//--------------MJD_initializationTest_3 - Was the jd value set to expectation?
		//Compare.jd==135000
		testFramework.assert(135000.0 == Copy.mjd);
		testFramework.next();

//--------------MJD_initializationTest_4 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Copy.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("= Operator");
		MJD Assigned;
		Assigned = Compare;

//--------------MJD_initializationTest_5 - Was the jd value set to expectation?
		//Compare.jd==135000
		testFramework.assert(135000.0 == Assigned.mjd);
		testFramework.next();

//--------------MJD_initializationTest_6 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Assigned.getTimeSystem());

		return testFramework.countFails();
	}


	/* Test will check if MJD variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */

	int setFromInfoTest (void)
	{
		TestUtil testFramework( "MJD", "setFromInfo", __FILE__, __LINE__ );
		testFramework.init();

		MJD setFromInfo1;
		MJD setFromInfo2;
		MJD Compare(135000.0,TimeSystem(2)), Compare2(0.0,TimeSystem(2));
		TimeTag::IdToValue Id;
		Id['Q'] = "135000.0";
		Id['P'] = "GPS";
//--------------MJD_setFromInfoTest_1 - Does a proper setFromInfo work with all information provided?
		testFramework.assert(setFromInfo1.setFromInfo(Id));
		testFramework.next();

//--------------MJD_setFromInfoTest_2 - Did the setFromInfo set the proper values?
		testFramework.assert(Compare == setFromInfo1);
		testFramework.next();

		Id.erase('Q');

//--------------MJD_setFromInfoTest_3 - Does a proper setFromInfo work with missing information?
		testFramework.assert(setFromInfo2.setFromInfo(Id));
		testFramework.next();

//--------------MJD_setFromInfoTest_4 - Did the previous setFromInfo set the proper values?
		testFramework.assert(Compare2 == setFromInfo2);		

		return testFramework.countFails();
	}

	/* Test will check if the ways to initialize and set an MJD object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{
		TestUtil testFramework( "MJD", "== Operator", __FILE__, __LINE__ );
		testFramework.init();

		gpstk::MJD Compare(135000); // Initialize with value
		gpstk::MJD LessThanMJD(134000); // Initialize with value
		gpstk::MJD CompareCopy(Compare); // Initialize with copy constructor
		gpstk::MJD CompareCopy2; //Empty initialization
		CompareCopy2 = CompareCopy; //Assignment


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	Why have CompareCopy2? Never called. Also, LessThanZCount was only called once
//	in the original test. Shouldn't it be called more for more rigorous testing?
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


//--------------MJD_operatorTest_1 - Are equivalent objects equivalent?
		testFramework.assert(Compare == CompareCopy);
		testFramework.next();

//--------------MJD_operatorTest_2 - Are equivalent objects equivalent?
		testFramework.assert(!(Compare == LessThanMJD));
		testFramework.next();

		testFramework.changeSourceMethod("!= Operator");
//--------------MJD_operatorTest_3 - Are non-equivalent objects not equivalent?
		testFramework.assert(Compare != LessThanMJD);
		testFramework.next();

//--------------MJD_operatorTest_4 - Are equivalent objects not equivalent?
		testFramework.assert(!(Compare != Compare));
		testFramework.next();

		testFramework.changeSourceMethod("< Operator");
//--------------MJD_operatorTest_5 - Does the < operator function when left_object < right_object?
		testFramework.assert(LessThanMJD < Compare);
		testFramework.next();

//--------------MJD_operatorTest_6 - Does the < operator function when left_object > right_object?
		testFramework.assert(!(Compare < LessThanMJD));
		testFramework.next();

//--------------MJD_operatorTest_7 - Does the < operator function when left_object = right_object?
		testFramework.assert(!(Compare < CompareCopy));
		testFramework.next();

		testFramework.changeSourceMethod("> Operator");
//--------------MJD_operatorTest_8 - Does the > operator function when left_object < right_object?
		testFramework.assert(!(LessThanMJD > Compare));
		testFramework.next();

//--------------MJD_operatorTest_9 - Does the > operator function when left_object > right_object?
		testFramework.assert(Compare > LessThanMJD);
		testFramework.next();

//--------------MJD_operatorTest_10 - Does the > operator function when left_object = right_object?
		testFramework.assert(!(Compare > CompareCopy));
		testFramework.next();

		testFramework.changeSourceMethod("<= Operator");
//--------------MJD_operatorTest_11 - Does the <= operator function when left_object < right_object?
		testFramework.assert(LessThanMJD <= Compare);
		testFramework.next();

//--------------MJD_operatorTest_12 - Does the <= operator function when left_object > right_object?
		testFramework.assert(!(Compare <= LessThanMJD));
		testFramework.next();

//--------------MJD_operatorTest_13 - Does the <= operator function when left_object = right_object?
		testFramework.assert(Compare <= CompareCopy);
		testFramework.next();

		testFramework.changeSourceMethod(">= Operator");
//--------------MJD_operatorTest_14 - Does the >= operator function when left_object < right_object?
		testFramework.assert(!(LessThanMJD >= Compare));
		testFramework.next();

//--------------MJD_operatorTest_15 - Does the >= operator function when left_object > right_object?
		testFramework.assert(Compare >= LessThanMJD);
		testFramework.next();

//--------------MJD_operatorTest_16 - Does the >= operator function when left_object = right_object?
		testFramework.assert(Compare >= CompareCopy);

		return testFramework.countFails();
	}

	/* Test will check the reset method. */
	int resetTest (void)
	{
		TestUtil testFramework( "MJD", "reset", __FILE__, __LINE__ );
		testFramework.init();

	  	MJD Compare(135000,TimeSystem(2)); //Initialize an object

	  	Compare.reset(); // Reset it

//--------------MJD_resetTest_1 - Was the mjd value reset to expectation?	
	  	testFramework.assert(0 == Compare.mjd);
	  	testFramework.next();

//--------------MJD_resetTest_2 - Was the time system reset to expectation?
	  	testFramework.assert(TimeSystem(0) == Compare.getTimeSystem()); 

		return testFramework.countFails();
	}

	/* Test will check converting to/from CommonTime. */
	int toFromCommonTimeTest (void)
	{
		TestUtil testFramework( "MJD", "isValid", __FILE__, __LINE__ );
		testFramework.init();

	  	MJD Compare(135000,TimeSystem(2)); //Initialize an object

//--------------MJD_toFromCommonTimeTest_1 - Is the time after the BEGINNING_OF_TIME?
		testFramework.assert(Compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);
		testFramework.next();

//--------------MJD_toFromCommonTimeTest_2 - Is the set object valid?
		testFramework.assert(Compare.isValid());
		testFramework.next();

  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

  		MJD Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

		testFramework.changeSourceMethod("CommonTime Conversion");
//--------------MJD_toFromCommonTimeTest_3 - Is the result of conversion the same?
		testFramework.assert(Test2 == Compare);
		testFramework.next();

//--------------MJD_toFromCommonTimeTest_4 - Is the time system after conversion what is expected?
		testFramework.assert(Compare.getTimeSystem()==TimeSystem(2));
		testFramework.next();

//--------------MJD_toFromCommonTimeTest_5 - Is the time after conversion what is expected?
		//Compare.jd==0
		testFramework.assert(135000 == Compare.mjd);// < eps && Compare.mjd - 135000 < eps);

		return testFramework.countFails();
	}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int timeSystemTest (void)
	{
		TestUtil testFramework( "MJD", "Differing TimeSystem == Operator", __FILE__, __LINE__ );
		testFramework.init();

  		MJD GPS1(135000,TimeSystem(2));
  		MJD GPS2(134000,TimeSystem(2));
  		MJD UTC1(135000,TimeSystem(5));
  		MJD UNKNOWN(135000,TimeSystem(0));
  		MJD ANY(135000,TimeSystem(1));

//--------------MJD_timeSystemTest_1 - Verify same Time System but different time inequality
		testFramework.assert(!(GPS1 == GPS2));
		testFramework.next();

//--------------MJD_timeSystemTest_2 - Verify same Time System equality
		testFramework.assert(GPS1.getTimeSystem() == GPS2.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("Differing TimeSystem != Operator");
//--------------MJD_timeSystemTest_3 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UTC1);
		testFramework.next();

//--------------MJD_timeSystemTest_4 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UNKNOWN);
		testFramework.next();

		testFramework.changeSourceMethod("ANY TimeSystem == Operator");		
//--------------MJD_timeSystemTest_5 - Verify TimeSystem=ANY does not matter in TimeSystem=GPS comparisons 
		testFramework.assert(GPS1 == ANY);
		testFramework.next();

//--------------MJD_timeSystemTest_6 - Verify TimeSystem=ANY does not matter in TimeSystem=UTC comparisons 
		testFramework.assert(UTC1 == ANY);
		testFramework.next();

//--------------MJD_timeSystemTest_7 - Verify TimeSystem=ANY does not matter in TimeSystem=UNKOWN comparisons 
		testFramework.assert(UNKNOWN == ANY);
		testFramework.next();

		testFramework.changeSourceMethod("ANY TimeSystem < Operator");	
//--------------MJD_timeSystemTest_8 - Verify TimeSystem=ANY does not matter in other operator comparisons 
		testFramework.assert(!(GPS2 == ANY) && (GPS2 < ANY));
		testFramework.next();

		testFramework.changeSourceMethod("setTimeSystem");	
  		UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem
//--------------MJD_timeSystemTest_9 - Ensure resetting a Time System changes it
		testFramework.assert(UNKNOWN.getTimeSystem()==TimeSystem(2));

		return testFramework.countFails();
	}

	/* Test for the formatted printing of MJD objects */
	int printfTest (void)
	{
		TestUtil testFramework( "MJD", "printf", __FILE__, __LINE__ );
		testFramework.init();

  		MJD GPS1(135000,TimeSystem(2));
  		MJD UTC1(135000,TimeSystem(7));
		
//--------------MJD_printfTest_1 - Verify printed output matches expectation
		testFramework.assert(GPS1.printf("%08Q %02P") == (std::string)"135000.000000 GPS");
		testFramework.next();

//--------------MJD_printfTest_2 - Verify printed output matches expectation
		testFramework.assert(UTC1.printf("%08Q %02P") == (std::string)"135000.000000 UTC");
		testFramework.next();

		testFramework.changeSourceMethod("printError");	
//--------------MJD_printfTest_3 - Verify printed error message matches expectation
		testFramework.assert(GPS1.printError("%08Q %02P") == (std::string)"ErrorBadTime ErrorBadTime");
		testFramework.next();

//--------------MJD_printfTest_4 - Verify printed error message matches expectation
		testFramework.assert(UTC1.printError("%08Q %02P") == (std::string)"ErrorBadTime ErrorBadTime");

		return testFramework.countFails();
	}
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	MJD_T testClass;

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
