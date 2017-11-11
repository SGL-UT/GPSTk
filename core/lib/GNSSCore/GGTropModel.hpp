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


#ifndef GG_TROP_MODEL_HPP
#define GG_TROP_MODEL_HPP

#include "TropModel.hpp"

namespace gpstk
{
   //---------------------------------------------------------------------------------
   /** Tropospheric model based on Goad and Goodman(1974),
    *  "A Modified Hopfield Tropospheric Refraction Correction Model," Paper
    * presented at the Fall Annual Meeting of the American Geophysical Union,
    * San Francisco, December 1974, as presented in Leick, "GPS Satellite Surveying,"
    * Wiley, NY, 1990, Chapter 9 (note particularly Table 9.1).
    */
   class GGTropModel : public TropModel
   {
   public:
         /// Empty constructor
      GGTropModel(void);

         /// Creates a trop model, with weather observation input
         /// @param wx the weather to use for this correction.
      GGTropModel(const WxObservation& wx)
         throw(InvalidParameter);

         /// Create a tropospheric model from explicit weather data
         /// @param T temperature in degrees Celsius
         /// @param P atmospheric pressure in millibars
         /// @param H relative humidity in percent
      GGTropModel(const double& T,
                  const double& P,
                  const double& H)
         throw(InvalidParameter);

         /// Return the name of the model
      virtual std::string name(void)
         { return std::string("GG"); }

         /// Compute and return the zenith delay for dry component
         /// of the troposphere
      virtual double dry_zenith_delay(void) const
         throw(InvalidTropModel);

         /// Compute and return the zenith delay for wet component
         /// of the troposphere
      virtual double wet_zenith_delay(void) const
         throw(InvalidTropModel);

         /// Compute and return the mapping function for dry component
         /// of the troposphere
         /// @param elevation Elevation of satellite as seen at receiver, in degrees
      virtual double dry_mapping_function(double elevation) const
         throw(InvalidTropModel);

         /// Compute and return the mapping function for wet component
         /// of the troposphere
         /// @param elevation Elevation of satellite as seen at receiver, in degrees
      virtual double wet_mapping_function(double elevation) const
         throw(InvalidTropModel);

         /// Re-define the tropospheric model with explicit weather data.
         /// Typically called initially, and whenever the weather changes.
         /// @param T temperature in degrees Celsius
         /// @param P atmospheric pressure in millibars
         /// @param H relative humidity in percent
      virtual void setWeather(const double& T,
                              const double& P,
                              const double& H)
         throw(InvalidParameter);

         /// Re-define the tropospheric model with explicit weather data.
         /// Typically called just before correction().
         /// @param wx the weather to use for this correction
      virtual void setWeather(const WxObservation& wx)
         throw(InvalidParameter);

   private:
      double Cdrydelay;
      double Cwetdelay;
      double Cdrymap;
      double Cwetmap;

   };
}
#endif
