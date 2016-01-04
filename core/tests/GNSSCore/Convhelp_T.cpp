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
#include <iostream>
#include <cmath>

#include "convhelp.hpp"
#include "GPSEllipsoid.hpp"

#include "TestUtil.hpp"

int main() 
{
   TUDEF("convhelp", "");
   double eps = 1e-12;

   gpstk::WGS84Ellipsoid wem;
   gpstk::GPSEllipsoid gem;

   testFramework.changeSourceMethod("WGS84Ellipsoid cycles2meters");
   TUASSERTFEPS(gpstk::cycles2meters(100000., 400000.,wem), 74948114.5, eps);
   TUASSERTFEPS(gpstk::cycles2meters(0,5,wem), 0, eps);
   TUASSERTFEPS(gpstk::cycles2meters(-100000.,400000.,wem), -74948114.5, eps);
   
   testFramework.changeSourceMethod("GPSEllipsoid cycles2meters");
   TUASSERTFEPS(gpstk::cycles2meters(100000.,400000.,gem), 74948114.5, eps);
   TUASSERTFEPS(gpstk::cycles2meters(0,5,gem), 0, eps);
   TUASSERTFEPS(gpstk::cycles2meters(-100000.,400000.,gem), -74948114.5, eps);

   testFramework.changeSourceMethod("WGS84Ellipsoid meters2cycles");
   TUASSERTFEPS(gpstk::meters2cycles(74948114.5,400000.,wem), 100000, eps);
   TUASSERTFEPS(gpstk::meters2cycles(0,5,wem), 0, eps);
   TUASSERTFEPS(gpstk::meters2cycles(-74948114.5,400000.,wem), -100000, eps);

   testFramework.changeSourceMethod("GPSEllipsoid meters2cycles");
   TUASSERTFEPS(gpstk::meters2cycles(74948114.5,400000.,gem), 100000, eps);
   TUASSERTFEPS(gpstk::meters2cycles(0,5,gem), 0, eps);
   TUASSERTFEPS(gpstk::meters2cycles(-74948114.5,400000.,gem), -100000, eps);
   
   testFramework.changeSourceMethod("cel2far");
   TUASSERTFEPS(gpstk::cel2far(100),   212, eps);
   TUASSERTFEPS(gpstk::cel2far(0),      32, eps);
   TUASSERTFEPS(gpstk::cel2far(-100), -148, eps);

   testFramework.changeSourceMethod("far2cel");
   TUASSERTFEPS(gpstk::far2cel(212),   100, eps);
   TUASSERTFEPS(gpstk::far2cel(32),      0, eps);
   TUASSERTFEPS(gpstk::far2cel(-148), -100, eps);

   testFramework.changeSourceMethod("mb2hg");
   TUASSERTFEPS(gpstk::mb2hg(100), 2.9529987508079487, eps);
   TUASSERTFEPS(gpstk::mb2hg(0), 0, eps);
   TUASSERTFEPS(gpstk::mb2hg(-100), -2.9529987508079487, eps);

   testFramework.changeSourceMethod("hg2mb");
   TUASSERTFEPS(gpstk::hg2mb(2.9529987508079487), 100, eps);
   TUASSERTFEPS(gpstk::hg2mb(0), 0, eps);
   TUASSERTFEPS(gpstk::hg2mb(-2.9529987508079487), -100, eps);

   std::cout << "Total Failures for " << __FILE__ << ": " << testFramework.countFails() << std::endl;

   return testFramework.countFails();
}
