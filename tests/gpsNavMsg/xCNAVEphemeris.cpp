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
*  Test program from June 2011.  Written to test the CNAVEphemeris 
*  module as it was being developed.
*
*********************************************************************/
#include <stdio.h>

#include "CNAVEphemeris.hpp"
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
   ObsID obsID( ObsID::otNavMsg, ObsID::cbL2, ObsID::tcC2LM );
   ObsID obsID2( ObsID::otNavMsg, ObsID::cbL5, ObsID::tcIQ5 );
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
   double deltaOMEGAdot1 = rOMEGAdot - OMEGADOT_REF;
   double ridot     = 5.52880172536E-10;

   double deltaA    = rA - A_REF;
   long TOWMsg10_1  = 382500;
   long TOWMsg11_1  = 382500; 
   long Top         = 378000;
   short URAoe      = 1;
   short AlertMsg10 = 0;
   short AlertMsg11 = 0;
   short L1Health   = 0;
   short L2Health   = 0;
   short L5Health   = 1;

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
   double deltaOMEGAdot2 = OMEGAdot2 - OMEGADOT_REF;
   double idot2     = 1.21433630E-10;
   double Tgd2      = -1.07102096E-08;
   double deltaA2   = A2 - A_REF;
   long TOWMsg10_2  = 0;
   long TOWMsg11_2  = 0; 
   long Top2        = 601200;
   short URAoe2     = 1;


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
   double deltaOMEGAdot3 = OMEGAdot3 - OMEGADOT_REF;
   double idot3     = 1.36791412E-10;
   double Tgd3      = -5.58793545E-09;
    double deltaA3  = A3 - A_REF;
   long TOWMsg10_3  = 597600;
   long TOWMsg11_3  = 597600; 
   long Top3        = 594000;
   short URAoe3     = 1;

  
   long subframe1[10] = { 0x22C2663D, 0x1F0E29B8, 0x2664002B, 0x09FCC1B6, 0x0F60EB8A,
                          0x1299CE93, 0x29CD3DB6, 0x0597BB0F, 0x00000B68, 0x17B28E5C };
   long subframe2[10] = { 0x22C2663D, 0x1F0E4A28, 0x05809675, 0x0EBD8AF1, 0x00089344,
                         0x008081F8, 0x1330CC2C, 0x0461E855, 0x034F8045, 0x17BB1E68 };
   long subframe3[10] = { 0x22C2663D, 0x1F0E6BA0, 0x3FE129CD, 0x26E31837, 0x0006C96A,
                          0x35A74DFC, 0x065C8B0F, 0x1E4F400A, 0x3FE8966D, 0x05860C44 };

   cout.precision(11);

      // First test case.  Create an CE object with data available from RINEX file.
   cout << endl << "Test Case 1: Creating CE object with data from RINEX file." << endl;
   cout << "Time = " << g << endl;
   CNAVEphemeris ce1;
   ce1.loadData(SysID, obsID, PRNID, AlertMsg10, TOWMsg10_1, AlertMsg11,
                TOWMsg11_1, rTOWWeek, Top, URAoe, L1Health, L2Health, 
		          L5Health, rToe, raccuracy, rCuc, rCus, rCrc, rCrs, rCic,
                rCis, rM0, rdn, rdnDot, recc, deltaA, rAdot, rOMEGA0, 
                ri0, rw, deltaOMEGAdot1, ridot );

   Xv  xv1 = ce1.svXv( dt ); 
   cout << "Position ce1: "           << xv1.x << endl;
   cout << "Velocity ce1: "           << xv1.v << endl;
   cout << "RelCorr ce1:  "           << ce1.svRelativity(dt) << endl;
   cout << "Time of Prediction ce1: " << GPSWeekSecond(ce1.getTimeOfPrediction()).printf("%F, %g") << endl;
   cout << "CNAV Accuracy Test:  "    << SV_CNAV_ACCURACY_MAX_INDEX[URAoe+15] << endl;
   cout << "legacy Accuracy Test: "   << SV_ACCURACY_MAX_INDEX[URAoe] << endl;

      // Second test case.  Create an CE object with data available from navdump.
   cout << endl << "Test Case 2: Creating CE object with data from navdump." << endl;
   cout << "Time = " << ct2 << endl;
   CNAVEphemeris ce2;
   ce2.loadData( SysID, obsID2, PRNID2, AlertMsg10, TOWMsg10_2, AlertMsg11,
                TOWMsg11_2, TOWWeek2, Top2, URAoe2, L1Health, L2Health, 
		          L5Health, Toe2, accuracy2, Cuc2, Cus2, Crc2, Crs2, Cic2,
                Cis2, M02, dn2, dnDot2, ecc2, deltaA2, Adot2, OMEGA02, 
                i02, w2, deltaOMEGAdot2, idot2 );

   Xv  xv2 = ce2.svXv( dt2 ); 
   cout << "Position ce2: "           << xv2.x << endl;
   cout << "Velocity ce2: "           << xv2.v << endl;
   cout << "RelCorr ce2:  "           << ce2.svRelativity(dt2) << endl;
   cout << "Time of Prediction ce2: " << GPSWeekSecond(ce2.getTimeOfPrediction()).printf("%F, %g") << endl;

      // Third test case.  Create an CE object with data available from navdump.
   cout << endl << "Test Case 3: Creating CE object with data from navdump." << endl;
   cout << "Time = " << ct3 << endl;
   CNAVEphemeris ce3;
   ce3.loadData( SysID, obsID, PRNID3, AlertMsg10, TOWMsg10_3, AlertMsg11,
                TOWMsg11_3, TOWWeek3, Top3, URAoe3, L1Health, L2Health, 
		          L5Health, Toe3, accuracy3, Cuc3, Cus3, Crc3, Crs3, Cic3,
                Cis3, M03, dn3, dnDot3, ecc3, deltaA3, Adot3, OMEGA03, 
                i03, w3, deltaOMEGAdot3, idot3 );

   Xv  xv3 = ce3.svXv( dt3 ); 
   cout << "Position ce3: "           << xv3.x << endl;
   cout << "Velocity ce3: "           << xv3.v << endl;
   cout << "RelCorr ce3:  "           << ce3.svRelativity(dt3) << endl;
   cout << "Time of Prediction ce3: " << GPSWeekSecond(ce3.getTimeOfPrediction()).printf("%F, %g") << endl;

      // Fourth test case.  Compare against "classic" EngEphemeris
   cout << endl << "Test Case 4: Calculated position using 'classic' EngEphemeris." << endl;
   cout<< "Time= "<< g << endl;
   EngEphemeris EE;
   EE.addSubframe(subframe1, TOWWeek, 3, 1);
   EE.addSubframe(subframe2, TOWWeek, 3, 1);
   EE.addSubframe(subframe3, TOWWeek, 3, 1);

   Xvt xvt = EE.svXvt(dt);
   cout<< "Position EE: " << xvt.x << endl;
   cout<< "Velocity EE: " << xvt.v << endl;
   cout<< "RelCorr EE:  " << EE.svRelativity(dt) << endl;

   cout << endl;
   cout << "CE Object Dump:" << endl;
   cout << ce1 << endl;
   if ( ce1.getOrbit().isHealthy()) cout << "CE.orbit is healthy." << endl;
   else cout << "CE.orbit is NOT healthy." << endl;

   cout << endl;
   cout << "CE2 Object Dump:" << endl;
   cout << ce2 << endl;
   if ( ce2.getOrbit().isHealthy()) cout << "CE2.orbit is healthy." << endl;
   else cout << "CE2.orbit is NOT healthy." << endl;

   cout << endl;
   cout << "CE3 Object Dump:" << endl;
   cout << ce3 << endl;
   if ( ce3.getOrbit().isHealthy()) cout << "CE3.orbit is healthy." << endl;
   else cout << "CE3.orbit is NOT healthy." << endl;

   cout << endl;
   cout << "Fit Interval Tests" << endl;
   cout << "BeginFit ce1:  "    << GPSWeekSecond(ce1.getOrbit().getBeginningOfFitInterval()).printf("%F, %g") << endl;
   cout << "BeginFit EE:   "    << GPSWeekSecond(EE.getOrbit().getBeginningOfFitInterval()).printf("%F, %g") << endl;

   cout << endl;
   cout << "EndFit ce1:   " << GPSWeekSecond(ce1.getOrbit().getEndOfFitInterval()).printf("%F, %g") << endl;
   cout << "EndFit EE  :  " << GPSWeekSecond(EE.getOrbit().getEndOfFitInterval()).printf("%F, %g") << endl;

   cout << endl;
   cout << "Within Fit Interval: " << ce1.getOrbit().withinFitInterval(dt) << endl;
   cout << "Within Fit Interval: " << EE.getOrbit().withinFitInterval(dt) << endl;

   return(0);
}
