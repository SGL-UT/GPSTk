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


#ifndef NB_TROP_MODEL_HPP
#define NB_TROP_MODEL_HPP

#include "TropModel.hpp"

namespace gpstk
{
   /** Tropospheric model developed by University of New Brunswick and described in
    * "A Tropospheric Delay Model for the User of the Wide Area Augmentation
    * System," J. Paul Collins and Richard B. Langley, Technical Report No. 187,
    * Dept. of Geodesy and Geomatics Engineering, University of New Brunswick,
    * 1997. See particularly Appendix C.
    *
    * This model includes a wet and dry component, and was designed for the user
    * without access to measurements of temperature, pressure and relative humidity
    * at ground level. Input of the receiver latitude, day of year and height
    * above the ellipsoid are required, because the mapping functions depend on
    * these quantities. In addition, if the weather (T,P,H) are not explicitly
    * provided, this model interpolates a table of values, using latitude and day
    * of year, to get the ground level weather parameters.
    *
    * Usually, the caller will set the latitude and day of year at the same
    * time the weather is set (if weather is available):
    *
    * @code
    *   NBTropModel nb;
    *   nb.setReceiverLatitude(lat);
    *   nb.setDayOfYear(doy);
    *   nb.setWeather(T,P,H);       // OPTIONAL
    * @endcode
    *
    * Then, when the correction (and/or delay and map) is computed, receiver height
    * should be set before the call to correction(elevation):
    *
    * @code
    *   nb.setReceiverHeight(height);
    *   trop = nb.correction(elevation);
    * @endcode
    *
    * NB in this model, units of 'temp' are degrees Kelvin, and 'humid'
    * is the water vapor partial pressure.
    */
   class NBTropModel : public TropModel
   {
   public:
         /// Empty constructor
      NBTropModel(void);

         /// Create a trop model using the minimum information: latitude and doy.
         /// Interpolate the weather unless setWeather (optional) is called.
         /// @param lat Latitude of the receiver in degrees.
         /// @param day Day of year.
      NBTropModel(const double& lat,
                  const int& day)
         throw(InvalidParameter);

         /// Create a trop model with weather.
         /// @param lat Latitude of the receiver in degrees.
         /// @param day Day of year.
         /// @param wx the weather to use for this correction.
      NBTropModel(const double& lat,
                  const int& day,
                  const WxObservation& wx)
         throw(InvalidParameter);

         /// Create a tropospheric model from explicit weather data
         /// @param lat Latitude of the receiver in degrees.
         /// @param day Day of year.
         /// @param T temperature in degrees Celsius
         /// @param P atmospheric pressure in millibars
         /// @param H relative humidity in percent
      NBTropModel(const double& lat,
                  const int& day,
                  const double& T,
                  const double& P,
                  const double& H)
         throw(InvalidParameter);

         /// Create a valid model from explicit input
         /// (weather will be estimated internally by this model).
         /// @param ht Height of the receiver in meters.
         /// @param lat Latitude of the receiver in degrees.
         /// @param day Day of year.
      NBTropModel(const double& ht,
                  const double& lat,
                  const int& day)
         throw(InvalidParameter);

      /// Return the name of the model
      virtual std::string name(void)
         { return std::string("NB"); }

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

         /// Compute and return the zenith delay for wet component
         /// of the troposphere
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

         /// Re-define the tropospheric model with explicit weather data.
         /// Typically called just before correction().
         /// @param wx the weather to use for this correction
      virtual void setWeather(const WxObservation& wx)
         throw(InvalidParameter);

         /// Define the weather data; typically called just before correction().
         /// @param T temperature in degrees Celsius
         /// @param P atmospheric pressure in millibars
         /// @param H relative humidity in percent
      virtual void setWeather(const double& T,
                              const double& P,
                              const double& H)
         throw(InvalidParameter);

         /// configure the model to estimate the weather using lat and doy
      void setWeather()
         throw(InvalidTropModel);

         /// Define the receiver height; this required before calling
         /// correction() or any of the zenith_delay or mapping_function routines.
         /// @param ht Height of the receiver in meters.
      void setReceiverHeight(const double& ht);

         /// Define the latitude of the receiver; this is required before calling
         /// correction() or any of the zenith_delay or mapping_function routines.
         /// @param lat Latitude of the receiver in degrees.
      void setReceiverLatitude(const double& lat);

         /// Define the day of year; this is required before calling
         /// correction() or any of the zenith_delay or mapping_function routines.
         /// @param d Day of year.
      void setDayOfYear(const int& d);

   private:
      bool interpolateWeather;      // if true, compute T,P,H from latitude,doy
      double height;                // height (m) of the receiver
      double latitude;              // latitude (deg) of receiver
      int doy;                      // day of year
      bool validWeather;
      bool validRxLatitude;
      bool validRxHeight;
      bool validDOY;
   };
   
}
#endif
