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

#include "CivilTime.hpp"
#include "TimeSystem.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>
#include <cmath>

using namespace gpstk;
using namespace std;

class CivilTime_T
{
	public:
	CivilTime_T(){eps = 1e-12;}// Default Constructor, set the precision value
	~CivilTime_T() {} // Default Desructor

	public:

//==========================================================================================================================
//	initializationTest ensures the constructors set the values properly
//==========================================================================================================================
	int  initializationTest (void)
	{
		TestUtil testFramework( "CivilTime", "Constructor", __FILE__, __LINE__ );
	  	CivilTime Compare(2008,8,21,13,30,15.,TimeSystem::GPS); //Initialize an object

		//---------------------------------------------------------------------
		//Were the attributes set to expectation with the explicit constructor?
		//---------------------------------------------------------------------
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem(), "Explicit constructor did not set the TimeSystem properly", __LINE__);
		testFramework.assert(2008 == (int)Compare.year,                "Explicit constructor did not set the year properly",       __LINE__);
		testFramework.assert( 8 == (int)Compare.month,                 "Explicit constructor did not set the month properly",      __LINE__);
		testFramework.assert(21 == (int)Compare.day,                   "Explicit constructor did not set the day properly",        __LINE__);
		testFramework.assert(13 == (int)Compare.hour,                  "Explicit constructor did not set the hour properly",       __LINE__);
		testFramework.assert(30 == (int)Compare.minute,                "Explicit constructor did not set the minute properly",     __LINE__); 
		testFramework.assert(15 == (double)Compare.second,             "Explicit constructor did not set the second properly",     __LINE__);
 

		testFramework.changeSourceMethod("ConstructorCopy");
		CivilTime Copy(Compare); // Initialize with copy constructor
		//---------------------------------------------------------------------
		//Were the attributes set to expectation with the copy constructor?
		//---------------------------------------------------------------------
		testFramework.assert(TimeSystem(2) == Copy.getTimeSystem(), "Copy constructor did not set the TimeSystem properly", __LINE__);
		testFramework.assert(2008 == (int)Copy.year,                "Copy constructor did not set the year properly",       __LINE__);
		testFramework.assert(8 == (int)Copy.month,                  "Copy constructor did not set the month properly",      __LINE__);
		testFramework.assert(21 == (int)Copy.day,                   "Copy constructor did not set the day properly",        __LINE__);
		testFramework.assert(13 == (int)Copy.hour,                  "Copy constructor did not set the hour properly",       __LINE__);
		testFramework.assert(30 == (int)Copy.minute,                "Copy constructor did not set the minute properly",     __LINE__); 
		testFramework.assert(15 == (double)Copy.second,             "Copy constructor did not set the second properly",     __LINE__); 


		CivilTime Assigned;
		Assigned = Compare;
		testFramework.changeSourceMethod("OperatorSet");
		//---------------------------------------------------------------------
		//Were the attributes set to expectation with the Set operator?
		//---------------------------------------------------------------------
		testFramework.assert(TimeSystem(2) == Assigned.getTimeSystem(), "Set Operator did not set the TimeSystem properly", __LINE__);
		testFramework.assert(2008 == (int)Assigned.year,                "Set Operator did not set the year properly",       __LINE__);
		testFramework.assert(8 == (int)Assigned.month,                  "Set Operator did not set the month properly",      __LINE__);
		testFramework.assert(21 == (int)Assigned.day,                   "Set Operator did not set the day properly",        __LINE__);
		testFramework.assert(13 == (int)Assigned.hour,                  "Set Operator did not set the hour properly",       __LINE__);
		testFramework.assert(30 == (int)Assigned.minute,                "Set Operator did not set the mimute properly",     __LINE__); 
		testFramework.assert(15 == (double)Assigned.second,             "Set Operator did not set the second properly",     __LINE__); 

		return testFramework.countFails();
	}

