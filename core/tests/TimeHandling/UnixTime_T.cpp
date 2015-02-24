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
//==========================================================================================================================
//	initializationTest ensures the constructors set the values properly
//==========================================================================================================================
	int initializationTest (void)
	{
		TestUtil testFramework( "UnixTime", "Constructor", __FILE__, __LINE__ );


		UnixTime Compare(1350000,1,TimeSystem(2)); //Initialize an object

		//---------------------------------------------------------------------
		//Were the attributes set to expectation with the explicit constructor?
		//---------------------------------------------------------------------
		testFramework.assert(1350000 == Compare.tv.tv_sec,             "Explicit constructor did not set the tv_sec value properly",   __LINE__);
		testFramework.assert(1 == Compare.tv.tv_usec,                  "Explicit constructor did not set the tv_usec value properly",  __LINE__);
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem(), "Explicit constructor did not set the TimeSystem properly",     __LINE__);


		testFramework.changeSourceMethod("ConstructorCopy");
		UnixTime Copy(Compare); // Initialize with copy constructor
		//---------------------------------------------------------------------
		//Were the attributes set to expectation with the copy constructor?
		//---------------------------------------------------------------------
		testFramework.assert(1350000 == Copy.tv.tv_sec,             "Copy constructor did not set the tv_sec value properly",   __LINE__);
		testFramework.assert(1 == Copy.tv.tv_usec,                  "Copy constructor did not set the tv_usec value properly",  __LINE__);
		testFramework.assert(TimeSystem(2) == Copy.getTimeSystem(), "Copy constructor did not set the TimeSystem properly",     __LINE__);

		testFramework.changeSourceMethod("OperatorSet");
		UnixTime Assigned;
		Assigned = Compare;
		//---------------------------------------------------------------------
		//Were the attributes set to expectation with the Set Operator?
		//---------------------------------------------------------------------
		testFramework.assert(1350000 == Assigned.tv.tv_sec,             "Set Operator did not set the tv_sec value properly",   __LINE__);
		testFramework.assert(1 == Assigned.tv.tv_usec,                  "Set Operator did not set the tv_usec value properly",  __LINE__);
		testFramework.assert(TimeSystem(2) == Assigned.getTimeSystem(), "Set Operator did not set the TimeSystem properly",     __LINE__);

		return testFramework.countFails();
	}



//==========================================================================================================================
//	Test will check if UnixTime variable can be set from a map.
//	Test also implicity tests whether the != operator functions. 
//==========================================================================================================================
	int setFromInfoTest (void)
	{
		TestUtil testFramework( "UnixTime", "setFromInfo", __FILE__, __LINE__ );


		UnixTime setFromInfo1;
		UnixTime setFromInfo2;
		UnixTime Compare(1350000,1,TimeSystem(2)),Compare2(0,1,TimeSystem(2));
		
		TimeTag::IdToValue Id;
		Id['U'] = "1350000";
		Id['u'] = "1";
		Id['P'] = "GPS";


		//---------------------------------------------------------------------
		//Does a proper setFromInfo work with all information provided?
		//---------------------------------------------------------------------
		testFramework.assert(setFromInfo1.setFromInfo(Id), "setFromInfo experienced an error and returned false", __LINE__);
		testFramework.assert(Compare == setFromInfo1,      "setFromInfo did not set all of the values properly",  __LINE__); 


		Id.erase('U');
		//---------------------------------------------------------------------
		//Does a proper setFromInfo work with missing information?
		//---------------------------------------------------------------------
		testFramework.assert(setFromInfo2.setFromInfo(Id), "setFromInfo experienced an error and returned false", __LINE__);
		testFramework.assert(Compare2 == setFromInfo2,     "setFromInfo did not set all of the values properly",  __LINE__); 	

		return testFramework.countFails();

	}


