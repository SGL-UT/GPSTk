#pragma ident "$Id$"
// file: 

#ifndef XIONOMODEL_HPP
#define XIONOMODEL_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "IonoModel.hpp"

using namespace std;

 
class xIonoModel: public CPPUNIT_NS :: TestFixture
{
	//Test Fixture information, e.g. what tests will be run
	CPPUNIT_TEST_SUITE (xIonoModel);
	CPPUNIT_TEST (equalityTest);
	CPPUNIT_TEST (nonEqualityTest);
	CPPUNIT_TEST (validTest);
	CPPUNIT_TEST (exceptionTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void); //Sets up test enviornment
		
	protected:
		
		void equalityTest (void);
		void nonEqualityTest (void);
		void validTest (void);
		void exceptionTest (void);

	private:		
		
};
#endif
