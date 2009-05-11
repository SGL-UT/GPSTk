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

#ifndef XRINEXNAV_HPP
#define XRINEXNAV_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavFilterOperators.hpp"
#include "StringUtils.hpp"

using namespace std;

class xRinexNav: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xRinexNav);
	CPPUNIT_TEST (hardCodeTest);
	CPPUNIT_TEST (headerExceptionTest);
	CPPUNIT_TEST (dataTest);
	CPPUNIT_TEST (filterOperatorsTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void);

	protected:
		void hardCodeTest (void);
		void headerExceptionTest (void);
		void dataTest (void);
		void filterOperatorsTest (void);
		bool fileEqualTest (char*, char*);

	private:

};

#endif
