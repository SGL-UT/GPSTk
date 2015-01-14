//demo rewrite of MJD_T
//formatting to ANSITime_T standard
#include "MJD.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

class MJD_T
{
	public:

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//Include initialization test here 
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


	/* Test will check if MJD variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//Error with "1350000" compared to 1350000?
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	int setFromInfoTest (void)
	{
		TestUtil testFramework( "MJD", "setFromInfo", __FILE__, __func__ );
		testFramework.init();

		MJD setFromInfo1;
		MJD setFromInfo2;
		MJD Compare(135000.0,TimeSystem(2)), Compare2(0.0,TimeSystem(2));
		TimeTag::IdToValue Id;
		Id['Q'] = "135000.0";
		Id['P'] = "GPS";
//--------------JulianDate_setFromInfoTest_1 - Does a proper setFromInfo work with all information provided?
		testFramework.assert(setFromInfo1.setFromInfo(Id));
		testFramework.next();

//--------------JulianDate_setFromInfoTest_2 - Did the setFromInfo set the proper values?
		testFramework.assert(Compare == setFromInfo1);
		testFramework.next();

		Id.erase('Q');

//--------------JulianDate_setFromInfoTest_3 - Does a proper setFromInfo work with missing information?
		testFramework.assert(setFromInfo2.setFromInfo(Id));
		testFramework.next();

//--------------JulianDate_setFromInfoTest_4 - Did the previous setFromInfo set the proper values?
		testFramework.assert(Compare2 == setFromInfo2);		

		return testFramework.countFails();
	}

	/* Test will check if the ways to initialize and set an MJD object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{
		TestUtil testFramework( "JulianDate", "== Operator", __FILE__, __func__ );
		testFramework.init();

		gpstk::MJD Compare(135000); // Initialize with value
		gpstk::MJD LessThanMJD(134000); // Initialize with value
		gpstk::MJD CompareCopy(Compare); // Initialize with copy constructor
		gpstk::MJD CompareCopy2; //Empty initialization
		CompareCopy2 = CompareCopy; //Assignment

//--------------JulianDate_operatorTest_1 - Are equivalent objects equivalent?
		testFramework.assert(Compare == CompareCopy);
		testFramework.next();

//--------------JulianDate_operatorTest_2 - Are equivalent objects equivalent?
		testFramework.assert(!(Compare == LessThanMJD));
		testFramework.next();

		testFramework.changeSourceMethod("!= Operator");
//--------------JulianDate_operatorTest_3 - Are non-equivalent objects not equivalent?
		testFramework.assert(Compare != LessThanMJD);
		testFramework.next();

//--------------JulianDate_operatorTest_4 - Are equivalent objects not equivalent?
		testFramework.assert(!(Compare != Compare));
		testFramework.next();

		testFramework.changeSourceMethod("< Operator");
//--------------JulianDate_operatorTest_5 - Does the < operator function when left_object < right_object?
		testFramework.assert(LessThanMJD < Compare);
		testFramework.next();

//--------------JulianDate_operatorTest_6 - Does the < operator function when left_object > right_object?
		testFramework.assert(!(Compare < LessThanMJD));
		testFramework.next();

//--------------JulianDate_operatorTest_7 - Does the < operator function when left_object = right_object?
		testFramework.assert(!(Compare < CompareCopy));
		testFramework.next();

		testFramework.changeSourceMethod("> Operator");
//--------------JulianDate_operatorTest_8 - Does the > operator function when left_object < right_object?
		testFramework.assert(!(LessThanMJD > Compare));
		testFramework.next();

//--------------JulianDate_operatorTest_9 - Does the > operator function when left_object > right_object?
		testFramework.assert(Compare > LessThanMJD);
		testFramework.next();

//--------------JulianDate_operatorTest_10 - Does the > operator function when left_object = right_object?
		testFramework.assert(!(Compare > CompareCopy));
		testFramework.next();

		testFramework.changeSourceMethod("<= Operator");
//--------------JulianDate_operatorTest_11 - Does the <= operator function when left_object < right_object?
		testFramework.assert(LessThanMJD <= Compare);
		testFramework.next();

//--------------JulianDate_operatorTest_12 - Does the <= operator function when left_object > right_object?
		testFramework.assert(!(Compare <= LessThanMJD));
		testFramework.next();

//--------------JulianDate_operatorTest_13 - Does the <= operator function when left_object = right_object?
		testFramework.assert(Compare <= CompareCopy);
		testFramework.next();

		testFramework.changeSourceMethod(">= Operator");
//--------------JulianDate_operatorTest_14 - Does the >= operator function when left_object < right_object?
		testFramework.assert(!(LessThanMJD >= Compare));
		testFramework.next();

//--------------JulianDate_operatorTest_15 - Does the >= operator function when left_object > right_object?
		testFramework.assert(Compare >= LessThanMJD);
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

	  	MJD Compare(135000,TimeSystem(2)); //Initialize an object

	  	Compare.reset(); // Reset it
//--------------JulianDate_resetTest_1 - Was the mjd value reset to expectation?

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// mjd recast as int! Is that okay? Does 0 as a double not return 0?
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-	  	
	  	testFramework.assert(0 == (int)Compare.mjd);
	  	testFramework.next();

//--------------JulianDate_resetTest_2 - Was the time system reset to expectation?
	  	testFramework.assert(TimeSystem(0) != Compare.getTimeSystem(); 

		return testFramework.countFails();
	}

	/* Test will check converting to/from CommonTime. */
	int toFromCommonTimeTest (void)
	{
		TestUtil testFramework( "JulianDate", "isValid", __FILE__, __func__ );
		testFramework.init();

	  	MJD Compare(135000,TimeSystem(2)); //Initialize an object

//--------------JulianDate_toFromCommonTimeTest_1 - Is the time after the BEGINNING_OF_TIME?
		testFramework.assert(Compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME);
		testFramework.next();

//--------------JulianDate_toFromCommonTimeTest_2 - Is the set object valid?
		testFramework.assert(Compare.isValid());
		testFramework.next();

  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

  		MJD Test2;
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
		testFramework.assert(135000 - Compare.mjd < eps && Compare.mjd - 135000 < eps);

		return testFramework.countFails();
	}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int timeSystemTest (void)
	{
		TestUtil testFramework( "JulianDate", "Differing TimeSystem == Operator", __FILE__, __func__ );
		testFramework.init();

  		MJD GPS1(135000,TimeSystem(2));
  		MJD GPS2(134000,TimeSystem(2));
  		MJD UTC1(135000,TimeSystem(5));
  		MJD UNKNOWN(135000,TimeSystem(0));
  		MJD ANY(135000,TimeSystem(1));

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

	/* Test for the formatted printing of MJD objects */
	int printfTest (void)
	{
		TestUtil testFramework( "JulianDate", "printf", __FILE__, __func__ );
		testFramework.init();

  		MJD GPS1(135000,TimeSystem(2));
  		MJD UTC1(135000,TimeSystem(7));
		
//--------------JulianDate_printfTest_1 - Verify printed output matches expectation
		testFramework.assert(GPS1.printf("%08Q %02P") == (std::string)"135000.000000 GPS");
		testFramework.next();

//--------------JulianDate_printfTest_2 - Verify printed output matches expectation
		testFramework.assert(UTC1.printf("%08Q %02P") = (std::string)"135000.000000 UTC");
		testFramework.next();

		testFramework.changeSourceMethod("printError");	
//--------------JulianDate_printfTest_3 - Verify printed error message matches expectation
		testFramework.assert(GPS1.printError("%08K %02P") == (std::string)"ErrorBadTime ErrorBadTime");
		testFramework.next();

//--------------JulianDate_printfTest_4 - Verify printed error message matches expectation
		testFramework.assert(UTC1.printError("%08K %02P") == (std::string)"ErrorBadTime ErrorBadTime");

		return testFramework.countFails();
	}
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	JulianDate_T testClass;

	check = testClass.initializationTest();
	errorCounter += check;

	check = testClass.operatorTest();
	errorCounter += check;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	check = testClass.setFromInfoTest(); 
// "Not run due to issue with Solaris compiler"
// ????
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	//errorCounter += check;

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
