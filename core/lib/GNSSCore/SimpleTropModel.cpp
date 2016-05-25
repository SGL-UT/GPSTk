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

#include "SimpleTropModel.hpp"
#include "GPSEllipsoid.hpp"

namespace gpstk
{
   // -----------------------------------------------------------------------
   // Simple Black model. This has been used as the 'default' for many years.

      // Default constructor
   SimpleTropModel::SimpleTropModel(void)
   {
      setWeather(20.0, 980.0, 50.0);
      Cwetdelay = 0.122382715318184;
      Cdrydelay = 2.235486646978727;
      Cwetmap = 1.000282213715744;
      Cdrymap = 1.001012704615527;
      valid = true;
   }

      // Creates a trop model from a weather observation
      // @param wx the weather to use for this correction.
   SimpleTropModel::SimpleTropModel(const WxObservation& wx)
      throw(InvalidParameter)
   {
      setWeather(wx);
      valid = true;
   }

      // Create a tropospheric model from explicit weather data
      // @param T temperature in degrees Celsius
      // @param P atmospheric pressure in millibars
      // @param H relative humidity in percent
   SimpleTropModel::SimpleTropModel(const double& T,
                                    const double& P,
                                    const double& H)
      throw(InvalidParameter)
   {
      setWeather(T,P,H);
      valid = true;
   }

      // Re-define the tropospheric model with explicit weather data.
      // Typically called just before correction().
      // @param T temperature in degrees Celsius
      // @param P atmospheric pressure in millibars
      // @param H relative humidity in percent
   void SimpleTropModel::setWeather(const double& T,
                                    const double& P,
                                    const double& H)
      throw(InvalidParameter)
   {
      TropModel::setWeather(T,P,H);
      GPSEllipsoid ell;
      Cdrydelay = 2.343*(press/1013.25)*(temp-3.96)/temp;
      double tks = temp * temp;
      Cwetdelay = 8.952/tks*humid*std::exp(-37.2465+0.213166*temp-(0.256908e-3)*tks);
      Cdrymap =1.0+(0.15)*148.98*(temp-3.96)/ell.a();
      Cwetmap =1.0+(0.15)*12000.0/ell.a();
      valid = true;
   }

      // Re-define the tropospheric model with explicit weather data.
      // Typically called just before correction().
      // @param wx the weather to use for this correction
   void SimpleTropModel::setWeather(const WxObservation& wx)
      throw(InvalidParameter)
   {
      TropModel::setWeather(wx);
   }

      // Compute and return the zenith delay for dry component of the troposphere
   double SimpleTropModel::dry_zenith_delay(void) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();
      return Cdrydelay;
   }

      // Compute and return the zenith delay for wet component of the troposphere
   double SimpleTropModel::wet_zenith_delay(void) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();
      return Cwetdelay;
   }

      // Compute and return the mapping function for dry component
      // of the troposphere
      // @param elevation is the Elevation of satellite as seen at receiver,
      //                  in degrees
   double SimpleTropModel::dry_mapping_function(double elevation) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();

      if(elevation < 0.0) return 0.0;

      double d = std::cos(elevation*DEG_TO_RAD);
      d /= Cdrymap;
      return (1.0/SQRT(1.0-d*d));
   }

      // Compute and return the mapping function for wet component
      // of the troposphere
      // @param elevation is the Elevation of satellite as seen at receiver,
      //                  in degrees
   double SimpleTropModel::wet_mapping_function(double elevation) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();

      if(elevation < 0.0) return 0.0;

      double d = std::cos(elevation*DEG_TO_RAD);
      d /= Cwetmap;
      return (1.0/SQRT(1.0-d*d));
   }

}
