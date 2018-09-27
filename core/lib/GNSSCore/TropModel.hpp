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

/**
 * @file TropModel.hpp
 * Base class for tropospheric models, plus implementations
 * of several published models
 */

#ifndef TROP_MODEL_HPP
#define TROP_MODEL_HPP

#include "Exception.hpp"
#include "ObsEpochMap.hpp"
#include "WxObsMap.hpp"
#include "Xvt.hpp"
#include "Position.hpp"
#include "Matrix.hpp"
#include "GNSSconstants.hpp"

#define THROW_IF_INVALID() {if (!valid) {InvalidTropModel e("Invalid model");GPSTK_THROW(e);}}

// Model of the troposphere, used to compute non-dispersive delay of
// satellite signal as function of satellite elevation as seen at the
// receiver. Both wet and hydrostatic (dry) components are computed.
//
// The default model (implemented here) is a simple Black model.
//
// In this model (and many others), the wet and hydrostatic (dry) components are
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
       * The wet and hydrostatic (dry) components of the tropospheric delay are each
       * the product of a zenith delay and a mapping function. Usually the zenith
       * delay depends only on the weather (temperature, pressure and humidity),
       * while the mapping function depends only on the satellite elevation, i.e.
       * the geometry of satellite and receiver. This may not be true in complex
       * models.
       * The full tropospheric delay is the sum of the wet and hydrostatic (dry)
       * components. A TropModel is valid only when all the necessary information
       * (weather + whatever else the model requires) is specified;
       * An InvalidTropModel exception will be thrown when any correction()
       * or zenith_delay() or mapping_function() routine is called for
       * an invalid TropModel.
       */

   /// Thrown when attempting to use a model for which all necessary
   /// parameters have not been specified.
   /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(InvalidTropModel, gpstk::Exception);

   class TropModel
   {
   public:
      static const double CELSIUS_TO_KELVIN;

         /// Destructor
      virtual ~TropModel() {}

         /// Return validity of model
      bool isValid(void)
         { return valid; }

         /// Return the name of the model
      virtual std::string name(void)
         { return std::string("Undefined"); }

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
         throw(InvalidTropModel)
      { Position R(RX),S(SV);  return TropModel::correction(R,S,tt); }

         /// Compute and return the zenith delay for hydrostatic (dry)
         /// component of the troposphere
      virtual double dry_zenith_delay(void) const
         throw(InvalidTropModel) = 0;

         /// Compute and return the zenith delay for wet component of the troposphere
      virtual double wet_zenith_delay(void) const
         throw(InvalidTropModel) = 0;

         /// Compute and return the mapping function for hydrostatic (dry)
         /// component of the troposphere.
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

         /// Define the receiver longitude; this is required by some models
         /// before calling correction() or any of the zenith_delay routines.
         /// @param lat  Longitude of receiver, in degrees East.
      virtual void setReceiverLongitude(const double& lon) {};

         /// Define the day of year; this is required by some models before calling
         /// correction() or any of the zenith_delay or mapping_function routines.
         /// @param d Day of year.
      virtual void setDayOfYear(const int& d) {};

         /// Saastamoinen hydrostatic zenith delay as modified by Davis for gravity.
         /// Used by multiple models.
         /// Ref. Leick, 3rd ed, pg 197, Leick, 4th ed, pg 482, and
         /// Saastamoinen 1973 Atmospheric correction for the troposphere and
         /// stratosphere in radio ranging of satellites. The use of artificial
         /// satellites for geodesy, Geophys. Monogr. Ser. 15, Amer. Geophys. Union,
         /// pp. 274-251, 1972.
         /// Davis, J.L, T.A. Herring, I.I. Shapiro, A.E.E. Rogers, and G. Elgered,
         /// Geodesy by Radio Interferometry: Effects of Atmospheric Modeling Errors
         /// on Estimates of Baseline Length, Radio Science, Vol. 20, No. 6,
         /// pp. 1593-1607, 1985.
         /// @param press pressure in millibars
         /// @param lat  latitude in degrees
         /// @param height ellipsoid height in meters
      double SaasDryDelay(const double pr, const double lat, const double ht) const
      {
         return (0.0022768*pr/(1-0.00266*::cos(2*lat*DEG_TO_RAD)-0.00028*ht/1000.));
      }

         /// get weather data by a standard atmosphere model
         /// reference to white paper of Bernese 5.0, P243
         /// @param ht     height of the receiver in meters.
         /// @param T      temperature in degrees Celsius
         /// @param P      atmospheric pressure in millibars
         /// @param H      relative humidity in percent
      static void weatherByStandardAtmosphereModel(
         const double& ht, double& T, double& P, double& H);

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
         /// Return the name of the model
      virtual std::string name(void)
         { return std::string("Zero"); }

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
                                const CommonTime& tt)
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
                                const CommonTime& tt)
         throw(InvalidTropModel)
         { return 0.0; }

         /// Compute and return the zenith delay for hydrostatic (dry)
         /// component of the troposphere
      virtual double dry_zenith_delay(void) const
         throw(InvalidTropModel)
         { return 0.0; }

         /// Compute and return the zenith delay for wet component of the troposphere
      virtual double wet_zenith_delay(void) const
         throw(InvalidTropModel)
         { return 0.0; }

         /// Compute and return the mapping function for hydrostatic (dry)
         /// component of the troposphere.
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

      //@}
}
#endif
