//============================================================================
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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

#include "YDSTime.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>

using namespace gpstk;
using namespace std;

class YDSTime_T
{
public:
//=============================================================================
//      initializationTest ensures the constructors set the values properly
//=============================================================================
   int initializationTest (void)
   {
      TestUtil testFramework( "YDSTime", "Constructor", __FILE__, __LINE__ );


      YDSTime Compare(2008,2,1,TimeSystem::GPS); // Initialize an object

         //--------------------------------------------------------------------
         // Were the attributes set to expectation with the explicit
         // constructor?
         //--------------------------------------------------------------------
      testFramework.assert(2008 == Compare.year,                     "Explicit constructor did not set the year value properly",     __LINE__);
      testFramework.assert(2 == Compare.doy,                         "Explicit constructor did not set the day value properly",      __LINE__);
      testFramework.assert(1 == Compare.sod,                         "Explicit constructor did not set the second value properly",   __LINE__);
      testFramework.assert(TimeSystem(2) == Compare.getTimeSystem(), "Explicit constructor did not set the TimeSystem properly",     __LINE__);


      testFramework.changeSourceMethod("Constructor(YDSTime)");
      YDSTime Copy(Compare); // Initialize the copy constructor
         //--------------------------------------------------------------------
         // Were the attributes set to expectation with the copy constructor?
         //--------------------------------------------------------------------
      testFramework.assert(2008 == Copy.year,                     "Copy constructor did not set the year value properly",     __LINE__);
      testFramework.assert(2 == Copy.doy,                         "Copy constructor did not set the day value properly",      __LINE__);
      testFramework.assert(1 == Copy.sod,                         "Copy constructor did not set the second value properly",   __LINE__);
      testFramework.assert(TimeSystem(2) == Copy.getTimeSystem(), "Copy constructor did not set the TimeSystem properly",     __LINE__);


      testFramework.changeSourceMethod("OperatorSet");
      YDSTime Assigned;
      Assigned = Compare;
         //--------------------------------------------------------------------
         // Were the attributes set to expectation with the Set Operator?
         //--------------------------------------------------------------------
      testFramework.assert(2008 == Assigned.year,                     "Set Operator did not set the year value properly",     __LINE__);
      testFramework.assert(2 == Assigned.doy,                         "Set Operator did not set the day value properly",      __LINE__);
      testFramework.assert(1 == Assigned.sod,                         "Set Operator did not set the second value properly",   __LINE__);
      testFramework.assert(TimeSystem(2) == Assigned.getTimeSystem(), "Set Operator did not set the TimeSystem properly",     __LINE__);

      return testFramework.countFails();
   }


//=============================================================================
//      Test will check if YDSTime variable can be set from a map.
//      Test also implicity tests whether the != operator functions.
//=============================================================================
   int setFromInfoTest (void)
   {
      TestUtil testFramework( "YDSTime", "setFromInfo", __FILE__, __LINE__ );


      YDSTime setFromInfo1;
      YDSTime setFromInfo2;
      YDSTime setFromInfo3;
      YDSTime setFromInfo4;
      YDSTime setFromInfo5;
      YDSTime Compare(2008,2,1,TimeSystem::GPS), Compare2(2006,2,1,TimeSystem::GPS);
      YDSTime Compare3(0,2,1,TimeSystem::GPS);

      TimeTag::IdToValue Id;
      Id['Y'] = "2008";
      Id['j'] = "2";
      Id['s'] = "1";
      Id['P'] = "GPS";

         //--------------------------------------------------------------------
         // Does a proper setFromInfo work with all information provided?
         //--------------------------------------------------------------------
      testFramework.assert(setFromInfo1.setFromInfo(Id), "setFromInfo experienced an error and returned false", __LINE__);
      testFramework.assert(Compare == setFromInfo1,      "setFromInfo did not set all of the values properly",  __LINE__); 


      Id.erase('Y');
      Id['y'] = "06";

         //--------------------------------------------------------------------
         // Does a proper setFromInfo work with 2 digit year?
         //--------------------------------------------------------------------
      testFramework.assert(setFromInfo2.setFromInfo(Id), "setFromInfo experienced an error and returned false", __LINE__);
      testFramework.assert(Compare2 == setFromInfo2,     "setFromInfo did not set all of the values properly",  __LINE__); 

                                                
         // Can we set a three digit year with 'y' option? Answer should be no.
      Id.erase('y');
      Id['y'] = "006";
         //--------------------------------------------------------------------
         // Can a YDSTime object be set with a 3 digit year? Answer should be no. 'y' option is for 2 digit years.
         //--------------------------------------------------------------------
      testFramework.assert(!setFromInfo3.setFromInfo(Id), "setFromInfo allowed a 3 digit year to be set with 'y' option", __LINE__);


      Id.erase('y');
      Id['y'] = "2008";
         //--------------------------------------------------------------------
         // Does a proper setFromInfo work with 4 digit year labeled as 2 digits?
         //--------------------------------------------------------------------
      testFramework.assert(!setFromInfo4.setFromInfo(Id), "setFromInfo experienced an error and returned false", __LINE__);


      Id.erase('y');
         //--------------------------------------------------------------------
         // Can a CivilTime object be set without a year?
         //--------------------------------------------------------------------
      testFramework.assert(setFromInfo5.setFromInfo(Id), "setFromInfo experienced an error and returned false", __LINE__);
      testFramework.assert(setFromInfo5 == Compare3,     "setFromInfo did not set all of the values properly",  __LINE__); 

      return testFramework.countFails();
   }


//=============================================================================
//      Test will check if the ways to initialize and set an YDSTime object.
//      Test also tests whether the comparison operators and isValid method function.
//=============================================================================
   int operatorTest (void)
   {
      TestUtil testFramework( "YDSTime", "OperatorEquivalent", __FILE__, __LINE__ );


      YDSTime Compare(2008,2,1);// Initialize with value
      YDSTime LessThanYear(2005,2,1);// Initialize with value with a smaller year
      YDSTime LessThanDOY(2008,1,1);// Initialize with value with a smaller day of year
      YDSTime LessThanSOD(2008,2,0);// Initialize with value with a smaller second of day
      YDSTime CompareCopy(Compare); // Initialize with copy constructor

         //--------------------------------------------------------------------
         // Does the == Operator function?
         //--------------------------------------------------------------------
      testFramework.assert(  Compare == CompareCopy,   "Equivalence operator found equivalent objects to be not equivalent",   __LINE__);
      testFramework.assert(!(Compare == LessThanYear), "Equivalence operator found different year objects to be equivalent",   __LINE__);
      testFramework.assert(!(Compare == LessThanDOY),  "Equivalence operator found different day objects to be equivalent",    __LINE__);
      testFramework.assert(!(Compare == LessThanSOD),  "Equivalence operator found different second objects to be equivalent", __LINE__);


      testFramework.changeSourceMethod("OperatorNotEquivalent");
         //--------------------------------------------------------------------
         // Does the != Operator function?
         //--------------------------------------------------------------------
      testFramework.assert(  Compare != LessThanYear,  "Not-equal operator found different year objects to be equivalent",   __LINE__);
      testFramework.assert(  Compare != LessThanDOY,   "Not-equal operator found different day objects to be equivalent",    __LINE__);
      testFramework.assert(  Compare != LessThanSOD,   "Not-equal operator found different second objects to be equivalent", __LINE__);
      testFramework.assert(!(Compare != CompareCopy),  "Not-equal operator found equivalent objects to not be equivalent",   __LINE__);
   
      testFramework.changeSourceMethod("OperatorLessThan");
         //--------------------------------------------------------------------
         // Does the < operator function?
         //--------------------------------------------------------------------
      testFramework.assert(  LessThanYear < Compare,  "Less-than operator found less-than year object to not be less than",   __LINE__);
      testFramework.assert(  LessThanDOY < Compare,   "Less-than operator found less-than day object to not be less than",    __LINE__);
      testFramework.assert(  LessThanSOD < Compare,   "Less-than operator found less-than second object to not be less than", __LINE__);
      testFramework.assert(!(Compare < LessThanYear), "Less-than operator found greater-than year object to be less than",    __LINE__);
      testFramework.assert(!(Compare < LessThanDOY),  "Less-than operator found greater-than day object to be less than",     __LINE__);
      testFramework.assert(!(Compare < LessThanSOD),  "Less-than operator found greater-than second object to be less than",  __LINE__);
      testFramework.assert(!(Compare < CompareCopy),  "Less-than operator found equivalent objects to be less than",          __LINE__);


      testFramework.changeSourceMethod("OperatorGreaterThan");
         //--------------------------------------------------------------------
         // Does the > operator function?
         //--------------------------------------------------------------------
      testFramework.assert(!(LessThanYear > Compare), "Greater-than operator found less-than year object to be greater-than",          __LINE__);
      testFramework.assert(!(LessThanDOY > Compare),  "Greater-than operator found less-than day object to be greater-than",           __LINE__);
      testFramework.assert(!(LessThanSOD > Compare),  "Greater-than operator found less-than second object to be greater-than",        __LINE__);
      testFramework.assert(  Compare > LessThanYear,  "Greater-than operator found greater-than year object to not be greater-than",   __LINE__);
      testFramework.assert(  Compare > LessThanDOY,   "Greater-than operator found greater-than day object to not be greater-than",    __LINE__);
      testFramework.assert(  Compare > LessThanSOD,   "Greater-than operator found greater-than second object to not be greater-than", __LINE__);
      testFramework.assert(!(Compare > CompareCopy),  "Greater-than operator found equivalent objects to be greater-than",             __LINE__);


      testFramework.changeSourceMethod("OperatorLessThanOrEqualTo");
         //--------------------------------------------------------------------
         // Does the <= operator function?
         //--------------------------------------------------------------------
      testFramework.assert(  LessThanYear <= Compare,  "Less-than-or-equal-to operator found less-than year object to not be less-than-or-equal-to",   __LINE__);
      testFramework.assert(  LessThanDOY <= Compare,   "Less-than-or-equal-to operator found less-than day object to not be less-than-or-equal-to",    __LINE__);
      testFramework.assert(  LessThanSOD <= Compare,   "Less-than-or-equal-to operator found less-than second object to not be less-than-or-equal-to", __LINE__);
      testFramework.assert(!(Compare <= LessThanYear), "Less-than-or-equal-to operator found greater-than year object to be less-than-or-equal-to",    __LINE__);
      testFramework.assert(!(Compare <= LessThanDOY),  "Less-than-or-equal-to operator found greater-than day object to be less-than-or-equal-to",     __LINE__);
      testFramework.assert(!(Compare <= LessThanSOD),  "Less-than-or-equal-to operator found greater-than second object to be less-than-or-equal-to",  __LINE__);
      testFramework.assert(  Compare <= CompareCopy,   "Less-than-or-equal-to operator found equivalent objects to not be less-than-or-equal-to",      __LINE__);


      testFramework.changeSourceMethod("OperatorGreaterThanOrEqualTo");
         //--------------------------------------------------------------------
         // Does the >= operator function?
         //--------------------------------------------------------------------
      testFramework.assert(!(LessThanYear >= Compare), "Greater-than-or-equal-to operator found less-than year object to be greater-than-or-equal-to",          __LINE__);
      testFramework.assert(!(LessThanDOY >= Compare),  "Greater-than-or-equal-to operator found less-than day object to be greater-than-or-equal-to",           __LINE__);
      testFramework.assert(!(LessThanSOD >= Compare),  "Greater-than-or-equal-to operator found less-than second object to be greater-than-or-equal-to",        __LINE__);
      testFramework.assert(  Compare >= LessThanYear,  "Greater-than-or-equal-to operator found greater-than year object to not be greater-than-or-equal-to",   __LINE__);
      testFramework.assert(  Compare >= LessThanDOY,   "Greater-than-or-equal-to operator found greater-than day object to not be greater-than-or-equal-to",    __LINE__);
      testFramework.assert(  Compare >= LessThanSOD,   "Greater-than-or-equal-to operator found greater-than second object to not be greater-than-or-equal-to", __LINE__);
      testFramework.assert(  Compare >= CompareCopy,   "Greater-than-or-equal-to operator found equivalent objects to not be greater-than-or-equal-to",         __LINE__);

      return testFramework.countFails();
   }


//=============================================================================
//      Test will check the reset method.
//=============================================================================
   int resetTest (void)
   {
      TestUtil testFramework( "YDSTime", "reset" , __FILE__, __LINE__ );


      YDSTime Compare(2008,2,1,TimeSystem::GPS); // Initialize an object

      Compare.reset(); // Reset it

         //--------------------------------------------------------------------
         // Were the attributes reset to expectation?
         //--------------------------------------------------------------------
      testFramework.assert(TimeSystem(0) == Compare.getTimeSystem(),  "reset() did not set the TimeSystem to UNK",  __LINE__);          
      testFramework.assert(0 == (int)Compare.year,                    "reset() did not set the year value to 0",    __LINE__);
      testFramework.assert(0 == (int)Compare.doy,                     "reset() did not set the doy value to 0",     __LINE__);          
      testFramework.assert(0 == (int)Compare.sod,                     "reset() did not set the sod value to 0",     __LINE__); 

      return testFramework.countFails();
   }


//=============================================================================
//      Test will check converting to/from CommonTime.
//=============================================================================
   int toFromCommonTimeTest (void)
   {
      TestUtil testFramework( "YDSTime", "isValid", __FILE__, __LINE__ );


      YDSTime Compare(2008,2,1,TimeSystem::GPS); // Initialize an object
      CommonTime Test = Compare.convertToCommonTime(); // Convert to

         //--------------------------------------------------------------------
         // Is the time after the BEGINNING_OF_TIME?
         //--------------------------------------------------------------------
      testFramework.assert(Compare.convertToCommonTime() > CommonTime::BEGINNING_OF_TIME, "Time provided found to be less than the beginning of time", __LINE__);


         //--------------------------------------------------------------------
         // Is the set object valid?
         //--------------------------------------------------------------------
      testFramework.assert(Compare.isValid(), "Time provided found to be unable to convert to/from CommonTime", __LINE__);


      YDSTime Test2;
      Test2.convertFromCommonTime(Test); // Convert From

      testFramework.changeSourceMethod("CommonTimeConversion");
         //--------------------------------------------------------------------
         // Is the result of conversion the same?
         //--------------------------------------------------------------------
      testFramework.assert(Compare.getTimeSystem()== Test2.getTimeSystem(),  "TimeSystem provided found to be different after converting to and from CommonTime", __LINE__);
      testFramework.assert(Test2.year == Compare.year,                       "Year provided found to be different after converting to and from CommonTime",       __LINE__);
      testFramework.assert(Test2.doy == Compare.doy,                         "DOY provided found to be different after converting to and from CommonTime",        __LINE__);
      testFramework.assert(Test2.sod == Compare.sod,                         "SOD provided found to be different after converting to and from CommonTime",        __LINE__);
      return testFramework.countFails();
   }


//=============================================================================
//      Test will check the TimeSystem comparisons when using the comparison operators.
//=============================================================================
   int timeSystemTest (void)
   {
      TestUtil testFramework( "YDSTime", "OperatorEquivalentWithDifferingTimeSystem", __FILE__, __LINE__ );


      YDSTime GPS1(   2008,2,1,TimeSystem::GPS    );
      YDSTime GPS2(   2005,2,1,TimeSystem::GPS    );
      YDSTime UTC1(   2008,2,1,TimeSystem::UTC    );
      YDSTime UNKNOWN(2008,2,1,TimeSystem::Unknown);
      YDSTime ANY(    2008,2,1,TimeSystem::Any    );
      YDSTime ANY2(   2005,2,1,TimeSystem::Any    );

         //--------------------------------------------------------------------
         // Verify differing TimeSystem sets equivalence operator to false
         // Note that the operator test checks for == in ALL members
         //--------------------------------------------------------------------
      testFramework.assert(!(GPS1 == UTC1), "Equivalence operator found objects with differing TimeSystems to be the same", __LINE__);
      testFramework.assert(GPS1 == ANY,     "Differing TimeSystems where one is TimeSystem::Any is not ignored for equals", __LINE__);
      testFramework.assert(UTC1 == ANY,     "Differing TimeSystems where one is TimeSystem::Any is not ignored for equals", __LINE__);
      testFramework.assert(UNKNOWN == ANY,  "Differing TimeSystems where one is TimeSystem::Any is not ignored for equals", __LINE__);

      testFramework.changeSourceMethod("OperatorNotEquivalentWithDifferingTimeSystem");
         //--------------------------------------------------------------------
         // Verify different Time System but same time inequality
         //--------------------------------------------------------------------
      testFramework.assert(GPS1 != UTC1,    "Equivalent objects with differing TimeSystems are found to be equal",                                  __LINE__);
      testFramework.assert(GPS1 != UNKNOWN, "Equivalent objects with differing TimeSystems are found to be equal",                                  __LINE__);
      testFramework.assert(!(GPS1 != ANY),  "Equivalent objects with differing TimeSystems where one is TimeSystem::Any are found to be not-equal", __LINE__);

      testFramework.changeSourceMethod("OperatorLessThanWithDifferingTimeSystem");      
         //--------------------------------------------------------------------
         // Verify TimeSystem=ANY does not matter in other operator comparisons 
         //--------------------------------------------------------------------
      testFramework.assert(ANY2 < GPS1, "Less than object with Any TimeSystem is not found to be less than", __LINE__);
      testFramework.assert(GPS2 < ANY,"Less than object with GPS TimeSystem is not found to be less-than a greater object with Any TimeSystem", __LINE__);

      testFramework.changeSourceMethod("setTimeSystem");        
      UNKNOWN.setTimeSystem(TimeSystem(2)); // Set the Unknown TimeSystem
         //--------------------------------------------------------------------
         // Ensure resetting a Time System changes it
         //--------------------------------------------------------------------
      testFramework.assert(UNKNOWN.getTimeSystem()==TimeSystem(2), "setTimeSystem was unable to set the TimeSystem", __LINE__);

      return testFramework.countFails();
   }


//=============================================================================
//      Test for the formatted printing of YDSTime objects
//=============================================================================
   int printfTest (void)
   {
      TestUtil testFramework( "YDSTime", "printf", __FILE__, __LINE__ );


      YDSTime GPS1(2008,2,1,TimeSystem::GPS);
      YDSTime UTC1(2008,2,1,TimeSystem::UTC);
                
         //--------------------------------------------------------------------
         // Verify printed output matches expectation
         //--------------------------------------------------------------------
      testFramework.assert(GPS1.printf("%04Y %02y %03j %02s %02P") == (std::string)"2008 08 002 1.000000 GPS", "printf did not output in the proper format", __LINE__);
      testFramework.assert(UTC1.printf("%04Y %02y %03j %02s %02P") == (std::string)"2008 08 002 1.000000 UTC", "printf did not output in the proper format", __LINE__);
  

         //--------------------------------------------------------------------
         // Verify printed error message matches expectation
         //--------------------------------------------------------------------
      testFramework.assert(GPS1.printError("%04Y %02y %03j %02s %02P") == (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime", "printError did not output in the proper format", __LINE__);
      testFramework.assert(UTC1.printError("%04Y %02y %03j %02s %02P") == (std::string)"ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime ErrorBadTime", "printError did not output in the proper format", __LINE__);


      return testFramework.countFails();
   }
};

int main() // Main function to initialize and run all tests above
{
   int check, errorCounter = 0;
   YDSTime_T testClass;

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

   return errorCounter; // Return the total number of errors
}
