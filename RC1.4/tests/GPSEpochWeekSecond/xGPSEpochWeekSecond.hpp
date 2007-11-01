#ifndef XGPSEPOCHWEEKSECOND_HPP
#define XGPSEPOCHWEEKSECOND_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "GPSEpochWeekSecond.hpp"

using namespace std;

class xGPSEpochWeekSecond: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xGPSEpochWeekSecond);
	CPPUNIT_TEST (setFromInfoTest);
	CPPUNIT_TEST (operatorTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void);

	protected:
		void operatorTest (void);
		void setFromInfoTest (void);

	private:

};

#endif
