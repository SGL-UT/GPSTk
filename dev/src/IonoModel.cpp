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
 * @file IonoModel.cpp
 * Implementation of the ICD-GPS-200 Ionosphere model.
 */

#include <math.h>
#include "icd_200_constants.hpp"
#include "IonoModel.hpp"

namespace gpstk
{
   IonoModel::IonoModel(const double a[4], const double b[4])
      throw()
         : valid(true)
   {
      for (int n = 0; n < 4; n++)
      {
         alpha[n] = a[n];
         beta[n] = b[n];
      }
   }

   IonoModel::IonoModel(const EngAlmanac& engalm)
      throw()
   {
      try
      {
         engalm.getIon(alpha, beta);
         valid = true;
      }
      catch(InvalidRequest& e)
      {
         valid = false;
      }
   }
   
   double IonoModel::getCorrection(const DayTime& time,
                                   const Geodetic& rxgeo,
                                   double svel,
                                   double svaz,
                                   Frequency freq) const
      throw(IonoModel::InvalidIonoModel)
   {

      if (!valid)
      {
         InvalidIonoModel e("Alpha and beta parameters invalid.");
         GPSTK_THROW(e);
      }
      
         // all angle units are in semi-circles (radians / TWO_PI)
         // Note: math functions (cos, sin, etc.) require arguments in
         // radians so all semi-circles must be multiplied by TWO_PI

      double svE = svel / 360.0;
      double svA = svaz / 360.0;

      double phi_u = rxgeo.getLatitude() / 360.0;
      double lambda_u = rxgeo.getLongitude() / 360.0;
      
      double psi = 0.0137 / (svE + 0.11) - 0.022;
      
      double phi_i = phi_u + psi * cos(svA*TWO_PI);
      if (phi_i > 0.416)
         phi_i = 0.416;
      if (phi_i < -0.416)
         phi_i = -0.416;

      double lambda_i = lambda_u + psi * sin(svA*TWO_PI) / cos(phi_i*TWO_PI);
      
      double phi_m = phi_i + 0.064 * cos((lambda_i - 1.617)*TWO_PI);
      
      double iAMP = 0.0;
      double iPER = 0.0;
      for (int n = 0; n < 4; n++)
      {
         iAMP += alpha[n] * pow(phi_m, n);
         iPER += beta[n] * pow(phi_m, n);
      }
      if (iAMP < 0.0)
         iAMP = 0.0;
      if (iPER < 72000.0)
         iPER = 72000.0;
      
      double t = 43200.0 * lambda_i + time.DOYsecond();
      if (t >= 86400.0)
         t -= 86400.0;
      if (t < 0)
         t += 86400.0;

      double x = TWO_PI * (t - 50400.0) / iPER; // x is in radians
      
      double iF = 1.0 + 16.0 * pow(0.53 - svE, 3);

      double t_iono = 0.0;
      if (fabs(x) < 1.57)
         t_iono = iF * (5.0e-9 + iAMP * (1 - pow(x, 2)/2 + pow(x, 4)/24));
      else
         t_iono = iF * 5.0e-9;
      
      if (freq == L2)
      {
            // see ICD-GPS-200 20.3.3.3.3.2
         double gamma = pow(1575.42 / 1227.6, 2); //  (fL1 / fL2)^2
         t_iono *= gamma;
      }
      
      double correction = t_iono * C_GPS_M;
      
      return correction;
   }
   
   bool IonoModel::operator==(const IonoModel& right) const
      throw()
   {
      for (int n = 0; n < 4; n++)
      {
         if (alpha[n] != right.alpha[n] || beta[n] != right.beta[n])
            return false;
      }
      return true;
   }

   bool IonoModel::operator!=(const IonoModel&right) const
      throw()
   {
      return !(operator==(right));
   }   
}

         
         
      
