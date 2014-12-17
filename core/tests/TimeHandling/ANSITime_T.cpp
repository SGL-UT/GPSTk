#include "ANSITime.hpp"
#include "TimeTag.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

class xANSITime
{
	public:

	// Test is not currently compiling with Solaris compilers. Leaving it commented for now.

	/* Test will check if ANSITime variable can be set from a map.
	   Test also implicity tests whether the != operator functions. */
	/*int setFromInfoTest (void)
	{
		ANSITime setFromInfo1;
		ANSITime setFromInfo2;
		ANSITime Compare(13500000,TimeSystem(2));
		IdToValue Id;
		Id.insert(make_pair('K',"13500000"));
		Id.insert(make_pair('P',"2"));
		if (!setFromInfo1.setFromInfo(Id)) return 1;
		if (setFromInfo1 != Compare) return 2;
		Id.erase('K');
		if(!setFromInfo2.setFromInfo(Id)) return 3;
		ofstream out("Logs/printfOutput");

		out << setFromInfo1 << endl;
		out << setFromInfo2 << endl;
		return 0;
	}*/

	/* Test will check if the ways to initialize and set an ANSITime object.
	   Test also tests whether the comparison operators and isValid method function. */
	int operatorTest (void)
	{

		ANSITime Compare(13500000); // Initialize with value
		ANSITime LessThan(13400000); // Initialize with value
		ANSITime CompareCopy(Compare); // Initialize with copy constructor
		ANSITime CompareCopy2; //Empty initialization
		CompareCopy2 = CompareCopy; //Assignment

		//Equality Assertion
		if (!(Compare == CompareCopy)) return 1;
		//Non-equality Assertion
		if (!(Compare != LessThan)) return 2;
		//Less than assertions
		if (!(LessThan < Compare)) return 3;
		if (Compare < LessThan) return 4;
		//Greater than assertions
		if(!(Compare > LessThan)) return 5;
		//Less than equals assertion
		if (!(LessThan <= Compare)) return 6;
		if(!(CompareCopy <= Compare)) return 7;
		//Greater than equals assertion
		if(!(Compare >= LessThan)) return 8;
		if(!(Compare >= CompareCopy)) return 9;
		//Validity check
		if(!(Compare.isValid())) return 10;
		return 0;
	}

	/* Test will check the reset method. */
	int  resetTest (void)
	{
	
	  	ANSITime Compare(13500000,TimeSystem(2)); //Initialize an object
		if (13500000 != Compare.time) return 1;
		if (TimeSystem(2) != Compare.getTimeSystem()) return 2;

	  	Compare.reset(); // Reset it
	  	if (TimeSystem(0) != Compare.getTimeSystem()) return 3; 
	  	if (0 != (int)Compare.time) return 4;
		return 0;
	}

	/* Test will check converting to/from CommonTime. */
	int  toFromCommonTimeTest (void)
	{
	  	ANSITime Compare(13500000,TimeSystem(2)); //Initialize an object
  		CommonTime Test = Compare.convertToCommonTime(); //Convert to

  		ANSITime Test2;
  		Test2.convertFromCommonTime(Test); //Convert From

  		if (!(Test2 == Compare)) return 1; // Converting to then from yields original

  		if (!(Compare.getTimeSystem()==TimeSystem(2))) return 2; // Recheck TimeSystem
  		if (13500000 != (int)Compare.time) return 3; // Recheck value
		return 0;
	}

	/* Test will check the TimeSystem comparisons when using the comparison operators. */
	int  timeSystemTest (void)
	{

  		ANSITime GPS1(13500000,TimeSystem(2));
  		ANSITime GPS2(13400000,TimeSystem(2));
  		ANSITime UTC1(13500000,TimeSystem(5));
  		ANSITime UNKNOWN(13500000,TimeSystem(0));
  		ANSITime ANY(13500000,TimeSystem(1));

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
	/* Test for the formatted printing of ANSITime objects */
	int  printfTest (void)
	{

  		ANSITime GPS1(13500000,TimeSystem(2));
  		ANSITime UTC1(13500000,TimeSystem(5));
		
  		if (GPS1.printf("%08K %02P") != (std::string)"13500000 GPS") return 1;
  		if (UTC1.printf("%08K %02P") != (std::string)"13500000 UTC") return 2;
  		if (GPS1.printError("%08K %02P") != (std::string)"ErrorBadTime ErrorBadTime") return 3; 
  		if (UTC1.printError("%08K %02P") != (std::string)"ErrorBadTime ErrorBadTime") return 4;
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
	xANSITime testClass;
	check = testClass.operatorTest();
        std::cout << "opertatorTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	/*check = testClass.setFromInfoTest(); // Not run due to issue with Solaris compiler.
        std::cout << "setFromInfoTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;*/
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
