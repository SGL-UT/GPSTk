#ifndef XRINEXNAV_HPP
#define XRINEXNAV_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavFilterOperators.hpp"
#include "StringUtils.hpp"

using namespace std;

class xRinexNav: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xRinexNav);
	CPPUNIT_TEST (hardCodeTest);
	CPPUNIT_TEST (headerExceptionTest);
	CPPUNIT_TEST (dataTest);
	CPPUNIT_TEST (filterOperatorsTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void);

	protected:
		void hardCodeTest (void);
		void headerExceptionTest (void);
		void dataTest (void);
		void filterOperatorsTest (void);
		bool fileEqualTest (char*, char*);

	private:

};

#endif
