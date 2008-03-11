#ifndef XCIVILTIME_HPP
#define XCIVILTIME_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "CivilTime.hpp"

using namespace std;

class xCivilTime: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xCivilTime);
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
