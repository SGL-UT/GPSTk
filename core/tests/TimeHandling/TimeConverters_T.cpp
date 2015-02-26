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

#include "TimeConverters.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
using namespace gpstk;


class xTimeConverters
{
	public:
		xTimeConverters(){eps = 1e-12;}// Default Constructor, set the precision value
		~xTimeConverters() {} // Default Desructor

//==========================================================================================================================
//	Julian Date (JD) to Calendar Date Tests
//==========================================================================================================================
		int JDtoCalendarTest()
		{
			TestUtil testFramework( "TimeConverters", "convertJDtoCalendar", __FILE__, __LINE__ );

			int year, month, day;
                        int inputJD[8]       = {2453971, 2299159, 2342032, 2377095, 1721118, 1721424, 1648549, 1719657};
			int expectedYear[8]  = {   2006,    1582,    1700,    1796,      -1,       1,    -200,      -5};
			int expectedMonth[8] = {      8,      10,       3,       2,       3,       1,       6,       3};
			int expectedDay[8]   = {     23,       3,       1,      29,       1,       1,      25,       1};

			
			for (int i = 0; i < 8; i++)
			{
				convertJDtoCalendar(inputJD[i],year,month,day);
				//---------------------------------------------------------------------
				//Was the correct calendar day found for the above JD?
				//---------------------------------------------------------------------		
				testFramework.assert(expectedYear[i]  == year , "The year from the JD conversion was not correct",  __LINE__);			
				testFramework.assert(expectedMonth[i] == month, "The month from the JD conversion was not correct", __LINE__);				
				testFramework.assert(expectedDay[i]   == day  , "The day from the JD conversion was not correct",   __LINE__);	
			}

			return testFramework.countFails();
		}


//==========================================================================================================================
//	Calendar to JD tests
//==========================================================================================================================
		int CalendartoJDTest()
		{
			TestUtil testFramework( "TimeConverters", "convertCalendarToJD", __FILE__, __LINE__ );


			long JD;
                        int expectedJD[8] = {2453971, 2299159, 2342032, 2377095, 1721118, 1721424, 1648549, 1719657};
			int inputYear[8]  = {   2006,    1582,    1700,    1796,      -1,       1,    -200,      -5};
			int inputMonth[8] = {      8,      10,       3,       2,       3,       1,       6,       3};
			int inputDay[8]   = {     23,       3,       1,      29,       1,       1,      25,       1};

			for (int i = 0 ; i < 8; i++)
			{
				JD = convertCalendarToJD(inputYear[i],inputMonth[i],inputDay[i]);
				//---------------------------------------------------------------------
				//Was the correct JD found for the above Calendar day?
				//---------------------------------------------------------------------	
				testFramework.assert(expectedJD[i] == JD, "The JD found from the calendar-JD conversion was not correct",  __LINE__);	
			}

			return testFramework.countFails();
		}


//==========================================================================================================================
//	Seconds of Day (SOD) to Time Tests
//==========================================================================================================================
		int SODtoTimeTest()
		{
			TestUtil testFramework( "TimeConverters", "convertSODToTime", __FILE__, __LINE__ );


			int hour;
			int minute;
			double second, relativeError;
			double inputSOD[3]       = { -0.1, 86401.11, 12345.67};
			int    expectedHour[3]   = {   23,        0,        3};
			int    expectedMinute[3] = {   59,        0,       25};
			double expectedSecond[3] = { 59.9,     1.11,    45.67};

			for (int i = 0 ; i < 3; i++)
			{
				convertSODtoTime(inputSOD[i],hour,minute,second);
				//---------------------------------------------------------------------
				//Was the correct Time found for the above SOD?
				//---------------------------------------------------------------------	
				relativeError = fabs(expectedSecond[i]-second)/fabs(expectedSecond[i]);
				testFramework.assert(expectedHour[i] == hour    , "The SOD to Time conversion found an incorrect hour"  , __LINE__ );
				testFramework.assert(expectedMinute[i] == minute, "The SOD to Time conversion found an incorrect minute", __LINE__ );
				testFramework.assert(relativeError < eps        , "The SOD to Time conversion found an incorrect second", __LINE__ );
			}

			return testFramework.countFails();
		}


//==========================================================================================================================
//	Time to SOD Tests
//==========================================================================================================================
		int TimetoSODTest()
		{


			TestUtil testFramework( "TimeConverters", "convertTimeToSOD", __FILE__, __LINE__ );
			int hour, minute;
			double second, SOD, relativeError;;
			double expectedSOD[3] = {4230.5, 86399.99, 12345.67};
			int    inputHour[3]   = {     1,       23,        3};
			int    inputMinute[3] = {    10,       59,       25};
			double inputSecond[3] = {  30.5,    59.99,    45.67};

			for (int i = 0 ; i < 3; i++)
			{
				SOD = convertTimeToSOD(inputHour[i],inputMinute[i],inputSecond[i]);
				//---------------------------------------------------------------------
				//Was the correct SOD found for the above Time?
				//---------------------------------------------------------------------	
				relativeError = fabs(expectedSOD[i]-SOD)/fabs(expectedSOD[i]);
				testFramework.assert(relativeError < eps, "The Time to SOD conversion found an incorrect SOD", __LINE__ );
			}

			return testFramework.countFails();
		}
	private:
		double eps;
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	xTimeConverters testClass;

	check = testClass.JDtoCalendarTest();
    errorCounter += check;

	check = testClass.CalendartoJDTest();
	errorCounter += check;

	check = testClass.SODtoTimeTest();
	errorCounter += check;

	check = testClass.TimetoSODTest(); 
	errorCounter += check;
	
	std::cout << "Total Errors for " << __FILE__<<": "<< errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
