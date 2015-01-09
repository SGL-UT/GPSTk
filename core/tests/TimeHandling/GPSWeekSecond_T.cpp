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


	/* Test will check if GPSWeekSecond variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */
	int setFromInfoTest (void)
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

	}

	/* Test will check if the ways to initialize and set an GPSWeekSecond object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{

		GPSWeekSecond Compare(1300,13500.);
		GPSWeekSecond LessThanWeek(1299,13500.);
		GPSWeekSecond LessThanSecond(1300,13400.);
		GPSWeekSecond CompareCopy(Compare); // Initialize with copy constructor
		GPSWeekSecond CompareCopy2; //Empty initialization
		CompareCopy2 = CompareCopy; //Assignment

		//Equality testAssertion
		if (!(Compare == CompareCopy)) return 1;
		//Non-equality testAssertion
		if (!(Compare != LessThanWeek)) return 2;
		//Less than testAssertions
		if (!(LessThanWeek < Compare)) return 3;
		if (Compare < LessThanWeek) return 4;
		if (!(LessThanSecond < Compare)) return 5;
		if (Compare < LessThanSecond) return 6;
		//Greater than testAssertions
		if(!(Compare > LessThanWeek)) return 7;
		//Less than equals testAssertion
		if (!(LessThanWeek <= Compare)) return 8;
		if(!(CompareCopy <= Compare)) return 9;
		//Greater than equals testAssertion
		if(!(Compare >= LessThanWeek)) return 10;
		if(!(Compare >= CompareCopy)) return 11;
		//Validity check
		if(!(Compare.isValid())) return 12;
		return 0;
	}

	/* Test will check the reset method. */
	int  resetTest (void)
	{
	
		GPSWeekSecond Compare(1300,13500.,TimeSystem::GPS); //Initialize an object

		if (1300 != Compare.week) return 1;
		if (13500 != Compare.sow) return 2;
		if (TimeSystem(2) != Compare.getTimeSystem()) return 3;

	  	Compare.reset(); // Reset it

		/*GPSWeekSecond resets to GPS TimeSystem!!!!*/
	  	if (TimeSystem(2) != Compare.getTimeSystem()) return 4; 
	  	if (0 != (int)Compare.week) return 5;
	  	if (0 != (int)Compare.sow) return 6;
		return 0;
	}

	/* Test will check converting to/from CommonTime. */
	int  toFromCommonTimeTest (void)
	{
	  	GPSWeekSecond Compare(1300,13500.,TimeSystem(2)); //Initialize an object
  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

  		GPSWeekSecond Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

  		if (!(Test2 == Compare)) return 1; // Converting to then from yields original

  		if (!(Compare.getTimeSystem()==TimeSystem(2))) return 2; // Recheck TimeSystem
  		if (13500 != (int)Test2.sow) return 3; // Recheck value
		if (1300 != Test2.week) return 4;
		return 0;
	}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int  timeSystemTest (void)
	{

  		GPSWeekSecond GPS1(1300,13500.,TimeSystem(2));
  		GPSWeekSecond GPS2(1200,13500.,TimeSystem(2));
  		GPSWeekSecond UTC1(1300,13500.,TimeSystem(5));
  		GPSWeekSecond UNKNOWN(1300,13500.,TimeSystem(0));
  		GPSWeekSecond ANY(1300,13500.,TimeSystem(1));

  		if (GPS1 == GPS2) return 1; // GPS1 and GPS2 should have different times
  		if (GPS1.getTimeSystem() != GPS2.getTimeSystem()) return 2; // Should have the same time system
  		if (GPS1 == UTC1) return 3; //Should have different time systems
  		if (GPS1 == UNKNOWN) return 4;

		// Perform comparisons to start of CommonTime
  		if (GPS1.convertToCommonTime() < CommonTime::BEGINNING_OF_TIME) return 11;
  		if (CommonTime::BEGINNING_OF_TIME > GPS1) return 12;
		
		// Make TimeSystem part not matter and perform comparisons
		// which solely depend on the time value.
  		if (GPS1 != ANY) return 5; 
  		if (UTC1 != ANY) return 6;
  		if (UNKNOWN != ANY) return 7;
  		if (GPS2 == ANY) return 8;
  		if (GPS2 > GPS1) return 9;
  		if (GPS2 > ANY) return 10;

  		UNKNOWN.setTimeSystem(TimeSystem(2)); //Set the Unknown TimeSystem
  		if (UNKNOWN.getTimeSystem()!=TimeSystem(2)) return 13;
		return 0;
	}
	/* Test for the formatted printing of GPSWeekSecond objects */
	int  printfTest (void)
	{

  		GPSWeekSecond GPS1(1300,13500.,TimeSystem(2));
  		GPSWeekSecond UTC1(1300,13500.,TimeSystem(7));
		
  		if (GPS1.printf("%04F %05g %02P") != (std::string)"1300 13500.000000 GPS") return 1;
  		if (UTC1.printf("%04F %05g %02P") != (std::string)"1300 13500.000000 UTC") return 2;
		//std::cout << GPS1.printError("%04F %05g %02P") << std::endl;
  		if (GPS1.printError("%04F %05g %02P") != (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime") return 3; 
  		if (UTC1.printError("%04F %05g %02P") != (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime") return 4;
		return 0;
	}
	private:
		double eps;
};

void checkResult(int check, int& errCount) // Function to handle test result output
{
	if (check == -1)
	{
		std::cout << "DIDN'T RUN!!!!" << std::endl;
	}
	else if (check == 0 )
	{
		std::cout << "GOOD!!!!" << std::endl;
	}
	else if (check > 0)
	{
		std::cout << "BAD!!!!" << std::endl;
		std::cout << "Error Message for Bad Test is Code " << check << std::endl;
		errCount++;
	}
}

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	GPSWeekSecond_T testClass;
	check = testClass.operatorTest();
        std::cout << "opertatorTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.setFromInfoTest();
        std::cout << "setFromInfoTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.resetTest();
        std::cout << "resetTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.timeSystemTest();
        std::cout << "timeSystemTest Result is: "; 
	checkResult(check, errorCounter);
	check = -1;
	check = testClass.toFromCommonTimeTest();
        std::cout << "toFromCommonTimeTest Result is: "; 
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.printfTest();
        std::cout << "printfTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;
	
	std::cout << "Total Errors: " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
