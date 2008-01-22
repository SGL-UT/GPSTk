#ifndef XGPSWEEKZCOUNT_HPP
#define XGPSWEEKZCOUNT_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "GPSWeekZcount.hpp"

using namespace std;

class xGPSWeekZcount: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xGPSWeekZcount);
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
