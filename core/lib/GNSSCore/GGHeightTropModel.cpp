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


#include "GPSEllipsoid.hpp"
#include "GGHeightTropModel.hpp"

#define THROW_IF_INVALID_DETAILED() {if (!valid) {                      \
         InvalidTropModel e;                                            \
         if(!validWeather) e.addText("Invalid trop model: weather");    \
         if(!validHeights)  e.addText("Invalid trop model: validHeights"); \
         if(!validRxHeight)   e.addText("Invalid trop model: validRxHeight"); \
         GPSTK_THROW(e);}}

namespace gpstk
{
   // ------------------------------------------------------------------------
   // Tropospheric model with heights based on Goad and Goodman(1974),
   // "A Modified Hopfield Tropospheric Refraction Correction Model," Paper
   // presented at the Fall Annual Meeting of the American Geophysical Union,
   // San Francisco, December 1974.
   // (Not the same as GGTropModel because this has height dependence,
   // and the computation of this model does not break cleanly into
   // wet and dry components.)

   GGHeightTropModel::GGHeightTropModel(void)
   {
      validWeather = false; //setWeather(20.0,980.0,50.0);
      validHeights = false; //setHeights(0.0,0.0,0.0);
      validRxHeight = false;
   }

      // Creates a trop model from a weather observation
      // @param wx the weather to use for this correction.
   GGHeightTropModel::GGHeightTropModel(const WxObservation& wx)
      throw(InvalidParameter)
   {
      valid = validRxHeight = validHeights = false;
      setWeather(wx);
   }

      // Create a tropospheric model from explicit weather data
      // @param T temperature in degrees Celsius
      // @param P atmospheric pressure in millibars
      // @param H relative humidity in percent
   GGHeightTropModel::GGHeightTropModel(const double& T,
                                        const double& P,
                                        const double& H)
      throw(InvalidParameter)
   {
      validRxHeight = validHeights = false;
      setWeather(T,P,H);
   }

      // Create a valid model from explicit input.
      // @param T temperature in degrees Celsius
      // @param P atmospheric pressure in millibars
      // @param H relative humidity in percent
      // @param hT height at which temperature applies in meters.
      // @param hP height at which atmospheric pressure applies in meters.
      // @param hH height at which relative humidity applies in meters.
   GGHeightTropModel::GGHeightTropModel(const double& T,
                                        const double& P,
                                        const double& H,
                                        const double hT,
                                        const double hP,
                                        const double hH)
      throw(InvalidParameter)
   {
      validRxHeight = false;
      setWeather(T,P,H);
      setHeights(hT,hP,hH);
   }

      // re-define this to get the throws
   double GGHeightTropModel::correction(double elevation) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID_DETAILED();
      
      if(elevation < 0.0) return 0.0;

