#pragma ident "$Id$"


/**
 * @file TropModel.hpp
 * Base class for tropospheric models, plus implementations
 * of several published models
 */

#ifndef TROPOSPHERIC_MODELS_GPSTK
#define TROPOSPHERIC_MODELS_GPSTK

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






#include "Exception.hpp"
#include "ObservationStore.hpp"
#include "Xvt.hpp"
#include "Position.hpp"

// Model of the troposphere, used to compute non-dispersive delay of
// satellite signal as function of satellite elevation as seen at the
// receiver. Both wet and dry components are computed.
//
// The default model (implemented here) is a simple Black model.
//
// In this model (and many others), the wet and dry components are
// independent, the zenith delays depend only on the weather
// (temperature, pressure and humidity), and the mapping functions
// depend only on the elevation of the satellite as seen at the
// receiver. In general, this is not true; other models depend on,
// for example, latitude or day of year.
//
// Other models may be implemented by inheriting this class and
// redefining the virtual functions, and (perhaps) adding other
// 'set...()' routines as needed.

namespace gpstk
{
   /** @addtogroup GPSsolutions */
   //@{

      /** Abstract base class for tropospheric models.
       * The wet and dry components of the tropospheric delay are each the
       * product of a zenith delay and a mapping function. Usually the zenith
       * delay depends only on the weather (temperature, pressure and humidity),
       * while the mapping function depends only on the satellite elevation, i.e.
       * the geometry of satellite and receiver. This may not be true in complex
       * models.
       * The full tropospheric delay is the sum of the wet and dry components.
       * A TropModel is valid only when all the necessary information
       * (weather + whatever else the model requires) is specified;
       * An InvalidTropModel exception will be thrown when any correction()
       * or zenith_delay() or mapping_function() routine is called for
       * an invalid TropModel.
       */
   class TropModel
   {
   public:
         /// Thrown when attempting to use a model for which all necessary
         /// parameters have not been specified.
         /// @ingroup exceptiongroup
      NEW_EXCEPTION_CLASS(InvalidTropModel, gpstk::Exception);

         /// Destructor
      virtual ~TropModel() {}

