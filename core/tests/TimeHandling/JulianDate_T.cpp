#include "JulianDate.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace gpstk;
using namespace std;


class JulianDate_T
{
	public:
	JulianDate_T() {eps = 1E-12;}
	~JulianDate_T() {}

	/* Test to ensure the values in the constructor go to their intended locations */
	int  initializationTest (void)
	{
		TestUtil testFramework( "JulianDate", "Constructor(jd,TimeSystem)", __FILE__, __func__ );
		testFramework.init();

	  	JulianDate Compare(1350000,TimeSystem(2)); //Initialize an object

//--------------JulianDate_initializationTest_1 - Was the jd value set to expectation?
		//Compare.jd==135000
		testFramework.assert(1350000 - Compare.jd < eps && Compare.jd - 1350000 < eps);
		testFramework.next();

//--------------JulianDate_initializationTest_2 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Compare.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("Constructor(JulianDate)");
		JulianDate Copy(Compare); // Initialize with copy constructor

//--------------JulianDate_initializationTest_3 - Was the jd value set to expectation?
		//Compare.jd==135000
		testFramework.assert(1350000-Copy.jd < eps && Copy.jd - 1350000 < eps);
		testFramework.next();

//--------------JulianDate_initializationTest_4 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Copy.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("= Operator");
		JulianDate Assigned;
		Assigned = Compare;

//--------------JulianDate_initializationTest_5 - Was the jd value set to expectation?
		//Compare.jd==135000
		testFramework.assert(1350000-Assigned.jd < eps && Assigned.jd - 1350000 < eps);
		testFramework.next();

//--------------JulianDate_initializationTest_6 - Was the time system set to expectation?
		testFramework.assert(TimeSystem(2) == Assigned.getTimeSystem());

		return testFramework.countFails();
	}

	/* Test will check if JulianDate variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */

	int setFromInfoTest (void)
	{

		TestUtil testFramework( "JulianDate", "setFromInfo", __FILE__, __func__ );
		testFramework.init();

		JulianDate setFromInfo1;
		JulianDate setFromInfo2;
		JulianDate Compare(1350000,TimeSystem(2)), Compare2(0,TimeSystem(2));
		TimeTag::IdToValue Id;
		Id['J'] = "1350000";
		Id['P'] = "GPS";

//--------------JulianDate_setFromInfoTest_1 - Does a proper setFromInfo work with all information provided?
		testFramework.assert(setFromInfo1.setFromInfo(Id));
		testFramework.next();

//--------------JulianDate_setFromInfoTest_2 - Did the setFromInfo set the proper values?
		testFramework.assert(Compare == setFromInfo1);
		testFramework.next();

		Id.erase('J');
//--------------JulianDate_setFromInfoTest_3 - Does a proper setFromInfo work with missing information?
		testFramework.assert(setFromInfo2.setFromInfo(Id));
		testFramework.next();

//--------------JulianDate_setFromInfoTest_4 - Did the previous setFromInfo set the proper values?
		testFramework.assert(Compare2 == setFromInfo2);		

		return testFramework.countFails();
	}

