//==============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#include "TimeSystem.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <sstream>
#include <cmath>

class TimeSystem_T
{
public: 
   unsigned operatorTest()
   {
      TUDEF("TimeSystem", "operator<<");
      std::string testMesg;

      gpstk::TimeSystem compare = gpstk::TimeSystem::GAL;

         //Does operator<< function correctly?
      std::string outputString, compareString;
      std::stringstream outputStream;
      outputStream << compare;
      outputString = outputStream.str();
      compareString = "GAL";
      TUASSERTE(std::string, compareString, outputString);

      TURETURN();
   }


   unsigned getLeapSecondsTest()
   {
      TUDEF("TimeSystem", "getLeapSeconds");

         //Check leap-second computation prior to 1972 but not before 1960

         //Should be (inputDate - lastRefDate) * rate + leapSeconds
         //4.213170+488*0.002592
      TUASSERTFE(5.478066, gpstk::getLeapSeconds(1969, 6, 3));
         //1.8458580 + 118*0.0011232
      TUASSERTFE(1.9783956, gpstk::getLeapSeconds(1962, 4,  29));
         //3.6401300 + 5*.001296
      TUASSERTFE(3.64661, gpstk::getLeapSeconds(1965, 3,  6));
         //4.2131700 + 295*.002592
      TUASSERTFE(4.97781, gpstk::getLeapSeconds(1968, 11,  22));
         //4.3131700 + 409*.002592
      TUASSERTFE(5.373298, gpstk::getLeapSeconds(1967, 2,  14));

         //Check leap-second computation after 1/1/1972
      TUASSERTFE(29, gpstk::getLeapSeconds(1995, 10, 13));
      TUASSERTFE(32, gpstk::getLeapSeconds(2004, 3, 25));
      TUASSERTFE(22, gpstk::getLeapSeconds(1984, 8, 27));
      TUASSERTFE(10, gpstk::getLeapSeconds(1972, 5, 8));

      TURETURN();
   }


   unsigned correctionTest()
   {
      TUDEF("TimeSystem", "getTimeSystemCorrection");

         //Check conversion from any given time system to UTC and back 
      TUASSERTFE(6, gpstk::getTimeSystemCorrection(gpstk::TimeSystem::UTC, gpstk::TimeSystem::GPS, 1990, 11, 6));
      TUASSERTFE(-13, gpstk::getTimeSystemCorrection(gpstk::TimeSystem::GPS, gpstk::TimeSystem::UTC, 2004, 11, 16));
      TUASSERTFE(0, gpstk::getTimeSystemCorrection(gpstk::TimeSystem::UTC, gpstk::TimeSystem::GLO, 1992, 10, 3));
      TUASSERTFE(0, gpstk::getTimeSystemCorrection(gpstk::TimeSystem::GLO, gpstk::TimeSystem::UTC, 1995, 5, 10));
      TUASSERTFE(12, gpstk::getTimeSystemCorrection(gpstk::TimeSystem::UTC, gpstk::TimeSystem::GAL, 1997, 7, 25));
      TUASSERTFE(-14, gpstk::getTimeSystemCorrection(gpstk::TimeSystem::GAL, gpstk::TimeSystem::UTC, 2008, 6, 5));
		
         // QZSS can't be converted 
         //testMesg = "Conversion from UTC time to QZS time was incorrect";
         //testFramework.assert(std::abs(gpstk::getTimeSystemCorrection(gpstk::TimeSystem::UTC, gpstk::TimeSystem::QZS, 1985, 8, 10) - 4) < eps, testMesg, __LINE__);
         //testMesg = "Conversion from QZS time to UTC time was incorrect";
         //testFramework.assert(std::abs(gpstk::getTimeSystemCorrection(gpstk::TimeSystem::QZS, gpstk::TimeSystem::UTC, 2010, 2, 14) - 15) < eps, testMesg, __LINE__);
		
      TUASSERTFE(0, gpstk::getTimeSystemCorrection(gpstk::TimeSystem::UTC, gpstk::TimeSystem::BDT, 2006, 9, 21));
      TUASSERTFE(-2, gpstk::getTimeSystemCorrection(gpstk::TimeSystem::BDT, gpstk::TimeSystem::UTC, 2012, 8, 27));
      TUASSERTFE(13, gpstk::getTimeSystemCorrection(gpstk::TimeSystem::UTC, gpstk::TimeSystem::IRN, 2004, 11, 16));
      TUASSERTFE(-13, gpstk::getTimeSystemCorrection(gpstk::TimeSystem::IRN, gpstk::TimeSystem::UTC, 2004, 11, 16));
      TUASSERTFE(35, gpstk::getTimeSystemCorrection(gpstk::TimeSystem::UTC, gpstk::TimeSystem::TAI, 2014, 6, 1));
      TUASSERTFE(-35, gpstk::getTimeSystemCorrection(gpstk::TimeSystem::TAI, gpstk::TimeSystem::UTC, 2015, 1, 1));
      TUASSERTFE(13 + 51.184, gpstk::getTimeSystemCorrection(gpstk::TimeSystem::UTC,  gpstk::TimeSystem::TT, 2005, 4, 31));
      TUASSERTFE(-(6 + 51.184), gpstk::getTimeSystemCorrection(gpstk::TimeSystem::TT, gpstk::TimeSystem::UTC, 1990, 7, 21));
         //reference section B of astronomical almanac for TDB conversion
      TUASSERTFE(65.1840299405112091335467994213104248046875,
                 gpstk::getTimeSystemCorrection(gpstk::TimeSystem::UTC,  gpstk::TimeSystem::TDB, 2007, 12, 25));
      TUASSERTFE(-58.1838658094272460630236309953033924102783203125,
                 gpstk::getTimeSystemCorrection(gpstk::TimeSystem::TDB, gpstk::TimeSystem::UTC, 1991, 4, 25));

      TURETURN();
   }
};


int main() //Main function to initialize and run all tests above
{
   TimeSystem_T testClass;
   unsigned errorCounter = 0;

   errorCounter += testClass.operatorTest();
   errorCounter += testClass.getLeapSecondsTest();
   errorCounter += testClass.correctionTest();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter
             << std::endl;

   return errorCounter; //Return the total number of errors
}
