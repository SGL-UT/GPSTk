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
//==========================================================================================================================
//	initializationTest ensures the constructors set the values properly
//==========================================================================================================================
	int  initializationTest (void)
	{
		TestUtil testFramework( "ANSITime", "Constructor", __FILE__, __LINE__ );


	  	ANSITime Compare(13500000,TimeSystem(2)); //Initialize an object
		//---------------------------------------------------------------------
		//Were the values set to expectation using the explicit constructor?
		//---------------------------------------------------------------------
		testFramework.assert(13500000 == (int)Compare.time,            "Explicit constructor did not set the time value properly",  __LINE__);
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem(), "Explicit constructor did not set the time system properly", __LINE__);

		testFramework.changeSourceMethod("ConstructorCopy");
		ANSITime Copy(Compare); // Initialize with copy constructor
		//---------------------------------------------------------------------
		//Was the values set to expectation using the copy constructor?
		//---------------------------------------------------------------------
		testFramework.assert(13500000 == (int)Copy.time,            "Copy constructor did not set the time value properly",  __LINE__);
		testFramework.assert(TimeSystem(2) == Copy.getTimeSystem(), "Copy constructor did not set the time system properly", __LINE__);


		testFramework.changeSourceMethod("OperatorSet");
		ANSITime Assigned;
		Assigned = Compare;
		//---------------------------------------------------------------------
		//Was the values set to expectation using the Set operator?
		//---------------------------------------------------------------------
		testFramework.assert(13500000 == (int)Assigned.time,            "Set Operator did not set the time value properly",  __LINE__);
		testFramework.assert(TimeSystem(2) == Assigned.getTimeSystem(), "Set Operator did not set the time system properly", __LINE__);

		return testFramework.countFails();
	}

//==========================================================================================================================
//	setFromInfoTest will check if ANSITime variable can be set from a map.
//	setFromInfoTest also implicity tests whether the != operator functions.
//==========================================================================================================================
	int setFromInfoTest (void)
	{
		TestUtil testFramework( "ANSITime", "setFromInfo", __FILE__, __LINE__ );

		ANSITime setFromInfo1;
		ANSITime setFromInfo2;
		ANSITime Compare(13500000,TimeSystem(2)), Compare2(0,TimeSystem(2));
		TimeTag::IdToValue Id;
		Id['K'] = "13500000";
		Id['P'] = "GPS";

		//---------------------------------------------------------------------
		//Does a proper setFromInfo work with all information provided?
		//---------------------------------------------------------------------
		testFramework.assert(setFromInfo1.setFromInfo(Id), "setFromInfo experienced an error and returned false", __LINE__);

		Id.erase('K');
		testFramework.assert(setFromInfo2.setFromInfo(Id), "setFromInfo experienced an error and returned false", __LINE__);

		//---------------------------------------------------------------------
		//Did the setFromInfo set the proper values?
		//---------------------------------------------------------------------
		testFramework.assert(Compare == setFromInfo1, "setFromInfo did not set a value properly", __LINE__);
		testFramework.assert(Compare2 == setFromInfo2, "setFromInfo did not set a value properly", __LINE__);	
	

		return testFramework.countFails();
	}

