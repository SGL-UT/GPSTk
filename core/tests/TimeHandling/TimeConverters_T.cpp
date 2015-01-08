#include "TimeConverters.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
using namespace gpstk;
using namespace std;

class xTimeConverters
{
	public:
		xTimeConverters(){eps = 1e-11;}// Default Constructor, set the precision value
		~xTimeConverters() {} // Default Desructor
		// Julian Date (JD) to Calendar Date Tests
		int JDtoCalendarTest()
		{
			int year;
			int month;
			int day;
	
			convertJDtoCalendar(2453971,year,month,day);
			if (2006 != year) return 1;
			if (8 != month) return 2;
			if (23 != day) return 3;
			convertJDtoCalendar(2299159,year,month,day);
			if (1582 != year) return 4;
			if (10 != month) return 5;
			if (3 != day) return 6;
			convertJDtoCalendar(2342032,year,month,day);
			if (1700 != year) return 7;
			if (3 != month) return 8;
			if (1 != day) return 9;
			convertJDtoCalendar(2377095,year,month,day);
			if (1796 != year) return 10;
			if (2 != month) return 11;
			if (29 != day) return 12;
			convertJDtoCalendar(1721118,year,month,day);
			if (-1 != year) return 13;
			if (3 != month) return 14;
			if (1 != day) return 15;
			convertJDtoCalendar(1721424,year,month,day);
			if (1 != year) return 16;
			if (1 != month) return 17;
			if (1 != day) return 18;		
			convertJDtoCalendar(1648549,year,month,day);
			if (-200 != year) return 19;
			if (6 != month) return 20;
			if (25 != day) return 21;
			return 0;
		}

		// Calendar to JD tests
		int CalendartoJDTest()
		{
			long JD;
			int year, month, day;

			year = 2006;
			month = 8;
			day = 23;
			JD = convertCalendarToJD(year,month,day);
			if ((long int)2453971 != JD) return 1;

			year = 1582;
			month = 10;
			day = 3;
			JD = convertCalendarToJD(year,month,day);	
			if ((long int)2299159 != JD) return 2;

			year = 1700;
			month = 3;
			day = 1;
			JD = convertCalendarToJD(year,month,day);		
			if ((long int)2342032 != JD) return 3;

			year = 0;
			month = 3;
			day = 1;
			JD = convertCalendarToJD(year,month,day);	
			if ((long int)1721118 != JD) return 4;

			year = -5;
			month = 3;
			day = 1;
			JD = convertCalendarToJD(year,month,day);
			if ((long int)1719657 != JD) return 5;

			year = -200;
			month = 6;
			day = 25;
			JD = convertCalendarToJD(year,month,day);
			if ((long int)1648549 != JD) return 6;
			return 0;
		}

		// Seconds of Day (SOD) to Time Tests
		int SODtoTimeTest()
		{
			int hour;
			int minute;
			double second;

			//Wraps Backwards
			convertSODtoTime(-.01,hour,minute,second);
			if (23 != hour) return 1;
			if (59 != minute) return 2;
			if (fabs(59.99-second) > eps) return 3;

			//Wraps Forwards
			convertSODtoTime(24*60*60+1.11,hour,minute,second);
			if (0 != hour) return 4;
			if (0 != minute) return 5;
			if (fabs(1.11 - second) > eps) return 6;

			//Standard
			convertSODtoTime(12345.67,hour,minute,second);
			if (3 != hour) return 7;
			if (25 != minute) return 8;
			if (fabs(45.67 - second) > eps) return 9;

			return 0;
		}


		// Time to SOD Tests
		int TimetoSODTest()
		{
			int hour, minute;
			double second, SOD;

			hour = 1;
			minute = 10;
			second = 30.5;
			SOD = convertTimeToSOD(hour,minute,second);
			if (fabs(60*60+600+30.5 - SOD) > eps) return 1;

			hour = 23;
			minute = 59;
			second = 59.99;
			SOD = convertTimeToSOD(hour,minute,second);
			if (fabs(23*3600+59*60+59.99 - SOD) > eps) return 2;

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
	xTimeConverters testClass;

	check = testClass.JDtoCalendarTest();
        std::cout << "JDtoCalendarTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.CalendartoJDTest();
        std::cout << "CalendartoJDTest Result is: ";
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.SODtoTimeTest();
        std::cout << "SODtoTime Result is: "; 
	checkResult(check, errorCounter);
	check = -1;

	check = testClass.TimetoSODTest();
        std::cout << "TimetoSOD Result is: "; 
	checkResult(check, errorCounter);
	check = -1;
	
	std::cout << "Total Errors: " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
