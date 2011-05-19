#pragma ident "$Id"

/**
 * @file GlonassSatelliteModel.cpp
 * This class implements a simple model of a GLONASS satellite.
 */

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2011
//
//============================================================================


#include "GlonassSatelliteModel.hpp"


namespace gpstk
{

      // Internal initialization method
   void GlonassSatelliteModel::Init(void)
   {
         // Let's initialize some important values to save some time
      j20 = pz90.j20();
      mu = pz90.gm();
      ae = pz90.a();
      we = pz90.angVelocity();
      we2 = we*we;

   }  // End of 'GlonassSatelliteModel::Init(void)'

      
      /* Implements "derivative()". It is based on accelerations.
       *
       * @param time          Time step.
       * @param inState       Internal state matrix.
       * @param inStateDot    Derivative of internal state matrix.
       */
   Matrix<double>& GlonassSatelliteModel::derivative( long double time,
                                                  const Matrix<double>& inState,
                                                  Matrix<double>& inStateDot )
   {

         // The GLONASS satellite accelerations are NOT just the luni-solar
         // accelerations. We need to compute the right ones.

         // Let's start getting the current satellite position and velocity
      double  x( inState(0,0) );          // X coordinate
      double vx( inState(1,0) );          // X velocity
      double  y( inState(2,0) );          // Y coordinate
      double vy( inState(3,0) );          // Y velocity
      double  z( inState(4,0) );          // Z coordinate
      double vz( inState(5,0) );          // Z velocity

         // The distance from satellite to Earth center is an important value
      double r2(x*x+y*y+z*z);
      double r( std::sqrt(r2) );
      double k1(mu/r2);
      double k2(1.5*j20*(ae/r)*(ae/r));
      double k3(5.0*(z/r)*(z/r));

         // Compute the GLONASS accelerations. Please note that they are
         // expressed in an ECEF system (i.e. Coriolis forces are included)
      double gloAx( (-1.0 + k2*(1.0-k3))*k1*(x/r) + ax + we2*x + 2.0*we*vy );
      double gloAy( (-1.0 + k2*(1.0-k3))*k1*(y/r) + ay + we2*y - 2.0*we*vx );
      double gloAz( (-1.0 + k2*(3.0-k3))*k1*(z/r) + az );

         // Let's insert data related to X coordinates
      inStateDot(0,0) = inState(1,0);     // Set X'  = Vx
      inStateDot(1,0) = gloAx;            // Set Vx' = gloAx

         // Let's insert data related to Y coordinates
      inStateDot(2,0) = inState(3,0);     // Set Y'  = Vy
      inStateDot(3,0) = gloAy;            // Set Vy' = gloAy

         // Let's insert data related to Z coordinates
      inStateDot(4,0) = inState(5,0);     // Set Z'  = Vz
      inStateDot(5,0) = gloAz;            // Set Vz' = gloAz

      return inStateDot;

   }  // End of method 'GlonassSatelliteModel::derivative()'


}  // End of namespace gpstk
