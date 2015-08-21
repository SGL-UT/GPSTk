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
#include <cmath>

class Convhelp_T
{
    public: 
	Convhelp_T(){ eps = 1E-15;}// Default Constructor, set the precision value
	~Convhelp_T() {} // Default Desructor

	int cycles2metersTest(void) 
	{
		TestUtil testFramework( "convhelp", "cycles2meters", __FILE__, __LINE__ );
		std::string failMesg;

		gpstk::WGS84Ellipsoid wem;
		gpstk::GPSEllipsoid gem;

		failMesg = "Did the computation work with positive values?";
		testFramework.assert(std::abs(gpstk::cycles2meters(100000.,400000.,wem) - 74948114.5) < eps, failMesg, __LINE__);
		testFramework.assert(std::abs(gpstk::cycles2meters(10000,40000,gem) - 1.3342563807926082) < eps, failMesg, __LINE__);
		failMesg = "Did the computation work with a 0 value?";
		testFramework.assert(gpstk::cycles2meters(0,5,wem) == 0, failMesg, __LINE__);
		testFramework.assert(gpstk::cycles2meters(0,5,gem) == 0, failMesg, __LINE__);
		failMesg = "Did the computation work with a negative value?";
		testFramework.assert(std::abs(gpstk::cycles2meters(-100000.,400000.,wem) + 74948114.5) < eps, failMesg, __LINE__);
		testFramework.assert(std::abs(gpstk::cycles2meters(-10000,40000,gem) + 1.3342563807926082) < eps, failMesg, __LINE__);

		return testFramework.countFails();
	}

	int meters2cyclesTest(void)
	{
		TestUtil testFramework( "convhelp", "meters2cycles", __FILE__, __LINE__ );
		std::string failMesg;

		gpstk::WGS84Ellipsoid wem;
		gpstk::GPSEllipsoid gem;

		failMesg = "Did the computation work with positive values?";
		testFramework.assert(std::abs(gpstk::meters2cycles(74948114.5,400000.,wem) - 100000) < eps, failMesg, __LINE__);
		failMesg = "Did the above computation work with the GPS ellipsoid?";		
		testFramework.assert(std::abs(gpstk::meters2cycles(10000,40000,gem) - 1.3342563807926082 ) < eps, failMesg, __LINE__);
		failMesg = "Did the computation work with a 0 value?";
		testFramework.assert(gpstk::meters2cycles(0,5,wem) == 0, failMesg, __LINE__);
		failMesg = "Did the above computation work with the GPS ellipsoid?";		
		testFramework.assert(gpstk::meters2cycles(0,5,gem) == 0, failMesg, __LINE__);
		failMesg = "Did the computation work with a negative value?";
		testFramework.assert(std::abs(gpstk::meters2cycles(-74948114.5,400000.,wem) + 100000) < eps, failMesg, __LINE__);
		failMesg = "Did the above computation work with the GPS ellipsoid?";
		testFramework.assert(std::abs(gpstk::meters2cycles(-74948114.5,400000.,wem) + 100000) < eps, failMesg, __LINE__);

		return testFramework.countFails();
	}

	int cel2farTest(void)
	{
		TestUtil testFramework( "convhelp", "cel2far", __FILE__, __LINE__ );
		std::string failMesg;

		failMesg = "Did the computation work with a postitive value?";
		testFramework.assert(gpstk::cel2far(100) == 212, failMesg, __LINE__);
		failMesg = "Did the computation work with a 0 value?";
		testFramework.assert(gpstk::cel2far(0) == 32, failMesg, __LINE__);
		failMesg = "Did the computation work with a negative value?";
		testFramework.assert(gpstk::cel2far(-100) == -148, failMesg, __LINE__);

		return testFramework.countFails();
	}

	int far2celTest(void)
	{
		TestUtil testFramework( "convhelp", "far2cel", __FILE__, __LINE__ );
		std::string failMesg;

		failMesg = "Did the computation work with a postitive value?";
		testFramework.assert(gpstk::far2cel(212) == 100, failMesg, __LINE__);
		failMesg = "Did the computation work with a 0 value?";
		testFramework.assert(gpstk::far2cel(32) == 0, failMesg, __LINE__);
		failMesg = "Did the computation work with a negative value?";
		testFramework.assert(gpstk::far2cel(-148) == -100, failMesg, __LINE__);

		return testFramework.countFails();
	}

	int mb2hgTest(void)
	{
		TestUtil testFramework( "convhelp", "mb2hg", __FILE__, __LINE__ );
		std::string failMesg;

		failMesg = "Did the computation work with a postitive value?";
		testFramework.assert(std::abs(gpstk::mb2hg(100) - 2.9529987508079487) < eps, failMesg, __LINE__);
		failMesg = "Did the computation work with a 0 value?";		
		testFramework.assert(gpstk::mb2hg(0) == 0, failMesg, __LINE__);
		failMesg = "Did the computation work with a negative value?";		
		testFramework.assert(std::abs(gpstk::mb2hg(-100) + 2.9529987508079487) < eps, failMesg, __LINE__);

		return testFramework.countFails();
	}

	int hg2mbTest(void)
	{
		TestUtil testFramework( "convhelp", "hg2mb", __FILE__, __LINE__ );
		std::string failMesg;

		failMesg = "Did the computation work with a postitive value?";
		testFramework.assert(std::abs(gpstk::hg2mb(2.9529987508079487) - 100) < eps, failMesg, __LINE__);
		failMesg = "Did the computation work with a 0 value?";
		testFramework.assert(gpstk::hg2mb(0) == 0, failMesg, __LINE__);
		failMesg = "Did the computation work with a negative value?";	
		testFramework.assert(std::abs(gpstk::hg2mb(-2.9529987508079487) + 100) < eps, failMesg, __LINE__);

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
