#include "TimeSystem.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <sstream>
#include <cmath>

class TimeSystem_T
{
public: 
   TimeSystem_T(){ eps = 1E-12; }// Default Constructor, set the precision value
   ~TimeSystem_T() {} // Default Desructor

//==========================================================================
//	initializationTest ensures the constructors set the values properly
//	get methods are tested implicitly
//==========================================================================
   int initializationTest(void)
   {
      gpstk::TestUtil testFramework("TimeSystem", "Constructor", __FILE__, __LINE__);
      std::string testMesg;

      gpstk::TimeSystem Empty;
      gpstk::TimeSystem ExplicitInt(2);
      gpstk::TimeSystem ExplicitString;
      ExplicitString.fromString("GLO");
      gpstk::TimeSystem Set;
      Set.setTimeSystem(gpstk::TimeSystem::Systems(5)); //QZSS

         //---------------------------------------------------------------------------
         //Checking the default constructor
         //---------------------------------------------------------------------------
      testMesg = "The default constructor stores the incorrect value";
      testFramework.assert(Empty.getTimeSystem() == 0, testMesg, __LINE__);
         //---------------------------------------------------------------------------
         //Checking the explicit constructor
         //---------------------------------------------------------------------------
      testMesg = "The explicit constructor stores the incorrect value";
      testFramework.assert(ExplicitInt.getTimeSystem() == 2, testMesg, __LINE__);
         //---------------------------------------------------------------------------
         //Checking the string constructor
         //---------------------------------------------------------------------------
      testMesg = "The fromString method stores the incorrect value";
      testFramework.assert(ExplicitString.getTimeSystem() == 3, testMesg, __LINE__);
         //---------------------------------------------------------------------------
         //Checking the set and get methods
         //---------------------------------------------------------------------------
      testMesg = "The setTimeSystem method stores the incorrect value";		
      testFramework.assert(Set.getTimeSystem() == 5, testMesg, __LINE__);

         //---------------------------------------------------------------------------
         //Checking the asString method
         //---------------------------------------------------------------------------
      testMesg = "The asString method reutrns the incorrect value";		
      testFramework.assert(ExplicitString.asString() == "GLO", testMesg, __LINE__);

      return testFramework.countFails();
   }

   int operatorTest(void)
   {
      gpstk::TestUtil testFramework("TimeSystem", "== Operator", __FILE__, __LINE__);
      std::string testMesg;

      gpstk::TimeSystem Compare(4);
      gpstk::TimeSystem Equivalent(4);
      gpstk::TimeSystem LessThan(3);
      gpstk::TimeSystem GreaterThan(5);

         //---------------------------------------------------------------------------
         //Does the == operator function correctly?
         //---------------------------------------------------------------------------
      testMesg = "Equivalent objects were not marked equivalent by ==";
      testFramework.assert(Compare == Equivalent, testMesg, __LINE__);
      testMesg = "Equivalent objects were marked not equivalent by ==";
      testFramework.assert(!(Compare == LessThan), testMesg, __LINE__);

      testFramework.changeSourceMethod("!= Operator");
         //---------------------------------------------------------------------------
         //Does the != operator function correctly?
         //---------------------------------------------------------------------------		
      testMesg = "Non-equivalent objects were marked equivalent by !=";
      testFramework.assert(Compare != LessThan, testMesg, __LINE__);
      testMesg = "Equivalent objects were marked equivalent by !=";
      testFramework.assert(!(Compare != Equivalent), testMesg, __LINE__);

      testFramework.changeSourceMethod("> Operator");
         //---------------------------------------------------------------------------
         //Does the > operator function correctly?
         //---------------------------------------------------------------------------
      testMesg = "Less-than object was not marked as lesser by the > operator";
      testFramework.assert(Compare > LessThan, testMesg, __LINE__);
      testMesg = "Less-than object was marked as greater by the > operator";
      testFramework.assert(!(LessThan > Compare), testMesg, __LINE__);
      testMesg = "Equivalent objects were marked as non-equivalent by the > operator";
      testFramework.assert(!(Compare > Equivalent), testMesg, __LINE__);

      testFramework.changeSourceMethod("< Operator");
         //---------------------------------------------------------------------------
         //Does the < operator function correctly?
         //---------------------------------------------------------------------------
      testMesg = "Greater-than object was not marked as greater by the < operator";
      testFramework.assert(Compare < GreaterThan, testMesg, __LINE__);
      testMesg = "Greater-than object was marked as lesser by the < operator";
      testFramework.assert(!(GreaterThan < Compare), testMesg, __LINE__);
      testMesg = "Equivalent objects were marked as non-equivalent by the < operator";
      testFramework.assert(!(Compare < Equivalent), testMesg, __LINE__);

      testFramework.changeSourceMethod(">= Operator");
         //---------------------------------------------------------------------------
         //Does the >= operator function correctly?
         //---------------------------------------------------------------------------
      testMesg = "Less-than object was not marked as lesser by the >= operator";		
      testFramework.assert(Compare >= LessThan, testMesg, __LINE__);
      testMesg = "Less-than object was marked as greater by the >= operator";
      testFramework.assert(!(LessThan >= Compare), testMesg, __LINE__);
      testMesg = "Equivalent objects were marked as non-equivalent by the >= operator";
      testFramework.assert(Compare >= Equivalent, testMesg, __LINE__);

      testFramework.changeSourceMethod("<= Operator");
         //---------------------------------------------------------------------------
         //Does the <= operator function correctly?
         //---------------------------------------------------------------------------
      testMesg = "Greater-than object was not marked as greater by the < operator";
      testFramework.assert(Compare <= GreaterThan, testMesg, __LINE__);
      testMesg = "Greater-than object was marked as lesser by the < operator";
      testFramework.assert(!(GreaterThan <= Compare), testMesg, __LINE__);
      testMesg = "Equivalent objects were marked as non-equivalent by the < operator";
      testFramework.assert(Compare <= Equivalent, testMesg, __LINE__);

      testFramework.changeSourceMethod("<< Operator");
         //---------------------------------------------------------------------------
         //Does the << operator function correctly?
         //---------------------------------------------------------------------------
      std::string outputString, compareString;
      std::stringstream outputStream;
      outputStream << Compare;
      outputString = outputStream.str();
      compareString = "GAL";
      testMesg = "The << operator did not function correctly";
      testFramework.assert(compareString == outputString, testMesg, __LINE__);

      return testFramework.countFails();
   }

