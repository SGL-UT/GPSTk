#pragma ident "$Id$"
#ifndef XRINEXOBS_HPP
#define XRINEXOBS_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsFilterOperators.hpp"

using namespace std;

class xRinexObs: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xRinexObs);
	CPPUNIT_TEST (headerExceptionTest);
	CPPUNIT_TEST (hardCodeTest);
	CPPUNIT_TEST (filterOperatorsTest);
	CPPUNIT_TEST (dataExceptionsTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void);

	protected:
		void headerExceptionTest (void);
		void hardCodeTest (void);
		void filterOperatorsTest (void);
		void dataExceptionsTest (void);
		bool fileEqualTest (char*, char*);

	private:

};

#endif
