#pragma ident "$Id$"

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

#include "xRACRotation.hpp"
#include <stdio.h>

#include "math.h"

#include "RACRotation.hpp"
#include "GNSSconstants.hpp"
#include "Xvt.hpp"
#include "Position.hpp"


CPPUNIT_TEST_SUITE_REGISTRATION (xRACRotation);

using namespace std;
using namespace gpstk;

void xRACRotation :: setUp (void)
{
	GPSAlt = 26000000.0;   // 26 million meters
}

// First test case.  SV at GPS orbit altitude at 0 deg N, 0 deg E
// Heading due N at 4 km/sec (Not realistic, but we want something
// simple to start with;
void xRACRotation :: firstTest (void)
{
   Triple testSVPos( GPSAlt, 0, 0);
   Triple testSVVel( 0, 0, 4000 );
   RACRotation rot( testSVPos, testSVVel );

      // OK, now give it an "error vector" that 1 m in each component
   testErrXYZ = Triple( 1.0, 1.0, 1.0);
   testErrRAC = rot.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_EQUAL(Triple(1,1,-1),testErrRAC);

   testErrXYZ = Triple( -1.0, -1.0, -1.0);
   testErrRAC = rot.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_EQUAL(Triple(-1,-1,1),testErrRAC);

   testErrXYZ = Triple( 0.0, 0.0, -1.0);
   testErrRAC = rot.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_EQUAL(Triple(0,-1,0),testErrRAC);

   testErrXYZ = Triple( 0.0, -1.0, 0.0);
   testErrRAC = rot.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_EQUAL(Triple(0,0,1),testErrRAC);
}

// Second test case.  SV at GPS orbit altitude at 0 deg N, 90 deg E
// Heading due S at 4 km/sec (Not realistic, but we want something
// simple to start with;
void xRACRotation :: secondTest (void)
{
   Triple testSVPos2( 0, GPSAlt, 0);
   Triple testSVVel2( 0, 0, -4000 );

   RACRotation rot2( testSVPos2, testSVVel2 );

      // OK, now give it an "error vector" that 1 m in each component
   testErrXYZ = Triple( 1.0, 1.0, 1.0);
   testErrRAC = rot2.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_EQUAL(Triple(1,-1,-1),testErrRAC);

   testErrXYZ = Triple( -1.0, -1.0, -1.0);
   testErrRAC = rot2.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_EQUAL(Triple(-1,1,1),testErrRAC);

   testErrXYZ = Triple( 0.0, 0.0, -1.0);
   testErrRAC = rot2.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_EQUAL(Triple(0,1,0),testErrRAC);

   testErrXYZ = Triple( 0.0, -1.0, 0.0);
   testErrRAC = rot2.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_EQUAL(Triple(-1,0,0),testErrRAC);
}

// Third test case.  SV at GPS orbit altitude at 90 deg N, X deg E
// Heading parallel to the Y axis at 4 km/sec
// (Not realistic, but I want to see degenerate cases in all axes)
void xRACRotation :: thirdTest (void)
{

   Triple testSVPos3( 0, 0, GPSAlt);
   Triple testSVVel3( 0, 4000, 0 );

   RACRotation rot3( testSVPos3, testSVVel3 );

      // OK, now give it an "error vector" that 1 m in each component
   testErrXYZ = Triple( 1.0, 1.0, 1.0);
   testErrRAC = rot3.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_EQUAL(Triple(1,1,-1),testErrRAC);

   testErrXYZ = Triple( -1.0, -1.0, -1.0);
   testErrRAC = rot3.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_EQUAL(Triple(-1,-1,1),testErrRAC);

   testErrXYZ = Triple( 0.0, 0.0, -1.0);
   testErrRAC = rot3.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_EQUAL(Triple(-1,0,0),testErrRAC);

   testErrXYZ = Triple( 0.0, -1.0, 0.0);
   testErrRAC = rot3.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_EQUAL(Triple(0,-1,0),testErrRAC);
}

