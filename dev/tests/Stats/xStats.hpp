#pragma ident "$Id$"
// file: xStats.hpp

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#ifndef XSTATS_HPP
#define XSTATS_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "Stats.hpp"

using namespace std;

 
class xStats: public CPPUNIT_NS :: TestFixture
{
	//Test Fixture information, e.g. what tests will be run
	CPPUNIT_TEST_SUITE (xStats);
	CPPUNIT_TEST (addTest);
	CPPUNIT_TEST (resetTest);
	CPPUNIT_TEST (subtractTest);
	CPPUNIT_TEST (loadTest);
	CPPUNIT_TEST (addEqualsTest);
	
	CPPUNIT_TEST (add2Test);
	CPPUNIT_TEST (subtract2Test);
	CPPUNIT_TEST (reset2Test);
	CPPUNIT_TEST (slopeTest);
	CPPUNIT_TEST (interceptTest);
	CPPUNIT_TEST (sigmaSlopeTest);
	CPPUNIT_TEST (correlationTest);
	CPPUNIT_TEST (sigmayxTest);
	CPPUNIT_TEST (addEquals2Test);
	
	CPPUNIT_TEST (medianTest);
	
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void); //Sets up test enviornment
		
		
	protected:
		void addTest (void);
		void resetTest (void);
		void subtractTest (void);
		void loadTest (void);
		void addEqualsTest (void);
		
		void add2Test (void);
		void subtract2Test (void);
		void reset2Test (void);
		void slopeTest (void);
		void interceptTest (void);
		void sigmaSlopeTest (void);
		void correlationTest (void);
		void sigmayxTest (void);
		void addEquals2Test (void);
		
		void medianTest (void);
	private:
		
};	
		
#endif

