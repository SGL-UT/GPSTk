#include "xANSITime.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xANSITime);

using namespace gpstk;

void xANSITime :: setUp (void)
{
}

void xANSITime :: setFromInfoTest (void)
{
  ANSITime setFromInfo1;
  ANSITime setFromInfo2;
  ANSITime Compare(13500000,TimeSystem(2));
  TimeTag::IdToValue Id;

  Id.insert(make_pair('K',"13500000"));
  Id.insert(make_pair('P',"2"));
  CPPUNIT_ASSERT(setFromInfo1.setFromInfo(Id));
  CPPUNIT_ASSERT_EQUAL(setFromInfo1,Compare);
  Id.erase('K');
  CPPUNIT_ASSERT(setFromInfo2.setFromInfo(Id));
}

void xANSITime :: operatorTest (void)
{

  ANSITime Compare(13500000);
  ANSITime LessThan(13400000);
  ANSITime CompareCopy(Compare);
  ANSITime CompareCopy2;

  //Assignment
  CompareCopy2 = CompareCopy;
  //Equality Assertion
  CPPUNIT_ASSERT_EQUAL(Compare,CompareCopy);
  //Non-equality Assertion
  CPPUNIT_ASSERT(Compare != LessThan);
  //Less than assertions
  CPPUNIT_ASSERT(LessThan < Compare);
  CPPUNIT_ASSERT(!(Compare < LessThan));
  //Greater than assertions
  CPPUNIT_ASSERT(Compare > LessThan);
  //Less than equals assertion
  CPPUNIT_ASSERT(LessThan <= Compare);
  CPPUNIT_ASSERT(CompareCopy <= Compare);
  //Greater than equals assertion
  CPPUNIT_ASSERT(Compare >= LessThan);
  CPPUNIT_ASSERT(Compare >= CompareCopy);

  CPPUNIT_ASSERT(Compare.isValid());

}

void xANSITime :: resetTest (void)
{

  ANSITime Compare(13500000,TimeSystem(2));

  CommonTime Test = Compare.convertToCommonTime();

  ANSITime Test2;
  Test2.convertFromCommonTime(Test);

  CPPUNIT_ASSERT_EQUAL(Test2,Compare);
  
  CPPUNIT_ASSERT(Compare.getTimeSystem()==TimeSystem(2));
  CPPUNIT_ASSERT_EQUAL(13500000,(int)Compare.time);

  Compare.reset();
  CPPUNIT_ASSERT(TimeSystem(0)==Compare.getTimeSystem());
  CPPUNIT_ASSERT_EQUAL(0,(int)Compare.time);

}

void xANSITime :: timeSystemTest (void)
{

  ANSITime GPS1(13500000,TimeSystem(2));
  ANSITime GPS2(13400000,TimeSystem(2));
  ANSITime UTC1(13500000,TimeSystem(5));
  ANSITime UNKNOWN(13500000,TimeSystem(0));
  ANSITime ANY(13500000,TimeSystem(1));

  CPPUNIT_ASSERT(GPS1 != GPS2);
  CPPUNIT_ASSERT(GPS1.getTimeSystem()==GPS2.getTimeSystem());
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

  UNKNOWN.setTimeSystem(TimeSystem(2));
  CPPUNIT_ASSERT(UNKNOWN.getTimeSystem()==TimeSystem(2));
}

void xANSITime :: printfTest (void)
{

  ANSITime GPS1(13500000,TimeSystem(2));
  ANSITime UTC1(13500000,TimeSystem(5));

  CPPUNIT_ASSERT_EQUAL(GPS1.printf("%08K %02P"),(std::string)"13500000 GPS");
  CPPUNIT_ASSERT_EQUAL(UTC1.printf("%08K %02P"),(std::string)"13500000 UTC");
  CPPUNIT_ASSERT_EQUAL(GPS1.printError("%08K %02P"),(std::string)"ErrorBadTime ErrorBadTime");
  CPPUNIT_ASSERT_EQUAL(UTC1.printError("%08K %02P"),(std::string)"ErrorBadTime ErrorBadTime");

}
