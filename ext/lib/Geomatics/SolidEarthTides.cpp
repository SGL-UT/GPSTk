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

/// @file SolidEarthTides.cpp
/// Implement the formula for the displacement of a point fixed to the solid Earth
/// due to the solid Earth tides resulting from the influence of the Sun and Moon.
/// Reference IERS Conventions (1996) found in IERS Technical Note 21 (IERS).
/// NB. Currently only the largest terms are implemented, yielding a result accurate
/// to the millimeter level. Specifically, IERS pg 61 eq 8 and IERS pg 65 eq 17,
/// (this includes removing the permanent component).
/// Class SolarSystem may be used to get Solar and Lunar ephemeris information,
/// including position and mass ratios.

//------------------------------------------------------------------------------------
// GPSTk
//#include "geometry.hpp"             // for DEG_TO_RAD
// geomatics
#include "logstream.hpp"
#include "SolidEarthTides.hpp"

using namespace std;

namespace gpstk
{
   //---------------------------------------------------------------------------------
   // Compute the site displacement due to solid Earth tides for the given Position
   // (assumed to be fixed to the solid Earth) at the given time, given the position
   // of the site of interest, positions and mass ratios of the sun and moon.
   // Return a Triple containing the site displacement in ECEF XYZ coordinates with
   // units meters.
   // Reference IERS Conventions (1996) found in IERS Technical Note 21
   //       and IERS Conventions (2003) found in IERS Technical Note 32
   //       and IERS Conventions (2010) found in IERS Technical Note 36.
   // NB. Currently only the largest terms are implemented, yielding a result
   // accurate to the millimeter level. Specifically, TN21 pg 61 eq 8 and
   // TN21 pg 65 eq 17.
   // param site Position  Nominal position of the site of interest.
   // param ttag EphTime   Time of interest.
   // param Sun Position   Position of the Sun at time
   // param Moon Position  Position of the Moon at time
   // param EMRAT double   Earth-to-Moon mass ratio (default to DE405 value)
   // param SERAT double   Sun-to-Earth mass ratio (default to DE405 value)
   // param IERSConvention IERS convention to use (default IERS2010)
   // return Triple        Displacement vector, ECEF XYZ in meters.
   Triple computeSolidEarthTides(const Position site,
                                 const EphTime ttag,
                                 const Position Sun,
                                 const Position Moon,
                                 const double EMRAT,
                                 const double SERAT,
                                 const IERSConvention iers)
      throw(Exception)
   {
   try {
      //if(ttag.getTimeSystem() == TimeSystem::Unknown) {
      //   Exception e("Time system is unknown");
      //   GPSTK_THROW(e);
      //}
      
      // Use REarth from solid.f example program
      static const double REarth=6378136.55;
      static const bool debug = (LOGlevel >= DEBUG7);
      // NB icount is a dummy used in test vs solid.f
      int i,icount(-1);
      double RSun, RMoon, Rx, Love, Shida, sunFactor, moonFactor;
      double sunDOTrx, moonDOTrx, REoRS, REoRM;
      double lat, lon, sinlat, coslat, sinlon, coslon;
      double latSun, lonSun, latMoon, lonMoon;
      Triple disp, sunUnit, moonUnit, rx, tSun, tMoon, north, east, up;
      // quantities for debug printing only
      Triple northGD, eastGD, upGD, tmp, tmp2, tmp3, tmp4;
   
      LOG(DEBUG7) << "Sun position " << ttag.asGPSString()
            << fixed << setprecision(3)
            << setw(23) << Sun.X() << setw(23) << Sun.Y() << setw(23) << Sun.Z();
      LOG(DEBUG7) << "Moon position" << ttag.asGPSString()
            << fixed << setprecision(3)
            << setw(23) << Moon.X() << setw(23) << Moon.Y() << setw(23) << Moon.Z();
   
      // distances (m)
      RSun = Sun.radius();
      RMoon = Moon.radius();
      Rx = site.radius();

      // unit vectors
      sunUnit = Triple(Sun.X()/RSun, Sun.Y()/RSun, Sun.Z()/RSun);
      moonUnit = Triple(Moon.X()/RMoon, Moon.Y()/RMoon, Moon.Z()/RMoon);
      rx = Triple(site.X()/Rx, site.Y()/Rx, site.Z()/Rx);
   
      // generate geodetic transformation first - for debug
      if(debug) {
         lat = site.getGeodeticLatitude()*DEG_TO_RAD;
         lon = site.getLongitude()*DEG_TO_RAD;
         sinlat = ::sin(lat);
         coslat = ::cos(lat);
         sinlon = ::sin(lon);
         coslon = ::cos(lon);

         // transform  X=(x,y,z) into (R*X)(north,east,up) using geodetic longitude
         northGD = Triple(-sinlat*coslon, -sinlat*sinlon, coslat);
         eastGD  = Triple(       -sinlon,         coslon,    0.0);
         upGD    = Triple( coslat*coslon,  coslat*sinlon, sinlat);
      }

      // use geocentric latitude for formulas
      latSun = Sun.getGeocentricLatitude()*DEG_TO_RAD;
      lonSun = Sun.getLongitude()*DEG_TO_RAD;
      latMoon = Moon.getGeocentricLatitude()*DEG_TO_RAD;
      lonMoon = Moon.getLongitude()*DEG_TO_RAD;
      lat = site.getGeocentricLatitude()*DEG_TO_RAD;
      lon = site.getLongitude()*DEG_TO_RAD;
      sinlat = ::sin(lat);
      coslat = ::cos(lat);
      sinlon = ::sin(lon);
      coslon = ::cos(lon);
   
      // transform  X=(x,y,z) into (R*X)(north,east,up) using geocentric longitude
      north = Triple(-sinlat*coslon, -sinlat*sinlon, coslat);
      east  = Triple(       -sinlon,         coslon,    0.0);
      up    = Triple( coslat*coslon,  coslat*sinlon, sinlat);
   
      // GM*R factors
      REoRS = REarth/RSun;                         // ratio Earth/Sun radius
      sunFactor = REarth*REoRS*REoRS*REoRS*SERAT;  // = (GMS/GME)*RE^4/RS^3
      REoRM = REarth/RMoon;                        // ratio Earth/Moon radius
      moonFactor = REarth*REoRM*REoRM*REoRM/EMRAT; // = (GMM/GME)*RE^4/RM^3
      // E/M mass ratio (403) 81.300584999999998         (405) 81.300560000000004
      // S/E mass ratio (403) 332946.048630181234330     (405) 332946.050894783285912
      //LOG(INFO) << " E/M mass ratio " << fixed << setprecision(15) << EMRAT;
      //LOG(INFO) << " S/E mass ratio " << fixed << setprecision(15) << SERAT;
   
      // dot products
      sunDOTrx = sunUnit.dot(rx);
      moonDOTrx = moonUnit.dot(rx);
   
      // transverse to radial direction - not unit vectors
      tSun = sunUnit - sunDOTrx * rx;
      tMoon = moonUnit - moonDOTrx * rx;
   
      // -------------------------------------------------------------------------
      // compute displacements
      disp = Triple(0,0,0);
      // Steps and equations refer to IERS(1996), esp table on page 60;
      // formulas are generally repeated in other IERS technical notes.

      // Step 1a IERS(1996) eq. (8) pg 61.
      // nominal degree 2 Love and Shida numbers pg 60
      double poly = sinlat;
      poly = (3.0*poly*poly-1.0)/2.0;

      // here is the only difference between 1996 and 2003/10
      if(iers == IERSConvention::IERS1996) {
         Love = 0.6026 - 0.0006*poly;
         Shida = 0.0831 + 0.0002*poly;
      }
      else {            // 2003 or 2010
         Love = 0.6078 - 0.0006*poly;
         Shida = 0.0847 + 0.0002*poly;
      }
      LOG(DEBUG6) << "H2L2 " << setw(4) << icount << fixed << setprecision(15)
               << " " << setw(18) << Love
               << " " << setw(18) << Shida
               << " " << setw(18) << poly;
      LOG(DEBUG6) << "P2 " << setw(4) << icount << fixed << setprecision(15)
               << " " << setw(18) << 3*(Love/2-Shida)*sunDOTrx*sunDOTrx-0.5*Love
               << " " << setw(18) << 3*(Love/2-Shida)*moonDOTrx*moonDOTrx-0.5*Love;
   
      tmp = sunFactor * (Love * (1.5*sunDOTrx*sunDOTrx-0.5) * rx
                         + 3.0*Shida*sunDOTrx*tSun);
      for(i=0; i<3; i++) disp[i] += tmp[i];
   
      tmp = moonFactor * (Love * (1.5*moonDOTrx*moonDOTrx-0.5) * rx
                          + 3.0*Shida*moonDOTrx*tMoon);
      for(i=0; i<3; i++) disp[i] += tmp[i];
   
      // Step 1b IERS(1996) eq. (9) pg 61.
      // nominal degree 3 Love and Shida numbers pg 60
      double Shida2=Shida;
      Love = 0.292;
      Shida = 0.015;
      tmp = sunFactor*REoRS * (Love * (2.5*sunDOTrx*sunDOTrx - 1.5) * sunDOTrx * rx
                               + Shida * (7.5*sunDOTrx*sunDOTrx - 1.5) * tSun);
      for(i=0; i<3; i++) disp[i] += tmp[i];
   
      LOG(DEBUG6) << "P3 " << setw(4) << icount << fixed << setprecision(15)
               << " " << setw(18) << 2.5*(Love-3*Shida)*sunDOTrx*sunDOTrx*sunDOTrx
                                       +1.5*(Shida-Love)*sunDOTrx
               << " " << setw(18) << 2.5*(Love-3*Shida)*moonDOTrx*moonDOTrx*moonDOTrx
                                       +1.5*(Shida-Love)*moonDOTrx;
      LOG(DEBUG6) << "X2 " << setw(4) << icount << fixed << setprecision(15)
               << " " << setw(18) << 3*Shida2*sunDOTrx
               << " " << setw(18) << 3*Shida2*moonDOTrx;
      LOG(DEBUG6) << "X3 " << setw(4) << icount << fixed << setprecision(15)
               << " " << setw(18) << 1.5*Shida*(5*sunDOTrx*sunDOTrx-1)
               << " " << setw(18) << 1.5*Shida*(5*moonDOTrx*moonDOTrx-1);
      LOG(DEBUG6) << "RAT " << setw(4) << icount << fixed << setprecision(6)
               << " " << setw(18) << SERAT << setprecision(15)
               << " " << setw(22) << EMRAT << setprecision(2)
               << " " << setw(11) << REarth;
      LOG(DEBUG6) << "FACT2 " << setw(4) << icount << fixed << setprecision(15)
               << " " << setw(18) << sunFactor
               << " " << setw(18) << moonFactor;
      LOG(DEBUG6) << "FACT3 " << setw(4) << icount << fixed << setprecision(15)
               << " " << setw(18) << sunFactor*REoRS
               << " " << setw(18) << moonFactor*REoRM;

      tmp = moonFactor*REoRM * (Love * (2.5*moonDOTrx*moonDOTrx-1.5) * moonDOTrx * rx
                            + Shida * (7.5*moonDOTrx*moonDOTrx-1.5) * tMoon);
      for(i=0; i<3; i++) disp[i] += tmp[i];
   
      // all of 8 and 9
      if(debug) {
         tmp2[0] = northGD[0]*disp[0] + northGD[1]*disp[1] + northGD[2]*disp[2];
         tmp2[1] =  eastGD[0]*disp[0] +  eastGD[1]*disp[1] +  eastGD[2]*disp[2];
         tmp2[2] =    upGD[0]*disp[0] +    upGD[1]*disp[1] +    upGD[2]*disp[2];
         LOG(DEBUG7) << "7SET solar/lunar/2nd/3rd " << ttag.asGPSString()
            << fixed << setprecision(9)
            << " " << disp[0] << " " << disp[1] << " " << disp[2]
            << " " << tmp2[0] << " " << tmp2[1] << " " << tmp2[2];
      }
      LOG(DEBUG6) << "DX0 " << setw(4) << icount << fixed << setprecision(15)
                  << " " << setw(18) << disp[0]
                  << " " << setw(18) << disp[1]
                  << " " << setw(18) << disp[2];
   
      // Step 1c IERS(1996) eq. (13) pg 63. diurnal tides
      Love = -0.0025;
      Shida = -0.0007;
      tmp = -0.75*Love*::sin(2*lat) *            // radial 13a
               ( sunFactor*::sin(2*latSun)*::sin(lon-lonSun)
                 + moonFactor*::sin(2*latMoon)*::sin(lon-lonMoon)) * rx
              
             - 1.5*Shida*::cos(2*lat) *          // north 13b
               ( sunFactor*::sin(2*latSun)*::sin(lon-lonSun)
                 + moonFactor*::sin(2*latMoon)*::sin(lon-lonMoon)) * north
   
             - 1.5*Shida*sinlat *                // east 13b
               ( sunFactor*::sin(2*latSun)*::cos(lon-lonSun)
                 + moonFactor*::sin(2*latMoon)*::cos(lon-lonMoon)) * east;

      LOG(DEBUG6) << "DX1 " << setw(4) << icount << fixed << setprecision(15)
                  << " " << setw(18) << tmp[0]
                  << " " << setw(18) << tmp[1]
                  << " " << setw(18) << tmp[2];

      for(i=0; i<3; i++) disp[i] += tmp[i];
   
      if(debug) {
         tmp2[0] = northGD[0]*tmp[0] + northGD[1]*tmp[1] + northGD[2]*tmp[2];
         tmp2[1] =  eastGD[0]*tmp[0] +  eastGD[1]*tmp[1] +  eastGD[2]*tmp[2];
         tmp2[2] =    upGD[0]*tmp[0] +    upGD[1]*tmp[1] +    upGD[2]*tmp[2];
         LOG(DEBUG7) << "7SET diurnal-band " << ttag.asGPSString()
            << fixed << setprecision(9)
            << " " << tmp[0] << " " << tmp[1] << " " << tmp[2]
            << " " << tmp2[0] << " " << tmp2[1] << " " << tmp2[2];
      }
   
      // Step 1d IERS(1996) eq. (14) pg 63. semidiurnal tides
      Love = -0.0022;
      Shida = -0.0007;
      tmp = -0.75*Love*coslat*coslat *          // radial 14a
            ( sunFactor*::cos(latSun)*::cos(latSun)*::sin(2*(lon-lonSun))
             + moonFactor*::cos(latMoon)*::cos(latMoon)*::sin(2*(lon-lonMoon)))*rx
   
            + 0.75*Shida*::sin(2*lat) *         // north 14b
            ( sunFactor*::cos(latSun)*::cos(latSun)*::sin(2*(lon-lonSun))
             + moonFactor*::cos(latMoon)*::cos(latMoon)*::sin(2*(lon-lonMoon)))*north
   
            - 1.50*Shida*coslat *               // east 14b
            ( sunFactor*::cos(latSun)*::cos(latSun)*::cos(2*(lon-lonSun))
             + moonFactor*::cos(latMoon)*::cos(latMoon)*::cos(2*(lon-lonMoon)))*east;

      LOG(DEBUG6) << "DX2 " << setw(4) << icount << fixed << setprecision(15)
                  << " " << setw(18) << tmp[0]
                  << " " << setw(18) << tmp[1]
                  << " " << setw(18) << tmp[2];

      for(i=0; i<3; i++) disp[i] += tmp[i];
   
      if(debug) {
         tmp2[0] = northGD[0]*tmp[0] + northGD[1]*tmp[1] + northGD[2]*tmp[2];
         tmp2[1] =  eastGD[0]*tmp[0] +  eastGD[1]*tmp[1] +  eastGD[2]*tmp[2];
         tmp2[2] =    upGD[0]*tmp[0] +    upGD[1]*tmp[1] +    upGD[2]*tmp[2];
         LOG(DEBUG7) << "7SET semi-diurnal-band " << ttag.asGPSString()
            << fixed << setprecision(9)
            << " " << tmp[0] << " " << tmp[1] << " " << tmp[2]
            << " " << tmp2[0] << " " << tmp2[1] << " " << tmp2[2];
      }
   
      // Step 1e IERS(1996) eq. (11) pg 62. latitude dependence of diurnal band
      Shida = 0.0012;
      tmp = - 3.0*Shida*sinlat*sinlat *         // north
              ( sunFactor*::cos(latSun)*::sin(latSun)*::cos(lon-lonSun)
               + moonFactor*::cos(latMoon)*::sin(latMoon)*::cos(lon-lonMoon)) * north
   
            + 3.0*Shida*sinlat*::cos(2*lat) *   // east
              ( sunFactor*::cos(latSun)*::sin(latSun)*::sin(lon-lonSun)
               + moonFactor*::cos(latMoon)*::sin(latMoon)*::sin(lon-lonMoon)) * east;

      for(i=0; i<3; i++) { tmp4[i] = tmp[i]; disp[i] += tmp[i]; }
   
      if(debug) {
         tmp3 = tmp;    // save for below
         tmp2[0] = northGD[0]*tmp[0] + northGD[1]*tmp[1] + northGD[2]*tmp[2];
         tmp2[1] =  eastGD[0]*tmp[0] +  eastGD[1]*tmp[1] +  eastGD[2]*tmp[2];
         tmp2[2] =    upGD[0]*tmp[0] +    upGD[1]*tmp[1] +    upGD[2]*tmp[2];
         LOG(DEBUG7) << "7SET latitude-diurnal-band " << ttag.asGPSString()
            << fixed << setprecision(9)
            << " " << tmp[0] << " " << tmp[1] << " " << tmp[2]
            << " " << tmp2[0] << " " << tmp2[1] << " " << tmp2[2];
      }
   
      // Step 1f IERS(1996) eq. (12) pg 62. semidiurnal band
      Shida = 0.0024;
      tmp = - 1.5*Shida*sinlat*coslat *            // north
              ( sunFactor*::cos(latSun)*::cos(latSun)*::cos(2*(lon-lonSun))
             + moonFactor*::cos(latMoon)*::cos(latMoon)*::cos(2*(lon-lonMoon)))*north
   
            - 1.5*Shida*sinlat*sinlat*coslat *     // east
              ( sunFactor*::cos(latSun)*::cos(latSun)*::sin(2*(lon-lonSun))
             + moonFactor*::cos(latMoon)*::cos(latMoon)*::sin(2*(lon-lonMoon)))*east;

      LOG(DEBUG6) << "DX3 " << setw(4) << icount << fixed << setprecision(15)
                  << " " << setw(18) << tmp4[0]+tmp[0]
                  << " " << setw(18) << tmp4[1]+tmp[1]
                  << " " << setw(18) << tmp4[2]+tmp[2];

      for(i=0; i<3; i++) disp[i] += tmp[i];
   
      if(debug) {
         tmp2[0] = northGD[0]*tmp[0] + northGD[1]*tmp[1] + northGD[2]*tmp[2];
         tmp2[1] =  eastGD[0]*tmp[0] +  eastGD[1]*tmp[1] +  eastGD[2]*tmp[2];
         tmp2[2] =    upGD[0]*tmp[0] +    upGD[1]*tmp[1] +    upGD[2]*tmp[2];
         LOG(DEBUG7) << "7SET latitude-semi-diurnal " << ttag.asGPSString()
            << fixed << setprecision(9)
            << " " << tmp[0] << " " << tmp[1] << " " << tmp[2]
            << " " << tmp2[0] << " " << tmp2[1] << " " << tmp2[2];

         // combine latitude-dependent terms for comparison with solid.f
         tmp = tmp + tmp3;
         tmp2[0] = northGD[0]*tmp[0] + northGD[1]*tmp[1] + northGD[2]*tmp[2];
         tmp2[1] =  eastGD[0]*tmp[0] +  eastGD[1]*tmp[1] +  eastGD[2]*tmp[2];
         tmp2[2] =    upGD[0]*tmp[0] +    upGD[1]*tmp[1] +    upGD[2]*tmp[2];
         LOG(DEBUG7) << "7SET latitude-dependent " << ttag.asGPSString()
            << fixed << setprecision(9)
            << " " << tmp[0] << " " << tmp[1] << " " << tmp[2]
            << " " << tmp2[0] << " " << tmp2[1] << " " << tmp2[2];
      }
   
      // Step 2a IERS(1996) eq. (15) pg 63.
      // frequency dependence of Love and Shida from diurnal band
      static double step2diurnalData[9*31] = {
        -3., 0., 2., 0., 0.,-0.01,-0.01,  0.0,  0.0,
        -3., 2., 0., 0., 0.,-0.01,-0.01,  0.0,  0.0,
        -2., 0., 1.,-1., 0.,-0.02,-0.01,  0.0,  0.0,
        -2., 0., 1., 0., 0.,-0.08, 0.00, 0.01, 0.01,
        -2., 2.,-1., 0., 0.,-0.02,-0.01,  0.0,  0.0,
        -1., 0., 0.,-1., 0.,-0.10, 0.00, 0.00, 0.00,
        -1., 0., 0., 0., 0.,-0.51, 0.00,-0.02, 0.03,
        -1., 2., 0., 0., 0., 0.01,  0.0,  0.0,  0.0,
         0.,-2., 1., 0., 0., 0.01,  0.0,  0.0,  0.0,
         0., 0.,-1., 0., 0., 0.02, 0.01,  0.0,  0.0,
         0., 0., 1., 0., 0., 0.06, 0.00, 0.00, 0.00,
         0., 0., 1., 1., 0., 0.01,  0.0,  0.0,  0.0,
         0., 2.,-1., 0., 0., 0.01,  0.0,  0.0,  0.0,
         1.,-3., 0., 0., 1.,-0.06, 0.00, 0.00, 0.00,
         1.,-2., 0., 1., 0., 0.01,  0.0,  0.0,  0.0,
         1.,-2., 0., 0., 0.,-1.23,-0.07, 0.06, 0.01,
         1.,-1., 0., 0.,-1., 0.02,  0.0,  0.0,  0.0,
         1.,-1., 0., 0., 1., 0.04,  0.0,  0.0,  0.0,
         1., 0., 0.,-1., 0.,-0.22, 0.01, 0.01, 0.00,
         1., 0., 0., 0., 0.,12.00,-0.78,-0.67,-0.03,
         1., 0., 0., 1., 0., 1.73,-0.12,-0.10, 0.00,
         1., 0., 0., 2., 0.,-0.04,  0.0,  0.0,  0.0,
         1., 1., 0., 0.,-1.,-0.50,-0.01, 0.03, 0.00,
         1., 1., 0., 0., 1., 0.01,  0.0,  0.0,  0.0,
         1., 1., 0., 1.,-1.,-0.01,  0.0,  0.0,  0.0,
         1., 2.,-2., 0., 0.,-0.01,  0.0,  0.0,  0.0,
         1., 2., 0., 0., 0.,-0.11, 0.01, 0.01, 0.00,
         2.,-2., 1., 0., 0.,-0.01,  0.0,  0.0,  0.0,
         2., 0.,-1., 0., 0.,-0.02, 0.02,  0.0, 0.01,
         3., 0., 0., 0., 0., 0.0,  0.01,  0.0, 0.01,
         3., 0., 0., 1., 0., 0.0,  0.01,  0.0,  0.0 };
   
      // times
      EphTime TT(ttag);
      TT.convertSystemTo(TimeSystem::TT);
      double T,fhr,fmjd = TT.dMJD();
      T = (fmjd-51544.0)/36525.0;            // MJD of J2000 is 51544.0
      fhr = (fmjd-int(fmjd))*24.0;
   
      // compute standard arguments
      double s,tau,pr,h,p,zns,ps;
      {
         double T2 = T*T;
         double T3 = T2*T;
         double T4 = T3*T;
         s = 218.31664563 + 481267.88194*T - 0.0014663889*T2 + 0.00000185139*T3;
         tau = fhr*15. + 280.4606184 + 36000.7700536*T + 0.00038793*T2
                                                       - 0.0000000258*T3;
         tau = tau - s;
         pr = 1.396971278*T + 0.000308889*T2 + 0.000000021*T3 + 0.000000007*T4;
         s = s + pr;
         h = 280.46645 + 36000.7697489*T + 0.00030322222*T2 + 0.000000020*T3
                                                            - 0.00000000654*T4;
         p = 83.35324312 + 4069.01363525*T - 0.01032172222*T2 - 0.0000124991*T3
                                                              + 0.00000005263*T4;
         zns = 234.95544499 + 1934.13626197*T - 0.00207561111*T2 - 0.00000213944*T3
                                                                  + 0.00000001650*T4;
         ps = 282.93734098 + 1.71945766667*T + 0.00045688889*T2 - 0.00000001778*T3
                                                                - 0.00000000334*T4;
         s   = fmod(s,  360.0);
         tau = fmod(tau,360.0);
         h   = fmod(h,  360.0);
         p   = fmod(p,  360.0);
         zns = fmod(zns,360.0);
         ps  = fmod(ps, 360.0);
      }
   
      double thetaf,ctl,stl,dr,dn,de;
      tmp = Triple(0,0,0);
      for(i=0; i<31; i++) {
         thetaf = (tau + step2diurnalData[0+9*i] * s
                       + step2diurnalData[1+9*i] * h
                       + step2diurnalData[2+9*i] * p
                       + step2diurnalData[3+9*i] * zns
                       + step2diurnalData[4+9*i] * ps) * DEG_TO_RAD;
         ctl = ::cos(thetaf+lon);
         stl = ::sin(thetaf+lon);
         dr = (step2diurnalData[5+9*i] * stl
              +step2diurnalData[6+9*i] * ctl) * 2 * sinlat * coslat;
         dn = (step2diurnalData[7+9*i] * stl
              +step2diurnalData[8+9*i] * ctl) * (coslat*coslat - sinlat*sinlat);
         de = (step2diurnalData[7+9*i] * ctl
              -step2diurnalData[8+9*i] * stl) * sinlat;
         tmp[0] += dr*up[0] + de*east[0] + dn*north[0];
         tmp[1] += dr*up[1] + de*east[1] + dn*north[1];
         tmp[2] += dr*up[2]              + dn*north[2];
      }
   
      // convert total to meters
      for(i=0; i<3; i++) tmp[i] /= 1000.0;   // mm -> m

         LOG(DEBUG6) << "DX4 " << setw(4) << icount << fixed << setprecision(15)
                     << " " << setw(18) << tmp[0]
                     << " " << setw(18) << tmp[1]
                     << " " << setw(18) << tmp[2];

      for(i=0; i<3; i++) disp[i] += tmp[i];
   
      if(debug) {
         tmp2[0] = northGD[0]*tmp[0] + northGD[1]*tmp[1] + northGD[2]*tmp[2];
         tmp2[1] =  eastGD[0]*tmp[0] +  eastGD[1]*tmp[1] +  eastGD[2]*tmp[2];
         tmp2[2] =    upGD[0]*tmp[0] +    upGD[1]*tmp[1] +    upGD[2]*tmp[2];
         LOG(DEBUG7) << "7SET diurnal-band-corrections " << ttag.asGPSString()
            << fixed << setprecision(9)
            << " " << tmp[0] << " " << tmp[1] << " " << tmp[2]
            << " " << tmp2[0] << " " << tmp2[1] << " " << tmp2[2];
      }
      
      //// Ref. Kouba 2009 and IERS technical note 3, 1989 (out of print)
      //// Kouba (pers comm 8/12/09) says it is not necessary unless using IERS(1989)
      //double Tg;        // Greenwhich sidereal time
      //Tg = TT.JD()-2451545.;
      //if(Tg <= 0.0) Tg -= 1.0;
      //Tg /= 36525.;
      //Tg = 0.279057273264 + 100.0021390378009*Tg
      //                    + (0.093104 - 6.2e-6*Tg)*Tg*Tg/86400.0;
      //Tg += TT.secOfDay()/86400.0;                      // days
      //Tg = fmod(Tg,1.0);
      //while(Tg < 0.0) Tg += 1.0;
      //Tg *= TWO_PI;         // radians
      //dr = -25. * sinlat * coslat * ::sin(Tg + lon);
      //for(i=0; i<3; i++) tmp[i] -= dr*up[i];
   
      // Step 2b IERS(1996) eq. (16) pg 64.
      // frequency dependence of Love and Shida from the long period band
      static double step2longData[9*5] = {
         0, 0, 0, 1, 0,  0.47, 0.23, 0.16, 0.07,
         0, 2, 0, 0, 0, -0.20,-0.12,-0.11,-0.05,
         1, 0,-1, 0, 0, -0.11,-0.08,-0.09,-0.04,
         2, 0, 0, 0, 0, -0.13,-0.11,-0.15,-0.07,
         2, 0, 0, 1, 0, -0.05,-0.05,-0.06,-0.03 };
   
      tmp = Triple(0,0,0);
      for(i=0; i<5; i++) {
         thetaf = (  step2longData[0+9*i] * s
                   + step2longData[1+9*i] * h
                   + step2longData[2+9*i] * p
                   + step2longData[3+9*i] * zns
                   + step2longData[4+9*i] * ps) * DEG_TO_RAD;
         ctl = ::cos(thetaf);
         stl = ::sin(thetaf);
         dr = (step2longData[5+9*i] * ctl
              +step2longData[7+9*i] * stl) * (3*sinlat*sinlat-1)/2;
         dn = (step2longData[6+9*i] * ctl
              +step2longData[8+9*i] * stl) * 2*sinlat*coslat;
         //de = 0.0; remove from next 3 lines
         tmp[0] += dr*up[0]              + dn*north[0];
         tmp[1] += dr*up[1]              + dn*north[1];
         tmp[2] += dr*up[2]              + dn*north[2];
      }
      for(i=0; i<3; i++) tmp[i] /= 1000.0;   // mm -> m

      LOG(DEBUG6) << "DX5 " << setw(4) << icount << fixed << setprecision(15)
                  << " " << setw(18) << tmp[0]
                  << " " << setw(18) << tmp[1]
                  << " " << setw(18) << tmp[2];

      for(i=0; i<3; i++) disp[i] += tmp[i];
   
      if(debug) {
         tmp2[0] = northGD[0]*tmp[0] + northGD[1]*tmp[1] + northGD[2]*tmp[2];
         tmp2[1] =  eastGD[0]*tmp[0] +  eastGD[1]*tmp[1] +  eastGD[2]*tmp[2];
         tmp2[2] =    upGD[0]*tmp[0] +    upGD[1]*tmp[1] +    upGD[2]*tmp[2];
         LOG(DEBUG7) << "7SET long-period-band-corr.s " << ttag.asGPSString()
            << fixed << setprecision(9)
            << " " << tmp[0] << " " << tmp[1] << " " << tmp[2]
            << " " << tmp2[0] << " " << tmp2[1] << " " << tmp2[2];
      }
   
      // remove permanent deformation IERS(1996) eq. 17 pg 65.
      //tmp = -0.1196*(1.5*sinlat*sinlat-0.5)*rx - 0.0247*::sin(2*lat)*north;
      //for(i=0; i<3; i++) disp[i] += tmp[i];
   
      if(debug) {
         tmp = -0.1196*(1.5*sinlat*sinlat-0.5)*rx - 0.0247*::sin(2*lat)*north;
         tmp2[0] = northGD[0]*tmp[0] + northGD[1]*tmp[1] + northGD[2]*tmp[2];
         tmp2[1] =  eastGD[0]*tmp[0] +  eastGD[1]*tmp[1] +  eastGD[2]*tmp[2];
         tmp2[2] =    upGD[0]*tmp[0] +    upGD[1]*tmp[1] +    upGD[2]*tmp[2];
         LOG(DEBUG7) << "7SET permanent-tide-not-incl. " << ttag.asGPSString()
            << fixed << setprecision(9)
            << " " << tmp[0] << " " << tmp[1] << " " << tmp[2]
            << " " << tmp2[0] << " " << tmp2[1] << " " << tmp2[2];
      }
   
      if(debug) {
         for(i=0; i<3; i++) tmp[i] = disp[i];
         tmp2[0] = northGD[0]*tmp[0] + northGD[1]*tmp[1] + northGD[2]*tmp[2];
         tmp2[1] =  eastGD[0]*tmp[0] +  eastGD[1]*tmp[1] +  eastGD[2]*tmp[2];
         tmp2[2] =    upGD[0]*tmp[0] +    upGD[1]*tmp[1] +    upGD[2]*tmp[2];
         LOG(DEBUG7) << "7SET total " << ttag.asGPSString()
            << fixed << setprecision(9)
            << " " << tmp[0] << " " << tmp[1] << " " << tmp[2]
            << " " << tmp2[0] << " " << tmp2[1] << " " << tmp2[2];
      }
   
      return disp;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e) {
      Exception E("std except: "+string(e.what()));
      GPSTK_THROW(E);
   }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }

   }  // end computeSolidEarthTides()

   //---------------------------------------------------------------------------------
   /// Compute the site displacement due to rotational deformation due to polar motion
   /// for the given Position (assumed to fixed to the solid Earth) at the given time,
   /// given the polar motion angles at time (cf.EarthOrientation).
   /// Return a Triple containing the site displacement in WGS84 ECEF XYZ coordinates
   /// with units meters.
   /// Reference (1996) IERS Technical Note 21 (IERS), ch. 7 page 67.
   /// Reference (2003) IERS Technical Note 32 (IERS), ch. 7 page 83-84.
   /// Reference (2010) IERS Technical Note 36 (IERS), ch. 7 page 114-116.
   /// param site                Nominal position of the site of interest.
   /// param ttag                Time of interest.
   /// param iers IERSConvention IERS convention to use
   /// param xp double           Polar motion angle in arcsec (cf. EarthOrientation)
   /// param yp double           Polar motion angle in arcsec (cf. EarthOrientation)
   /// return disp Triple disp   Displacement vector, ECEF XYZ meters.
   Triple computePolarTides(const Position site, const EphTime ttag,
                            const double xp, const double yp,
                            const IERSConvention iers)
      throw(Exception)
   {
   try {
      double m1, m2, upcoef;

      if(iers == IERSConvention::IERS1996) {    // 1996
         m1 = xp;                   // arcsec
         m2 = yp;                   // arcsec
         upcoef = 0.032;
      }
      else {                                    // 2003 and 2010
         // compute time since J2000 in years and mean pole wander
         double dt((ttag.dMJD()-51544.5)/365.25);
         double xmean, ymean;
         // mean sums in milliarcsec
         if(iers == IERSConvention::IERS2003) {
            xmean = (0.054 + 0.00083*dt)/1000.0;      // convert to arcsec
            ymean = (0.357 + 0.00395*dt)/1000.0;      // convert to arcsec
            upcoef = 0.032;
         }
         else {                                 // 2003 and 2010
            // mean sums are different until 2010 and after 2010 (in milliarcsec)
            if(ttag.year() > 2010) {
               xmean = 23.513 + 7.6141 * dt;
               ymean = 358.891 - 0.6287 * dt;
            }
            else {
               xmean = 55.974 + (1.8243 + (0.18413 + 0.007024 * dt) * dt) * dt;
               ymean = 346.346 + (1.7896 - (0.10729 + 0.000908 * dt) * dt) * dt;
            }
            xmean /= 1000.0;                          // convert to arcsec
            ymean /= 1000.0;                          // convert to arcsec
            //  is this 33 a typo in Tech Note 36? other years are 32
            upcoef = 0.033;
         }
         m1 = (xp - xmean);          // arcsec
         m2 = -(yp - ymean);         // arcsec
      }
      LOG(DEBUG7) << " poletide means " << iers.asString()
         << fixed << setprecision(15) << " " << m1 << " " << m2;

      // the rest is nearly identical in all conventions
      double lat, lon, theta, sinlat, coslat, sinlon, coslon;
      Triple disp, dispXYZ;
   
      lat = site.getGeocentricLatitude();
      lon = site.getLongitude();
      sinlat = ::sin(lat*DEG_TO_RAD);
      coslat = ::cos(lat*DEG_TO_RAD);
      sinlon = ::sin(lon*DEG_TO_RAD);
      coslon = ::cos(lon*DEG_TO_RAD);
      theta = (90.0-lat)*DEG_TO_RAD;
   
      // NEU components (r==Up, theta=S, lambda=E)
      disp[0] =   0.009 * ::cos(2*theta) * (m1 * coslon + m2 * sinlon); // -S = N
      disp[1] =   0.009 * ::cos(theta) * (m1 * sinlon - m2 * coslon);   // E
      disp[2] = -upcoef * ::sin(2*theta) * (m1 * coslon + m2 * sinlon); // U
   
      LOG(DEBUG7) << " poletide " << iers.asString() << " (NEU) "
            << ttag.asGPSString()
            << fixed << setprecision(9)
            << disp[0] << " " << disp[1] << " " << disp[2];
   
      //// transform  X=(x,y,z) into (R*X)(north,east,up)
      //R(0,0) = -sa*co;  R(0,1) = -sa*so;  R(0,2) = ca;
      //R(1,0) =    -so;  R(1,1) =     co;  R(1,2) = 0.;
      //R(2,0) =  ca*co;  R(2,1) =  ca*so;  R(2,2) = sa;
   
      dispXYZ[0] = - sinlat*coslon*disp[0] - sinlon*disp[1] + coslat*coslon*disp[2];
      dispXYZ[1] = - sinlat*sinlon*disp[0] + coslon*disp[1] + coslat*sinlon*disp[2];
      dispXYZ[2] =          coslat*disp[0]                  +        sinlat*disp[2];
   
      return dispXYZ;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   }

}  // end namespace gpstk

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