//==========================================================================================================================
//	Test will check if CivilTime variable can be set from a map.
//==========================================================================================================================
	int setFromInfoTest (void)
	{
		TestUtil testFramework( "CivilTime", "setFromInfo", __FILE__, __LINE__ );

		//Create several objects for testing
		CivilTime setFromInfo1;
		CivilTime setFromInfo2;
		CivilTime setFromInfo3;
		CivilTime setFromInfo4;
		CivilTime setFromInfo5;

		//Set several values to set the objects
		TimeTag::IdToValue Id;
		Id['b'] = "Dec";
		Id['d'] = "31";
		Id['Y'] = "2008";
		Id['H'] = "12";
		Id['M'] = "00";
		Id['S'] = "00";
		Id['P'] = "GPS";
		CivilTime Check(2008,12,31,12,0,0,TimeSystem::GPS);
		//---------------------------------------------------------------------
		//Can a CivilTime object be set with b,d,Y,H,M,S,P options?
		//---------------------------------------------------------------------
		testFramework.assert(setFromInfo1.setFromInfo(Id), "setFromInfo experienced an error and returned false", __LINE__);
		testFramework.assert(setFromInfo1 == Check, "setFromInfo did not set all of the values properly", __LINE__); 


		Id.erase('b');
		Id.erase('Y');
		Id['m'] = "12";
		Id['y'] = "06";
    		CivilTime Check2(2006,12,31,12,0,0,TimeSystem::GPS);
		//---------------------------------------------------------------------
		//Can a CivilTime object be set with d,m,y,H,M,S,P options?
		//---------------------------------------------------------------------
		testFramework.assert(setFromInfo2.setFromInfo(Id), "setFromInfo experienced an error and returned false", __LINE__);
    		testFramework.assert(setFromInfo2 == Check2, "setFromInfo did not set all of the values properly", __LINE__); 

		
		Id.erase('y');
		Id['y'] = "006";
		//---------------------------------------------------------------------
		//Can a CivilTime object be set with a 3 digit year? Answer should be no. 'y' option is for 2 digit years.
		//---------------------------------------------------------------------
		testFramework.assert(!setFromInfo3.setFromInfo(Id), "setFromInfo allowed a 3 digit year to be set with 'y' option", __LINE__);


		Id.erase('y');
		//---------------------------------------------------------------------
		//Can a CivilTime object be set without a year?
		//---------------------------------------------------------------------
		testFramework.assert(setFromInfo4.setFromInfo(Id), "setFromInfo experienced an error and returned false", __LINE__);


		Id.erase('m');
		Id['b'] = "AAA";
		//---------------------------------------------------------------------
		//Can a CivilTime object be set with an improper month?
		//---------------------------------------------------------------------
		testFramework.assert(!(setFromInfo5.setFromInfo(Id)), "setFromInfo allowed the month to be set with an improper value", __LINE__);


		return testFramework.countFails();
	}


