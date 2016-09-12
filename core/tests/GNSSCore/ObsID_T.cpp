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

#include "ObsID.hpp"

#include "TestUtil.hpp"
#include <iostream>
#include <sstream>

class ObsID_T
{
public:
	
   int initialization(void)
   {		
      gpstk::TestUtil testFramework("ObsID", "Default Constructor", __FILE__, __LINE__);
      std::string failMesg;

      gpstk::ObsID empty;

      failMesg = "Did the default constructor assign the right type value?";
      testFramework.assert(empty.type == gpstk::ObsID::otUnknown, failMesg, __LINE__);

      failMesg = "Did the default constructor assign the right band value?";
      testFramework.assert(empty.band == gpstk::ObsID::cbUnknown, failMesg, __LINE__);

      failMesg = "Did the default constructor assign the right code value?";
      testFramework.assert(empty.code == gpstk::ObsID::tcUnknown, failMesg, __LINE__);	

      testFramework.changeSourceMethod("Explicit Constructor");

      gpstk::ObsID Compare(gpstk::ObsID::otRange, gpstk::ObsID::cbL1, gpstk::ObsID::tcCA);

      failMesg = "Did the explicit constructor assign the right type value?";
      testFramework.assert(Compare.type == gpstk::ObsID::otRange, failMesg, __LINE__);

      failMesg = "Did the explicit constructor assign the right band value?";
      testFramework.assert(Compare.band == gpstk::ObsID::cbL1, failMesg, __LINE__);

      failMesg = "Did the explicit constructor assign the right code value?";
      testFramework.assert(Compare.code == gpstk::ObsID::tcCA, failMesg, __LINE__);

      return testFramework.countFails();
   }

   int dumpTest(void)
   {
      gpstk::TestUtil testFramework("ObsID", "dump", __FILE__, __LINE__);
      std::string failMesg;
      gpstk::ObsID Compare(gpstk::ObsID::otDoppler, gpstk::ObsID::cbL2, gpstk::ObsID::tcY);
		
      std::string outputString, referenceString;
      std::stringstream outputStream;

      Compare.dump(outputStream);
      outputString = outputStream.str();

      referenceString = "L2 GPSY doppler";

      failMesg = "Did the dump method function correctly?";
      testFramework.assert(outputString == referenceString, failMesg, __LINE__);

      return testFramework.countFails();
   }

   int asStringTest(void)
   {
      gpstk::TestUtil testFramework("ObsID", "asString", __FILE__, __LINE__);
      std::string failMesg;
      gpstk::ObsID Compare(gpstk::ObsID::otPhase, gpstk::ObsID::cbE5b, gpstk::ObsID::tcIE5);

      std::string outputString, referenceString;

      outputString = gpstk::StringUtils::asString(Compare);
      referenceString = "E5b GALI5 phase";

      failMesg = "Did the asString function work correctly?";
      testFramework.assert(outputString == referenceString, failMesg, __LINE__);

      return testFramework.countFails();
   }

      //reads in 3-4 length string
   int fromStringConstructorTest(void)
   {
      gpstk::TestUtil testFramework("ObsID", "Constructor from String", __FILE__, __LINE__);
      std::string failMesg;

         //set with invalid length
      failMesg = "[testing] ObsID constructor from invalid string, [expected] exception gpstk::Exception, [actual] threw no exception";		
      try{gpstk::ObsID invalidID("G 10 "); testFramework.assert(false, failMesg, __LINE__);}
      catch (gpstk::Exception e) {testFramework.assert(true, failMesg, __LINE__);}

      try{gpstk::ObsID invalidID("G1"); testFramework.assert(false, failMesg, __LINE__);}
      catch (gpstk::Exception e) {testFramework.assert(true, failMesg, __LINE__);}

         //testing base assign w/out using any of the reused codes
      gpstk::ObsID obs1("GC1C"); // GPS L1 C/A PseudoRange

      failMesg = "Was the type value stored correctly?";
      testFramework.assert(obs1.type == gpstk::ObsID::otRange, failMesg, __LINE__);

      failMesg = "Was the band value stored correctly?";
      testFramework.assert(obs1.band == gpstk::ObsID::cbL1, failMesg, __LINE__);

      failMesg = "Was the code value stored correctly?";
      testFramework.assert(obs1.code == gpstk::ObsID::tcCA, failMesg, __LINE__);

         //testing only case of reassinged codes for GPS
      gpstk::ObsID obs2("GD5X"); // GPS L5 IQ Doppler

      failMesg = "Was the type value stored correctly?";
      testFramework.assert(obs2.type == gpstk::ObsID::otDoppler, failMesg, __LINE__);

      failMesg = "Was the band value stored correctly?";
      testFramework.assert(obs2.band == gpstk::ObsID::cbL5, failMesg, __LINE__);

      failMesg = "Was the code value stored correctly?";
      testFramework.assert(obs2.code == gpstk::ObsID::tcIQ5, failMesg, __LINE__);

         //testing completely random case
      gpstk::ObsID obs3("JL6L"); // QZSS E6 L Carrier Phase

      failMesg = "Was the type value stored correctly?";
      testFramework.assert(obs3.type == gpstk::ObsID::otPhase, failMesg, __LINE__);

      failMesg = "Was the band value stored correctly?";
      testFramework.assert(obs3.band == gpstk::ObsID::cbE6, failMesg, __LINE__);

      failMesg = "Was the code value stored correctly?";
      testFramework.assert(obs3.code == gpstk::ObsID::tcJQ6, failMesg, __LINE__);

      return testFramework.countFails();
   }

