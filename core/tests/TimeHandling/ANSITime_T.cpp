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

#include "ANSITime.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace gpstk;
using namespace std;


class ANSITime_T
{
	public:
	ANSITime_T() {eps = 1E-12;}
	~ANSITime_T() {}
/* =========================================================================================================================
	initializationTest ensures the constructors set the values properly
========================================================================================================================= */
	int  initializationTest (void)
	{
		TestUtil testFramework( "ANSITime", "Constructor", __FILE__, __LINE__ );


	  	ANSITime Compare(13500000,TimeSystem(2)); //Initialize an object
		//=====================================================================
		//Was the time value set to expectation using the explicit constructor?
		//=====================================================================
		testFramework.assert(13500000 == (int)Compare.time, "Explicit constructor did not set the time value properly", __LINE__);


		//=====================================================================
		//Was the time system set to expectation using the explicit constructor?
		//=====================================================================
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem(), "Explicit constructor did not set the time system properly", __LINE__);


		ANSITime Copy(Compare); // Initialize with copy constructor
		//=====================================================================
		//Was the time value set to expectation using the copy constructor?
		//=====================================================================
		testFramework.assert(13500000 == (int)Copy.time, "Copy constructor did not set the time value properly", __LINE__);


		//=====================================================================
		//Was the time system set to expectation using the copy constructor?
		//=====================================================================
		testFramework.assert(TimeSystem(2) == Copy.getTimeSystem(), "Copy constructor did not set the time system properly", __LINE__);


		testFramework.changeSourceMethod("OperatorEquals");
		ANSITime Assigned;
		Assigned = Compare;
		//=====================================================================
		//Was the time value set to expectation using the equals operator?
		//=====================================================================
		testFramework.assert(13500000 == (int)Assigned.time, "Equals Operator did not set the time value properly", __LINE__);


		//=====================================================================
		//Was the time value system to expectation using the equals operator?
		//=====================================================================
		testFramework.assert(TimeSystem(2) == Assigned.getTimeSystem(), "Equals Operator did not set the time system properly", __LINE__);

		return testFramework.countFails();
	}

/* =========================================================================================================================
	setFromInfoTest will check if ANSITime variable can be set from a map.
	setFromInfoTest also implicity tests whether the != operator functions.
========================================================================================================================= */
	int setFromInfoTest (void)
	{
		TestUtil testFramework( "ANSITime", "setFromInfo", __FILE__, __LINE__ );

		ANSITime setFromInfo1;
		ANSITime setFromInfo2;
		ANSITime Compare(13500000,TimeSystem(2)), Compare2(0,TimeSystem(2));
		TimeTag::IdToValue Id;
		Id['K'] = "13500000";
		Id['P'] = "GPS";

		//=====================================================================
		//Does a proper setFromInfo work with all information provided?
		//=====================================================================
		testFramework.assert(setFromInfo1.setFromInfo(Id), "setFromInfo experienced an error and returned false", __LINE__);

		//=====================================================================
		//Did the setFromInfo set the proper values?
		//=====================================================================
		testFramework.assert(Compare == setFromInfo1, "setFromInfo did not set a value properly", __LINE__);

		Id.erase('K');
		//=====================================================================
		//Does a proper setFromInfo work with missing information?
		//=====================================================================
		testFramework.assert(setFromInfo2.setFromInfo(Id), "setFromInfo experienced an error and returned false", __LINE__);

		//=====================================================================
		//Did the previous setFromInfo set the proper values?
		//=====================================================================
		testFramework.assert(Compare2 == setFromInfo2, "setFromInfo did not set a value properly", __LINE__);		

		return testFramework.countFails();
	}

