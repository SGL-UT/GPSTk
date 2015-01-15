#include "GPSWeekSecond.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

#define testAssertion(x, y) \
	if (!(x)) \
	{ \
		cout << "Error!! testAssert on line " << __LINE__ << " has failed." << endl; \
		y++; \
	}



class GPSWeekSecond_T
{
	public:
		GPSWeekSecond_T(){eps = 1e-11;}// Default Constructor, set the precision value
		~GPSWeekSecond_T() {} // Default Desructor

	/* Test to ensure the values in the constructor go to their intended locations */
	int initializationTest(void)
	{
		TestUtil testFramework( "GPSWeekSecond", "Constructor(week,second,TimeSystem)", __FILE__, __func__ );
		testFramework.init();

		GPSWeekSecond Compare(1300,13500.,TimeSystem(2)); //Initialize an object
//--------------GPSWeekSecondTime_initializationTest_1 - Was the week value set to expectation?
		testFramework.assert(1300 == Compare.week);
		testFramework.next();

//--------------GPSWeekSecondTime_initializationTest_2 - Was the sow value set to expectation?
		testFramework.assert(13500 - Compare.sow < eps && Compare.sow - 13500 < eps);
		testFramework.next();

//--------------GPSWeekSecondTime_initializationTest_3 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("Constructor(GPSWeekSecond)");
		GPSWeekSecond Copy(Compare); // Initialize with copy constructor

//--------------GPSWeekSecondTime_initializationTest_4 - Was the week value set to expectation?
		testFramework.assert(1300 == Compare.week);
		testFramework.next();

//--------------GPSWeekSecondTime_initializationTest_5 - Was the sow value set to expectation?
		testFramework.assert(13500 == Compare.sow);
		testFramework.next();

//--------------GPSWeekSecondTime_initializationTest_6 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("= Operator");
		GPSWeekSecond Assigned;
		Assigned = Compare;

//--------------GPSWeekSecondTime_initializationTest_7 - Was the week value set to expectation?
		testFramework.assert(1300 == Compare.week);
		testFramework.next();

//--------------GPSWeekSecondTime_initializationTest_8 - Was the sow value set to expectation?
		testFramework.assert(13500 == Compare.sow);
		testFramework.next();

//--------------GPSWeekSecondTime_initializationTest_9 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());
		testFramework.next();

		return testFramework.countFails();

	}

	/* Test will check if GPSWeekSecond variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */
/*	int setFromInfoTest (void)
	{
	
		GPSWeekSecond setFromInfo1;
		GPSWeekSecond setFromInfo2;
		int numErrors = 0;

		TimeTag::IdToValue Id;
		Id['F'] = "1300";
		Id['g'] = "13500";
		Id['P'] = "GPS";
		testAssertion(setFromInfo1.setFromInfo(Id),numErrors);
		GPSWeekSecond Compare(1300,13500.,TimeSystem(2));
		testAssertion(Compare == setFromInfo1 , numErrors);
		Id.erase('F');
		testAssertion(setFromInfo2.setFromInfo(Id), numErrors);
		GPSWeekSecond Compare2(0,13500.,TimeSystem(2));
		testAssertion(Compare2 == setFromInfo2, numErrors);
	
		return numErrors;

	}*/

