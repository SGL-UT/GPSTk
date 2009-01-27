#include "xUnixTime.hpp"
#include "TimeSystem.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xUnixTime);

using namespace Rinex3;

void xUnixTime :: setUp (void)
{
}

void xUnixTime :: setFromInfoTest (void)
{
	UnixTime setFromInfo1;
	UnixTime setFromInfo2;
        UnixTime Compare(1350000,0,TimeSys::GPS);
	
	TimeTag::IdToValue Id;
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
	
	UnixTime Compare(1350000, 100);
	UnixTime LessThanSec(1340000, 100);
	UnixTime LessThanMicroSec(1350000,0);
	
	UnixTime CompareCopy(Compare);
	
	UnixTime CompareCopy2;
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
	
	CPPUNIT_ASSERT(Compare.isValid());
}

void xUnixTime :: resetTest (void)
{
        UnixTime Compare(1350000,0,TimeSys::GPS);

	CommonTime Test = Compare.convertToCommonTime();

	UnixTime Test2;
	Test2.convertFromCommonTime(Test);

	CPPUNIT_ASSERT_EQUAL(Test2,Compare);

	CPPUNIT_ASSERT_EQUAL(TimeSys::GPS,Compare.getTimeSystem());

	CPPUNIT_ASSERT_EQUAL(1350000,(int)Compare.tv.tv_sec);
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.tv.tv_usec);

	Compare.reset();
	CPPUNIT_ASSERT_EQUAL(TimeSys::Unknown,Compare.getTimeSystem());
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.tv.tv_sec);
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.tv.tv_usec);
}

void xUnixTime :: timeSystemTest (void)
{

	UnixTime GPS1(1350000,0,TimeSys::GPS);
	UnixTime GPS2(1340000,0,TimeSys::GPS);
	UnixTime UTC1(1350000,0,TimeSys::UTC);
	UnixTime UNKNOWN(1350000,0,TimeSys::Unknown);
	UnixTime ANY(1350000,0,TimeSys::Any);

	CPPUNIT_ASSERT(GPS1 != GPS2);
	CPPUNIT_ASSERT_EQUAL(GPS1.getTimeSystem(),GPS2.getTimeSystem());
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

	UNKNOWN.setTimeSystem(TimeSys::GPS);
	CPPUNIT_ASSERT_EQUAL(UNKNOWN.getTimeSystem(),TimeSys::GPS);
}

void xUnixTime :: printfTest (void)
{

	UnixTime GPS1(1350000,0,TimeSys::GPS);
	UnixTime UTC1(1350000,0,TimeSys::UTC);

	CPPUNIT_ASSERT_EQUAL(GPS1.printf("%07U %02u %02P"),(std::string)"1350000 00 [GPS]");
	CPPUNIT_ASSERT_EQUAL(UTC1.printf("%07U %02u %02P"),(std::string)"1350000 00 [UTC]");
	CPPUNIT_ASSERT_EQUAL(GPS1.printError("%07U %02u %02P"),(std::string)"ErrorBadTime ErrorBadTime ErrorBadTime");
	CPPUNIT_ASSERT_EQUAL(UTC1.printError("%07U %02u %02P"),(std::string)"ErrorBadTime ErrorBadTime ErrorBadTime");
}
