#pragma ident "$Id$"
// file: xStats.hpp

#ifndef XSTATS_HPP
#define XSTATS_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "Stats.hpp"

using namespace std;

 
class xStats: public CPPUNIT_NS :: TestFixture
{
	//Test Fixture information, e.g. what tests will be run
	CPPUNIT_TEST_SUITE (xStats);
	CPPUNIT_TEST (addTest);
	CPPUNIT_TEST (resetTest);
	CPPUNIT_TEST (subtractTest);
	CPPUNIT_TEST (loadTest);
	CPPUNIT_TEST (addEqualsTest);
	
	CPPUNIT_TEST (add2Test);
	CPPUNIT_TEST (subtract2Test);
	CPPUNIT_TEST (reset2Test);
	CPPUNIT_TEST (slopeTest);
	CPPUNIT_TEST (interceptTest);
	CPPUNIT_TEST (sigmaSlopeTest);
	CPPUNIT_TEST (correlationTest);
	CPPUNIT_TEST (sigmayxTest);
	CPPUNIT_TEST (addEquals2Test);
	
	CPPUNIT_TEST (medianTest);
	
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void); //Sets up test enviornment
		
		
	protected:
		void addTest (void);
		void resetTest (void);
		void subtractTest (void);
		void loadTest (void);
		void addEqualsTest (void);
		
		void add2Test (void);
		void subtract2Test (void);
		void reset2Test (void);
		void slopeTest (void);
		void interceptTest (void);
		void sigmaSlopeTest (void);
		void correlationTest (void);
		void sigmayxTest (void);
		void addEquals2Test (void);
		
		void medianTest (void);
	private:
		
};	
		
#endif