/* =========================================================================================================================
	operatorTest will check if the ways to initialize and set an ANSITime object.
	operatorTest also tests whether the comparison operators and isValid method function.
========================================================================================================================= */
	int operatorTest (void)
	{
		TestUtil testFramework( "ANSITime", "Equivalence Operator", __FILE__, __LINE__ );


		ANSITime Compare(13500000); // Initialize with value
		ANSITime LessThan(13400000); // Initialize with value
		ANSITime CompareCopy(Compare); // Initialize with copy constructor
		ANSITime CompareCopy2; //Empty initialization
		CompareCopy2 = CompareCopy; //Assignment

		//=====================================================================
		//Are equivalent objects equivalent?
		//=====================================================================
		testFramework.assert(Compare == CompareCopy, "Equivalence Operator found equivalent objects as not equivalent", __LINE__);

		//=====================================================================
		//Are non-equivalent objects equivalent?
		//=====================================================================
		testFramework.assert(!(Compare == LessThan), "Equivalence Operator found non-equivalent objects as equivalent", __LINE__);


		testFramework.changeSourceMethod("Not Equals Operator");
		//=====================================================================
		//Are non-equivalent objects not equivalent?
		//=====================================================================
		testFramework.assert(Compare != LessThan, "Not-Equals Operator found non-equivalent objects as equivalent", __LINE__);

		//=====================================================================
		//Are equivalent objects not equivalent?
		//=====================================================================
		testFramework.assert(!(Compare != Compare), "Not-Equals Operator found equivalent objects as not equivalent", __LINE__);


		testFramework.changeSourceMethod("LessThan Operator");
		//=====================================================================
		//Does the < operator function when left_object < right_object?
		//=====================================================================
		testFramework.assert(LessThan < Compare, "Less-Than Operator found a smaller time as not less-than", __LINE__);

		//=====================================================================
		//Does the < operator function when left_object > right_object?
		//=====================================================================
		testFramework.assert(!(Compare < LessThan), "Less-Than Operator found a greater time as less-than", __LINE__);

		//=====================================================================
		//Does the < operator function when left_object = right_object?
		//=====================================================================
		testFramework.assert(!(Compare < CompareCopy), "Less-Than Operator found an equivalent time as less-than", __LINE__);


		testFramework.changeSourceMethod("GreaterThan Operator");
		//=====================================================================
		//Does the > operator function when left_object < right_object?
		//=====================================================================
		testFramework.assert(!(LessThan > Compare), "Greater-Than Operator found a smaller time as greater-than", __LINE__);

		//=====================================================================
		//Does the > operator function when left_object > right_object?
		//=====================================================================
		testFramework.assert(Compare > LessThan, "Greater-Than Operator found a greater time as not greater-than", __LINE__);

		//=====================================================================
		//Does the > operator function when left_object = right_object?
		//=====================================================================
		testFramework.assert(!(Compare > CompareCopy), "Greater-Than Operator found an equivalent time as greater-than", __LINE__);


		testFramework.changeSourceMethod("LessThanOrEqualTo Operator");
		//=====================================================================
		//Does the <= operator function when left_object < right_object?
		//=====================================================================
		testFramework.assert(LessThan <= Compare, "Less-Than-Or-Equal-To Operator found a smaller time as not less-than-or-equal-to", __LINE__);

		//=====================================================================
		//Does the <= operator function when left_object > right_object?
		//=====================================================================
		testFramework.assert(!(Compare <= LessThan), "Less-Than-Or-Equal-To Operator found a greater time as less-than-or-equal-to", __LINE__);

		//=====================================================================
		//Does the <= operator function when left_object = right_object?
		//=====================================================================
		testFramework.assert(Compare <= CompareCopy, "Less-Than-Or-Equal-To Operator found an equivalent time as not less-than-or-equal-to", __LINE__);


		testFramework.changeSourceMethod("GreaterThanOrEqualTo Operator");
		//=====================================================================
		//Does the >= operator function when left_object < right_object?
		//=====================================================================
		testFramework.assert(!(LessThan >= Compare), "Greater-Than-Or-Equal-To Operator found a smaller time as greater-than-or-equal-to", __LINE__);

		//=====================================================================
		//Does the >= operator function when left_object > right_object?
		//=====================================================================
		testFramework.assert(Compare >= LessThan, "Greater-Than-Or-Equal-To Operator found a greater time as not greater-than-or-equal-to", __LINE__);

		//=====================================================================
		//Does the >= operator function when left_object = right_object?
		//=====================================================================
		testFramework.assert(Compare >= CompareCopy, "Greater-Than-Or-Equal-To Operator found an equivalent time as not greater-than-or-equal-to", __LINE__);

		return testFramework.countFails();
	}


