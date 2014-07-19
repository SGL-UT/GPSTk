#pragma ident "$Id$"

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

#include "xMSC.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xMSC);

using namespace gpstk;

void xMSC :: setUp (void)
{
}

void xMSC :: firstTest (void)
{
	MSCStream Input("Logs/CoordFile");
	MSCStream Out("Logs/Output",ios::out);
	MSCData Data;

	Input >> Data;
	gpstk::YDSTime Time(2001L,360L,0.0,TimeSystem::Unknown);
	CPPUNIT_ASSERT_EQUAL(Time,Data.time);
	CPPUNIT_ASSERT_EQUAL((long unsigned) 11111,Data.station);
	CPPUNIT_ASSERT_EQUAL((string)"AAA    " ,Data.mnemonic);
	
	Out << Data;
	while (Input >> Data)
	{
		Out << Data;
	}

	MSCStream Input2("Logs/Output");
	MSCStream Out2("Logs/Output2",ios::out);
	MSCData Data2;
	while (Input2 >> Data2)
	{
		Out2 << Data2;
	}
	CPPUNIT_ASSERT(fileEqualTest("Logs/CoordFile","Logs/Output"));
	CPPUNIT_ASSERT(fileEqualTest("Logs/CoordFile","Logs/Output2"));
}

bool xMSC :: fileEqualTest (const char* handle1, const char* handle2)
{
	bool isEqual = false;
	
	ifstream File1;
	ifstream File2;
	
	std::string File1Line;
	std::string File2Line;
	
	File1.open(handle1);
	File2.open(handle2);
	
	while (!File1.eof())
	{
		if (File2.eof()) 
			return isEqual;
		getline (File1, File1Line);
		getline (File2, File2Line);

		if (File1Line != File2Line)
			return isEqual;
	}
	if (!File2.eof())
		return isEqual;
	else
		return isEqual = true;
}
