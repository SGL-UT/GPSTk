//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//	Add grepable comments for each case below
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "TimeConverters.hpp"
#include "TestUtil.hpp"
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
			TestUtil testFramework( "TimeConverters", "JDtoCalendar", __FILE__, __func__ );
			testFramework.init();

			int year;
			int month;
			int day;
			
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	Why pick these values for the conversion?
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//	Comments are repetative, need to be more descriptive based on what the
// 	conversion is testing
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

			convertJDtoCalendar(2453971,year,month,day);

//--------------TimeConverters_JDtoCalandarTest_1 - Was the year value set to expectation?			
			testFramework.assert((2006 == year);
			testFramework.next();

//--------------TimeConverters_JDtoCalandarTest_2 - Was the month value set to expectation?			
			testFramework.assert(8 == month);
			testFramework.next();

//--------------TimeConverters_JDtoCalandarTest_3 - Was the day value set to expectation?			
			testFramework.assert(23 == day);
			testFramework.next();

			convertJDtoCalendar(2299159,year,month,day);

//--------------TimeConverters_JDtoCalandarTest_4 - Was the year value set to expectation?			
			testFramework.assert((1582 == year);
			testFramework.next();

//--------------TimeConverters_JDtoCalandarTest_5 - Was the month value set to expectation?			
			testFramework.assert(10 == month);
			testFramework.next();

//--------------TimeConverters_JDtoCalandarTest_6 - Was the day value set to expectation?			
			testFramework.assert(3 == day);
			testFramework.next();

			convertJDtoCalendar(2342032,year,month,day);

//--------------TimeConverters_JDtoCalandarTest_7 - Was the year value set to expectation?			
			testFramework.assert((1700 == year);
			testFramework.next();

//--------------TimeConverters_JDtoCalandarTest_8 - Was the month value set to expectation?			
			testFramework.assert(3 == month);
			testFramework.next();

//--------------TimeConverters_JDtoCalandarTest_9 - Was the day value set to expectation?			
			testFramework.assert(1 == day);
			testFramework.next();

			convertJDtoCalendar(2377095,year,month,day);

//--------------TimeConverters_JDtoCalandarTest_10 - Was the year value set to expectation?			
			testFramework.assert((1796 == year);
			testFramework.next();

//--------------TimeConverters_JDtoCalandarTest_11 - Was the month value set to expectation?			
			testFramework.assert(2 == month);
			testFramework.next();

//--------------TimeConverters_JDtoCalandarTest_12 - Was the day value set to expectation?			
			testFramework.assert(29 == day);
			testFramework.next();

			convertJDtoCalendar(1721118,year,month,day);

//--------------TimeConverters_JDtoCalandarTest_13 - Was the year value set to expectation?			
			testFramework.assert((-1 == year);
			testFramework.next();

//--------------TimeConverters_JDtoCalandarTest_14 - Was the month value set to expectation?			
			testFramework.assert(3 == month);
			testFramework.next();

//--------------TimeConverters_JDtoCalandarTest_15 - Was the day value set to expectation?			
			testFramework.assert(1 == day);
			testFramework.next();

			convertJDtoCalendar(1721424,year,month,day);

//--------------TimeConverters_JDtoCalandarTest_16 - Was the year value set to expectation?			
			testFramework.assert((1 == year);
			testFramework.next();

//--------------TimeConverters_JDtoCalandarTest_17 - Was the month value set to expectation?			
			testFramework.assert(1 == month);
			testFramework.next();

//--------------TimeConverters_JDtoCalandarTest_18 - Was the day value set to expectation?			
			testFramework.assert(1 == day);
			testFramework.next();

			convertJDtoCalendar(1648549,year,month,day);

//--------------TimeConverters_JDtoCalandarTest_19 - Was the year value set to expectation?			
			testFramework.assert((-200 == year);
			testFramework.next();

//--------------TimeConverters_JDtoCalandarTest_20 - Was the month value set to expectation?			
			testFramework.assert(6 == month);
			testFramework.next();

//--------------TimeConverters_JDtoCalandarTest_21 - Was the day value set to expectation?			
			testFramework.assert(25 == day);

			return testFramework.countFails();
		}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//	Add grepable comments for each case
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		// Calendar to JD tests
		int CalendartoJDTest()
		{
			TestUtil testFramework( "TimeConverters", "CalendartoJD", __FILE__, __func__ );
			testFramework.init();

			long JD;
			int year, month, day;

			year = 2006;
			month = 8;
			day = 23;
			JD = convertCalendarToJD(year,month,day);

			testFramework.assert((long int)2453971 == JD);
			testFramework.next();

			year = 1582;
			month = 10;
			day = 3;
			JD = convertCalendarToJD(year,month,day);	
			testFramework.assert((long int)2299159 == JD);
			testFramework.next();

			year = 1700;
			month = 3;
			day = 1;
			JD = convertCalendarToJD(year,month,day);		
			testFramework.assert((long int)2342032 == JD);
			testFramework.next();

			year = 0;
			month = 3;
			day = 1;
			JD = convertCalendarToJD(year,month,day);	
			testFramework.assert((long int)1721118 == JD);
			testFramework.next();

			year = -5;
			month = 3;
			day = 1;
			JD = convertCalendarToJD(year,month,day);
			testFramework.assert((long int)1719657 == JD);
			testFramework.next();

			year = -200;
			month = 6;
			day = 25;
			JD = convertCalendarToJD(year,month,day);
			testFramework.assert((long int)1648549 == JD);

			return testFramework.countFails();
		}

		// Seconds of Day (SOD) to Time Tests
		int SODtoTimeTest()
		{
			TestUtil testFramework( "TimeConverters", "SODtoTime", __FILE__, __func__ );
			testFramework.init();

			int hour;
			int minute;
			double second;

			//Wraps Backwards
			convertSODtoTime(-.01,hour,minute,second);

			testFramework.assert(23 == hour);
			testFramework.next();

			testFramework.assert(59 == minute);
			testFramework.next();

			testFramework.assert(fabs(59.99-second) < eps);
			testFramework.next();

			//Wraps Forwards
			convertSODtoTime(24*60*60+1.11,hour,minute,second);
			
			testFramework.assert(0 == hour);
			testFramework.next();

			testFramework.assert(0 == minute);
			testFramework.next();

			testFramework.assert(fabs(1.11 - second) < eps);
			testFramework.next();

			//Standard
			convertSODtoTime(12345.67,hour,minute,second);
			
			testFramework.assert(3 == hour);
			testFramework.next();

			testFramework.assert(25 == minute);
			testFramework.next();

			testFramework.assert(fabs(45.67 - second) < eps);

			return testFramework.countFails();
		}


		// Time to SOD Tests
		int TimetoSODTest()
		{
			int hour, minute;
			double second, SOD;

			TestUtil testFramework( "TimeConverters", "SODtoTime", __FILE__, __func__ );
			testFramework.init();

			hour = 1;
			minute = 10;
			second = 30.5;
			SOD = convertTimeToSOD(hour,minute,second);

			testFramework.assert(fabs(60*60+600+30.5 - SOD) < eps);
			testFramework.next();

			hour = 23;
			minute = 59;
			second = 59.99;
			SOD = convertTimeToSOD(hour,minute,second);
			testFramework.assert(fabs(23*3600+59*60+59.99 - SOD) < eps);

			return testFramework.countFails();
		}
	private:
		double eps;
};

int main() //Main function to initialize and run all tests above
{
	int check, errorCounter = 0;
	xTimeConverters testClass;

	check = testClass.JDtoCalendarTest();
    errorCounter += check;

	check = testClass.CalendartoJDTest();
	errorCounter += check;

	check = testClass.SODtoTimeTest();
	errorCounter += check;

	check = testClass.TimetoSODTest(); 
	errorCounter += check;
	
	std::cout << "Total Errors for " << __FILE__<<": "<< errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