	/* Test will check if the ways to initialize and set an JulianDate object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{
		TestUtil testFramework( "JulianDate", "== Operator", __FILE__, __func__ );
		testFramework.init();

		JulianDate Compare(1350000); // Initialize with value
		JulianDate LessThanJD(134000); // Initialize with value
		JulianDate CompareCopy(Compare); // Initialize with copy constructor
		JulianDate CompareCopy2; //Empty initialization
		CompareCopy2 = CompareCopy; //Assignment

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	Why have CompareCopy2? Never called. Also, LessThanZCount was only called once
//	in the original test. Shouldn't it be called more for more rigorous testing?
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//--------------JulianDate_operatorTest_1 - Are equivalent objects equivalent?
		testFramework.assert(Compare == CompareCopy);
		testFramework.next();

//--------------JulianDate_operatorTest_2 - Are equivalent objects equivalent?
		testFramework.assert(!(Compare == LessThanJD));
		testFramework.next();

		testFramework.changeSourceMethod("!= Operator");
//--------------JulianDate_operatorTest_3 - Are non-equivalent objects not equivalent?
		testFramework.assert(Compare != LessThanJD);
		testFramework.next();

//--------------JulianDate_operatorTest_4 - Are equivalent objects not equivalent?
		testFramework.assert(!(Compare != Compare));
		testFramework.next();

		testFramework.changeSourceMethod("< Operator");
//--------------JulianDate_operatorTest_5 - Does the < operator function when left_object < right_object?
		testFramework.assert(LessThanJD < Compare);
		testFramework.next();

//--------------JulianDate_operatorTest_6 - Does the < operator function when left_object > right_object?
		testFramework.assert(!(Compare < LessThanJD));
		testFramework.next();

//--------------JulianDate_operatorTest_7 - Does the < operator function when left_object = right_object?
		testFramework.assert(!(Compare < CompareCopy));
		testFramework.next();

		testFramework.changeSourceMethod("> Operator");
//--------------JulianDate_operatorTest_8 - Does the > operator function when left_object < right_object?
		testFramework.assert(!(LessThanJD > Compare));
		testFramework.next();

//--------------JulianDate_operatorTest_9 - Does the > operator function when left_object > right_object?
		testFramework.assert(Compare > LessThanJD);
		testFramework.next();

//--------------JulianDate_operatorTest_10 - Does the > operator function when left_object = right_object?
		testFramework.assert(!(Compare > CompareCopy));
		testFramework.next();

		testFramework.changeSourceMethod("<= Operator");
//--------------JulianDate_operatorTest_11 - Does the <= operator function when left_object < right_object?
		testFramework.assert(LessThanJD <= Compare);
		testFramework.next();

//--------------JulianDate_operatorTest_12 - Does the <= operator function when left_object > right_object?
		testFramework.assert(!(Compare <= LessThanJD));
		testFramework.next();

//--------------JulianDate_operatorTest_13 - Does the <= operator function when left_object = right_object?
		testFramework.assert(Compare <= CompareCopy);
		testFramework.next();

		testFramework.changeSourceMethod(">= Operator");
//--------------JulianDate_operatorTest_14 - Does the >= operator function when left_object < right_object?
		testFramework.assert(!(LessThanJD >= Compare));
		testFramework.next();

//--------------JulianDate_operatorTest_15 - Does the >= operator function when left_object > right_object?
		testFramework.assert(Compare >= LessThanJD);
		testFramework.next();

//--------------JulianDate_operatorTest_16 - Does the >= operator function when left_object = right_object?
		testFramework.assert(Compare >= CompareCopy);

		return testFramework.countFails();
	}

		/* Test will check the reset method. */
	int resetTest (void)
	{
		TestUtil testFramework( "JulianDate", "reset", __FILE__, __func__ );
		testFramework.init();

	  	JulianDate Compare(1350000,TimeSystem(2)); //Initialize an object

	  	Compare.reset(); // Reset it

//--------------JulianDate_resetTest_1 - Was the jd value reset to expectation?
		testFramework.assert(Compare.jd==0);
		testFramework.next();

//--------------JulianDate_resetTest_2 - Was the time system reset to expectation?
		testFramework.assert(TimeSystem(0) == Compare.getTimeSystem());

		return testFramework.countFails();
	}
	/* Test will check converting to/from CommonTime. */
	int  toFromCommonTimeTest (void)
	{
		TestUtil testFramework( "JulianDate", "isValid", __FILE__, __func__ );
		testFramework.init();

	  	JulianDate Compare(1350000,TimeSystem(2)); //Initialize an object

//--------------JulianDate_toFromCommonTimeTest_1 - Is the time after the BEGINNING_OF_TIME?
  		testFramework.assert(Compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);
		testFramework.next();

//--------------JulianDate_toFromCommonTimeTest_2 - Is the set object valid?
		testFramework.assert(Compare.isValid());
		testFramework.next();

  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

  		JulianDate Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

		testFramework.changeSourceMethod("CommonTime Conversion");
//--------------JulianDate_toFromCommonTimeTest_3 - Is the result of conversion the same?
		testFramework.assert(Test2 == Compare);
		testFramework.next();

//--------------JulianDate_toFromCommonTimeTest_4 - Is the time system after conversion what is expected?
		testFramework.assert(Compare.getTimeSystem()==TimeSystem(2));
		testFramework.next();

//--------------JulianDate_toFromCommonTimeTest_5 - Is the time after conversion what is expected?
		//Compare.jd==0
		testFramework.assert(1350000 - Compare.jd < eps && Compare.jd - 1350000 < eps);

		return testFramework.countFails();
	}
	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int  timeSystemTest (void)
	{
		TestUtil testFramework( "JulianDate", "Differing TimeSystem == Operator", __FILE__, __func__ );
		testFramework.init();

  		JulianDate GPS1(1350000,TimeSystem(2));
  		JulianDate GPS2(1340000,TimeSystem(2));
  		JulianDate UTC1(1350000,TimeSystem(7));
  		JulianDate UNKNOWN(1350000,TimeSystem(0));
  		JulianDate ANY(1350000,TimeSystem(1));

//--------------JulianDate_timeSystemTest_1 - Verify same Time System but different time inequality
		testFramework.assert(!(GPS1 == GPS2));
		testFramework.next();

//--------------JulianDate_timeSystemTest_2 - Verify same Time System equality
		testFramework.assert(GPS1.getTimeSystem() == GPS2.getTimeSystem());
		testFramework.next();

		testFramework.changeSourceMethod("Differing TimeSystem != Operator");
//--------------JulianDate_timeSystemTest_3 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UTC1);
		testFramework.next();

//--------------JulianDate_timeSystemTest_4 - Verify different Time System but same time inequality
		testFramework.assert(GPS1 != UNKNOWN);
		testFramework.next();

		testFramework.changeSourceMethod("ANY TimeSystem == Operator");		
//--------------JulianDate_timeSystemTest_5 - Verify TimeSystem=ANY does not matter in TimeSystem=GPS comparisons 
		testFramework.assert(GPS1 == ANY);
		testFramework.next();

//--------------JulianDate_timeSystemTest_6 - Verify TimeSystem=ANY does not matter in TimeSystem=UTC comparisons 
		testFramework.assert(UTC1 == ANY);
		testFramework.next();

//--------------JulianDate_timeSystemTest_7 - Verify TimeSystem=ANY does not matter in TimeSystem=UNKOWN comparisons 
		testFramework.assert(UNKNOWN == ANY);
		testFramework.next();

		testFramework.changeSourceMethod("ANY TimeSystem < Operator");	
//--------------JulianDate_timeSystemTest_8 - Verify TimeSystem=ANY does not matter in other operator comparisons 
		testFramework.assert(!(GPS2 == ANY) && (GPS2 < ANY));
		testFramework.next();

		testFramework.changeSourceMethod("setTimeSystem");	
  		UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem
//--------------JulianDate_timeSystemTest_9 - Ensure resetting a Time System changes it
		testFramework.assert(UNKNOWN.getTimeSystem()==TimeSystem(2));

		return testFramework.countFails();
	}

