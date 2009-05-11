#pragma ident "$Id$"
#ifndef XTIMECONVERTERS_HPP
#define XTIMECONVERTERS_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "TimeConverters.hpp"

using namespace std;

class xTimeConverters: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xTimeConverters);
	CPPUNIT_TEST (convertTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void);

	protected:
		void convertTest (void);

	private:

};

#endif
