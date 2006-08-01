#pragma ident "$Id$"


//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
 * @file ECEF.cpp
 * Encapsulate Earth-centered, Earth-fixed Cartesian coordinates
 */

#include "geometry.hpp"
#include "ECEF.hpp"
#include "Geodetic.hpp"
#include "MiscMath.hpp"

namespace gpstk
{
   using namespace std;

   ECEF :: ECEF()
         : Triple()
   {
   }

   ECEF :: ECEF(const ECEF& right)
         : Triple(right)
   {
   }

   ECEF& ECEF :: operator=(const ECEF& right)
   {
      Triple::operator=(right);
      return *this;
   }

   // Convert Earth-centered, Earth-fixed XYZ coordinates (m)
   // to Geodetic coordinates (lat,lon(E),ht) (deg,degE,m).
   Geodetic ECEF::asGeodetic(GeoidModel* geoid)
   {
      double X = this->operator[](0); //ecef[0];     // m
      double Y = this->operator[](1); //ecef[1];     // m
      double Z = this->operator[](2); //ecef[2];     // m
      double p = RSS(X,Y);
      double latd = atan2(Z,p*(1.0-geoid->eccSquared()));
      double ht = 0.0, slatd, N, htold, latdold;
      for(int i=0; i<5; i++) {
         slatd = sin(latd);
         N = geoid->a() / SQRT(1.0-geoid->eccSquared()*slatd*slatd);
         htold = ht;
         ht = p/cos(latd) - N;
         latdold = latd;
         latd = atan2(Z,p*(1.0-geoid->eccSquared()*(N/(N+ht))));
         if(ABS(latd-latdold) < 1.0e-9 && 
            ABS(ht-htold) < (1.0e-9*geoid->a())) break;
      }
      double lon = atan2(Y,X);
      if(lon < 0.0) lon += 6.2831853071796;

      Geodetic g(latd*RAD_TO_DEG,lon*RAD_TO_DEG,ht,geoid);      // deg,deg E,m
      return g;

   }  // end asGeodetic(geoid)

}  // namespace gpstk
