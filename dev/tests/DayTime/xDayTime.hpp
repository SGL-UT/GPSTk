#pragma ident "$Id$"
#ifndef XDAYTIME_HPP
#define XDAYTIME_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "DayTime.hpp"

using namespace std;

class xDayTime: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xDayTime);
	CPPUNIT_TEST (constrTest);
	CPPUNIT_TEST (arithmeticTest);
	CPPUNIT_TEST (comparisonTest);
	CPPUNIT_TEST (setTest);
	CPPUNIT_TEST (stringTest);
	CPPUNIT_TEST (exceptionTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void);

	protected:
		void constrTest (void);
		void arithmeticTest (void);
		void comparisonTest (void);
		void setTest (void);
		void stringTest (void);
		void exceptionTest (void);

	private:

};

#endif
