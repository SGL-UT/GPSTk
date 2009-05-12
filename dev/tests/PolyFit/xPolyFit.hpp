#pragma ident "$Id$"
// file: xPolyFit.hpp

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
