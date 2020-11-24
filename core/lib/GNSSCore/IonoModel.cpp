//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file IonoModel.cpp
 * Implementation of the ICD-GPS-200 Ionosphere model (20.3.3.5.2.5).
 */

#include <math.h>
#include "GNSSconstants.hpp"
#include "IonoModel.hpp"
#include "YDSTime.hpp"
#include "GNSSconstants.hpp"

namespace gpstk
{
   IonoModel::IonoModel(const double a[4], const double b[4], const bool semicircle_units) throw()
   {
        setModel(a, b, semicircle_units);
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


   void IonoModel::setModel(const double a[4], const double b[4], const bool semicircle_units) throw()
   {
      for (int n = 0; n < 4; n++)
      {
         alpha[n] = a[n];
         beta[n] = b[n];
      }
         // Convert powers of inverse radians to inverse semi-circles, as needed by getCorrection.
         // This is a necessary optional flag for historical GPSTk code reasons.
      if (!semicircle_units)
      {
         alpha[1] *= PI;
         alpha[2] *= PI * PI;
         alpha[3] *= PI * PI * PI;
         beta[1] *= PI;
         beta[2] *= PI * PI;
         beta[3] *= PI * PI * PI;
      }
      valid = true;
   }


   double IonoModel::getCorrection(const CommonTime& time,
                                   const Position& rxgeo,
                                   double svel,
                                   double svaz,
                                   CarrierBand band) const
   {

      if (!valid)
      {
         InvalidIonoModel e("Alpha and beta parameters invalid.");
         GPSTK_THROW(e);
      }

         // All angle units are in semi-circles (radians/PI), per IS-GPS-200.
         // Note: Math functions (cos, sin, etc.) require arguments in radians,
         //       so all semi-circles must be multiplied by PI.

      double azRad = svaz * DEG_TO_RAD;
      double svE = svel / 180.0;

      double phi_u = rxgeo.getGeodeticLatitude() / 180.0;
      double lambda_u = rxgeo.getLongitude() / 180.0;

      double psi = (0.0137 / (svE + 0.11)) - 0.022;

      double phi_i = phi_u + psi * std::cos(azRad);
      if (phi_i > 0.416)
         phi_i = 0.416;
      if (phi_i < -0.416)
         phi_i = -0.416;

      double lambda_i = lambda_u + psi * ::sin(azRad) / ::cos(phi_i*PI);

      double phi_m = phi_i + 0.064 * ::cos((lambda_i - 1.617)*PI);

      double iAMP = 0.0;
      double iPER = 0.0;
      iAMP = alpha[0]+phi_m*(alpha[1]+phi_m*(alpha[2]+phi_m*alpha[3]));
      iPER =  beta[0]+phi_m*( beta[1]+phi_m*( beta[2]+phi_m* beta[3]));

      if (iAMP < 0.0)
         iAMP = 0.0;
      if (iPER < 72000.0)
         iPER = 72000.0;

      double t = 43200.0 * lambda_i + YDSTime(time).sod;
      if (t >= 86400.0)
         t -= 86400.0;
      if (t < 0)
         t += 86400.0;

      double x = TWO_PI * (t - 50400.0) / iPER; // x is in radians

      double iF = 1.0 + 16.0 * (0.53 - svE)*(0.53 - svE)*(0.53 - svE);

      double t_iono = 0.0;
      if (fabs(x) < 1.57)
         t_iono = iF * (5.0e-9 + iAMP * (1 + x*x * (-0.5 + x*x/24.0)));
      else
         t_iono = iF * 5.0e-9;

      // Correction factor for GPS band; see ICD-GPS-200 20.3.3.3.3.2.
      if (band == CarrierBand::L2)
      {
         t_iono *= GAMMA_GPS_12;  //  GAMMA_GPS = (fL1 / fL2)^2
      }
      else if (band == CarrierBand::L5)
      {
         t_iono *= GAMMA_GPS_15;  //  GAMMA_GPS = (fL1 / fL5)^2
      }
      else if (band != CarrierBand::L1)
      {
         throw InvalidIonoModel("Invalid CarrierBand, not one of L1,L2,L5.");
      }

      double correction = t_iono * C_MPS;  // return correction in [m]

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
