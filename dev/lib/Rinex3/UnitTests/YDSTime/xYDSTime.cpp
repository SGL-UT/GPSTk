#include "xYDSTime.hpp"
#include "TimeSystem.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xYDSTime);

using namespace Rinex3;

void xYDSTime :: setUp (void)
{
}

void xYDSTime :: setFromInfoTest (void)
{
	YDSTime setFromInfo1;
	YDSTime setFromInfo2;
	YDSTime setFromInfo3;
	YDSTime setFromInfo4;
	YDSTime setFromInfo5;
	YDSTime Compare(2008,2,1,TimeSys::GPS);
	
	TimeTag::IdToValue Id;
	Id.insert(make_pair('Y',"2008"));
	Id.insert(make_pair('j',"2"));
	Id.insert(make_pair('s',"1"));
	Id.insert(make_pair('P',"02"));
	CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
	CPPUNIT_ASSERT_EQUAL(setFromInfo1,Compare);
	Id.erase('Y');
	Id.insert(make_pair('y',"06"));
	CPPUNIT_ASSERT(setFromInfo2.setFromInfo(Id));
	Id.erase('y');
	Id.insert(make_pair('y',"006"));
	CPPUNIT_ASSERT(setFromInfo3.setFromInfo(Id));
	Id.erase('y');
	Id.insert(make_pair('y',"2008"));
	CPPUNIT_ASSERT(setFromInfo4.setFromInfo(Id));
	Id.erase('y');
	CPPUNIT_ASSERT(setFromInfo5.setFromInfo(Id));
}

void xYDSTime :: operatorTest (void)
{
	
	YDSTime Compare(2008,2,1);
	YDSTime LessThanYear(2005,2,1);
	YDSTime LessThanDOY(2008,1,1);
	YDSTime LessThanSOD(2008,2,0);
	
	YDSTime CompareCopy(Compare);
	
	YDSTime CompareCopy2;
	//Assignment
	CompareCopy2 = CompareCopy;
	//Equality Assertion
	CPPUNIT_ASSERT_EQUAL(Compare,CompareCopy);
	//Non-equality Assertion
	CPPUNIT_ASSERT(Compare != LessThanYear);
	//Less than assertions
	CPPUNIT_ASSERT(LessThanYear < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanYear));
	CPPUNIT_ASSERT(LessThanDOY < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanDOY));
	CPPUNIT_ASSERT(LessThanSOD < Compare);
	CPPUNIT_ASSERT(!(Compare < LessThanSOD));
	//Greater than assertions
	CPPUNIT_ASSERT(Compare > LessThanYear);
	CPPUNIT_ASSERT(Compare > LessThanDOY);
	CPPUNIT_ASSERT(Compare > LessThanSOD);
	//Less than equals assertion
	CPPUNIT_ASSERT(LessThanYear <= Compare);
	CPPUNIT_ASSERT(CompareCopy <= Compare);
	//Greater than equals assertion
	CPPUNIT_ASSERT(Compare >= LessThanYear);
	CPPUNIT_ASSERT(Compare >= CompareCopy);
	
	CPPUNIT_ASSERT(Compare.isValid());
}

void xYDSTime :: resetTest (void)
{
        YDSTime Compare(2008,2,1,TimeSys::GPS);

	CommonTime Test = Compare.convertToCommonTime();

	YDSTime Test2;
	Test2.convertFromCommonTime(Test);

	CPPUNIT_ASSERT_EQUAL(Test2,Compare);

	CPPUNIT_ASSERT_EQUAL(TimeSys::GPS,Compare.getTimeSystem());

	CPPUNIT_ASSERT_EQUAL(2008,(int)Compare.year);
	CPPUNIT_ASSERT_EQUAL(2,(int)Compare.doy);
	CPPUNIT_ASSERT_EQUAL(1,(int)Compare.sod);

	Compare.reset();
	CPPUNIT_ASSERT_EQUAL(TimeSys::Unknown,Compare.getTimeSystem());
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.year);	
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.doy);
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.sod);
}

void xYDSTime :: timeSystemTest (void)
{

	YDSTime GPS1(2008,2,1,TimeSys::GPS);
	YDSTime GPS2(2005,2,1,TimeSys::GPS);
	YDSTime UTC1(2008,2,1,TimeSys::UTC);
	YDSTime UNKNOWN(2008,2,1,TimeSys::Unknown);
	YDSTime ANY(2008,2,1,TimeSys::Any);

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

void xYDSTime :: printfTest (void)
{

	YDSTime GPS1(2008,2,1,TimeSys::GPS);
	YDSTime UTC1(2008,2,1,TimeSys::UTC);

	CPPUNIT_ASSERT_EQUAL(GPS1.printf("%04Y %02y %03j %02s %02P"),(std::string)"2008 08 002 1.000000 02");
	CPPUNIT_ASSERT_EQUAL(UTC1.printf("%04Y %02y %03j %02s %02P"),(std::string)"2008 08 002 1.000000 03");
	CPPUNIT_ASSERT_EQUAL(GPS1.printError("%04Y %02y %03j %02s %02P"),(std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime");
	CPPUNIT_ASSERT_EQUAL(UTC1.printError("%04Y %02y %03j %02s %02P"),(std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime");
}
