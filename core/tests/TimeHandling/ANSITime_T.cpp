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
	ANSITime_T() {eps = 1E-12;}
	~ANSITime_T() {}

	/* Test to ensure the values in the constructor go to their intended locations */
	int  initializationTest (void)
	{
		TestUtil testFramework( "ANSITime", "Constructor(time,TimeSystem)", __FILE__, __func__ );

	  	ANSITime Compare(13500000,TimeSystem(2)); //Initialize an object
//--------------ANSITime_initializationTest_1 - Was the time value set to expectation?
		testFramework.assert(13500000 == (int)Compare.time);

//--------------ANSITime_initializationTest_2 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());

		testFramework.changeSourceMethod("Constructor(ANSITime)");
		ANSITime Copy(Compare); // Initialize with copy constructor
//--------------ANSITime_initializationTest_3 - Was the time value set to expectation?
		testFramework.assert(13500000 == (int)Copy.time);

//--------------ANSITime_initializationTest_4 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Copy.getTimeSystem());

		testFramework.changeSourceMethod("= Operator");
		ANSITime Assigned;
		Assigned = Compare;
//--------------ANSITime_initializationTest_5 - Was the time value set to expectation?
		testFramework.assert(13500000 == (int)Assigned.time);

//--------------ANSITime_initializationTest_6 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Assigned.getTimeSystem());

		return testFramework.countFails();
	}

	/* Test will check if ANSITime variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */
	int setFromInfoTest (void)
	{
		TestUtil testFramework( "ANSITime", "setFromInfo", __FILE__, __func__ );

		ANSITime setFromInfo1;
		ANSITime setFromInfo2;
		ANSITime Compare(13500000,TimeSystem(2)), Compare2(0,TimeSystem(2));
		TimeTag::IdToValue Id;
		Id['K'] = "13500000";
		Id['P'] = "GPS";
//--------------ANSITime_setFromInfoTest_1 - Does a proper setFromInfo work with all information provided?
		testFramework.assert(setFromInfo1.setFromInfo(Id));

//--------------ANSITime_setFromInfoTest_2 - Did the setFromInfo set the proper values?
		testFramework.assert(Compare == setFromInfo1);

		Id.erase('K');
//--------------ANSITime_setFromInfoTest_3 - Does a proper setFromInfo work with missing information?
		testFramework.assert(setFromInfo2.setFromInfo(Id));

//--------------ANSITime_setFromInfoTest_4 - Did the previous setFromInfo set the proper values?
		testFramework.assert(Compare2 == setFromInfo2);		

		return testFramework.countFails();
	}

	/* Test will check if the ways to initialize and set an ANSITime object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{
		TestUtil testFramework( "ANSITime", "== Operator", __FILE__, __func__ );

		ANSITime Compare(13500000); // Initialize with value
		ANSITime LessThan(13400000); // Initialize with value
		ANSITime CompareCopy(Compare); // Initialize with copy constructor
		ANSITime CompareCopy2; //Empty initialization
		CompareCopy2 = CompareCopy; //Assignment

//--------------ANSITime_operatorTest_1 - Are equivalent objects equivalent?
		testFramework.assert(Compare == CompareCopy);

//--------------ANSITime_operatorTest_2 - Are equivalent objects equivalent?
		testFramework.assert(!(Compare == LessThan));

		testFramework.changeSourceMethod("!= Operator");
//--------------ANSITime_operatorTest_3 - Are non-equivalent objects not equivalent?
		testFramework.assert(Compare != LessThan);

//--------------ANSITime_operatorTest_4 - Are non-equivalent objects not equivalent?
		testFramework.assert(!(Compare != Compare));

		testFramework.changeSourceMethod("< Operator");
//--------------ANSITime_operatorTest_5 - Does the < operator function when left_object < right_object?
		testFramework.assert(LessThan < Compare);

//--------------ANSITime_operatorTest_6 - Does the < operator function when left_object > right_object?
		testFramework.assert(!(Compare < LessThan));

//--------------ANSITime_operatorTest_7 - Does the < operator function when left_object = right_object?
		testFramework.assert(!(Compare < CompareCopy));

		testFramework.changeSourceMethod("> Operator");
//--------------ANSITime_operatorTest_8 - Does the > operator function when left_object < right_object?
		testFramework.assert(!(LessThan > Compare));

//--------------ANSITime_operatorTest_9 - Does the > operator function when left_object > right_object?
		testFramework.assert(Compare > LessThan);

//--------------ANSITime_operatorTest_10 - Does the > operator function when left_object = right_object?
		testFramework.assert(!(Compare > CompareCopy));

		testFramework.changeSourceMethod("<= Operator");
//--------------ANSITime_operatorTest_11 - Does the <= operator function when left_object < right_object?
		testFramework.assert(LessThan <= Compare);

//--------------ANSITime_operatorTest_12 - Does the <= operator function when left_object > right_object?
		testFramework.assert(!(Compare <= LessThan));

//--------------ANSITime_operatorTest_13 - Does the <= operator function when left_object = right_object?
		testFramework.assert(Compare <= CompareCopy);

		testFramework.changeSourceMethod(">= Operator");
//--------------ANSITime_operatorTest_14 - Does the >= operator function when left_object < right_object?
		testFramework.assert(!(LessThan >= Compare));

//--------------ANSITime_operatorTest_15 - Does the >= operator function when left_object > right_object?
		testFramework.assert(Compare >= LessThan);

//--------------ANSITime_operatorTest_16 - Does the >= operator function when left_object = right_object?
		testFramework.assert(Compare >= CompareCopy);

		return testFramework.countFails();
	}

	/* Test will check the reset method. */
	int  resetTest (void)
	{
		TestUtil testFramework( "ANSITime", "reset", __FILE__, __func__ );

	  	ANSITime Compare(13500000,TimeSystem(2)); //Initialize an object

	  	Compare.reset(); // Reset it
//--------------ANSITime_operatorTest_1 - Was the time value reset to expectation?
		testFramework.assert(0 == (int)Compare.time);

//--------------ANSITime_operatorTest_2 - Was the time system reset to expectation?
		testFramework.assert(TimeSystem(0) == Compare.getTimeSystem());

		return testFramework.countFails();
	}

	/* Test will check converting to/from CommonTime. */
	int  toFromCommonTimeTest (void)
	{
		TestUtil testFramework( "ANSITime", "isValid", __FILE__, __func__ );

	  	ANSITime Compare(13500000,TimeSystem(2)); //Initialize an object

//--------------ANSITime_toFromCommonTimeTest_1 - Is the time after the BEGINNING_OF_TIME?
  		testFramework.assert(Compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);

//--------------ANSITime_toFromCommonTimeTest_2 - Is the set object vaild?
		testFramework.assert(Compare.isValid());

  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

  		ANSITime Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

		testFramework.changeSourceMethod("CommonTime Conversion");
//--------------ANSITime_toFromCommonTimeTest_3 - Is the result of conversion the same?
		testFramework.assert(Test2 == Compare);

//--------------ANSITime_toFromCommonTimeTest_4 - Is the time system after conversion what is expected?
		testFramework.assert(Compare.getTimeSystem()==TimeSystem(2));

//--------------ANSITime_toFromCommonTimeTest_5 - Is the time after conversion what is expected?
		testFramework.assert(13500000 == (int)Compare.time);

		return testFramework.countFails();
	}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int  timeSystemTest (void)
	{
		TestUtil testFramework( "ANSITime", "Differing TimeSystem == Operator", __FILE__, __func__ );

  		ANSITime GPS1(13500000,TimeSystem(2));
  		ANSITime GPS2(13400000,TimeSystem(2));
  		ANSITime UTC1(13500000,TimeSystem(7));
  		ANSITime UNKNOWN(13500000,TimeSystem(0));
  		ANSITime ANY(13500000,TimeSystem(1));

//--------------ANSITime_timeSystemTest_1 - Verify same Time System but different time inequality
		testFramework.assert(!(GPS1 == GPS2));

//--------------ANSITime_timeSystemTest_2 - Verify same Time System equality
		testFramework.assert(GPS1.getTimeSystem() == GPS2.getTimeSystem());

		testFramework.changeSourceMethod("Differing TimeSystem != Operator");
//--------------ANSITime_timeSystemTest_3 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UTC1);

//--------------ANSITime_timeSystemTest_4 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UNKNOWN);

//--------------ANSITime_timeSystemTest_5 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UNKNOWN);

		testFramework.changeSourceMethod("ANY TimeSystem == Operator");		
//--------------ANSITime_timeSystemTest_6 - Verify TimeSystem=ANY does not matter in TimeSystem=GPS comparisons 
		testFramework.assert(GPS1 == ANY);

//--------------ANSITime_timeSystemTest_7 - Verify TimeSystem=ANY does not matter in TimeSystem=UTC comparisons 
		testFramework.assert(UTC1 == ANY);

//--------------ANSITime_timeSystemTest_8 - Verify TimeSystem=ANY does not matter in TimeSystem=UNKOWN comparisons 
		testFramework.assert(UNKNOWN == ANY);

		testFramework.changeSourceMethod("ANY TimeSystem < Operator");	
//--------------ANSITime_timeSystemTest_9 - Verify TimeSystem=ANY does not matter in other operator comparisons 
		testFramework.assert(!(GPS2 == ANY) && (GPS2 < ANY));

		testFramework.changeSourceMethod("setTimeSystem");	
  		UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem
//--------------ANSITime_timeSystemTest_10 - Ensure resetting a Time System changes it
		testFramework.assert(UNKNOWN.getTimeSystem()==TimeSystem(2));

		return testFramework.countFails();
	}
	/* Test for the formatted printing of ANSITime objects */
	int  printfTest (void)
	{
		TestUtil testFramework( "ANSITime", "printf", __FILE__, __func__ );

  		ANSITime GPS1(13500000,TimeSystem(2));
  		ANSITime UTC1(13500000,TimeSystem(7));

//--------------ANSITime_printfTest_1 - Verify printed output matches expectation
		testFramework.assert(GPS1.printf("%08K %02P") == (std::string)"13500000 GPS");

//--------------ANSITime_printfTest_2 - Verify printed output matches expectation
		testFramework.assert(UTC1.printf("%08K %02P") == (std::string)"13500000 UTC");

		testFramework.changeSourceMethod("printError");	
//--------------ANSITime_printfTest_3 - Verify printed error message matches expectation
		testFramework.assert(GPS1.printError("%08K %02P") == (std::string)"ErrorBadTime ErrorBadTime");

//--------------ANSITime_printfTest_4 - Verify printed error message matches expectation
		testFramework.assert(UTC1.printError("%08K %02P") == (std::string)"ErrorBadTime ErrorBadTime");

		return testFramework.countFails();
	}

	private:
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
