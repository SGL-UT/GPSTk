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

#include "xANSITime.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xANSITime);

using namespace gpstk;

void xANSITime :: setUp (void)
{
}

void xANSITime :: setFromInfoTest (void)
{
	gpstk::ANSITime setFromInfo1;
	gpstk::ANSITime setFromInfo2;
	
	gpstk::TimeTag::IdToValue Id;
	Id.insert(make_pair('K',"13500000"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
	Id.erase('K');
	CPPUNIT_ASSERT(setFromInfo2.setFromInfo(Id));
	ofstream out("Logs/printfOutput");
	
	out << setFromInfo1 << endl;
	out << setFromInfo2 << endl;
}

void xANSITime :: operatorTest (void)
{
	
	gpstk::ANSITime Compare(13500000);
	gpstk::ANSITime LessThan(13400000);
	
	gpstk::ANSITime CompareCopy(Compare);
	
	gpstk::ANSITime CompareCopy2;
	//Assignment
	CompareCopy2 = CompareCopy;
	//Equality Assertion
	CPPUNIT_ASSERT_EQUAL(Compare,CompareCopy);
	//Non-equality Assertion
	CPPUNIT_ASSERT(Compare != LessThan);
	//Less than assertions
	CPPUNIT_ASSERT(LessThan < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThan));
	//Greater than assertions
	CPPUNIT_ASSERT(Compare > LessThan);
	//Less than equals assertion
	CPPUNIT_ASSERT(LessThan <= Compare);
	CPPUNIT_ASSERT(CompareCopy <= Compare);
	//Greater than equals assertion
	CPPUNIT_ASSERT(Compare >= LessThan);
	CPPUNIT_ASSERT(Compare >= CompareCopy);
	
	CPPUNIT_ASSERT(Compare.isValid());
}

