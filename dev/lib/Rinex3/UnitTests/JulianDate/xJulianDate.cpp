#include "xJulianDate.hpp"
#include "TimeSystem.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xJulianDate);

using namespace Rinex3;

void xJulianDate :: setUp (void)
{
}

void xJulianDate :: setFromInfoTest (void)
{
  JulianDate setFromInfo1;
  JulianDate setFromInfo2;
  JulianDate Compare(1350000,TimeSys::GPS);	

  TimeTag::IdToValue Id;
  Id.insert(make_pair('J',"1350000"));
  Id.insert(make_pair('P',"02"));
  CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
  CPPUNIT_ASSERT_EQUAL(setFromInfo1,Compare);
  Id.erase('J');
  CPPUNIT_ASSERT(setFromInfo2.setFromInfo(Id));
}

void xJulianDate :: operatorTest (void)
{
  JulianDate Compare(1350000);
  JulianDate LessThanJD(1340000);

  JulianDate CompareCopy(Compare);

  JulianDate CompareCopy2;
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
  JulianDate Compare(1350000,TimeSys::GPS);

  CommonTime Test = Compare.convertToCommonTime();

  JulianDate Test2;
  Test2.convertFromCommonTime(Test);

  CPPUNIT_ASSERT_EQUAL(Test2,Compare);

  CPPUNIT_ASSERT_EQUAL(TimeSys::GPS,Compare.getTimeSystem());
  CPPUNIT_ASSERT_EQUAL(1350000,(int)Compare.jd);

  Compare.reset();
  CPPUNIT_ASSERT_EQUAL(TimeSys::Unknown,Compare.getTimeSystem());
  CPPUNIT_ASSERT_EQUAL(0,(int)Compare.jd);
}

void xJulianDate :: timeSystemTest (void)
{

	JulianDate GPS1(1350000,TimeSys::GPS);
	JulianDate GPS2(1340000,TimeSys::GPS);
	JulianDate UTC1(1350000,TimeSys::UTC);
	JulianDate UNKNOWN(1350000,TimeSys::Unknown);
	JulianDate ANY(1350000,TimeSys::Any);

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

void xJulianDate :: printfTest (void)
{
  JulianDate GPS1(1350000,TimeSys::GPS);
  JulianDate UTC1(1350000,TimeSys::UTC);

  CPPUNIT_ASSERT_EQUAL(GPS1.printf("%08J %02P"),(std::string)"1350000.000000 02");
  CPPUNIT_ASSERT_EQUAL(UTC1.printf("%08J %02P"),(std::string)"1350000.000000 03");
  CPPUNIT_ASSERT_EQUAL(GPS1.printError("%08J %02P"),(std::string)"ErrorBadTime ErrorBadTime");
  CPPUNIT_ASSERT_EQUAL(UTC1.printError("%08J %02P"),(std::string)"ErrorBadTime ErrorBadTime");
}
