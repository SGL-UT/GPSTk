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

#include "TimeTag.hpp"
#include "ANSITime.hpp"
#include "CivilTime.hpp"
#include "MJD.hpp"
#include "JulianDate.hpp"
#include "GPSWeekSecond.hpp"
#include "GPSWeekZcount.hpp"
#include "UnixTime.hpp"
#include "YDSTime.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace gpstk;
using namespace std;

//==========================================================================================================================
//	This test file will contain a series of scanf checks for each of the directly tested TimeTag classes.
//==========================================================================================================================

class TimeTag_T
{
	public:
	TimeTag_T() {}
	~TimeTag_T() {}

//==========================================================================================================================
//	ANSITime scanf Test
//==========================================================================================================================
	int scanfANSITime( void )
	{
		TestUtil testFramework( "TimeTag", "scanf(ANSITime)", __FILE__, __LINE__ );


		ANSITime hardCodedTime(13500000,TimeSystem(2));                //Set a hardcoded time
		std::string formatString = "%08K %03P";                        //Provide a format string
		std::string timeString = hardCodedTime.printf(formatString);   //Print the time using that format
		ANSITime scannedTime;

		scannedTime.scanf(timeString, formatString);                   //Read the formatted string back into a new
		                                                               //time variable

		//Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardCodedTime, "scanf was unable to scan the time appropriately", __LINE__);

		return testFramework.countFails();
	}

//==========================================================================================================================
//	CivilTime scanf Test
//==========================================================================================================================
	int scanfCivilTime( void )
	{
		TestUtil testFramework( "TimeTag", "scanf(CivilTime)", __FILE__, __LINE__ );


		CivilTime hardCodedTime(2008,8,21,13,30,15.,TimeSystem::UTC);      //Set a hardcoded time
		std::string formatString = "%04Y %02m %02d %02H %02M %02S %03P";   //Provide a format string
		std::string timeString = hardCodedTime.printf(formatString);       //Print the time using that format
		CivilTime scannedTime;

		scannedTime.scanf(timeString, formatString);                       //Read the formatted string back into a new
		                                                                   //time variable

		//Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardCodedTime, "scanf was unable to scan the time appropriately", __LINE__);

		scannedTime.reset();                                               //Reset the time for a new format to be scanned


		formatString = "%02y %02m %02d %02H %02M %02S %03P";               //Provide a format string
		timeString = hardCodedTime.printf(formatString);                   //Print the time using that format
		scannedTime.scanf(timeString, formatString);                       //Read the formatted string back into a new
		                                                                   //time variable

		//Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardCodedTime, "scanf was unable to scan the time appropriately", __LINE__);

		scannedTime.reset();                                               //Reset the time for a new format to be scanned

		formatString = "%04Y %03b %02d %02H %02M %02S %03P";               //Provide a format string
		timeString = hardCodedTime.printf(formatString);                   //Print the time using that format
		scannedTime.scanf(timeString, formatString);                       //Read the formatted string back into a new
		                                                                   //time variable

		//Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardCodedTime, "scanf was unable to scan the time appropriately", __LINE__);

		scannedTime.reset();                                               //Reset the time for a new format to be scanned

		formatString = "%04Y %03b %02d %02H %02M %5.2f %03P";               //Provide a format string
		timeString = hardCodedTime.printf(formatString);                   //Print the time using that format
		scannedTime.scanf(timeString, formatString);                       //Read the formatted string back into a new
		                                                                   //time variable

		//Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardCodedTime, "scanf was unable to scan the time appropriately", __LINE__);

		scannedTime.reset();                                               //Reset the time for a new format to be scanned

		formatString = "%02y %03b %02d %02H %02M %5.2f %03P";               //Provide a format string
		timeString = hardCodedTime.printf(formatString);                   //Print the time using that format
		scannedTime.scanf(timeString, formatString);                       //Read the formatted string back into a new
		                                                                   //time variable

		//Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardCodedTime, "scanf was unable to scan the time appropriately", __LINE__);

		scannedTime.reset();                                               //Reset the time for a new format to be scanned

		return testFramework.countFails();
	}



//==========================================================================================================================
//	GPSWeekSecond scanf Test
//==========================================================================================================================
	int scanfGPSWeekSecond( void )
	{
		TestUtil testFramework( "TimeTag", "scanf(GPSWeekSecond)", __FILE__, __LINE__ );


  		GPSWeekSecond hardCodedTime(1300,13500.,TimeSystem::GPS);      //Set a hardcoded time
		std::string formatString = "%04F %8.2g %03P";                   //Provide a format string
		std::string timeString = hardCodedTime.printf(formatString);   //Print the time using that format
		GPSWeekSecond scannedTime;

		scannedTime.scanf(timeString, formatString);                   //Read the formatted string back into a new
		                                                               //time variable

		//Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardCodedTime, "scanf was unable to scan the time appropriately", __LINE__);

		return testFramework.countFails();
	}

//==========================================================================================================================
//	GPSWeekZcount scanf Test
//==========================================================================================================================
	int scanfGPSWeekZcount( void )
	{
		TestUtil testFramework( "TimeTag", "scanf(GPSWeekZcount)", __FILE__, __LINE__ );


  		GPSWeekZcount hardCodedTime(1300,13500.,TimeSystem(2));        //Set a hardcoded time
		std::string formatString = "%04F %05z %03P";                   //Provide a format string
		std::string timeString = hardCodedTime.printf(formatString);   //Print the time using that format
		GPSWeekZcount scannedTime;

		scannedTime.scanf(timeString, formatString);                   //Read the formatted string back into a new
		                                                               //time variable

		//Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardCodedTime, "scanf was unable to scan the time appropriately", __LINE__);

		scannedTime.reset();                                           //Reset the time for a new format to be scanned


		formatString = "%04F %10C %03P";                               //Provide a format string
		timeString = hardCodedTime.printf(formatString);               //Print the time using that format
		scannedTime.scanf(timeString, formatString);                   //Read the formatted string back into a new
		                                                               //time variable

		//Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardCodedTime, "scanf was unable to scan the time appropriately", __LINE__);

		scannedTime.reset();                                           //Reset the time for a new format to be scanned


		formatString = "%04F %10c %03P";                               //Provide a format string
		timeString = hardCodedTime.printf(formatString);               //Print the time using that format
		scannedTime.scanf(timeString, formatString);                   //Read the formatted string back into a new
		                                                               //time variable

		//Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardCodedTime, "scanf was unable to scan the time appropriately", __LINE__);

		return testFramework.countFails();
	}

//==========================================================================================================================
//	JulianDate scanf Test
//==========================================================================================================================
	int scanfJulianDate( void )
	{
		TestUtil testFramework( "TimeTag", "scanf(JulianDate)", __FILE__, __LINE__ );


  		JulianDate hardCodedTime(1234567,TimeSystem(2));                //Set a hardcoded time
		std::string formatString = "%10.2J %03P";                        //Provide a format string
		std::string timeString = hardCodedTime.printf(formatString);   //Print the time using that format
		JulianDate scannedTime;

		scannedTime.scanf(timeString, formatString);                   //Read the formatted string back into a new
		                                                               //time variable

		//Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardCodedTime, "scanf was unable to scan the time appropriately", __LINE__);

		return testFramework.countFails();
	}

//==========================================================================================================================
//	MJD scanf Test
//==========================================================================================================================
	int scanfMJD( void )
	{
		TestUtil testFramework( "TimeTag", "scanf(MJD)", __FILE__, __LINE__ );


  		MJD hardCodedTime(123456,TimeSystem(1));                       //Set a hardcoded time
		std::string formatString = "%08Q %03P";                        //Provide a format string
		std::string timeString = hardCodedTime.printf(formatString);   //Print the time using that format
		MJD scannedTime;

		scannedTime.scanf(timeString, formatString);                   //Read the formatted string back into a new
		                                                               //time variable

		//Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardCodedTime, "scanf was unable to scan the time appropriately", __LINE__);

		return testFramework.countFails();
	}

//==========================================================================================================================
//	UnixTime scanf Test
//==========================================================================================================================
	int scanfUnixTime( void )
	{
		TestUtil testFramework( "TimeTag", "scanf(UnixTime)", __FILE__, __LINE__ );


  		UnixTime hardCodedTime(1654321,10,TimeSystem(5));              //Set a hardcoded time
		std::string formatString = "%07U %02u %03P";                   //Provide a format string
		std::string timeString = hardCodedTime.printf(formatString);   //Print the time using that format
		UnixTime scannedTime;

		scannedTime.scanf(timeString, formatString);                   //Read the formatted string back into a new
		                                                               //time variable

		//Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardCodedTime, "scanf was unable to scan the time appropriately", __LINE__);

		return testFramework.countFails();
	}


//==========================================================================================================================
//	YDSTime scanf Test
//==========================================================================================================================
	int scanfYDSTime( void )
	{
		TestUtil testFramework( "TimeTag", "scanf(YDSTime)", __FILE__, __LINE__ );


  		YDSTime hardCodedTime(2008,200,1000,TimeSystem::GPS);          //Set a hardcoded time
		std::string formatString = "%04Y %03j %7.2s %03P";              //Provide a format string
		std::string timeString = hardCodedTime.printf(formatString);   //Print the time using that format
		YDSTime scannedTime;

		scannedTime.scanf(timeString, formatString);                   //Read the formatted string back into a new
		                                                               //time variable

		//Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardCodedTime, "scanf was unable to scan the time appropriately", __LINE__);
		scannedTime.reset();                                           //Reset the time for a new format to be scanned


		formatString = "%02y %03j %7.2s %03P";                          //Provide a format string
		timeString = hardCodedTime.printf(formatString);               //Print the time using that format
		scannedTime.scanf(timeString, formatString);                   //Read the formatted string back into a new
		                                                               //time variable

		//Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardCodedTime, "scanf was unable to scan the time appropriately", __LINE__);

		return testFramework.countFails();
	}
};



int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	TimeTag_T testClass;

	check = testClass.scanfANSITime();
	errorCounter += check;

	check = testClass.scanfCivilTime();
	errorCounter += check;

	check = testClass.scanfGPSWeekSecond();
	errorCounter += check;

	check = testClass.scanfGPSWeekZcount();
	errorCounter += check;

	check = testClass.scanfJulianDate();
	errorCounter += check;

	check = testClass.scanfMJD();
	errorCounter += check;

	check = testClass.scanfUnixTime();
	errorCounter += check;

	check = testClass.scanfYDSTime();
	errorCounter += check;
	
	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
