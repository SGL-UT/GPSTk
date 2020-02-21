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

#include "xCivilTime.hpp"
//#include "GPSWeekZcount.hpp"
//#include "GPSWeekSecond.hpp"
//#include "GPSWeek.hpp"
//#include "YDSTime.hpp"
#include "TimeSystem.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xCivilTime);

using namespace gpstk;

void xCivilTime :: setUp (void)
{
}

void xCivilTime :: setFromInfoTest (void)
{
	gpstk::CivilTime setFromInfo1;
	gpstk::CivilTime setFromInfo2;
	gpstk::CivilTime setFromInfo3;
	gpstk::CivilTime setFromInfo4;
	gpstk::CivilTime setFromInfo5;

	gpstk::TimeTag::IdToValue Id;
	Id.insert(make_pair('b',"Dec"));
	Id.insert(make_pair('d',"31"));
	Id.insert(make_pair('Y',"2008"));
	Id.insert(make_pair('H',"12"));
	Id.insert(make_pair('M',"00"));
	Id.insert(make_pair('S',"00"));
	Id.insert(make_pair('P',"02"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));

    CivilTime Check(2008,12,31,12,0,0,TimeSystem::GPS);
    CPPUNIT_ASSERT_EQUAL(setFromInfo1,Check);
	Id.erase('b');
	Id.erase('Y');
	Id.insert(make_pair('m',"12"));
	Id.insert(make_pair('y',"06"));
	CPPUNIT_ASSERT(setFromInfo2.setFromInfo(Id));
	Id.erase('y');
	Id.insert(make_pair('y',"006"));
	CPPUNIT_ASSERT(setFromInfo3.setFromInfo(Id));
	Id.erase('y');
	CPPUNIT_ASSERT(setFromInfo4.setFromInfo(Id));
	Id.erase('m');
	Id.insert(make_pair('b',"AAA"));
	CPPUNIT_ASSERT(!(setFromInfo5.setFromInfo(Id)));
	ofstream out("Logs/printfOutput");

	out << setFromInfo1 << endl;
	out << setFromInfo2 << endl;
	out << setFromInfo3 << endl;
	out << setFromInfo4 << endl;

	CommonTime time;
    time = Check.convertToCommonTime();
    std::cout << Check.printf("%02P") << std::endl;
    std::cout << Check.printf("%04Y %02m %02d %02H %02M") << std::endl;
    std::cout << ((CivilTime)time).printf("%04Y %02m %02d %02H %02M")<< std::endl;
//  std::cout << ((YDSTime)(time)).printf("%10Y")<< std::endl;
//  std::cout << ((GPSWeekSecond)(time)).printf("%02F")<< std::endl;

//  GPSWeekSecond time2;
//  std::cout << time2.printf("%02w") << std::endl;
//  time = time2.convertToCommonTime();
//  std::cout << ((GPSWeekSecond)(time)).printf("%02F")<< std::endl;
}

