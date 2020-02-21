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

#include "xJulianDate.hpp"
#include "TimeSystem.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xJulianDate);

using namespace gpstk;

void xJulianDate :: setUp (void)
{
}

void xJulianDate :: setFromInfoTest (void)
{
	gpstk::JulianDate setFromInfo1;
	gpstk::JulianDate setFromInfo2;
    JulianDate Compare(1350000,TimeSystem::GPS);

	gpstk::TimeTag::IdToValue Id;
	Id.insert(make_pair('J',"1350000"));
	Id.insert(make_pair('P',"02"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
	CPPUNIT_ASSERT_EQUAL(setFromInfo1,Compare);
	Id.erase('J');
	CPPUNIT_ASSERT(setFromInfo2.setFromInfo(Id));
	ofstream out("Logs/printfOutput");

	out << setFromInfo1 << endl;
	out << setFromInfo2 << endl;
}

void xJulianDate :: operatorTest (void)
{

	gpstk::JulianDate Compare(1350000);
	gpstk::JulianDate LessThanJD(1340000);

	gpstk::JulianDate CompareCopy(Compare);

	gpstk::JulianDate CompareCopy2;
	CompareCopy2 = CompareCopy;
	//Equality Assertion
	CPPUNIT_ASSERT_EQUAL(Compare,CompareCopy);
	//Non-equality Assertion
	CPPUNIT_ASSERT(Compare != LessThanJD);
	//Less than assertions
	CPPUNIT_ASSERT(LessThanJD < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanJD));
	//Greater than assertions
	CPPUNIT_ASSERT(Compare > LessThanJD);
	//Less than equals assertion
	CPPUNIT_ASSERT(LessThanJD <= Compare);
	CPPUNIT_ASSERT(CompareCopy <= Compare);
	//Greater than equals assertion
	CPPUNIT_ASSERT(Compare >= LessThanJD);
	CPPUNIT_ASSERT(Compare >= CompareCopy);

	CPPUNIT_ASSERT(Compare.isValid());
}

void xJulianDate :: resetTest (void)
{
  JulianDate Compare(1350000,TimeSystem::GPS);

  CommonTime Test = Compare.convertToCommonTime();

  JulianDate Test2;
  Test2.convertFromCommonTime(Test);

  CPPUNIT_ASSERT_EQUAL(Test2,Compare);

  CPPUNIT_ASSERT(TimeSystem(TimeSystem::GPS) == Compare.getTimeSystem());
  CPPUNIT_ASSERT_EQUAL(1350000,(int)Compare.jd);

  Compare.reset();
  CPPUNIT_ASSERT(TimeSystem(TimeSystem::Unknown) == Compare.getTimeSystem());
  CPPUNIT_ASSERT_EQUAL(0,(int)Compare.jd);
}

void xJulianDate :: timeSystemTest (void)
{

	JulianDate GPS1(1350000,TimeSystem::GPS);
	JulianDate GPS2(1340000,TimeSystem::GPS);
	JulianDate UTC1(1350000,TimeSystem::UTC);
	JulianDate UNKNOWN(1350000,TimeSystem::Unknown);
	JulianDate ANY(1350000,TimeSystem::Any);

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

void xJulianDate :: printfTest (void)
{
  JulianDate GPS1(1350000,TimeSystem::GPS);
  JulianDate UTC1(1350000,TimeSystem::UTC);

  CPPUNIT_ASSERT_EQUAL(GPS1.printf("%08J %02P"),(std::string)"1350000.000000 GPS");
  CPPUNIT_ASSERT_EQUAL(UTC1.printf("%08J %02P"),(std::string)"1350000.000000 UTC");
  CPPUNIT_ASSERT_EQUAL(GPS1.printError("%08J %02P"),(std::string)"ErrorBadTime ErrorBadTime");
  CPPUNIT_ASSERT_EQUAL(UTC1.printError("%08J %02P"),(std::string)"ErrorBadTime ErrorBadTime");
}
