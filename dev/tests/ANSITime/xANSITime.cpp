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
	gpstk::ANSITime Compare(13500000,TimeSystem(2));
	gpstk::TimeTag::IdToValue Id;
	Id.insert(make_pair('K',"13500000"));
	Id.insert(make_pair('P',"2"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
	CPPUNIT_ASSERT_EQUAL(setFromInfo1,Compare);
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

void xANSITime :: resetTest (void)
{

  ANSITime Compare(13500000,TimeSystem(2));

  CommonTime Test = Compare.convertToCommonTime();

  ANSITime Test2;
  Test2.convertFromCommonTime(Test);

  CPPUNIT_ASSERT_EQUAL(Test2,Compare);

  CPPUNIT_ASSERT(Compare.getTimeSystem()==TimeSystem(2));
  CPPUNIT_ASSERT_EQUAL(13500000,(int)Compare.time);

  Compare.reset();
  CPPUNIT_ASSERT(TimeSystem(0)==Compare.getTimeSystem());
  CPPUNIT_ASSERT_EQUAL(0,(int)Compare.time);

}

void xANSITime :: timeSystemTest (void)
{

  ANSITime GPS1(13500000,TimeSystem(2));
  ANSITime GPS2(13400000,TimeSystem(2));
  ANSITime UTC1(13500000,TimeSystem(5));
  ANSITime UNKNOWN(13500000,TimeSystem(0));
  ANSITime ANY(13500000,TimeSystem(1));

  CPPUNIT_ASSERT(GPS1 != GPS2);
  CPPUNIT_ASSERT(GPS1.getTimeSystem()==GPS2.getTimeSystem());
  CPPUNIT_ASSERT(GPS1 != UTC1);
  CPPUNIT_ASSERT(GPS1 != UNKNOWN);
  CPPUNIT_ASSERT(GPS1.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);
  CPPUNIT_ASSERT(CommonTime::BEGINNING_OF_TIME < GPS1);
  CPPUNIT_ASSERT_EQUAL(GPS1,ANY);
  CPPUNIT_ASSERT_EQUAL(UTC1,ANY);
  CPPUNIT_ASSERT_EQUAL(UNKNOWN,ANY);
  CPPUNIT_ASSERT(GPS2 != ANY);
  CPPUNIT_ASSERT(GPS2 < GPS1);
  CPPUNIT_ASSERT(GPS2 < ANY);

  UNKNOWN.setTimeSystem(TimeSystem(2));
  CPPUNIT_ASSERT(UNKNOWN.getTimeSystem()==TimeSystem(2));
}

void xANSITime :: printfTest (void)
{

  ANSITime GPS1(13500000,TimeSystem(2));
  ANSITime UTC1(13500000,TimeSystem(5));

  CPPUNIT_ASSERT_EQUAL(GPS1.printf("%08K %02P"),(std::string)"13500000 GPS");
  CPPUNIT_ASSERT_EQUAL(UTC1.printf("%08K %02P"),(std::string)"13500000 UTC");
  CPPUNIT_ASSERT_EQUAL(GPS1.printError("%08K %02P"),(std::string)"ErrorBadTime ErrorBadTime");
  CPPUNIT_ASSERT_EQUAL(UTC1.printError("%08K %02P"),(std::string)"ErrorBadTime ErrorBadTime");

}
