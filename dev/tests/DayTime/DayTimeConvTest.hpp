#pragma ident "$Id$"
// file: DayTimeConvTest.hpp

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