//==========================================================================================================================
//	Test will check if the ways to initialize and set an UnixTime object.
//	Test also tests whether the comparison operators and isValid method function.
//==========================================================================================================================
	int operatorTest (void)
	{
		TestUtil testFramework( "UnixTime", "OperatorEquivalent", __FILE__, __LINE__ );


		UnixTime Compare(1350000,100); // Initialize with value
		UnixTime LessThanSec(1340000, 100); //Initialize with fewer seconds
		UnixTime LessThanMicroSec(1350000,0); //Initialize with fewer microseconds
		UnixTime CompareCopy(Compare); // Initialize with copy constructor

		//---------------------------------------------------------------------
		//Does the == Operator function?
		//---------------------------------------------------------------------
		testFramework.assert(  Compare == CompareCopy ,      "Equivalence operator found equivalent objects to not be equivalent",        __LINE__);
		testFramework.assert(!(Compare == LessThanSec),      "Equivalence operator found different second objects to be equivalent",      __LINE__);
		testFramework.assert(!(Compare == LessThanMicroSec), "Equivalence operator found different microsecond objects to be equivalent", __LINE__);

		//---------------------------------------------------------------------
		//Does the != Operator function?
		//---------------------------------------------------------------------
		testFramework.assert(!(Compare != CompareCopy),      "Not-equal operator found equivalent objects to be not equivalent",        __LINE__);
		testFramework.assert(  Compare != LessThanSec ,      "Not-equal operator found different second objects to be equivalent",      __LINE__);
		testFramework.assert(  Compare != LessThanMicroSec , "Not-equal operator found different microsecond objects to be equivalent", __LINE__);


		testFramework.changeSourceMethod("OperatorLessThan");
		//---------------------------------------------------------------------
		//Does the < Operator function?
		//---------------------------------------------------------------------
		testFramework.assert(  LessThanSec < Compare ,        "Less-than operator found less-than second object to not be less-than",      __LINE__);
		testFramework.assert(  LessThanMicroSec < Compare ,   "Less-than operator found less-than microsecond object to not be less-than", __LINE__);
		testFramework.assert(!(Compare < LessThanSec),        "Less-than operator found greater-than second object to be less-than",       __LINE__);
		testFramework.assert(!(Compare < LessThanMicroSec),   "Less-than operator found greater-than microsecond object to be less-than",  __LINE__);
		testFramework.assert(!(Compare < CompareCopy),        "Less-than operator found equivalent object to be less-than",                __LINE__);


		testFramework.changeSourceMethod("OperatorGreaterThan");
		//---------------------------------------------------------------------
		//Does the > Operator function?
		//---------------------------------------------------------------------
		testFramework.assert(!(LessThanSec > Compare),        "Greater-than operator found less-than second object to not be greater-than",      __LINE__);
		testFramework.assert(!(LessThanMicroSec > Compare),   "Greater-than operator found less-than microsecond object to not be greater-than", __LINE__);
		testFramework.assert(  Compare > LessThanSec ,        "Greater-than operator found greater-than second object to be greater-than",       __LINE__);
		testFramework.assert(  Compare > LessThanMicroSec ,   "Greater-than operator found greater-than microsecond object to be greater-than",  __LINE__);
		testFramework.assert(!(Compare > CompareCopy),        "Greater-than operator found equivalent object to be greater-than",                __LINE__);
	

		testFramework.changeSourceMethod("OperatorLessThanOrEqualTo");
		//---------------------------------------------------------------------
		//Does the <= Operator function?
		//---------------------------------------------------------------------
		testFramework.assert(  LessThanSec <= Compare ,        "Less-than-or-equal-to operator found less-than second object to not be less-than-or-equal-to",      __LINE__);
		testFramework.assert(  LessThanMicroSec <= Compare ,   "Less-than-or-equal-to operator found less-than microsecond object to not be less-than-or-equal-to", __LINE__);
		testFramework.assert(!(Compare <= LessThanSec),        "Less-than-or-equal-to operator found greater-than second object to be less-than-or-equal-to",       __LINE__);
		testFramework.assert(!(Compare <= LessThanMicroSec),   "Less-than-or-equal-to operator found greater-than microsecond object to be less-than-or-equal-to",  __LINE__);
		testFramework.assert(  Compare <= CompareCopy ,        "Less-than-or-equal-to operator found equivalent object to not be less-than-or-equal-to",            __LINE__);


		testFramework.changeSourceMethod("OperatorGreaterThanOrEqualTo");
		//---------------------------------------------------------------------
		//Does the >= Operator function?
		//---------------------------------------------------------------------
		testFramework.assert(!(LessThanSec >= Compare),        "Greater-than-or-equal-to operator found less-than second object to not be greater-than-or-equal-to",      __LINE__);
		testFramework.assert(!(LessThanMicroSec >= Compare),   "Greater-than-or-equal-to operator found less-than microsecond object to not be greater-than-or-equal-to", __LINE__);
		testFramework.assert(  Compare >= LessThanSec ,        "Greater-than-or-equal-to operator found greater-than second object to be greater-than-or-equal-to",       __LINE__);
		testFramework.assert(  Compare >= LessThanMicroSec ,   "Greater-than-or-equal-to operator found greater-than microsecond object to be greater-than-or-equal-to",  __LINE__);
		testFramework.assert(  Compare >= CompareCopy ,        "Greater-than-or-equal-to operator found equivalent object to not be greater-than-or-equal-to",            __LINE__);

		return testFramework.countFails();
	}


