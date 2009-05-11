#pragma ident "$Id$"
#ifndef XCOMMONTIME_HPP
#define XCOMMONTIME_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "CommonTime.hpp"

using namespace std;

class xCommonTime: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xCommonTime);
	CPPUNIT_TEST (setTest);
	CPPUNIT_TEST (arithmiticTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void);

	protected:
		void setTest (void);
		void arithmiticTest (void);
		
	private:

};

#endif
