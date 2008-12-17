#include "xUnixTime.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xUnixTime);

using namespace gpstk;

void xUnixTime :: setUp (void)
{
}

void xUnixTime :: setFromInfoTest (void)
{
	Rinex3::UnixTime setFromInfo1;
	Rinex3::UnixTime setFromInfo2;
	
	gpstk::TimeTag::IdToValue Id;
	Id.insert(make_pair('U',"1350000"));
	Id.insert(make_pair('u',"0"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
	Id.erase('U');
	CPPUNIT_ASSERT(setFromInfo2.setFromInfo(Id));
	ofstream out("Logs/printfOutput");
	
	out << setFromInfo1 << endl;
	out << setFromInfo2 << endl;
}

void xUnixTime :: operatorTest (void)
{
	
	Rinex3::UnixTime Compare(1350000, 100);
	Rinex3::UnixTime LessThanSec(1340000, 100);
	Rinex3::UnixTime LessThanMicroSec(1350000,0);
	
	Rinex3::UnixTime CompareCopy(Compare);
	
	Rinex3::UnixTime CompareCopy2;
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
	
	//gpstk::CommonTime Rollover(1,1,.9999999999999);
	//Rinex3::UnixTime Temp;
	//Temp.convertFromCommonTime(Rollover);
	
	CPPUNIT_ASSERT(Compare.isValid());
}

