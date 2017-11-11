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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
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
 * @file MoonForce.cpp
 * This class calculate the gravitational effect of the moon.
 */

#include "MoonForce.hpp"
#include "ASConstant.hpp"
#include "ReferenceFrames.hpp"

namespace gpstk
{

   MoonForce::MoonForce()
      : mu(ASConstant::GM_Moon)
   {}


      /* Call the relevant methods to compute the acceleration.
      * @param t Time reference class
      * @param bRef Body reference class
      * @param sc Spacecraft parameters and state
      * @return the acceleration [m/s^s]
      */
   void MoonForce::doCompute(UTCTime utc, EarthBody& rb, Spacecraft& sc)
   {
      /* Oliver P69 and P248
       * a = GM*( (s-r)/norm(s-r)^3 - s/norm(s)^3 )
       *
       * da/dr = -GM*( I/norm(r-s)^3 - 3(r-s)transpose(r-s)/norm(r-s)^5)
       */

      Vector<double> r_moon = ReferenceFrames::getJ2kPosition(utc.asTDB(), SolarSystem::idMoon);
      
      r_moon = r_moon * 1000.0;         // from km to m

      Vector<double> d = sc.R() - r_moon;
      double dmag = norm(d);
      double dcubed = dmag * dmag *dmag;

      Vector<double> temp1 = d / dcubed;         //  detRJ/detRJ^3

      double smag = norm(r_moon);
      double scubed = smag * smag * smag;

      Vector<double> temp2 = r_moon / scubed;   //  Rj/Rj^3

      Vector<double> sum = temp1 + temp2;
      a = sum * (-mu);                          // a

      // da_dr
      da_dr.resize(3,3,0.0);
      double muod3 = mu / dcubed;
      double jk = 3.0 * muod3/dmag/dmag; 

      double xx = d(0);
      double yy = d(1);
      double zz = d(2);

      da_dr(0,0) = jk * xx * xx - muod3;
      da_dr(0,1) = jk * xx * yy;
      da_dr(0,2) = jk * xx * zz;

      da_dr(1,0) = da_dr(0,1);
      da_dr(1,1) = jk * yy * yy - muod3;
      da_dr(1,2) = jk * yy * zz;

      da_dr(2,0) = da_dr(0,2);
      da_dr(2,1) = da_dr(1,2);
      da_dr(2,2) = jk * zz * zz - muod3;

      // da_dv
      da_dv.resize(3,3,0.0);


      //da_dp
      
   }  // End of method 'MoonForce::doCompute()'


}  // End of namespace 'gpstk'
