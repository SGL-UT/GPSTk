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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include "xTimeConverters.hpp"
#include <iostream>

CPPUNIT_TEST_SUITE_REGISTRATION (xTimeConverters);

using namespace gpstk;

void xTimeConverters :: setUp (void)
{
}

//I am testing these conversions against the U.S. Naval Observatory's Julian Date
//Converter found at http://aa.usno.navy.mil/data/docs/JulianDate.html

void xTimeConverters :: convertTest (void)
{
	int year;
	int month;
	int day;
	
	gpstk::convertJDtoCalendar(2453971,year,month,day);
	CPPUNIT_ASSERT_EQUAL(2006,year);
	CPPUNIT_ASSERT_EQUAL(8,month);
	CPPUNIT_ASSERT_EQUAL(23,day);
	gpstk::convertJDtoCalendar(2299159,year,month,day);
	CPPUNIT_ASSERT_EQUAL(1582,year);
	CPPUNIT_ASSERT_EQUAL(10,month);
	CPPUNIT_ASSERT_EQUAL(3,day);
	gpstk::convertJDtoCalendar(2342032,year,month,day);
	CPPUNIT_ASSERT_EQUAL(1700,year);
	CPPUNIT_ASSERT_EQUAL(3,month);
	CPPUNIT_ASSERT_EQUAL(1,day);
	gpstk::convertJDtoCalendar(2377095,year,month,day);
	CPPUNIT_ASSERT_EQUAL(1796,year);
	CPPUNIT_ASSERT_EQUAL(2,month);
	CPPUNIT_ASSERT_EQUAL(29,day);
	gpstk::convertJDtoCalendar(1721118,year,month,day);
	CPPUNIT_ASSERT_EQUAL(-1,year);
	CPPUNIT_ASSERT_EQUAL(3,month);
	CPPUNIT_ASSERT_EQUAL(1,day);
	
	long JD;
	year = 2006;
	month = 8;
	day = 23;
	JD = gpstk::convertCalendarToJD(year,month,day);
	CPPUNIT_ASSERT_EQUAL((long int)2453971,JD);
	year = 1582;
	month = 10;
	day = 3;
	JD = gpstk::convertCalendarToJD(year,month,day);
	
	CPPUNIT_ASSERT_EQUAL((long int)2299159,JD);
	year = 1700;
	month = 3;
	day = 1;
	JD = gpstk::convertCalendarToJD(year,month,day);
		
	CPPUNIT_ASSERT_EQUAL((long int)2342032,JD);
	year = 0;
	month = 3;
	day = 1;
	JD = gpstk::convertCalendarToJD(year,month,day);
	
	CPPUNIT_ASSERT_EQUAL((long int)1721118,JD);
	year = -5;
	month = 3;
	day = 1;
	JD = gpstk::convertCalendarToJD(year,month,day);
	CPPUNIT_ASSERT_EQUAL((long int)1719657,JD);
	
	int hour;
	int minute;
	double second;
	gpstk::convertSODtoTime(-.01,hour,minute,second);
	CPPUNIT_ASSERT_EQUAL(23,hour);
	CPPUNIT_ASSERT_EQUAL(59,minute);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(59.99,second,1e-6);
	gpstk::convertSODtoTime(24*60*60+1.11,hour,minute,second);
	CPPUNIT_ASSERT_EQUAL(0,hour);
	CPPUNIT_ASSERT_EQUAL(0,minute);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.11,second,1e-6);
	
	hour = 1;
	minute = 10;
	second = 30.5;
	double SOD = gpstk::convertTimeToSOD(hour,minute,second);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(60*60+600+30.5,SOD,1e-6);
}