// Fourth test case.  SV at GPS orbit altitude at 45 deg N, 45 deg E
// Heading  4 km/sec
// (Not realistic, but I want to see degenerate cases in all axes)
void xRACRotation :: fourthTest (void)
{
      // A little Spherical to Cartesian slight of hand to make sure
      // I know where the SV vectors are.
      // ECEF
      // Spherical: R = 26M m, phi = 45 deg N, theta = 45 deg E
      // x = r sin phi cos theta
      // y = r sin phi sin theta
      // z = r cos phi
   double thetaphi = 45.0 * ( PI / 180.0 );
   double x4 = GPSAlt * sin(thetaphi) * cos(thetaphi);
   double y4 = GPSAlt * sin(thetaphi) * sin(thetaphi);
   double z4 = GPSAlt * cos(thetaphi);
   Triple testSVPos4( x4, y4, z4 );

      // Velocity
      // Spherical: R = 4000 m/sec, phi = 45 deg E, theta = 135 deg E
      // This rotates the radius vector 90 degrees about Z so the
      // velocity should be perpendicular to the radius.
      // Should make the end result roughly local E
      // x = r sin phi cos theta
      // y = r sin phi sin theta
      // z = r cos phi
   double theta = 135.0 * (PI/180.0);
   double phi = 45.0 * (PI/180.0);
   double xv4 = GPSAlt * sin(phi) * cos(theta);
   double yv4 = GPSAlt * sin(phi) * sin(theta);
   double zv4 = GPSAlt * cos(phi);
   Triple testSVVel4( xv4, yv4, zv4 );

   RACRotation rot4( testSVPos4, testSVVel4 );

      // OK, now give it an "error vector" that 1 m in each component
   testErrXYZ = Triple( 1.0, 1.0, 1.0);
   testErrRAC = rot4.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_DOUBLES_EQUAL(1.70711,testErrRAC[0],1e-4);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.169102,testErrRAC[1],1e-4);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(-.239146,testErrRAC[2],1e-4);

   testErrXYZ = Triple( -1.0, -1.0, -1.0);
   testErrRAC = rot4.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_DOUBLES_EQUAL(-1.70711,testErrRAC[0],1e-4);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(0.169102,testErrRAC[1],1e-4);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(.239146,testErrRAC[2],1e-4);

   testErrXYZ = Triple( 0.0, 0.0, -1.0);
   testErrRAC = rot4.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_DOUBLES_EQUAL(-.70711,testErrRAC[0],1e-4);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(-.408248,testErrRAC[1],1e-4);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(-.57735,testErrRAC[2],1e-4);

   testErrXYZ = Triple( 0.0, -1.0, 0.0);
   testErrRAC = rot4.convertToRAC( testErrXYZ );

   CPPUNIT_ASSERT_DOUBLES_EQUAL(-.5,testErrRAC[0],1e-4);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(-.288675,testErrRAC[1],1e-4);
   CPPUNIT_ASSERT_DOUBLES_EQUAL(.816497,testErrRAC[2],1e-4);
}

      // Repeat first test case using the Xvt signatures of
      // the RACRotation class
void xRACRotation :: fifthTest (void)
{

   Xvt rotxvt;
   rotxvt.x = Position( GPSAlt, 0, 0 );
   rotxvt.v = Triple( 0, 0, 4000 );
   rotxvt.clkbias = 0.0;
   rotxvt.clkdrift = 0.0;
   rotxvt.relcorr = 0.0;

   RACRotation rot5( rotxvt );

      // OK, now set up a unit vector in the original radius and the
      // original velocity and see that the results align properly.
   Xvt testxvt1;
   testxvt1.x = Position( 1.0, 0.0, 0.0 );
   testxvt1.v = Triple( 0.0, 0.0, 1.0 );
   testxvt1.clkbias = 0.0;
   testxvt1.clkdrift = 0.0;
   testxvt1.relcorr = 0.0;
   Xvt testxvtRAC1 = rot5.convertToRAC( testxvt1 );

   CPPUNIT_ASSERT_EQUAL(Position(1,0,0),(gpstk::Position)testxvtRAC1.x);
   CPPUNIT_ASSERT_EQUAL(Triple(0,1,0),testxvtRAC1.v);
   CPPUNIT_ASSERT_EQUAL(0.0,testxvtRAC1.clkbias);
   CPPUNIT_ASSERT_EQUAL(0.0,testxvtRAC1.clkdrift);
   CPPUNIT_ASSERT_EQUAL(0.0,testxvtRAC1.relcorr);

      // Reverse the two vectors and observe the results
   testxvt1.x = Position( -1.0, 0.0, 0.0);
   testxvt1.v = Triple(  0.0, 0.0,-1.0);
   testxvt1.clkbias = 0.0;
   testxvt1.clkdrift = 0.0;
   testxvt1.relcorr = 0.0;
   testxvtRAC1 = rot5.convertToRAC( testxvt1 );

   CPPUNIT_ASSERT_EQUAL(Position(-1,0,0),(gpstk::Position)testxvtRAC1.x);
   CPPUNIT_ASSERT_EQUAL(Triple(0,-1,0),testxvtRAC1.v);
   CPPUNIT_ASSERT_EQUAL(0.0,testxvtRAC1.clkbias);
   CPPUNIT_ASSERT_EQUAL(0.0,testxvtRAC1.clkdrift);
   CPPUNIT_ASSERT_EQUAL(0.0,testxvtRAC1.relcorr);

   testErrXYZ = Triple( 0.0, 0.0, -1.0);
   testErrRAC = rot5.convertToRAC( testErrXYZ );
}

