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


#ifndef GG_HEIGHT_TROP_MODEL_HPP
#define GG_HEIGHT_TROP_MODEL_HPP

#include "TropModel.hpp"

namespace gpstk
{
   //---------------------------------------------------------------------------------
   /** Tropospheric model with heights based on Goad and Goodman(1974),
    *  "A Modified Hopfield Tropospheric Refraction Correction Model," Paper
    *  presented at the Fall Annual Meeting of the American Geophysical Union,
    *  San Francisco, December 1974.
    *
    *  (Not the same as GGTropModel because this has height dependence, and the
    *  computation of this model does not break cleanly into wet and dry components.)
    *
    *  NB this model requires heights, both of the weather parameters,
    *    and of the receiver.
    *  Thus, usually, caller will set heights at the same time the weather is set:
    *
    * @code
    *    GGHeightTropModel ggh;
    *    ggh.setWeather(T,P,H);
    *    ggh.setHeights(hT,hP,hH);
    * @endcode
    *
    *  and when the correction (and/or delay and map) is computed,
    *  receiver height is set before the call to correction(elevation):
    *
    * @code
    *    ggh.setReceiverHeight(height);
    *    trop = ggh.correction(elevation);
    * @endcode
    *
    *  NB setReceiverHeight(ht) sets the 'weather heights' as well, if they are not
    *    already defined.
    */
   class GGHeightTropModel : public TropModel
   {
   public:
         /// Empty constructor
      GGHeightTropModel(void);

         /// Creates a trop model, with weather observation input
         /// @param wx the weather to use for this correction.
      GGHeightTropModel(const WxObservation& wx)
         throw(InvalidParameter);

         /// Create a tropospheric model from explicit weather data
         /// @param T temperature in degrees Celsius
         /// @param P atmospheric pressure in millibars
         /// @param H relative humidity in percent
      GGHeightTropModel(const double& T,
                        const double& P,
                        const double& H)
         throw(InvalidParameter);

         /// Create a valid model from explicit input.
         /// @param T temperature in degrees Celsius
         /// @param P atmospheric pressure in millibars
         /// @param H relative humidity in percent
         /// @param hT height at which temperature applies in meters.
         /// @param hP height at which atmospheric pressure applies in meters.
         /// @param hH height at which relative humidity applies in meters.
      GGHeightTropModel(const double& T,
                        const double& P,
                        const double& H,
                        const double hT,
                        const double hP,
                        const double hH)
         throw(InvalidParameter);

         /// Return the name of the model
      virtual std::string name(void)
         { return std::string("GGHeight"); }

         /// Compute and return the full tropospheric delay
         /// @param elevation Elevation of satellite as seen at receiver, in degrees
      virtual double correction(double elevation) const
         throw(InvalidTropModel);

         /**
          * Compute and return the full tropospheric delay, given the positions of
          * receiver and satellite and the time tag. This version is most useful
          * within positioning algorithms, where the receiver position and timetag
          * may vary; it computes the elevation (and other receiver location
          * information) and passes them to appropriate set...() routines and the
          * correction(elevation) routine.
          * @param RX  Receiver position
          * @param SV  Satellite position
          * @param tt  Time tag of the signal
          */
      virtual double correction(const Position& RX,
                                const Position& SV,
                                const CommonTime& tt)
         throw(InvalidTropModel);

         /** \deprecated
          * Compute and return the full tropospheric delay, given the positions of
          * receiver and satellite and the time tag. This version is most useful
          * within positioning algorithms, where the receiver position and timetag
          * may vary; it computes the elevation (and other receiver location
          * information) and passes them to appropriate set...() routines and the
          * correction(elevation) routine.
          * @param RX  Receiver position in ECEF cartesian coordinates (meters)
          * @param SV  Satellite position in ECEF cartesian coordinates (meters)
          * @param tt  Time tag of the signal
          */
      virtual double correction(const Xvt& RX,
                                const Xvt& SV,
                                const CommonTime& tt)
         throw(InvalidTropModel);

         /// Compute and return the zenith delay for dry component
         /// of the troposphere
      virtual double dry_zenith_delay(void) const
         throw(InvalidTropModel);

         /// Compute and return the zenith delay for wet component of the troposphere
      virtual double wet_zenith_delay(void) const
         throw(InvalidTropModel);

         /// Compute and return the mapping function for dry component of
         /// the troposphere.
         /// @param elevation Elevation of satellite as seen at receiver, in degrees
      virtual double dry_mapping_function(double elevation) const
         throw(InvalidTropModel);

         /// Compute and return the mapping function for wet component of
         /// the troposphere.
         /// @param elevation Elevation of satellite as seen at receiver, in degrees
      virtual double wet_mapping_function(double elevation) const
         throw(InvalidTropModel);

         /// Re-define the weather data.
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

         /// Re-define the heights at which the weather parameters apply.
         /// Typically called whenever setWeather is called.
         /// @param hT height at which temperature applies in meters.
         /// @param hP height at which atmospheric pressure applies in meters.
         /// @param hH height at which relative humidity applies in meters.
      void setHeights(const double& hT,
                      const double& hP,
                      const double& hH);

         /// Define the receiver height; this required before calling
         /// correction() or any of the zenith_delay or mapping_function routines.
      void setReceiverHeight(const double& ht);

   private:
      double height;                // height (m) of the receiver
      double htemp;                 // height (m) at which temp applies
      double hpress;                // height (m) at which press applies
      double hhumid;                // height (m) at which humid applies
      bool validWeather;
      bool validHeights;
      bool validRxHeight;
   };
}

#endif
