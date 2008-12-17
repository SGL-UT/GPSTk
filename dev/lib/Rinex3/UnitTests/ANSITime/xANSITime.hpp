#ifndef XANSITIME_HPP
#define XANSITIME_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "ANSITime.hpp"

using namespace std;

class xANSITime: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xANSITime);
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
