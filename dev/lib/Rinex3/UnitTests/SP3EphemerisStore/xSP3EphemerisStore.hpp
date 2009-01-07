// file: xSP3EphemerisStore.hpp

#ifndef XSP3EPHEMERISSTORE_HPP
#define XSP3EPHEMERISSTORE_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "SP3EphemerisStore.hpp"
#include "SatID.hpp"
#include <fstream>

using namespace std;
 
class xSP3EphemerisStore: public CPPUNIT_NS :: TestFixture
{
	//Test Fixture information, e.g. what tests will be run
	CPPUNIT_TEST_SUITE (xSP3EphemerisStore);
	CPPUNIT_TEST (RESTest);
	CPPUNIT_TEST (getXvtTest);
	CPPUNIT_TEST (dumpTest);
	CPPUNIT_TEST (addEphemerisTest);
	CPPUNIT_TEST (editTest);
	CPPUNIT_TEST (clearTest);
	CPPUNIT_TEST (dumpBadPositionsTest);   
	CPPUNIT_TEST (dumpBadClocksTest); 
	CPPUNIT_TEST (maxIntervalTest);
	CPPUNIT_TEST (getTimeTest);    
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void); //Sets up test enviornment
		

	protected:
		
		void RESTest (void); 
		void getXvtTest (void);
		void dumpTest (void);
		void addEphemerisTest (void);
		void editTest (void);
		void clearTest (void);
		void dumpBadPositionsTest (void);
		void dumpBadClocksTest (void);
		void maxIntervalTest (void);
		void getTimeTest (void);
		bool fileEqualTest (char[], char[]);

	private:		

};	
#endif
