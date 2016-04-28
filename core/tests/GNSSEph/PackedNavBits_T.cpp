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

/**
 * @file PackedNavBits_T.cpp
 * Tests for gpstk/ext/lib/GNSSEph/PackedNavBits
 */
#include "CivilTime.hpp"
#include "CommonTime.hpp"
#include "GNSSconstants.hpp"
#include "ObsID.hpp"
#include "PackedNavBits.hpp"
#include "SatID.hpp"
#include "TestUtil.hpp"
#include "TimeString.hpp"
#include "TimeSystem.hpp"

using namespace std;
using namespace gpstk;

class PackedNavBits_T
{
public:
   PackedNavBits_T();

   void init();

   unsigned abstractTest();
   unsigned realDataTest();
   unsigned equalityTest();

   double eps; 
};

PackedNavBits_T ::
PackedNavBits_T()
{
   init();
}

void PackedNavBits_T ::
init()
{
   TUDEF("PackedNavBits", "initialize");
   eps = 1E-12;   // Set the precision value
}

   // These tests are NOT real data, but are designed to test
   // edge cases and limits.
unsigned PackedNavBits_T ::
abstractTest()
{
    // Test Unsigned Integers
   TUDEF("PackedNavBits", "addUnsigned");
   unsigned long u_i1 = 32767;
   int u_n1           = 16;
   int u_s1           = 1;

   unsigned long u_i2 = 1;
   int u_n2           = 8;
   int u_s2           = 1;

   unsigned long u_i3 = 255;
   int u_n3           = 8;
   int u_s3           = 1;

   unsigned long u_i4 = 604500;
   int u_n4           = 11;
   int u_s4           = 300;

      // Pack the data
   PackedNavBits pnb;
   pnb.addUnsignedLong(u_i1, u_n1, u_s1);
   pnb.addUnsignedLong(u_i2, u_n2, u_s2);
   pnb.addUnsignedLong(u_i3, u_n3, u_s3);
   pnb.addUnsignedLong(u_i4, u_n4, u_s4);

      // Unpack the data and see that the round-trip worked.
   int startbit = 0;
   unsigned long ultest = pnb.asUnsignedLong(startbit, u_n1, u_s1);
   TUASSERTE(unsigned long, ultest, u_i1); 
   startbit += u_n1;
   ultest = pnb.asUnsignedLong(startbit, u_n2, u_s2);
   TUASSERTE(unsigned long, ultest, u_i2);
   startbit += u_n2;
   ultest = pnb.asUnsignedLong(startbit, u_n3, u_s3);
   TUASSERTE(unsigned long, ultest, u_i3);
   startbit += u_n3;
   ultest = pnb.asUnsignedLong(startbit, u_n4, u_s4);
   TUASSERTE(unsigned long, ultest, u_i4);
   startbit += u_n4;

    // Test Signed Integers
   TUCSM("addSigned");
   long s_i1 = 15;
   int s_n1  = 5;
   int s_s1  = 1;

   long s_i2 = -16;
   int s_n2  = 5;
   int s_s2  = 1;

   long s_i3 = -1;
   int s_n3  = 5;
   int s_s3  = 1;

   long s_i4 = 0;
   int s_n4  = 6;
   int s_s4  = 1;
   
   long s_i5 = 4194304;
   int s_n5  = 24;
   int s_s5  = 1;

   pnb.addLong(s_i1, s_n1, s_s1);
   pnb.addLong(s_i2, s_n2, s_s2);
   pnb.addLong(s_i3, s_n3, s_s3);
   pnb.addLong(s_i4, s_n4, s_s4);
   pnb.addLong(s_i5, s_n5, s_s5);

   long ltest;
   ltest = pnb.asLong(startbit, s_n1, s_s1);
   TUASSERTE(long, ltest, s_i1);
   startbit += s_n1;
   ltest = pnb.asLong(startbit, s_n2, s_s2);
   TUASSERTE(long, ltest, s_i2);
   startbit += s_n2;
   ltest = pnb.asLong(startbit, s_n3, s_s3);
   TUASSERTE(long, ltest, s_i3);
   startbit += s_n3;
   ltest = pnb.asLong(startbit, s_n4, s_s4);
   TUASSERTE(long, ltest, s_i4);
   startbit += s_n4;
   ltest = pnb.asLong(startbit, s_n5, s_s5);
   TUASSERTE(long, ltest, s_i5);
   startbit += s_n5;

   // Test Signed Doubles
   TUCSM("addDouble");
   double d_i1 = 0.490005493;
   int d_n1    = 16;
   int d_s1    = -16;
   double d_e1 = pow(2.0,d_s1);   // value of lsb

   double d_i2 = -0.5;
   int d_n2    = 16;
   int d_s2    = -16;
   double d_e2 = pow(2.0,d_s2);   // value of lsb

   double d_i3 = 0;
   int d_n3    = 16;
   int d_s3    = -16;
   double d_e3 = pow(2.0,d_s3);   // value of lsb

   // Test Unsigned Doubles
   double d_i4 = 32000.0;
   int d_n4    = 16;
   int d_s4    = 0;
   double d_e4 = pow(2.0,d_s4);   // value of lsb

   pnb.addSignedDouble(d_i1, d_n1, d_s1);
   pnb.addSignedDouble(d_i2, d_n2, d_s2);
   pnb.addSignedDouble(d_i3, d_n3, d_s3);
   pnb.addUnsignedDouble(d_i4, d_n4, d_s4);

   double dtest;
   dtest = pnb.asSignedDouble(startbit, d_n1, d_s1);
   TUASSERTFEPS(dtest, d_i1, d_e1);
   startbit += d_n1;
   dtest = pnb.asSignedDouble(startbit, d_n2, d_s2);
   TUASSERTFEPS(dtest, d_i2, d_e2);
   startbit += d_n2;
   dtest = pnb.asSignedDouble(startbit, d_n3, d_s3);
   TUASSERTFEPS(dtest, d_i3, d_e3);
   startbit += d_n3;
   dtest = pnb.asUnsignedDouble(startbit, d_n4, d_s4);
   TUASSERTFEPS(dtest, d_i4, d_e4);
   startbit += d_n4;

   // Test Semi-Circles
   TUCSM("addSemiCircles");
   double sd_i1 = PI-2*pow(2.0,-31);
   int sd_n1    = 32;
   int sd_s1    = -31;
   double sd_e1 = pow(2.0,sd_s1) * 3.0;   // value of lsb in semi-circles

   double sd_i2 = -PI;
   int sd_n2    = 32;
   int sd_s2    = -31;
   double sd_e2 = pow(2.0,sd_s1) * 3.0;   // value of lsb

   pnb.addDoubleSemiCircles(sd_i1, sd_n1, sd_s1);
   pnb.addDoubleSemiCircles(sd_i2, sd_n2, sd_s2); 

   dtest = pnb.asDoubleSemiCircles(startbit, sd_n1, sd_s1);
   TUASSERTFEPS(dtest, sd_i1, sd_e1);
   startbit += sd_n1;
   dtest = pnb.asDoubleSemiCircles(startbit, sd_n2, sd_s2);
   TUASSERTFEPS(dtest, sd_i2, sd_e2);

   return testFramework.countFails();
}

   // These test cases are examples from real data.