//==========================================================================================================================
//	Test will check if the ways to initialize and set an CivilTime object.
//	Test also tests whether the comparison operators and isValid method function.
//==========================================================================================================================
	int operatorTest (void)
	{
		TestUtil testFramework( "CivilTime", "OperatorEquivalent", __FILE__, __LINE__ );

		CivilTime Aug21(2008,8,21,13,30,15.); //Reference time
		// Series of conditions less than Aug21 above
		CivilTime LessThanYear(2005,8,21,13,30,15.);
		CivilTime LessThanMonth(2008,7,21,13,30,15.);
		CivilTime LessThanDay(2008,8,20,13,30,15.);
		CivilTime LessThanHour(2008,8,21,12,30,15.);
		CivilTime LessThanMinute(2008,8,21,13,20,15.);
		CivilTime LessThanSecond(2008,8,21,13,30,0.);
		CivilTime Aug21Copy(Aug21); //Use copy constructor
		CivilTime Aug21Copy2 = Aug21Copy; //Use the = set operator.

		//---------------------------------------------------------------------
		//Does the == Operator function?
		//---------------------------------------------------------------------
		testFramework.assert(  Aug21 == Aug21Copy,       "Equivalence operator found equivalent objects to be not equivalent",   __LINE__);
		testFramework.assert(!(Aug21 == LessThanYear),   "Equivalence operator found different year objects to be equivalent",   __LINE__);
		testFramework.assert(!(Aug21 == LessThanMonth),  "Equivalence operator found different month objects to be equivalent",  __LINE__);
		testFramework.assert(!(Aug21 == LessThanDay),    "Equivalence operator found different day objects to be equivalent",    __LINE__);
		testFramework.assert(!(Aug21 == LessThanHour),   "Equivalence operator found different hour objects to be equivalent",   __LINE__);
		testFramework.assert(!(Aug21 == LessThanMinute), "Equivalence operator found different minute objects to be equivalent", __LINE__);
		testFramework.assert(!(Aug21 == LessThanSecond), "Equivalence operator found different second objects to be equivalent", __LINE__);


		testFramework.changeSourceMethod("OperatorNotEquivalent");
		//---------------------------------------------------------------------
		//Does the != Operator function?
		//---------------------------------------------------------------------
		testFramework.assert(  Aug21 != LessThanYear,   "Not-equal operator found different year objects to be equivalent",   __LINE__);
		testFramework.assert(  Aug21 != LessThanMonth,  "Not-equal operator found different month objects to be equivalent",  __LINE__);
		testFramework.assert(  Aug21 != LessThanDay,    "Not-equal operator found different day objects to be equivalent",    __LINE__);
		testFramework.assert(  Aug21 != LessThanHour,   "Not-equal operator found different hour objects to be equivalent",   __LINE__);
		testFramework.assert(  Aug21 != LessThanMinute, "Not-equal operator found different minute objects to be equivalent", __LINE__);
		testFramework.assert(  Aug21 != LessThanSecond, "Not-equal operator found different second objects to be equivalent", __LINE__);
		testFramework.assert(!(Aug21 != Aug21Copy),     "Not-equal operator found equivalent objects to not be equivalent",   __LINE__);
   
		testFramework.changeSourceMethod("OperatorLessThan");
		//---------------------------------------------------------------------
		//Does the < operator function?
		//---------------------------------------------------------------------
		testFramework.assert(  LessThanYear < Aug21,    "Less-than operator found less-than year object to not be less than",   __LINE__);
		testFramework.assert(  LessThanMonth < Aug21,   "Less-than operator found less-than month object to not be less than",  __LINE__);
		testFramework.assert(  LessThanDay < Aug21,     "Less-than operator found less-than day object to not be less than",    __LINE__);
		testFramework.assert(  LessThanHour < Aug21,    "Less-than operator found less-than hour object to not be less than",   __LINE__);
		testFramework.assert(  LessThanMinute < Aug21,  "Less-than operator found less-than minute object to not be less than", __LINE__);
		testFramework.assert(  LessThanSecond < Aug21,  "Less-than operator found less-than second object to not be less than", __LINE__);
		testFramework.assert(!(Aug21 < LessThanYear),   "Less-than operator found greater-than year object to be less than",    __LINE__);
		testFramework.assert(!(Aug21 < LessThanMonth),  "Less-than operator found greater-than month object to be less than",   __LINE__);
		testFramework.assert(!(Aug21 < LessThanDay),    "Less-than operator found greater-than day object to be less than",     __LINE__);
		testFramework.assert(!(Aug21 < LessThanHour),   "Less-than operator found greater-than hour object to be less than",    __LINE__);
		testFramework.assert(!(Aug21 < LessThanMinute), "Less-than operator found greater-than minute object to be less than",  __LINE__);
		testFramework.assert(!(Aug21 < LessThanSecond), "Less-than operator found greater-than second object to be less than",  __LINE__);
		testFramework.assert(!(Aug21 < Aug21Copy),      "Less-than operator found equivalent objects to be less than",          __LINE__);


		testFramework.changeSourceMethod("OperatorGreaterThan");
		//---------------------------------------------------------------------
		//Does the > operator function?
		//---------------------------------------------------------------------
		testFramework.assert(!(LessThanYear > Aug21),   "Greater-than operator found less-than year object to be greater-than",          __LINE__);
		testFramework.assert(!(LessThanMonth > Aug21),  "Greater-than operator found less-than month object to be greater-than",         __LINE__);
		testFramework.assert(!(LessThanDay > Aug21),    "Greater-than operator found less-than day object to be greater-than",           __LINE__);
		testFramework.assert(!(LessThanHour > Aug21),   "Greater-than operator found less-than hour object to be greater-than",          __LINE__);
		testFramework.assert(!(LessThanMinute > Aug21), "Greater-than operator found less-than minute object to be greater-than",        __LINE__);
		testFramework.assert(!(LessThanSecond > Aug21), "Greater-than operator found less-than second object to be greater-than",        __LINE__);
		testFramework.assert(  Aug21 > LessThanYear,    "Greater-than operator found greater-than year object to not be greater-than",   __LINE__);
		testFramework.assert(  Aug21 > LessThanMonth,   "Greater-than operator found greater-than month object to not be greater-than",  __LINE__);
		testFramework.assert(  Aug21 > LessThanDay,     "Greater-than operator found greater-than day object to not be greater-than",    __LINE__);
		testFramework.assert(  Aug21 > LessThanHour,    "Greater-than operator found greater-than hour object to not be greater-than",   __LINE__);
		testFramework.assert(  Aug21 > LessThanMinute,  "Greater-than operator found greater-than minute object to not be greater-than", __LINE__);
		testFramework.assert(  Aug21 > LessThanSecond,  "Greater-than operator found greater-than second object to not be greater-than", __LINE__);
		testFramework.assert(!(Aug21 > Aug21Copy),      "Greater-than operator found equivalent objects to be greater-than",             __LINE__);


		testFramework.changeSourceMethod("OperatorLessThanOrEqualTo");
		//---------------------------------------------------------------------
		//Does the <= operator function?
		//---------------------------------------------------------------------
		testFramework.assert(  LessThanYear <= Aug21,    "Less-than-or-equal-to operator found less-than year object to not be less-than-or-equal-to",   __LINE__);
		testFramework.assert(  LessThanMonth <= Aug21,   "Less-than-or-equal-to operator found less-than month object to not be less-than-or-equal-to",  __LINE__);
		testFramework.assert(  LessThanDay <= Aug21,     "Less-than-or-equal-to operator found less-than day object to not be less-than-or-equal-to",    __LINE__);
		testFramework.assert(  LessThanHour <= Aug21,    "Less-than-or-equal-to operator found less-than hour object to not be less-than-or-equal-to",   __LINE__);
		testFramework.assert(  LessThanMinute <= Aug21,  "Less-than-or-equal-to operator found less-than minute object to not be less-than-or-equal-to", __LINE__);
		testFramework.assert(  LessThanSecond <= Aug21,  "Less-than-or-equal-to operator found less-than second object to not be less-than-or-equal-to", __LINE__);
		testFramework.assert(!(Aug21 <= LessThanYear),   "Less-than-or-equal-to operator found greater-than year object to be less-than-or-equal-to",    __LINE__);
		testFramework.assert(!(Aug21 <= LessThanMonth),  "Less-than-or-equal-to operator found greater-than month object to be less-than-or-equal-to",   __LINE__);
		testFramework.assert(!(Aug21 <= LessThanDay),    "Less-than-or-equal-to operator found greater-than day object to be less-than-or-equal-to",     __LINE__);
		testFramework.assert(!(Aug21 <= LessThanHour),   "Less-than-or-equal-to operator found greater-than hour object to be less-than-or-equal-to",    __LINE__);
		testFramework.assert(!(Aug21 <= LessThanMinute), "Less-than-or-equal-to operator found greater-than minute object to be less-than-or-equal-to",  __LINE__);
		testFramework.assert(!(Aug21 <= LessThanSecond), "Less-than-or-equal-to operator found greater-than second object to be less-than-or-equal-to",  __LINE__);
		testFramework.assert(  Aug21 <= Aug21Copy,       "Less-than-or-equal-to operator found equivalent objects to not be less-than-or-equal-to",      __LINE__);


		testFramework.changeSourceMethod("OperatorGreaterThanOrEqualTo");
		//---------------------------------------------------------------------
		//Does the >= operator function?
		//---------------------------------------------------------------------
		testFramework.assert(!(LessThanYear >= Aug21),   "Greater-than-or-equal-to operator found less-than year object to be greater-than-or-equal-to",          __LINE__);
		testFramework.assert(!(LessThanMonth >= Aug21),  "Greater-than-or-equal-to operator found less-than month object to be greater-than-or-equal-to",         __LINE__);
		testFramework.assert(!(LessThanDay >= Aug21),    "Greater-than-or-equal-to operator found less-than day object to be greater-than-or-equal-to",           __LINE__);
		testFramework.assert(!(LessThanHour >= Aug21),   "Greater-than-or-equal-to operator found less-than hour object to be greater-than-or-equal-to",          __LINE__);
		testFramework.assert(!(LessThanMinute >= Aug21), "Greater-than-or-equal-to operator found less-than minute object to be greater-than-or-equal-to",        __LINE__);
		testFramework.assert(!(LessThanSecond >= Aug21), "Greater-than-or-equal-to operator found less-than second object to be greater-than-or-equal-to",        __LINE__);
		testFramework.assert(  Aug21 >= LessThanYear,    "Greater-than-or-equal-to operator found greater-than year object to not be greater-than-or-equal-to",   __LINE__);
		testFramework.assert(  Aug21 >= LessThanMonth,   "Greater-than-or-equal-to operator found greater-than month object to not be greater-than-or-equal-to",  __LINE__);
		testFramework.assert(  Aug21 >= LessThanDay,     "Greater-than-or-equal-to operator found greater-than day object to not be greater-than-or-equal-to",    __LINE__);
		testFramework.assert(  Aug21 >= LessThanHour,    "Greater-than-or-equal-to operator found greater-than hour object to not be greater-than-or-equal-to",   __LINE__);
		testFramework.assert(  Aug21 >= LessThanMinute,  "Greater-than-or-equal-to operator found greater-than minute object to not be greater-than-or-equal-to", __LINE__);
		testFramework.assert(  Aug21 >= LessThanSecond,  "Greater-than-or-equal-to operator found greater-than second object to not be greater-than-or-equal-to", __LINE__);
		testFramework.assert(  Aug21 >= Aug21Copy,       "Greater-than-or-equal-to operator found equivalent objects to not be greater-than-or-equal-to",         __LINE__);


		return testFramework.countFails();
	}


