#pragma ident "$Id$"
#ifndef XMSC_HPP
#define XMSC_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "MSCData.hpp"
#include "MSCStream.hpp"

using namespace std;

class xMSC: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xMSC);
	CPPUNIT_TEST (firstTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void);

	protected:
		void firstTest (void);
		bool fileEqualTest (char*, char*);

	private:

};

#endif
