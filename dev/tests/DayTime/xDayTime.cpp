#pragma ident "$Id$"
#include "xDayTime.hpp"
#include "RinexMetStream.hpp"
#include <iostream>
#include <fstream>

CPPUNIT_TEST_SUITE_REGISTRATION (xDayTime);

using namespace gpstk;

void xDayTime :: setUp (void)
{
}

void xDayTime :: constrTest (void)
{
	gpstk::DayTime CurrentTime(2006,8,21,13,30,0);
	double JD = CurrentTime.JD();
	double MJD = CurrentTime.MJD();
	short year = CurrentTime.year();
	short month = CurrentTime.month();
	short day = CurrentTime.day();
	short dayOfWeek = CurrentTime.dayOfWeek();
	short hour = CurrentTime.hour();
	short minute = CurrentTime.minute();
	double second = CurrentTime.second();
	short GPS10bitweek = CurrentTime.GPS10bitweek();
	long GPSzcount = CurrentTime.GPSzcount();
	long GPSzcountFloor = CurrentTime.GPSzcountFloor();
	double GPSsow = CurrentTime.GPSsow();
	short GPSfullweek = CurrentTime.GPSfullweek();
	short DOY = CurrentTime.DOY();
	double secOfDay = CurrentTime.secOfDay();
	long double MJDasLongDouble = CurrentTime.getMJDasLongDouble();
	gpstk::DayTime unixTime = CurrentTime.unixTime();
	unsigned long fullZcount = CurrentTime.fullZcount();
	unsigned long fullZcountFloor = CurrentTime.fullZcountFloor();
	gpstk::GPSZcount fullZcountOperator = GPSZcount(CurrentTime);
	gpstk::CommonTime commonTimeTime = CommonTime(CurrentTime);
	
	gpstk::DayTime GPSTimeFullWeek(GPSfullweek,GPSsow);
	gpstk::DayTime GPStime(GPS10bitweek,GPSsow,year);
	gpstk::DayTime GPStime2(GPS10bitweek,GPSzcount,year);
	gpstk::DayTime GPStimeZcount(fullZcount);
	gpstk::DayTime GPStimeZcount2(fullZcountOperator);
	gpstk::DayTime commonTime(commonTimeTime);
	gpstk::DayTime calenderTime(year,month,day,hour,minute,second);
	gpstk::DayTime LongModJD(MJDasLongDouble);
	gpstk::DayTime ModJD(MJD);
	gpstk::DayTime DayofYear(year,DOY,secOfDay);
	gpstk::DayTime Unix(unixTime);
	
	CPPUNIT_ASSERT_EQUAL(GPSTimeFullWeek,GPStime);
	CPPUNIT_ASSERT_EQUAL(GPSTimeFullWeek,GPStime2);
	CPPUNIT_ASSERT_EQUAL(GPSTimeFullWeek,GPStimeZcount);
	CPPUNIT_ASSERT_EQUAL(GPSTimeFullWeek,GPStimeZcount2);
	//CPPUNIT_ASSERT_EQUAL(GPSTimeFullWeek,commonTime);
	CPPUNIT_ASSERT_EQUAL(GPSTimeFullWeek,calenderTime);
	CPPUNIT_ASSERT_EQUAL(GPSTimeFullWeek,LongModJD);
	CPPUNIT_ASSERT_EQUAL(GPSTimeFullWeek,ModJD);
	CPPUNIT_ASSERT_EQUAL(GPSTimeFullWeek,DayofYear);
	CPPUNIT_ASSERT_EQUAL(GPSTimeFullWeek,Unix);
}

