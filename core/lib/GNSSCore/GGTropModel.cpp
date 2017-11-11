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
#include "GGTropModel.hpp"

#define THROW_IF_INVALID_DETAILED() {if (!valid) {                   \
         InvalidTropModel e;                                            \
         if(!validWeather) e.addText("Invalid trop model: weather");    \
         if(!validHeights)  e.addText("Invalid trop model: validHeights"); \
         if(!validRxHeight)   e.addText("Invalid trop model: validRxHeight"); \
         GPSTK_THROW(e);}}

namespace gpstk
{
   // ------------------------------------------------------------------------
   // Tropospheric model based on Goad and Goodman(1974),
   // "A Modified Hopfield Tropospheric Refraction Correction Model," Paper
   // presented at the Fall Annual Meeting of the American Geophysical Union,
   // San Francisco, December 1974.
   // See Leick, "GPS Satellite Surveying," Wiley, NY, 1990, Chapter 9,
   // particularly Table 9.1.
   // ------------------------------------------------------------------------

   static const double GGdryscale = 8594.777388436570600;
   static const double GGwetscale = 2540.042008403690900;

      // Default constructor
   GGTropModel::GGTropModel(void)
   {
      TropModel::setWeather(20.0, 980.0, 50.0);
      Cdrydelay = 2.59629761092150147e-4;    // zenith delay, dry
      Cwetdelay = 4.9982784999977412e-5;     // zenith delay, wet
      Cdrymap = 42973.886942182834900;       // height for mapping, dry
      Cwetmap = 12700.210042018454260;       // height for mapping, wet
      valid = true;
   }  // end GGTropModel::GGTropModel()

      // Creates a trop model from a weather observation
      // @param wx the weather to use for this correction.
   GGTropModel::GGTropModel(const WxObservation& wx)
      throw(InvalidParameter)
   {
      setWeather(wx);
      valid = true;
   }

      // Create a tropospheric model from explicit weather data
      // @param T temperature in degrees Celsius
      // @param P atmospheric pressure in millibars
      // @param H relative humidity in percent
   GGTropModel::GGTropModel(const double& T,
                            const double& P,
                            const double& H)
      throw(InvalidParameter)
   {
      setWeather(T,P,H);
      valid = true;
   }

   double GGTropModel::dry_zenith_delay(void) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();
      return (Cdrydelay * GGdryscale);
   }

   double GGTropModel::wet_zenith_delay(void) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();
      return (Cwetdelay * GGwetscale);
   }

   double GGTropModel::dry_mapping_function(double elevation) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();

      if(elevation < 0.0) return 0.0;

      GPSEllipsoid ell;
      double ce=std::cos(elevation*DEG_TO_RAD), se=std::sin(elevation*DEG_TO_RAD);
      double ad = -se/Cdrymap;
      double bd = -ce*ce/(2.0*ell.a()*Cdrymap);
      double Rd = SQRT((ell.a()+Cdrymap)*(ell.a()+Cdrymap)
                - ell.a()*ell.a()*ce*ce) - ell.a()*se;

      double Ad[9], ad2=ad*ad, bd2=bd*bd;
      Ad[0] = 1.0;
      Ad[1] = 4.0*ad;
      Ad[2] = 6.0*ad2 + 4.0*bd;
      Ad[3] = 4.0*ad*(ad2+3.0*bd);
      Ad[4] = ad2*ad2 + 12.0*ad2*bd + 6.0*bd2;
      Ad[5] = 4.0*ad*bd*(ad2+3.0*bd);
      Ad[6] = bd2*(6.0*ad2+4.0*bd);
      Ad[7] = 4.0*ad*bd*bd2;
      Ad[8] = bd2*bd2;

         // compute dry component of the mapping function
      double sumd=0.0;
      for(int j=9; j>=1; j--) {
         sumd += Ad[j-1]/double(j);
         sumd *= Rd;
      }
      return sumd/GGdryscale;

   }

      // compute wet component of the mapping function
   double GGTropModel::wet_mapping_function(double elevation) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();

      if(elevation < 0.0) return 0.0;

      GPSEllipsoid ell;
      double ce = std::cos(elevation*DEG_TO_RAD), se = std::sin(elevation*DEG_TO_RAD);
      double aw = -se/Cwetmap;
      double bw = -ce*ce/(2.0*ell.a()*Cwetmap);
      double Rw = SQRT((ell.a()+Cwetmap)*(ell.a()+Cwetmap)
                - ell.a()*ell.a()*ce*ce) - ell.a()*se;

      double Aw[9], aw2=aw*aw, bw2=bw*bw;
      Aw[0] = 1.0;
      Aw[1] = 4.0*aw;
      Aw[2] = 6.0*aw2 + 4.0*bw;
      Aw[3] = 4.0*aw*(aw2+3.0*bw);
      Aw[4] = aw2*aw2 + 12.0*aw2*bw + 6.0*bw2;
      Aw[5] = 4.0*aw*bw*(aw2+3.0*bw);
      Aw[6] = bw2*(6.0*aw2+4.0*bw);
      Aw[7] = 4.0*aw*bw*bw2;
      Aw[8] = bw2*bw2;

      double sumw=0.0;
      for(int j=9; j>=1; j--) {
         sumw += Aw[j-1]/double(j);
         sumw *= Rw;
      }
      return sumw/GGwetscale;

   }

   void GGTropModel::setWeather(const double& T,
                                const double& P,
                                const double& H)
      throw(InvalidParameter)
   {
      TropModel::setWeather(T,P,H);
      double th=300./temp;
         // water vapor partial pressure (mb)
         // this comes from Leick and is not good.
         // double wvpp=6.108*(RHum*0.01)*exp((17.15*Tk-4684.0)/(Tk-38.45));
      double wvpp=2.409e9*humid*th*th*th*th*std::exp(-22.64*th);
      Cdrydelay = 7.7624e-5*press/temp;
      Cwetdelay = 1.0e-6*(-12.92+3.719e+05/temp)*(wvpp/temp);
      Cdrymap = (5.0*0.002277*press)/Cdrydelay;
      Cwetmap = (5.0*0.002277/Cwetdelay)*(1255.0/temp+0.5)*wvpp;
      valid = true;
   }  // end GGTropModel::setWeather(T,P,H)

      // Re-define the tropospheric model with explicit weather data.
      // Typically called just before correction().
      // @param wx the weather to use for this correction
   void GGTropModel::setWeather(const WxObservation& wx)
      throw(InvalidParameter)
   {
      TropModel::setWeather(wx);
   }
}
