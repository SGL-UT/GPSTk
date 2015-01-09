#include "ANSITime.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace gpstk;
using namespace std;


class ANSITime_T
{
	public:
	ANSITime_T() 
	{
		eps = 1E-12;
	}

	/* Test will check if ANSITime variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */
	int setFromInfoTest (void)
	{
		TestUtil test1( "ANSITime", "setFromInfo", __FILE__, __func__ );
		test1.init();
		ANSITime setFromInfo1;
		ANSITime setFromInfo2;
		ANSITime Compare(13500000,TimeSystem(2)), Compare2(0,TimeSystem(2));
		TimeTag::IdToValue Id;
		Id['K'] = "13500000";
		Id['P'] = "GPS";
//--------------ANSITime_setFromInfoTest_1 - Does a proper setFromInfo work with all information provided?
		test1.assert (setFromInfo1.setFromInfo(Id));

//--------------ANSITime_setFromInfoTest_2 - Did the setFromInfo set the proper values?
		test1.assert (Compare == setFromInfo1);

		Id.erase('K');
//--------------ANSITime_setFromInfoTest_3 - Does a proper setFromInfo work with missing information?
		test1.assert (setFromInfo2.setFromInfo(Id));

//--------------ANSITime_setFromInfoTest_4 - Did the previous setFromInfo set the proper values?
		test1.assert (Compare2 == setFromInfo2);		

		return test1.countFails();
	}

	/* Test will check if the ways to initialize and set an ANSITime object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{
		TestUtil test2( "ANSITime", "operatorTest", __FILE__, __func__ );
		test2.init();
		ANSITime Compare(13500000); // Initialize with value
		ANSITime LessThan(13400000); // Initialize with value
		ANSITime CompareCopy(Compare); // Initialize with copy constructor
		ANSITime CompareCopy2; //Empty initialization
		CompareCopy2 = CompareCopy; //Assignment

//--------------ANSITime_operatorTest_1 - Are equivalent objects equivalent?
		test2.assert (Compare == CompareCopy);

//--------------ANSITime_operatorTest_2 - Are non-equivalent objects not equivalent?
		test2.assert (Compare != LessThan);

//--------------ANSITime_operatorTest_3 - Does the < operator function when left_object < right_object?
		test2.assert (LessThan < Compare);

//--------------ANSITime_operatorTest_4 - Does the < operator function when left_object > right_object?
		test2.assert (!(Compare < LessThan));

//--------------ANSITime_operatorTest_5 - Does the < operator function when left_object = right_object?
		test2.assert (!(Compare < CompareCopy));

//--------------ANSITime_operatorTest_6 - Does the > operator function when left_object < right_object?
		test2.assert (!(LessThan > Compare));

//--------------ANSITime_operatorTest_7 - Does the > operator function when left_object > right_object?
		test2.assert (Compare > LessThan);

//--------------ANSITime_operatorTest_8 - Does the > operator function when left_object = right_object?
		test2.assert (!(Compare > CompareCopy));

//--------------ANSITime_operatorTest_9 - Does the <= operator function when left_object < right_object?
		test2.assert (LessThan <= Compare);

//--------------ANSITime_operatorTest_10 - Does the <= operator function when left_object > right_object?
		test2.assert (!(Compare <= LessThan));

//--------------ANSITime_operatorTest_11 - Does the <= operator function when left_object = right_object?
		test2.assert (Compare <= CompareCopy);

//--------------ANSITime_operatorTest_12 - Does the >= operator function when left_object < right_object?
		test2.assert (!(LessThan >= Compare));

//--------------ANSITime_operatorTest_13 - Does the >= operator function when left_object > right_object?
		test2.assert (Compare >= LessThan);

//--------------ANSITime_operatorTest_14 - Does the >= operator function when left_object = right_object?
		test2.assert (Compare >= CompareCopy);

		return test2.countFails();
	}

	/* Test will check the reset method. */
	int  resetTest (void)
	{
		TestUtil test3( "ANSITime", "operatorTest", __FILE__, __func__ );
		test3.init();
	  	ANSITime Compare(13500000,TimeSystem(2)); //Initialize an object

	  	Compare.reset(); // Reset it
//--------------ANSITime_operatorTest_1 - Was the time value reset to expectation?
		test3.assert (0 == (int)Compare.time);

//--------------ANSITime_operatorTest_2 - Was the time system reset to expectation?
		test3.assert (TimeSystem(0) == Compare.getTimeSystem());

		return test3.countFails();
	}

