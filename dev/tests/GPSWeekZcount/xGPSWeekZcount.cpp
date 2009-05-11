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

#include "xGPSWeekZcount.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xGPSWeekZcount);

using namespace gpstk;

void xGPSWeekZcount :: setUp (void)
{
}

void xGPSWeekZcount :: setFromInfoTest (void)
{
	gpstk::GPSWeekZcount setFromInfo1;
	gpstk::GPSWeekZcount setFromInfo2;
	gpstk::GPSWeekZcount setFromInfo3;
	
	gpstk::TimeTag::IdToValue Id;
	Id.insert(make_pair('F',"1300"));
	Id.insert(make_pair('z',"13500"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
	Id.erase('z');
        Id.insert(make_pair('w',"3"));
	CPPUNIT_ASSERT(setFromInfo2.setFromInfo(Id));
        Id.erase('F');
	CPPUNIT_ASSERT(setFromInfo3.setFromInfo(Id));
	ofstream out("Logs/printfOutput");
	
	out << setFromInfo1 << endl;
	out << setFromInfo2 << endl;
}

void xGPSWeekZcount :: operatorTest (void)
{
	gpstk::GPSWeekZcount Compare(1300,13500);
	gpstk::GPSWeekZcount LessThanWeek(1299,13500);
	gpstk::GPSWeekZcount LessThanZcount(1300,13400);
	
	gpstk::GPSWeekZcount CompareCopy(Compare);
	
	gpstk::GPSWeekZcount CompareCopy2;
	CompareCopy2 = CompareCopy;
	//Equality Assertion
	CPPUNIT_ASSERT_EQUAL(Compare,CompareCopy);
	//Non-equality Assertion
	CPPUNIT_ASSERT(Compare != LessThanWeek);
	//Less than assertions
	CPPUNIT_ASSERT(LessThanWeek < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanWeek));
	CPPUNIT_ASSERT(LessThanZcount < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanZcount));
	//Greater than assertions
	CPPUNIT_ASSERT(Compare > LessThanWeek);
	//Less than equals assertion
	CPPUNIT_ASSERT(LessThanWeek <= Compare);
	CPPUNIT_ASSERT(CompareCopy <= Compare);
	//Greater than equals assertion
	CPPUNIT_ASSERT(Compare >= LessThanWeek);
	CPPUNIT_ASSERT(Compare >= CompareCopy);
	
	CPPUNIT_ASSERT(Compare.isValid());
}
