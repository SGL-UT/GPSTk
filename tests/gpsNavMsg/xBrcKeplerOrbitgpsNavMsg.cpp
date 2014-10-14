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
*  Test program from August 2011.  Written to test the BrcKeplerOrbit 
*  module as it was being developed using the cppunit test module.
*
*********************************************************************/
#include <stdio.h>
#include "xBrcKeplerOrbitgpsNavMsg.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION (xBrcKeplerOrbitgpsNavMsg);

using namespace std;
using namespace gpstk;

void xBrcKeplerOrbitgpsNavMsg::setUp(void)
{
}

void xBrcKeplerOrbitgpsNavMsg::firstTest(void)
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
   short URAoe     = 5;
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
   CommonTime ToeCT = GPSWeekSecond(weeknum, Toe, TimeSystem::GPS);
   CommonTime beginFit = GPSWeekSecond(weeknum, Toe - 7200, TimeSystem::GPS);
   CommonTime endFit = GPSWeekSecond(weeknum, Toe + 7200, TimeSystem::GPS);

      // Test Data copied from RINEX file	
   double rToe      = 388800.0;
   short rweeknum   = 1638;     // By rules of Kepler Orbit, this must be week of Toe
   short rURAoe     = 5;
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
   CommonTime rToeCT = GPSWeekSecond(rweeknum, rToe, TimeSystem::GPS);  
  
   long subframe1[10] = { 0x22C2663D, 0x1F0E29B8, 0x2664002B, 0x09FCC1B6, 0x0F60EB8A,
                          0x1299CE93, 0x29CD3DB6, 0x0597BB0F, 0x00000B68, 0x17B28E5C };
   long subframe2[10] = { 0x22C2663D, 0x1F0E4A28, 0x05809675, 0x0EBD8AF1, 0x00089344,
                          0x008081F8, 0x1330CC2C, 0x0461E855, 0x034F8045, 0x17BB1E68 };
   long subframe3[10] = { 0x22C2663D, 0x1F0E6BA0, 0x3FE129CD, 0x26E31837, 0x0006C96A,
                          0x35A74DFC, 0x065C8B0F, 0x1E4F400A, 0x3FE8966D, 0x05860C44 };

   ofstream outf("Logs/BrcKeplerOrbit_Output", ios::out);

      // First test case.  Create an empty KO object, then load the data.
   outf << "Test Case 1: Creating an empty KO object and loading the data." << endl;
   BrcKeplerOrbit ko1;
   ko1.loadData( SysID, obsID, PRNID, beginFit, endFit, ToeCT, URAoe, healthy, 
		           Cuc, Cus, Crc, Crs, Cic, Cis, M0, dn, dnDot, ecc, A, Ahalf, Adot,
                 OMEGA0, i0, w, OMEGAdot, idot ); 

   Xv  xv1 = ko1.svXv( dt );
   outf.precision(11); 
   outf << "Position ko1: " << xv1.x[0] << ", " << xv1.x[1] << ", " << xv1.x[2] << ", " << endl;

      // Second test case.  Create an KO object with data available at time of construction.
   outf << endl << "Test Case 2: Creating KO object with data." << endl;
   BrcKeplerOrbit ko2( SysID, obsID, PRNID, beginFit, endFit, ToeCT, URAoe, healthy, 
		                 Cuc, Cus, Crc, Crs, Cic, Cis, M0, dn, dnDot, ecc, A, Ahalf, Adot, 
		                 OMEGA0, i0, w, OMEGAdot, idot ); 

   Xv  xv2 = ko2.svXv( dt ); 
   outf << "Position ko2: " << xv2.x << endl;
   outf << "Velocity ko2: " << xv2.v << endl;

      // Third test case.  Create a KO object using raw legacy navigation message data
   outf << endl << "Test Case 3: Creating KO object with raw legacy nav message data." << endl;
   BrcKeplerOrbit ko3(obsID, PRNID, weeknum, subframe1, subframe2, subframe3 );

   Xv  xv3 = ko3.svXv( dt ); 
   outf << "Position ko3: " << xv3.x << endl;
   outf << "Velocity ko3: " << xv3.v << endl;

      // Fourth test case.  Create a KO object using raw legacy navigation message data
   outf << endl << "Test Case 4: Creating KO object with raw legacy nav message data." << endl;
   BrcKeplerOrbit ko4;
   ko4.loadData(obsID, PRNID, weeknum, subframe1, subframe2, subframe3 );

   Xv  xv4 = ko4.svXv( dt ); 
   outf << "Position ko4: " << xv4.x << endl;
   outf << "Velocity ko4: " << xv4.v << endl;
   double RelCorr = ko4.svRelativity( dt ); 
   outf << "RelCorr ko4:  " << RelCorr<< endl;

      // Fifth test case.  Create an KO object with data available from RINEX file.
   outf << endl << "Test Case 5: Creating KO object with data from RINEX file." << endl;
   BrcKeplerOrbit ko5( SysID, obsID, PRNID, beginFit, endFit, rToeCT, rURAoe, healthy,
                       rCuc, rCus, rCrc, rCrs, rCic, rCis, rM0, rdn, rdnDot,
		                 recc, rA, rAhalf, rAdot, rOMEGA0, ri0, rw, rOMEGAdot, ridot ); 

   Xv  xv5 = ko5.svXv( dt ); 
   outf << "Position ko5: " << xv5.x << endl;
   outf << "Velocity ko5: " << xv5.v << endl;

      // Sixth test case.  Compare against "classic" EngEphemeris
   outf << endl << "Test Case 6: Calculated position using 'classic' EngEphemeris." << endl;
   outf << "Time= "<< g << endl;
   EngEphemeris EE;
   EE.addSubframe(subframe1, weeknum, 3, 1);
   EE.addSubframe(subframe2, weeknum, 3, 1);
   EE.addSubframe(subframe3, weeknum, 3, 1);

   Xvt xvt = EE.svXvt(dt);
   outf << "Position EE: " << xvt.x << endl;
   outf << "Velocity EE: " << xvt.v << endl;
   outf << "RelCorr EE:  " << EE.svRelativity(dt) << endl;

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
   outf << endl << "Test Case 7: Calculated position using 'classic' EngEphemeris." << endl;
   outf << "Time = " << ct2 << endl;
   EngEphemeris EEA;
   EEA.addSubframe(subframeA1, weeknumEEA, 9, 1);
   EEA.addSubframe(subframeA2, weeknumEEA, 9, 1);
   EEA.addSubframe(subframeA3, weeknumEEA, 9, 1);

   xvt = EEA.svXvt(dt2);
   outf << "Position EE: " << xvt.x << endl;
   outf << "Velocity EE: " << xvt.v << endl;
   outf << "Relativity : " << EE.svRelativity( dt2 ) << endl; 


   CivilTime ct3(2011, 6, 5, 1, 0, 0.0, TimeSystem::GPS );
   CommonTime dt3 = ct3.convertToCommonTime( );
   outf << endl << "Test Case 8: Calculated position using 'classic' EngEphemeris." << endl;
   outf << "Time = " << ct3 << endl;

   xvt = EEA.svXvt(dt3);
   outf << "Position EE: " << xvt.x << endl;
   outf << "Velocity EE: " << xvt.v << endl;
   outf << "Relativity : " << EE.svRelativity( dt3 ) << endl; 

      // Ninth test case. load setSubframe() methods from EngEphemeris
   outf << endl << "Test Case 9: loading setSubframe methods from EngEphemeris." << endl;
   EngEphemeris EEload;

   unsigned short tlm[3] = { 0x008B,
                             0x008B,
                             0x008B};
   long how[3] = { 381606,
                   381612,
                   381618};
   short asalert[3] = {1,1,1};
   EEload.loadData( SysID, tlm, how, asalert, rTracker, PRNID, rweeknum, rcflags, rURAoe, 
                    rhealth, riodc, rl2pdata, raodo,  rTgd, rToc, raf2,  raf1, raf0, riode,
                    rCrs, rdn, rM0,  rCuc, recc, rCus, rAhalf, rToe, rfitInt, rCic, rOMEGA0,
                    rCis, ri0, rCrc, rw,  rOMEGAdot, ridot); 

      // Tenth test case. load seSF methods from EngEphemeris
   outf << endl << "Test Case 10: loading setSF methods from EngEphemeris." << endl;
   EngEphemeris ESFload;

   unsigned tlm1  = 0x008B;
   double how1    = 381606;
   double how2    = 381612;
   double how3    = 381618;
   short asalert1 = 1;
   outf << "Before ESF1load.setSF1" << endl;

   ESFload.setSF1( tlm1,  how1, asalert1, rweeknum, rcflags, rURAoe, rhealth, riodc,
                   rl2pdata, rTgd, rToc, raf2, raf1, raf0, rTracker, PRNID );

   ESFload.setSF2( tlm1, how2, asalert1, riode, rCrs, rdn, rM0, rCuc, recc, rCus,
                   rAhalf, rToe, rfitInt );

   ESFload.setSF3( tlm1, how3, asalert1, rCic, rOMEGA0, rCis, ri0, rCrc, rw, rOMEGAdot, ridot );

   outf << endl << "dump output for Test Cases 4,6, and 9." << endl;
   
   outf << "EE dump: " << endl;
   outf << EE << endl;

   outf << "ko4 dump: "<< endl;
   outf << ko4 << endl;

   outf << "setSubframe methods: " << endl;
   outf << EEload << endl;

   outf << "SF1,2, and 3 methods: " << endl;
   outf << ESFload << endl;

   outf << EEA << endl;
   
   outf << endl;
   outf << "Fit Interval Tests"  << endl;
   outf << "BeginFit ko1:      " << GPSWeekSecond(ko1.getBeginningOfFitInterval()).printf("%F, %g") << endl;
   outf << "BeginFit ko2:      " << GPSWeekSecond(ko2.getBeginningOfFitInterval()).printf("%F, %g") << endl;
   outf << "BeginFit ko3:      " << GPSWeekSecond(ko3.getBeginningOfFitInterval()).printf("%F, %g") << endl;
   outf << "BeginFit ko4:      " << GPSWeekSecond(ko4.getBeginningOfFitInterval()).printf("%F, %g") << endl;
   outf << "BeginFit ko5:      " << GPSWeekSecond(ko5.getBeginningOfFitInterval()).printf("%F, %g") << endl;
   outf << "BeginFit EE:       " << GPSWeekSecond(EE.getOrbit().getBeginningOfFitInterval()).printf("%F, %g") << endl;
   outf << "BeginFit EEload:   " << GPSWeekSecond(EEload.getOrbit().getBeginningOfFitInterval()).printf("%F, %g") << endl;
   outf << "BeginFit ESFload:  " << GPSWeekSecond(ESFload.getOrbit().getBeginningOfFitInterval()).printf("%F, %g") << endl;

   outf << endl;
   outf << "BeginFit EEA:    " << GPSWeekSecond(EEA.getOrbit().getBeginningOfFitInterval()).printf("%F, %g") << endl;
   outf << endl;

   outf << "EndFit ko1:      " << GPSWeekSecond(ko1.getEndOfFitInterval()).printf("%F, %g") << endl;
   outf << "EndFit ko2:      " << GPSWeekSecond(ko2.getEndOfFitInterval()).printf("%F, %g") << endl;
   outf << "EndFit ko3:      " << GPSWeekSecond(ko3.getEndOfFitInterval()).printf("%F, %g") << endl;
   outf << "EndFit ko4:      " << GPSWeekSecond(ko4.getEndOfFitInterval()).printf("%F, %g") << endl;
   outf << "EndFit ko5:      " << GPSWeekSecond(ko5.getEndOfFitInterval()).printf("%F, %g") << endl;
   outf << "EndFit EE:       " << GPSWeekSecond(EE.getOrbit().getEndOfFitInterval()).printf("%F, %g") << endl;
   outf << "EndFit EEload:   " << GPSWeekSecond(EEload.getOrbit().getEndOfFitInterval()).printf("%F, %g") << endl;
   outf << "EndFit ESFload:  " << GPSWeekSecond(ESFload.getOrbit().getEndOfFitInterval()).printf("%F, %g") << endl;

   outf << endl;
   outf << "EndFit EEA:      " << GPSWeekSecond(EEA.getOrbit().getEndOfFitInterval()).printf("%F, %g") << endl;

   outf << endl;   
   outf << "Within Fit Interval ko1: " << ko1.withinFitInterval(dt) << endl;
   outf << "Within Fit Interval ko2: " << ko2.withinFitInterval(dt) << endl;
   outf << "Within Fit Interval ko3: " << ko3.withinFitInterval(dt) << endl;
   outf << "Within Fit Interval ko4: " << ko4.withinFitInterval(dt) << endl;
   outf << "Within Fit Interval ko5: " << ko5.withinFitInterval(dt) << endl;
   outf << "Within Fit Interval EE:  " << EE.getOrbit().withinFitInterval(dt) << endl;
   outf << "Within Fit Interval EEA: " << EEA.getOrbit().withinFitInterval(dt2) << endl;

   CPPUNIT_ASSERT(fileEqualTest((char*)"Logs/BrcKeplerOrbit_Truth",(char*)"Logs/BrcKeplerOrbit_Output"));
}

bool xBrcKeplerOrbitgpsNavMsg :: fileEqualTest (char* handle1, char* handle2)
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