	/* Test will check converting to/from CommonTime. */
	int  toFromCommonTimeTest (void)
	{
		TestUtil test4( "ANSITime", "operatorTest", __FILE__, __func__ );
		test4.init();
	  	ANSITime Compare(13500000,TimeSystem(2)); //Initialize an object

//--------------ANSITime_toFromCommonTimeTest_1 - Is the time after the BEGINNING_OF_TIME?
  		test4.assert (Compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);

//--------------ANSITime_toFromCommonTimeTest_2 - Is the set object vaild?
		test4.assert (Compare.isValid());

  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

  		ANSITime Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

//--------------ANSITime_toFromCommonTimeTest_3 - Is the result of conversion the same?
		test4.assert (Test2 == Compare);

//--------------ANSITime_toFromCommonTimeTest_4 - Is the time system after conversion what is expected?
		test4.assert (Compare.getTimeSystem()==TimeSystem(2));

//--------------ANSITime_toFromCommonTimeTest_5 - Is the time after conversion what is expected?
		test4.assert (13500000 == (int)Compare.time);

		return test4.countFails();
	}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int  timeSystemTest (void)
	{
		TestUtil test5( "ANSITime", "operatorTest", __FILE__, __func__ );
		test5.init();
  		ANSITime GPS1(13500000,TimeSystem(2));
  		ANSITime GPS2(13400000,TimeSystem(2));
  		ANSITime UTC1(13500000,TimeSystem(7));
  		ANSITime UNKNOWN(13500000,TimeSystem(0));
  		ANSITime ANY(13500000,TimeSystem(1));

//--------------ANSITime_timeSystemTest_1 - Verify same Time System but different time inequality
		test5.assert (!(GPS1 == GPS2));

//--------------ANSITime_timeSystemTest_2 - Verify same Time System equality
		test5.assert (GPS1.getTimeSystem() == GPS2.getTimeSystem());

//--------------ANSITime_timeSystemTest_3 - Verify different Time System but same time inequality
		test5.assert (GPS1 != UTC1);

//--------------ANSITime_timeSystemTest_4 - Verify different Time System but same time inequality
		test5.assert (GPS1 != UNKNOWN);

//--------------ANSITime_timeSystemTest_5 - Verify different Time System but same time inequality
		test5.assert (GPS1 != UNKNOWN);
		
//--------------ANSITime_timeSystemTest_6 - Verify TimeSystem=ANY does not matter in TimeSystem=GPS comparisons 
		test5.assert (GPS1 == ANY);

//--------------ANSITime_timeSystemTest_7 - Verify TimeSystem=ANY does not matter in TimeSystem=UTC comparisons 
		test5.assert (UTC1 == ANY);

//--------------ANSITime_timeSystemTest_8 - Verify TimeSystem=ANY does not matter in TimeSystem=UNKOWN comparisons 
		test5.assert (UNKNOWN == ANY);

//--------------ANSITime_timeSystemTest_9 - Verify TimeSystem=ANY does not matter in other operator comparisons 
		test5.assert (!(GPS2 == ANY) && (GPS2 < ANY));

  		UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem
//--------------ANSITime_timeSystemTest_10 - Ensure resetting a Time System changes it
		test5.assert (UNKNOWN.getTimeSystem()==TimeSystem(2));

		return test5.countFails();
	}
	/* Test for the formatted printing of ANSITime objects */
	int  printfTest (void)
	{
		TestUtil test6( "ANSITime", "operatorTest", __FILE__, __func__ );
		test6.init();
  		ANSITime GPS1(13500000,TimeSystem(2));
  		ANSITime UTC1(13500000,TimeSystem(7));

//--------------ANSITime_printfTest_1 - Verify printed output matches expectation
		test6.assert (GPS1.printf("%08K %02P") == (std::string)"13500000 GPS");

//--------------ANSITime_printfTest_2 - Verify printed output matches expectation
		test6.assert (UTC1.printf("%08K %02P") == (std::string)"13500000 UTC");

//--------------ANSITime_printfTest_3 - Verify printed error message matches expectation
		test6.assert (GPS1.printError("%08K %02P") == (std::string)"ErrorBadTime ErrorBadTime");

//--------------ANSITime_printfTest_4 - Verify printed error message matches expectation
		test6.assert (UTC1.printError("%08K %02P") == (std::string)"ErrorBadTime ErrorBadTime");

		return test6.countFails();
	}

	private:
		std::string outputString;
		bool passFail;
		int failCount;
		double eps;
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	ANSITime_T testClass;

	check = testClass.operatorTest();
	errorCounter += check;

	check = testClass.setFromInfoTest();
	errorCounter += check;

	check = testClass.resetTest();
	errorCounter += check;

	check = testClass.timeSystemTest();
	errorCounter += check;

	check = testClass.toFromCommonTimeTest();
	errorCounter += check;

	check = testClass.printfTest();
	errorCounter += check;
	
	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
