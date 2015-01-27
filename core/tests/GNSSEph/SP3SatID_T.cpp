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
	SP3SatID_T(){}// Default Constructor, set the precision value
	~SP3SatID_T() {} // Default Desructor

	int initializationTest(void)
	{
		TestUtil testFramework("SP3SatID", "Explicit Constructor", __FILE__, __LINE__);
		testFramework.init();

		gpstk::SP3SatID Compare1(5, gpstk::SatID::SatelliteSystem(1));

//--------------SP3SatID_initializationTest_1 - Did the constructor store the correct data?
		testFramework.assert(Compare1.id == 5 && Compare1.system == gpstk::SatID::SatelliteSystem(1));
		testFramework.next();

		testFramework.changeSourceMethod("Default Constructor");
		gpstk::SP3SatID Compare2;

//--------------SP3SatID_initializationTest_2 - Did the constructor store the correct SatelliteSystem?
		testFramework.assert(Compare2.id == -1 && Compare2.system == gpstk::SatID::SatelliteSystem(1));
		testFramework.next();

		testFramework.changeSourceMethod("fromString Constructor");
		gpstk::SP3SatID Compare3("G 10");

//--------------SP3SatID_initializationTest_3 - Did the constructor store the proper data from a string?
		testFramework.assert(Compare3.id == 10 && Compare3.system == gpstk::SatID::SatelliteSystem(1));
		testFramework.next();

//--------------SP3SatID_initializationTest_4 - Did the constructor throw an exception for an improper string?
		try{gpstk::SP3SatID Compare4("Z 1"); testFramework.failTest();}
		catch(gpstk::Exception e) {testFramework.passTest();}

		testFramework.changeSourceMethod("SatID Constructor");
		gpstk::SatID sat1(7, gpstk::SatID::SatelliteSystem(1));
		gpstk::SP3SatID Compare4(sat1);

//--------------SP3SatID_initializationTest_5 - Did the constructor convert an SatID object to SP3SatID?
		testFramework.assert(Compare4.id == 7 && Compare4.system == gpstk::SatID::SatelliteSystem(1));

		return testFramework.countFails();
	}

	int operatorTest(void) //including <<
	{
		TestUtil testFramework( "SP3SatID", "== Operator", __FILE__, __LINE__);
		testFramework.init();		

		gpstk::SP3SatID Compare1(5, gpstk::SatID::SatelliteSystem (1));
		gpstk::SP3SatID Compare2(5, gpstk::SatID::SatelliteSystem (1));

//--------------SP3SatID_operatorTest_1 - Are equivalent objects equivalent?
		testFramework.assert(Compare1 == Compare2);
		testFramework.next();

		Compare1.id = 6;

		testFramework.changeSourceMethod("!= Operator");

//--------------SP3SatID_operatorTest_2 - Are nonequivalent objects nonequivalent due to id?
		testFramework.assert(Compare1 != Compare2);
		testFramework.next();

		Compare1.id = 5;
		Compare1.system = gpstk::SatID::SatelliteSystem (2);

//--------------SP3SatID_operatorTest_3 - Are nonequivalent objects nonequivalent due to system?
		testFramework.assert(Compare1 != Compare2);
		testFramework.next();

		Compare2.id = 6;
		Compare1.system = gpstk::SatID::SatelliteSystem (1);

		testFramework.changeSourceMethod("< Operator");

//--------------SP3SatID_operatorTest_4 - Is the object greater due to id?
		testFramework.assert(Compare1 < Compare2);
		testFramework.next();

//--------------SP3SatID_operatorTest_5 - Is the object greater due to id?
		testFramework.assert(!(Compare2 < Compare1));
		testFramework.next();

		Compare2.system = gpstk::SatID::SatelliteSystem (2);

//--------------SP3SatID_operatorTest_6 - Is the object greater due to system?
		testFramework.assert(Compare1 < Compare2);
		testFramework.next();

//--------------SP3SatID_operatorTest_7 - Is the object greater due to system?
		testFramework.assert(!(Compare2 < Compare1));
		testFramework.next();

		testFramework.changeSourceMethod("> Operator");

//--------------SP3SatID_operatorTest_8 - Is the object lesser due to id?
		testFramework.assert(Compare2 > Compare1);
		testFramework.next();

//--------------SP3SatID_operatorTest_9 - Is the object lesser due to id?
		testFramework.assert(!(Compare1 > Compare2));
		testFramework.next();

		Compare2.system = gpstk::SatID::SatelliteSystem (1);

//--------------SP3SatID_operatorTest_10 - Is the object lesser due to system?
		testFramework.assert(Compare2 > Compare1);
		testFramework.next();

//--------------SP3SatID_operatorTest_11 - Is the object lesser due to system?
		testFramework.assert(!(Compare1 > Compare2));
		testFramework.next();

		testFramework.changeSourceMethod("<< Operator");

		std::string outputString, compareString;
		std::stringstream outputStream;
		outputStream << Compare2;
		outputString = outputStream.str();
		compareString = "G06";

//--------------SP3SatID_operatorTest_12 - Was the data printed correctly?
		testFramework.assert(compareString == outputString);

		return testFramework.countFails();
	}

	int fromStringTest(void)
	{
		TestUtil testFramework("SP3SatID", "fromString", __FILE__, __LINE__);
		testFramework.init();

		gpstk::SP3SatID Compare1;

		Compare1.fromString("7");
//--------------SP3SatID_fromStringTest_1 - Did the method convert the string to SP3SatID properly?		
		testFramework.assert(Compare1.id == 7 && Compare1.system == gpstk::SatID::SatelliteSystem(1));
		testFramework.next();

		Compare1.fromString("07");
//--------------SP3SatID_fromStringTest_2 - Did the method convert the string to SP3SatID properly?		
		testFramework.assert(Compare1.id == 7 && Compare1.system == gpstk::SatID::SatelliteSystem(1));
		testFramework.next();		

		Compare1.fromString("30");
//--------------SP3SatID_fromStringTest_3 - Did the method convert the string to SP3SatID properly?		
		testFramework.assert(Compare1.id == 30 && Compare1.system == gpstk::SatID::SatelliteSystem(1));
		testFramework.next();

		Compare1.fromString("E10");
//--------------SP3SatID_fromStringTest_4 - Did the method convert the string to SP3SatID properly?		
		testFramework.assert(Compare1.id == 10 && Compare1.system == gpstk::SatID::SatelliteSystem(2));
		testFramework.next();		

		Compare1.fromString("E100");
//--------------SP3SatID_fromStringTest_5 - Did the method convert the string to SP3SatID properly?
		testFramework.assert(Compare1.id == 100 && Compare1.system == gpstk::SatID::SatelliteSystem(2));
		testFramework.next();

//--------------SP3SatID_fromStringTest_6 - Did the method throw the right error when given an invalid string?
		try{Compare1.fromString("Z 1"); testFramework.failTest();}
		catch(gpstk::Exception e) {testFramework.passTest();}

		return testFramework.countFails();
	}

	int toStringTest(void)
	{
		TestUtil testFramework("SP3SatID", "toString", __FILE__, __LINE__);
		testFramework.init();

		gpstk::SP3SatID Compare1(5, gpstk::SatID::SatelliteSystem(1));
		std::string outputString1, compareString1;
		outputString1 = Compare1.toString();
		compareString1 = "G05";

//--------------SP3SatID_toStringTest_1 - Was the correct string outputted?
		testFramework.assert(outputString1 == compareString1);
		testFramework.next();

		gpstk::SP3SatID Compare2(20, gpstk::SatID::SatelliteSystem(2));
		std::string outputString2, compareString2;
		outputString2 = Compare2.toString();
		compareString2 = "E20";

//--------------SP3SatID_toStringTest_2 - Was the correct string outputted?
		testFramework.assert(outputString2 == compareString2);
		testFramework.next();

		gpstk::SP3SatID Compare3(-5, gpstk::SatID::SatelliteSystem(1));
		std::string outputString3, compareString3;
		outputString3 = Compare3.toString();
		compareString3 = "G-5";

//--------------SP3SatID_toStringTest_3 - Was the correct string outputted?
		testFramework.assert(outputString3 == compareString3);
		testFramework.next();

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

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; 	
}