   int operatorTest(void)
   {
      gpstk::TestUtil testFramework("ObsID", "== Operator", __FILE__, __LINE__);
      std::string failMesg;

      gpstk::ObsID Compare1(gpstk::ObsID::otRange, gpstk::ObsID::cbL1, gpstk::ObsID::tcCA);
      gpstk::ObsID Compare2(gpstk::ObsID::otRange, gpstk::ObsID::cbL1, gpstk::ObsID::tcCA);
      gpstk::ObsID Compare3(gpstk::ObsID::otDoppler, gpstk::ObsID::cbL1, gpstk::ObsID::tcCA);		

      failMesg = "Are equivalent objects equivalent?";
      testFramework.assert(Compare1 == Compare2, failMesg, __LINE__);

      failMesg = "Are non-equivalent objects equivalent?";
      testFramework.assert(!(Compare1 == Compare3), failMesg, __LINE__);

      testFramework.changeSourceMethod("!= Operator");

      failMesg = "Are non-equivalent objects not equivalent?";
      testFramework.assert(Compare1 != Compare3, failMesg, __LINE__);

      failMesg = "Are equivalent objects not equivalent?";
      testFramework.assert(!(Compare1 != Compare2), failMesg, __LINE__);

      return testFramework.countFails();
   }


   int newIDTest(void)
   {
      gpstk::TestUtil testFramework("ObsID", "newID", __FILE__, __LINE__);
      std::string failMesg;

      failMesg = "[testing] ObsID::newID to redefine existing ID, [expected] exception gpstk::Exception, [actual] threw no exception";
      try{gpstk::ObsID::newID("C6Z", "L6 Z range"); testFramework.assert(false, failMesg, __LINE__);}
      catch(gpstk::Exception e) {testFramework.assert(true, failMesg, __LINE__);}

         //create a fictional ID completely
      gpstk::ObsID fic(gpstk::ObsID::newID("T9W", "L9 W test"));

      failMesg = "Was the fictional type value stored in the map?";
      testFramework.assert(gpstk::ObsID::char2ot.count('T'), failMesg, __LINE__);

      failMesg = "Was the fictional band value stored in the map?";
      testFramework.assert(gpstk::ObsID::char2cb.count('9'), failMesg, __LINE__);

      failMesg = "Was the fictional code value stored in the map?";
      testFramework.assert(gpstk::ObsID::char2tc.count('W'), failMesg, __LINE__);

      failMesg = "Was the fictional type value stored in an ObsID?";
      testFramework.assert(fic.type == gpstk::ObsID::char2ot['T'], failMesg, __LINE__);

      failMesg = "Was the fictional band value stored in an ObsID?";
      testFramework.assert(fic.band == gpstk::ObsID::char2cb['9'], failMesg, __LINE__);

      failMesg = "Was the fictional code value stored in an ObsID?";
      testFramework.assert(fic.code == gpstk::ObsID::char2tc['W'], failMesg, __LINE__);

      failMesg = "[testing] ObsID::newID to redefine existing ID, [expected] exception gpstk::Exception, [actual] threw no exception";
      try{gpstk::ObsID::newID("T9W", "L9 W test"); testFramework.assert(false, failMesg, __LINE__);}
      catch(gpstk::Exception e) {testFramework.assert(true, failMesg, __LINE__);}

      return testFramework.countFails();
   }
};

int main()
{
	int check = 0, errorCounter = 0;
	ObsID_T testClass;

	check = testClass.initialization();
	errorCounter += check;

	check = testClass.fromStringConstructorTest();
	errorCounter += check;

	check = testClass.dumpTest();
	errorCounter += check;

	check = testClass.newIDTest();
	errorCounter += check;

	check = testClass.asStringTest();
	errorCounter += check;

	check = testClass.operatorTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter;
}
