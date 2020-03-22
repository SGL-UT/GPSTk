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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#include "SatID.hpp"

#include "TestUtil.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <map>

class SatID_T
{
public:
   SatID_T() {} // Default Constructor, set the precision value
   ~SatID_T() {} // Default Desructor


      /// ensure the constructors set the values properly
   unsigned initializationTest()
   {
      TUDEF("SatID", "Constructor");

      gpstk::SatID compare1(5, gpstk::SatID::SatelliteSystem (1));
      TUASSERTE(int, 5, compare1.id);
      TUASSERTE(gpstk::SatID::SatelliteSystem,
                gpstk::SatID::SatelliteSystem(1),
                compare1.system);

      gpstk::SatID compare2(0, gpstk::SatID::SatelliteSystem (12));
      TUASSERTE(int, 0, compare2.id);
      TUASSERTE(gpstk::SatID::SatelliteSystem,
                gpstk::SatID::SatelliteSystem(12),
                compare2.system);

      gpstk::SatID compare3(-1, gpstk::SatID::SatelliteSystem (-1));
      TUASSERTE(int, -1, compare3.id);
      TUASSERTE(gpstk::SatID::SatelliteSystem,
                gpstk::SatID::SatelliteSystem(-1),
                compare3.system);

      TURETURN();
   }


      /// check the output from SatID::dump meets its expectations
   unsigned dumpTest()
   {
      TUDEF("SatID", "dump(std::stream)");

         //---------------------------------------------------------------------
         //Output for GPS satellite and single digit ID
         //---------------------------------------------------------------------
      gpstk::SatID sat1(5, gpstk::SatID::SatelliteSystem (1));
      std::string outputString1, compareString1;
      std::stringstream outputStream1;

      sat1.dump(outputStream1);
      outputString1 = outputStream1.str();
      compareString1 = "GPS 5";
      TUASSERTE(std::string, compareString1, outputString1);

         //---------------------------------------------------------------------
         //Output for invalid UserDefined satellite and triple digit ID
         //---------------------------------------------------------------------
      gpstk::SatID sat2(110, gpstk::SatID::SatelliteSystem (11));
      std::string outputString2, compareString2;
      std::stringstream outputStream2;

      sat2.dump(outputStream2);
      outputString2 = outputStream2.str();
      compareString2 = "UserDefined 110";
      TUASSERTE(std::string, compareString2, outputString2);

         //---------------------------------------------------------------------
         //Output for invalid satellite and negative ID
         //---------------------------------------------------------------------
      gpstk::SatID sat3(-10, gpstk::SatID::SatelliteSystem (50));
      std::string outputString3, compareString3;
      std::stringstream outputStream3;

      sat3.dump(outputStream3);
      outputString3 = outputStream3.str();
      compareString3 = "?? -10";
      TUASSERTE(std::string, compareString3, outputString3);

      TURETURN();

   }


      /// check that a SatID object can be reported as a string
   unsigned asStringTest()
   {
      TUDEF("SatID", "asStringTest");

      std::string compareString1,compareString2,compareString3;

         //---------------------------------------------------------------------
         //Output for GPS satellite and single digit ID
         //---------------------------------------------------------------------
      gpstk::SatID sat1(5, gpstk::SatID::SatelliteSystem (1));
      compareString1 = "GPS 5";
      TUASSERTE(std::string, compareString1,gpstk::StringUtils::asString(sat1));

         //---------------------------------------------------------------------
         //Output for invalid UserDefined satellite and triple digit ID
         //---------------------------------------------------------------------
      gpstk::SatID sat2(110, gpstk::SatID::SatelliteSystem (11));
      compareString2 = "UserDefined 110";
      TUASSERTE(std::string, compareString2,gpstk::StringUtils::asString(sat2));

         //---------------------------------------------------------------------
         //Output for invalid satellite and negative ID
         //---------------------------------------------------------------------
      gpstk::SatID sat3(-10, gpstk::SatID::SatelliteSystem (50));
      compareString3 = "?? -10";
      TUASSERTE(std::string, compareString3,gpstk::StringUtils::asString(sat3));

      TURETURN();
   }