//==========================================================================================================================
//	operatorTest will check if the ways to initialize and set an ANSITime object.
//	operatorTest also tests whether the comparison operators and isValid method function.
//==========================================================================================================================
	int operatorTest (void)
	{
		TestUtil testFramework( "ANSITime", "OperatorEquivalent", __FILE__, __LINE__ );


		ANSITime Compare(13500000); // Initialize with value
		ANSITime LessThan(13400000); // Initialize with value
		ANSITime CompareCopy(Compare); // Initialize with copy constructor

		//---------------------------------------------------------------------
		//Does the == Operator function?
		//---------------------------------------------------------------------
		testFramework.assert(Compare == CompareCopy, "Equivalence Operator found equivalent objects as not equivalent", __LINE__);
		testFramework.assert(!(Compare == LessThan), "Equivalence Operator found non-equivalent objects as equivalent", __LINE__);


		testFramework.changeSourceMethod("OperatorNotEquivalent");
		//---------------------------------------------------------------------
		//Does the != operator function
		//---------------------------------------------------------------------
		testFramework.assert(Compare != LessThan, "Not-Equals Operator found non-equivalent objects as equivalent", __LINE__);
		testFramework.assert(!(Compare != Compare), "Not-Equals Operator found equivalent objects as not equivalent", __LINE__);


		testFramework.changeSourceMethod("OperatorLessThan");
		//---------------------------------------------------------------------
		//Does the < operator function?
		//---------------------------------------------------------------------
		testFramework.assert(LessThan < Compare, "Less-Than Operator found a smaller time as not less-than", __LINE__);
		testFramework.assert(!(Compare < LessThan), "Less-Than Operator found a greater time as less-than", __LINE__);
		testFramework.assert(!(Compare < CompareCopy), "Less-Than Operator found an equivalent time as less-than", __LINE__);


		testFramework.changeSourceMethod("OperatorGreaterThan");
		//---------------------------------------------------------------------
		//Does the > operator function?
		//---------------------------------------------------------------------
		testFramework.assert(!(LessThan > Compare), "Greater-Than Operator found a smaller time as greater-than", __LINE__);
		testFramework.assert(Compare > LessThan, "Greater-Than Operator found a greater time as not greater-than", __LINE__);
		testFramework.assert(!(Compare > CompareCopy), "Greater-Than Operator found an equivalent time as greater-than", __LINE__);


		testFramework.changeSourceMethod("OperatorLessThanOrEqualTo");
		//---------------------------------------------------------------------
		//Does the <= operator function?
		//---------------------------------------------------------------------
		testFramework.assert(LessThan <= Compare, "Less-Than-Or-Equal-To Operator found a smaller time as not less-than-or-equal-to", __LINE__);
		testFramework.assert(!(Compare <= LessThan), "Less-Than-Or-Equal-To Operator found a greater time as less-than-or-equal-to", __LINE__);
		testFramework.assert(Compare <= CompareCopy, "Less-Than-Or-Equal-To Operator found an equivalent time as not less-than-or-equal-to", __LINE__);


		testFramework.changeSourceMethod("OperatorGreaterThanOrEqualTo");
		//---------------------------------------------------------------------
		//Does the >= operator function?
		//---------------------------------------------------------------------
		testFramework.assert(!(LessThan >= Compare), "Greater-Than-Or-Equal-To Operator found a smaller time as greater-than-or-equal-to", __LINE__);
		testFramework.assert(Compare >= LessThan, "Greater-Than-Or-Equal-To Operator found a greater time as not greater-than-or-equal-to", __LINE__);
		testFramework.assert(Compare >= CompareCopy, "Greater-Than-Or-Equal-To Operator found an equivalent time as not greater-than-or-equal-to", __LINE__);

		return testFramework.countFails();
	}


//==========================================================================================================================
//	resetTest will check the reset method.
//==========================================================================================================================
	int  resetTest (void)
	{
		TestUtil testFramework( "ANSITime", "reset", __FILE__, __LINE__ );


	  	ANSITime Compare(13500000,TimeSystem(2)); //Initialize an object

	  	Compare.reset(); // Reset it
		//---------------------------------------------------------------------
		//Were the attributes reset to expectation
		//---------------------------------------------------------------------
		testFramework.assert(0 == (int)Compare.time, "Reset did not set the time to its default value (0)", __LINE__);
		testFramework.assert(TimeSystem(0) == Compare.getTimeSystem(), "Reset did not set the time system to its default value (Unknown)", __LINE__);

		return testFramework.countFails();
	}


//==========================================================================================================================
//	toFromCommonTimeTest will check converting to/from CommonTime.
//==========================================================================================================================
	int  toFromCommonTimeTest (void)
	{
		TestUtil testFramework( "ANSITime", "isValid", __FILE__, __LINE__ );


	  	ANSITime Compare(13500000,TimeSystem(2)); //Initialize an object
		//---------------------------------------------------------------------
		//Is the time after the BEGINNING_OF_TIME?
		//---------------------------------------------------------------------
  		testFramework.assert(Compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME, "Time provided is found to be earlier than the BEGINNING_OF_TIME object", __LINE__);

		//---------------------------------------------------------------------
		//Is the set object valid?
		//---------------------------------------------------------------------
		testFramework.assert(Compare.isValid(), "Time provided is found to not be valid for CommonTime conversions", __LINE__);


  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

  		ANSITime Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

		testFramework.changeSourceMethod("CommonTimeConversion");
		//---------------------------------------------------------------------
		//Is the result of conversion the same?
		//---------------------------------------------------------------------
		testFramework.assert(Compare.getTimeSystem()==Test2.getTimeSystem(), "Conversion to/from CommonTime changed the time system", __LINE__);
		testFramework.assert(Test2.time == Compare.time,                      "Conversion to/from CommonTime changed the time",        __LINE__);

		return testFramework.countFails();
	}


