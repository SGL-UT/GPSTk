// file: xBinUtils.hpp

#ifndef XBINUTILS_HPP
#define XBINUTILS_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "BinUtils.hpp"

using namespace std;

 
class xBinUtils: public CPPUNIT_NS :: TestFixture
{
	//Test Fixture information, e.g. what tests will be run
	CPPUNIT_TEST_SUITE (xBinUtils);
	CPPUNIT_TEST (firstTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void); //Sets up test enviornment
		
	protected:
		
		void firstTest (void); // Test constructors
		
	private:
};		
		
#endif