      /// verify the various operators of the SatID class
   unsigned operatorTest()
   {
      TUDEF("SatID", "OperatorEquivalence");

      gpstk::SatID compare    (5, gpstk::SatID::SatelliteSystem(2));
      gpstk::SatID equivalent (5, gpstk::SatID::SatelliteSystem(2));
      gpstk::SatID lessThanID (2, gpstk::SatID::SatelliteSystem(2));
      gpstk::SatID diffSatSys (5, gpstk::SatID::SatelliteSystem(5));
      gpstk::SatID diffEvery  (2, gpstk::SatID::SatelliteSystem(5));
      gpstk::SatID diffEvery2 (7, gpstk::SatID::SatelliteSystem(1));
      gpstk::SatID redirected (6, gpstk::SatID::SatelliteSystem(1));

         //---------------------------------------------------------------------
         //Does the == Operator function?
         //---------------------------------------------------------------------
      TUASSERT( compare == equivalent);
      TUASSERT(!(compare == lessThanID));
      TUASSERT(!(compare == diffSatSys));

      TUCSM("operator!=");
         //---------------------------------------------------------------------
         //Does the != Operator function?
         //---------------------------------------------------------------------
      TUASSERT(!(compare != equivalent));
      TUASSERT( compare != lessThanID);
      TUASSERT( compare != diffSatSys);

      TUCSM("operator<");
         //---------------------------------------------------------------------
         //Does the < Operator function?
         //---------------------------------------------------------------------

         //ID only comparisons
      TUASSERT(!(compare < lessThanID));
      TUASSERT( lessThanID < compare);
      TUASSERT(!(compare < equivalent));

         //SatelliteSystem only comparisons
      TUASSERT( compare < diffSatSys);
      TUASSERT(!(diffSatSys < compare));

         //Completely different comparisons
      TUASSERT( compare < diffEvery);
      TUASSERT(!(diffEvery < compare));
      TUASSERT(!(compare < diffEvery2));
      TUASSERT( diffEvery2 < compare);

      TUCSM("operator>");
         //---------------------------------------------------------------------
         //Does the > Operator function?
         //---------------------------------------------------------------------

         //ID only comparisons
      TUASSERT((compare > lessThanID));
      TUASSERT(!(lessThanID > compare));
      TUASSERT(!(compare > equivalent));

         //SatelliteSystem only comparisons
      TUASSERT(!(compare > diffSatSys));
      TUASSERT((diffSatSys > compare));

         //Completely different comparisons
      TUASSERT(!(compare > diffEvery));
      TUASSERT((diffEvery > compare));
      TUASSERT((compare > diffEvery2));
      TUASSERT(!(diffEvery2 > compare));

      TUCSM("operator<=");
         //---------------------------------------------------------------------
         //Does the <= Operator function?
         //---------------------------------------------------------------------

         //ID only comparisons
      TUASSERT(!(compare <= lessThanID));
      TUASSERT( lessThanID <= compare);
      TUASSERT((compare <= equivalent));

         //SatelliteSystem only comparisons
      TUASSERT( compare <= diffSatSys);
      TUASSERT(!(diffSatSys <= compare));

         //Completely different comparisons
      TUASSERT( compare <= diffEvery);
      TUASSERT(!(diffEvery <= compare));
      TUASSERT(!(compare <= diffEvery2));
      TUASSERT( diffEvery2 <= compare);

      TUCSM("operator>=");
         //---------------------------------------------------------------------
         //Does the >= Operator function?
         //---------------------------------------------------------------------

         //ID only comparisons
      TUASSERT((compare >= lessThanID));
      TUASSERT(!(lessThanID >= compare));
      TUASSERT((compare >= equivalent));

         //SatelliteSystem only comparisons
      TUASSERT(!(compare >= diffSatSys));
      TUASSERT((diffSatSys >= compare));

         //Completely different comparisons
      TUASSERT(!(compare >= diffEvery));
      TUASSERT((diffEvery >= compare));
      TUASSERT((compare >= diffEvery2));
      TUASSERT(!(diffEvery2 >= compare));

      TUCSM("operator<<");
         //---------------------------------------------------------------------
         //Does the << Operator function?
         //---------------------------------------------------------------------

      std::string outputString, compareString;
      std::stringstream outputStream;
      outputStream << redirected;
      outputString = outputStream.str();
      compareString = "GPS 6";

      TUASSERTE(std::string, compareString, outputString);

      TURETURN();
   }


