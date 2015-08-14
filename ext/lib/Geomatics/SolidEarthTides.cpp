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
 * @file SolidEarthTides.cpp
 * Implement the formula for the displacement of a point fixed to the solid Earth
 * due to the solid Earth tides resulting from the influence of the Sun and Moon.
 * Reference IERS Conventions (1996) found in IERS Technical Note 21 (IERS).
 * NB. Currently only the largest terms are implemented, yielding a result accurate
 * to the millimeter level. Specifically, IERS pg 61 eq 8 and IERS pg 65 eq 17,
 * (this includes removing the permanent component).
 * Class SolarSystem is used to get Solar and Lunar ephemeris information, as well
 * as GM for sun and moon.
 */

#include "GNSSconstants.hpp"             // for DEG_TO_RAD

#include "logstream.hpp"
#include "GeodeticFrames.hpp"
#include "SolidEarthTides.hpp"

using namespace std;
using namespace gpstk;

void computeArgs(const double t, const double fhr, double& s, double& tau, double& pr,
                 double& h, double& p, double& zns, double& ps) throw();

//------------------------------------------------------------------------------------
// Compute the site displacement due to solid Earth tides for the given Position
// (assumed to be fixed to the solid Earth) at the given time. Return a Triple
// containing the site displacement in WGS84 ECEF XYZ coordinates with units meters.
// Reference IERS Conventions (1996) found in IERS Technical Note 21 (IERS).
// Also Kouba and Heroux (2001) state that the permanent deformation terms are
// required by the ITRF.
// NB. Currently only the largest terms are implemented, however this still yields a
// result accurate to the millimeter level. Specifically, IERS pg 61 eq 8 and IERS pg
// 65 eq 17 are implemented.
// @param  Position site    Nominal position of the site of interest.
// @param  CommonTime time     Time of interest.
// @param  SolarSystem sse  Reference to solar system ephemeris (class SolarSystem),
//                          must be initialized appropriately for time (by calling
//                          SolarSystem::initializeWithBinaryFile() ).
// @param  EarthOrientation eo Earth orientation parameters appropriate for time.
// @return Triple disp      Displacement vector, WGS84 ECEF XYZ meters.
// @throw if solar system ephemeris is not valid.
Triple computeSolidEarthTides(Position site, CommonTime time,
                              SolarSystem& sse, EarthOrientation& eo)
   throw(Exception)
{
try {
   if(sse.JPLNumber() == -1 ||
      sse.startTime()-time > 1.e-8 ||
      time-sse.endTime() > 1.e-8) {
         Exception e("Solar system ephemeris is not valid");
         GPSTK_THROW(e);
   }
   
   bool debug = (LOGlevel >= DEBUG7);
   int i;
   double RSun, RMoon, REarth=6378137.0, Rx, Love, Shida, sunFactor, moonFactor;
   double sunDOTrx, moonDOTrx, REoRS, REoRM, EMRAT;
   double lat, lon, sinlat, coslat, sinlon, coslon;
   double latSun, lonSun, latMoon, lonMoon;
   Position Sun, Moon;
   Triple disp, sunUnit, moonUnit, rx, tSun, tMoon, north, east, up, tmp, tmpNEU;

   // get positions (WGS84 m)
   Sun = sse.WGS84Position(SolarSystem::Sun, time, eo);
   Moon = sse.WGS84Position(SolarSystem::Moon, time, eo);
   LOG(DEBUG7) << "Sun position " << printTime(time,"%4F %10.3g ")
            << fixed << setprecision(3)
      << setw(23) << Sun.X() << setw(23) << Sun.Y() << setw(23) << Sun.Z();
   LOG(DEBUG7) << "Moon position" << printTime(time,"%4F %10.3g ")
            << fixed << setprecision(3)
      << setw(23) << Moon.X() << setw(23) << Moon.Y() << setw(23) << Moon.Z();

   // get distances (m)
   RSun = Sun.radius();
   RMoon = Moon.radius();
   Rx = site.radius();

   // get unit vectors
   sunUnit = Triple(Sun.X()/RSun, Sun.Y()/RSun, Sun.Z()/RSun);
   moonUnit = Triple(Moon.X()/RMoon, Moon.Y()/RMoon, Moon.Z()/RMoon);
   rx = Triple(site.X()/Rx, site.Y()/Rx, site.Z()/Rx);

   // latitude (geocentric) and longitude (East)
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

   //// transform  X=(x,y,z) into (R*X)(north,east,up)
   //R(0,0) = -sa*co;  R(0,1) = -sa*so;  R(0,2) = ca;
   //R(1,0) =    -so;  R(1,1) =     co;  R(1,2) = 0.;
   //R(2,0) =  ca*co;  R(2,1) =  ca*so;  R(2,2) = sa;
   north = Triple(-sinlat*coslon, -sinlat*sinlon, coslat);
   east  = Triple(       -sinlon,         coslon,    0.0);
   up    = Triple( coslat*coslon,  coslat*sinlon, sinlat);

   // GM*R factors
   EMRAT = sse.getConstant("EMRAT");
   REoRS = REarth/RSun;
   sunFactor = REarth*REoRS*REoRS*REoRS;        // RE^4/RS^3
   sunFactor *= sse.getConstant("GMS")*((1.0+EMRAT)/EMRAT)/sse.getConstant("GMB");
                                                // (GMS/GME)*RE^4/RS^3
   REoRM = REarth/RMoon;
   moonFactor = REarth*REoRM*REoRM*REoRM/EMRAT; // (GMM/GME)*RE^4/RM^3

   LOG(DEBUG7) << "Moon/Earth mass ratio " << printTime(time,"%4F %10.3g ")
            << fixed << setprecision(9) << 1.0/EMRAT;
   LOG(DEBUG7) << "Earth/Sun radius ratio " << printTime(time,"%4F %10.3g ")
            << fixed << setprecision(9) << REoRS;
   LOG(DEBUG7) << "Sun/Earth mass ratio " << printTime(time,"%4F %10.3g ")
            << fixed << setprecision(3)
      << sse.getConstant("GMS")*((1.0+EMRAT)/EMRAT)/sse.getConstant("GMB");
   LOG(DEBUG7) << "Earth/Moon radius ratio " << printTime(time,"%4F %10.3g ")
            << fixed << setprecision(9) << REoRM;
   LOG(DEBUG7) << "Sunfactor " << printTime(time,"%4F %10.3g ")
            << fixed << setprecision(9) << sunFactor;
   LOG(DEBUG7) << "Moonfactor " << printTime(time,"%4F %10.3g ")
            << fixed << setprecision(9) << moonFactor;

   // dot products
   sunDOTrx = sunUnit.dot(rx);
   moonDOTrx = moonUnit.dot(rx);

   // transverse to radial direction - not unit vectors
   tSun = sunUnit - sunDOTrx * rx;
   tMoon = moonUnit - moonDOTrx * rx;

   disp = Triple(0,0,0);

   // Steps refer to IERS(1996) table on page 60.
   // Step 1a IERS(1996) eq. (8) pg 61.
   // nominal degree 2 Love and Shida numbers pg 60
   double poly = sinlat;
   poly = (3.0*poly*poly-1.0)/2.0;
   Love = 0.6026 - 0.0006*poly;
   Shida = 0.0831 + 0.0002*poly;

   tmp = sunFactor*(Love*(1.5*sunDOTrx*sunDOTrx-0.5)*rx + 3.0*Shida*sunDOTrx*tSun);
   for(i=0; i<3; i++) disp[i] += tmp[i];

   if(debug) {
      tmpNEU[0] = north[0]*tmp[0] + north[1]*tmp[1] + north[2]*tmp[2];
      tmpNEU[1] =  east[0]*tmp[0] +  east[1]*tmp[1] +  east[2]*tmp[2];
      tmpNEU[2] =    up[0]*tmp[0] +    up[1]*tmp[1] +    up[2]*tmp[2];
      LOG(DEBUG7) << " 2sunTerm(NEU) " << printTime(time,"%4F %10.3g ")
               << fixed << setprecision(9)
               << tmpNEU[0] << " " << tmpNEU[1] << " " << tmpNEU[2];
   }

   tmp = moonFactor*(Love*(1.5*moonDOTrx*moonDOTrx-0.5)*rx+3.0*Shida*moonDOTrx*tMoon);
   for(i=0; i<3; i++) disp[i] += tmp[i];

   if(debug) {
      tmpNEU[0] = north[0]*tmp[0] + north[1]*tmp[1] + north[2]*tmp[2];
      tmpNEU[1] =  east[0]*tmp[0] +  east[1]*tmp[1] +  east[2]*tmp[2];
      tmpNEU[2] =    up[0]*tmp[0] +    up[1]*tmp[1] +    up[2]*tmp[2];
      LOG(DEBUG7) << " 2moonTerm(NEU) " << printTime(time,"%4F %10.3g ")
               << fixed << setprecision(9)
               << tmpNEU[0] << " " << tmpNEU[1] << " " << tmpNEU[2];
   }

   // Step 1b IERS(1996) eq. (9) pg 61.
   // nominal degree 3 Love and Shida numbers pg 60
   Love = 0.292;
   Shida = 0.015;
   tmp = sunFactor*REoRS*(Love*(2.5*sunDOTrx*sunDOTrx - 1.5)*sunDOTrx * rx
                         + Shida*(7.5*sunDOTrx*sunDOTrx - 1.5) * tSun);
   for(i=0; i<3; i++) disp[i] += tmp[i];

   if(debug) {
      tmpNEU[0] = north[0]*tmp[0] + north[1]*tmp[1] + north[2]*tmp[2];
      tmpNEU[1] =  east[0]*tmp[0] +  east[1]*tmp[1] +  east[2]*tmp[2];
      tmpNEU[2] =    up[0]*tmp[0] +    up[1]*tmp[1] +    up[2]*tmp[2];
      LOG(DEBUG7) << " 3sunTerm(NEU) " << printTime(time,"%4F %10.3g ")
               << fixed << setprecision(9)
               << tmpNEU[0] << " " << tmpNEU[1] << " " << tmpNEU[2];
   }

   tmp = moonFactor*REoRM*(Love*(2.5*moonDOTrx*moonDOTrx - 1.5)*moonDOTrx * rx
                         + Shida*(7.5*moonDOTrx*moonDOTrx - 1.5) * tMoon);
   for(i=0; i<3; i++) disp[i] += tmp[i];

   if(debug) {
      tmpNEU[0] = north[0]*tmp[0] + north[1]*tmp[1] + north[2]*tmp[2];
      tmpNEU[1] =  east[0]*tmp[0] +  east[1]*tmp[1] +  east[2]*tmp[2];
      tmpNEU[2] =    up[0]*tmp[0] +    up[1]*tmp[1] +    up[2]*tmp[2];
      LOG(DEBUG7) << " 3moonTerm(NEU) " << printTime(time,"%4F %10.3g ")
               << fixed << setprecision(9)
               << tmpNEU[0] << " " << tmpNEU[1] << " " << tmpNEU[2];
   }

   // all of 8 and 9
   if(debug) {
      tmp = disp;
      tmpNEU[0] = north[0]*tmp[0] + north[1]*tmp[1] + north[2]*tmp[2];
      tmpNEU[1] =  east[0]*tmp[0] +  east[1]*tmp[1] +  east[2]*tmp[2];
      tmpNEU[2] =    up[0]*tmp[0] +    up[1]*tmp[1] +    up[2]*tmp[2];
      LOG(DEBUG7) << " eqn8+9(NEU) " << printTime(time,"%4F %10.3g ")
               << fixed << setprecision(9)
               << tmpNEU[0] << " " << tmpNEU[1] << " " << tmpNEU[2];
   }

   // Step 1c IERS(1996) eq. (13) pg 63. diurnal tides
   Love = -0.0025;
   Shida = -0.0007;
   tmp = -0.75*Love*::sin(2*lat) * (                                  // radial 13a
              sunFactor*::sin(2*latSun)*::sin(lon-lonSun)
           + moonFactor*::sin(2*latMoon)*::sin(lon-lonMoon)) * rx
           
          - 1.5*Shida*::cos(2*lat) * (                                 // north 13b
              sunFactor*::sin(2*latSun)*::sin(lon-lonSun)
           + moonFactor*::sin(2*latMoon)*::sin(lon-lonMoon)) * north

          - 1.5*Shida*sinlat * (                                       // east 13b
              sunFactor*::sin(2*latSun)*::cos(lon-lonSun)
           + moonFactor*::sin(2*latMoon)*::cos(lon-lonMoon)) * east;
   for(i=0; i<3; i++) disp[i] += tmp[i];

   if(debug) {
      tmpNEU[0] = north[0]*tmp[0] + north[1]*tmp[1] + north[2]*tmp[2];
      tmpNEU[1] =  east[0]*tmp[0] +  east[1]*tmp[1] +  east[2]*tmp[2];
      tmpNEU[2] =    up[0]*tmp[0] +    up[1]*tmp[1] +    up[2]*tmp[2];
      LOG(DEBUG7) << " diurnal-band(NEU) " << printTime(time,"%4F %10.3g ")
               << fixed << setprecision(9)
               << tmpNEU[0] << " " << tmpNEU[1] << " " << tmpNEU[2];
   }

   // Step 1d IERS(1996) eq. (14) pg 63. semidiurnal tides
   Love = -0.0022;
   Shida = -0.0007;
   tmp = -0.75*Love*coslat*coslat * (                                 // radial 14a
               sunFactor*::cos(latSun)*::cos(latSun)*::sin(2*(lon-lonSun))
          + moonFactor*::cos(latMoon)*::cos(latMoon)*::sin(2*(lon-lonMoon))) * rx

         + 0.75*Shida*::sin(2*lat) * (                                  // north 14b
               sunFactor*::cos(latSun)*::cos(latSun)*::sin(2*(lon-lonSun))
          + moonFactor*::cos(latMoon)*::cos(latMoon)*::sin(2*(lon-lonMoon))) * north

         - 1.50*Shida*coslat * (                                        // east 14b
               sunFactor*::cos(latSun)*::cos(latSun)*::cos(2*(lon-lonSun))
          + moonFactor*::cos(latMoon)*::cos(latMoon)*::cos(2*(lon-lonMoon))) * east;
   for(i=0; i<3; i++) disp[i] += tmp[i];

   if(debug) {
      tmpNEU[0] = north[0]*tmp[0] + north[1]*tmp[1] + north[2]*tmp[2];
      tmpNEU[1] =  east[0]*tmp[0] +  east[1]*tmp[1] +  east[2]*tmp[2];
      tmpNEU[2] =    up[0]*tmp[0] +    up[1]*tmp[1] +    up[2]*tmp[2];
      LOG(DEBUG7) << " semi-diurnal-band(NEU) " << printTime(time,"%4F %10.3g ")
               << fixed << setprecision(9)
               << tmpNEU[0] << " " << tmpNEU[1] << " " << tmpNEU[2];
   }

   // Step 1e IERS(1996) eq. (11) pg 62. latitude dependence of diurnal band
   Shida = 0.0012;
   tmp = - 3.0*Shida*sinlat*sinlat * (                                  // north
               sunFactor*::cos(latSun)*::sin(latSun)*::cos(lon-lonSun)
            + moonFactor*::cos(latMoon)*::sin(latMoon)*::cos(lon-lonMoon)) * north

         + 3.0*Shida*sinlat*::cos(2*lat) * (                            // east
               sunFactor*::cos(latSun)*::sin(latSun)*::sin(lon-lonSun)
            + moonFactor*::cos(latMoon)*::sin(latMoon)*::sin(lon-lonMoon)) * east;
   for(i=0; i<3; i++) disp[i] += tmp[i];

   if(debug) {
      tmpNEU[0] = north[0]*tmp[0] + north[1]*tmp[1] + north[2]*tmp[2];
      tmpNEU[1] =  east[0]*tmp[0] +  east[1]*tmp[1] +  east[2]*tmp[2];
      tmpNEU[2] =    up[0]*tmp[0] +    up[1]*tmp[1] +    up[2]*tmp[2];
      LOG(DEBUG7) << " lat-dep-diurnal-band(NEU) " << printTime(time,"%4F %10.3g ")
               << fixed << setprecision(9)
               << tmpNEU[0] << " " << tmpNEU[1] << " " << tmpNEU[2];
   }

   // Step 1f IERS(1996) eq. (12) pg 62. semidiurnal band
   Shida = 0.0024;
   tmp = - 1.5*Shida*sinlat*coslat * (                                 // north
             sunFactor*::cos(latSun)*::cos(latSun)*::cos(2*(lon-lonSun))
          + moonFactor*::cos(latMoon)*::cos(latMoon)*::cos(2*(lon-lonMoon))) * north

         - 1.5*Shida*sinlat*sinlat*coslat * (                            // east
             sunFactor*::cos(latSun)*::cos(latSun)*::sin(2*(lon-lonSun))
          + moonFactor*::cos(latMoon)*::cos(latMoon)*::sin(2*(lon-lonMoon))) * east;
   for(i=0; i<3; i++) disp[i] += tmp[i];

   if(debug) {
      tmpNEU[0] = north[0]*tmp[0] + north[1]*tmp[1] + north[2]*tmp[2];
      tmpNEU[1] =  east[0]*tmp[0] +  east[1]*tmp[1] +  east[2]*tmp[2];
      tmpNEU[2] =    up[0]*tmp[0] +    up[1]*tmp[1] +    up[2]*tmp[2];
      LOG(DEBUG7) << " lat-dep-semi-diurnal-band(NEU) " << printTime(time,"%4F %10.3g ")
               << fixed << setprecision(9)
               << tmpNEU[0] << " " << tmpNEU[1] << " " << tmpNEU[2];
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

   double fhr,t,s,tau,pr,h,p,zns,ps;
   double fmjd = static_cast<MJD>(time).mjd+51.184/86400.0;
   t = (fmjd-51545.0)/36525.0;
   fhr = (fmjd-int(fmjd))*24.0;
   computeArgs(t, fhr, s, tau, pr, h, p, zns, ps);

   for(i=0; i<3; i++) tmp[i] = 0.0;
   double thetaf,dr,dn,de;
   for(i=0; i<31; i++) {
      thetaf = (tau + step2diurnalData[0+9*i] * s
                    + step2diurnalData[1+9*i] * h
                    + step2diurnalData[2+9*i] * p
                    + step2diurnalData[3+9*i] * zns
                    + step2diurnalData[4+9*i] * ps) * DEG_TO_RAD;
      dr = (step2diurnalData[5+9*i] * ::sin(thetaf+lon)
           +step2diurnalData[6+9*i] * ::cos(thetaf+lon)) * 2 * sinlat * coslat;
      dn = (step2diurnalData[7+9*i] * ::sin(thetaf+lon)
           +step2diurnalData[8+9*i] * ::cos(thetaf+lon))
                                           * (coslat*coslat - sinlat*sinlat);
      de = (step2diurnalData[7+9*i] * ::cos(thetaf+lon)
           +step2diurnalData[8+9*i] * ::sin(thetaf+lon)) * sinlat;
      tmp[0] += dr*up[0] + de*east[0] + dn*north[0];
      tmp[1] += dr*up[1] + de*east[1] + dn*north[1];
      tmp[2] += dr*up[2]              + dn*north[2];
   }

   // convert total to meters
   for(i=0; i<3; i++) tmp[i] /= 1000.0;   // mm -> m
   for(i=0; i<3; i++) disp[i] += tmp[i];

   if(debug) {
      tmpNEU[0] = north[0]*tmp[0] + north[1]*tmp[1] + north[2]*tmp[2];
      tmpNEU[1] =  east[0]*tmp[0] +  east[1]*tmp[1] +  east[2]*tmp[2];
      tmpNEU[2] =    up[0]*tmp[0] +    up[1]*tmp[1] +    up[2]*tmp[2];
      LOG(DEBUG7) << " step2diu(NEU) " << printTime(time,"%4F %10.3g ")
               << fixed << setprecision(9)
               << tmpNEU[0] << " " << tmpNEU[1] << " " << tmpNEU[2];
   }
   
   //// Ref. Kouba 2009 and IERS technical note 3, 1989 (out of print)
   //// Kouba (personal communication 8/12/09) : not necessary unless using IERS(1989)
   //double Tg;        // Greenwhich sidereal time
   //Tg = time.JD()-2451545.;
   //if(Tg <= 0.0) Tg -= 1.0;
   //Tg /= 36525.;
   //Tg = 0.279057273264 + 100.0021390378009*Tg
   //                    + (0.093104 - 6.2e-6*Tg)*Tg*Tg/86400.0;
   //Tg += time.secOfDay()/86400.0;                      // days
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

   for(i=0; i<3; i++) tmp[i] = 0.0;
   for(i=0; i<5; i++) {
      thetaf = (tau + step2longData[0+9*i] * s
                    + step2longData[1+9*i] * h
                    + step2longData[2+9*i] * p
                    + step2longData[3+9*i] * zns
                    + step2longData[4+9*i] * ps) * DEG_TO_RAD;
      dr = (step2longData[5+9*i] * ::cos(thetaf)
           +step2longData[7+9*i] * ::sin(thetaf+lon)) * (3*sinlat*sinlat-1);
      dn = (step2longData[6+9*i] * ::cos(thetaf+lon)
           +step2longData[8+9*i] * ::sin(thetaf+lon)) * 2*sinlat*coslat;
      de = 0.0;
      tmp[0] += dr*up[0] + de*east[0] + dn*north[0];
      tmp[1] += dr*up[1] + de*east[1] + dn*north[1];
      tmp[2] += dr*up[2]              + dn*north[2];
   }
   for(i=0; i<3; i++) tmp[i] /= 1000.0;   // mm -> m
   for(i=0; i<3; i++) disp[i] += tmp[i];

   LOG(DEBUG7) << " step2lon(NEU) " << printTime(time,"%4F %10.3g ")
            << fixed << setprecision(9)
              << dn/1000.0 << " " << de/1000.0 << " " << dr/1000.0;

   // remove permanent deformation IERS(1996) eq. 17 pg 65.
   // This term must NOT be included in order to be consistent with ITRF (ref Kouba)
   //tmp = -0.1196*(1.5*sinlat*sinlat-0.5)*rx - 0.0247*::sin(2*lat)*north;
   //for(i=0; i<3; i++) disp[i] += tmp[i];

   if(debug) {
      tmp = -0.1196*(1.5*sinlat*sinlat-0.5)*rx - 0.0247*::sin(2*lat)*north;
      tmpNEU[0] = north[0]*tmp[0] + north[1]*tmp[1] + north[2]*tmp[2];
      tmpNEU[1] =  east[0]*tmp[0] +  east[1]*tmp[1] +  east[2]*tmp[2];
      tmpNEU[2] =    up[0]*tmp[0] +    up[1]*tmp[1] +    up[2]*tmp[2];
      LOG(DEBUG7) << " permanent(NEU) " << fixed << setprecision(9)
               << tmpNEU[0] << " " << tmpNEU[1] << " " << tmpNEU[2];

      for(i=0; i<3; i++) tmp[i] = disp[i];
      tmpNEU[0] = north[0]*tmp[0] + north[1]*tmp[1] + north[2]*tmp[2];
      tmpNEU[1] =  east[0]*tmp[0] +  east[1]*tmp[1] +  east[2]*tmp[2];
      tmpNEU[2] =    up[0]*tmp[0] +    up[1]*tmp[1] +    up[2]*tmp[2];
      LOG(DEBUG7) << " totalSET(NEU) " << printTime(time,"%4F %10.3g ")
               << fixed << setprecision(9)
               << tmpNEU[0] << " " << tmpNEU[1] << " " << tmpNEU[2];

   }

   return disp;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void computeArgs(const double t, const double fhr, double& s, double& tau, double& pr,
                 double& h, double& p, double& zns, double& ps) throw()
{
   double t2 = t*t;
   double t3 = t2*t;
   double t4 = t3*t;
   s = 218.31664563 + 481267.88194*t - 0.0014663889*t2 + 0.00000185139*t3;
   tau = fhr*15. + 280.4606184 + 36000.7700536*t + 0.00038793*t2 - 0.0000000258*t3;
   tau = tau - s;
   pr = 1.396971278*t + 0.000308889*t2 + 0.000000021*t3 + 0.000000007*t4;
   s = s + pr;
   h = 280.46645 + 36000.7697489*t + 0.00030322222*t2 + 0.000000020*t3
                                          - 0.00000000654*t4;
   p = 83.35324312 + 4069.01363525*t - 0.01032172222*t2 - 0.0000124991*t3
                                          + 0.00000005263*t4;
   zns = 234.95544499  + 1934.13626197*t - 0.00207561111*t2 - 0.00000213944*t3
                                          + 0.00000001650*t4;
   ps = 282.93734098 + 1.71945766667*t + 0.00045688889*t2 - 0.00000001778*t3
                                          - 0.00000000334*t4;
   s   = fmod(s,  360.0);
   tau = fmod(tau,360.0);
   h   = fmod(h,  360.0);
   p   = fmod(p,  360.0);
   zns = fmod(zns,360.0);
   ps  = fmod(ps, 360.0);
}

//------------------------------------------------------------------------------------
/// Compute the site displacement due to rotational deformation due to polar motion
/// for the given Position (assumed to fixed to the solid Earth) at the given time.
/// Return a Triple containing the site displacement in WGS84 ECEF XYZ coordinates
/// with units meters. Reference IERS Conventions (1996) found in IERS Technical
/// Note 21 (IERS), ch. 7 page 67.
/// @param  Position site        Nominal position of the site of interest.
/// @param  CommonTime time         Time of interest.
/// @param  EarthOrientation eo  Earth orientation parameters appropriate for time.
/// @return Triple disp          Displacement vector, WGS84 ECEF XYZ meters.
gpstk::Triple computePolarTides(gpstk::Position site,
                                gpstk::CommonTime time,
                                gpstk::EarthOrientation& eo)
   throw(gpstk::Exception)
{
try {
   double lat, lon, theta, sinlat, coslat, sinlon, coslon;
   Triple disp, dispXYZ;

   lat = site.getGeocentricLatitude();
   lon = site.getLongitude();
   sinlat = ::sin(lat*DEG_TO_RAD);
   coslat = ::cos(lat*DEG_TO_RAD);
   sinlon = ::sin(lon*DEG_TO_RAD);
   coslon = ::cos(lon*DEG_TO_RAD);
   theta = (90.0-lat)*DEG_TO_RAD;

   // NEU components - IERS(1996) pg 67, eqn. 22 (in eqn 22, r==Up, theta=S, lambda=E)
   disp[0] = -0.009 * ::cos(2*theta) * (eo.xp * coslon - eo.yp * sinlon);  // -S = N
   disp[1] = -0.009 * ::cos(theta) * (eo.xp * sinlon + eo.yp * coslon);    // E
   disp[2] =  0.032 * ::sin(2*theta) * (eo.xp * coslon - eo.yp * sinlon);  // U

   LOG(DEBUG7) << " poletide(NEU) " << printTime(time,"%4F %10.3g ")
               << fixed << setprecision(9)
               << disp[0] << " " << disp[1] << " " << disp[2];

   //// transform  X=(x,y,z) into (R*X)(north,east,up)
   //R(0,0) = -sa*co;  R(0,1) = -sa*so;  R(0,2) = ca;
   //R(1,0) =    -so;  R(1,1) =     co;  R(1,2) = 0.;
   //R(2,0) =  ca*co;  R(2,1) =  ca*so;  R(2,2) = sa;

   dispXYZ[0] = - sinlat*coslon*disp[0] - sinlat*sinlon*disp[1] + coslat*disp[2];
   dispXYZ[1] =        - sinlon*disp[0] +        coslon*disp[1];
   dispXYZ[2] =   coslat*coslon*disp[0] + coslat*sinlon*disp[1] + sinlat*disp[2];

   return dispXYZ;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
