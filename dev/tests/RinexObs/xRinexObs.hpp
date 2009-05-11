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

#ifndef XRINEXOBS_HPP
#define XRINEXOBS_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsFilterOperators.hpp"

using namespace std;

class xRinexObs: public CPPUNIT_NS :: TestFixture
{
	CPPUNIT_TEST_SUITE (xRinexObs);
	CPPUNIT_TEST (headerExceptionTest);
	CPPUNIT_TEST (hardCodeTest);
	CPPUNIT_TEST (filterOperatorsTest);
	CPPUNIT_TEST (dataExceptionsTest);
	CPPUNIT_TEST_SUITE_END ();

	public:
		void setUp (void);

	protected:
		void headerExceptionTest (void);
		void hardCodeTest (void);
		void filterOperatorsTest (void);
		void dataExceptionsTest (void);
		bool fileEqualTest (char*, char*);

	private:

};

#endif
