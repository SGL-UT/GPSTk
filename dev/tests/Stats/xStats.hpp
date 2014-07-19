#pragma ident "$Id$"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

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

