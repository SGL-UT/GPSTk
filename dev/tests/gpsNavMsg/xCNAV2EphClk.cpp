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
*  Test program from June 2011.  Written to test the CNAV2EphClk 
*  module as it was being developed.
*
*********************************************************************/
#include <stdio.h>

#include "CNAV2EphClk.hpp"
#include "EngEphemeris.hpp"
#include "CivilTime.hpp"
#include "CommonTime.hpp"
#include "GPSWeekSecond.hpp"

using namespace std;
using namespace gpstk;

int main( int argc, char * argv[] )
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
   short TOWWeek   = 1638;     // By rules of Kepler Orbit, this must be week of Toe
   double accuracy = 10.61;
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

      // Test Data copied from RINEX file	
   double rToe      = 388800.0;
   short rTOWWeek   = 1638;     // By rules of Kepler Orbit, this must be week of Toe
   double raccuracy = 10.61;
   short raccflag   = 0;
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

   double deltaA    = rA - A_REF;
   long TOW         = 382500; 
   long Top         = 378000;
   short URAoe      = 1;
   short URAoc      = 1;
   short URAoc1     = 2;
   short URAoc2     = 3;
   double ISCL1cp   = 1E-8;        // Synthetic, reasonable values
   double ISCL1cd   = -1E-8;       // Synthetic, reasonable values

      // Set time to Day 156, 2011 (6/5/2011) at 1 am
   CivilTime ct2( 2011, 6, 5, 1, 0, 0.0, TimeSystem::GPS );
   CommonTime dt2 = ct2.convertToCommonTime();

      // Test data (copied from navdmp output for PRN 7 Day 156, 2011 at 00:00:00 Transmit Time)
      // Generally, we'd load these data from the file
   short PRNID2     = 7;
   double Toe2      = 7200.0;
   short TOWWeek2   = 1639;     // By rules of Kepler Orbit, this must be week of Toe
   double accuracy2 = 10.61;
   double Cuc2      = 6.33299351E-08;
   double Cus2      = 9.79751348E-07;
   double Crc2      = 3.67843750E+02;
   double Crs2      = 3.75000000E+01;
   double Cic2      = 1.13621354E-07;
   double Cis2      = 2.79396772E-08;
   double M02       = 5.40667729E-01;
   double dn2       = 4.46804325E-09;
   double dnDot2    = 0.0;                // Doesn't exist in legacy navigation message
   double ecc2      = 4.48677479E-03;
   double Ahalf2    = 5.15365555E+03; 
   double A2        = Ahalf2 * Ahalf2;    // Changed from legacy navigation message
   double Adot2     = 0.0;                // Doesn't exist in legacy navigation message  
   double OMEGA02   = 2.03414883+00;
   double i02       = 9.74404003E-01;
   double w2        = -3.13294415E+00;
   double OMEGAdot2 = -8.35713382E-09;
   double idot2     = 1.21433630E-10;
   double af0_2     = 1.32815912E-05;
   double af1_2     = 1.25055521E-12;
   double af2_2     = 0.0;
   double Tgd2      = -1.07102096E-08;
   short health     = 0;
   double deltaA2   = A2 - A_REF;
   long TOW2        = 0; 
   long Top2        = 601200;
   short URAoe2     = 1;
   short URAoc_2    = 1;
   short URAoc1_2   = 2;
   short URAoc2_2   = 3;
   double ISCL1cp2  = 1E-8;        //Synthetic, reasonable values
   double ISCL1cd2  = -1E-8;       //Synthetic, reasonable values


      // Set time to Day 156, 2011 (6/5/2011) at midnight
   CivilTime ct3( 2011, 6, 5, 0, 0, 0.0, TimeSystem::GPS );
   CommonTime dt3 = ct2.convertToCommonTime();

      // Test data (copied from navdmp output for PRN 9 Day 155, 2011 at 22:00:00 Transmit Time)
      // Generally, we'd load these data from the file
   short PRNID3     = 9;
   double Toe3      = 0.0;
   short TOWWeek3   = 1638;     // By rules of Kepler Orbit, this must be week of Toe
   double accuracy3 = 10.61;
   double Cuc3      = 1.31130219E-06;
   double Cus3      = 1.83656812E-06;
   double Crc3      = 3.60718750E+02;
   double Crs3      = 1.67812500E+01;
   double Cic3      = -2.42143869E-07;
   double Cis3      = 3.55765224E-07;
   double M03       = -1.22303559E+00;
   double dn3       = 4.41946980E-09;
   double dnDot3    = 0.0;                // Doesn't exist in legacy navigation message
   double ecc3      = 1.73985478E-02;
   double Ahalf3    = 5.15366363E+03; 
   double A3        = Ahalf3 * Ahalf3;    // Changed from legacy navigation message
   double Adot3     = 0.0;                // Doesn't exist in legacy navigation message  
   double OMEGA03   = 2.00581875E+00;
   double i03       = 9.83297588E-01;
   double w3        = 1.57216573E+00;
   double OMEGAdot3 = -8.40356433E-09;
   double idot3     = 1.36791412E-10;
   double af0_3     = 8.43554735E-05;
   double af1_3     = 2.38742359E-12;
   double af2_3     = 0.0;
   double Tgd3      = -5.58793545E-09;
   short health3    = 0;
   double deltaA3   = A3 - A_REF;
   long TOW3        = 597600; 
   long Top3        = 594000;
   short URAoe3     = 1;
   short URAoc_3    = 1;
   short URAoc1_3   = 2;
   short URAoc2_3   = 3;
   double ISCL1cp3  = 1E-8;        // Synthetic, reasonable values
   double ISCL1cd3  = -1E-8;       // Synthetic, reasonable values

  
   long subframe1[10] = { 0x22C2663D, 0x1F0E29B8, 0x2664002B, 0x09FCC1B6, 0x0F60EB8A,
                          0x1299CE93, 0x29CD3DB6, 0x0597BB0F, 0x00000B68, 0x17B28E5C };
   long subframe2[10] = { 0x22C2663D, 0x1F0E4A28, 0x05809675, 0x0EBD8AF1, 0x00089344,
                         0x008081F8, 0x1330CC2C, 0x0461E855, 0x034F8045, 0x17BB1E68 };
   long subframe3[10] = { 0x22C2663D, 0x1F0E6BA0, 0x3FE129CD, 0x26E31837, 0x0006C96A,
                          0x35A74DFC, 0x065C8B0F, 0x1E4F400A, 0x3FE8966D, 0x05860C44 };

   cout.precision(11);

      // First test case.  Create an CEC object with data available from RINEX file.
   cout << endl << "Test Case 1: Creating CEC object with data from RINEX file." << endl;
   cout << "Time = " << g << endl;
   CNAV2EphClk cec;
   cec.loadData( SysID, obsID, PRNID, rTOWWeek, TOW, Top,
                    rhealth, URAoe, rToe, deltaA, rAdot, rdn,
                    rdnDot, rM0, recc, rw, rOMEGA0, ri0,
                    rOMEGAdot, ridot, rCic, rCis, rCrc, rCrs,
                    rCuc, rCus, URAoc, URAoc1, URAoc2, raf0,
                    raf1, raf2, rTgd, ISCL1cp, ISCL1cd);

   Xvt  xvt1 = cec.svXvt( dt ); 
   cout << "Position cec:    "        << xvt1.x << endl;
   cout << "Velocity cec:    "        << xvt1.v << endl;
   cout << "Clock Bias cec:  "        << xvt1.clkbias << endl;
   cout << "Clock Drift cec: "        << xvt1.clkdrift << endl;
   cout << "RelCorr cec:     "        << xvt1.relcorr << endl;
   cout << "Time of Prediction cec: " << GPSWeekSecond(cec.getTimeOfPrediction()).printf("%F, %g") << endl;
   cout<< "CNAV Accuracy Test:   "    << SV_CNAV_ACCURACY_MAX_INDEX[URAoe+15] << endl;
   cout<< "legacy Accuracy Test: "    << SV_ACCURACY_MAX_INDEX[URAoe] << endl;


      // Second test case.  Create an CEC object with data available from navdump.
   cout << endl << "Test Case 2: Creating CEC object with data from navdump." << endl;
   cout << "Time = " << ct2 << endl;
   CNAV2EphClk cec2;
   cec2.loadData( SysID, obsID, PRNID2, TOWWeek2, TOW2, Top2,
                    health, URAoe2, Toe2, deltaA2, Adot2, dn2,
                    dnDot2, M02, ecc2, w2, OMEGA02, i02,
                    OMEGAdot2, idot2, Cic2, Cis2, Crc2, Crs2,
                    Cuc2, Cus2, URAoc_2, URAoc1_2, URAoc2_2, af0_2,
                    af1_2, af2_2, Tgd2, ISCL1cp2, ISCL1cd2);

   Xvt  xvt2 = cec2.svXvt( dt2 ); 
   cout << "Position cec2:    "        << xvt2.x << endl;
   cout << "Velocity cec2:    "        << xvt2.v << endl;
   cout << "Clock Bias cec2:  "        << xvt2.clkbias << endl;
   cout << "Clock Drift cec2: "        << xvt2.clkdrift << endl;
   cout << "RelCorr cec2:     "        << xvt2.relcorr << endl;
   cout << "Time of Prediction cec2: " << GPSWeekSecond(cec2.getTimeOfPrediction()).printf("%F, %g") << endl;

      // Third test case.  Create an CEC object with data available from navdump.
   cout << endl << "Test Case 3: Creating CEC object with data from navdump." << endl;
   cout << "Time = " << ct3 << endl;
   CNAV2EphClk cec3;
   cec3.loadData( SysID, obsID, PRNID3, TOWWeek3, TOW3, Top3,
                    health, URAoe3, Toe3, deltaA3, Adot3, dn3,
                    dnDot3, M03, ecc3, w3, OMEGA03, i03,
                    OMEGAdot3, idot3, Cic3, Cis3, Crc3, Crs3,
                    Cuc3, Cus3, URAoc_3, URAoc1_3, URAoc2_3, af0_3,
                    af1_3, af2_3, Tgd3, ISCL1cp3, ISCL1cd3);

   Xvt  xvt3 = cec3.svXvt( dt3 ); 
   cout << "Position cec3:    "        << xvt3.x << endl;
   cout << "Velocity cec3:    "        << xvt3.v << endl;
   cout << "Clock Bias cec3:  "        << xvt3.clkbias << endl;
   cout << "Clock Drift cec3: "        << xvt3.clkdrift << endl;
   cout << "RelCorr cec3:     "        << xvt3.relcorr << endl;
   cout << "Time of Prediction cec3: " << GPSWeekSecond(cec3.getTimeOfPrediction()).printf("%F, %g") << endl;

      // Fourth test case.  Compare against "classic" EngEphemeris
   cout << endl << "Test Case 4: Calculated position using 'classic' EngEphemeris." << endl;
   cout<< "Time= "<< g << endl;
   EngEphemeris EE;
   EE.addSubframe(subframe1, TOWWeek, 3, 1);
   EE.addSubframe(subframe2, TOWWeek, 3, 1);
   EE.addSubframe(subframe3, TOWWeek, 3, 1);

   Xvt xvt = EE.svXvt(dt);
   cout<< "Position EE:    " << xvt.x << endl;
   cout<< "Velocity EE:    " << xvt.v << endl;
   cout<< "Clock Bias EE:  " << xvt.clkbias << endl;
   cout<< "Clock Drift EE: " << xvt.clkdrift << endl;
   cout<< "RelCorr EE:     " << EE.svRelativity(dt) << endl;

   cout << endl;
   cout << "CEC Object Dump:" << endl;
   cout << cec << endl;

   cout << endl;
   cout << "CEC2 Object Dump:" << endl;
   cout << cec2 << endl;

   cout << endl;
   cout << "CEC3 Object Dump:" << endl;
   cout << cec3 << endl;

   cout << endl;
   cout << "Fit Interval Tests" << endl;
   cout << "BeginFit cec:  "    << GPSWeekSecond(cec.getOrbit().getBeginningOfFitInterval()).printf("%F, %g") << endl;
   cout << "BeginFit EE:   "    << GPSWeekSecond(EE.getOrbit().getBeginningOfFitInterval()).printf("%F, %g") << endl;

   cout << endl;
   cout << "EndFit cec:   " << GPSWeekSecond(cec.getOrbit().getEndOfFitInterval()).printf("%F, %g") << endl;
   cout << "EndFit EE  :  " << GPSWeekSecond(EE.getOrbit().getEndOfFitInterval()).printf("%F, %g") << endl;

   cout << endl;
   cout << "Within Fit Interval: " << cec.getOrbit().withinFitInterval(dt) << endl;
   cout << "Within Fit Interval: " << EE.getOrbit().withinFitInterval(dt) << endl;

   return(0);
}
