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

#include "TestUtil.hpp"
#include "SystemTime.hpp"
#include "ANSITime.hpp"
#include "CommonTime.hpp"
#include "UnixTime.hpp"
#include <iostream>
#include <ctime>

using namespace std;
using namespace gpstk;

class SystemTime_T
{
        public: 
		SystemTime_T(){}// Default Constructor, set the precision value
		~SystemTime_T() {} // Default Desructor

		/* SystemTime inherits from UnixTime. The only added ability is to read the time from the machine.
                   This one test will read the system time and compare it with the ctime value. However, since
                   this test will need to read the time twice no guarantees can be made for strict tolerances.
                   In fact, the tolerance will be set so that the time will be accurate to the second only. */
		int obtainTimeFromSystemTest ( void )
		{
			TestUtil testFramework( "SystemTime", "Constructor", __FILE__, __LINE__ );
			testFramework.init();

			long day, day2;
			long sod, sod2;
			double fsod, fsod2;
			
			SystemTime timeFrom_SystemTime;
			time_t t;
			time( &t );
			ANSITime   timeFrom_ctime( t );

			CommonTime commonTime_SystemTime = timeFrom_SystemTime.convertToCommonTime();
			CommonTime commonTime_ctime      = timeFrom_ctime.convertToCommonTime();

			commonTime_SystemTime.get(day,sod,fsod);
			commonTime_ctime.get(day2,sod2,fsod2);

//--------------SystemTime_ _1 - Is the day value accurate?
			testFramework.assert( day == day2 );
			testFramework.next();

//--------------SystemTime_ _2 - Is the second value accurate?
			testFramework.assert( sod == sod2 );
			testFramework.next();

//--------------SystemTime_ _3 - Is the TimeSystem set to UTC
			testFramework.assert( commonTime_SystemTime.getTimeSystem() == TimeSystem(7) );
			testFramework.next();

			return testFramework.countFails();
		}

};


int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	SystemTime_T testClass;

	check = testClass.obtainTimeFromSystemTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
