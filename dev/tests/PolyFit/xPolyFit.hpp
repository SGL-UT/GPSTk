#pragma ident "$Id$"
// file: xPolyFit.hpp

#ifndef XPOLYFIT_HPP
#define XPOLYFIT_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "PolyFit.hpp"


using namespace std;

 
class xPolyFit: public CPPUNIT_NS :: TestFixture
{
	//Test Fixture information, e.g. what tests will be run
	CPPUNIT_TEST_SUITE (xPolyFit);
	CPPUNIT_TEST (constrTest);
	CPPUNIT_TEST (addTest);
	CPPUNIT_TEST (resetTest);
	CPPUNIT_TEST (evalTestSingle);
	CPPUNIT_TEST (evalTestVector);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void); //Sets up test enviornment
		
	protected:
		
		void constrTest (void);
		void addTest (void);
		void resetTest (void);
		void evalTestSingle (void); 
		void evalTestVector (void);
		
		
	private:
	
};		
		
#endif
