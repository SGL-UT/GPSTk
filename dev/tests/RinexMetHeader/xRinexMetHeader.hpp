// CppUnit-Tutorial
// file: fractiontest.h
#ifndef RMETHEADERTEST_HPP
#define RMETHEADERTEST_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "RinexMetHeader.hpp"
#include "Exception.hpp"
#include <fstream>

using namespace std;

class rMetHeaderTest : public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (rMetHeaderTest);
        CPPUNIT_TEST (bitsAsStringTest);
	CPPUNIT_TEST (bitStringTest);
	CPPUNIT_TEST (reallyPutRecordTest);
	CPPUNIT_TEST (reallyGetRecordTest);
	CPPUNIT_TEST (dumpTest);
	CPPUNIT_TEST (convertObsTypeSTRTest);
	CPPUNIT_TEST (convertObsTypeHeaderTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void);
		void tearDown (void);

	protected:
  	void bitsAsStringTest (void);
	void bitStringTest (void);
	void reallyPutRecordTest (void);
	void reallyGetRecordTest (void);
	void dumpTest (void);
	void convertObsTypeSTRTest (void);
	void convertObsTypeHeaderTest (void);

	private:

};

#endif