void xCivilTime :: operatorTest (void)
{
	gpstk::CivilTime Zero;

	gpstk::CivilTime Aug21(2008,8,21,13,30,15.);
	gpstk::CivilTime LessThanYear(2005,8,21,13,30,15.);
	gpstk::CivilTime LessThanMonth(2008,7,21,13,30,15.);
	gpstk::CivilTime LessThanDay(2008,8,20,13,30,15.);
	gpstk::CivilTime LessThanHour(2008,8,21,12,30,15.);
	gpstk::CivilTime LessThanMinute(2008,8,21,13,20,15.);
	gpstk::CivilTime LessThanSecond(2008,8,21,13,30,0.);

	gpstk::CivilTime Aug21Copy(Aug21);

	gpstk::CivilTime Aug21Copy2 = Aug21Copy;
	Aug21Copy2 = Aug21Copy;
	//Equality Assertion
	CPPUNIT_ASSERT_EQUAL(Aug21,Aug21Copy);
	//Non-equality Assertion
	CPPUNIT_ASSERT(Aug21 != LessThanYear);
	//Less than assertions
	CPPUNIT_ASSERT(LessThanYear < Aug21);
	CPPUNIT_ASSERT(!(Aug21 < LessThanYear));
	CPPUNIT_ASSERT(LessThanMonth < Aug21);
	CPPUNIT_ASSERT(!(Aug21 < LessThanMonth));
	CPPUNIT_ASSERT(LessThanDay < Aug21);
	CPPUNIT_ASSERT(!(Aug21 < LessThanDay));
	CPPUNIT_ASSERT(LessThanHour < Aug21);
	CPPUNIT_ASSERT(!(Aug21 < LessThanHour));
	CPPUNIT_ASSERT(LessThanMinute < Aug21);
	CPPUNIT_ASSERT(!(Aug21 < LessThanMinute));
	CPPUNIT_ASSERT(LessThanSecond < Aug21);
	CPPUNIT_ASSERT(!(Aug21 < LessThanSecond));
	//Greater than assertions
	CPPUNIT_ASSERT(Aug21 > LessThanYear);
	//Less than equals assertion
	CPPUNIT_ASSERT(LessThanYear <= Aug21);
	CPPUNIT_ASSERT(Aug21Copy <= Aug21);
	//Greater than equals assertion
	CPPUNIT_ASSERT(Aug21 >= LessThanYear);
	CPPUNIT_ASSERT(Aug21 >= Aug21Copy);

	CPPUNIT_ASSERT(Aug21.isValid());
	CPPUNIT_ASSERT(!Zero.isValid());
}

void xCivilTime :: resetTest (void)
{
  CivilTime Aug21(2008,8,21,13,30,15.,TimeSystem::GPS);

  CommonTime Test = Aug21.convertToCommonTime();

  CivilTime Test2;
  Test2.convertFromCommonTime(Test);

  CPPUNIT_ASSERT_EQUAL(Test2,Aug21);
  CPPUNIT_ASSERT(TimeSystem(2)==Aug21.getTimeSystem());

  Aug21.reset();
  CPPUNIT_ASSERT(TimeSystem(0)==Aug21.getTimeSystem());
  CPPUNIT_ASSERT_EQUAL(0,(int)Aug21.year);
  CPPUNIT_ASSERT_EQUAL(1,(int)Aug21.month);
  CPPUNIT_ASSERT_EQUAL(1,(int)Aug21.day);
  CPPUNIT_ASSERT_EQUAL(0,(int)Aug21.hour);
  CPPUNIT_ASSERT_EQUAL(0,(int)Aug21.minute);
  CPPUNIT_ASSERT_EQUAL(0,(int)Aug21.second);
}

void xCivilTime :: timeSystemTest (void)
{
  CivilTime GPS1(   2008,8,21,13,30,15.,TimeSystem::GPS);
  CivilTime GPS2(   2005,8,21,13,30,15.,TimeSystem::GPS);
  CivilTime UTC1(   2008,8,21,13,30,15.,TimeSystem::UTC);
  CivilTime UNKNOWN(2008,8,21,13,30,15.,TimeSystem::Unknown);
  CivilTime ANY(    2008,8,21,13,30,15.,TimeSystem::Any);

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

  UNKNOWN.setTimeSystem(TimeSystem::GPS);
  CPPUNIT_ASSERT(UNKNOWN.getTimeSystem()==TimeSystem(2));
}

void xCivilTime :: printfTest (void)
{
  CivilTime GPS1(2008,8,21,13,30,15.,TimeSystem::GPS);
  CivilTime UTC1(2008,8,21,13,30,15.,TimeSystem::UTC);

  CPPUNIT_ASSERT_EQUAL(GPS1.printf("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P"),
                       (std::string)"2008 08 08 Aug 21 13 30 15 15.000000 GPS");
  CPPUNIT_ASSERT_EQUAL(UTC1.printf("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P"),
                       (std::string)"2008 08 08 Aug 21 13 30 15 15.000000 UTC");
  CPPUNIT_ASSERT_EQUAL(GPS1.printError("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P"),
                       (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime");
  CPPUNIT_ASSERT_EQUAL(UTC1.printError("%04Y %02y %02m %02b %02d %02H %02M %02S %02f %02P"),
                       (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime");
}
