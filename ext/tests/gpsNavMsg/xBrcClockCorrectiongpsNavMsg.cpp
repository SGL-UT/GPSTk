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

/*********************************************************************
*  $Id:$
*
*  Test program from August 2011. Written to test the BrcClockCorrection 
*  module as it was being developed using the cppunit test module.
*
*********************************************************************/
#include <stdio.h>
#include "xBrcClockCorrectiongpsNavMsg.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xBrcClockCorrectiongpsNavMsg);

using namespace std;
using namespace gpstk;

void xBrcClockCorrectiongpsNavMsg::setUp(void)
{
}

void xBrcClockCorrectiongpsNavMsg::firstTest(void)
{
      // Set time to Day 153, 2011 (6/2/2011) at noon
   CivilTime g( 2011, 6, 2, 12, 14, 44.0, TimeSystem::GPS );
   CommonTime dt = g.convertToCommonTime();

      // Test data (copied from navdmp output for .....)
      // Generally, we'd load these data from the file
   std::string SysID = "G";
   ObsID obsID( ObsID::otUndefined, ObsID::cbL1, ObsID::tcCA );
   short PRNID     = 3;
   double Toc      = 388800.0;
   double Top      = 345600.0;
   short weeknum   = 1638;     // By rules of Clock Correction, this must be week of Toc
   short URAoc     = 5;
   short URAoc1    = 7;
   short URAoc2    = 7;
   bool healthy    = true;
   double af0      = 7.23189674E-04;
   double af1      = 5.11590770E-12;
   double af2      = 0.00000000E+00;
   CommonTime TocCT = GPSWeekSecond(weeknum, Toc, TimeSystem::GPS);
   CommonTime TopCT = GPSWeekSecond(weeknum, Top, TimeSystem::GPS);
  
      // Test Data copied from RINEX file	
   double rToc      = 388800.0;
   double rTop      = 345600.0;
   short rweeknum   = 1638;     // By rules of Clock Corection, this must be week of Toc
   short rURAoc     = 5;
   short rURAoc1    = 7;
   short rURAoc2    = 7;
   bool rhealthy    = true;
   double raf0      = 7.23189674318E-04;
   double raf1      = 5.11590769747E-12;
   double raf2      = 0.00000000000E+00;
   CommonTime rTocCT = GPSWeekSecond(rweeknum, rToc, TimeSystem::GPS);
   CommonTime rTopCT = GPSWeekSecond(rweeknum, rTop, TimeSystem::GPS);

   long subframe1[10] = { 0x22C2663D, 0x1F0E29B8, 0x2664002B, 0x09FCC1B6, 0x0F60EB8A,
                          0x1299CE93, 0x29CD3DB6, 0x0597BB0F, 0x00000B68, 0x17B28E5C };
   long subframe2[10] = { 0x22C2663D, 0x1F0E4A28, 0x05809675, 0x0EBD8AF1, 0x00089344,
                          0x008081F8, 0x1330CC2C, 0x0461E855, 0x034F8045, 0x17BB1E68 };
   long subframe3[10] = { 0x22C2663D, 0x1F0E6BA0, 0x3FE129CD, 0x26E31837, 0x0006C96A,
                          0x35A74DFC, 0x065C8B0F, 0x1E4F400A, 0x3FE8966D, 0x05860C44 };

   ofstream outf("Logs/BrcClkCorr_Output", ios::out);

      // First test case. Create an empty CC object, then load the data.
   outf << "Test Case 1: Creating an empty CC object and loading the data." << endl;
   BrcClockCorrection co1;
   co1.loadData( SysID, obsID, PRNID, TocCT, TopCT, URAoc, URAoc1, URAoc2, healthy, 
		           af0, af1, af2 ); 

   double ClkCorr1 = co1.svClockBias( dt );
   outf.precision(11); 
   outf << "Clock Correction co1: " << ClkCorr1 << endl << endl;

      // Second test case. Create an CC object with data available at time of construction.
   outf << "Test Case 2: Creating CC object with data." << endl;
   BrcClockCorrection co2( SysID, obsID, PRNID, TocCT, TopCT, URAoc, URAoc1, URAoc2, healthy, 
		                     af0, af1, af2 ); 

   double ClkCorr2 = co2.svClockBias( dt ); 
   outf << "ClockCorrection co2: " << ClkCorr2 << endl << endl;

      // Third test case. Create a CC object using raw legacy navigation message data
   outf << "Test Case 3: Creating CC object with raw legacy nav message data." << endl;
   BrcClockCorrection co3(obsID, PRNID, weeknum, subframe1 );

   double ClkCorr3 = co3.svClockBias( dt ); 
   outf << "Clock Correction co3: " << ClkCorr3 << endl << endl;

      // Fourth test case. Create a CC object using raw legacy navigation message data
   outf << "Test Case 4: Creating CC object with raw legacy nav message data." << endl;
   BrcClockCorrection co4;
   co4.loadData(obsID, PRNID, weeknum, subframe1 );

   double ClkCorr4 = co4.svClockBias( dt ); 
   outf << "Clock Correction co4: " << ClkCorr4 << endl << endl;

      // Fifth test case. Create an CC object with data available from RINEX file.
   outf << "Test Case 5: Creating CC object with data from RINEX file." << endl;
   BrcClockCorrection co5( SysID, obsID, PRNID, rTocCT, TopCT, URAoc, URAoc1, URAoc2, rhealthy, 
		                     raf0, raf1, raf2 ); 

   double ClkCorr5 = co5.svClockBias( dt ); 
   outf << "Clock Correction co5: " << ClkCorr5 << endl << endl;

      // Sixth test case. Compare against "classic" EngEphemeris
   outf << "Test Case 6: Calculated clock correction using 'classic' EngEphemeris." << endl;
   outf<< "Time= "<< g << endl;
   EngEphemeris EE;
   EE.addSubframe(subframe1, weeknum, 3, 1);
   EE.addSubframe(subframe2, weeknum, 3, 1);
   EE.addSubframe(subframe3, weeknum, 3, 1);

   Xvt xvt = EE.svXvt(dt);
   outf << "Clock Bias EE:  " << xvt.clkbias << endl;
	outf << "Clock Drift EE: " << xvt.clkdrift << endl;
 
      // Test data (copied from navdmp output for PRN 6 Day 155, 2011)
   long subframeA1[10] = { 0x22C2663D, 0x30A2291C, 0x2664002B, 0x0DB9B68A, 0x12746316,
                           0x0BAC1EAA, 0x0DA73D35, 0x1A80002C, 0x00000574, 0x02C3A0F4 };
   long subframeA2[10] = { 0x22C2663D, 0x30A24A8C, 0x1A80864C, 0x0C15B3B1, 0x0AD1AB66,
                           0x00B00201, 0x3A1D9937, 0x00F6A87A, 0x0353C6C1, 0x00001F0C };
   long subframeA3[10] = { 0x22C2663D, 0x30A26B04, 0x3FDF944D, 0x2E5CB356, 0x002FCA3A,
                           0x040A9DDC, 0x0B45D00B, 0x03922318, 0x3FE905EF, 0x1A817FAC };

   CivilTime ct2(2011, 6, 4, 11, 30, 0.0, TimeSystem::GPS );
   dt = ct2.convertToCommonTime( );
   outf << endl << "Test Case 7: Calculated clock corrections using 'classic' EngEphemeris." << endl;
   outf << "Time = " << ct2 << endl;
   EngEphemeris EEA;
   EEA.addSubframe(subframeA1, weeknum, 9, 1);
   EEA.addSubframe(subframeA2, weeknum, 9, 1);
   EEA.addSubframe(subframeA3, weeknum, 9, 1);

   xvt = EEA.svXvt(dt);
   outf << "Clock Bias EE:  "  << xvt.clkbias << endl;
   outf << "Clock Drift EE: " << xvt.clkdrift << endl;

   CivilTime ct3(2011, 6, 5, 1, 0, 0.0, TimeSystem::GPS );
   dt = ct3.convertToCommonTime( );
   outf << endl << "Test Case 8: Calculated clock corrections using 'classic' EngEphemeris." << endl;
   outf << "Time = " << ct3 << endl;

   xvt = EEA.svXvt(dt);
   outf << "Clock Bias EE:  " << xvt.clkbias << endl;
   outf << "Clock Drift EE: " << xvt.clkdrift << endl << endl; 
 
   outf << "Dump Output for Test Cases 4 and 6." << endl;

   outf << "dump EE:" << endl;
   outf << EE << endl;

   outf << "dump co4:" << endl;
   outf << co4 << endl;

   CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/BrcClkCorr_Truth",(char*)"Logs/BrcClkCorr_Output"));
}

bool xBrcClockCorrectiongpsNavMsg :: fileEqualTest (char* handle1, char* handle2)
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
