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
#include "convhelp.hpp"
#include "GPSEllipsoid.hpp"

#include "TestUtil.hpp"
#include <iostream>

class Convhelp_T
{
    public: 
	Convhelp_T(){ eps = 1E-12;}// Default Constructor, set the precision value
	~Convhelp_T() {} // Default Desructor

	int cycles2metersTest(void) 
	{
		TestUtil testFramework( "convhelp", "cycles2meters(phase, frequency, elliposid model)", __FILE__, __LINE__ );
		testFramework.init();

		gpstk::WGS84Ellipsoid wem;
		//gpstk:GPSEllipsoid gem;

//--------------Convhelp_cycles2meters_1 - Did the computation work with positive values?
		testFramework.assert(abs(gpstk::cycles2meters(100000.,400000.,wem) - 74948114.5) < eps);
		testFramework.next();
		//testFramework.assert(fabs(gpstk::cycles2meters(10000,40000,gem) - 1.3342563807926082 ) < eps);
		//testFramework.next();
//--------------Convhelp_cycles2meters_2 - Did the computation work with a 0 value?
		testFramework.assert(gpstk::cycles2meters(0,5,wem) == 0);
		testFramework.next();
		//testFramework.assert(gpstk::cycles2meters(0,5,gem) == 0);
		//testFramework.next();
//--------------Convhelp_cycles2meters_3 - Did the computation work with a negative value?		
		testFramework.assert(abs(gpstk::cycles2meters(-100000.,400000.,wem) + 74948114.5) < eps);

		return testFramework.countFails();
	}

	int meters2cyclesTest(void)
	{
		TestUtil testFramework( "convhelp", "meters2cycles(phase, frequency, elliposid model)", __FILE__, __LINE__ );
		testFramework.init();

		gpstk::WGS84Ellipsoid wem;
		gpstk::GPSEllipsoid gem;

//--------------Convhelp_meters2cyclesTest_1 - Did the computation work with positive values?
		testFramework.assert(abs(gpstk::meters2cycles(74948114.5,400000.,wem) - 100000) < eps);
		testFramework.next();
//--------------Convhelp_meters2cyclesTest_2 - Did the above computation work with the GPS ellipsoid?		
		testFramework.assert(abs(gpstk::meters2cycles(10000,40000,gem) - 1.3342563807926082 ) < eps);
		testFramework.next();
//--------------Convhelp_meters2cyclesTest_3 - Did the computation work with a 0 value?
		testFramework.assert(gpstk::meters2cycles(0,5,wem) == 0);
		testFramework.next();
//--------------Convhelp_meters2cyclesTest_4 - Did the above computation work with the GPS ellipsoid?		
		testFramework.assert(gpstk::meters2cycles(0,5,gem) == 0);
		testFramework.next();
//--------------Convhelp_meters2cyclesTest_5 - Did the computation work with a negative value?		
		testFramework.assert(abs(gpstk::meters2cycles(-74948114.5,400000.,wem) + 100000) < eps);
		testFramework.next();
//--------------Convhelp_meters2cyclesTest_6 -Did the above computation work with the GPS ellipsoid?
		testFramework.assert(abs(gpstk::meters2cycles(-74948114.5,400000.,wem) + 100000) < eps);
		testFramework.next();

		return testFramework.countFails();
	}

	int cel2farTest(void)
	{
		TestUtil testFramework( "convhelp", "cel2far(Celsius)", __FILE__, __LINE__ );
		testFramework.init();

//--------------Convhelp_cel2farTest_1 - Did the computation work with a postitive value?
		testFramework.assert(gpstk::cel2far(100) == 212);
		testFramework.next();
//--------------Convhelp_cel2farTest_2 - Did the computation work with a 0 value?
		testFramework.assert(gpstk::cel2far(0) == 32);
		testFramework.next();
//--------------Convhelp_cel2farTest_3 - Did the computation work with a negative value?		
		testFramework.assert(gpstk::cel2far(-100) == -148);

		return testFramework.countFails();
	}

	int far2celTest(void)
	{
		TestUtil testFramework( "convhelp", "far2cel(Farenheit)", __FILE__, __LINE__ );
		testFramework.init();

//--------------Convhelp_far2celTest_1 - Did the computation work with a postitive value?
		testFramework.assert(gpstk::far2cel(212) == 100);
		testFramework.next();
//--------------Convhelp_far2celTest_2 - Did the computation work with a 0 value?		
		testFramework.assert(gpstk::far2cel(32) == 0);
		testFramework.next();
//--------------Convhelp_far2celTest_3 - Did the computation work with a negative value?		
		testFramework.assert(gpstk::far2cel(-148) == -100);

		return testFramework.countFails();
	}

	int mb2hgTest(void)
	{
		TestUtil testFramework( "convhelp", "mb2hg(Millibar)", __FILE__, __LINE__ );
		testFramework.init();

//--------------Convhelp_mb2hgTest_1 - Did the computation work with a postitive value?
		testFramework.assert(abs(gpstk::mb2hg(100) - 2.9529987508079487) < eps);
		testFramework.next();
//--------------Convhelp_mb2hgTest_2 - Did the computation work with a 0 value?		
		testFramework.assert(gpstk::mb2hg(0) == 0);
		testFramework.next();
//--------------Convhelp_mb2hgTest_3 - Did the computation work with a negative value?		
		testFramework.assert(abs(gpstk::mb2hg(-100) + 2.9529987508079487) < eps);

		return testFramework.countFails();
	}

	int hg2mbTest(void)
	{
		TestUtil testFramework( "convhelp", "hg2mb(inchOfMercury)", __FILE__, __LINE__ );
		testFramework.init();

//--------------Convhelp_hg2mbTest_1 - Did the computation work with a postitive value?
		testFramework.assert(abs(gpstk::hg2mb(2.9529987508079487) - 100) < eps);
		testFramework.next();
//--------------Convhelp_hg2mbTest_2 - Did the computation work with a 0 value?		
		testFramework.assert(gpstk::hg2mb(0) == 0);
		testFramework.next();
//--------------Convhelp_hg2mbTest_3 - Did the computation work with a negative value?		
		testFramework.assert(abs(gpstk::hg2mb(-2.9529987508079487) + 100) < eps);

		return testFramework.countFails();
	}

	private:
	double eps;
};


int main() //Main function to initialize and run all tests above
{
	int check = 0, errorCounter = 0;
	Convhelp_T testClass;

	check = testClass.cycles2metersTest();
	errorCounter += check;

	check = testClass.meters2cyclesTest();
	errorCounter += check;

	check = testClass.cel2farTest();
	errorCounter += check;

	check = testClass.far2celTest();
	errorCounter += check;

	check = testClass.mb2hgTest();
	errorCounter += check;

	check = testClass.hg2mbTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