      /// check that the isValid method returns the proper value
   unsigned isValidTest()
   {
      TUDEF("SatID", "isValid()");

      gpstk::SatID compare1(5  , gpstk::SatID::SatelliteSystem(1));
      gpstk::SatID compare2(1  , gpstk::SatID::SatelliteSystem(15));
      gpstk::SatID compare3(-1 , gpstk::SatID::SatelliteSystem(-1));
      gpstk::SatID compare4(100, gpstk::SatID::SatelliteSystem(-1));
      gpstk::SatID compare5(0  , gpstk::SatID::SatelliteSystem(1));
      gpstk::SatID compare6(32 , gpstk::SatID::SatelliteSystem(1));
      gpstk::SatID compare7(50 , gpstk::SatID::SatelliteSystem(1));
      gpstk::SatID compare8(0  , gpstk::SatID::SatelliteSystem(1));
      gpstk::SatID compare9(-3 , gpstk::SatID::SatelliteSystem(1));

      TUASSERT(compare1.isValid());
      TUASSERT(compare2.isValid());
      TUASSERT(!compare3.isValid());
      TUASSERT(!compare4.isValid());
      TUASSERT(!compare5.isValid());
      TUASSERT(compare6.isValid());
      TUASSERT(!compare7.isValid());
      TUASSERT(!compare8.isValid());
      TUASSERT(!compare9.isValid());

      TURETURN();
   }

      /// Regression testing for string <-> enum translation
   unsigned stringConvertTest()
   {
      TUDEF("SatID", "convertSatelliteSystemToString");
      std::map<gpstk::SatID::SatelliteSystem, std::string> testVals =
         {
            { gpstk::SatID::systemGPS, "GPS" },
            { gpstk::SatID::systemGalileo, "Galileo" },
            { gpstk::SatID::systemGlonass, "GLONASS" },
            { gpstk::SatID::systemGeosync, "Geostationary" },
            { gpstk::SatID::systemLEO, "LEO" },
            { gpstk::SatID::systemTransit, "Transit" },
            { gpstk::SatID::systemBeiDou, "BeiDou" },
            { gpstk::SatID::systemQZSS, "QZSS" },
            { gpstk::SatID::systemIRNSS, "IRNSS" },
            { gpstk::SatID::systemMixed, "Mixed" },
            { gpstk::SatID::systemUserDefined, "UserDefined" },
            { gpstk::SatID::systemUnknown, "Unknown" }
         };

      for (const auto& tvi : testVals)
      {
         TUCSM("convertSatelliteSystemToString");
         TUASSERTE(std::string, tvi.second,
                   gpstk::SatID::convertSatelliteSystemToString(tvi.first));
         TUCSM("convertStringToSatelliteSystem");
         TUASSERTE(gpstk::SatID::SatelliteSystem, tvi.first,
                   gpstk::SatID::convertStringToSatelliteSystem(tvi.second));
      }

      TURETURN();
   }
};


int main() //Main function to initialize and run all tests above
{
   SatID_T testClass;
   unsigned errorTotal = 0;

   errorTotal += testClass.initializationTest();
   errorTotal += testClass.dumpTest();
   errorTotal += testClass.operatorTest();
   errorTotal += testClass.isValidTest();
   errorTotal += testClass.stringConvertTest();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal
             << std::endl;

   return errorTotal; //Return the total number of errors
}