void xDayTime :: arithmeticTest (void)
{
	gpstk::DayTime CurrentTime(2006,8,21,13,30,0);
	gpstk::DayTime Guess(2006,8,21,13,30,30);
	gpstk::DayTime Guess2(2006,8,21,13,30,0);
	gpstk::DayTime Guess3(2006,8,21,13,31,0);
	gpstk::DayTime GuessMilli(2006,8,21,13,31,30);
	gpstk::DayTime GuessMicro(2006,8,21,13,32,0);
	
	CurrentTime+=30;
	CPPUNIT_ASSERT_EQUAL(Guess,CurrentTime);
	CurrentTime-=30;
	CPPUNIT_ASSERT_EQUAL(Guess2,CurrentTime);
	CPPUNIT_ASSERT_EQUAL(30.,Guess-CurrentTime);
	CurrentTime = CurrentTime + 30;
	CPPUNIT_ASSERT_EQUAL(Guess,CurrentTime);
	CurrentTime = CurrentTime - 30;
	CPPUNIT_ASSERT_EQUAL(Guess2,CurrentTime);
	CurrentTime.addSeconds(30.);
	CPPUNIT_ASSERT_EQUAL(Guess,CurrentTime);
	CurrentTime.addSeconds((long) 30);
	CPPUNIT_ASSERT_EQUAL(Guess3,CurrentTime);
	CurrentTime.addMilliSeconds((long) 30000);
	CPPUNIT_ASSERT_EQUAL(GuessMilli,CurrentTime);
	CurrentTime.addMicroSeconds((long) 30e6);
	CPPUNIT_ASSERT_EQUAL(GuessMicro,CurrentTime);
}

void xDayTime :: comparisonTest (void)
{
	gpstk::DayTime Time1(2006,8,21,13,30,0);
	gpstk::DayTime Time2(2006,8,21,13,30,0);
	gpstk::DayTime Time3(2006,8,23,13,30,0);
	
	CPPUNIT_ASSERT(Time1 == Time2);
	CPPUNIT_ASSERT(Time1 != Time3);
	CPPUNIT_ASSERT(Time1 < Time3);
	CPPUNIT_ASSERT(Time3 > Time1);
	CPPUNIT_ASSERT(Time1 <= Time2);
	CPPUNIT_ASSERT(Time1 <= Time3);
	CPPUNIT_ASSERT(Time3 >= Time1);
	CPPUNIT_ASSERT(Time2 >= Time1);

}

void xDayTime :: setTest (void)
{
	gpstk::DayTime setTest1;
	gpstk::DayTime setTest2;
	gpstk::DayTime setTest3;
	gpstk::DayTime setTest4;
	gpstk::DayTime setTest5;
	gpstk::DayTime setTest6;
	gpstk::DayTime setTest7;
	gpstk::DayTime setTest8;
	gpstk::DayTime setTest9;
	gpstk::DayTime setTest10;
	gpstk::DayTime setTest11;
	gpstk::DayTime setTest12;
	gpstk::DayTime setTest13;
	gpstk::DayTime setTest14;
	gpstk::DayTime setTest15;
	gpstk::DayTime setTest16;
	gpstk::DayTime setTest17;
	gpstk::DayTime setTest18;
	
	setTest1.setYMDHMS(2006,8,21,13,30,0);
	setTest2.setGPS(setTest1.GPS10bitweek(),setTest1.GPSsow());
	setTest3.setGPS(setTest2.GPS10bitweek(),setTest2.GPSzcount());
	setTest4.setGPS(setTest3.GPS10bitweek(),setTest3.GPSzcount(),2006);
	setTest5.setGPS(setTest4.GPS10bitweek(),setTest3.GPSsow(),2006);
	setTest6.setGPS(setTest5.fullZcount());
	setTest7.setGPS(setTest6.GPSfullweek(),setTest6.GPSsow());
	
	setTest8.setGPSZcount(setTest7.GPSzcount());
	gpstk::CommonTime commonTime = CommonTime(setTest7);
	setTest9.setCommonTime(commonTime);
	
	setTest10.setYDoySod(setTest7.year(),setTest7.DOY(),setTest7.secOfDay());
	setTest11.setMJD(setTest10.MJD());
	setTest12.setUnix(setTest11.unixTime());
	setTest18.setAllButTimeFrame(setTest12);
	
	time_t t;
        time(&t);
        struct tm  *ltod;
        ltod = localtime(&t);
	//Just setting this to the local time becasue Im not sure how time_t works
	//Won't be able to do any equality assertions
	setTest13.setANSI(t);

	setTest14.setSystemTime();
	setTest15.setLocalTime();
	setTest16.setYMD(2006,8,21);
	setTest16.setHMS(13,30,0);
	setTest17.setSecOfDay((double) 13*60*60+30*60);
	setTest17.setYDoy(2006,setTest15.DOY());
	
	
	CPPUNIT_ASSERT_EQUAL(setTest1,setTest2);
	CPPUNIT_ASSERT_EQUAL(setTest2,setTest3);
	CPPUNIT_ASSERT_EQUAL(setTest3,setTest4);
	CPPUNIT_ASSERT_EQUAL(setTest4,setTest5);
	CPPUNIT_ASSERT_EQUAL(setTest5,setTest6);
	CPPUNIT_ASSERT_EQUAL(setTest6,setTest7);
	//CPPUNIT_ASSERT_EQUAL(setTest7,setTest8);
	//CPPUNIT_ASSERT_EQUAL(setTest8,setTest9);
	CPPUNIT_ASSERT_EQUAL(setTest7,setTest10);
	CPPUNIT_ASSERT_EQUAL(setTest10,setTest11);
	CPPUNIT_ASSERT_EQUAL(setTest11,setTest12);
	//CPPUNIT_ASSERT_EQUAL(setTest14,setTest15+60*60*5);
	//CPPUNIT_ASSERT_EQUAL(setTest16,setTest17);
	CPPUNIT_ASSERT_EQUAL(setTest1,setTest18);
}

