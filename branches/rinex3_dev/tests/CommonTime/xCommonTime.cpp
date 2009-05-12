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

#include "xCommonTime.hpp"
#include "TimeConstants.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xCommonTime);

using namespace gpstk;

void xCommonTime :: setUp (void)
{
}

void xCommonTime :: setTest (void)
{
	try
	{
		CommonTime Zero;
		CommonTime Test1(700000,0,0.);
		//Copy-Constructer
		CommonTime Test2(Test1);
		CommonTime Test3 = Test1;
		CommonTime Test4;
		//Assignment
		Test4 = Test1;
		//Set Exceptions
		CPPUNIT_ASSERT_THROW(Test1.set(-1,0,0.),gpstk::Exception);
		CPPUNIT_ASSERT_THROW(Test1.set(3442449,0,0.),gpstk::Exception);
		CPPUNIT_ASSERT_THROW(Test1.set(700000,-1,0.),gpstk::Exception);
		CPPUNIT_ASSERT_THROW(Test1.set(700000,24*60*60+1,0.),gpstk::Exception);
		CPPUNIT_ASSERT_THROW(Test1.set(700000,0,-1.),gpstk::Exception);
		CPPUNIT_ASSERT_THROW(Test1.set(700000,0,2.),gpstk::Exception);
		
		Test1.set(700001,1,.1);
		Test2.set(700001,1.1);
		double dec = 1.1/SEC_PER_DAY;
		Test3.set(700001 + dec);
		
		long day, day2;
		long sod, sod2;
		double fsod, fsod2;
		//Load up compare variables
		Test1.get(day,sod,fsod);
		Test2.get(day2,sod2,fsod2);
		//Compare different sets
		CPPUNIT_ASSERT_EQUAL(day,day2);
		CPPUNIT_ASSERT_EQUAL(sod,sod2);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(fsod,fsod2,1e-6);
		//Load up compare variables
		Test3.get(day2,sod2,fsod2);
		//Compare different sets
		CPPUNIT_ASSERT_EQUAL(day,day2);
		CPPUNIT_ASSERT_EQUAL(sod,sod2);
		//CPPUNIT_ASSERT_DOUBLES_EQUAL(fsod,fsod2,1e-6);
		
		//Check to make sure that the proper exceptions are thrown for setInterval
		CPPUNIT_ASSERT_THROW(Test4.setInternal(-1,0,0.),gpstk::Exception);
		CPPUNIT_ASSERT_THROW(Test4.setInternal(700001,-1,0.),gpstk::Exception);
		CPPUNIT_ASSERT_THROW(Test4.setInternal(700001,86400001,0.),gpstk::Exception);
		CPPUNIT_ASSERT_THROW(Test4.setInternal(700001,1001,-1.),gpstk::Exception);
		CPPUNIT_ASSERT_THROW(Test4.setInternal(700001,1001,1001.),gpstk::Exception);
		//Load up compare variables
		Test4.setInternal(700001,1100,0.);
		Test4.get(day2,sod2,fsod2);
		//Compare setInternal
		CPPUNIT_ASSERT_EQUAL(day,day2);
		CPPUNIT_ASSERT_EQUAL(sod,sod2);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(fsod,fsod2,1e-6);
	}
	catch (gpstk::Exception& e)
	{
		cout << e;
	}
}

void xCommonTime :: arithmiticTest (void)
{
	CommonTime Arith1(700000,1,0.);
	CommonTime Arith2(700000,0,0.);
	double day;
	long day2;
	double sod;
	double fsod;
	//- between two CommonTimes
	CPPUNIT_ASSERT_EQUAL(1.,Arith1-Arith2);
	
	//Add Seconds with +
	Arith2 = Arith2 + 1;
	CPPUNIT_ASSERT_EQUAL(0.,Arith1-Arith2);
	
	//Subtract seconds with -
	Arith2 = Arith2 - 1;
	CPPUNIT_ASSERT_EQUAL(1.,Arith1-Arith2);
	
	//Add seconds with +=
	Arith2 += 1;
	CPPUNIT_ASSERT_EQUAL(0.,Arith1-Arith2);
	
	//Subtract seconds with -=
	Arith2 -= 1;
	CPPUNIT_ASSERT_EQUAL(1.,Arith1-Arith2);
	
	//Add days with addDays
	Arith2.addDays((long)1);
	day = Arith2.getDays();
	CPPUNIT_ASSERT_EQUAL(700001.,day);
	Arith2.addDays((long)-1);
	
	//Add seconds with addSeconds(double)
	Arith2.addSeconds(86400000.+1000.);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(86401000.,Arith2-Arith1,10e-3);

	//Add seconds with addSeconds(long)
	Arith2.addSeconds((long)-86401000);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.,Arith2-Arith1,10e-3);
	
	Arith2.get(day2,sod);
	CPPUNIT_ASSERT_EQUAL((long)700000,day2);
	CPPUNIT_ASSERT_DOUBLES_EQUAL((double).001,sod,1e-6);
	//Check seconds using getSecondOfDay()
	CPPUNIT_ASSERT_DOUBLES_EQUAL(sod,Arith2.getSecondOfDay(),1e-6);
	
	//Add milliseconds with addMilliseconds(long)
	Arith2.addMilliseconds((long)1);
	Arith2.addMilliseconds((long)-1);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(sod,Arith2.getSecondOfDay(),1e-6);
	
	
}