//==========================================================================================================================
//	Test will check the TimeSystem comparisons when using the comparison operators.
//==========================================================================================================================
	int  timeSystemTest (void)
	{
		TestUtil testFramework( "ANSITime", "OperatorEquivalentWithDifferingTimeSystem", __FILE__, __LINE__ );


  		ANSITime GPS1(13500000,TimeSystem(2));
  		ANSITime GPS2(13400000,TimeSystem(2));
  		ANSITime UTC1(13500000,TimeSystem(7));
  		ANSITime UNKNOWN(13500000,TimeSystem(0));
  		ANSITime ANY(13500000,TimeSystem(1));
		ANSITime ANY2(13400000, TimeSystem(1));

		//---------------------------------------------------------------------
		//Verify differing TimeSystem sets equivalence operator to false
		//Note that the operator test checks for == in ALL members
		//---------------------------------------------------------------------
		testFramework.assert(!(GPS1 == UTC1), "Equivalence operator found objects with differing TimeSystems to be the same", __LINE__);
		testFramework.assert(GPS1 == ANY,     "Differing TimeSystems where one is TimeSystem::Any is not ignored for equals", __LINE__);
		testFramework.assert(UTC1 == ANY,     "Differing TimeSystems where one is TimeSystem::Any is not ignored for equals", __LINE__);
		testFramework.assert(UNKNOWN == ANY,  "Differing TimeSystems where one is TimeSystem::Any is not ignored for equals", __LINE__);

		testFramework.changeSourceMethod("OperatorNotEquivalentWithDifferingTimeSystem");
		//---------------------------------------------------------------------
		//Verify different Time System but same time inequality
		//---------------------------------------------------------------------
		testFramework.assert(GPS1 != UTC1,    "Equivalent objects with differing TimeSystems are found to be equal",                                  __LINE__);
		testFramework.assert(GPS1 != UNKNOWN, "Equivalent objects with differing TimeSystems are found to be equal",                                  __LINE__);
		testFramework.assert(!(GPS1 != ANY),  "Equivalent objects with differing TimeSystems where one is TimeSystem::Any are found to be not-equal", __LINE__);

		testFramework.changeSourceMethod("OperatorLessThanWithDifferingTimeSystem");	
		//---------------------------------------------------------------------
		//Verify TimeSystem=ANY does not matter in other operator comparisons 
		//---------------------------------------------------------------------
		testFramework.assert(ANY2 < GPS1, "Less than object with Any TimeSystem is not found to be less than", __LINE__);
		testFramework.assert(GPS2 < ANY,"Less than object with GPS TimeSystem is not found to be less-than a greater object with Any TimeSystem", __LINE__);

		testFramework.changeSourceMethod("setTimeSystem");	
  		UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem
		//---------------------------------------------------------------------
		//Ensure resetting a Time System changes it
		//---------------------------------------------------------------------
		testFramework.assert(UNKNOWN.getTimeSystem()==TimeSystem(2), "setTimeSystem was unable to set the TimeSystem", __LINE__);

		return testFramework.countFails();
	}


//==========================================================================================================================
//	Test for the formatted printing of ANSITime objects
//==========================================================================================================================
	int  printfTest (void)
	{
		TestUtil testFramework( "ANSITime", "printf", __FILE__, __LINE__ );


  		ANSITime GPS1(13500000,TimeSystem(2));
  		ANSITime UTC1(13500000,TimeSystem(7));

		//---------------------------------------------------------------------
		//Verify printed output matches expectation
		//---------------------------------------------------------------------
		testFramework.assert(GPS1.printf("%08K %02P") == (std::string)"13500000 GPS", "printf did not output in the proper format", __LINE__);
		testFramework.assert(UTC1.printf("%08K %02P") == (std::string)"13500000 UTC", "printf did not output in the proper format", __LINE__);


		testFramework.changeSourceMethod("printError");	
		//---------------------------------------------------------------------
		//Verify printed error message matches expectation
		//---------------------------------------------------------------------
		testFramework.assert(GPS1.printError("%08K %02P") == (std::string)"ErrorBadTime ErrorBadTime", "printError did not output in the proper format", __LINE__);
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
