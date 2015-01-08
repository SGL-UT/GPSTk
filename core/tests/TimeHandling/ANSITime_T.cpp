#include "ANSITime.hpp"
#include "TimeTag.hpp"
#include "testFramework.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace gpstk;
using namespace std;


class xANSITime : public testFramework
{
	public:
	xANSITime() 
	{
		ostringstream out;
		out << "TestOutput:ANSITime";
		outputString = out.str();
		eps = 1E-12;
	}

	/* Test will check if ANSITime variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */
	int setFromInfoTest (void)
	{
		testSetup();
		ANSITime setFromInfo1;
		ANSITime setFromInfo2;
		ANSITime Compare(13500000,TimeSystem(2)), Compare2(0,TimeSystem(2));
		TimeTag::IdToValue Id;
		Id['K'] = "13500000";
		Id['P'] = "GPS";
//--------------ANSITime_setFromInfoTest_1 - Does a proper setFromInfo work with all information provided?
		passFail = (setFromInfo1.setFromInfo(Id));
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_setFromInfoTest_2 - Did the setFromInfo set the proper values?
		passFail = (Compare == setFromInfo1);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

		Id.erase('K');
//--------------ANSITime_setFromInfoTest_3 - Does a proper setFromInfo work with missing information?
		passFail = setFromInfo2.setFromInfo(Id);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_setFromInfoTest_4 - Did the previous setFromInfo set the proper values?
		passFail = (Compare2 == setFromInfo2);		
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

		return failCount;
	}

	/* Test will check if the ways to initialize and set an ANSITime object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{
		testSetup();
		ANSITime Compare(13500000); // Initialize with value
		ANSITime LessThan(13400000); // Initialize with value
		ANSITime CompareCopy(Compare); // Initialize with copy constructor
		ANSITime CompareCopy2; //Empty initialization
		CompareCopy2 = CompareCopy; //Assignment

//--------------ANSITime_operatorTest_1 - Are equivalent objects equivalent?
		passFail = (Compare == CompareCopy);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_operatorTest_2 - Are non-equivalent objects not equivalent?
		passFail = (Compare != LessThan);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_operatorTest_3 - Does the < operator function when left_object < right_object?
		passFail = (LessThan < Compare);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_operatorTest_4 - Does the < operator function when left_object > right_object?
		passFail = !(Compare < LessThan);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_operatorTest_5 - Does the < operator function when left_object = right_object?
		passFail = !(Compare < CompareCopy);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_operatorTest_6 - Does the > operator function when left_object < right_object?
		passFail = !(LessThan > Compare);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_operatorTest_7 - Does the > operator function when left_object > right_object?
		passFail = (Compare > LessThan);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_operatorTest_8 - Does the > operator function when left_object = right_object?
		passFail = !(Compare > CompareCopy);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_operatorTest_9 - Does the <= operator function when left_object < right_object?
		passFail = (LessThan <= Compare);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_operatorTest_10 - Does the <= operator function when left_object > right_object?
		passFail = !(Compare <= LessThan);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_operatorTest_11 - Does the <= operator function when left_object = right_object?
		passFail = (Compare <= CompareCopy);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_operatorTest_12 - Does the >= operator function when left_object < right_object?
		passFail = !(LessThan >= Compare);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_operatorTest_13 - Does the >= operator function when left_object > right_object?
		passFail = (Compare >= LessThan);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_operatorTest_14 - Does the >= operator function when left_object = right_object?
		passFail = (Compare >= CompareCopy);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

		return failCount;
	}

	/* Test will check the reset method. */
	int  resetTest (void)
	{
		testSetup();
	  	ANSITime Compare(13500000,TimeSystem(2)); //Initialize an object

	  	Compare.reset(); // Reset it
//--------------ANSITime_operatorTest_1 - Was the time value reset to expectation?
		passFail = (0 == (int)Compare.time);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_operatorTest_2 - Was the time system reset to expectation?
		passFail = (TimeSystem(0) == Compare.getTimeSystem());
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

		return failCount;
	}