	/* Test for the formatted printing of JulianDate objects */
	int  printfTest (void)
	{
		TestUtil testFramework( "JulianDate", "printf", __FILE__, __func__ );
		testFramework.init();

  		JulianDate GPS1(1350000,TimeSystem(2));
  		JulianDate UTC1(1350000,TimeSystem(7));

//--------------JulianDate_printfTest_1 - Verify printed output matches expectation
		testFramework.assert(GPS1.printf("%08J %02P") == (std::string)"1350000.000000 GPS");
		testFramework.next();

//--------------JulianDate_printfTest_2 - Verify printed output matches expectation
		testFramework.assert(UTC1.printf("%08J %02P") == (std::string)"1350000.000000 UTC");
		testFramework.next();

		testFramework.changeSourceMethod("printError");	
//--------------JulianDate_printfTest_3 - Verify printed error message matches expectation
		testFramework.assert(GPS1.printError("%08J %02P") == (std::string)"ErrorBadTime ErrorBadTime");
		testFramework.next();

//--------------JulianDate_printfTest_4 - Verify printed error message matches expectation
		testFramework.assert(UTC1.printError("%08J %02P") == (std::string)"ErrorBadTime ErrorBadTime");

		return testFramework.countFails();
	}

	private:
		double eps;
};


int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	JulianDate_T testClass;

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