void xDayTime :: stringTest (void)
{
	std::fstream out("Logs/DayTimeDump",ios::out);
	
	gpstk::DayTime stringTest1;
	gpstk::DayTime stringTest2;
	gpstk::DayTime stringTest3;
	gpstk::DayTime stringTest4;
	gpstk::DayTime stringTest5;
	gpstk::DayTime stringTest6;
	gpstk::DayTime stringTest7;
	gpstk::DayTime stringTest8;
	gpstk::DayTime stringTest9;
	gpstk::DayTime stringTest10;
	gpstk::DayTime stringTest11;
	gpstk::DayTime stringTest12;
	gpstk::DayTime stringTest13;
	gpstk::DayTime stringTest14;
	
	gpstk::DayTime CurrentTime(2006,8,21,13,30,0);
	stringTest1.setToString("Aug 21, 2006 13:30:00", "%b %d, %Y %H:%M:%S");
	stringTest2.setToString("53968.5625","%Q");
	stringTest3.setToString("365 135000 2006","%G %g %Y");
	stringTest4.setToString("365 90000 2006","%G %Z %Y");
	stringTest5.setToString("2006 233 48600","%Y %j %s");
	stringTest6.setToString("1389 135000","%F %g");
	stringTest7.setToString("1389 1 48600","%F %w %s");
	stringTest8.setToString("1389 Mon 48600","%F %a %s");
	stringTest9.setToString("8/21/2006 13:30:00", "%m/%d/%Y %H:%M:%f");
	stringTest10.setToString("1156166999 1000000","%U %u");
	stringTest11.setToString("191455120","%C");
	stringTest12.setToString("8 21 2006 13:30:0.0", "%m %d %Y %H:%M:%f");
	stringTest13.setToString("2006   233  13: 30: 0.0", "%Y %j %H:%M:%f");

	CPPUNIT_ASSERT_EQUAL(CurrentTime,stringTest1);
	CPPUNIT_ASSERT_EQUAL(CurrentTime,stringTest2);
	CPPUNIT_ASSERT_EQUAL(CurrentTime,stringTest3);
	CPPUNIT_ASSERT_EQUAL(CurrentTime,stringTest4);
	CPPUNIT_ASSERT_EQUAL(CurrentTime,stringTest5);
	CPPUNIT_ASSERT_EQUAL(CurrentTime,stringTest6);
	CPPUNIT_ASSERT_EQUAL(CurrentTime,stringTest7);
	CPPUNIT_ASSERT_EQUAL(CurrentTime,stringTest8);
	CPPUNIT_ASSERT_EQUAL(CurrentTime,stringTest9);
	CPPUNIT_ASSERT_EQUAL(CurrentTime,stringTest10);
	CPPUNIT_ASSERT_EQUAL(CurrentTime,stringTest11);
	CPPUNIT_ASSERT_EQUAL(CurrentTime,stringTest12);
	CPPUNIT_ASSERT_EQUAL(CurrentTime,stringTest13);
	
	CurrentTime.dump(out);
	stringTest10.dump(out);
	out << stringTest10 << endl;
	out << stringTest10.asString() << endl;
}


