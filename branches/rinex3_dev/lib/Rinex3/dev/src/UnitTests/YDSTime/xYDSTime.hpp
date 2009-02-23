#ifndef XYDSTIME_HPP
#define XYDSTIME_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "YDSTime.hpp"

using namespace std;

class xYDSTime: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xYDSTime);
	CPPUNIT_TEST (operatorTest);
        CPPUNIT_TEST (resetTest);
	CPPUNIT_TEST (timeSystemTest);
	CPPUNIT_TEST (printfTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void);

	protected:
		void operatorTest (void);
		void setFromInfoTest (void);
		void resetTest (void);
		void timeSystemTest (void);
		void printfTest (void);
	private:

};

#endif
