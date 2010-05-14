#pragma ident "$Id: $"

/**
 * @file RelativityEffect.cpp
 * Calculate Relativity Effect to earth satellite
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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================


#include "RelativityEffect.hpp"
#include "ASConstant.hpp"

namespace gpstk
{
   // this is the real one
   void RelativityEffect::doCompute(UTCTime utc, EarthBody& rb, Spacecraft& sc)
   {
      /* reference: Jisheng,Li P110 Bernese5 GENREL.f
         a_rl = a_rl1 + a_rl2 + a_rl3

         a_rl2 and a_rl3 are ignored for precise orbit determination
      */
      const double GM = ASConstant::GM_Earth;
      const double C = ASConstant::SPEED_OF_LIGHT;
      
      Vector<double> r = sc.R();
      Vector<double> v = sc.V();

      double beta = 1.0;
      double gama = 1.0;
      
      double c2 = C * C;
      double r2 = dot(r,r);
      double v2 = dot(v,v);

      double r_mag = norm(r);
      double r3 = r2 * r_mag;
      
      double p = GM/c2/r3;
      
      // a
      a.resize(3,0.0);
      
      double pr = 2.0 * (beta + gama) * GM / r_mag - gama * v2;
      double pv = 2.0 * (1.0 + gama) * dot(r,v);
      
      a = p * ( pr * r + pv * v );

      // da_dr
      da_dr.resize(3,3,0.0);

      double prr = -(GM/r_mag)*(GM/r_mag)*(2.0*(beta+gama)/c2);
      double pvv = (GM/r3)*(2.0*(1.0+gama)/c2);
      double par = -3.0/r2;
      double ppr = (GM/r3)*((GM/r_mag)*(2.0*(beta+gama)/c2)-gama*v2/c2);

      for(int i=0; i<3; i++)
      {
         for(int j=0; j<3; j++)
         {
            double det = (i == j) ? 1.0 : 0.0;
            
            da_dr(i,j) = prr*r(i)*r(j)
               + pvv*v(i)*v(j)
               + par*a(i)*r(j)
               + ppr*det;
         }
      }
      
      // da_dv
      da_dv.resize(3,3,0.0);
      
      double prv = -(GM/r3)*(2.0*gama/c2);
      double pvr = (GM/r3)*(2.0*(1.0+gama)/c2);
      double ppv = pvr*dot(r,v);

      for(int i=0;i<3;i++)
      {
         for(int j=0;j<3;j++)
         {
            double det = (i == j) ? 1.0 : 0.0;

            da_dr(i,j) = prv*r(i)*v(j)
               + pvr*v(i)*r(j)
               + ppv*det;
         }
      }

      // da_dp  add it later...
      //da_GM da_dbeta da_gama
      
   }  // End of method 'RelativityEffect::doCompute()'


}  // End of namespace 'gpstk'




