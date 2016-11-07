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
#include "SP3SatID.hpp"

#include "TestUtil.hpp"
#include <iostream>
#include <string>
#include <sstream>

class SP3SatID_T
{
public:
   SP3SatID_T() {} // Default Constructor, set the precision value
   ~SP3SatID_T() {} // Default Desructor

//==========================================================================================================================
// initializationTest ensures the constructors set the values properly
//==========================================================================================================================
   int initializationTest(void)
   {
      gpstk::TestUtil testFramework("SP3SatID", "Constructor", __FILE__, __LINE__);


         //---------------------------------------------------------------------
         //Does the Explicit Constructor function?
         //---------------------------------------------------------------------
      gpstk::SP3SatID Compare1(5, gpstk::SatID::SatelliteSystem(1));
      testFramework.assert(Compare1.id == 5,
                           "Explicit constructor did not set the correct ID", __LINE__);
      testFramework.assert(Compare1.system == gpstk::SatID::SatelliteSystem(1),
                           "Explicit constructor did not set the correct SatelliteSystem", __LINE__);


         //---------------------------------------------------------------------
         //Does the Default Constructor function?
         //---------------------------------------------------------------------
      testFramework.changeSourceMethod("ConstructorDefault");
      gpstk::SP3SatID Compare2;
      testFramework.assert(Compare2.id == -1,
                           "Default constructor did not set the expected ID", __LINE__);
      testFramework.assert(Compare2.system == gpstk::SatID::SatelliteSystem(1),
                           "Default constructor did not set the expected SatelliteSystem", __LINE__);


         //---------------------------------------------------------------------
         //Does the fromString Constructor function?
         //---------------------------------------------------------------------
      testFramework.changeSourceMethod("ConstructorFromString");
      gpstk::SP3SatID Compare3("G 10");
      testFramework.assert(Compare3.id == 10,
                           "fromString constructor did not set the correct ID", __LINE__);
      testFramework.assert(Compare3.system == gpstk::SatID::SatelliteSystem(1),
                           "fromString constructor did not set the correct SatelliteSystem", __LINE__);

         //Did the constructor throw an exception for an improper string?
      try
      {
         gpstk::SP3SatID Compare4("Z 1");
         testFramework.assert(false,
                              "fromString constructor did not throw an exception for an improper string",
                              __LINE__);
      }
      catch(gpstk::Exception& e)
      {
         testFramework.assert(true, "fromString threw the expected exception",
                              __LINE__);
      }

         //---------------------------------------------------------------------
         //Does the SatID Constructor function?
         //---------------------------------------------------------------------
      testFramework.changeSourceMethod("ConstructorSatID");
      gpstk::SatID sat1(7, gpstk::SatID::SatelliteSystem(1));
      gpstk::SP3SatID Compare4(sat1);
      testFramework.assert(Compare4.id == 7,
                           "SatID constructor did not set the correct ID", __LINE__);
      testFramework.assert(Compare4.system == gpstk::SatID::SatelliteSystem(1),
                           "SatID constructor did not set the correct SatelliteSystem", __LINE__);

      return testFramework.countFails();
   }


//==========================================================================================================================
// operatorTest verifies the various operators of the SatID class
//==========================================================================================================================
   int operatorTest(void) //including <<
   {
      gpstk::TestUtil testFramework( "SatID", "OperatorEquivalence", __FILE__, __LINE__);


      gpstk::SP3SatID Compare    (5, gpstk::SatID::SatelliteSystem(2) );
      gpstk::SP3SatID Equivalent (5, gpstk::SatID::SatelliteSystem(2) );
      gpstk::SP3SatID LessThanID (2, gpstk::SatID::SatelliteSystem(2) );
      gpstk::SP3SatID DiffSatSys (5, gpstk::SatID::SatelliteSystem(3) );
      gpstk::SP3SatID DiffEvery  (2, gpstk::SatID::SatelliteSystem(3) );
      gpstk::SP3SatID DiffEvery2 (7, gpstk::SatID::SatelliteSystem(1) );
      gpstk::SP3SatID Redirected (6, gpstk::SatID::SatelliteSystem(1) );

         //---------------------------------------------------------------------
         //Does the == Operator function?
         //---------------------------------------------------------------------
      testFramework.assert(  Compare == Equivalent ,
                             "Equivalence Operator found equivalent objects to not be equal"    , __LINE__);
      testFramework.assert(!(Compare == LessThanID),
                           "Equivalence Operator found differing IDs to be equal"             , __LINE__);
      testFramework.assert(!(Compare == DiffSatSys),
                           "Equivalence Operator found differing SatteliteSystems to be equal", __LINE__);


      testFramework.changeSourceMethod("OperatorNotEquals");
         //---------------------------------------------------------------------
         //Does the != Operator function?
         //---------------------------------------------------------------------
      testFramework.assert(!(Compare != Equivalent),
                           "Not Equals Operator found equivalent objects to be not equal"    , __LINE__);
      testFramework.assert(  Compare != LessThanID ,
                             "Not Equals Operator found differing IDs to be equal"             , __LINE__);
      testFramework.assert(  Compare != DiffSatSys ,
                             "Not Equals Operator found differing SatteliteSystems to be equal", __LINE__);


      testFramework.changeSourceMethod("OperatorLessThan");
         //---------------------------------------------------------------------
         //Does the < Operator function?
         //---------------------------------------------------------------------

         //ID only comparisons
      testFramework.assert(!(Compare < LessThanID),
                           "Less-than Operator found object with greater IDs and same SatSys to be less-than"
                           , __LINE__);
      testFramework.assert(  LessThanID < Compare ,
                             "Less-than Operator found object with lesser IDs and same SatSys to not be less-than",
                             __LINE__);
      testFramework.assert(!(Compare < Equivalent),
                           "Less-than Operator found equivalent object to be less-than"
                           , __LINE__);

         //SatelliteSystem only comparisons
      testFramework.assert(  Compare < DiffSatSys ,
                             "Less-than Operator found object with lesser SatSys and same IDs to not be less-than",
                             __LINE__);
      testFramework.assert(!(DiffSatSys < Compare),
                           "Less-than Operator found object with greater SatSys and same IDs to be less-than"
                           , __LINE__);

         //Completely different comparisons
      testFramework.assert(  Compare < DiffEvery  ,
                             "Less-than Operator found object with lesser SatSys and greater ID to not be less-than",
                             __LINE__);
      testFramework.assert(!(DiffEvery < Compare) ,
                           "Less-than Operator found object with greater SatSys and lesser ID to be less-than"
                           , __LINE__);
      testFramework.assert(!(Compare < DiffEvery2),
                           "Less-than Operator found object with greater SatSys and lesser ID to be less-than"
                           , __LINE__);
      testFramework.assert(  DiffEvery2 < Compare ,
                             "Less-than Operator found object with lesser SatSys and greater ID to not be less-than",
                             __LINE__);

      testFramework.changeSourceMethod("OperatorGreaterThan");
         //---------------------------------------------------------------------
         //Does the > Operator function?
         //---------------------------------------------------------------------

         //ID only comparisons
      testFramework.assert( (Compare > LessThanID),
                            "Greater-than Operator found object with greater IDs and same SatSys to not be greater-than",
                            __LINE__);
      testFramework.assert(!(LessThanID > Compare),
                           "Greater-than Operator found object with lesser IDs and same SatSys to be greater-than"
                           , __LINE__);
      testFramework.assert(!(Compare > Equivalent),
                           "Greater-than Operator found equivalent object to be greater-than"
                           , __LINE__);

         //SatelliteSystem only comparisons
      testFramework.assert(!(Compare > DiffSatSys),
                           "Greater-than Operator found object with lesser SatSys and same IDs to be greater-than"
                           , __LINE__);
      testFramework.assert( (DiffSatSys > Compare),
                            "Greater-than Operator found object with greater SatSys and same IDs to not be greater-than",
                            __LINE__);

         //Completely different comparisons
      testFramework.assert(!(Compare > DiffEvery) ,
                           "Greater-than Operator found object with lesser SatSys and greater ID to be greater-than"
                           , __LINE__);
      testFramework.assert( (DiffEvery > Compare) ,
                            "Greater-than Operator found object with greater SatSys and lesser ID to not be greater-than",
                            __LINE__);
      testFramework.assert( (Compare > DiffEvery2),
                            "Greater-than Operator found object with greater SatSys and lesser ID to not be greater-than",
                            __LINE__);
      testFramework.assert(!(DiffEvery2 > Compare),
                           "Greater-than Operator found object with lesser SatSys and greater ID to be greater-than"
                           , __LINE__);


      testFramework.changeSourceMethod("OperatorLessThanOrEqualTo");
         //---------------------------------------------------------------------
         //Does the <= Operator function?
         //---------------------------------------------------------------------

         //ID only comparisons
      testFramework.assert(!(Compare <= LessThanID),
                           "Less-than-or-equal-to Operator found object with greater IDs and same SatSys to be less-than-or-equal-to"
                           , __LINE__);
      testFramework.assert(  LessThanID <= Compare ,
                             "Less-than-or-equal-to Operator found object with lesser IDs and same SatSys to not be less-than-or-equal-to",
                             __LINE__);
      testFramework.assert( (Compare <= Equivalent),
                            "Less-than-or-equal-to Operator found equivalent object to not be less-than-or-equal-to"
                            , __LINE__);

         //SatelliteSystem only comparisons
      testFramework.assert(  Compare <= DiffSatSys ,
                             "Less-than-or-equal-to Operator found object with lesser SatSys and same IDs to not be less-than-or-equal-to",
                             __LINE__);
      testFramework.assert(!(DiffSatSys <= Compare),
                           "Less-than-or-equal-to Operator found object with greater SatSys and same IDs to be less-than-or-equal-to"
                           , __LINE__);

         //Completely different comparisons
      testFramework.assert(  Compare <= DiffEvery  ,
                             "Less-than-or-equal-to Operator found object with lesser SatSys and greater ID to not be less-than-or-equal-to",
                             __LINE__);
      testFramework.assert(!(DiffEvery <= Compare) ,
                           "Less-than-or-equal-to Operator found object with greater SatSys and lesser ID to be less-than-or-equal-to"
                           , __LINE__);
      testFramework.assert(!(Compare <= DiffEvery2),
                           "Less-than-or-equal-to Operator found object with greater SatSys and lesser ID to be less-than-or-equal-to"
                           , __LINE__);
      testFramework.assert(  DiffEvery2 <= Compare ,
                             "Less-than-or-equal-to Operator found object with lesser SatSys and greater ID to not be less-than-or-equal-to",
                             __LINE__);

      testFramework.changeSourceMethod("OperatorGreaterThanOrEqualTo");
         //---------------------------------------------------------------------
         //Does the >= Operator function?
         //---------------------------------------------------------------------

         //ID only comparisons
      testFramework.assert( (Compare >= LessThanID),
                            "Greater-than-or-equal-to Operator found object with greater IDs and same SatSys to not be greater-than-or-equal-to",
                            __LINE__);
      testFramework.assert(!(LessThanID >= Compare),
                           "Greater-than-or-equal-to Operator found object with lesser IDs and same SatSys to be greater-than-or-equal-to"
                           , __LINE__);
      testFramework.assert( (Compare >= Equivalent),
                            "Greater-than-or-equal-to Operator found equivalent object to not be greater-than-or-equal-to"
                            , __LINE__);

         //SatelliteSystem only comparisons
      testFramework.assert(!(Compare >= DiffSatSys),
                           "Greater-than-or-equal-to Operator found object with lesser SatSys and same IDs to be greater-than-or-equal-to"
                           , __LINE__);
      testFramework.assert( (DiffSatSys >= Compare),
                            "Greater-than-or-equal-to Operator found object with greater SatSys and same IDs to not be greater-than-or-equal-to",
                            __LINE__);

         //Completely different comparisons
      testFramework.assert(!(Compare >= DiffEvery) ,
                           "Greater-than-or-equal-to Operator found object with lesser SatSys and greater ID to be greater-than-or-equal-to"
                           , __LINE__);
      testFramework.assert( (DiffEvery >= Compare) ,
                            "Greater-than-or-equal-to Operator found object with greater SatSys and lesser ID to not be greater-than-or-equal-to",
                            __LINE__);
      testFramework.assert( (Compare >= DiffEvery2),
                            "Greater-than-or-equal-to Operator found object with greater SatSys and lesser ID to not be greater-than-or-equal-to",
                            __LINE__);
      testFramework.assert(!(DiffEvery2 >= Compare),
                           "Greater-than-or-equal-to Operator found object with lesser SatSys and greater ID to be greater-than-or-equal-to"
                           , __LINE__);


      testFramework.changeSourceMethod("OperatorRedirect");
         //---------------------------------------------------------------------
         //Does the << Operator function?
         //---------------------------------------------------------------------
      std::string outputString, compareString;
      std::stringstream outputStream;
      outputStream << Redirected;
      outputString = outputStream.str();
      compareString = "G06";
      testFramework.assert(outputString == compareString,
                           "Redirect operator did not function properly", __LINE__);


      return testFramework.countFails();
   }


//==========================================================================================================================
// fromStringTest verifies that the fromString method sets the SP3SatID attributes appropriately
//==========================================================================================================================
   int fromStringTest(void)
   {
      gpstk::TestUtil testFramework("SP3SatID", "fromString", __FILE__, __LINE__);

      std::string inputStringArray[12] = {"7", "07", "30", "E10", "E100", "G08", "E08", "R08", "L08", "C08", "J08", "M08"};
      int         expectedID[12]       = {  7,    7,   30,    10,    100,     8,     8,     8,     8,     8,     8,     8};
      gpstk::SatID::SatelliteSystem expectedSatSysArray[12] =
         {
            gpstk::SatID::systemGPS,
            gpstk::SatID::systemGPS,
            gpstk::SatID::systemGPS,
            gpstk::SatID::systemGalileo,
            gpstk::SatID::systemGalileo,
            gpstk::SatID::systemGPS,
            gpstk::SatID::systemGalileo,
            gpstk::SatID::systemGlonass,
            gpstk::SatID::systemLEO,
            gpstk::SatID::systemBeiDou,
            gpstk::SatID::systemQZSS,
            gpstk::SatID::systemMixed
         };
      gpstk::SP3SatID Compare;


      for (int i = 0; i < (sizeof(expectedID)/sizeof(expectedID[0])); i++)
      {
         try
         {
            Compare.fromString(inputStringArray[i]);
         }
         catch(gpstk::Exception& e)
         {
            std::stringstream s;
            s << "Exception in fromString call on input: " << inputStringArray[i];
            testFramework.assert(false,
                                 s.str(),__LINE__);
         }
         
         testFramework.assert(Compare.id == expectedID[i],
                              "fromString did not set the correct ID", __LINE__);
         testFramework.assert(Compare.system == expectedSatSysArray[i],
                              "fromString did not set the correct SatelliteSystem", __LINE__);
      }

         //Additional check to ensure fromString cannot set an improper string
      try
      {
         gpstk::SP3SatID Compare4("Z 1");
         testFramework.assert(false,
                              "fromString did not throw an exception for an improper string", __LINE__);
      }
      catch(gpstk::Exception& e)
      {
         testFramework.assert(true, "fromString threw the expected exception",
                              __LINE__);
      }

      return testFramework.countFails();
   }


//==========================================================================================================================
// toStringTest checks that a SP3SatID object can be output as a string
//==========================================================================================================================
   int toStringTest(void)
   {
      gpstk::TestUtil testFramework("SP3SatID", "toString", __FILE__, __LINE__);
      std::stringstream s;
      std::string outputString1, compareString1;
      std::string outputString2, compareString2;
      std::string outputString3, compareString3;
      std::string outputString4, compareString4;
      std::string outputString5, compareString5;
      std::string outputString6, compareString6;
      std::string outputString7, compareString7;
      std::string outputString8, compareString8;
      std::string outputString9, compareString9;
      
      try
      {
         gpstk::SP3SatID Compare1(5, gpstk::SatID::systemGPS);
         outputString1 = Compare1.toString();
         compareString1 = "G05";
         s.str("");
         s  << "toString did not return the expected string, exp: "
            << compareString1 << ", but got: " << outputString1;
         testFramework.assert(outputString1 == compareString1,
                              s.str(), __LINE__);
      }
      catch(gpstk::Exception& e)
      {
         s.str("Exception in toString call, expected output: ");
         s << compareString1;
         testFramework.assert(false,
                              s.str(),__LINE__);
      }

      try
      {
         gpstk::SP3SatID Compare2(20, gpstk::SatID::systemGalileo);
         outputString2 = Compare2.toString();
         compareString2 = "E20";
         s.str("");
         s  << "toString did not return the expected string, exp: "
            << compareString2 << ", but got: " << outputString2;
         testFramework.assert(outputString2 == compareString2,
                              s.str(), __LINE__);
      }
      catch(gpstk::Exception& e)
      {
         s.str("Exception in toString call, expected output: ");
         s << compareString2;
         testFramework.assert(false,
                              s.str(),__LINE__);
      }

      try
      {
         gpstk::SP3SatID Compare3(-5, gpstk::SatID::systemGPS);
         outputString3 = Compare3.toString();
         compareString3 = "G-5";
         s.str("");
         s  << "toString did not return the expected string, exp: "
            << compareString3 << ", but got: " << outputString3;
         testFramework.assert(outputString3 == compareString3,
                              s.str(), __LINE__);
      }
      catch(gpstk::Exception& e)
      {
         s.str("Exception in toString call, expected output: ");
         s << compareString3;
         testFramework.assert(false,
                              s.str(),__LINE__);
      }

      try
      {
         gpstk::SP3SatID Compare4(1, gpstk::SatID::systemGlonass);
         outputString4 = Compare4.toString();
         compareString4 = "R01";
         s.str("");
         s  << "toString did not return the expected string, exp: "
            << compareString4 << ", but got: " << outputString4;
         testFramework.assert(outputString4 == compareString4,
                              s.str(), __LINE__);
      }
      catch(gpstk::Exception& e)
      {
         s.str("Exception in toString call, expected output: ");
         s << compareString4;
         testFramework.assert(false,
                              s.str(),__LINE__);
      }

      try
      {
         gpstk::SP3SatID Compare5(1, gpstk::SatID::systemLEO);
         outputString5 = Compare5.toString();
         compareString5 = "L01";
         s.str("");
         s  << "toString did not return the expected string, exp: "
            << compareString5 << ", but got: " << outputString5;
         testFramework.assert(outputString5 == compareString5,
                              s.str(), __LINE__);
      }
      catch(gpstk::Exception& e)
      {
         s.str("Exception in toString call, expected output: ");
         s << compareString5;
         testFramework.assert(false,
                              s.str(),__LINE__);
      }

      try
      {
         gpstk::SP3SatID Compare6(1, gpstk::SatID::systemBeiDou);
         outputString6 = Compare6.toString();
         compareString6 = "C01";
         s.str("");
         s  << "toString did not return the expected string, exp: "
            << compareString6 << ", but got: " << outputString6;
         testFramework.assert(outputString6 == compareString6,
                              s.str(), __LINE__);
      }
      catch(gpstk::Exception& e)
      {
         s.str("Exception in toString call, expected output: ");
         s << compareString6;
         testFramework.assert(false,
                              s.str(),__LINE__);
      }

      try
      {
         gpstk::SP3SatID Compare7(1, gpstk::SatID::systemQZSS);
         outputString7 = Compare7.toString();
         compareString7 = "J01";
         s.str("");
         s  << "toString did not return the expected string, exp: "
            << compareString7 << ", but got: " << outputString7;
         testFramework.assert(outputString7 == compareString7,
                              s.str(), __LINE__);
      }
      catch(gpstk::Exception& e)
      {
         s.str("Exception in toString call, expected output: ");
         s << compareString7;
         testFramework.assert(false,
                              s.str(),__LINE__);
      }

      try
      {
         gpstk::SP3SatID Compare8(1, gpstk::SatID::systemMixed);
         outputString8 = Compare8.toString();
         compareString8 = "M01";
         s.str("");
         s  << "toString did not return the expected string, exp: "
            << compareString8 << ", but got: " << outputString8;
         testFramework.assert(outputString8 == compareString8,
                              s.str(), __LINE__);
      }
      catch(gpstk::Exception& e)
      {
         s.str("Exception in toString call, expected output: ");
         s << compareString8;
         testFramework.assert(false,
                              s.str(),__LINE__);
      }

      // finally, test that bad system inputs cause '?' outputs from toString
      try
      {
         gpstk::SP3SatID Compare9(1, gpstk::SatID::systemUnknown);
         outputString9 = Compare9.toString();
         compareString9 = "?-1";
         s.str("");
         s  << "toString did not return the expected string, exp: "
            << compareString9 << ", but got: " << outputString9;
         testFramework.assert(outputString9 == compareString9,
                              s.str(), __LINE__);
      }
      catch(gpstk::Exception& e)
      {
         s.str("Exception in toString call, expected output: ");
         s << compareString9;
         testFramework.assert(false,
                              s.str(),__LINE__);
      }
      
      return testFramework.countFails();
   }

};


int main() //Main function to initialize and run all tests above
{
   SP3SatID_T testClass;
   int check = 0,errorCounter = 0;

   check = testClass.initializationTest();
   errorCounter += check;

   check = testClass.operatorTest();
   errorCounter += check;

   check = testClass.fromStringTest();
   errorCounter += check;

   check = testClass.toStringTest();
   errorCounter += check;

   std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter <<
             std::endl;

   return errorCounter;
}
