// file: xRungeKutta4.hpp

#ifndef XRUNGEKUTTA4_HPP
#define XRUNGEKUTTA4_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>


using namespace std;

 
class xRungeKutta4: public CPPUNIT_NS :: TestFixture
{
	//Test Fixture information, e.g. what tests will be run
	CPPUNIT_TEST_SUITE (xRungeKutta4);
	CPPUNIT_TEST (quarterTest);
	CPPUNIT_TEST (halfTest);
	CPPUNIT_TEST (fullTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void); //Sets up test enviornment
		
	protected:
		
		void quarterTest (void); // Test constructors
		void halfTest (void);
		void fullTest (void);

		
	private:		
	
};
#endif