   int getLeapSecondsTest(void)
   {
      gpstk::TestUtil testFramework("TimeSystem", "Leap Seconds", __FILE__, __LINE__);
      std::string testMesg;

      gpstk::TimeSystem Compare(1);

         //-------------------------------------------------------------------------
         //Check leap-second computation prior to 1972 but not before 1960
         //-------------------------------------------------------------------------
      testMesg = "Incorrect value returned for getLeapSeconds";

         //Should be (inputDate - lastRefDate) * rate + leapSeconds
         //4.213170+488*0.002592
      testFramework.assert(std::abs(Compare.getLeapSeconds(1969, 6, 3) - 5.478066) < eps, testMesg, __LINE__);
         //1.8458580 + 118*0.0011232
      testFramework.assert(std::abs(Compare.getLeapSeconds(1962, 4,  29) - 1.9783956) < eps, testMesg, __LINE__);
         //3.6401300 + 5*.001296
      testFramework.assert(std::abs(Compare.getLeapSeconds(1965, 3,  6) - 3.64661) < eps, testMesg, __LINE__);
         //4.2131700 + 295*.002592
      testFramework.assert(std::abs(Compare.getLeapSeconds(1968, 11,  22) - 4.97781) < eps, testMesg, __LINE__);
         //4.3131700 + 409*.002592
      testFramework.assert(std::abs(Compare.getLeapSeconds(1967, 2,  14) - 5.373298) < eps, testMesg, __LINE__);

         //-------------------------------------------------------------------------
         //Check leap-second computation after 1/1/1972
         //-------------------------------------------------------------------------
      testFramework.assert(std::abs(Compare.getLeapSeconds(1995, 10, 13) - 29) < eps, testMesg, __LINE__);
      testFramework.assert(std::abs(Compare.getLeapSeconds(2004, 3, 25) - 32) < eps, testMesg, __LINE__); 
      testFramework.assert(std::abs(Compare.getLeapSeconds(1984, 8, 27) - 22) < eps, testMesg, __LINE__);
      testFramework.assert(std::abs(Compare.getLeapSeconds(1972, 5, 8) - 10) < eps, testMesg, __LINE__);


      return testFramework.countFails();
   }