//==========================================================================================================================
//	Test will check converting to/from CommonTime.
//==========================================================================================================================
	int  toFromCommonTimeTest (void)
	{
  		TestUtil testFramework( "CivilTime", "isValid", __FILE__, __LINE__ );

  		CivilTime Aug21(2008,8,21,13,30,15.,TimeSystem::GPS);
		//---------------------------------------------------------------------
		//Is the time after the BEGINNING_OF_TIME?
		//---------------------------------------------------------------------
  		testFramework.assert(Aug21.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME, "Time provided found to be less than the beginning of time", __LINE__);

		//---------------------------------------------------------------------
		//Is the set object valid?
		//---------------------------------------------------------------------
		testFramework.assert(Aug21.isValid(), "Time provided found to be unable to convert to/from CommonTime", __LINE__);

  		
  		CommonTime Test = Aug21.convertToCommonTime();

  		CivilTime Test2;
  		Test2.convertFromCommonTime(Test);

		testFramework.changeSourceMethod("CommonTimeConversion");
		//---------------------------------------------------------------------
		//Is the result of conversion the same?
		//---------------------------------------------------------------------
		testFramework.assert(Test2.getTimeSystem()==Aug21.getTimeSystem(), "TimeSystem provided found to be different after converting to and from CommonTime", __LINE__);
  		testFramework.assert(Test2.year == Aug21.year,                     "Year provided found to be different after converting to and from CommonTime",       __LINE__);
  		testFramework.assert(Test2.month == Aug21.month,                   "Month provided found to be different after converting to and from CommonTime",      __LINE__);
  		testFramework.assert(Test2.day == Aug21.day,                       "Day provided found to be different after converting to and from CommonTime",        __LINE__);
  		testFramework.assert(Test2.hour == Aug21.hour,                     "Hour provided found to be different after converting to and from CommonTime",       __LINE__);
  		testFramework.assert(Test2.minute == Aug21.minute,                 "Minute provided found to be different after converting to and from CommonTime",     __LINE__);
  		testFramework.assert(Test2.second == Aug21.second,                 "Second provided found to be different after converting to and from CommonTime",     __LINE__);		

		return testFramework.countFails();
	}




