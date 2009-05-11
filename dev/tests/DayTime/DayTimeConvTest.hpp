#pragma ident "$Id$"
// file: DayTimeConvTest.hpp


//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//============================================================================

// This test provides the same functionality as DayTimeConversionTest.cpp
// but uses the CppUnit unit testing framework.

// This test evaluates the user defined comparision tolerance feature of DayTime

#ifndef DAYTIMECONVTEST_HPP
#define DAYTIMECONVTEST_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "StringUtils.hpp"
#include "DayTime.hpp"

using namespace std;
using gpstk::DayTime;

class DayTimeConvTest: public CPPUNIT_NS :: TestFixture
{
	//Test Fixture information, e.g. what tests will be run
	CPPUNIT_TEST_SUITE (DayTimeConvTest);
	CPPUNIT_TEST (constrTest);
	CPPUNIT_TEST (mutatTest);
	CPPUNIT_TEST (randTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void); //Sets up test enviornment
		
	protected:
		
		void constrTest (void); // Test constructors
		void mutatTest (void); // Test mutators
		void randTest (void); // Test frandom dates

		
		
	private:
		
		// Functions that help set up the tests
		gpstk::DayTime** constrSetup(short,short,short,short,short,double,short,
				 double,long,short,double);
		gpstk::DayTime** mutatSetup(short,short,short,short,short,double,short,
				double,short,double,long,short,double);
		
		//Definitions for objects used in the test
		gpstk::DayTime ccase[4];
		gpstk::DayTime mcase[6];
		gpstk::DayTime rcase[6];
		gpstk::DayTime** ccases;
		gpstk::DayTime** mcases;
		gpstk::DayTime** rcases;
};

#endif
