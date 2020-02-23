#pragma ident "$Id$"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

#include "xGPSWeekSecond.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xGPSWeekSecond);

using namespace gpstk;

void xGPSWeekSecond :: setUp (void)
{
}

void xGPSWeekSecond :: setFromInfoTest (void)
{
	gpstk::GPSWeekSecond setFromInfo1;
	gpstk::GPSWeekSecond setFromInfo2;
	
	gpstk::TimeTag::IdToValue Id;
	Id.insert(make_pair('F',"1300"));
	Id.insert(make_pair('g',"13500"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
	Id.erase('F');
	CPPUNIT_ASSERT(setFromInfo2.setFromInfo(Id));
	ofstream out("Logs/printfOutput");
	
	out << setFromInfo1 << endl;
	out << setFromInfo2 << endl;
}

void xGPSWeekSecond :: operatorTest (void)
{
	gpstk::GPSWeekSecond Zero;
	
	gpstk::GPSWeekSecond Compare(1300,13500.);
	gpstk::GPSWeekSecond LessThanWeek(1299,13500.);
	gpstk::GPSWeekSecond LessThanSecond(1300,13400.);
	
	gpstk::GPSWeekSecond CompareCopy(Compare);
	
	gpstk::GPSWeekSecond CompareCopy2;
	CompareCopy2 = CompareCopy;
	//Equality Assertion
	CPPUNIT_ASSERT_EQUAL(Compare,CompareCopy);
	//Non-equality Assertion
	CPPUNIT_ASSERT(Compare != LessThanWeek);
	//Less than assertions
	CPPUNIT_ASSERT(LessThanWeek < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanWeek));
	CPPUNIT_ASSERT(LessThanSecond < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanSecond));
	//Greater than assertions
	CPPUNIT_ASSERT(Compare > LessThanWeek);
	//Less than equals assertion
	CPPUNIT_ASSERT(LessThanWeek <= Compare);
	CPPUNIT_ASSERT(CompareCopy <= Compare);
	//Greater than equals assertion
	CPPUNIT_ASSERT(Compare >= LessThanWeek);
	CPPUNIT_ASSERT(Compare >= CompareCopy);
	
	CPPUNIT_ASSERT(Compare.isValid());
	//CPPUNIT_ASSERT(!Zero.isValid());
}