/* =========================================================================================================================
	resetTest will check the reset method.
========================================================================================================================= */
	int  resetTest (void)
	{
		TestUtil testFramework( "ANSITime", "reset", __FILE__, __LINE__ );


	  	ANSITime Compare(13500000,TimeSystem(2)); //Initialize an object

	  	Compare.reset(); // Reset it
		//=====================================================================
		//Was the time value reset to expectation?
		//=====================================================================
		testFramework.assert(0 == (int)Compare.time, "Reset did not set the time to its default value (0)", __LINE__);

		//=====================================================================
		//Was the time system reset to expectation?
		//=====================================================================
		testFramework.assert(TimeSystem(0) == Compare.getTimeSystem(), "Reset did not set the time system to its default value (Unknown)", __LINE__);

		return testFramework.countFails();
	}


/* =========================================================================================================================
	toFromCommonTimeTest will check converting to/from CommonTime.
========================================================================================================================= */
	int  toFromCommonTimeTest (void)
	{
		TestUtil testFramework( "ANSITime", "isValid", __FILE__, __LINE__ );


	  	ANSITime Compare(13500000,TimeSystem(2)); //Initialize an object
		//=====================================================================
		//Is the time after the BEGINNING_OF_TIME?
		//=====================================================================
  		testFramework.assert(Compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME, "Time provided is found to be earlier than the BEGINNING_OF_TIME object", __LINE__);

		//=====================================================================
		//Is the set object valid?
		//=====================================================================
		testFramework.assert(Compare.isValid(), "Time provided is found to not be valid for CommonTime conversions", __LINE__);


  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

  		ANSITime Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

		testFramework.changeSourceMethod("CommonTime Conversion");
		//=====================================================================
		//Is the result of conversion the same?
		//=====================================================================
		testFramework.assert(Test2 == Compare, "Conversion to/from CommonTime changed a value", __LINE__);

		//=====================================================================
		//Is the time system after conversion what is expected?
		//=====================================================================
		testFramework.assert(Compare.getTimeSystem()==TimeSystem(2), "Conversion to/from CommonTime changed the time system", __LINE__);

		//=====================================================================
		//Is the time after conversion what is expected?
		//=====================================================================
		testFramework.assert(13500000 == (int)Compare.time, "Conversion to/from CommonTime changed the time", __LINE__);

		return testFramework.countFails();
	}


