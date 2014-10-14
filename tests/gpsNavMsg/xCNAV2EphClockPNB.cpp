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
*  Test program from July 2011. Written to test the CNAV2EphClk and
*  PackedNavBits modules as they were being developed. First, nav
*  message data is packed using PackedNavBits, and then unpacked
*  using CNAV2EphClk in order to calculate position, velocity, 
*  and clock correction.
*
*********************************************************************/
#include <stdio.h>
#include <math.h>

#include "PackedNavBits.hpp"
#include "CivilTime.hpp"
#include "CommonTime.hpp"
#include "GPSWeekSecond.hpp"
#include "GNSSconstants.hpp"
#include "CNAV2EphClk.hpp"

using namespace std;
using namespace gpstk;

int main( int argc, char * argv[] )
{
      // Set time to Day 153, 2011 (6/2/2011) at noon
   CivilTime g( 2011, 6, 2, 12, 14, 44.0, TimeSystem::GPS );
   CommonTime TransmitTime = g.convertToCommonTime();

   SatID satSys(1, SatID::systemGPS);
   ObsID obsID( ObsID::otNavMsg, ObsID::cbL2, ObsID::tcC2LM );

      // Test Data copied from RINEX file for PRN3, week 1638, day 153 2011
      // Subframe 2 Data

   unsigned long TOWWeek   = 1638;
   int n_TOWWeek           = 13;
   int s_TOWWeek           = 1;

   unsigned long ITOW      = 53;
   int n_ITOW              = 8;
   int s_ITOW              = 1;

   unsigned long Top       = 378000;
   int n_Top               = 11;
   int s_Top               = 300;

   unsigned long L1CHealth = 0;
   int n_L1CHealth         = 1;
   int s_L1CHealth         = 1;

   long URAoe              = -16;
   int n_URAoe             = 5;
   int s_URAoe             = 1;

   unsigned long Toe       = 388800;
   int n_Toe               = 11;
   int s_Toe               = 300;

   double Ahalf            = 5.15365527534E+03; 
   int n_Ahalf             = 32;
   int s_Ahalf             = -19;

   double deltaA           = Ahalf*Ahalf - A_REF_GPS;
   int n_deltaA            = 26;
   int s_deltaA            = -9;

   double Adot             = 0;
   int n_Adot              = 25;
   int s_Adot              = -21;

   double dn               = 5.39093883996E-09;
   int n_dn                = 17;
   int s_dn                = -44;

   double dndot            = 0;
   int n_dndot             = 23;
   int s_dndot             = -57;

   double M0               = 1.05539162795E+00;
   int n_M0                = 33;
   int s_M0                = -32;

   double ecc              = 1.42575260252E-02;
   int n_ecc               = 33;
   int s_ecc               = -34;

   double w                = 1.09154604931E+00;
   int n_w                 = 33;
   int s_w                 = -32;

   double OMEGA0           = -2.16947563164E+00;
   int n_OMEGA0            = 33;
   int s_OMEGA0            = -32;

   double i0               = 9.28692497530E-01;
   int n_i0                = 33;
   int s_i0                = -32;

   double OMEGAdot         = -8.56285667735E-09;
   int n_OMEGAdot          = 24;
   int s_OMEGAdot          = -43;

   double deltaOMEGAdot    = OMEGAdot - OMEGADOT_REF_GPS;
   int n_deltaOMEGAdot     = 17;
   int s_deltaOMEGAdot     = -44;

   double idot             = 5.52880172536E-10;
   int n_idot              = 15;
   int s_idot              = -44;

   double Cis              = 5.02914190292E-08;
   int n_Cis               = 16;
   int s_Cis               = -30;

   double Cic              = -2.30967998505E-07;
   int n_Cic               = 16;
   int s_Cic               = -30;

   double Crs              = 1.87812500000E+01;
   int n_Crs               = 24;
   int s_Crs               = -8;

   double Crc              = 2.03562500000E+02;
   int n_Crc               = 24;
   int s_Crc               = -8;

   double Cus              = 8.35768878460E-06;
   int n_Cus               = 21;
   int s_Cus               = -30;

   double Cuc              = 9.57399606705E-07;
   int n_Cuc               = 21;
   int s_Cuc               = -30;

   long URAoc              = 15;
   int n_URAoc             = 5;
   int s_URAoc             = 1;

   unsigned long URAoc1    = 1;
   int n_URAoc1            = 3;
   int s_URAoc1            = 1;

   unsigned long URAoc2    = 2;
   int n_URAoc2            = 3;
   int s_URAoc2            = 1;
   
   double af0              = 7.23189674318E-04;
   int n_af0               = 26;
   int s_af0               = -35;

   double af1              = 5.11590769747E-12;
   int n_af1               = 20;
   int s_af1               = -48;

   double af2              = 0.0;
   int n_af2               = 10;
   int s_af2               = -60;

   double Tgd              = -4.65661287308E-09;
   int n_Tgd               = 13;
   int s_Tgd               = -35;

   double ISCL1cp          = 1E-8;
   int n_ISCL1cp           = 13;
   int s_ISCL1cp           = -35;

   double ISCL1cd          = -1E-8; 
   int n_ISCL1cd           = 13;
   int s_ISCL1cd           = -35;

   unsigned long sflag     = 0;
   int n_sflag             = 1;
   int s_sflag             = 1;

   unsigned long reservedBits = 0;
   int n_reservedBits         = 10;
   int s_reservedBits         = 1;

   unsigned long CRC       = 0;
   int n_CRC               = 24;
   int s_CRC               = 1; 

      // First Test Case. Create PNB object in which to store subframe 2 data
      // for the CNAV2 nav message.
   PackedNavBits pnb;

   /* Pack Subframe 2 data */
   pnb.setSatID(satSys);
   pnb.setObsID(obsID);
   pnb.setTime(TransmitTime);
   pnb.addUnsignedLong(TOWWeek, n_TOWWeek, s_TOWWeek);
   pnb.addUnsignedLong(ITOW, n_ITOW, s_ITOW);
   pnb.addUnsignedLong(Top, n_Top, s_Top);
   pnb.addUnsignedLong(L1CHealth, n_L1CHealth, s_L1CHealth);
   pnb.addLong(URAoe, n_URAoe, s_URAoe);
   pnb.addUnsignedLong(Toe, n_Toe, s_Toe);
   pnb.addSignedDouble(deltaA, n_deltaA, s_deltaA);
   pnb.addSignedDouble(Adot, n_Adot, s_Adot);
   pnb.addDoubleSemiCircles(dn, n_dn, s_dn);
   pnb.addDoubleSemiCircles(dndot, n_dndot, s_dndot);
   pnb.addDoubleSemiCircles(M0, n_M0, s_M0);
   pnb.addUnsignedDouble(ecc, n_ecc, s_ecc);
   pnb.addDoubleSemiCircles(w, n_w, s_w);
   pnb.addDoubleSemiCircles(OMEGA0, n_OMEGA0, s_OMEGA0);
   pnb.addDoubleSemiCircles(i0, n_i0, s_i0);
   pnb.addDoubleSemiCircles(deltaOMEGAdot, n_deltaOMEGAdot, s_deltaOMEGAdot);
   pnb.addDoubleSemiCircles(idot, n_idot, s_idot); 
   pnb.addSignedDouble(Cis, n_Cis, s_Cis);
   pnb.addSignedDouble(Cic, n_Cic, s_Cic);
   pnb.addSignedDouble(Crs, n_Crs, s_Crs);
   pnb.addSignedDouble(Crc, n_Crc, s_Crc);
   pnb.addSignedDouble(Cus, n_Cus, s_Cus);
   pnb.addSignedDouble(Cuc, n_Cuc, s_Cuc);
   pnb.addLong(URAoc, n_URAoc, s_URAoc);
   pnb.addUnsignedLong(URAoc1, n_URAoc1, s_URAoc1);
   pnb.addUnsignedLong(URAoc2, n_URAoc2, s_URAoc2);
   pnb.addSignedDouble(af0, n_af0, s_af0);
   pnb.addSignedDouble(af1, n_af1, s_af1);
   pnb.addSignedDouble(af2, n_af2, s_af2);
   pnb.addSignedDouble(Tgd, n_Tgd, s_Tgd);
   pnb.addSignedDouble(ISCL1cp, n_ISCL1cp, s_ISCL1cp);
   pnb.addSignedDouble(ISCL1cd, n_ISCL1cd, s_ISCL1cd);
   pnb.addUnsignedLong(sflag, n_sflag, s_sflag);
   pnb.addUnsignedLong(reservedBits, n_reservedBits, s_reservedBits);
   pnb.addUnsignedLong(CRC, n_CRC, s_CRC);

   cout << endl;
   cout << "Time of Transmission: "     << pnb.getTransmitTime() << endl;
   cout << "Time of Transmission pnb: " << GPSWeekSecond(pnb.getTransmitTime()).printf("%F, %g") << endl;

      // Resize the vector holding the packed nav message data
   pnb.trimsize();
   
   cout << "PNB Object Dump:" << endl;   
   cout << pnb << endl;

      // Second test case.  Create a CEC object with the packed data above, available from RINEX file.
   cout << endl << "Test Case 2: Creating CEC object with data from RINEX file." << endl;
   cout << "Time = " << g << endl;
   CommonTime dt = TransmitTime;
   short PRNIDArg = 3;
   int TOI = 50;   // 9 bit word from subframe 1
   CNAV2EphClk cec;   
   cec.loadData( obsID, PRNIDArg, TOI, pnb);
   Xvt  xvt = cec.svXvt( dt ); 
   double ClkCorr = cec.svClockBias( dt );
   double ClkDrift = cec.svClockDrift( dt ); 
   cout << "Position cec:    " << xvt.x<< endl;
   cout << "Velocity cec:    " << xvt.v << endl;
   cout.setf(ios::scientific, ios::floatfield);
   cout.precision(11);
   cout << "RelCorr cec:     "        << cec.svRelativity(dt) << endl;
   cout << "Clock Bias cec:  "        << ClkCorr << endl;
   cout << "Clock Drift cec: "        << ClkDrift << endl; 
   cout << "Time of Prediction cec:"  << GPSWeekSecond(cec.getTimeOfPrediction()).printf("%F, %g") << endl;

   cout << endl << "CEC Object Dump:" << endl;
   cout << cec << endl;

   return(0);
}