//==========================================================================================================================
//	Test will check the reset method.
//==========================================================================================================================
	int  resetTest (void)
	{
		TestUtil testFramework( "UnixTime", "reset" , __FILE__, __LINE__ );


	  	UnixTime Compare(1350000,0,TimeSystem(2)); //Initialize an object

	  	Compare.reset(); // Reset it

		//---------------------------------------------------------------------
		//Were the attributes reset to expectation?
		//---------------------------------------------------------------------
	  	testFramework.assert(TimeSystem(0) == Compare.getTimeSystem(),  "reset() did not set the TimeSystem to UNK",       __LINE__); 	
		testFramework.assert(0 == (int)Compare.tv.tv_sec,               "reset() did not set the second value to 0",       __LINE__);
		testFramework.assert(0 == (int)Compare.tv.tv_usec,              "reset() did not set the microsecond value to 0",  __LINE__); 

		return testFramework.countFails();
	}


//==========================================================================================================================
//	Test will check converting to/from CommonTime.
//==========================================================================================================================
	int  toFromCommonTimeTest (void)
	{
		TestUtil testFramework( "UnixTime", "isValid", __FILE__, __LINE__ );


	  	UnixTime Compare(1350000,0,TimeSystem(2)); //Initialize an object
  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

		//---------------------------------------------------------------------
		//Is the time after the BEGINNING_OF_TIME?
		//---------------------------------------------------------------------
  		testFramework.assert(Compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME, "Time provided found to be less than the beginning of time", __LINE__);


		//---------------------------------------------------------------------
		//Is the set object valid?
		//---------------------------------------------------------------------
		testFramework.assert(Compare.isValid(), "Time provided found to be unable to convert to/from CommonTime", __LINE__);


  		UnixTime Test2;
  		Test2.convertFromCommonTime(Test); //Convert From
		testFramework.changeSourceMethod("CommonTimeConversion");
		//---------------------------------------------------------------------
		//Is the result of conversion the same?
		//---------------------------------------------------------------------
		testFramework.assert(Compare.getTimeSystem()== Test2.getTimeSystem(),  "TimeSystem provided found to be different after converting to and from CommonTime",  __LINE__);
		testFramework.assert(Test2.tv.tv_sec == Compare.tv.tv_sec,             "Second provided found to be different after converting to and from CommonTime",      __LINE__);
		testFramework.assert(Test2.tv.tv_usec == Compare.tv.tv_usec,           "Microsecond provided found to be different after converting to and from CommonTime", __LINE__);

		return testFramework.countFails();
	}


//==========================================================================================================================
//	Test will check the TimeSystem comparisons when using the comparison operators.
//==========================================================================================================================
	int  timeSystemTest (void)
	{
		TestUtil testFramework( "UnixTime", "OperatorEquivalentWithDifferingTimeSystem", __FILE__, __LINE__ );


		UnixTime GPS1(1350000,0,TimeSystem::GPS);
		UnixTime GPS2(1340000,0,TimeSystem::GPS);
		UnixTime UTC1(1350000,0,TimeSystem::UTC);
		UnixTime UNKNOWN(1350000,0,TimeSystem::Unknown);
		UnixTime ANY(1350000,0,TimeSystem::Any);
		UnixTime ANY2(1340000,0,TimeSystem::Any);

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
//	Test for the formatted printing of UnixTime objects 
//==========================================================================================================================
	int  printfTest (void)
	{
		TestUtil testFramework( "UnixTime", "printf", __FILE__, __LINE__ );


  		UnixTime GPS1(1350000,0,TimeSystem(2));
  		UnixTime UTC1(1350000,0,TimeSystem(7));
		
		//---------------------------------------------------------------------
		//Verify printed output matches expectation
		//---------------------------------------------------------------------	
  		testFramework.assert(GPS1.printf("%07U %02u %02P") == (std::string)"1350000 00 GPS", "printf did not output in the proper format", __LINE__);
  		testFramework.assert(UTC1.printf("%07U %02u %02P") == (std::string)"1350000 00 UTC", "printf did not output in the proper format", __LINE__);
  

		//---------------------------------------------------------------------
		//Verify printed error message matches expectation
		//---------------------------------------------------------------------
  		testFramework.assert(GPS1.printError("%07U %02u %02P") == (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime", "printError did not output in the proper format", __LINE__);
  		testFramework.assert(UTC1.printError("%07U %02u %02P") == (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime", "printError did not output in the proper format", __LINE__);
		
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