/* =========================================================================================================================
	Test will check the TimeSystem comparisons when using the comparison operators.
========================================================================================================================= */
	int  timeSystemTest (void)
	{
		TestUtil testFramework( "ANSITime", "Equivalence Operator With Differing TimeSystem", __FILE__, __LINE__ );


  		ANSITime GPS1(13500000,TimeSystem(2));
  		ANSITime GPS2(13400000,TimeSystem(2));
  		ANSITime UTC1(13500000,TimeSystem(7));
  		ANSITime UNKNOWN(13500000,TimeSystem(0));
  		ANSITime ANY(13500000,TimeSystem(1));

		//=====================================================================
		//Verify same Time System but different time inequality
		//=====================================================================
		testFramework.assert(!(GPS1 == GPS2), "Equivalence operator did not find the differing time values", __LINE__);

		//=====================================================================
		//Verify different Time System but same time inequality
		//=====================================================================
		testFramework.assert(!(GPS1 == UTC1), "Equivalence operator did not find the differing TimeSystems", __LINE__);

		//=====================================================================
		//Verify same Time System equality
		//=====================================================================
		testFramework.assert(GPS1.getTimeSystem() == GPS2.getTimeSystem(), "Equivalence Operator did not find the equivalent TimeSystems", __LINE__);


		testFramework.changeSourceMethod("NotEquals Operator With Differing TimeSystem");
		//=====================================================================
		//Verify different Time System but same time inequality
		//=====================================================================
		testFramework.assert(GPS1 != UTC1, "Not-equals operator did not find the differing TimeSystems", __LINE__);

		//=====================================================================
		//Verify different Time System but same time inequality
		//=====================================================================
		testFramework.assert(GPS1 != UNKNOWN, "Not-equals operator did not find the differing TimeSystems", __LINE__);


		testFramework.changeSourceMethod("Equivalence Operator With ANY TimeSystem");		
		//=====================================================================
		//Verify TimeSystem=ANY does not matter in TimeSystem=GPS comparisons 
		//=====================================================================
		testFramework.assert(GPS1 == ANY, "Equivalence operator found equivalent ANY object as not equals", __LINE__);

		//=====================================================================
		//Verify TimeSystem=ANY does not matter in TimeSystem=UTC comparisons 
		//=====================================================================
		testFramework.assert(UTC1 == ANY, "Equivalence operator found equivalent ANY object as not equals", __LINE__);

		//=====================================================================
		//Verify TimeSystem=ANY does not matter in TimeSystem=UNKOWN comparisons 
		//=====================================================================
		testFramework.assert(UNKNOWN == ANY, "Equivalence operator found equivalent ANY object as not equals", __LINE__);


		testFramework.changeSourceMethod("LessThan Operator With ANY TimeSystem");	
		//=====================================================================
		//Verify TimeSystem=ANY does not matter in other operator comparisons 
		//=====================================================================
		testFramework.assert(!(GPS2 == ANY) && (GPS2 < ANY), "Less-than operator did not find less than object due to Any TimeSystem", __LINE__);


		testFramework.changeSourceMethod("setTimeSystem");	
  		UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem
		//=====================================================================
		//Ensure resetting a Time System changes it
		//=====================================================================
		testFramework.assert(UNKNOWN.getTimeSystem()==TimeSystem(2), "setTimeSystem did not set the proper TimeSystem", __LINE__);

		return testFramework.countFails();
	}


/* =========================================================================================================================
	Test for the formatted printing of ANSITime objects
========================================================================================================================= */
	int  printfTest (void)
	{
		TestUtil testFramework( "ANSITime", "printf", __FILE__, __LINE__ );


  		ANSITime GPS1(13500000,TimeSystem(2));
  		ANSITime UTC1(13500000,TimeSystem(7));

		//=====================================================================
		//Verify printed output matches expectation
		//=====================================================================
		testFramework.assert(GPS1.printf("%08K %02P") == (std::string)"13500000 GPS", "printf did not output in the proper format", __LINE__);

		//=====================================================================
		//Verify printed output matches expectation
		//=====================================================================
		testFramework.assert(UTC1.printf("%08K %02P") == (std::string)"13500000 UTC", "printf did not output in the proper format", __LINE__);


		testFramework.changeSourceMethod("printError");	
		//=====================================================================
		//Verify printed error message matches expectation
		//=====================================================================
		testFramework.assert(GPS1.printError("%08K %02P") == (std::string)"ErrorBadTime ErrorBadTime", "printError did not output in the proper format", __LINE__);

		//=====================================================================
		//Verify printed error message matches expectation
		//=====================================================================
		testFramework.assert(UTC1.printError("%08K %02P") == (std::string)"ErrorBadTime ErrorBadTime", "printError did not output in the proper format", __LINE__);

		return testFramework.countFails();
	}

	private:
		double eps;
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	ANSITime_T testClass;

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