   int correctionTest(void)
   {
      gpstk::TestUtil testFramework("TimeSystem", "Correction", __FILE__, __LINE__);
      std::string testMesg;

      gpstk::TimeSystem GPStime(2);
      gpstk::TimeSystem GLOtime(3);
      gpstk::TimeSystem GALtime(4);
      gpstk::TimeSystem QZStime(5);
      gpstk::TimeSystem BDTtime(6);
      gpstk::TimeSystem UTCtime(7);
      gpstk::TimeSystem TAItime(8);
      gpstk::TimeSystem TTtime(9);
      gpstk::TimeSystem TDBtime(10);


         //-------------------------------------------------------------------------
         //Check conversion from any given time system to UTC and back 
         //-------------------------------------------------------------------------
      testMesg = "Conversion from UTC time to GPS time was incorrect";
      testFramework.assert(std::abs(gpstk::TimeSystem::Correction(UTCtime, GPStime, 1990, 11, 6) - 6) < eps, testMesg, __LINE__);
      testMesg = "Conversion from GPS time to UTC time was incorrect";
      testFramework.assert(std::abs(gpstk::TimeSystem::Correction(GPStime, UTCtime, 2004, 11, 16) + 13) < eps, testMesg, __LINE__);		
      testMesg = "Conversion from UTC time to GLO time was incorrect";
      testFramework.assert(std::abs(gpstk::TimeSystem::Correction(UTCtime, GLOtime, 1992, 10, 3) - 0) < eps, testMesg, __LINE__);
      testMesg = "Conversion from GLO time to UTC time was incorrect";
      testFramework.assert(std::abs(gpstk::TimeSystem::Correction(GLOtime, UTCtime, 1995, 5, 10) - 0) < eps, testMesg, __LINE__);
      testMesg = "Conversion from UTC time to GAL time was incorrect";
      testFramework.assert(std::abs(gpstk::TimeSystem::Correction(UTCtime, GALtime, 1997, 7, 25) - 12) < eps, testMesg, __LINE__);
      testMesg = "Conversion from GAL time to UTC time was incorrect";
      testFramework.assert(std::abs(gpstk::TimeSystem::Correction(GALtime, UTCtime, 2008, 6, 5) + 14) < eps, testMesg, __LINE__);
		
         // QZSS can't be converted 
         //testMesg = "Conversion from UTC time to QZS time was incorrect";
         //testFramework.assert(std::abs(gpstk::TimeSystem::Correction(UTCtime, QZStime, 1985, 8, 10) - 4) < eps, testMesg, __LINE__);
         //testMesg = "Conversion from QZS time to UTC time was incorrect";
         //testFramework.assert(std::abs(gpstk::TimeSystem::Correction(QZStime, UTCtime, 2010, 2, 14) - 15) < eps, testMesg, __LINE__);
		
      testMesg = "Conversion from UTC time to BDT time was incorrect";
      testFramework.assert(std::abs(gpstk::TimeSystem::Correction(UTCtime, BDTtime, 2001, 9, 21) - 0) < eps, testMesg, __LINE__);
      testMesg = "Conversion from BDT time to UTC time was incorrect";
      testFramework.assert(std::abs(gpstk::TimeSystem::Correction(BDTtime, UTCtime, 2012, 8, 27) - 0) < eps, testMesg, __LINE__);
      testMesg = "Conversion from UTC time to TAI time was incorrect";
      testFramework.assert(std::abs(gpstk::TimeSystem::Correction(UTCtime, TAItime, 2014, 6, 1) - 35) < eps, testMesg, __LINE__);
      testMesg = "Conversion from TAI time to UTC time was incorrect";
      testFramework.assert(std::abs(gpstk::TimeSystem::Correction(TAItime, UTCtime, 2015, 1, 1) + 35) < eps, testMesg, __LINE__);	
      testMesg = "Conversion from UTC time to TT time was incorrect";
      testFramework.assert(std::abs(gpstk::TimeSystem::Correction(UTCtime,  TTtime, 2005, 4, 31) - (13 + 51.184)) < eps, testMesg, __LINE__);
      testMesg = "Conversion from TT time to UTC time was incorrect";
      testFramework.assert(std::abs(gpstk::TimeSystem::Correction(TTtime, UTCtime, 1990, 7, 21) + (6 + 51.184) ) < eps, testMesg, __LINE__);
         //reference section B of astronomical almanac for TDB conversion
      testMesg = "Conversion from UTC time to TDB time was incorrect";
      testFramework.assert(std::abs(gpstk::TimeSystem::Correction(UTCtime,  TDBtime, 2007, 12, 25) - 65.1840299405112091335467994213104248046875) < eps, testMesg, __LINE__);
      testMesg = "Conversion from TDB time to UTC time was incorrect";
      testFramework.assert(std::abs(gpstk::TimeSystem::Correction(TDBtime, UTCtime, 1991, 4, 25) + 58.1838658094272460630236309953033924102783203125) < eps, testMesg, __LINE__);

      return testFramework.countFails();
   }

private:
   double eps;

};


int main() //Main function to initialize and run all tests above
{
	TimeSystem_T testClass;
	int check, errorCounter = 0;

	check = testClass.initializationTest();
	errorCounter += check;

	check = testClass.operatorTest();
	errorCounter += check;

	check = testClass.getLeapSecondsTest();
	errorCounter += check;

	check = testClass.correctionTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
