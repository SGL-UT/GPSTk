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

//=============================================================================
//	This test file will contain a series of scanTime checks for
//	each of the directly tested TimeTag classes.
//=============================================================================
class TimeString_T
{
public:
   TimeString_T() {}
   ~TimeString_T() {}

//=============================================================================
//	ANSITime scanTime Test
//=============================================================================
   int scanTimeANSITime( void )
   {
      TestUtil testFramework( "TimeString", "scanTime(ANSITime)", __FILE__, __LINE__ );


      ANSITime scannedTime;
      CommonTime scannedCommonTime, hardcodedCommonTime;
      std::string formatString, timeString;
         // Set a hardcoded time
      ANSITime hardcodedTime(13500000,TimeSystem(2));
      hardcodedCommonTime = hardcodedTime.convertToCommonTime();

         // Provide a format string
      formatString = "%08K %03P";
         // Print the time using that format
      timeString = hardcodedTime.printf(formatString);
         // Read the formatted string back into a new time variable
      scanTime(scannedTime, timeString, formatString);
         // Verify the scanned time is the same as the hardcoded time
      TUASSERTE(ANSITime,hardcodedTime,scannedTime);

         // Reset the TimeTag child object
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      scanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(ANSITime,hardcodedTime,scannedTime);

      scannedCommonTime.reset();
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      mixedScanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CommonTime,hardcodedCommonTime,scannedCommonTime);
      
      return testFramework.countFails();
   }

//=============================================================================
//	CivilTime scanTime Test
//=============================================================================
   int scanTimeCivilTime( void )
   {
      TestUtil testFramework( "TimeString", "scanTime(CivilTime)", __FILE__, __LINE__ );


      CivilTime scannedTime;
      CommonTime scannedCommonTime, hardcodedCommonTime;
      std::string formatString, timeString;
         // Set a hardcoded time
      CivilTime hardcodedTime(2008,8,21,13,30,15.,TimeSystem::UTC);
      hardcodedCommonTime = hardcodedTime.convertToCommonTime();

         // Provide a format string
      formatString = "%04Y %02m %02d %02H %02M %02S %03P";
         // Print the time using that format
      timeString = hardcodedTime.printf(formatString);
         // Read the formatted string back into a new time variable
      scanTime(scannedTime, timeString, formatString);
         // Verify the scanned time is the same as the hardcoded time
      TUASSERTE(CivilTime,hardcodedTime,scannedTime);

         // Reset the TimeTag child object
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      scanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CivilTime,hardcodedTime,scannedTime);

      scannedCommonTime.reset();
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      mixedScanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CommonTime,hardcodedCommonTime,scannedCommonTime); 

      scannedTime.reset();


         // Provide a format string
      formatString = "%02y %02m %02d %02H %02M %02S %03P";
         // Print the time using that format
      timeString = hardcodedTime.printf(formatString);
         // Read the formatted string back into a new time variable
      scanTime(scannedTime, timeString, formatString);
         // Verify the scanned time is the same as the hardcoded time
      TUASSERTE(CivilTime,hardcodedTime,scannedTime);

         // Reset the TimeTag child object
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      scanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CivilTime,hardcodedTime,scannedTime);

      scannedCommonTime.reset();
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      mixedScanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);

         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CommonTime,hardcodedCommonTime,scannedCommonTime);

         // Reset the time for a new format to be scanned
      scannedTime.reset();

         // Provide a format string
      formatString = "%04Y %03b %02d %02H %02M %02S %03P";
         // Print the time using that format
      timeString = hardcodedTime.printf(formatString);
         // Read the formatted string back into a new time variable
      scanTime(scannedTime, timeString, formatString);
         // Verify the scanned time is the same as the hardcoded time
      TUASSERTE(CivilTime,hardcodedTime,scannedTime);

         // Reset the TimeTag child object
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      scanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CivilTime,hardcodedTime,scannedTime);

      scannedCommonTime.reset();
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      mixedScanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);

         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CommonTime,hardcodedCommonTime,scannedCommonTime); 

         // Reset the time for a new format to be scanned
      scannedTime.reset();

         // Provide a format string
      formatString = "%04Y %03b %02d %02H %02M %5.2f %03P";
         // Print the time using that format
      timeString = hardcodedTime.printf(formatString);
         // Read the formatted string back into a new time variable
      scanTime(scannedTime, timeString, formatString);
         // Verify the scanned time is the same as the hardcoded time
      TUASSERTE(CivilTime,hardcodedTime,scannedTime);

         // Reset the TimeTag child object
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      scanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CivilTime,hardcodedTime,scannedTime);

      scannedCommonTime.reset();
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      mixedScanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);

         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CommonTime,hardcodedCommonTime,scannedCommonTime); 

         // Reset the time for a new format to be scanned
      scannedTime.reset();

         // Provide a format string
      formatString = "%02y %03b %02d %02H %02M %5.2f %03P";
         // Print the time using that format
      timeString = hardcodedTime.printf(formatString);
         // Read the formatted string back into a new time variable
      scanTime(scannedTime, timeString, formatString);
         // Verify the scanned time is the same as the hardcoded time
      TUASSERTE(CivilTime,hardcodedTime,scannedTime);

         // Reset the TimeTag child object
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      scanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CivilTime,hardcodedTime,scannedTime);

      scannedCommonTime.reset();
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      mixedScanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);

         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CommonTime,hardcodedCommonTime,scannedCommonTime); 

         // Reset the time for a new format to be scanned
      scannedTime.reset();

      return testFramework.countFails();
   }



