#ifndef XGPSWEEKSECOND_HPP
#define XGPSWEEKSECOND_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "GPSWeekSecond.hpp"

using namespace std;

class xGPSWeekSecond: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xGPSWeekSecond);
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
