#pragma ident "$Id: xMatrix.hpp 1895 2009-05-12 19:34:29Z afarris $"
// file: xMatrix.hpp

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

#ifndef XMATRIX_HPP
#define XMATRIX_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "Matrix.hpp"

using namespace std;

 
class xMatrix: public CPPUNIT_NS :: TestFixture
{
	//Test Fixture information, e.g. what tests will be run
	CPPUNIT_TEST_SUITE (xMatrix);
	CPPUNIT_TEST (sizeTest);
	CPPUNIT_TEST (getTest);
	CPPUNIT_TEST (operatorTest);
	
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void); //Sets up test enviornment
		void tearDown (void) ; //Tears down test environment
	protected:
		void sizeTest (void);
		void getTest (void);
		void operatorTest (void);

	private:
      gpstk::Matrix<double> *a, *b, *c, *d, *e, *f, *g, *h;
		
};	
		
#endif