//=============================================================================
// 	GPSWeekSecond scanTime Test
//=============================================================================
   int scanTimeGPSWeekSecond( void )
   {
      TestUtil testFramework( "TimeString", "scanTime(GPSWeekSecond)", __FILE__, __LINE__ );


      GPSWeekSecond scannedTime;
      CommonTime scannedCommonTime, hardcodedCommonTime;
      std::string formatString, timeString;
         // Set a hardcoded time
      GPSWeekSecond hardcodedTime(1300,13500.,TimeSystem::GPS);
      hardcodedCommonTime = hardcodedTime.convertToCommonTime();

         // Provide a format string
      formatString = "%04F %8.2g %03P";
         // Print the time using that format
      timeString = hardcodedTime.printf(formatString);
         // Read the formatted string back into a new time variable
      scanTime(scannedTime, timeString, formatString);
         // Verify the scanned time is the same as the hardcoded time
      TUASSERTE(GPSWeekSecond,hardcodedTime,scannedTime);

         // Reset the TimeTag child object
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      scanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(GPSWeekSecond,hardcodedTime,scannedTime);

      scannedCommonTime.reset();
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      mixedScanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CommonTime,hardcodedCommonTime,scannedCommonTime); 

      return testFramework.countFails();
   }

//=============================================================================
// 	GPSWeekZcount scanTime Test
//=============================================================================
   int scanTimeGPSWeekZcount( void )
   {
      TestUtil testFramework( "TimeString", "scanTime(GPSWeekZcount)", __FILE__, __LINE__ );


      GPSWeekZcount scannedTime;
      CommonTime scannedCommonTime, hardcodedCommonTime;
      std::string formatString, timeString;
         // Set a hardcoded time
      GPSWeekZcount hardcodedTime(1300,13500.,TimeSystem(2));
      hardcodedCommonTime = hardcodedTime.convertToCommonTime();

         // Provide a format string
      formatString = "%04F %05z %03P";
         // Print the time using that format
      timeString = hardcodedTime.printf(formatString);
         // Read the formatted string back into a new time variable
      scanTime(scannedTime, timeString, formatString);
         // Verify the scanned time is the same as the hardcoded time
      TUASSERTE(GPSWeekZcount,hardcodedTime,scannedTime);

         // Reset the TimeTag child object
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      scanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(GPSWeekZcount,hardcodedTime,scannedTime);

      scannedCommonTime.reset();
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      mixedScanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CommonTime,hardcodedCommonTime,scannedCommonTime); 

         // Reset the time for a new format to be scanned
      scannedTime.reset();


         // Provide a format string
      formatString = "%04F %10C %03P";
         // Print the time using that format
      timeString = hardcodedTime.printf(formatString);
         // Read the formatted string back into a new time variable
      scanTime(scannedTime, timeString, formatString);
         // Verify the scanned time is the same as the hardcoded time
      TUASSERTE(GPSWeekZcount,hardcodedTime,scannedTime);

         // Reset the TimeTag child object
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      scanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(GPSWeekZcount,hardcodedTime,scannedTime);

      scannedCommonTime.reset();
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      mixedScanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CommonTime,hardcodedCommonTime,scannedCommonTime); 

         // Reset the time for a new format to be scanned
      scannedTime.reset();


         // Provide a format string
      formatString = "%04F %10c %03P";
         // Print the time using that format
      timeString = hardcodedTime.printf(formatString);
         // Read the formatted string back into a new time variable
      scanTime(scannedTime, timeString, formatString);
         // Verify the scanned time is the same as the hardcoded time
      TUASSERTE(GPSWeekZcount,hardcodedTime,scannedTime);

         // Reset the TimeTag child object
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      scanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);

         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(GPSWeekZcount,hardcodedTime,scannedTime);

      scannedCommonTime.reset();
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      mixedScanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CommonTime,hardcodedCommonTime,scannedCommonTime); 

      return testFramework.countFails();
   }

//=============================================================================
// 	JulianDate scanTime Test
//=============================================================================
   int scanTimeJulianDate( void )
   {
      TestUtil testFramework( "TimeString", "scanTime(JulianDate)", __FILE__, __LINE__ );


      JulianDate scannedTime;
      CommonTime scannedCommonTime, hardcodedCommonTime;
      std::string formatString, timeString;
         // Set a hardcoded time
      JulianDate hardcodedTime(1234567,TimeSystem(2));
      hardcodedCommonTime = hardcodedTime.convertToCommonTime();

         // Provide a format string
      formatString = "%10.2J %03P";
         // Print the time using that format
      timeString = hardcodedTime.printf(formatString);
         // Read the formatted string back into a new time variable
      scanTime(scannedTime, timeString, formatString);
         // Verify the scanned time is the same as the hardcoded time
      TUASSERTE(JulianDate,hardcodedTime,scannedTime);

         // Reset the TimeTag child object
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      scanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(JulianDate,hardcodedTime,scannedTime);

      scannedCommonTime.reset();
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      mixedScanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);

         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CommonTime,hardcodedCommonTime,scannedCommonTime); 

      return testFramework.countFails();
   }

