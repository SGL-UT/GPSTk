#pragma ident "$Id$"
#include "xCivilTime.hpp"
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
	Id.insert(make_pair('Y',"2006"));
	Id.insert(make_pair('H',"12"));
	Id.insert(make_pair('M',"00"));
	Id.insert(make_pair('S',"00"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
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
}

void xCivilTime :: operatorTest (void)
{
	gpstk::CivilTime Zero;
	
	gpstk::CivilTime Aug21(2006,8,21,13,30,15.);
	gpstk::CivilTime LessThanYear(2005,8,21,13,30,15.);
	gpstk::CivilTime LessThanMonth(2006,7,21,13,30,15.);
	gpstk::CivilTime LessThanDay(2006,8,20,13,30,15.);
	gpstk::CivilTime LessThanHour(2006,8,21,12,30,15.);
	gpstk::CivilTime LessThanMinute(2006,8,21,13,20,15.);
	gpstk::CivilTime LessThanSecond(2006,8,21,13,30,0.);
	
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