      return (dry_zenith_delay() * dry_mapping_function(elevation)
            + wet_zenith_delay() * wet_mapping_function(elevation));

   }

      // Compute and return the full tropospheric delay, given the positions of
      // receiver and satellite and the time tag. This version is most useful
      // within positioning algorithms, where the receiver position and timetag
      // may vary; it computes the elevation (and other receiver location
      // information) and passes them to appropriate set...() routines and
      // the correction(elevation) routine.
      // @param RX  Receiver position
      // @param SV  Satellite position
      // @param tt  Time tag of the signal
   double GGHeightTropModel::correction(const Position& RX,
                                        const Position& SV,
                                        const CommonTime& tt)
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID_DETAILED();

      // compute height from RX
      setReceiverHeight(RX.getHeight());

      return TropModel::correction(RX.elevation(SV));

   }  // end GGHeightTropModel::correction(RX,SV,TT)

      // Compute and return the full tropospheric delay, given the positions of
      // receiver and satellite and the time tag. This version is most useful
      // within positioning algorithms, where the receiver position and timetag
      // may vary; it computes the elevation (and other receiver location
      // information) and passes them to appropriate set...() routines and
      // the correction(elevation) routine.
      // @param RX  Receiver position in ECEF cartesian coordinates (meters)
      // @param SV  Satellite position in ECEF cartesian coordinates (meters)
      // @param tt  Time tag of the signal
      // This function is deprecated; use the Position version
   double GGHeightTropModel::correction(const Xvt& RX,
                                        const Xvt& SV,
                                        const CommonTime& tt)
      throw(InvalidTropModel)
   {
      Position R(RX),S(SV);
      return GGHeightTropModel::correction(R,S,tt);
   }

      // Compute and return the zenith delay for dry component of the troposphere
   double GGHeightTropModel::dry_zenith_delay(void) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID_DETAILED();
      double hrate=6.5e-3;
      double Ts=temp+hrate*height;
      double em=978.77/(2.8704e4*hrate);
      double Tp=Ts-hrate*hpress;
      double ps=press*std::pow(Ts/Tp,em)/1000.0;
      double rs=77.624e-3/Ts;
      double ho=11.385/rs;
      rs *= ps;
      double zen=(ho-height)/ho;
      zen = rs*zen*zen*zen*zen;
         // normalize
      zen *= (ho-height)/5;
      return zen;

   }

      // Compute and return the zenith delay for wet component of the troposphere
   double GGHeightTropModel::wet_zenith_delay(void) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID_DETAILED();
      
      double hrate=6.5e-3; //   deg K / m
      double Th=temp-273.15-hrate*(hhumid-htemp);
      double Ta=7.5*Th/(237.3+Th);
         // water vapor partial pressure
      double e0=6.11e-5*humid*std::pow(10.0,Ta);
      double Ts=temp+hrate*htemp;
      double em=978.77/(2.8704e4*hrate);
      double Tk=Ts-hrate*hhumid;
      double es=e0*std::pow(Ts/Tk,4.0*em);
      double rs=(371900.0e-3/Ts-12.92e-3)/Ts;
      double ho=11.385*(1255/Ts+0.05)/rs;
      double zen=(ho-height)/ho;
      zen = rs*es*zen*zen*zen*zen;
         //normalize
      zen *= (ho-height)/5;
      return zen;

   }

      // Compute and return the mapping function for dry component
      // of the troposphere
      // @param elevation Elevation of satellite as seen at receiver,
      //                  in degrees
   double GGHeightTropModel::dry_mapping_function(double elevation) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID_DETAILED();
      
      if(elevation < 0.0) return 0.0;

      double hrate=6.5e-3;
      double Ts=temp+hrate*htemp;
      double ho=(11.385/77.624e-3)*Ts;
      double se=std::sin(elevation*DEG_TO_RAD);
      if(se < 0.0) se=0.0;

      GPSEllipsoid ell;
      double rt,a,b,rn[8],al[8],er=ell.a();
      rt = (er+ho)/(er+height);
      rt = rt*rt - (1.0-se*se);
      if(rt < 0) rt=0.0;
      rt = (er+height)*(SQRT(rt)-se);
      a = -se/(ho-height);
      b = -(1.0-se*se)/(2.0*er*(ho-height));
      rn[0] = rt*rt;
      for(int j=1; j<8; j++) rn[j]=rn[j-1]*rt;
      al[0] = 2*a;
      al[1] = 2*a*a+4*b/3;
      al[2] = a*(a*a+3*b);
      al[3] = a*a*a*a/5+2.4*a*a*b+1.2*b*b;
      al[4] = 2*a*b*(a*a+3*b)/3;
      al[5] = b*b*(6*a*a+4*b)*0.1428571;
      if(b*b > 1.0e-35) {
         al[6] = a*b*b*b/2;
         al[7] = b*b*b*b/9;
      } else {
         al[6] = 0.0;
         al[7] = 0.0;
      }
      double map=rt;
      for(int k=0; k<8; k++) map += al[k]*rn[k];
         // normalize
      double norm=(ho-height)/5;
      return map/norm;
   }

      // Compute and return the mapping function for wet component
      // of the troposphere
      // @param elevation Elevation of satellite as seen at receiver,
      //                  in degrees
   double GGHeightTropModel::wet_mapping_function(double elevation) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID_DETAILED();
      if(elevation < 0.0) return 0.0;

      double hrate=6.5e-3;
      double Ts=temp+hrate*htemp;
      double rs=(371900.0e-3/Ts-12.92e-3)/Ts;
      double ho=11.385*(1255/Ts+0.05)/rs;
      double se=std::sin(elevation*DEG_TO_RAD);
      if(se < 0.0) se=0.0;

      GPSEllipsoid ell;
      double rt,a,b,rn[8],al[8],er=ell.a();
      rt = (er+ho)/(er+height);
      rt = rt*rt - (1.0-se*se);
      if(rt < 0) rt=0.0;
      rt = (er+height)*(SQRT(rt)-se);
      a = -se/(ho-height);
      b = -(1.0-se*se)/(2.0*er*(ho-height));
      rn[0] = rt*rt;
      for(int i=1; i<8; i++) rn[i]=rn[i-1]*rt;
      al[0] = 2*a;
      al[1] = 2*a*a+4*b/3;
      al[2] = a*(a*a+3*b);
      al[3] = a*a*a*a/5+2.4*a*a*b+1.2*b*b;
      al[4] = 2*a*b*(a*a+3*b)/3;
      al[5] = b*b*(6*a*a+4*b)*0.1428571;
      if(b*b > 1.0e-35) {
         al[6] = a*b*b*b/2;
         al[7] = b*b*b*b/9;
      } else {
         al[6] = 0.0;
         al[7] = 0.0;
      }
      double map=rt;
      for(int j=0; j<8; j++) map += al[j]*rn[j];
         // normalize map function
      double norm=(ho-height)/5;
      return map/norm;

   }

      // Re-define the weather data.
      // Typically called just before correction().
      // @param T temperature in degrees Celsius
      // @param P atmospheric pressure in millibars
      // @param H relative humidity in percent
   void GGHeightTropModel::setWeather(const double& T,
                                      const double& P,
                                      const double& H)
      throw(InvalidParameter)
   {
      try
      {
         TropModel::setWeather(T,P,H);
         validWeather = true;
         valid = validWeather && validHeights && validRxHeight;
      }
      catch(InvalidParameter& e)
      {
         valid = validWeather = false;
         GPSTK_RETHROW(e);
      }
   }

      // Re-define the tropospheric model with explicit weather data.
      // Typically called just before correction().
      // @param wx the weather to use for this correction
   void GGHeightTropModel::setWeather(const WxObservation& wx)
      throw(InvalidParameter)
   {
      try
      {
         TropModel::setWeather(wx);
         validWeather = true;
         valid = validWeather && validHeights && validRxHeight;
      }
      catch(InvalidParameter& e)
      {
         valid = validWeather = false;
         GPSTK_RETHROW(e);
      }
   }


      // Re-define the heights at which the weather parameters apply.
      // Typically called just before correction().
      // @param hT height (m) at which temperature applies
      // @param hP height (m) at which atmospheric pressure applies
      // @param hH height (m) at which relative humidity applies
   void GGHeightTropModel::setHeights(const double& hT,
                                      const double& hP,
                                      const double& hH)
   {
      htemp = hT;                 // height (m) at which temp applies
      hpress = hP;                // height (m) at which press applies
      hhumid = hH;                // height (m) at which humid applies
      validHeights = true;
      valid = validWeather && validHeights && validRxHeight;
   }

      // Define the receiver height; this required before calling
      // correction() or any of the zenith_delay or mapping_function routines.
   void GGHeightTropModel::setReceiverHeight(const double& ht)
   {
      height = ht;
      validRxHeight = true;
      if(!validHeights) {
         htemp = hpress = hhumid = ht;
         validHeights = true;
      }
      valid = validWeather && validHeights && validRxHeight;
   }

}
