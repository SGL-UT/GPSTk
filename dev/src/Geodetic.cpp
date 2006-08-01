#pragma ident "$Id$"


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






/**
 * @file Geodetic.cpp
 * Geodetic coordinates (lat, lon, alt)
 */

#include "geometry.hpp"
#include "Geodetic.hpp"
#include "MiscMath.hpp"
#include "icd_200_constants.hpp"  // for TWO_PI

namespace gpstk
{
   using namespace std;

   Geodetic :: Geodetic()
         : Triple(), geoid(NULL)
   {
   }

   Geodetic :: Geodetic(const Geodetic& right)
         : Triple(right), geoid(right.geoid)
   {
   }

   Geodetic :: Geodetic(const double& lat, const double& lon, const double& alt,
                        GeoidModel* geo)
         : Triple(lat, lon, alt), geoid(geo)
   { 
   }

   Geodetic :: Geodetic(const Triple& t, GeoidModel* geo)
         : Triple(t), geoid(geo)
   {
   }
   
   Geodetic :: Geodetic(const ECEF& right, GeoidModel* geo)
      throw(NoGeoidException)
   {
      double X = right[0];     // m
      double Y = right[1];     // m
      double Z = right[2];     // m
      double p = RSS(X,Y);
      double latd = atan2(Z, p * (1.0 - geo->eccSquared()) );
      double ht = 0.0, slatd, N, htold, latdold;
      
      for(int i=0; i<5; i++)
      {
         slatd = sin(latd);
         N = geo->a() / SQRT(1.0 - geo->eccSquared() * slatd * slatd);
         htold = ht;
         ht = p/cos(latd) - N;
         latdold = latd;
         latd = atan2(Z, p * (1.0 - geo->eccSquared() * (N/(N+ht)) ) );
         if(ABS(latd-latdold) < 1.0e-9 && 
            ABS(ht-htold) < (1.0e-9 * geo->a()))    break;
      }

      double lon = atan2(Y,X);

      if(lon < 0.0) 
         lon += TWO_PI;

      theArray[0] = latd * RAD_TO_DEG; // deg
      theArray[1] = lon * RAD_TO_DEG;  // deg
      theArray[2] = ht;                // m
      geoid = geo;
   }

   Geodetic& Geodetic :: operator=(const Geodetic& right)
   {
      Triple::operator=(right);
      geoid = right.geoid;
      return *this;
   }

#if 0
      // This function is preserved here in case someone actually goes
      // about verifying it and finishing it.
      // As of July 31, 2002, David Munton, who implemented this
      // particular bit of code, recommends not using it.
   void tidal_corrections ( double rad_gdlat, double rad_gdlon, 
                            double& xval, double& yval, double& zval)
   {
      double radial_correction, transverse_correction;
      double gclat, re1;
      double corr_array[3];

      /*convert lat to geocentric */
      /* re1 = rad_earth(gdlat)
         gclat=geod_to_geoc(gdlat,re1);*/

      /* compute radial correction */

      radial_correction=-0.1196*(1.50*pow(sin(rad_gdlat),2)-0.5);

      /* compute tranverse correction */

      transverse_correction=-0.0247*sin(2.0*rad_gdlat);

      /* compute correction components then add to station locations*/

      corr_array[0]=radial_correction*cos(rad_gdlat)*cos(rad_gdlon) - 
         transverse_correction*sin(rad_gdlat)*cos(rad_gdlon);
      corr_array[1]=radial_correction*cos(rad_gdlat)*sin(rad_gdlon) - 
         transverse_correction*sin(rad_gdlat)*sin(rad_gdlon);
      corr_array[2]=radial_correction*sin(rad_gdlat) + 
         transverse_correction*cos(rad_gdlat);

      /* scale results to km units */

      //corr_array[0]=corr_array[0]*0.001;
      //corr_array[1]=corr_array[1]*0.001;
      //corr_array[2]=corr_array[2]*0.001;

      xval = xval + corr_array[0];
      yval = yval + corr_array[1];
      zval = zval + corr_array[2];
   }
#endif

      // based on formulae 2.30 and 2.31 in section 2.1.4, page 19 of
      // Satellite Geodesy by Gunter Seeber, 1993.
   gpstk::ECEF Geodetic :: asECEF() const throw(NoGeoidException)
   {
      double rad_cur, gdlat, gdlon;
      double gdalt = getAltitude();

      if (geoid == NULL)
      {
         NoGeoidException exc
            ("Must specify a geoid to use to change systems");
         GPSTK_THROW(exc);
      }

         // convert angles to radians
      gdlat = DEG_TO_RAD * getLatitude();
      gdlon = DEG_TO_RAD * getLongitude();

         // radius of curvature in the prime vertical, formula 2.31
      rad_cur  = geoid->a() /
         sqrt(1.0-geoid->eccSquared()*pow((sin(gdlat)),2.0));

         // formula 2.30
      double xval = (rad_cur + gdalt) * ::cos(gdlat) * ::cos(gdlon);
      double yval = (rad_cur + gdalt) * ::cos(gdlat) * ::sin(gdlon);
      double zval = ((1.0 - geoid->eccSquared()) * rad_cur + gdalt) * ::sin(gdlat);

      ECEF ecef(xval, yval, zval);

         // see comments for tidalCorrections above for why this is
         // commented out.
//      tidalCorrections(gdlat, gdlon, xarray[0], xarray[1], xarray[2]);

      return ecef;
   }
} // namespace gpstk
