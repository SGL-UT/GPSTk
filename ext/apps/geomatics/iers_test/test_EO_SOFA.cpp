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

/// @file test_EO_SOFA.cpp Test IERS routines by comparing to output of examsofa.c,
/// which implements examples from SOFA document "SOFA Tools for Earth Attitude".
/// SOFA is Standards Of Fundamental Astronomy of the IAU.
/// cf. Doc at bottom of ext/lib/Geomatics/EarthOrientation.cpp

// system includes
#include <string>
// gpstk
#include "Exception.hpp"
#include "GNSSconstants.hpp"
#include "Matrix.hpp"
// geomatics
#include "EphTime.hpp"
#include "CommandLine.hpp"
#include "SolarSystem.hpp"
#include "EarthOrientation.hpp"
#include "logstream.hpp"

using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
static const string testVersion("1.0 5/19/17");

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   int i;
   EarthOrientation nGF96; nGF96.convention = IERSConvention::IERS1996;
   EarthOrientation nGF03; nGF03.convention = IERSConvention::IERS2003;
   EarthOrientation nGF10; nGF10.convention = IERSConvention::IERS2010;

   // sofa example section 5 of "SOFA Tools for Earth Attitude"

   // turn on debug7 so we get output of intermediate quantities from within class EO
   LOGlevel = ConfigureLOG::Level("DEBUG7");

   // define input
   //ttag.setYMDHMS(2007,4,5,12,0,0.0,TimeSystem::UTC);
   double mjd = 54195.5;      // UTC
   EphTime ttag;
   ttag.setMJD(static_cast<long double>(54195.5));
   ttag.setTimeSystem(TimeSystem::UTC);
   double UT1mUTC,xp,yp;
   UT1mUTC = -0.072073685;    // sec
   xp = 0.0349282;            // arcsec
   yp = 0.4833163;            // arcsec

   LOG(INFO) << "Test EarthOrientation";
   LOG(INFO) << "Epoch 2007/ 4/ 5 12: 0:0.000 UTC";
   LOG(INFO) << "Polar angles xp yp = " << fixed << setprecision(7)
               << xp << " " << yp << " arcsec";
   LOG(INFO) << "UT1-UTC = " << fixed << setprecision(9) << UT1mUTC;

   // units - don't do this, ARL:UT code takes them in arcsec
   //xp *= EarthOrientation::ARCSEC_TO_RAD;   // rad
   //yp *= EarthOrientation::ARCSEC_TO_RAD;   // rad

   // time tags
   double UT1=mjd+UT1mUTC/SEC_PER_DAY;
   double dt = TimeSystem::Correction(TimeSystem::UTC,TimeSystem::TAI,2007,4,5.5);
   double TAI=mjd+dt/SEC_PER_DAY;
   dt = TimeSystem::Correction(TimeSystem::UTC,TimeSystem::TT,2007,4,5.5);
   double TT =mjd+dt/SEC_PER_DAY;
   LOG(INFO) << fixed
      << setprecision(1) << "Times: UTC " << MJD_TO_JD << " + "
         << setprecision(15) << setw(21) << mjd << endl
      << setprecision(1) << "       TAI " << MJD_TO_JD << " + "
         << setprecision(15) << setw(21) << TAI << endl
      << setprecision(1) << "       TT  " << MJD_TO_JD << " + "
         << setprecision(15) << setw(21) << TT << endl
      << setprecision(1) << "       UT1 " << MJD_TO_JD << " + "
         << setprecision(15) << setw(21) << UT1;

   Matrix<double> M;

   LOG(INFO) << "\nIERS 1996 / IAU 1976/1980 "
      << "------------------------------------------" << "\n\nNO Adjustments made";
   nGF96.xp = xp; nGF96.yp = yp; nGF96.UT1mUTC = UT1mUTC;
   M = nGF96.ECEFtoInertial(ttag);
   LOG(INFO) << "\ncelestial-to-terrestrial matrix:\n" << showpos << fixed
      << setprecision(15) << setw(18) << transpose(M);

   LOG(INFO) << "\nIERS 2003 / IAU 2000A, CIO "
      << "-----------------------------------------" << "\n\nNO Adjustments made";
   nGF03.xp = xp; nGF03.yp = yp; nGF03.UT1mUTC = UT1mUTC;
   M = nGF03.ECEFtoInertial(ttag);
   LOG(INFO) << "\ncelestial-to-terrestrial matrix:\n" << showpos << fixed
      << setprecision(15) << setw(18) << transpose(M);

   LOG(INFO) << "\nIERS 2010 / IAU 2000A, CIO "
      << "-----------------------------------------\n"
      << "\nUsing XY series\n"
      << "\nNO Adjustments made\n";
   nGF10.xp = xp; nGF10.yp = yp; nGF10.UT1mUTC = UT1mUTC;
   M = nGF10.ECEFtoInertial(ttag);
   LOG(INFO) << "\ncelestial-to-terrestrial matrix:\n" << showpos << fixed
      << setprecision(15) << setw(18) << transpose(M);

   return 0;
}
catch(Exception& e) { cerr << "Exception: " << e; }
catch (...) { cerr << "Unknown exception.  Abort." << endl; }
   return 1;
}   // end main()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