//=============================================================================
// 	MJD scanTime Test
//=============================================================================
   int scanTimeMJD( void )
   {
      TestUtil testFramework( "TimeString", "scanTime(MJD)", __FILE__, __LINE__ );


      MJD scannedTime;
      CommonTime scannedCommonTime, hardcodedCommonTime;
      std::string formatString, timeString;
         // Set a hardcoded time
      MJD hardcodedTime(123456.0,TimeSystem(1));
      hardcodedCommonTime = hardcodedTime.convertToCommonTime();

         // Provide a format string
      formatString = "%9.2Q %03P";
         // Print the time using that format
      timeString = hardcodedTime.printf(formatString);
         // Read the formatted string back into a new time variable
      scanTime(scannedTime, timeString, formatString);
         // Verify the scanned time is the same as the hardcoded time
      TUASSERTE(MJD,hardcodedTime,scannedTime);

         // Reset the TimeTag child object
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      scanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(MJD,hardcodedTime,scannedTime);

      scannedCommonTime.reset();
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      mixedScanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CommonTime,hardcodedCommonTime,scannedCommonTime); 

      return testFramework.countFails();
   }

//=============================================================================
// 	UnixTime scanTime Test
//=============================================================================
   int scanTimeUnixTime( void )
   {
      TestUtil testFramework( "TimeString", "scanTime(UnixTime)", __FILE__, __LINE__ );


      UnixTime scannedTime;
      CommonTime scannedCommonTime, hardcodedCommonTime;
      std::string formatString, timeString;
         // Set a hardcoded time
      UnixTime hardcodedTime(1654321,10,TimeSystem(5));
      hardcodedCommonTime = hardcodedTime.convertToCommonTime();

         // Provide a format string
      formatString = "%07U %02u %03P";
         // Print the time using that format
      timeString = hardcodedTime.printf(formatString);
         // Read the formatted string back into a new time variable
      scanTime(scannedTime, timeString, formatString);
         // Verify the scanned time is the same as the hardcoded time
      TUASSERTE(UnixTime,hardcodedTime,scannedTime);

         // Reset the TimeTag child object
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      scanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(UnixTime,hardcodedTime,scannedTime);

      scannedCommonTime.reset();
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      mixedScanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CommonTime,hardcodedCommonTime,scannedCommonTime); 

      return testFramework.countFails();
   }


//=============================================================================
// 	YDSTime scanTime Test
//=============================================================================
   int scanTimeYDSTime( void )
   {
      TestUtil testFramework( "TimeString", "scanTime(YDSTime)", __FILE__, __LINE__ );


      YDSTime scannedTime;
      CommonTime scannedCommonTime, hardcodedCommonTime;
      std::string formatString, timeString;
         // Set a hardcoded time
      YDSTime hardcodedTime(2008,200,1000,TimeSystem::GPS);
      hardcodedCommonTime = hardcodedTime.convertToCommonTime();

         // Provide a format string
      formatString = "%04Y %03j %7.2s %03P";
         // Print the time using that format
      timeString = hardcodedTime.printf(formatString);
         // Read the formatted string back into a new time variable
      scanTime(scannedTime, timeString, formatString);
         // Verify the scanned time is the same as the hardcoded time
      TUASSERTE(YDSTime,hardcodedTime,scannedTime);

         // Reset the TimeTag child object
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      scanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(YDSTime,hardcodedTime,scannedTime);

      scannedCommonTime.reset();
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      mixedScanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CommonTime,hardcodedCommonTime,scannedCommonTime); 

      scannedTime.reset();
      scannedCommonTime.reset();

         // Provide a format string
      formatString = "%02y %03j %7.2s %03P";
         // Print the time using that format
      timeString = hardcodedTime.printf(formatString);
         // Read the formatted string back into a new time variable
      scanTime(scannedTime, timeString, formatString);
         // Verify the scanned time is the same as the hardcoded time
      TUASSERTE(YDSTime,hardcodedTime,scannedTime);

         // Reset the TimeTag child object
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      scanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(YDSTime,hardcodedTime,scannedTime);

      scannedCommonTime.reset();
      scannedTime.reset();
         // Read the formatted string back into a CommonTime time
         // variable
      mixedScanTime(scannedCommonTime, timeString, formatString);
         // Convert to the TimeTag child class
      scannedTime.convertFromCommonTime(scannedCommonTime);
         // Verify the scanned CommonTime time is the same as the
         // hardcoded time
      TUASSERTE(CommonTime,hardcodedCommonTime,scannedCommonTime); 
      return testFramework.countFails();
   }
};



int main() // Main function to initialize and run all tests above
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

      // Return the total number of errors
   return errorCounter;
}
