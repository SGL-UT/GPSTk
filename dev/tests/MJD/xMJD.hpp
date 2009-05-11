#pragma ident "$Id$"
#ifndef XMJD_HPP
#define XMJD_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "MJD.hpp"

using namespace std;

class xMJD: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xMJD);
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
