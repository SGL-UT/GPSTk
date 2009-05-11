#pragma ident "$Id$"
#ifndef XRINEXMET_HPP
#define XRINEXMET_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "RinexMetBase.hpp"
#include "RinexMetData.hpp"
#include "RinexMetHeader.hpp"
#include "RinexMetFilterOperators.hpp"
#include "RinexMetStream.hpp"
#include "Exception.hpp"
#include <fstream>

/*
**** This test covers the RinexMet*.[ch]pp files for Rinex I/O Manipulation
*/

using namespace std;

class xRinexMet: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xRinexMet);
        CPPUNIT_TEST (bitsAsStringTest);
	CPPUNIT_TEST (bitStringTest);
	CPPUNIT_TEST (reallyPutRecordTest);
	CPPUNIT_TEST (reallyGetRecordTest);
	CPPUNIT_TEST (convertObsTypeSTRTest);
	CPPUNIT_TEST (convertObsTypeHeaderTest);
	CPPUNIT_TEST (hardCodeTest);
	CPPUNIT_TEST (continuationTest);
	CPPUNIT_TEST (dataExceptionsTest);
	CPPUNIT_TEST (filterOperatorsTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void);

	protected:
		void bitsAsStringTest (void);
		void bitStringTest (void);
		void reallyPutRecordTest (void);
		void reallyGetRecordTest (void);
		void convertObsTypeSTRTest (void);
		void convertObsTypeHeaderTest (void);
		void hardCodeTest (void);
		void continuationTest (void);
		void dataExceptionsTest (void);
		void filterOperatorsTest (void);
		bool fileEqualTest (char* handle1, char* handle2);
	private:

};

#endif
