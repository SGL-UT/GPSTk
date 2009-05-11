#pragma ident "$Id$"

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

#ifndef XDAYTIME_HPP
#define XDAYTIME_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "DayTime.hpp"

using namespace std;

class xDayTime: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xDayTime);
	CPPUNIT_TEST (constrTest);
	CPPUNIT_TEST (arithmeticTest);
	CPPUNIT_TEST (comparisonTest);
	CPPUNIT_TEST (setTest);
	CPPUNIT_TEST (stringTest);
	CPPUNIT_TEST (exceptionTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void);

	protected:
		void constrTest (void);
		void arithmeticTest (void);
		void comparisonTest (void);
		void setTest (void);
		void stringTest (void);
		void exceptionTest (void);

	private:

};

#endif
