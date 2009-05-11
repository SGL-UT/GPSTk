#pragma ident "$Id$"
// file: xRungeKutta4.hpp

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
//============================================================================

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