		/* Test will check if GPSWeekSecond variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */
	int setFromInfoTest (void)
	{

		TestUtil testFramework( "GPSWeekSecond", "setFromInfo", __FILE__, __func__ );
		testFramework.init();

		GPSWeekSecond setFromInfo1;
		GPSWeekSecond setFromInfo2;
		GPSWeekSecond setFromInfo3;
		TimeTag::IdToValue Id;
		Id['F'] = "1300";
		Id['g'] = "13500";
		Id['P'] = "GPS";

//--------------GPSWeekSecond_setFromInfoTest_1 - Does a proper setFromInfo work with all information provided?
		testFramework.assert(setFromInfo1.setFromInfo(Id));
		testFramework.next();

		GPSWeekSecond Compare(1300,13500.,TimeSystem(2)); //Initialize an object

//--------------GPSWeekSecond_setFromInfoTest_2 - Did the setFromInfo set the proper values?
		testFramework.assert(Compare == setFromInfo1);
		testFramework.next();

		Id.erase('F');

//--------------GPSWeekSecond_setFromInfoTest_3 - Does a proper setFromInfo work with missing data?
		testFramework.assert(setFromInfo2.setFromInfo(Id));
		testFramework.next();


		GPSWeekSecond Compare2(0,13500.,TimeSystem(2));

//--------------GPSWeekSecond_setFromInfoTest_4 - Did the previous setFromInfo set the proper values?
		testFramework.assert(Compare2 == setFromInfo2);
		testFramework.next();



		return testFramework.countFails();
	}

	/* Test will check if the ways to initialize and set an GPSWeekSecond object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{
		TestUtil testFramework( "GPSWeekSecond", "== Operator", __FILE__, __func__ );
		testFramework.init();

		GPSWeekSecond Compare(1300,13500.);
		GPSWeekSecond LessThanWeek(1299,13500.);
		GPSWeekSecond LessThanSecond(1300,13400.);
		GPSWeekSecond CompareCopy(Compare); // Initialize with copy constructor
		GPSWeekSecond CompareCopy2; //Empty initialization
		CompareCopy2 = CompareCopy; //Assignment

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	Why have CompareCopy2? Never called. Also, LessThanZCount was only called once
//	in the original test. Shouldn't it be called more for more rigorous testing?
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//--------------GPSWeekSecond_operatorTest_1 - Are equivalent objects equivalent?
		testFramework.assert(Compare == CompareCopy);
		testFramework.next();

//--------------GPSWeekSecond_operatorTest_2 - Are equivalent objects equivalent?
		testFramework.assert(!(Compare == LessThanWeek));
		testFramework.next();

		testFramework.changeSourceMethod("!= Operator");
//--------------GPSWeekSecond_operatorTest_3 - Are non-equivalent objects not equivalent?
		testFramework.assert(Compare != LessThanWeek);
		testFramework.next();

//--------------GPSWeekSecond_operatorTest_4 - Are equivalent objects not equivalent?
		testFramework.assert(!(Compare != Compare));
		testFramework.next();

		testFramework.changeSourceMethod("< Operator");
//--------------GPSWeekSecond_operatorTest_5 - Does the < operator function when left_object < right_object?
		testFramework.assert(LessThanWeek < Compare);
		testFramework.next();

//--------------GPSWeekSecond_operatorTest_6 - Does the < operator function when left_object > right_object?
		testFramework.assert(!(Compare < LessThanWeek));
		testFramework.next();

//--------------GPSWeekSecond_operatorTest_7 - Does the second cause left_object > right_object?
		testFramework.assert(Compare > LessThanSecond);

//--------------GPSWeekSecond_operatorTest_8 - Does the < operator function when left_object = right_object?
		testFramework.assert(!(Compare < CompareCopy));
		testFramework.next();

		testFramework.changeSourceMethod("> Operator");
//--------------GPSWeekSecond_operatorTest_9 - Does the > operator function when left_object < right_object?
		testFramework.assert(!(LessThanWeek > Compare));
		testFramework.next();

//--------------GPSWeekSecond_operatorTest_10 - Does the > operator function when left_object > right_object?
		testFramework.assert(Compare > LessThanWeek);
		testFramework.next();

//--------------GPSWeekSecond_operatorTest_11 - Does the > operator function when left_object = right_object?
		testFramework.assert(!(Compare > CompareCopy));
		testFramework.next();

		testFramework.changeSourceMethod("<= Operator");
//--------------GPSWeekSecond_operatorTest_12 - Does the <= operator function when left_object < right_object?
		testFramework.assert(LessThanWeek <= Compare);
		testFramework.next();

//--------------GPSWeekSecond_operatorTest_13 - Does the <= operator function when left_object > right_object?
		testFramework.assert(!(Compare <= LessThanWeek));
		testFramework.next();

//--------------GPSWeekSecond_operatorTest_14 - Does the <= operator function when left_object = right_object?
		testFramework.assert(Compare <= CompareCopy);
		testFramework.next();

		testFramework.changeSourceMethod(">= Operator");
//--------------GPSWeekSecond_operatorTest_15 - Does the >= operator function when left_object < right_object?
		testFramework.assert(!(LessThanWeek >= Compare));
		testFramework.next();

//--------------GPSWeekSecond_operatorTest_16 - Does the >= operator function when left_object > right_object?
		testFramework.assert(Compare >= LessThanWeek);
		testFramework.next();

//--------------GPSWeekSecond_operatorTest_17 - Does the >= operator function when left_object = right_object?
		testFramework.assert(Compare >= CompareCopy);
		testFramework.next();

		testFramework.changeSourceMethod("isValid Method");

//--------------GPSWeekSecond_operatorTest_17 - Does the isValid method function properly?
		testFramework.assert(Compare.isValid());

		return testFramework.countFails();
	}

	/* Test will check the reset method. */
	int  resetTest (void)
	{

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	Tests 1-3 seem redundant to setFromInfoTests
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

		TestUtil testFramework( "GPSWeekSecond", "reset", __FILE__, __func__ );
		testFramework.init();	
		GPSWeekSecond Compare(1300,13500.,TimeSystem::GPS); //Initialize an object

//--------------GPSWeekSecond_resetTest_1 - Was the week value set to expectation?
		testFramework.assert(1300 == Compare.week);
		testFramework.next();

//--------------GPSWeekSecond_resetTest_2 - Was the sow value set to expectation?
		testFramework.assert(13500 == Compare.sow);
		testFramework.next();

//--------------GPSWeekSecond_resetTest_3 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());
		testFramework.next();

	  	Compare.reset(); // Reset it

//--------------GPSWeekSecond_resetTest_4 - Was the week value reset to expectation?
	  	testFramework.assert(0 == (int)Compare.week);
	  	testFramework.next();

//--------------GPSWeekSecond_resetTest_5 - Was the sow value reset to expectation?
	  	testFramework.assert(0 == (int)Compare.sow);
	  	testFramework.next();

//--------------GPSWeekSecond_resetTest_6 - Was the time system reset to expectation?
		/*GPSWeekSecond resets to GPS TimeSystem!!!!*/
	  	testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());