unsigned PackedNavBits_T ::
realDataTest()
{
   TUDEF("PackedNavBits", "real data round-trip");

      //Test Data copied from RINEX file for PRN3, week 1638, day 153 2011
   SatID satID(3, SatID::systemGPS);
   ObsID obsID( ObsID::otNavMsg, ObsID::cbL1, ObsID::tcCA );
   std::string rxID = "rx1";
   CommonTime ct = CivilTime( 2011, 6, 2, 10, 00, 0.0, TimeSystem::GPS );
   PackedNavBits pnb(satID,obsID,rxID,ct);

   double rToe         = 388800.0;
   int n_rToe          = 16;
   int s_rToe          = 4;

   unsigned long riodc = 22;
   int n_riodc         = 8;
   int s_riodc         = 1;

   unsigned long riode = 22;
   int n_riode         = 8;
   int s_riode         = 1;

   unsigned long raodo = 10;
   int n_raodo         = 5;
   int s_raodo         = 1;

   unsigned long rfitInt = 0;
   int n_rfitInt    = 1;
   int s_rfitInt    = 1;

   double rToc      = 388800.0;
   int n_rToc       = 16;
   int s_rToc       = 4;

   double rCuc      = 9.57399606705E-07;
   int n_rCuc       = 16;
   int s_rCuc       = -29;

   double rCus      = 8.35768878460E-06;
   int n_rCus       = 16;
   int s_rCus       = -29;

   double rCrc      = 2.03562500000E+02;
   int n_rCrc       = 16;
   int s_rCrc       = -5;

   double rCrs      = 1.87812500000E+01;
   int n_rCrs       = 16;
   int s_rCrs       = -5;

   double rCic      = -2.30967998505E-07;
   int n_rCic       = 16;
   int s_rCic       = -29;

   double rCis      = 5.02914190292E-08;
   int n_rCis       = 16;
   int s_rCis       = -29;

   double rM0       = 1.05539162795E+00;
   int n_rM0        = 32;
   int s_rM0        = -31;

   double rdn       = 5.39093883996E-09;
   int n_rdn        = 16;
   int s_rdn        = -43;

   double recc      = 1.42575260252E-02;
   int n_recc       = 32;
   int s_recc       = -33;

   double rAhalf    = 5.15365527534E+03; 
   int n_rAhalf     = 32;
   int s_rAhalf     = -19;

   double rOMEGA0   = -2.16947563164E+00;
   int n_rOMEGA0    = 32;
   int s_rOMEGA0    = -31;

   double ri0       = 9.28692497530E-01;
   int n_ri0        = 32;
   int s_ri0        = -31;

   double rw        = 1.09154604931E+00;
   int n_rw         = 32;
   int s_rw         = -31;

   double rOMEGAdot = -8.56285667735E-09;
   int n_rOMEGAdot  = 24;
   int s_rOMEGAdot  = -43;

   double ridot     = 5.52880172536E-10;
   int n_ridot      = 14;
   int s_ridot       = -43;

   double raf0      = 7.23189674318E-04;
   int n_raf0       = 22;
   int s_raf0       = -31;

   double raf1      = 5.11590769747E-12;
   int n_raf1       = 16;
   int s_raf1       = -43;

   double raf2      = 0.0;
   int n_raf2       = 8;
   int s_raf2       = -55;

   double rTgd      = -4.65661287308E-09;
   int n_rTgd       = 8;
   int s_rTgd       = -31;

      // Pack legacy nav message data in order
      // (But w/o parity)
   pnb.addSignedDouble(rTgd, n_rTgd, s_rTgd);
   pnb.addUnsignedLong(riodc, n_riodc, s_riodc);
   pnb.addUnsignedDouble(rToc, n_rToc, s_rToc);
   pnb.addSignedDouble(raf2, n_raf2, s_raf2);
   pnb.addSignedDouble(raf1, n_raf1, s_raf1);
   pnb.addSignedDouble(raf0, n_raf0, s_raf0);
   pnb.addUnsignedLong(riode, n_riode, s_riode);
   pnb.addSignedDouble(rCrs, n_rCrs, s_rCrs);
   pnb.addDoubleSemiCircles(rdn, n_rdn, s_rdn);
   pnb.addDoubleSemiCircles(rM0, n_rM0, s_rM0);
   pnb.addSignedDouble(rCuc, n_rCuc, s_rCuc);
   pnb.addUnsignedDouble(recc, n_recc, s_recc);
   pnb.addSignedDouble(rCus, n_rCus, s_rCus);
   pnb.addUnsignedDouble(rAhalf, n_rAhalf, s_rAhalf);
   pnb.addUnsignedDouble(rToe, n_rToe, s_rToe);
   pnb.addUnsignedLong(rfitInt, n_rfitInt, s_rfitInt);
   pnb.addUnsignedLong(raodo, n_raodo, s_raodo);
   pnb.addSignedDouble(rCic, n_rCic, s_rCic);
   pnb.addDoubleSemiCircles(rOMEGA0, n_rOMEGA0, s_rOMEGA0);
   pnb.addSignedDouble(rCis, n_rCis, s_rCis);
   pnb.addDoubleSemiCircles(ri0, n_ri0, s_ri0);
   pnb.addSignedDouble(rCrc, n_rCrc, s_rCrc);
   pnb.addDoubleSemiCircles(rw, n_rw, s_rw);
   pnb.addDoubleSemiCircles(rOMEGAdot, n_rOMEGAdot, s_rOMEGAdot);
   pnb.addUnsignedLong(riode, n_riode, s_riode);
   pnb.addDoubleSemiCircles(ridot, n_ridot, s_ridot); 
   pnb.trimsize();

   int startbit = 0;
   double dtest;
   unsigned long ultest;
   long ltest;
   double eps;     // value of lsb

   dtest = pnb.asSignedDouble(startbit, n_rTgd, s_rTgd);
   eps = pow(2.0,s_rTgd);
   TUASSERTFEPS(dtest,rTgd,eps);

   startbit += n_rTgd;
   ultest = pnb.asUnsignedLong(startbit, n_riodc, s_riodc);
   TUASSERTE(unsigned long, ultest, riodc);

   startbit += n_riodc;
   dtest = pnb.asUnsignedDouble(startbit, n_rToc, s_rToc);
   eps = pow(2.0,s_rToc);
   TUASSERTFEPS(dtest,rToc,eps);

   startbit += n_rToc;
   dtest = pnb.asSignedDouble(startbit, n_raf2, s_raf2);
   eps = pow(2.0,s_raf2);
   TUASSERTFEPS(dtest,raf2,eps);

   startbit += n_raf2;
   dtest = pnb.asSignedDouble(startbit, n_raf1, s_raf1);
   eps = pow(2.0,s_raf1);
   TUASSERTFEPS(dtest,raf1,eps);

   startbit += n_raf1;
   dtest = pnb.asSignedDouble(startbit, n_raf0, s_raf0);
   eps = pow(2.0,s_raf0);
   TUASSERTFEPS(dtest,raf0,eps);

   startbit += n_raf0;
   ultest = pnb.asUnsignedLong(startbit, n_riode, s_riode);
   TUASSERTE(unsigned long, ultest, riode);

   startbit += n_riode;
   dtest = pnb.asSignedDouble(startbit, n_rCrs, s_rCrs);
   eps = pow(2.0,s_rCrs);
   TUASSERTFEPS(dtest,rCrs,eps);

   startbit += n_rCrs;
   dtest = pnb.asDoubleSemiCircles(startbit, n_rdn, s_rdn);
   eps = pow(2.0,s_rdn) * 3.0;
   TUASSERTFEPS(dtest,rdn,eps);

   startbit += n_rdn;
   dtest = pnb.asDoubleSemiCircles(startbit, n_rM0, s_rM0);
   eps = pow(2.0,s_rM0) * 3.0;
   TUASSERTFEPS(dtest,rM0,eps);

   startbit += n_rM0;
   dtest = pnb.asSignedDouble(startbit, n_rCuc, s_rCuc);
   eps = pow(2.0,s_rCuc);
   TUASSERTFEPS(dtest,rCuc,eps);

   startbit += n_rCuc;
   dtest = pnb.asUnsignedDouble(startbit, n_recc, s_recc);
   eps = pow(2.0,s_recc);
   TUASSERTFEPS(dtest,recc,eps);

   startbit += n_recc;
   dtest = pnb.asSignedDouble(startbit, n_rCus, s_rCus);
   eps = pow(2.0,s_rCus);
   TUASSERTFEPS(dtest,rCus,eps);

   startbit += n_rCus;
   dtest = pnb.asUnsignedDouble(startbit, n_rAhalf, s_rAhalf);
   eps = pow(2.0,s_rAhalf);
   TUASSERTFEPS(dtest,rAhalf,eps);

   startbit += n_rAhalf;
   dtest = pnb.asUnsignedDouble(startbit, n_rToe, s_rToe);
   eps = pow(2.0,s_rToe);
   TUASSERTFEPS(dtest,rToe,eps);

   startbit += n_rToe;
   ultest = pnb.asUnsignedLong(startbit, n_rfitInt, s_rfitInt);
   TUASSERTE(unsigned long, ultest, rfitInt);

   startbit += n_rfitInt;
   ultest = pnb.asUnsignedLong(startbit, n_raodo, s_raodo);
   TUASSERTE(unsigned long, ultest, raodo);

   startbit += n_raodo;
   dtest = pnb.asSignedDouble(startbit, n_rCic, s_rCic);
   eps = pow(2.0,s_rCic);
   TUASSERTFEPS(dtest,rCic,eps);

   startbit += n_rCic;
   dtest = pnb.asDoubleSemiCircles(startbit, n_rOMEGA0, s_rOMEGA0);
   eps = pow(2.0,s_rOMEGA0) * 3.0;
   TUASSERTFEPS(dtest,rOMEGA0,eps);

   startbit += n_rOMEGA0;
   dtest = pnb.asSignedDouble(startbit, n_rCis, s_rCis);
   eps = pow(2.0,s_rCis);
   TUASSERTFEPS(dtest,rCis,eps);

   startbit += n_rCis;
   dtest = pnb.asDoubleSemiCircles(startbit, n_ri0, s_ri0);
   eps = pow(2.0,s_ri0) * 3.0;
   TUASSERTFEPS(dtest,ri0,eps);

   startbit += n_ri0;
   dtest = pnb.asSignedDouble(startbit, n_rCrc, s_rCrc);
   eps = pow(2.0,s_rCrc);
   TUASSERTFEPS(dtest,rCrc,eps);

   startbit += n_rCrc;
   dtest = pnb.asDoubleSemiCircles(startbit, n_rw, s_rw);
   eps = pow(2.0,s_rw) * 3.0;
   TUASSERTFEPS(dtest,rw,eps);

   startbit += n_rw;
   dtest = pnb.asDoubleSemiCircles(startbit, n_rOMEGAdot, s_rOMEGAdot);
   eps = pow(2.0,s_rOMEGAdot) * 3.0;
   TUASSERTFEPS(dtest,rOMEGAdot,eps);

   startbit += n_rOMEGAdot;
   dtest = pnb.asUnsignedLong(startbit, n_riode, s_riode);
   eps = pow(2.0,s_riode);
   TUASSERTFEPS(dtest,riode,eps);

   startbit += n_riode;
   dtest = pnb.asDoubleSemiCircles(startbit, n_ridot, s_ridot);
   eps = pow(2.0,s_ridot) * 3.0;
   TUASSERTFEPS(dtest,ridot,eps);

   return testFramework.countFails();
}