         /// Return validity of model
      bool isValid(void)
         { return valid; }

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
                                const DayTime& tt)
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
                                const DayTime& tt)
         throw(InvalidTropModel)
      { Position R(RX),S(SV);  return TropModel::correction(R,S,tt); }

         /// Compute and return the zenith delay for dry component of the troposphere
      virtual double dry_zenith_delay(void) const
         throw(InvalidTropModel) = 0;

         /// Compute and return the zenith delay for wet component of the troposphere
      virtual double wet_zenith_delay(void) const
         throw(InvalidTropModel) = 0;

         /// Compute and return the mapping function for dry component of
         /// the troposphere.
         /// @param elevation Elevation of satellite as seen at receiver, in degrees
      virtual double dry_mapping_function(double elevation)
         const throw(InvalidTropModel) = 0;

         /// Compute and return the mapping function for wet component of
         /// the troposphere.
         /// @param elevation Elevation of satellite as seen at receiver, in degrees
      virtual double wet_mapping_function(double elevation)
         const throw(InvalidTropModel) = 0;

         /// Re-define the tropospheric model with explicit weather data.
         /// Typically called just before correction().
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

         /// Define the receiver height; this required by some models before calling
         /// correction() or any of the zenith_delay or mapping_function routines.
         /// @param ht Height of the receiver in meters.
      virtual void setReceiverHeight(const double& ht) {};

         /// Define the latitude of the receiver; this is required by some models
         /// before calling correction() or any of the zenith_delay or
         /// mapping_function routines.
         /// @param lat Latitude of the receiver in degrees.
      virtual void setReceiverLatitude(const double& lat) {};

         /// Define the day of year; this is required by some models before calling
         /// correction() or any of the zenith_delay or mapping_function routines.
         /// @param d Day of year.
      virtual void setDayOfYear(const int& d) {};

   protected:
      bool valid;                 // true only if current model parameters are valid
      double temp;                // latest value of temperature (kelvin or celsius)
      double press;               // latest value of pressure (millibars)
      double humid;               // latest value of relative humidity (percent)

   }; // end class TropModel
   

   //---------------------------------------------------------------------------------
   /// The 'zero' trop model, meaning it always returns zero.
   class ZeroTropModel : public TropModel
   {
   public:
         /// Compute and return the full tropospheric delay
         /// @param elevation Elevation of satellite as seen at receiver, in degrees
      virtual double correction(double elevation) const
         throw(InvalidTropModel)
         { return 0.0; }

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
                                const DayTime& tt)
         throw(InvalidTropModel)
         { return 0.0; }

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
                                const DayTime& tt)
         throw(InvalidTropModel)
         { return 0.0; }

         /// Compute and return the zenith delay for dry component of the troposphere
      virtual double dry_zenith_delay(void) const
         throw(InvalidTropModel)
         { return 0.0; }

         /// Compute and return the zenith delay for wet component of the troposphere
      virtual double wet_zenith_delay(void) const
         throw(InvalidTropModel)
         { return 0.0; }

         /// Compute and return the mapping function for dry component of
         /// the troposphere.
         /// @param elevation Elevation of satellite as seen at receiver, in degrees
      virtual double dry_mapping_function(double elevation)
         const throw(InvalidTropModel)
         { return 0.0; }

         /// Compute and return the mapping function for wet component of
         /// the troposphere.
         /// @param elevation Elevation of satellite as seen at receiver, in degrees
      virtual double wet_mapping_function(double elevation)
         const throw(InvalidTropModel)
         { return 0.0; }

   }; // end class ZeroTropModel
   
   //---------------------------------------------------------------------------------
   /// A simple Black model of the troposphere. temp is in Kelvin.
   class SimpleTropModel : public TropModel
   {
   public:
         /// Empty constructor
      SimpleTropModel(void);

         /// Creates a trop model, with weather observation input
         /// @param wx the weather to use for this correction.
      SimpleTropModel(const WxObservation& wx)
         throw(InvalidParameter);

         /// Create a tropospheric model from explicit weather data
         /// @param T temperature in degrees Celsius
         /// @param P atmospheric pressure in millibars
         /// @param H relative humidity in percent
      SimpleTropModel(const double& T,
                      const double& P,
                      const double& H)
         throw(InvalidParameter);

         /// Compute and return the zenith delay for dry component of the troposphere
      virtual double dry_zenith_delay(void) const
         throw(InvalidTropModel);

         /// Compute and return the zenith delay for wet component of the troposphere
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
         /// Typically called just before correction().
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

   };    // end class SimpleTropModel

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

   };    // end class GGTropModel

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
    *    GGHeightTropModel ggh;
    *    ggh.setWeather(T,P,H);
    *    ggh.setHeights(hT,hP,hH);
    *  and when the correction (and/or delay and map) is computed,
    *  receiver height is set before the call to correction(elevation):
    *    ggh.setReceiverHeight(height);
    *    trop = ggh.correction(elevation);
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
                                const DayTime& tt)
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
                                const DayTime& tt)
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

   };    // end class GGHeightTropModel


   //---------------------------------------------------------------------------------
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
    *   NBTropModel nb;
    *   nb.setReceiverLatitude(lat);
    *   nb.setDayOfYear(doy);
    *   nb.setWeather(T,P,H);       // OPTIONAL
    * Then, when the correction (and/or delay and map) is computed, receiver height
    * should be set before the call to correction(elevation):
    *   nb.setReceiverHeight(height);
    *   trop = nb.correction(elevation);
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
                  const int& day);

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
                  const int& day);

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
                                const DayTime& tt)
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
                                const DayTime& tt)
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

   };    // end class NBTropModel

   //---------------------------------------------------------------------------------
   /** Saastamoinen tropospheric model based on Saastamoinen, J., 'Atmospheric
    * Correction for the Troposphere and Stratosphere in Radio Ranging of
    * Satellites,' Geophysical Monograph 15, American Geophysical Union, 1972,
    * and Ch. 9 of McCarthy, D. and Petit, G., IERS Conventions (2003), IERS
    * Technical Note 32, IERS, 2004. The mapping functions are from
    * Neill, A.E., 1996, 'Global Mapping Functions for the Atmosphere Delay of
    * Radio Wavelengths,' J. Geophys. Res., 101, pp. 3227-3246 (also see IERS TN 32).
    *
    * This model includes a wet and dry component, and requires input of the
    * geodetic latitude, day of year and height above the ellipsoid of the receiver.
    *
    * Usually, the caller will set the latitude and day of year at the same
    * time the weather is set
    *   SaasTropModel stm;
    *   stm.setReceiverLatitude(lat);
    *   stm.setDayOfYear(doy);
    *   stm.setWeather(T,P,H);
    * Then, when the correction (and/or delay and map) is computed, receiver height
    * should be set before the call to correction(elevation):
    *   stm.setReceiverHeight(height);
    *   trop_corr = stm.correction(elevation);
    *
    * NB in this model, units of 'temp' are degrees Celsius and
    * humid actually stores water vapor partial pressure in mbars
    */
   class SaasTropModel : public TropModel
   {
   public:
         /// Empty constructor
      SaasTropModel(void);

         /// Create a trop model using the minimum information: latitude and doy.
         /// @param lat Latitude of the receiver in degrees.
         /// @param day Day of year.
      SaasTropModel(const double& lat,
                    const int& day);

         /// Create a trop model with weather.
         /// @param lat Latitude of the receiver in degrees.
         /// @param day Day of year.
         /// @param wx the weather to use for this correction.
      SaasTropModel(const double& lat,
                    const int& day,
                    const WxObservation& wx)
         throw(InvalidParameter);

         /// Create a tropospheric model from explicit weather data
         /// @param lat Latitude of the receiver in degrees.
         /// @param day Day of year.
         /// @param T temperature in degrees Celsius
         /// @param P atmospheric pressure in millibars
         /// @param H relative humidity in percent
      SaasTropModel(const double& lat,
                    const int& day,
                    const double& T,
                    const double& P,
                    const double& H)
         throw(InvalidParameter);

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
                                const DayTime& tt)
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
         /// the troposphere. NB this function will return infinity at zero elevation.
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
      double height;                /// height (m) of the receiver above the geoid
      double latitude;              /// latitude (deg) of receiver
      int doy;                      /// day of year
      bool validWeather;
      bool validRxLatitude;
      bool validRxHeight;
      bool validDOY;

   };    // end class SaasTropModel

   //@}
   
}

#endif
