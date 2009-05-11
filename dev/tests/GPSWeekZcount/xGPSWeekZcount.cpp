#pragma ident "$Id$"
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