		return testFramework.countFails();
	}

	/* Test will check converting to/from CommonTime. */
	int  toFromCommonTimeTest (void)
	{
		TestUtil testFramework( "GPSWeekSecond", "isValid", __FILE__, __func__ );
		testFramework.init();

	  	GPSWeekSecond Compare(1300,13500.,TimeSystem(2)); //Initialize an object

//--------------GPSWeekSecond_toFromCommonTimeTest_1 - Is the time after the BEGINNING_OF_TIME?
  		testFramework.assert(Compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);
		testFramework.next();

//--------------GPSWeekSecond_toFromCommonTimeTest_2 - Is the set object valid?
		testFramework.assert(Compare.isValid());
		testFramework.next();

  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

  		GPSWeekSecond Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

		testFramework.changeSourceMethod("CommonTime Conversion");
//--------------GPSWeekSecond_toFromCommonTimeTest_3 - Is the result of conversion the same?
		testFramework.assert(Test2 == Compare);
		testFramework.next();

//--------------GPSWeekSecond_toFromCommonTimeTest_4 - Is the time system after conversion what is expected?
		testFramework.assert(Compare.getTimeSystem()==TimeSystem(2));
		testFramework.next();

//--------------GPSWeekSecond_toFromCommonTimeTest_5 - Is the week after conversion what is expected?
		testFramework.assert(Compare.week==1300);
		testFramework.next();

//--------------GPSWeekSecond_toFromCommonTimeTest_6 - Is the sow after conversion what is expected?
		testFramework.assert(Compare.sow==13500);

		return testFramework.countFails();
	}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int  timeSystemTest (void)
	{
		TestUtil testFramework( "GPSWeekSecond", "Differing TimeSystem == Operator", __FILE__, __func__ );
		testFramework.init();

  		GPSWeekSecond GPS1(1300,13500.,TimeSystem(2));
  		GPSWeekSecond GPS2(1200,13500.,TimeSystem(2));
  		GPSWeekSecond UTC1(1300,13500.,TimeSystem(5));
  		GPSWeekSecond UNKNOWN(1300,13500.,TimeSystem(0));
  		GPSWeekSecond ANY(1300,13500.,TimeSystem(1));
//--------------GPSWeekSecond_timeSystemTest_1 - Verify same Time System but different time inequality
		testFramework.assert(!(GPS1 == GPS2));
		testFramework.next();

//--------------GPSWeekSecond_timeSystemTest_2 - Verify same Time System equality
		testFramework.assert(GPS1.getTimeSystem() == GPS2.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("Differing TimeSystem != Operator");
//--------------GPSWeekSecond_timeSystemTest_3 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UTC1);
		testFramework.next();

//--------------GPSWeekSecond_timeSystemTest_4 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UNKNOWN);
		testFramework.next();

		testFramework.changeSourceMethod("ANY TimeSystem == Operator");		
//--------------GPSWeekSecond_timeSystemTest_5 - Verify TimeSystem=ANY does not matter in TimeSystem=GPS comparisons 
		testFramework.assert(GPS1 == ANY);
		testFramework.next();

//--------------GPSWeekSecond_timeSystemTest_6 - Verify TimeSystem=ANY does not matter in TimeSystem=UTC comparisons 
		testFramework.assert(UTC1 == ANY);
		testFramework.next();

//--------------GPSWeekSecond_timeSystemTest_7 - Verify TimeSystem=ANY does not matter in TimeSystem=UNKOWN comparisons 
		testFramework.assert(UNKNOWN == ANY);
		testFramework.next();

		testFramework.changeSourceMethod("ANY TimeSystem < Operator");	
//--------------GPSWeekSecond_timeSystemTest_8 - Verify TimeSystem=ANY does not matter in other operator comparisons 
		testFramework.assert(!(GPS2 == ANY) && (GPS2 < ANY));
		testFramework.next();

		testFramework.changeSourceMethod("setTimeSystem");
			
  		UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem

//--------------GPSWeekSecond_timeSystemTest_9 - Ensure resetting a Time System changes it
		testFramework.assert(UNKNOWN.getTimeSystem()==TimeSystem(2));

		return testFramework.countFails();
	}
	/* Test for the formatted printing of GPSWeekSecond objects */
	int  printfTest (void)
	{
		TestUtil testFramework( "GPSWeekSecond", "printf", __FILE__, __func__ );
		testFramework.init();

  		GPSWeekSecond GPS1(1300,13500.,TimeSystem(2));
  		GPSWeekSecond UTC1(1300,13500.,TimeSystem(7));
		
//--------------GPSWeekSecond_printfTest_1 - Verify printed output matches expectation
		testFramework.assert(GPS1.printf("%04F %05g %02P") == (std::string)"1300 13500.000000 GPS");
		testFramework.next();

//--------------GPSWeekSecond_printfTest_2 - Verify printed output matches expectation
		testFramework.assert(UTC1.printf("%04F %05g %02P") == (std::string)"1300 13500.000000 UTC");
		testFramework.next();

		testFramework.changeSourceMethod("printError");

//--------------GPSWeekSecond_printfTest_3 - Verify printed error message matches expectation
		testFramework.assert(GPS1.printError("%04F %05z %02P") == (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime");
		testFramework.next();

//--------------GPSWeekSecond_printfTest_4 - Verify printed error message matches expectation
		testFramework.assert(UTC1.printError("%04F %05z %02P") == (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime");

		return testFramework.countFails();
	}
	private:
		double eps;
};


int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	GPSWeekSecond_T testClass;

	check = testClass.initializationTest();
	errorCounter += check;

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