//Exceptions and Weird Cases
void xDayTime :: exceptionTest (void)
{
	try
	{
		gpstk::DayTime GenericException;
		gpstk::DayTime WeirdCase1(1999,1,1,0,0,0);
		gpstk::DayTime WeirdCase2(1999,10,1,0,0,0);
		//10 bit rollover during 1999, before Aug rollover
		WeirdCase1.setGPS(WeirdCase1.GPS10bitweek(),WeirdCase1.GPSsow(),WeirdCase1.year());
		//10 bit rollover during 1999, after Aug rollover
		WeirdCase2.setGPS(WeirdCase2.GPS10bitweek(),WeirdCase2.GPSsow(),WeirdCase2.year());
		//Make sure a set throw is thrown for year < 1980 in setGPS (line 836)
		CPPUNIT_ASSERT_THROW(WeirdCase2.setGPS(WeirdCase2.GPS10bitweek(),WeirdCase2.GPSsow(),0),
			gpstk::Exception);
		//Check to see if setGPS for fullZcount is thrown for zcount of -1
		CPPUNIT_ASSERT_THROW(GenericException.setGPS((unsigned long) -1),gpstk::Exception);
		//Throw for setGPSfullweek with a fullweek of -1
		CPPUNIT_ASSERT_THROW(GenericException.setGPSfullweek((short) -1,0.),gpstk::Exception);
		//Throw for very very low MJD which should not exist
		CPPUNIT_ASSERT_THROW(GenericException.setMJD((long double) -10000000),gpstk::Exception);
		//Bad YMD
		CPPUNIT_ASSERT_THROW(GenericException.setYMD(-5000,-5000,-5000),gpstk::Exception);
		//Bad Sec of Day
		CPPUNIT_ASSERT_THROW(GenericException.setSecOfDay(-1.),gpstk::Exception);
		//Bad Day of Year
		CPPUNIT_ASSERT_THROW(GenericException.setYDoy(2006,-100),gpstk::Exception);
		//Bad HMS
		CPPUNIT_ASSERT_THROW(GenericException.setHMS(-5000,-5000,-5000),gpstk::Exception);
		//Go through all of the elseif statments for Jan-Nov
		GenericException.setToString("Dec 21, 2006 13:30:00", "%b %d, %Y %H:%M:%S");
		//Not a real Jan-Dec abbreviation
		CPPUNIT_ASSERT_THROW(GenericException.setToString("AAA 21, 2006 13:30:00", "%b %d, %Y %H:%M:%S"),
			gpstk::Exception);
		//2 digit %y check
		GenericException.setToString("Dec 21, 06 13:30:00", "%b %d, %y %H:%M:%S");
		//3 digit %y check
		GenericException.setToString("Dec 21, 006 13:30:00", "%b %d, %y %H:%M:%S");
		//4 digit %y check
		GenericException.setToString("Dec 21, 2006 13:30:00", "%b %d, %y %H:%M:%S");
		//Go through all of the else statments for Sun-Fri to get to Sat
		GenericException.setToString("1389 Sat 48600","%F %a %s");
		//Not a real Sun-Sat abbreviation
		CPPUNIT_ASSERT_THROW(GenericException.setToString("1389 AAA 48600","%F %a %s"),
			gpstk::Exception);
		//Too much format info
		CPPUNIT_ASSERT_THROW(GenericException.setToString("1389 Mon 48600","%F %a %s %s"),
			gpstk::Exception);
		//Too much string info
		//CPPUNIT_ASSERT_THROW(GenericException.setToString("1389 Mon 48600 48600","%F %a %s"),
		//	gpstk::Exception);
		//Not enough info to form a complete DayTime
		CPPUNIT_ASSERT_THROW(GenericException.setToString("",""),
			gpstk::Exception);
		//Century/non-400 non-leap check
		GenericException.setToString("Feb 29, 1700 13:30:00", "%b %d, %Y %H:%M:%S");
		//There is no year 0 (turn to 1)
		GenericException.setToString("Feb 29, 0 13:30:00", "%b %d, %Y %H:%M:%S");
		
		GenericException.setToString("1389 Sat 48600","%%03F %a %s");
	}
	catch (gpstk::Exception& e)
	{
		//cout << e;
	}
}
