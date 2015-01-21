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
#include "TimeString.hpp"
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

/* =========================================================================================================================
	This test file will contain a series of scanTime checks for each of the directly tested TimeTag classes.
========================================================================================================================= */

class TimeString_T
{
	public:
	TimeString_T() {}
	~TimeString_T() {}

/* =========================================================================================================================
	ANSITime scanTime Test
========================================================================================================================= */
	int scanTimeANSITime( void )
	{
		TestUtil testFramework( "TimeString", "scanTime(ANSITime)", __FILE__, __func__ );
		testFramework.init();

		ANSITime scannedTime;
		CommonTime scannedCommonTime, hardcodedCommonTime;
		std::string formatString, timeString;
		ANSITime hardcodedTime(13500000,TimeSystem(2));                //Set a hardcoded time
		hardcodedCommonTime = hardcodedTime.convertToCommonTime();

		formatString = "%08K %03P";                                    //Provide a format string
		timeString = hardcodedTime.printf(formatString);               //Print the time using that format
		scanTime(scannedTime, timeString, formatString);               //Read the formatted string back into a new
		                                                               //time variable
//--------------TimeString_scanTimeANSITime_1 - Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedTime.reset();                                           //Reset the TimeTag child object
		scanTime(scannedCommonTime, timeString, formatString);         //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeANSITime_2 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedCommonTime.reset();
		scannedTime.reset();
		mixedScanTime(scannedCommonTime, timeString, formatString);    //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeANSITime_3 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedCommonTime == hardcodedCommonTime);     
      
		return testFramework.countFails();
	}

/* =========================================================================================================================
	CivilTime scanTime Test
========================================================================================================================= */
	int scanTimeCivilTime( void )
	{
		TestUtil testFramework( "TimeString", "scanTime(CivilTime)", __FILE__, __func__ );
		testFramework.init();

		CivilTime scannedTime;
		CommonTime scannedCommonTime, hardcodedCommonTime;
		std::string formatString, timeString;
		CivilTime hardcodedTime(2008,8,21,13,30,15.,TimeSystem::UTC);  //Set a hardcoded time
		hardcodedCommonTime = hardcodedTime.convertToCommonTime();

		formatString = "%04Y %02m %02d %02H %02M %02S %03P";           //Provide a format string
		timeString = hardcodedTime.printf(formatString);               //Print the time using that format
		scanTime(scannedTime, timeString, formatString);               //Read the formatted string back into a new
		                                                               //time variable
//--------------TimeString_scanTimeCivilTime_1 - Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedTime.reset();                                           //Reset the TimeTag child object
		scanTime(scannedCommonTime, timeString, formatString);         //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeCivilTime_2 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedCommonTime.reset();
		scannedTime.reset();
		mixedScanTime(scannedCommonTime, timeString, formatString);    //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeCivilTime_3 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedCommonTime == hardcodedCommonTime); 
		testFramework.next();
		scannedTime.reset();


		formatString = "%02y %02m %02d %02H %02M %02S %03P";           //Provide a format string
		timeString = hardcodedTime.printf(formatString);               //Print the time using that format
		scanTime(scannedTime, timeString, formatString);               //Read the formatted string back into a new
		                                                               //time variable
//--------------TimeString_scanTimeCivilTime_4 - Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedTime.reset();                                           //Reset the TimeTag child object
		scanTime(scannedCommonTime, timeString, formatString);         //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeCivilTime_5 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedCommonTime.reset();
		scannedTime.reset();
		mixedScanTime(scannedCommonTime, timeString, formatString);    //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class

//--------------TimeString_scanTimeCivilTime_6 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedCommonTime == hardcodedCommonTime); 
		testFramework.next();
		scannedTime.reset();                                           //Reset the time for a new format to be scanned

		formatString = "%04Y %03b %02d %02H %02M %02S %03P";           //Provide a format string
		timeString = hardcodedTime.printf(formatString);               //Print the time using that format
		scanTime(scannedTime, timeString, formatString);               //Read the formatted string back into a new
		                                                               //time variable
//--------------TimeString_scanTimeCivilTime_7 - Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedTime.reset();                                           //Reset the TimeTag child object
		scanTime(scannedCommonTime, timeString, formatString);         //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeCivilTime_8 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedCommonTime.reset();
		scannedTime.reset();
		mixedScanTime(scannedCommonTime, timeString, formatString);    //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class

//--------------TimeString_scanTimeCivilTime_9 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedCommonTime == hardcodedCommonTime); 
		testFramework.next();
		scannedTime.reset();                                           //Reset the time for a new format to be scanned

		formatString = "%04Y %03b %02d %02H %02M %5.2f %03P";          //Provide a format string
		timeString = hardcodedTime.printf(formatString);               //Print the time using that format
		scanTime(scannedTime, timeString, formatString);               //Read the formatted string back into a new
		                                                               //time variable
//--------------TimeString_scanTimeCivilTime_10 - Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedTime.reset();                                           //Reset the TimeTag child object
		scanTime(scannedCommonTime, timeString, formatString);         //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeCivilTime_11 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedCommonTime.reset();
		scannedTime.reset();
		mixedScanTime(scannedCommonTime, timeString, formatString);    //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class

//--------------TimeString_scanTimeCivilTime_12 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedCommonTime == hardcodedCommonTime); 
		testFramework.next();
		scannedTime.reset();                                           //Reset the time for a new format to be scanned

		formatString = "%02y %03b %02d %02H %02M %5.2f %03P";          //Provide a format string
		timeString = hardcodedTime.printf(formatString);               //Print the time using that format
		scanTime(scannedTime, timeString, formatString);               //Read the formatted string back into a new
		                                                               //time variable
//--------------TimeString_scanTimeCivilTime_13 - Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedTime.reset();                                           //Reset the TimeTag child object
		scanTime(scannedCommonTime, timeString, formatString);         //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeCivilTime_14 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedCommonTime.reset();
		scannedTime.reset();
		mixedScanTime(scannedCommonTime, timeString, formatString);    //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class

//--------------TimeString_scanTimeCivilTime_15 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedCommonTime == hardcodedCommonTime); 
		testFramework.next();
		scannedTime.reset();                                           //Reset the time for a new format to be scanned

		return testFramework.countFails();
	}



/* =========================================================================================================================
	GPSWeekSecond scanTime Test
========================================================================================================================= */
	int scanTimeGPSWeekSecond( void )
	{
		TestUtil testFramework( "TimeString", "scanTime(GPSWeekSecond)", __FILE__, __func__ );
		testFramework.init();

		GPSWeekSecond scannedTime;
		CommonTime scannedCommonTime, hardcodedCommonTime;
		std::string formatString, timeString;
  		GPSWeekSecond hardcodedTime(1300,13500.,TimeSystem::GPS);      //Set a hardcoded time
		hardcodedCommonTime = hardcodedTime.convertToCommonTime();

		formatString = "%04F %8.2g %03P";                              //Provide a format string
		timeString = hardcodedTime.printf(formatString);               //Print the time using that format
		scanTime(scannedTime, timeString, formatString);               //Read the formatted string back into a new
		                                                               //time variable
//--------------TimeString_scanTimeGPSWeekSecond_1 - Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedTime.reset();                                           //Reset the TimeTag child object
		scanTime(scannedCommonTime, timeString, formatString);         //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeGPSWeekSecond_2 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedCommonTime.reset();
		scannedTime.reset();
		mixedScanTime(scannedCommonTime, timeString, formatString);    //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeGPSWeekSecond_3 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedCommonTime == hardcodedCommonTime); 

		return testFramework.countFails();
	}

/* =========================================================================================================================
	GPSWeekZcount scanTime Test
========================================================================================================================= */
	int scanTimeGPSWeekZcount( void )
	{
		TestUtil testFramework( "TimeString", "scanTime(GPSWeekZcount)", __FILE__, __func__ );
		testFramework.init();

		GPSWeekZcount scannedTime;
		CommonTime scannedCommonTime, hardcodedCommonTime;
		std::string formatString, timeString;
  		GPSWeekZcount hardcodedTime(1300,13500.,TimeSystem(2));        //Set a hardcoded time
		hardcodedCommonTime = hardcodedTime.convertToCommonTime();

		formatString = "%04F %05z %03P";                               //Provide a format string
		timeString = hardcodedTime.printf(formatString);               //Print the time using that format
		scanTime(scannedTime, timeString, formatString);               //Read the formatted string back into a new
		                                                               //time variable
//--------------TimeString_scanTimeGPSWeekZcount_1 - Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedTime.reset();                                           //Reset the TimeTag child object
		scanTime(scannedCommonTime, timeString, formatString);         //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeGPSWeekZcount_2 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedCommonTime.reset();
		scannedTime.reset();
		mixedScanTime(scannedCommonTime, timeString, formatString);    //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeGPSWeekZcount_3 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedCommonTime == hardcodedCommonTime); 
		testFramework.next();
		scannedTime.reset();                                           //Reset the time for a new format to be scanned


		formatString = "%04F %10C %03P";                               //Provide a format string
		timeString = hardcodedTime.printf(formatString);               //Print the time using that format
		scanTime(scannedTime, timeString, formatString);               //Read the formatted string back into a new
		                                                               //time variable
//--------------TimeString_scanTimeGPSWeekZcount_4 - Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedTime.reset();                                           //Reset the TimeTag child object
		scanTime(scannedCommonTime, timeString, formatString);         //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeGPSWeekZcount_5 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedCommonTime.reset();
		scannedTime.reset();
		mixedScanTime(scannedCommonTime, timeString, formatString);    //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeGPSWeekZcount_6 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedCommonTime == hardcodedCommonTime); 
		testFramework.next();
		scannedTime.reset();                                           //Reset the time for a new format to be scanned


		formatString = "%04F %10c %03P";                               //Provide a format string
		timeString = hardcodedTime.printf(formatString);               //Print the time using that format
		scanTime(scannedTime, timeString, formatString);               //Read the formatted string back into a new
		                                                               //time variable
//--------------TimeString_scanTimeGPSWeekZcount_7 - Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedTime.reset();                                           //Reset the TimeTag child object
		scanTime(scannedCommonTime, timeString, formatString);         //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class

//--------------TimeString_scanTimeGPSWeekZcount_8 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedCommonTime.reset();
		scannedTime.reset();
		mixedScanTime(scannedCommonTime, timeString, formatString);    //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeGPSWeekZcount_9 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedCommonTime == hardcodedCommonTime); 

		return testFramework.countFails();
	}

/* =========================================================================================================================
	JulianDate scanTime Test
========================================================================================================================= */
	int scanTimeJulianDate( void )
	{
		TestUtil testFramework( "TimeString", "scanTime(JulianDate)", __FILE__, __func__ );
		testFramework.init();

		JulianDate scannedTime;
		CommonTime scannedCommonTime, hardcodedCommonTime;
		std::string formatString, timeString;
  		JulianDate hardcodedTime(1234567,TimeSystem(2));               //Set a hardcoded time
		hardcodedCommonTime = hardcodedTime.convertToCommonTime();

		formatString = "%10.2J %03P";                                  //Provide a format string
		timeString = hardcodedTime.printf(formatString);               //Print the time using that format
		scanTime(scannedTime, timeString, formatString);               //Read the formatted string back into a new
		                                                               //time variable
//--------------TimeString_scanTimeJulianDate_1 - Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedTime.reset();                                           //Reset the TimeTag child object
		scanTime(scannedCommonTime, timeString, formatString);         //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeJulianDate_2 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedCommonTime.reset();
		scannedTime.reset();
		mixedScanTime(scannedCommonTime, timeString, formatString);    //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class

//--------------TimeString_scanTimeJulianDate_3 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedCommonTime == hardcodedCommonTime); 

		return testFramework.countFails();
	}

/* =========================================================================================================================
	MJD scanTime Test
========================================================================================================================= */
	int scanTimeMJD( void )
	{
		TestUtil testFramework( "TimeString", "scanTime(MJD)", __FILE__, __func__ );
		testFramework.init();

		MJD scannedTime;
		CommonTime scannedCommonTime, hardcodedCommonTime;
		std::string formatString, timeString;
  		MJD hardcodedTime(123456.0,TimeSystem(1));                       //Set a hardcoded time
		hardcodedCommonTime = hardcodedTime.convertToCommonTime();

		formatString = "%9.2Q %03P";                                    //Provide a format string
		timeString = hardcodedTime.printf(formatString);               //Print the time using that format
		scanTime(scannedTime, timeString, formatString);               //Read the formatted string back into a new
		                                                               //time variable
//--------------TimeString_scanTimeMJD_1 - Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedTime.reset();                                           //Reset the TimeTag child object
		scanTime(scannedCommonTime, timeString, formatString);         //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeMJD_2 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedCommonTime.reset();
		scannedTime.reset();
		mixedScanTime(scannedCommonTime, timeString, formatString);    //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeMJD_3 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedCommonTime == hardcodedCommonTime); 

		return testFramework.countFails();
	}

/* =========================================================================================================================
	UnixTime scanTime Test
========================================================================================================================= */
	int scanTimeUnixTime( void )
	{
		TestUtil testFramework( "TimeString", "scanTime(UnixTime)", __FILE__, __func__ );
		testFramework.init();

		UnixTime scannedTime;
		CommonTime scannedCommonTime, hardcodedCommonTime;
		std::string formatString, timeString;
  		UnixTime hardcodedTime(1654321,10,TimeSystem(5));              //Set a hardcoded time
		hardcodedCommonTime = hardcodedTime.convertToCommonTime();

		formatString = "%07U %02u %03P";                               //Provide a format string
		timeString = hardcodedTime.printf(formatString);               //Print the time using that format
		scanTime(scannedTime, timeString, formatString);               //Read the formatted string back into a new
		                                                               //time variable
//--------------TimeString_scanTimeUnixTime_1 - Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedTime.reset();                                           //Reset the TimeTag child object
		scanTime(scannedCommonTime, timeString, formatString);         //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeUnixTime_2 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedCommonTime.reset();
		scannedTime.reset();
		mixedScanTime(scannedCommonTime, timeString, formatString);    //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeUnixTime_3 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedCommonTime == hardcodedCommonTime); 

		return testFramework.countFails();
	}


/* =========================================================================================================================
	YDSTime scanTime Test
========================================================================================================================= */
	int scanTimeYDSTime( void )
	{
		TestUtil testFramework( "TimeString", "scanTime(YDSTime)", __FILE__, __func__ );
		testFramework.init();

		YDSTime scannedTime;
		CommonTime scannedCommonTime, hardcodedCommonTime;
		std::string formatString, timeString;
  		YDSTime hardcodedTime(2008,200,1000,TimeSystem::GPS);          //Set a hardcoded time
		hardcodedCommonTime = hardcodedTime.convertToCommonTime();

		formatString = "%04Y %03j %7.2s %03P";                         //Provide a format string
		timeString = hardcodedTime.printf(formatString);               //Print the time using that format
		scanTime(scannedTime, timeString, formatString);               //Read the formatted string back into a new
		                                                               //time variable
//--------------TimeString_scanTimeYDSTime_1 - Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedTime.reset();                                           //Reset the TimeTag child object
		scanTime(scannedCommonTime, timeString, formatString);         //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeYDSTime_2 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedCommonTime.reset();
		scannedTime.reset();
		mixedScanTime(scannedCommonTime, timeString, formatString);    //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeYDSTime_3 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedCommonTime == hardcodedCommonTime); 
		testFramework.next();
		scannedTime.reset();
		scannedCommonTime.reset();

		formatString = "%02y %03j %7.2s %03P";                         //Provide a format string
		timeString = hardcodedTime.printf(formatString);               //Print the time using that format
		scanTime(scannedTime, timeString, formatString);               //Read the formatted string back into a new
		                                                               //time variable
//--------------TimeString_scanTimeYDSTime_1 - Verify the scanned time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedTime.reset();                                           //Reset the TimeTag child object
		scanTime(scannedCommonTime, timeString, formatString);         //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeYDSTime_2 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedTime == hardcodedTime);
		testFramework.next();
		scannedCommonTime.reset();
		scannedTime.reset();
		mixedScanTime(scannedCommonTime, timeString, formatString);    //Read the formatted string back into a
		                                                               //CommonTime time variable
		scannedTime.convertFromCommonTime(scannedCommonTime);          //Convert to the TimeTag child class
//--------------TimeString_scanTimeYDSTime_3 - Verify the scanned CommonTime time is the same as the hardcoded time
		testFramework.assert(scannedCommonTime == hardcodedCommonTime); 
		return testFramework.countFails();
	}
};



int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	TimeString_T testClass;

	check = testClass.scanTimeANSITime();
	errorCounter += check;

	check = testClass.scanTimeCivilTime();
	errorCounter += check;

	check = testClass.scanTimeGPSWeekSecond();
	errorCounter += check;

	check = testClass.scanTimeGPSWeekZcount();
	errorCounter += check;

	check = testClass.scanTimeJulianDate();
	errorCounter += check;

	check = testClass.scanTimeMJD();
	errorCounter += check;

	check = testClass.scanTimeUnixTime();
	errorCounter += check;

	check = testClass.scanTimeYDSTime();
	errorCounter += check;
	
	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
