// file: xSP3EphemerisStore.hpp

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#ifndef XRINEXEPHEMERISSTORE_HPP
#define XRINEXEPHEMERISSTORE_HPP

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
	CPPUNIT_TEST (SP3Test);
	CPPUNIT_TEST (SP3getXvtTest);
	CPPUNIT_TEST (SP3getInitialTimeTest);
	CPPUNIT_TEST (SP3getFinalTimeTest);
	CPPUNIT_TEST (SP3getPositionTest);
	CPPUNIT_TEST (SP3getVelocityTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void); //Sets up test enviornment

	protected:

		void SP3Test (void);
		void SP3getXvtTest (void);
		void SP3getInitialTimeTest (void);
		void SP3getFinalTimeTest (void);
		void SP3getPositionTest (void);
		void SP3getVelocityTest (void);
		bool fileEqualTest (char[], char[]);

	private:

};
#endif