	/* Test will check converting to/from CommonTime. */
	int  toFromCommonTimeTest (void)
	{
		testSetup();
	  	ANSITime Compare(13500000,TimeSystem(2)); //Initialize an object

//--------------ANSITime_toFromCommonTimeTest_1 - Is the time after the BEGINNING_OF_TIME?
  		passFail = (Compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_toFromCommonTimeTest_2 - Is the set object vaild?
		passFail = (Compare.isValid());
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

  		ANSITime Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

//--------------ANSITime_toFromCommonTimeTest_3 - Is the result of conversion the same?
		passFail = (Test2 == Compare);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_toFromCommonTimeTest_4 - Is the time system after conversion what is expected?
		passFail = (Compare.getTimeSystem()==TimeSystem(2));
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_toFromCommonTimeTest_5 - Is the time after conversion what is expected?
		passFail = (13500000 == (int)Compare.time);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

		return failCount;
	}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int  timeSystemTest (void)
	{
		testSetup();
  		ANSITime GPS1(13500000,TimeSystem(2));
  		ANSITime GPS2(13400000,TimeSystem(2));
  		ANSITime UTC1(13500000,TimeSystem(7));
  		ANSITime UNKNOWN(13500000,TimeSystem(0));
  		ANSITime ANY(13500000,TimeSystem(1));

//--------------ANSITime_timeSystemTest_1 - Verify same Time System but different time inequality
		passFail = !(GPS1 == GPS2);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_timeSystemTest_2 - Verify same Time System equality
		passFail = (GPS1.getTimeSystem() == GPS2.getTimeSystem());
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_timeSystemTest_3 - Verify different Time System but same time inequality
		passFail = (GPS1 != UTC1);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_timeSystemTest_4 - Verify different Time System but same time inequality
		passFail = (GPS1 != UNKNOWN);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_timeSystemTest_5 - Verify different Time System but same time inequality
		passFail = (GPS1 != UNKNOWN);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);
		
//--------------ANSITime_timeSystemTest_6 - Verify TimeSystem=ANY does not matter in TimeSystem=GPS comparisons 
		passFail = (GPS1 == ANY);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_timeSystemTest_7 - Verify TimeSystem=ANY does not matter in TimeSystem=UTC comparisons 
		passFail = (UTC1 == ANY);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_timeSystemTest_8 - Verify TimeSystem=ANY does not matter in TimeSystem=UNKOWN comparisons 
		passFail = (UNKNOWN == ANY);
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_timeSystemTest_9 - Verify TimeSystem=ANY does not matter in other operator comparisons 
		passFail = (!(GPS2 == ANY) && (GPS2 < ANY));
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

  		UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem
//--------------ANSITime_timeSystemTest_10 - Ensure resetting a Time System changes it
		passFail = (UNKNOWN.getTimeSystem()==TimeSystem(2));
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

		return failCount;
	}
	/* Test for the formatted printing of ANSITime objects */
	int  printfTest (void)
	{
		testSetup();
  		ANSITime GPS1(13500000,TimeSystem(2));
  		ANSITime UTC1(13500000,TimeSystem(7));

//--------------ANSITime_printfTest_1 - Verify printed output matches expectation
		passFail = (GPS1.printf("%08K %02P") == (std::string)"13500000 GPS");
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_printfTest_2 - Verify printed output matches expectation
		passFail = (UTC1.printf("%08K %02P") == (std::string)"13500000 UTC");
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_printfTest_3 - Verify printed error message matches expectation
		passFail = (GPS1.printError("%08K %02P") == (std::string)"ErrorBadTime ErrorBadTime");
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

//--------------ANSITime_printfTest_4 - Verify printed error message matches expectation
		passFail = (UTC1.printError("%08K %02P") == (std::string)"ErrorBadTime ErrorBadTime");
		testAssertion(passFail, "setFromInfo", __FILE__, __func__);

		return failCount;
	}

	/*private:
		std::string outputString;
		bool passFail;
		int failCount;
		double eps;*/
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	xANSITime testClass;

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
