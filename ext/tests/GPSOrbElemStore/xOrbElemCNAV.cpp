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
*  $Id$
*
*  Test program from July 2011. Written to test the CNAVEphemeris
*  and PackedNavBits modules as they were being developed. First, nav
*  message data is packed using PackedNavBits, and then unpacked
*  using CNAVEphemeris in order to calculate position and velocity.
*
*********************************************************************/
#include <stdio.h>
#include <math.h>
#include <typeinfo>

#include "PackedNavBits.hpp"
#include "CivilTime.hpp"
#include "CommonTime.hpp"
#include "GPSWeekSecond.hpp"
#include "GNSSconstants.hpp"
#include "CNAVEphemeris.hpp"
#include "OrbElemCNAV.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;

int main( int argc, char * argv[] )
{
      // Set time to Day 153, 2011 (6/2/2011) at noon
   CivilTime g( 2011, 6, 2, 12, 14, 44.0, TimeSystem::GPS );
   CommonTime TransmitTime = g.convertToCommonTime();

   SatID satSys(1, SatID::systemGPS);
   ObsID obsID( ObsID::otNavMsg, ObsID::cbL2, ObsID::tcC2LM );
   SatID satID( 1, SatID::systemGPS );
   int bits = 2;


      // Test Data copied from RINEX file for PRN3, week 1638, day 153 2011
      // Message Type 10
   unsigned long PREAMBLEMsg10 = 139;
   int n_PREAMBLEMsg10         = 8;
   int s_PREAMBLEMsg10         = 1;

   unsigned long PRNIDMsg10    = 3;
   int n_PRNIDMsg10            = 6;
   int s_PRNIDMsg10            = 1;

   unsigned long Msg10ID       = 10;
   int n_Msg10ID               = 6;
   int s_Msg10ID               = 1;

   unsigned long TOWMsg10      = 382512;
   int n_TOWMsg10              = 17;
   int s_TOWMsg10              = 300;

   unsigned long AlertMsg10    = 0;
   int n_AlertMsg10            = 1;
   int s_AlertMsg10            = 1;

   unsigned long TOWWeek       = 1638;
   int n_TOWWeek               = 13;
   int s_TOWWeek               = 1;

   unsigned long L1Health      = 0;
   int n_L1Health              = 1;
   int s_L1Health              = 1;

   unsigned long L2Health      = 0;
   int n_L2Health              = 1;
   int s_L2Health              = 1;

   unsigned long L5Health      = 0;
   int n_L5Health              = 1;
   int s_L5Health              = 1;

   unsigned long Top           = 378000;
   int n_Top                   = 11;
   int s_Top                   = 300;

   long URAoe                  = 1;
   int n_URAoe                 = 5;
   int s_URAoe                 = 1;

   unsigned long rToeMsg10     = 388800;
   int n_rToeMsg10             = 11;
   int s_rToeMsg10             = 300;

   double rAhalf               = 5.15365527534E+03; 
   int n_rAhalf                = 32;
   int s_rAhalf                = -19;

   double deltaA               = rAhalf*rAhalf - A_REF_GPS;
   int n_deltaA                = 26;
   int s_deltaA                = -9;

   double Adot                 = 0;
   int n_Adot                  = 25;
   int s_Adot                  = -21;

   double rdn                  = 5.39093883996E-09;
   int n_rdn                   = 17;
   int s_rdn                   = -44;

   double dndot                = 0;
   int n_dndot                 = 23;
   int s_dndot                 = -57;

   double rM0                  = 1.05539162795E+00;
   int n_rM0                   = 33;
   int s_rM0                   = -32;

   double recc                 = 1.42575260252E-02;
   int n_recc                  = 33;
   int s_recc                  = -34;

   double rw                   = 1.09154604931E+00;
   int n_rw                    = 33;
   int s_rw                    = -32;

   unsigned long sflag         = 0;
   int n_sflag                 = 1;
   int s_sflag                 = 1;

   unsigned long L2CPhasing    = 0;
   int n_L2CPhasing            = 1;
   int s_L2CPhasing            = 1;

   unsigned long reservedBitsMsg10 = 0;
   int n_reservedBitsMsg10         = 3;
   int s_reservedBitsMsg10         = 1;

   unsigned long CRCMsg10      = 0;
   int n_CRCMsg10              = 24;
   int s_CRCMsg10              = 1;

   // Message Type 11
   unsigned long PREAMBLEMsg11 = 139;
   int n_PREAMBLEMsg11         = 8;
   int s_PREAMBLEMsg11         = 1;

   unsigned long PRNIDMsg11    = 3;
   int n_PRNIDMsg11            = 6;
   int s_PRNIDMsg11            = 1;

   unsigned long Msg11ID       = 11;
   int n_Msg11ID               = 6;
   int s_Msg11ID               = 1; 

   unsigned long TOWMsg11      = 382524;
   int n_TOWMsg11              = 17;
   int s_TOWMsg11              = 300;

   unsigned long AlertMsg11    = 0;
   int n_AlertMsg11            = 1;
   int s_AlertMsg11            = 1;

   unsigned long rToeMsg11     = 388800;
   int n_rToeMsg11             = 11;
   int s_rToeMsg11             = 300;

   double rOMEGA0              = -2.16947563164E+00;
   int n_rOMEGA0               = 33;
   int s_rOMEGA0               = -32;

   double ri0                  = 9.28692497530E-01;
   int n_ri0                   = 33;
   int s_ri0                   = -32;

   double rOMEGAdot            = -8.56285667735E-09;
   int n_rOMEGAdot             = 24;
   int s_rOMEGAdot             = -43;

   double deltaOMEGAdot        = rOMEGAdot - OMEGADOT_REF_GPS;
   int n_deltaOMEGAdot         = 17;
   int s_deltaOMEGAdot         = -44;

   double ridot     = 5.52880172536E-10;
   int n_ridot      = 15;
   int s_ridot      = -44;

   double rCis      = 5.02914190292E-08;
   int n_rCis       = 16;
   int s_rCis       = -30;

   double rCic      = -2.30967998505E-07;
   int n_rCic       = 16;
   int s_rCic       = -30;

   double rCrs      = 1.87812500000E+01;
   int n_rCrs       = 24;
   int s_rCrs       = -8;

   double rCrc      = 2.03562500000E+02;
   int n_rCrc       = 24;
   int s_rCrc       = -8;

   double rCus      = 8.35768878460E-06;
   int n_rCus       = 21;
   int s_rCus       = -30;

   double rCuc      = 9.57399606705E-07;
   int n_rCuc       = 21;
   int s_rCuc       = -30;

   unsigned long reservedBitsMsg11 = 0;
   int n_reservedBitsMsg11         = 7;
   int s_reservedBitsMsg11         = 1;

   unsigned long CRCMsg11 = 0;
   int n_CRCMsg11         = 24;
   int s_CRCMsg11         = 1;

   
      // Test Data copied from RINEX file for PRN3, week 1638, day 153 2011
      // Message Type Clk
   unsigned long PREAMBLE = 139;
   int n_PREAMBLE         = 8;
   int s_PREAMBLE         = 1;

   unsigned long PRNID    = 3;
   int n_PRNID            = 6;
   int s_PRNID            = 1;

   unsigned long MsgID    = 10;
   int n_MsgID            = 6;
   int s_MsgID            = 1;

   unsigned long TOWMsg   = 382536;
   int n_TOWMsg           = 17;
   int s_TOWMsg           = 300;

   unsigned long Alert    = 0;
   int n_Alert            = 1;
   int s_Alert            = 1;

   long URAoc             = 15;
   int n_URAoc            = 5;
   int s_URAoc            = 1;

   unsigned long URAoc1   = 1;
   int n_URAoc1           = 3;
   int s_URAoc1           = 1;

   unsigned long URAoc2   = 2;
   int n_URAoc2           = 3;
   int s_URAoc2           = 1;

   unsigned long Toc      = 388800;
   int n_Toc              = 11;
   int s_Toc              = 300;
   
   double af0             = 7.23189674318E-04;
   int n_af0              = 26;
   int s_af0              = -35;

   double af1             = 5.11590769747E-12;
   int n_af1              = 20;
   int s_af1              = -48;

   double af2             = 0.0;
   int n_af2              = 10;
   int s_af2              = -60;

      // First Test Case. Create PNB objects in which to store Message Type 10 and 11 data.
   PackedNavBits pnb10;
   PackedNavBits pnb11;
   
   /* Pack Message Type 10 data */
   pnb10.setSatID(satSys);
   pnb10.setObsID(obsID);
   pnb10.setTime(TransmitTime);
   pnb10.addUnsignedLong(PREAMBLEMsg10, n_PREAMBLEMsg10, s_PREAMBLEMsg10);
   pnb10.addUnsignedLong(PRNIDMsg10, n_PRNIDMsg10, s_PRNIDMsg10);
   pnb10.addUnsignedLong(Msg10ID, n_Msg10ID, s_Msg10ID);
   pnb10.addUnsignedLong(TOWMsg10, n_TOWMsg10, s_TOWMsg10);
   pnb10.addUnsignedLong(AlertMsg10, n_AlertMsg10, s_AlertMsg10);
   pnb10.addUnsignedLong(TOWWeek, n_TOWWeek, s_TOWWeek);
   pnb10.addUnsignedLong(L1Health, n_L1Health, s_L1Health);
   pnb10.addUnsignedLong(L2Health, n_L2Health, s_L2Health);
   pnb10.addUnsignedLong(L5Health, n_L5Health, s_L5Health);
   pnb10.addUnsignedLong(Top, n_Top, s_Top);
   pnb10.addLong(URAoe, n_URAoe, s_URAoe);
   pnb10.addUnsignedLong(rToeMsg10, n_rToeMsg10, s_rToeMsg10);
   pnb10.addSignedDouble(deltaA, n_deltaA, s_deltaA);
   pnb10.addSignedDouble(Adot, n_Adot, s_Adot);
   pnb10.addDoubleSemiCircles(rdn, n_rdn, s_rdn);
   pnb10.addDoubleSemiCircles(dndot, n_dndot, s_dndot);
   pnb10.addDoubleSemiCircles(rM0, n_rM0, s_rM0);
   pnb10.addUnsignedDouble(recc, n_recc, s_recc);
   pnb10.addDoubleSemiCircles(rw, n_rw, s_rw);
   pnb10.addUnsignedLong(sflag, n_sflag, s_sflag);
   pnb10.addUnsignedLong(L2CPhasing, n_L2CPhasing, s_L2CPhasing);
   pnb10.addUnsignedLong(reservedBitsMsg10, n_reservedBitsMsg10, s_reservedBitsMsg10);
   pnb10.addUnsignedLong(CRCMsg10, n_CRCMsg10, s_CRCMsg10);

      /* Pack Message Type 11 data */
   pnb11.setSatID(satSys);
   pnb11.setObsID(obsID);
   pnb11.setTime(TransmitTime);
   pnb11.addUnsignedLong(PREAMBLEMsg11, n_PREAMBLEMsg11, s_PREAMBLEMsg11);
   pnb11.addUnsignedLong(PRNIDMsg11, n_PRNIDMsg11, s_PRNIDMsg11);
   pnb11.addUnsignedLong(Msg11ID, n_Msg11ID, s_Msg11ID);
   pnb11.addUnsignedLong(TOWMsg11, n_TOWMsg11, s_TOWMsg11);
   pnb11.addUnsignedLong(AlertMsg11, n_AlertMsg11, s_AlertMsg11);
   pnb11.addUnsignedLong(rToeMsg11, n_rToeMsg11, s_rToeMsg11);
   pnb11.addDoubleSemiCircles(rOMEGA0, n_rOMEGA0, s_rOMEGA0);
   pnb11.addDoubleSemiCircles(ri0, n_ri0, s_ri0);
   pnb11.addDoubleSemiCircles(deltaOMEGAdot, n_deltaOMEGAdot, s_deltaOMEGAdot);
   pnb11.addDoubleSemiCircles(ridot, n_ridot, s_ridot); 
   pnb11.addSignedDouble(rCis, n_rCis, s_rCis);
   pnb11.addSignedDouble(rCic, n_rCic, s_rCic);
   pnb11.addSignedDouble(rCrs, n_rCrs, s_rCrs);
   pnb11.addSignedDouble(rCrc, n_rCrc, s_rCrc);
   pnb11.addSignedDouble(rCus, n_rCus, s_rCus);
   pnb11.addSignedDouble(rCuc, n_rCuc, s_rCuc);
   pnb11.addUnsignedLong(reservedBitsMsg11, n_reservedBitsMsg11, s_reservedBitsMsg11);
   pnb11.addUnsignedLong(CRCMsg11, n_CRCMsg11, s_CRCMsg11);

      // First Test Case. Create PNB object in which to store the first 128 bits of Message Types 30-37.
   PackedNavBits pnb3_;

      /* Pack Message Type 30-37 data */
   pnb3_.setSatID(satSys);
   pnb3_.setObsID(obsID);
   pnb3_.setTime(TransmitTime);
   pnb3_.addUnsignedLong(PREAMBLE, n_PREAMBLE, s_PREAMBLE);
   pnb3_.addUnsignedLong(PRNID, n_PRNID, s_PRNID);
   pnb3_.addUnsignedLong(MsgID, n_MsgID, s_MsgID);
   pnb3_.addUnsignedLong(TOWMsg, n_TOWMsg, s_TOWMsg);
   pnb3_.addUnsignedLong(Alert, n_Alert, s_Alert);
   pnb3_.addUnsignedLong(Top, n_Top, s_Top);
   pnb3_.addLong(URAoc, n_URAoc, s_URAoc);
   pnb3_.addUnsignedLong(URAoc1, n_URAoc1, s_URAoc1);
   pnb3_.addUnsignedLong(URAoc2, n_URAoc2, s_URAoc2);
   pnb3_.addUnsignedLong(Toc, n_Toc, s_Toc);
   pnb3_.addSignedDouble(af0, n_af0, s_af0);
   pnb3_.addSignedDouble(af1, n_af1, s_af1);
   pnb3_.addSignedDouble(af2, n_af2, s_af2);

      /* Test Packing Strings for Message Types 36 and 15 Text Messages. */
   string String = "TEST";
   int nc = 4;
   pnb3_.addString(String, nc);
      /* Unpack and print the String */
   cout << "String: " << pnb3_.asString(127,nc) << endl;

   cout << endl;
   cout << "Time of Transmission: " << pnb10.getTransmitTime() << endl;
   cout << "Time of Transmission pnb: " << GPSWeekSecond(pnb10.getTransmitTime()).printf("%F, %g") << endl;

      /* Resize the vectors holding the packed nav message data. */
   pnb10.trimsize();
   pnb11.trimsize();

      /* Resize the vector holding the packed nav message data. */
   pnb3_.trimsize();

   cout << "PNB10 Object Dump:" << endl;   
   cout << pnb10 << endl;
   cout << "PNB11 Object Dump:" << endl;
   cout << pnb11 << endl; 

   cout << endl << "PNB3_ Object Dump:" << endl;   
   cout << pnb3_ << endl;  

      // Second test case.  Create a CE object with the packed data above, available from RINEX file.
   cout << endl << "Test Case 2: Creating CE object with data from RINEX file." << endl;
   cout << "Time = " << g << endl;
   
      // Second test case.  Create a CE object with the packed data above, available from RINEX file.
   cout << endl << "Test Case 2: Creating CC object with data from RINEX file." << endl;
   cout << "Time = " << g << endl;
   CommonTime dt = TransmitTime;
   OrbElemCNAV cec;   
   cec.loadData( obsID, satID, pnb10, pnb11, pnb3_ );
   Xvt  xvt = cec.svXvt( dt ); 
   double ClkCorr = cec.svClockBias( dt );
   double ClkDrift = cec.svClockDrift( dt ); 
   cout.precision(11);
   cout << "Position cec:    " << xvt.x<< endl;
   cout << "Velocity cec:    " << xvt.v << endl;
   cout.setf(ios::scientific, ios::floatfield);
   cout << "RelCorr cec:     "        << cec.svRelativity(dt) << endl;
   cout << "Clock Bias cec:  "        << ClkCorr << endl;
   cout << "Clock Drift cec: "        << ClkDrift << endl; 
   cout << "Time of Prediction cec:"  << printTime(cec.ctTop,"%F, %g") << endl;

   cout << endl << "CEC Object Dump:" << endl;
   cout << cec << endl;

   return(0);
}