unsigned PackedNavBits_T ::
equalityTest()
{
      // Create a master PackedNavBits, a copy, and 
      // several not-quite-copies.
      // At this point, the "packed bits" section of 
      // this object is empty.  We are focused on testing
      // the metadata handling. 
   SatID satID(1, SatID::systemGPS);
   ObsID obsID( ObsID::otNavMsg, ObsID::cbL2, ObsID::tcC2LM );
   std::string rxID = "rx1";
   CommonTime ct = CivilTime( 2011, 6, 2, 12, 14, 44.0, TimeSystem::GPS );

   SatID satID2(2, SatID::systemGPS);
   ObsID obsID2(ObsID::otNavMsg, ObsID::cbL5, ObsID::tcQ5 );
   std::string rxID2 = "rx2";
   CommonTime ctPlus = ct + 900.0;

   PackedNavBits master(satID,obsID,rxID,ct);
   PackedNavBits masterCopy(master);

   PackedNavBits diffSat(satID2,obsID,rxID,ct);
   PackedNavBits diffObs(satID,obsID2,rxID,ct); 
   PackedNavBits diffRx(satID,obsID,rxID2,ct); 
   PackedNavBits diffTime(satID,obsID,rxID,ctPlus); 
      // Typical same SV/OBS across multiple Rx/Time case.
   PackedNavBits diffRxTime(satID,obsID,rxID2,ctPlus); 

   TUDEF("PackedNavBits","matchMetaData");
   TUASSERTE(bool,true,master.matchMetaData(master));
   TUASSERTE(bool,true,master.matchMetaData(masterCopy));
   TUASSERTE(bool,false,master.matchMetaData(diffSat));
   TUASSERTE(bool,false,master.matchMetaData(diffObs));
   TUASSERTE(bool,false,master.matchMetaData(diffRx));
   TUASSERTE(bool,false,master.matchMetaData(diffTime));
 
   unsigned int ignoreSAT  =         PackedNavBits::mmOBS |  PackedNavBits::mmRX | PackedNavBits::mmTIME;
   unsigned int ignoreOBS  = PackedNavBits::mmSAT |          PackedNavBits::mmRX | PackedNavBits::mmTIME;
   unsigned int ignoreRX   = PackedNavBits::mmSAT | PackedNavBits::mmOBS |         PackedNavBits::mmTIME;
   unsigned int ignoreTIME = PackedNavBits::mmSAT | PackedNavBits::mmOBS | PackedNavBits::mmRX;
   unsigned int checkRXTIME = PackedNavBits::mmRX | PackedNavBits::mmTIME;
   unsigned int checkSATOBS = PackedNavBits::mmSAT | PackedNavBits::mmOBS;
   TUASSERTE(bool,true,master.matchMetaData( diffSat, ignoreSAT));
   TUASSERTE(bool,true,master.matchMetaData( diffObs, ignoreOBS));
   TUASSERTE(bool,true,master.matchMetaData(  diffRx,  ignoreRX));
   TUASSERTE(bool,true,master.matchMetaData(diffTime,ignoreTIME));
   TUASSERTE(bool,true,master.matchMetaData(diffRxTime,checkSATOBS));

   TUASSERTE(bool,false,master.matchMetaData(  diffSat,  PackedNavBits::mmSAT));
   TUASSERTE(bool,false,master.matchMetaData(  diffObs,  PackedNavBits::mmOBS));
   TUASSERTE(bool,false,master.matchMetaData(   diffRx,   PackedNavBits::mmRX));
   TUASSERTE(bool,false,master.matchMetaData( diffTime, PackedNavBits::mmTIME));
   TUASSERTE(bool,false,master.matchMetaData(diffRxTime, checkRXTIME));

      // Now keep the metadata the same across copies, but add some 
      // bits.  NOTE: The metadata is all left identical. 
   TUCSM("matchBits");
   PackedNavBits withBits(satID,obsID,rxID,ct);
   PackedNavBits withSameBits(satID,obsID,rxID,ct); 
   PackedNavBits withShortBits(satID,obsID,rxID,ct);
   PackedNavBits withLongBits(satID,obsID,rxID,ct);

      // Reuse test data from abstractTest( )
   unsigned long u_i1 = 32767;
   int u_n1           = 16;
   int u_s1           = 1;

   unsigned long u_i2 = 1;
   int u_n2           = 8;
   int u_s2           = 1;

   unsigned long u_i3 = 255;
   int u_n3           = 8;
   int u_s3           = 1;

   unsigned long u_i4 = 604500;
   int u_n4           = 11;
   int u_s4           = 300;

      // Pack the data
   withBits.addUnsignedLong(u_i1, u_n1, u_s1);
   withBits.addUnsignedLong(u_i2, u_n2, u_s2);
   withBits.addUnsignedLong(u_i3, u_n3, u_s3);
   withBits.trimsize();

   withSameBits.addUnsignedLong(u_i1, u_n1, u_s1);
   withSameBits.addUnsignedLong(u_i2, u_n2, u_s2);
   withSameBits.addUnsignedLong(u_i3, u_n3, u_s3);
   withSameBits.trimsize();

   withShortBits.addUnsignedLong(u_i1, u_n1, u_s1);
   withShortBits.addUnsignedLong(u_i2, u_n2, u_s2);
   withShortBits.trimsize();

   withLongBits.addUnsignedLong(u_i1, u_n1, u_s1);
   withLongBits.addUnsignedLong(u_i2, u_n2, u_s2);
   withLongBits.addUnsignedLong(u_i3, u_n3, u_s3);
   withLongBits.addUnsignedLong(u_i4, u_n4, u_s4);
   withLongBits.trimsize();

      // Basic "test for complete match"
   TUASSERTE(bool, true,withBits.matchBits(withSameBits));
   TUASSERTE(bool,false,withBits.matchBits(withShortBits));
   TUASSERTE(bool,false,withBits.matchBits(withLongBits));

      // Test for same bits in area that matches
      // NOTE: The second argument is the ending BIT NUMBER
      // NOT the number of bits.   
   TUASSERTE(bool, true,withBits.matchBits( withSameBits,0,23));

      // It is CORRECT that these two fail. Even though the
      // bits match, the overall length of the bits sets 
      // does NOT match.
   TUASSERTE(bool,false,withBits.matchBits(withShortBits,0,23));
   TUASSERTE(bool,false,withBits.matchBits( withLongBits,0,23));

      // Same bits as withBits, but in different order.  
      // Therefore, same length, but different contents.
      // In fact, the first TWO entries are swapped, but
      // the third entry should be the same bits in the 
      // same location.
   PackedNavBits diffOrder(satID,obsID,rxID,ct);

   diffOrder.addUnsignedLong(u_i2, u_n2, u_s2);  // 8 bits  (0- 7)
   diffOrder.addUnsignedLong(u_i1, u_n1, u_s1);  // 16 bits (8-23)
   diffOrder.addUnsignedLong(u_i3, u_n3, u_s3);
   diffOrder.trimsize();
   TUASSERTE(bool,false,withBits.matchBits(diffOrder));
   TUASSERTE(bool, true,withBits.matchBits(diffOrder,24,31));

      // Now build some test cases with both metadata AND bits
   TUCSM("match");
   PackedNavBits sameAsWithBits(satID,obsID,rxID,ct);
   sameAsWithBits.addUnsignedLong(u_i1, u_n1, u_s1);
   sameAsWithBits.addUnsignedLong(u_i2, u_n2, u_s2);
   sameAsWithBits.addUnsignedLong(u_i3, u_n3, u_s3);
   sameAsWithBits.trimsize();

   PackedNavBits diffMetaWithBits(satID2,obsID2,rxID2,ctPlus);
   diffMetaWithBits.addUnsignedLong(u_i1, u_n1, u_s1);
   diffMetaWithBits.addUnsignedLong(u_i2, u_n2, u_s2);
   diffMetaWithBits.addUnsignedLong(u_i3, u_n3, u_s3);
   diffMetaWithBits.trimsize();

      // Same SatID and ObsID, but different Rx and XmitTime
      // and with same bits 24-31 but different bits 0-23.
   PackedNavBits diffMetaWithBits2(satID,obsID,rxID2,ctPlus);
   diffMetaWithBits2.addUnsignedLong(u_i2, u_n2, u_s2);
   diffMetaWithBits2.addUnsignedLong(u_i1, u_n1, u_s1);
   diffMetaWithBits2.addUnsignedLong(u_i3, u_n3, u_s3);
   diffMetaWithBits2.trimsize();

   TUASSERTE(bool,  true,withBits.match(sameAsWithBits));
   TUASSERTE(bool, false,withBits.match(diffMetaWithBits));
   TUASSERTE(bool, false,withBits.match(diffMetaWithBits2));
   TUASSERTE(bool, false,withBits.match(diffMetaWithBits2,0,-1,checkSATOBS));
   TUASSERTE(bool,  true,withBits.match(diffMetaWithBits2,24,31,checkSATOBS));

   TUCSM("operator==");
   TUASSERTE(bool,  true,withBits==sameAsWithBits);
   TUASSERTE(bool, false,withBits==diffMetaWithBits2);

   TUCSM("operator<");
   PackedNavBits rightTest(satID,obsID,rxID2,ct); 
   PackedNavBits leftSmall(satID,obsID,rxID2,ct);
   PackedNavBits leftLarge(satID,obsID,rxID2,ct);
   PackedNavBits leftEqual(satID,obsID,rxID2,ct);
   PackedNavBits longer(satID,obsID,rxID2,ct);
   rightTest.rawBitInput("035 0xFFFFFF0F 0xE0000000");
   leftEqual.rawBitInput("035 0xFFFFFF0F 0xE0000000");
   leftSmall.rawBitInput("035 0xFFFFFE0F 0xE0000000");
   leftLarge.rawBitInput("035 0xFFFFFFFF 0xE0000000");
   longer.rawBitInput(   "064 0x00000000 0x00000000");
   TUASSERTE(bool,  true, leftSmall<rightTest);
   TUASSERTE(bool, false, leftEqual<rightTest);
   TUASSERTE(bool, false, leftLarge<rightTest);
   TUASSERTE(bool,  true, leftSmall<longer);
   TUASSERTE(bool, false, longer<leftSmall);

   TURETURN();
}

int main()
{
   unsigned errorTotal = 0;

   PackedNavBits_T testClass;

   errorTotal += testClass.abstractTest();
   errorTotal += testClass.realDataTest();
   errorTotal += testClass.equalityTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; // Return the total number of errors
}


