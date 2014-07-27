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
*  Test program from July 2011. Written to test the CNAVClockPNB and
*  PackedNavBits modules as they were being developed. First, nav
*  message data is packed using PackedNavBits, and then unpacked
*  using CNAVClock in order to calculate the clock correction.
*
*********************************************************************/
#include <stdio.h>
#include <math.h>

#include "PackedNavBits.hpp"
#include "CivilTime.hpp"
#include "CommonTime.hpp"
#include "GPSWeekSecond.hpp"
#include "GNSSconstants.hpp"
#include "CNAVClock.hpp"

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
      // Message Type 3_
   unsigned long PREAMBLE = 139;
   int n_PREAMBLE         = 8;
   int s_PREAMBLE         = 1;

   unsigned long PRNID    = 3;
   int n_PRNID            = 6;
   int s_PRNID            = 1;

   unsigned long MsgID    = 10;
   int n_MsgID            = 6;
   int s_MsgID            = 1;

   unsigned long TOWMsg   = 382500;
   int n_TOWMsg           = 17;
   int s_TOWMsg           = 300;

   unsigned long Alert    = 0;
   int n_Alert            = 1;
   int s_Alert            = 1;

   unsigned long Top      = 378000;
   int n_Top              = 11;
   int s_Top              = 300;

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
   cout << "Time of Transmission:     " << pnb3_.getTransmitTime() << endl;
   cout << "Time of Transmission pnb: " << GPSWeekSecond(pnb3_.getTransmitTime()).printf("%F, %g") << endl;

      /* Resize the vector holding teh packed nav message data. */
   pnb3_.trimsize();
   
   cout << endl << "PNB3_ Object Dump:" << endl;   
   cout << pnb3_ << endl;

      // Second test case.  Create a CE object with the packed data above, available from RINEX file.
   cout << endl << "Test Case 2: Creating CC object with data from RINEX file." << endl;
   cout << "Time = " << g << endl;
   CommonTime dt  = TransmitTime;
   short PRNIDArg = 3; 
   short TOWWeek  = 1638;
   CNAVClock cc;   
   cc.loadData( obsID, PRNIDArg, TOWWeek, pnb3_);
   double ClkCorr = cc.svClockBias( dt );
   double ClkDrift = cc.svClockDrift( dt ); 
   cout.setf(ios::scientific, ios::floatfield);
   cout.precision(11);
   cout << "Clock Bias cc:  "         << ClkCorr << endl;
   cout << "Clock Drift cc: "        << ClkDrift << endl; 
   cout << "Time of Prediction cc: " << GPSWeekSecond(cc.getTimeOfPrediction()).printf("%F, %g") << endl;

   cout << endl << "CC Object Dump:" << endl;
   cout << cc << endl;

   return(0);
}
