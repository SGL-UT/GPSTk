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
*  Test program from August 2011.  Written to test the EngEphemeris 
*  module using the CPPUNIT test module.
*
*********************************************************************/
#include <stdio.h>
#include "xEngEphemerisgpsNavMsg.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xEngEphemerisgpsNavMsg);

using namespace std;
using namespace gpstk;

void xEngEphemerisgpsNavMsg::setUp(void)
{
}

void xEngEphemerisgpsNavMsg::firstTest(void)
{
      // Set time to Day 153, 2011 (6/2/2011) at noon
   CivilTime g( 2011, 6, 2, 12, 14, 44.0, TimeSystem::GPS );
   CommonTime dt = g.convertToCommonTime();

      // Test data (copied from navdmp output for .....)
      // Generally, we'd load these data from the file
   std::string SysID = "G";
   ObsID obsID( ObsID::otUndefined, ObsID::cbL1, ObsID::tcCA );
   short PRNID     = 3;
   double Toe      = 388800.0;
   short weeknum   = 1638;     // By rules of Kepler Orbit, this must be week of Toe
   double accuracy = 10.61;
   bool healthy    = true;
   double Cuc      = 9.57399607E-07;
   double Cus      = 8.35768878E-06;
   double Crc      = 2.03562500E+02;
   double Crs      = 1.87812500E+01;
   double Cic      = -2.30967999E-07;
   double Cis      = 5.02914190E-08;
   double M0       = 1.05539163E+00;
   double dn       = 5.39093884E-09;
   double dnDot    = 0.0;                // Doesn't exist in legacy navigation message
   double ecc      = 1.42575260E-02;
   double Ahalf    = 5.15365528E+03; 
   double A        = Ahalf * Ahalf;      // Changed from legacy navigation message
   double Adot     = 0.0;                // Doesn't exist in legacy navigation message  
   double OMEGA0   = -2.16947563E+00;
   double i0       = 9.28692498E-01;
   double w        = 1.09154605E+00;
   double OMEGAdot = -8.56285668E-09;
   double idot     = 5.52880173E-10;
   CommonTime beginFit = GPSWeekSecond(weeknum, Toe - 7200, TimeSystem::GPS);
   CommonTime endFit = GPSWeekSecond(weeknum, Toe + 7200, TimeSystem::GPS);

      // Test Data copied from RINEX file	
   double rToe      = 388800.0;
   short rweeknum   = 1638;     // By rules of Kepler Orbit, this must be week of Toe
   double raccuracy = 10.61;
   short raccflag   = 0;
   bool rhealthy    = true;
   short rhealth    = 0;
   short riodc      = 22;
   short rfitInt    = 0;
   short rl2pdata   = 0;
   short rcflags    = 1;
   short riode      = 22;
   long raodo       = 10;
   double rToc      = 388800.0;
   short rTracker   = 1;
   double rCuc      = 9.57399606705E-07;
   double rCus      = 8.35768878460E-06;
   double rCrc      = 2.03562500000E+02;
   double rCrs      = 1.87812500000E+01;
   double rCic      = -2.30967998505E-07;
   double rCis      = 5.02914190292E-08;
   double rM0       = 1.05539162795E+00;
   double rdn       = 5.39093883996E-09;
   double rdnDot    = 0.0;                // Doesn't exist in legacy navigation message
   double recc      = 1.42575260252E-02;
   double rAhalf    = 5.15365527534E+03; 
   double rA        = rAhalf * rAhalf;    // Changed from legacy navigation message
   double rAdot     = 0.0;                // Doesn't exist in legacy navigation message  
   double rOMEGA0   = -2.16947563164E+00;
   double ri0       = 9.28692497530E-01;
   double rw        = 1.09154604931E+00;
   double rOMEGAdot = -8.56285667735E-09;
   double ridot     = 5.52880172536E-10;
   double raf0      = 7.23189674318E-04;
   double raf1      = 5.11590769747E-12;
   double raf2      = 0.0;
   double rTgd      = -4.65661287308E-09;
  
   long subframe1[10] = { 0x22C2663D, 0x1F0E29B8, 0x2664002B, 0x09FCC1B6, 0x0F60EB8A,
                          0x1299CE93, 0x29CD3DB6, 0x0597BB0F, 0x00000B68, 0x17B28E5C };
   long subframe2[10] = { 0x22C2663D, 0x1F0E4A28, 0x05809675, 0x0EBD8AF1, 0x00089344,
                          0x008081F8, 0x1330CC2C, 0x0461E855, 0x034F8045, 0x17BB1E68 };
   long subframe3[10] = { 0x22C2663D, 0x1F0E6BA0, 0x3FE129CD, 0x26E31837, 0x0006C96A,
                          0x35A74DFC, 0x065C8B0F, 0x1E4F400A, 0x3FE8966D, 0x05860C44 };

   ofstream outf("Logs/EngEphemeris_Output", ios::out);

      // First test case. Create an EE object with data available from RINEX file.
   outf << endl << "Test Case 1: Calculated position and clock correction using 'classic' EngEphemeris." << endl;
   outf << "Time= "<< g << endl;
   EngEphemeris EE;
   EE.addSubframe(subframe1, weeknum, 3, 1);
   EE.addSubframe(subframe2, weeknum, 3, 1);
   EE.addSubframe(subframe3, weeknum, 3, 1);

   Xvt xvt = EE.svXvt(dt);
   outf << "Position EE: "    << xvt.x << endl;
   outf << "Velocity EE: "    << xvt.v << endl;
   outf << "RelCorr EE:  "    << EE.svRelativity(dt) << endl;
   outf << "Clock Bias EE:  " << xvt.clkbias << endl;
	outf << "Clock Drift EE: " << xvt.clkdrift << endl;

      // Test data (copied from navdmp output for PRN 9 Day 155, 2011 at 2200 transmit time)
   long subframeA1[10] = { 0x22C2663D, 0x30A2291C, 0x2664002B, 0x0DB9B68A, 0x12746316,
                           0x0BAC1EAA, 0x0DA73D35, 0x1A80002C, 0x00000574, 0x02C3A0F4 };
   long subframeA2[10] = { 0x22C2663D, 0x30A24A8C, 0x1A80864C, 0x0C15B3B1, 0x0AD1AB66,
                           0x00B00201, 0x3A1D9937, 0x00F6A87A, 0x0353C6C1, 0x00001F0C };
   long subframeA3[10] = { 0x22C2663D, 0x30A26B04, 0x3FDF944D, 0x2E5CB356, 0x002FCA3A,
                           0x040A9DDC, 0x0B45D00B, 0x03922318, 0x3FE905EF, 0x1A817FAC };
   short weeknumEEA = 1639;

   CivilTime ct2(2011, 6, 4, 23, 30, 0.0, TimeSystem::GPS );
   CommonTime dt2 = ct2.convertToCommonTime( );
   outf << endl << "Test Case 2: Calculated position and clock correction using 'classic' EngEphemeris." << endl;
   outf << "Time = " << ct2 << endl;
   EngEphemeris EEA;
   EEA.addSubframe(subframeA1, weeknumEEA, 9, 1);
   EEA.addSubframe(subframeA2, weeknumEEA, 9, 1);
   EEA.addSubframe(subframeA3, weeknumEEA, 9, 1);

   xvt = EEA.svXvt(dt2);
   outf << "Position EE: " << xvt.x << endl;
   outf << "Velocity EE: " << xvt.v << endl;
   outf << "Relativity : " << EE.svRelativity( dt2 ) << endl;
   outf << "Clock Bias EE:  " << xvt.clkbias << endl;
	outf << "Clock Drift EE: " << xvt.clkdrift << endl; 


   CivilTime ct3(2011, 6, 5, 1, 0, 0.0, TimeSystem::GPS );
   CommonTime dt3 = ct3.convertToCommonTime( );
   outf << endl << "Test Case 3: Calculated position and clock correction using 'classic' EngEphemeris." << endl;
   outf << "Time = " << ct3 << endl;

   xvt = EEA.svXvt(dt3);
   outf << "Position EE: " << xvt.x << endl;
   outf << "Velocity EE: " << xvt.v << endl;
   outf << "Relativity : " << EE.svRelativity( dt3 ) << endl; 
   outf << "Clock Bias EE:  " << xvt.clkbias << endl;
	outf << "Clock Drift EE: " << xvt.clkdrift << endl;

      // Fourth test case. load setSubframe() methods from EngEphemeris
   outf << endl << "Test Case 4: loading setSubframe methods from EngEphemeris." << endl;
   EngEphemeris EEload;

   unsigned short tlm[3] = { 0x008B,
                             0x008B,
                             0x008B};
   long how[3] = { 381606,
                   381612,
                   381618};
   short asalert[3] = {1,1,1};
   EEload.loadData( SysID, tlm, how, asalert, rTracker, PRNID, rweeknum, rcflags, raccflag, 
                    rhealth, riodc, rl2pdata, raodo,  rTgd, rToc, raf2,  raf1, raf0, riode,
                    rCrs, rdn, rM0,  rCuc, recc, rCus, rAhalf, rToe, rfitInt, rCic, rOMEGA0,
                    rCis, ri0, rCrc, rw,  rOMEGAdot, ridot); 

      // Fifth test case. load setSF methods from EngEphemeris
   outf << endl << "Test Case 5: loading setSF methods from EngEphemeris." << endl;
   EngEphemeris ESFload;

   unsigned tlm1  = 0x008B;
   double how1    = 381606;
   double how2    = 381612;
   double how3    = 381618;
   short asalert1 = 1;

   ESFload.setSF1( tlm1,  how1, asalert1, rweeknum, rcflags, raccflag, rhealth, riodc,
                   rl2pdata, rTgd, rToc, raf2, raf1, raf0, rTracker, PRNID );

   ESFload.setSF2( tlm1, how2, asalert1, riode, rCrs, rdn, rM0, rCuc, recc, rCus,
                   rAhalf, rToe, rfitInt );

   ESFload.setSF3( tlm1, how3, asalert1, rCic, rOMEGA0, rCis, ri0, rCrc, rw, rOMEGAdot, ridot );

   outf << endl << "Dump Output:" << endl;
   
   outf << "EE dump: " << endl;
   outf << EE << endl;

   outf << "setSubframe methods: " << endl;
   outf << EEload << endl;

   outf << "SF1,2, and 3 methods: " << endl;
   outf << ESFload << endl;

   outf << "EEA dump: " << endl;
   outf << EEA << endl;
   
   outf << endl;
   outf << "Fit Interval Tests"  << endl;
   outf << "BeginFit EE:       " << GPSWeekSecond(EE.getOrbit().getBeginningOfFitInterval()).printf("%F, %g") << endl;
   outf << "BeginFit EEload:   " << GPSWeekSecond(EEload.getOrbit().getBeginningOfFitInterval()).printf("%F, %g") << endl;
   outf << "BeginFit ESFload:  " << GPSWeekSecond(ESFload.getOrbit().getBeginningOfFitInterval()).printf("%F, %g") << endl;

   outf << endl;
   outf << "BeginFit EEA:    " << GPSWeekSecond(EEA.getOrbit().getBeginningOfFitInterval()).printf("%F, %g") << endl;
   outf << endl;

   outf << "EndFit EE:       " << GPSWeekSecond(EE.getOrbit().getEndOfFitInterval()).printf("%F, %g") << endl;
   outf << "EndFit EEload:   " << GPSWeekSecond(EEload.getOrbit().getEndOfFitInterval()).printf("%F, %g") << endl;
   outf << "EndFit ESFload:  " << GPSWeekSecond(ESFload.getOrbit().getEndOfFitInterval()).printf("%F, %g") << endl;

   outf << endl;
   outf << "EndFit EEA:      " << GPSWeekSecond(EEA.getOrbit().getEndOfFitInterval()).printf("%F, %g") << endl;

   outf << endl;   
   outf << "Within Fit Interval EE:  " << EE.getOrbit().withinFitInterval(dt) << endl;
   outf << "Within Fit Interval EEA: " << EEA.getOrbit().withinFitInterval(dt2) << endl;

   CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/EngEphemeris_Truth",(char*)"Logs/EngEphemeris_Output"));
}

bool xEngEphemerisgpsNavMsg :: fileEqualTest (char* handle1, char* handle2)
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
