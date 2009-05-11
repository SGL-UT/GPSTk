#pragma ident "$Id$"
#ifndef XRACROTATION_HPP
#define XRACROTATION_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "RACRotation.hpp"

using namespace std;

class xRACRotation: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xRACRotation);
	CPPUNIT_TEST (firstTest);
	CPPUNIT_TEST (secondTest);
	CPPUNIT_TEST (thirdTest);
	CPPUNIT_TEST (fourthTest);
	CPPUNIT_TEST (fifthTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void);

	protected:
		void firstTest (void);
		void secondTest (void);
		void thirdTest (void);
		void fourthTest (void);
		void fifthTest (void);

	private:
		double GPSAlt;   // 26 million meters
		gpstk::Triple testErrXYZ, testErrRAC;
};

#endif