//==========================================================================================================================
//	Test will check the reset method.
//==========================================================================================================================

	int resetTest (void)
	{
		TestUtil testFramework( "CivilTime", "reset", __FILE__, __LINE__ );
		//Initialize a time
  		CivilTime Aug21(2008,8,21,13,30,15.,TimeSystem::GPS);

  		Aug21.reset();
		//---------------------------------------------------------------------
		//Were the attributes reset to expectation?
		//---------------------------------------------------------------------
  		testFramework.assert(Aug21.getTimeSystem()==TimeSystem(0), "TimeSystem not set to default (Unknown) after reset", __LINE__);
  		testFramework.assert(0 == (int)Aug21.year,                 "Year not set to default (0) after reset",             __LINE__);
  		testFramework.assert(1 == (int)Aug21.month,                "Month not set to default (1) after reset",            __LINE__);
  		testFramework.assert(1 == (int)Aug21.day,                  "Day not set to default (1) after reset",              __LINE__);
  		testFramework.assert(0 == (int)Aug21.hour,                 "Hour not set to default (0) after reset",             __LINE__);
  		testFramework.assert(0 == (int)Aug21.minute,               "Minute not set to default (0) after reset",           __LINE__);
  		testFramework.assert(0 == (int)Aug21.second,               "Second not set to default (0) after reset",           __LINE__);

		return testFramework.countFails();
	}


