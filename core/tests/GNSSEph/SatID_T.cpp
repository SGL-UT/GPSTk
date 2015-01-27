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
	SatID_T(){}// Default Constructor, set the precision value
	~SatID_T() {} // Default Desructor

	int initializationTest(void)
	{
		TestUtil testFramework( "SatID", "Constructor", __FILE__, __LINE__);
		testFramework.init();

		gpstk::SatID Compare1(5, gpstk::SatID::SatelliteSystem (1));

//--------------SatID_initializationTest_1 - Did the constructor store the correct id?
		testFramework.assert(Compare1.id == 5);
		testFramework.next();

//--------------SatID_initializationTest_2 - Did the constructor store the correct SatelliteSystem?
		testFramework.assert(Compare1.system == gpstk::SatID::SatelliteSystem(1));
		testFramework.next();

		gpstk::SatID Compare2(0, gpstk::SatID::SatelliteSystem (12));

//--------------SatID_initializationTest_3 - Did the constructor store the 0 id?
		testFramework.assert(Compare2.id == 0);
		testFramework.next();

//--------------SatID_initializationTest_4 - Did the constructor store the undefined SatelliteSystem?
		testFramework.assert(Compare2.system == gpstk::SatID::SatelliteSystem(12));
		testFramework.next();

		gpstk::SatID Compare3(-1, gpstk::SatID::SatelliteSystem (-1));

//--------------SatID_initializationTest_5 - Did the constructor store the negative id?
		testFramework.assert(Compare3.id == -1);
		testFramework.next();

//--------------SatID_initializationTest_6 - Did the constructor store the negative SatelliteSystem?
		testFramework.assert(Compare3.system == gpstk::SatID::SatelliteSystem(-1));

		return testFramework.countFails();
	}	

	int dumpTest(void)
	{
		TestUtil testFramework( "SatID", "dump(std::stream)", __FILE__, __LINE__);
		testFramework.init();

		gpstk::SatID sat1(5, gpstk::SatID::SatelliteSystem (1));
		std::string outputString1, compareString1;
		std::stringstream outputStream1;

		sat1.dump(outputStream1);
		outputString1 = outputStream1.str();
		compareString1 = "GPS 5";

//--------------SatID_dumpTest_1 - Did the method output the valid GPS satellite properly?
		testFramework.assert(outputString1 == compareString1);
		testFramework.next();

		gpstk::SatID sat2(110, gpstk::SatID::SatelliteSystem (10));
		std::string outputString2, compareString2;
		std::stringstream outputStream2;

		sat2.dump(outputStream2);
		outputString2 = outputStream2.str();
		compareString2 = "UserDefined 110";

//--------------SatID_dumpTest_2 - Did the method output the invalid UserDefined satellite properly?
		testFramework.assert(outputString2 == compareString2);
		testFramework.next();

		gpstk::SatID sat3(-10, gpstk::SatID::SatelliteSystem (50));
		std::string outputString3, compareString3;
		std::stringstream outputStream3;

		sat3.dump(outputStream3);
		outputString3 = outputStream3.str();
		compareString3 = "?? -10";

//--------------SatID_dumpTest_3 - Did the method output the invalid ?? satellite properly?
		testFramework.assert(outputString3 == compareString3);

		return testFramework.countFails();

	}

	int asStringTest(void)
	{
		TestUtil testFramework( "SatID", "asStringTest", __FILE__, __LINE__);
		testFramework.init();

		std::string compareString1,compareString2,compareString3;

		gpstk::SatID sat1(5, gpstk::SatID::SatelliteSystem (1));
		compareString1 = "GPS 5";

//--------------SatID_asStringTest_1 - Did the asString method function as expected?
		testFramework.assert(gpstk::StringUtils::asString(sat1) == compareString1);
		testFramework.next();

		gpstk::SatID sat2(110, gpstk::SatID::SatelliteSystem (10));
		compareString2 = "UserDefined 110";

//--------------SatID_asStringTest_2 - Did the asString method function as expected?
		testFramework.assert(gpstk::StringUtils::asString(sat2) == compareString2);
		testFramework.next();

		gpstk::SatID sat3(-10, gpstk::SatID::SatelliteSystem (50));
		compareString3 = "?? -10";

//--------------SatID_asStringTest_3 - Did the asString method function as expected?
		testFramework.assert(gpstk::StringUtils::asString(sat3) == compareString3);

		return testFramework.countFails();
	}

	int operatorTest(void)
	{
		TestUtil testFramework( "SatID", "== Operator", __FILE__, __LINE__);
		testFramework.init();		

		gpstk::SatID Compare1(5, gpstk::SatID::SatelliteSystem (1));
		gpstk::SatID Compare2(5, gpstk::SatID::SatelliteSystem (1));

//--------------SatID_operatorTest_1 - Are equivalent objects equivalent?
		testFramework.assert(Compare1 == Compare2);
		testFramework.next();

		Compare1.id = 6;

		testFramework.changeSourceMethod("!= Operator");

//--------------SatID_operatorTest_2 - Are nonequivalent objects nonequivalent due to id?
		testFramework.assert(Compare1 != Compare2);
		testFramework.next();

		Compare1.id = 5;
		Compare1.system = gpstk::SatID::SatelliteSystem (2);

//--------------SatID_operatorTest_3 - Are nonequivalent objects nonequivalent due to system?
		testFramework.assert(Compare1 != Compare2);
		testFramework.next();

		Compare2.id = 6;
		Compare1.system = gpstk::SatID::SatelliteSystem (1);

		testFramework.changeSourceMethod("< Operator");

//--------------SatID_operatorTest_4 - Is the object greater due to id?
		testFramework.assert(Compare1 < Compare2);
		testFramework.next();

//--------------SatID_operatorTest_5 - Is the object greater due to id?
		testFramework.assert(!(Compare2 < Compare1));
		testFramework.next();

		Compare2.system = gpstk::SatID::SatelliteSystem (2);

//--------------SatID_operatorTest_6 - Is the object greater due to system?
		testFramework.assert(Compare1 < Compare2);
		testFramework.next();

//--------------SatID_operatorTest_7 - Is the object greater due to system?
		testFramework.assert(!(Compare2 < Compare1));
		testFramework.next();

		testFramework.changeSourceMethod("> Operator");

//--------------SatID_operatorTest_8 - Is the object lesser due to id?
		testFramework.assert(Compare2 > Compare1);
		testFramework.next();

//--------------SatID_operatorTest_9 - Is the object lesser due to id?
		testFramework.assert(!(Compare1 > Compare2));
		testFramework.next();

		Compare2.system = gpstk::SatID::SatelliteSystem (1);

//--------------SatID_operatorTest_10 - Is the object lesser due to system?
		testFramework.assert(Compare2 > Compare1);
		testFramework.next();

//--------------SatID_operatorTest_11 - Is the object lesser due to system?
		testFramework.assert(!(Compare1 > Compare2));
		testFramework.next();

		testFramework.changeSourceMethod("<< Operator");

		std::string outputString, compareString;
		std::stringstream outputStream;
		outputStream << Compare2;
		outputString = outputStream.str();
		compareString = "GPS 6";

//--------------SatID_operatorTest_12 - Was the data printed correctly? 
		testFramework.assert(outputString == compareString);

		return testFramework.countFails();
	}


	int isValidTest(void)
	{
		TestUtil testFramework( "SatID", "isValid()", __FILE__, __LINE__);
		testFramework.init();

		gpstk::SatID Compare1(5, gpstk::SatID::SatelliteSystem (1));

//--------------SatID_isValidTest_1 - Was the valid GPS satellite marked as valid?
		testFramework.assert(Compare1.isValid());
		testFramework.next();

		gpstk::SatID Compare2(1, gpstk::SatID::SatelliteSystem (14));

//--------------SatID_isValidTest_2 - Was the valid satellite with undefined SatelliteSystem marked as valid?
		testFramework.assert(Compare2.isValid());
		testFramework.next();

		gpstk::SatID Compare3(-1, gpstk::SatID::SatelliteSystem (-1));

//--------------SatID_isValidTest_3 - Was the invalid satellite with id<0 marked as invalid?
		testFramework.assert(!Compare3.isValid());
		testFramework.next();

		gpstk::SatID Compare4(100, gpstk::SatID::SatelliteSystem (-1));

//--------------SatID_isValidTest_4 - Was the invalid satellite with id>100 marked as invalid?
		testFramework.assert(!Compare4.isValid());
		testFramework.next();

		gpstk::SatID Compare5(0, gpstk::SatID::SatelliteSystem (1));

//--------------SatID_isValidTest_5 - Was the invalid satellite with id=0 marked as invalid?
		testFramework.assert(!Compare5.isValid());
		testFramework.next();

		gpstk::SatID Compare6(32, gpstk::SatID::SatelliteSystem (1));

//--------------SatID_isValidTest_6 - Was the valid GPS satellite marked as valid?
		testFramework.assert(Compare6.isValid());
		testFramework.next();

		gpstk::SatID Compare7(33, gpstk::SatID::SatelliteSystem (1));

//--------------SatID_isValidTest_7 - Was the invalid GPS satellite with id=33 marked as invalid?
		testFramework.assert(!Compare7.isValid());		

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

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
