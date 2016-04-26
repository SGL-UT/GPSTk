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
#include "SatID.hpp"

#include "TestUtil.hpp"
#include <iostream>
#include <string>
#include <sstream>

class SatID_T
{
public:
   SatID_T() {} // Default Constructor, set the precision value
   ~SatID_T() {} // Default Desructor


//==========================================================================================================================
// initializationTest ensures the constructors set the values properly
//==========================================================================================================================
   int initializationTest(void)
   {
      gpstk::TestUtil testFramework( "SatID", "Constructor", __FILE__, __LINE__);

      gpstk::SatID Compare1(5, gpstk::SatID::SatelliteSystem (1));
      testFramework.assert(Compare1.id == 5,
                           "Explicit constructor did not set the correct id value", __LINE__);
      testFramework.assert(Compare1.system == gpstk::SatID::SatelliteSystem(1),
                           "Explicit constructor did not set the correct SatelliteSystem", __LINE__);


      gpstk::SatID Compare2(0, gpstk::SatID::SatelliteSystem (12));
      testFramework.assert(Compare2.id == 0,
                           "Explicit constructor did not set the correct id value", __LINE__);
      testFramework.assert(Compare2.system == gpstk::SatID::SatelliteSystem(12),
                           "Explicit constructor did not set the correct SatelliteSystem", __LINE__);


      gpstk::SatID Compare3(-1, gpstk::SatID::SatelliteSystem (-1));
      testFramework.assert(Compare3.id == -1,
                           "Explicit constructor did not set the correct id value", __LINE__);
      testFramework.assert(Compare3.system == gpstk::SatID::SatelliteSystem(-1),
                           "Explicit constructor did not set the correct SatelliteSystem", __LINE__);

      return testFramework.countFails();
   }


//==========================================================================================================================
// dumpTest checks the output from SatID::dump meets its expectations
//==========================================================================================================================
   int dumpTest(void)
   {
      gpstk::TestUtil testFramework( "SatID", "dump(std::stream)", __FILE__, __LINE__);


         //---------------------------------------------------------------------
         //Output for GPS satellite and single digit ID
         //---------------------------------------------------------------------
      gpstk::SatID sat1(5, gpstk::SatID::SatelliteSystem (1));
      std::string outputString1, compareString1;
      std::stringstream outputStream1;

      sat1.dump(outputStream1);
      outputString1 = outputStream1.str();
      compareString1 = "GPS 5";

      testFramework.assert(outputString1 == compareString1,
                           "dump did not output the expected response", __LINE__);


         //---------------------------------------------------------------------
         //Output for invalid UserDefined satellite and triple digit ID
         //---------------------------------------------------------------------
      gpstk::SatID sat2(110, gpstk::SatID::SatelliteSystem (10));
      std::string outputString2, compareString2;
      std::stringstream outputStream2;

      sat2.dump(outputStream2);
      outputString2 = outputStream2.str();
      compareString2 = "UserDefined 110";

      testFramework.assert(outputString2 == compareString2,
                           "dump did not output the expected response", __LINE__);

         //---------------------------------------------------------------------
         //Output for invalid satellite and negative ID
         //---------------------------------------------------------------------
      gpstk::SatID sat3(-10, gpstk::SatID::SatelliteSystem (50));
      std::string outputString3, compareString3;
      std::stringstream outputStream3;

      sat3.dump(outputStream3);
      outputString3 = outputStream3.str();
      compareString3 = "?? -10";

      testFramework.assert(outputString3 == compareString3,
                           "dump did not output the expected response", __LINE__);

      return testFramework.countFails();

   }


//==========================================================================================================================
// asStringTest checks that a SatID object can be reported as a string
//==========================================================================================================================
   int asStringTest(void)
   {
      gpstk::TestUtil testFramework( "SatID", "asStringTest", __FILE__, __LINE__);


      std::string compareString1,compareString2,compareString3;

         //---------------------------------------------------------------------
         //Output for GPS satellite and single digit ID
         //---------------------------------------------------------------------
      gpstk::SatID sat1(5, gpstk::SatID::SatelliteSystem (1));
      compareString1 = "GPS 5";
      testFramework.assert(gpstk::StringUtils::asString(sat1) == compareString1,
                           "asString did not produce the expected result", __LINE__);


         //---------------------------------------------------------------------
         //Output for invalid UserDefined satellite and triple digit ID
         //---------------------------------------------------------------------
      gpstk::SatID sat2(110, gpstk::SatID::SatelliteSystem (10));
      compareString2 = "UserDefined 110";
      testFramework.assert(gpstk::StringUtils::asString(sat2) == compareString2,
                           "asString did not produce the expected result", __LINE__);


         //---------------------------------------------------------------------
         //Output for invalid satellite and negative ID
         //---------------------------------------------------------------------
      gpstk::SatID sat3(-10, gpstk::SatID::SatelliteSystem (50));
      compareString3 = "?? -10";
      testFramework.assert(gpstk::StringUtils::asString(sat3) == compareString3,
                           "asString did not produce the expected result", __LINE__);

      return testFramework.countFails();
   }


//==========================================================================================================================
// operatorTest verifies the various operators of the SatID class
//==========================================================================================================================
   int operatorTest(void)
   {
      gpstk::TestUtil testFramework( "SatID", "OperatorEquivalence", __FILE__, __LINE__);


      gpstk::SatID Compare    (5, gpstk::SatID::SatelliteSystem(2) );
      gpstk::SatID Equivalent (5, gpstk::SatID::SatelliteSystem(2) );
      gpstk::SatID LessThanID (2, gpstk::SatID::SatelliteSystem(2) );
      gpstk::SatID DiffSatSys (5, gpstk::SatID::SatelliteSystem(5) );
      gpstk::SatID DiffEvery  (2, gpstk::SatID::SatelliteSystem(5) );
      gpstk::SatID DiffEvery2 (7, gpstk::SatID::SatelliteSystem(1) );
      gpstk::SatID Redirected (6, gpstk::SatID::SatelliteSystem(1) );

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
      compareString = "GPS 6";

      testFramework.assert(outputString == compareString,
                           "Redirect operator did not function properly", __LINE__);

      return testFramework.countFails();
   }


//==========================================================================================================================
// isValidTest checks that the isValid method returns the proper value
//==========================================================================================================================
   int isValidTest(void)
   {
      gpstk::TestUtil testFramework( "SatID", "isValid()", __FILE__, __LINE__);


      gpstk::SatID Compare1(5  , gpstk::SatID::SatelliteSystem(1) );
      gpstk::SatID Compare2(1  , gpstk::SatID::SatelliteSystem(14));
      gpstk::SatID Compare3(-1 , gpstk::SatID::SatelliteSystem(-1));
      gpstk::SatID Compare4(100, gpstk::SatID::SatelliteSystem(-1));
      gpstk::SatID Compare5(0  , gpstk::SatID::SatelliteSystem(1) );
      gpstk::SatID Compare6(32 , gpstk::SatID::SatelliteSystem(1) );
      gpstk::SatID Compare7(50 , gpstk::SatID::SatelliteSystem(1) );
      gpstk::SatID Compare8(0  , gpstk::SatID::SatelliteSystem(1) );
      gpstk::SatID Compare9(-3 , gpstk::SatID::SatelliteSystem(1) );


      testFramework.assert( Compare1.isValid(),
                            "isValid returned false for a valid SatID"                        , __LINE__);
      testFramework.assert( Compare2.isValid(),
                            "isValid returned false for a valid undefined SatSys"             , __LINE__);
      testFramework.assert(!Compare3.isValid(),
                           "isValid returned true for an invalid SatSys with negative ID"    , __LINE__);
      testFramework.assert(!Compare4.isValid(),
                           "isValid returned true for an invalid SatSys with triple digit ID", __LINE__);
      testFramework.assert(!Compare5.isValid(),
                           "isValid returned true for an invalid SatSys with zero ID"        , __LINE__);
      testFramework.assert( Compare6.isValid(),
                            "isValid returned false for an valid GPS SatSys"                  , __LINE__);
      testFramework.assert(!Compare7.isValid(),
                           "isValid returned true for a GPS SatSys with ID > 32"             , __LINE__);
      testFramework.assert(!Compare8.isValid(),
                           "isValid returned true for a GPS SatSys with 0 ID"                , __LINE__);
      testFramework.assert(!Compare9.isValid(),
                           "isValid returned true for a GPS SatSys with negative ID"         , __LINE__);

      return testFramework.countFails();
   }

};


int main() //Main function to initialize and run all tests above
{
   SatID_T testClass;
   int check = 0,errorCounter = 0;

   check = testClass.initializationTest();
   errorCounter += check;

   check = testClass.dumpTest();
   errorCounter += check;

   check = testClass.operatorTest();
   errorCounter +=check;

   check = testClass.isValidTest();
   errorCounter += check;

   std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter <<
             std::endl;

   return errorCounter; //Return the total number of errors
}