//==========================================================================================================================
//	Test to check the TimeSystem comparisons when using the comparison operators.
//==========================================================================================================================
	int timeSystemTest (void)
	{
  		TestUtil testFramework( "CivilTime", "OperatorEquivalentWithDifferingTimeSystem", __FILE__, __LINE__ );

  		CivilTime GPS1(   2008,8,21,13,30,15.,TimeSystem::GPS);
  		CivilTime GPS2(   2005,8,21,13,30,15.,TimeSystem::GPS); // Time is less than the others
  		CivilTime UTC1(   2008,8,21,13,30,15.,TimeSystem::UTC);
  		CivilTime UNKNOWN(2008,8,21,13,30,15.,TimeSystem::Unknown);
  		CivilTime ANY(    2008,8,21,13,30,15.,TimeSystem::Any);
		CivilTime ANY2(   2005,8,21,13,30,15.,TimeSystem::Any);

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
//	Test for the formatted printing of CivilTime objects
//==========================================================================================================================
	int printfTest (void)
	{
		TestUtil testFramework( "CivilTime", "printf", __FILE__, __LINE__ );

  		CivilTime GPS1(2008,8,21,13,30,15.,TimeSystem::GPS);
  		CivilTime UTC1(2008,8,21,13,30,15.,TimeSystem::UTC);
		//---------------------------------------------------------------------
		//Verify printed output matches expectation
		//---------------------------------------------------------------------
		testFramework.assert(GPS1.printf("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P") ==
                       (std::string)"2008 08 08 Aug 21 13 30 15 15.000000 GPS", "printf did not output in the proper format", __LINE__);
		testFramework.assert(UTC1.printf("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P") ==
                       (std::string)"2008 08 08 Aug 21 13 30 15 15.000000 UTC", "printf did not output in the proper format", __LINE__);


		testFramework.changeSourceMethod("printError");	
		//---------------------------------------------------------------------
		//Verify printed error message matches expectation
		//---------------------------------------------------------------------
		testFramework.assert(GPS1.printError("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P") ==
                       (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime", "printError did not output in the proper format", __LINE__);
		testFramework.assert(UTC1.printError("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P") ==
                       (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime", "printError did not output in the proper format", __LINE__);

		
		return testFramework.countFails();
	}

	private:
		double eps;
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	CivilTime_T testClass;

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
	
	std::cout << "Total Errors: " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
