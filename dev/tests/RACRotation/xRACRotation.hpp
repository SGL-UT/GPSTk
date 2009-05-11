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

#ifndef XRACROTATION_HPP
#define XRACROTATION_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "RACRotation.hpp"

using namespace std;

class xRACRotation: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xRACRotation);
	CPPUNIT_TEST (firstTest);
	CPPUNIT_TEST (secondTest);
	CPPUNIT_TEST (thirdTest);
	CPPUNIT_TEST (fourthTest);
	CPPUNIT_TEST (fifthTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void);

	protected:
		void firstTest (void);
		void secondTest (void);
		void thirdTest (void);
		void fourthTest (void);
		void fifthTest (void);

	private:
		double GPSAlt;   // 26 million meters
		gpstk::Triple testErrXYZ, testErrRAC;
};

#endif
