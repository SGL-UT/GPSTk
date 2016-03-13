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
#include "CommonTime.hpp"
#include "GNSSconstants.hpp"
#include "ObsID.hpp"
#include "PackedNavBits.hpp"
#include "SatID.hpp"
#include "TestUtil.hpp"
#include "TimeString.hpp"

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
   double d_e1 = pow(2,d_s1);   // value of lsb

   double d_i2 = -0.5;
   int d_n2    = 16;
   int d_s2    = -16;
   double d_e2 = pow(2,d_s2);   // value of lsb

   double d_i3 = 0;
   int d_n3    = 16;
   int d_s3    = -16;
   double d_e3 = pow(2,d_s3);   // value of lsb

   // Test Unsigned Doubles
   double d_i4 = 32000.0;
   int d_n4    = 16;
   int d_s4    = 0;
   double d_e4 = pow(2,d_s4);   // value of lsb

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
   double sd_i1 = PI-2*pow(2,-31);
   int sd_n1    = 32;
   int sd_s1    = -31;
   double sd_e1 = pow(2,sd_s1) * 3.0;   // value of lsb in semi-circles

   double sd_i2 = -PI;
   int sd_n2    = 32;
   int sd_s2    = -31;
   double sd_e2 = pow(2,sd_s1) * 3.0;   // value of lsb

   pnb.addDoubleSemiCircles(sd_i1, sd_n1, sd_s1);
   pnb.addDoubleSemiCircles(sd_i2, sd_n2, sd_s2); 

   dtest = pnb.asDoubleSemiCircles(startbit, sd_n1, sd_s1);
   TUASSERTFEPS(dtest, sd_i1, sd_e1);

   std::cout << setprecision(12) << " sd_i1 " << sd_i1 << std::endl;
   std::cout << " dtest : " << dtest << std::endl;
   std::cout << setprecision(6);
   long bits = pnb.asLong(startbit, sd_n1, 1);
   std::cout << " unsigned : " << bits << std::endl;

   startbit += sd_n1;
   dtest = pnb.asDoubleSemiCircles(startbit, sd_n2, sd_s2);
   TUASSERTFEPS(dtest, sd_i2, sd_e2);

   return testFramework.countFails();
}

unsigned PackedNavBits_T ::
realDataTest()
{
   TUDEF("PackedNavBits", "addUnsigned");
   TUCSM("addSigned");
   TUCSM("addDouble");
   TUCSM("addSemiCircles");
   return testFramework.countFails();
}

unsigned PackedNavBits_T ::
equalityTest()
{
   TUDEF("PackedNavBits","matchMetaData");
   TUCSM("matchBits");
   TUCSM("match");
   TUCSM("operator==");
   return testFramework.countFails();
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


