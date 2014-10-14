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
*  Test program from August 2011.  Written to test the CNAVClock 
*  module as it was being developed using the cppunit test module.
*
*********************************************************************/
#include <stdio.h>
#include "xCNAVClockgpsNavMsg.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xCNAVClockgpsNavMsg);

using namespace std;
using namespace gpstk;

void xCNAVClockgpsNavMsg::setUp(void)
{
}

void xCNAVClockgpsNavMsg::firstTest(void)
{
      // Set time to Day 153, 2011 (6/2/2011) at noon
   CivilTime g( 2011, 6, 2, 12, 14, 44.0, TimeSystem::GPS );
   CommonTime dt = g.convertToCommonTime();

      // Test data (copied from navdmp output for .....)
      // Generally, we'd load these data from the file
   std::string SysID = "G";
   ObsID obsID( ObsID::otNavMsg, ObsID::cbL2, ObsID::tcC2LM );
   ObsID obsID2( ObsID::otNavMsg, ObsID::cbL5, ObsID::tcIQ5 );
   short PRNID     = 3;
   double Toc      = 388800.0;
   short TOWWeek   = 1638;     // By rules of Clock Correction, this must be week of Toc
   double accuracy = 10.61;

      // Test Data copied from RINEX file	
   short rTOWWeek   = 1638;    // By rules of Clock Correction, this must be week of Toc
   double raccuracy = 10.61;
   double rToc      = 388800.0;
   double raf0      = 7.23189674318E-04;
   double raf1      = 5.11590769747E-12;
   double raf2      = 0.0;
   long TOWMsg_1    = 382500;
   long Top         = 378000;
   short AlertMsg   = 0;
   short URAoc_1    = 4;
   short URAoc1_1   = 1;
   short URAoc2_1   = 2;

      // Set time to Day 156, 2011 (6/5/2011) at 1 am
   CivilTime ct2( 2011, 6, 5, 1, 0, 0.0, TimeSystem::GPS );
   CommonTime dt2 = ct2.convertToCommonTime();

      // Test data (copied from navdmp output for PRN 7 Day 156, 2011 at 00:00:00 Transmit Time)
      // Generally, we'd load these data from the file
   short PRNID2     = 7;
   double Toc2      = 7200.0;
   short TOWWeek2   = 1639;     // By rules of Clock Correction, this must be week of Toc
   double accuracy2 = 10.61;
   double af0_2     = 1.32815912E-05;
   double af1_2     = 1.25055521E-12;
   double af2_2     = 0.0;
   long TOWMsg_2    = 0;
   long Top2        = 601200;
   short AlertMsg2  = 1;
   short URAoc_2    = 1;
   short URAoc1_2   = 2;
   short URAoc2_2   = 3;


      // Set time to Day 156, 2011 (6/5/2011) at midnight
   CivilTime ct3( 2011, 6, 5, 0, 0, 0.0, TimeSystem::GPS );
   CommonTime dt3 = ct2.convertToCommonTime();

      // Test data (copied from navdmp output for PRN 9 Day 155, 2011 at 22:00:00 Transmit Time)
      // Generally, we'd load these data from the file
   short PRNID3     = 9;
   double Toc3      = 0.0;
   short TOWWeek3   = 1638;     // By rules of Clock Corection, this must be week of Toc
   double accuracy3 = 10.61;
   long TOWMsg_3    = 597600;
   long Top3        = 594000;
   double af0_3     = 8.43554735E-05;
   double af1_3     = 2.38742359E-12;
   double af2_3     = 0.0;
   short health3    = 0;
   long TOW3        = 597600; 
   short URAoc_3    = 1;
   short URAoc1_3   = 2;
   short URAoc2_3   = 3;

  
   long subframe1[10] = { 0x22C2663D, 0x1F0E29B8, 0x2664002B, 0x09FCC1B6, 0x0F60EB8A,
                          0x1299CE93, 0x29CD3DB6, 0x0597BB0F, 0x00000B68, 0x17B28E5C };
   long subframe2[10] = { 0x22C2663D, 0x1F0E4A28, 0x05809675, 0x0EBD8AF1, 0x00089344,
                         0x008081F8, 0x1330CC2C, 0x0461E855, 0x034F8045, 0x17BB1E68 };
   long subframe3[10] = { 0x22C2663D, 0x1F0E6BA0, 0x3FE129CD, 0x26E31837, 0x0006C96A,
                          0x35A74DFC, 0x065C8B0F, 0x1E4F400A, 0x3FE8966D, 0x05860C44 };

   ofstream outf("Logs/CNAVClock_Output", ios::out);
   outf.precision(11);

      // First test case.  Create an CC object with data available from RINEX file.
   outf << endl << "Test Case 1: Creating CC object with data from RINEX file." << endl;
   outf << "Time = " << g << endl;
   CNAVClock cc1;
   cc1.loadData( SysID, obsID, PRNID, AlertMsg, TOWMsg_1, rTOWWeek, Top,
                 rToc, raccuracy, URAoc_1, URAoc1_1, URAoc2_1, raf0, raf1, raf2 );

   double ClkCorr1 = cc1.svClockBias( dt );
   double ClkDrift1 = cc1.svClockDrift( dt ); 
   outf << "Clock Bias cc1:         " << ClkCorr1 << endl;
   outf << "Clock Drift cc1:        " << ClkDrift1 << endl;
   outf << "Time of Prediction cc1: " << GPSWeekSecond(cc1.getTimeOfPrediction()).printf("%F, %g") << endl;
   outf << "CNAV Accuracy Test:     " << SV_CNAV_ACCURACY_GPS_MAX_INDEX[URAoc_1+15] << endl;
   outf << "legacy Accuracy Test:   " << SV_ACCURACY_GPS_MAX_INDEX[URAoc_1] << endl;

      // Second test case.  Create an CC object with data available from navdump.
   outf << endl << "Test Case 2: Creating CC object with data from navdump." << endl;
   outf << "Time = " << ct2 << endl;
   CNAVClock cc2;
   cc2.loadData( SysID, obsID2, PRNID2, AlertMsg2, TOWMsg_2, TOWWeek2, Top2,
                 Toc2, accuracy2, URAoc_2, URAoc1_2, URAoc2_2, af0_2, af1_2, af2_2 );

   double ClkCorr2 = cc2.svClockBias( dt2 );
   double ClkDrift2 = cc2.svClockDrift( dt2 ); 
   outf << "Clock Bias cc2:  "        << ClkCorr2 << endl;
   outf << "Clock Drift cc2: "        << ClkDrift2 << endl; 
   outf << "Time of Prediction cc2: " << GPSWeekSecond(cc2.getTimeOfPrediction()).printf("%F, %g") << endl;

      // Third test case.  Create an CC object with data available from navdump.
   outf << endl << "Test Case 3: Creating CC object with data from navdump." << endl;
   outf << "Time = " << ct3 << endl;
   CNAVClock cc3;
   cc3.loadData( SysID, obsID, PRNID3, AlertMsg, TOWMsg_3, TOWWeek3, Top3,
                 Toc3, accuracy3, URAoc_3, URAoc1_3, URAoc2_3, af0_3, af1_3, af2_3 );

   double ClkCorr3 = cc3.svClockBias( dt3 );
   double ClkDrift3 = cc3.svClockDrift( dt3 ); 
   outf << "Clock Bias cc3:  "        << ClkCorr3 << endl;
   outf << "Clock Drift cc3: "        << ClkDrift3 << endl; 
   outf << "Time of Prediction cc3: " << GPSWeekSecond(cc3.getTimeOfPrediction()).printf("%F, %g") << endl;

      // Fourth test case.  Compare against "classic" EngEphemeris
   outf << endl << "Test Case 4: Calculated position using 'classic' EngEphemeris." << endl;
   outf<< "Time= "<< g << endl;
   EngEphemeris EE;
   EE.addSubframe(subframe1, TOWWeek, 3, 1);
   EE.addSubframe(subframe2, TOWWeek, 3, 1);
   EE.addSubframe(subframe3, TOWWeek, 3, 1);

   Xvt xvt = EE.svXvt(dt);
   outf<< "Clock Bias EE:  " << xvt.clkbias << endl;
   outf<< "Clocl Drift EE: " << xvt.clkdrift << endl;

   outf << endl;
   outf << "CC1 Object Dump:" << endl;
   outf << cc1 << endl;

   outf << endl;
   outf << "CC2 Object Dump:" << endl;
   outf << cc2 << endl;

   outf << endl;
   outf << "CC3 Object Dump:" << endl;
   outf << cc3 << endl;

   CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/CNAVClock_Truth",(char*)"Logs/CNAVClock_Output"));
}

bool xCNAVClockgpsNavMsg :: fileEqualTest (char* handle1, char* handle2)
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
