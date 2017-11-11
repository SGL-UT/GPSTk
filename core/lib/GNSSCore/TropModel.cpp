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
 * @file TropModel.cpp
 * Base class for tropospheric models, plus implementations of several
 * published models
 */

#include "TropModel.hpp"
#include "EphemerisRange.hpp"         // for Elevation()
#include "MathBase.hpp"               // SQRT
#include "GNSSconstants.hpp"          // DEG_TO_RAD
#include "GPSEllipsoid.hpp"           // ell.a() = R earth
#include "GNSSconstants.hpp"          // TWO_PI
#include "YDSTime.hpp"


namespace gpstk
{
      // for temperature conversion from Celcius to Kelvin
   const double TropModel::CELSIUS_TO_KELVIN = 273.15;

      // Compute and return the full tropospheric delay. Typically call
      // setWeather(T,P,H) before making this call.
      // @param elevation Elevation of satellite as seen at receiver, in degrees
   double TropModel::correction(double elevation) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();

      if(elevation < 0.0)
         return 0.0;

      return (dry_zenith_delay() * dry_mapping_function(elevation)
            + wet_zenith_delay() * wet_mapping_function(elevation));

   }  // end TropModel::correction(elevation)

      // Compute and return the full tropospheric delay, given the positions of
      // receiver and satellite and the time tag. This version is most useful
      // within positioning algorithms, where the receiver position and timetag may
      // vary; it computes the elevation (and other receiver location information)
      // and passes them to appropriate set...() routines and the
      // correction(elevation) routine.
      // @param RX  Receiver position in ECEF cartesian coordinates (meters)
      // @param SV  Satellite position in ECEF cartesian coordinates (meters)
      // @param tt  Time tag of the signal
   double TropModel::correction(const Position& RX,
                                const Position& SV,
                                const CommonTime& tt)
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();

      double c;
      try
      {
         c = correction(RX.elevation(SV));
      }
      catch(InvalidTropModel& e)
      {
         GPSTK_RETHROW(e);
      }
      return c;
   }  // end TropModel::correction(RX,SV,TT)

      // Re-define the tropospheric model with explicit weather data.
      // Typically called just before correction().
      // @param T temperature in degrees Celsius
      // @param P atmospheric pressure in millibars
      // @param H relative humidity in percent
   void TropModel::setWeather(const double& T,
                              const double& P,
                              const double& H)
      throw(InvalidParameter)
   {
      temp = T + CELSIUS_TO_KELVIN;
      press = P;
      humid = H;
      if (temp < 0.0)
      {
         valid = false;
         InvalidParameter e("Invalid temperature.");
         GPSTK_THROW(e);
      }
      if (press < 0.0)
      {
         valid = false;
         InvalidParameter e("Invalid pressure.");
         GPSTK_THROW(e);
      }
      if (humid < 0.0 || humid > 105.0)
      {
         valid = false;
         InvalidParameter e("Invalid humidity.");
         GPSTK_THROW(e);
      }
         // truncates humidity to 100 if between 105 and 100.
         // models cannot handle supersaturation.
      if (humid > 100)
      {
         humid = 100.0;
      }
   }  // end TropModel::setWeather(T,P,H)

      // Re-define the tropospheric model with explicit weather data.
      // Typically called just before correction().
      // @param wx the weather to use for this correction
   void TropModel::setWeather(const WxObservation& wx)
      throw(InvalidParameter)
   {
      if (wx.isAllValid())
      {
         try
         {
            setWeather(wx.temperature, wx.pressure, wx.humidity);
            valid = true;
         }
         catch(InvalidParameter& e)
         {
            valid = false;
            GPSTK_RETHROW(e);
         }
      }
      else
      {
         valid = false;
         InvalidParameter e("Invalid weather data.");
         GPSTK_THROW(e);
      }
   }
      /// get weather data by a standard atmosphere model
      /// reference to white paper of Bernese 5.0, P243
      /// @param ht     Height of the receiver in meters.
      /// @param T      temperature in degrees Celsius
      /// @param P      atmospheric pressure in millibars
      /// @param H      relative humidity in percent
   void TropModel::weatherByStandardAtmosphereModel(const double& ht, double& T, double& P, double& H)
   {
         // reference height and it's relate weather(T P H)
      const double h0  = 0.0;			   // meter
      const double Tr  = +18.0;	      // Celsius
      const double pr  = 1013.25;		// millibarc
      const double Hr  = 50;			   // humidity

      T = Tr-0.0065*(ht-h0);
      P = pr * std::pow((1 - 0.0000226 * (ht - h0)), 5.225);
      H = Hr * std::exp(-0.0006396 * (ht - h0));

   }

}
