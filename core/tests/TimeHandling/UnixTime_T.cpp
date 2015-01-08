#include "UnixTime.hpp"
#include "TimeTag.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

class xUnixTime
{
	public:

	/* Test will check if UnixTime variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */
	int setFromInfoTest (void)
	{
		UnixTime setFromInfo1;
		UnixTime setFromInfo2;
		UnixTime Compare(1350000,1,TimeSystem(2)),Compare2(0,1,TimeSystem(2));
		TimeTag::IdToValue Id;
		Id.insert(make_pair('U',"1350000"));
		Id.insert(make_pair('u',"1"));
		Id.insert(make_pair('P',"GPS"));
		if (!setFromInfo1.setFromInfo(Id)) return 1;
		if (setFromInfo1 != Compare) return 2;
		Id.erase('U');
		if(!setFromInfo2.setFromInfo(Id)) return 3;
		if (setFromInfo2 != Compare2) return 4;
		return 0;
	}

	/* Test will check if the ways to initialize and set an UnixTime object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{

		UnixTime Compare(1350000,100); // Initialize with value
		UnixTime LessThanSec(1340000, 100); //Initialize with fewer seconds
		UnixTime LessThanMicroSec(1350000,0); //Initialize with fewer microseconds
		UnixTime CompareCopy(Compare); // Initialize with copy constructor
		UnixTime CompareCopy2; //Empty initialization
		CompareCopy2 = CompareCopy; //Assignment

		//Equality Assertion
		if (!(Compare == CompareCopy)) return 1;
		//Non-equality Assertion
		if (!(Compare != LessThanSec)) return 2;
		//Less than assertions
		if (!(LessThanSec < Compare)) return 4;
		if (!(LessThanMicroSec < Compare)) return 5;
		if (Compare < LessThanSec) return 6;
		//Greater than assertions
		if(!(Compare > LessThanSec)) return 7;
		//Less than equals assertion
		if (!(LessThanSec <= Compare)) return 8;
		if(!(CompareCopy <= Compare)) return 9;
		//Greater than equals assertion
		if(!(Compare >= LessThanSec)) return 10;
		if(!(Compare >= CompareCopy)) return 11;
		//Validity check
		if(!(Compare.isValid())) return 12;
		return 0;
	}

	/* Test will check the reset method. */
	int  resetTest (void)
	{
	
	  	UnixTime Compare(1350000,0,TimeSystem(2)); //Initialize an object
		//Verify correct initialization
  		if (!(Compare.getTimeSystem()==TimeSystem(2))) return 1;
		if (1350000 != (int)Compare.tv.tv_sec) return 2; 
		if (0 != (int)Compare.tv.tv_usec) return 3;

	  	Compare.reset(); // Reset it
	  	if (TimeSystem(0) != Compare.getTimeSystem()) return 4; 
		if (0 != (int)Compare.tv.tv_sec) return 5;
		if (0 != (int)Compare.tv.tv_usec) return 6;
		return 0;
	}

	/* Test will check converting to/from CommonTime. */
	int  toFromCommonTimeTest (void)
	{
	  	UnixTime Compare(1350000,0,TimeSystem(2)); //Initialize an object
  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

  		UnixTime Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

  		if (!(Test2 == Compare)) return 1; // Converting to then from yields original
		// Recheck Values
  		if (!(Compare.getTimeSystem()==TimeSystem(2))) return 2; 
		if (1350000 != (int)Compare.tv.tv_sec) return 3; 
		if (0 != (int)Compare.tv.tv_usec) return 4;
		return 0;
	}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int  timeSystemTest (void)
	{

		UnixTime GPS1(1350000,0,TimeSystem::GPS);
		UnixTime GPS2(1340000,0,TimeSystem::GPS);
		UnixTime UTC1(1350000,0,TimeSystem::UTC);
		UnixTime UNKNOWN(1350000,0,TimeSystem::Unknown);
		UnixTime ANY(1350000,0,TimeSystem::Any);

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
	/* Test for the formatted printing of UnixTime objects */
	int  printfTest (void)
	{

  		UnixTime GPS1(1350000,0,TimeSystem(2));
  		UnixTime UTC1(1350000,0,TimeSystem(7));
		
  		if (GPS1.printf("%07U %02u %02P") != (std::string)"1350000 00 GPS") return 1;
  		if (UTC1.printf("%07U %02u %02P") != (std::string)"1350000 00 UTC") return 2;
  		if (GPS1.printError("%07U %02u %02P") != (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime") return 3; 
  		if (UTC1.printError("%07U %02u %02P") != (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime") return 4;
		return 0;
	}
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
	xUnixTime testClass;
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
