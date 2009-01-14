#include "xYDSTime.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xYDSTime);

using namespace gpstk;

void xYDSTime :: setUp (void)
{
}

void xYDSTime :: setFromInfoTest (void)
{
	Rinex3::YDSTime setFromInfo1;
	Rinex3::YDSTime setFromInfo2;
	Rinex3::YDSTime setFromInfo3;
	Rinex3::YDSTime setFromInfo4;
	Rinex3::YDSTime setFromInfo5;
	Rinex3::YDSTime Compare(2008,2,1,GPS);
	
	gpstk::TimeTag::IdToValue Id;
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
	
	Rinex3::YDSTime Compare(2008,2,1);
	Rinex3::YDSTime LessThanYear(2005,2,1);
	Rinex3::YDSTime LessThanDOY(2008,1,1);
	Rinex3::YDSTime LessThanSOD(2008,2,0);
	
	Rinex3::YDSTime CompareCopy(Compare);
	
	Rinex3::YDSTime CompareCopy2;
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
        Rinex3::YDSTime Compare(2008,2,1,gpstk::GPS);

	CommonTime Test = Compare.convertToCommonTime();

	Rinex3::YDSTime Test2;
	Test2.convertFromCommonTime(Test);

	CPPUNIT_ASSERT_EQUAL(Test2,Compare);

	CPPUNIT_ASSERT_EQUAL(gpstk::GPS,Compare.getTimeSystem());

	CPPUNIT_ASSERT_EQUAL(2008,(int)Compare.year);
	CPPUNIT_ASSERT_EQUAL(2,(int)Compare.doy);
	CPPUNIT_ASSERT_EQUAL(1,(int)Compare.sod);

	Compare.reset();
	CPPUNIT_ASSERT_EQUAL(gpstk::Unknown,Compare.getTimeSystem());
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.year);	
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.doy);
	CPPUNIT_ASSERT_EQUAL(0,(int)Compare.sod);
}

void xYDSTime :: timeSystemTest (void)
{

	Rinex3::YDSTime GPS1(2008,2,1,gpstk::GPS);
	Rinex3::YDSTime GPS2(2005,2,1,gpstk::GPS);
	Rinex3::YDSTime UTC(2008,2,1,gpstk::UTC);
	Rinex3::YDSTime UNKNOWN(2008,2,1,gpstk::Unknown);
	Rinex3::YDSTime ANY(2008,2,1,gpstk::Any);

	CPPUNIT_ASSERT(GPS1 != GPS2);
	CPPUNIT_ASSERT_EQUAL(GPS1.getTimeSystem(),GPS2.getTimeSystem());
	CPPUNIT_ASSERT(GPS1 != UTC);
	CPPUNIT_ASSERT(GPS1 != UNKNOWN);
	CPPUNIT_ASSERT(GPS1.convertToCommonTime() > gpstk::CommonTime::BEGINNING_OF_TIME);
	CPPUNIT_ASSERT(gpstk::CommonTime::BEGINNING_OF_TIME < GPS1);
	CPPUNIT_ASSERT_EQUAL(GPS1,ANY);
	CPPUNIT_ASSERT_EQUAL(UTC,ANY);
	CPPUNIT_ASSERT_EQUAL(UNKNOWN,ANY);
	CPPUNIT_ASSERT(GPS2 != ANY);
	CPPUNIT_ASSERT(GPS2 < GPS1);
	CPPUNIT_ASSERT(GPS2 < ANY);

	UNKNOWN.setTimeSystem(gpstk::GPS);
	CPPUNIT_ASSERT_EQUAL(UNKNOWN.getTimeSystem(),gpstk::GPS);
}

void xYDSTime :: printfTest (void)
{

	Rinex3::YDSTime GPS1(2008,2,1,gpstk::GPS);
	Rinex3::YDSTime UTC(2008,2,1,gpstk::UTC);

	CPPUNIT_ASSERT_EQUAL(GPS1.printf("%04Y %02y %03j %02s %02P"),(std::string)"2008 08 002 1.000000 02");
	CPPUNIT_ASSERT_EQUAL(UTC.printf("%04Y %02y %03j %02s %02P"), (std::string)"2008 08 002 1.000000 03");
	CPPUNIT_ASSERT_EQUAL(GPS1.printError("%04Y %02y %03j %02s %02P"),(std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime");
	CPPUNIT_ASSERT_EQUAL(UTC.printError("%04Y %02y %03j %02s %02P"),(std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime");
}
