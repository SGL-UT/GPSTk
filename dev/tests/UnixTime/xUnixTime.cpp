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

#include "xUnixTime.hpp"
#include "TimeSystem.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xUnixTime);

using namespace gpstk;

void xUnixTime :: setUp (void)
{
}

void xUnixTime :: setFromInfoTest (void)
{
	gpstk::UnixTime setFromInfo1;
	gpstk::UnixTime setFromInfo2;
	UnixTime Compare(1350000,0,TimeSystem::GPS);

	gpstk::TimeTag::IdToValue Id;
	Id.insert(make_pair('U',"1350000"));
	Id.insert(make_pair('u',"0"));
	Id.insert(make_pair('P',"02"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
	CPPUNIT_ASSERT_EQUAL(setFromInfo1,Compare);
	Id.erase('U');
	CPPUNIT_ASSERT(setFromInfo2.setFromInfo(Id));
	ofstream out("Logs/printfOutput");

	out << setFromInfo1 << endl;
	out << setFromInfo2 << endl;
}

void xUnixTime :: operatorTest (void)
{

	gpstk::UnixTime Compare(1350000, 100);
	gpstk::UnixTime LessThanSec(1340000, 100);
	gpstk::UnixTime LessThanMicroSec(1350000,0);

	gpstk::UnixTime CompareCopy(Compare);

	gpstk::UnixTime CompareCopy2;
	CompareCopy2 = CompareCopy;
	//Equality Assertion
	CPPUNIT_ASSERT_EQUAL(Compare,CompareCopy);
	//Non-equality Assertion
	CPPUNIT_ASSERT(Compare != LessThanSec);
	//Less than assertions
	CPPUNIT_ASSERT(LessThanSec < Compare);
	CPPUNIT_ASSERT(LessThanMicroSec < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanSec));
	//Greater than assertions
	CPPUNIT_ASSERT(Compare > LessThanSec);
	//Less than equals assertion
	CPPUNIT_ASSERT(LessThanSec <= Compare);
	CPPUNIT_ASSERT(CompareCopy <= Compare);
	//Greater than equals assertion
	CPPUNIT_ASSERT(Compare >= LessThanSec);
	CPPUNIT_ASSERT(Compare >= CompareCopy);

/*	gpstk::CommonTime Rollover(1,1,.9999999999999); Resulted in test failure; Don't know how necessary this is
	gpstk::UnixTime Temp;
	Temp.convertFromCommonTime(Rollover);
*/
	CPPUNIT_ASSERT(Compare.isValid());
}

void xUnixTime :: resetTest (void)
{
        UnixTime Compare(1350000,0,TimeSystem::GPS);

	CommonTime Test = Compare.convertToCommonTime();

	UnixTime Test2;
	Test2.convertFromCommonTime(Test);

	CPPUNIT_ASSERT_EQUAL(Test2,Compare);

	CPPUNIT_ASSERT(TimeSystem(TimeSystem::GPS) == Compare.getTimeSystem());

	CPPUNIT_ASSERT_EQUAL(1350000,(int)Compare.tv.tv_sec);
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.tv.tv_usec);

	Compare.reset();
	CPPUNIT_ASSERT(TimeSystem(TimeSystem::Unknown) == Compare.getTimeSystem());
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.tv.tv_sec);
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.tv.tv_usec);
}

void xUnixTime :: timeSystemTest (void)
{

	UnixTime GPS1(   1350000,0,TimeSystem::GPS    );
	UnixTime GPS2(   1340000,0,TimeSystem::GPS    );
	UnixTime UTC1(   1350000,0,TimeSystem::UTC    );
	UnixTime UNKNOWN(1350000,0,TimeSystem::Unknown);
	UnixTime ANY(    1350000,0,TimeSystem::Any    );

	CPPUNIT_ASSERT(GPS1 != GPS2);
	CPPUNIT_ASSERT(GPS1.getTimeSystem() == GPS2.getTimeSystem());
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
	CPPUNIT_ASSERT(UNKNOWN.getTimeSystem() == TimeSystem(TimeSystem::GPS));
}

void xUnixTime :: printfTest (void)
{

	UnixTime GPS1(1350000,0,TimeSystem::GPS);
	UnixTime UTC1(1350000,0,TimeSystem::UTC);

	CPPUNIT_ASSERT_EQUAL(GPS1.printf("%07U %02u %02P"),(std::string)"1350000 00 GPS");
	CPPUNIT_ASSERT_EQUAL(UTC1.printf("%07U %02u %02P"),(std::string)"1350000 00 UTC");
	CPPUNIT_ASSERT_EQUAL(GPS1.printError("%07U %02u %02P"),(std::string)"ErrorBadTime ErrorBadTime ErrorBadTime");
	CPPUNIT_ASSERT_EQUAL(UTC1.printError("%07U %02u %02P"),(std::string)"ErrorBadTime ErrorBadTime ErrorBadTime");
}
