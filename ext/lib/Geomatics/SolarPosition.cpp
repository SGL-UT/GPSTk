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
 * @file SolarPosition.cpp  Compute solar and lunar positions with a simple algorithm.
 */

//------------------------------------------------------------------------------------
// includes
// system
// GPSTk
#include "CommonTime.hpp"
#include "Position.hpp"
#include "GNSSconstants.hpp"       // TWO_PI
#include "GNSSconstants.hpp"                // DEG_TO_RAD
#include "SolarPosition.hpp"
#include "YDSTime.hpp"
#include "JulianDate.hpp"

namespace gpstk {
using namespace std;

//------------------------------------------------------------------------------------
// Compute Greenwich Mean Sidereal Time in degrees
static double GMST(gpstk::CommonTime t)
{

   static const long JulianEpoch=2451545;
   // days since epoch
   double days =( static_cast<JulianDate>(t).jd - JulianEpoch);                         // days
   if(days <= 0.0) days -= 1.0;
   double Tp = days/36525.0;                                   // dim-less

      // Compute GMST in radians
   double G;
   // seconds (24060s = 6h 41min)
   //G = 24110.54841 + (8640184.812866 + (0.093104 - 6.2e-6*Tp)*Tp)*Tp;   // sec
   //G /= 86400.0; // instead, divide the numbers above manually
   G = 0.279057273264 + 100.0021390378009*Tp        // seconds/86400 = days
                      + (0.093104 - 6.2e-6*Tp)*Tp*Tp/86400.0;
   G += static_cast<YDSTime>(t).sod/86400.0;                      // days

   // put answer between 0 and 360 deg
   G = fmod(G,1.0);
   while(G < 0.0) G += 1.0;
   G *= 360.0;                                                 // degrees

   return G;
}



//------------------------------------------------------------------------------------
// accuracy is about 1 arcminute, when t is within 2 centuries of 2000.
// Ref. Astronomical Almanac pg C24, as presented on USNO web site.
// input
//    t             epoch of interest
// output
//    lat,lon,R     latitude, longitude and distance (deg,deg,m in ECEF) of sun at t.
//    AR            apparent angular radius of sun as seen at Earth (deg) at t.
Position SolarPosition(CommonTime t, double& AR) throw()
{
   //const double mPerAU = 149598.0e6;
   double D;     // days since J2000
   double g,q;   // q is mean longitude of sun, corrected for aberration
   double L;     // sun's geocentric apparent ecliptic longitude (deg)
   //double b=0; // sun's geocentric apparent ecliptic latitude (deg)
   double e;     // mean obliquity of the ecliptic (deg)
   //double R;   // sun's distance from Earth (m)
   double RA;    // sun's right ascension (deg)
   double DEC;   // sun's declination (deg)
   //double AR;  // sun's apparent angular radius as seen at Earth (deg)

   D = static_cast<JulianDate>(t).jd - 2451545.0;
   g = (357.529 + 0.98560028 * D) * DEG_TO_RAD;
   // AA 1990 has g = (357.528 + 0.9856003 * D) * DEG_TO_RAD;
   q = 280.459 + 0.98564736 * D;
   // AA 1990 has q = 280.460 + 0.9856474 * D;
   L = (q + 1.915 * std::sin(g) + 0.020 * std::sin(2*g)) * DEG_TO_RAD;

   e = (23.439 - 0.00000036 * D) * DEG_TO_RAD;
   // AA 1990 has e = (23.439 - 0.0000004 * D) * DEG_TO_RAD;
   RA = std::atan2(std::cos(e)*std::sin(L),std::cos(L)) * RAD_TO_DEG;
   DEC = std::asin(std::sin(e)*std::sin(L)) * RAD_TO_DEG;

   // equation of time = apparent solar time minus mean solar time
   // = [q-RA (deg)]/(15deg/hr)

   // compute the hour angle of the vernal equinox = GMST and convert RA to lon
   double lon = fmod(RA-GMST(t),360.0);
   if(lon < -180.0) lon += 360.0;
   if(lon >  180.0) lon -= 360.0;

   double lat = DEC;

   // ECEF unit vector in direction Earth to sun
   double xhat = std::cos(lat*DEG_TO_RAD)*std::cos(lon*DEG_TO_RAD);
   double yhat = std::cos(lat*DEG_TO_RAD)*std::sin(lon*DEG_TO_RAD);
   double zhat = std::sin(lat*DEG_TO_RAD);

   // R in AU
   double R = 1.00014 - 0.01671 * std::cos(g) - 0.00014 * std::cos(2*g);
   // apparent angular radius in degrees
   AR = 0.2666/R;
   // convert to meters
   R *= 149598.0e6;

   Position es;
   es.setECEF(R*xhat,R*yhat,R*zhat);
   return es;
}

//------------------------------------------------------------------------------------
// Compute the position (latitude and longitude, in degrees) of the sun
// given the day of year and the hour of the day.
// Adapted from sunpos by D. Coco 12/15/94
void CrudeSolarPosition(CommonTime t, double& lat, double& lon) throw()
{
   int doy = static_cast<YDSTime>(t).doy;
   int hod = int(static_cast<YDSTime>(t).sod/3600.0 + 0.5);
   lat = std::sin(23.5*DEG_TO_RAD)*std::sin(TWO_PI*double(doy-83)/365.25);
   lat = lat / std::sqrt(1.0-lat*lat);
   lat = RAD_TO_DEG*std::atan(lat);
   lon = 180.0 - hod*15.0;
}

//------------------------------------------------------------------------------------
// Consider the sun and the earth as seen from the satellite. Let the sun be a circle
// of angular radius r, center in direction s, and the earth be a (larger) circle
// of angular radius R, center in direction e. The circles overlap if |e-s| < R+r;
// complete overlap if |e-s| < R. Let L == |e-s|.
//    What is the area of overlap if R-r < L < R+r ?
// Call the two points where the circles intersect p1 and p2. Draw a line from e to s;
// call the points where this line intersects the two circles r1 and R1, respectively.
// Draw lines from e to s, e to p1, e to p2, s to p1 and s to p2. Call the angle
// between e-s and e-p1 alpha, and that between s-e and s-p1, beta.
// Draw a rectangle with top and bottom parallel to e-s passing through p1 and p2,
// and with sides passing through s and r1. Similarly for e and R1. Note that the
// area of intersection lies within the intersection of these two rectangles.
// Call the area of the rectangle outside the circles A and B. The height H of the
// rectangles is
// H = 2Rsin(alpha) = 2rsin(beta)
// also L = rcos(beta)+Rcos(alpha)
// The area A will be the area of the rectangle
//              minus the area of the wedge formed by the angle 2*alpha
//              minus the area of the two triangles which meet at s :
// A = RH - (2alpha/2pi)*pi*R*R - 2*(1/2)*(H/2)Rcos(alpha)
// Similarly
// B = rH - (2beta/2pi)*pi*r*r  - 2*(1/2)*(H/2)rcos(beta)
// The area of intersection will be the area of the rectangular intersection
//                            minus the area A
//                            minus the area B
// Intersection = H(R+r-L) - A - B
//              = HR+Hr-HL -HR+alpha*R*R+(H/2)Rcos(alpha) -Hr+beta*r*r+(H/2)rcos(beta)
// Cancel terms, and substitute for L using above equation L = ..
//              = -(H/2)rcos(beta)-(H/2)Rcos(alpha)+alpha*R*R+beta*r*r
// substitute for H/2
//              = -R*R*sin(alpha)cos(alpha)-r*r*sin(beta)cos(beta)+alpha*R*R+beta*r*r
// Intersection = R*R*[alpha-sin(alpha)cos(alpha)]+r*r*[beta-sin(beta)cos(beta)]
// Solve for alpha and beta in terms of R, r and L using the H and L relations above
// (r/R)cos(beta)=(L/R)-cos(alpha)
// (r/R)sin(beta)=sin(alpha)
// so
// (r/R)^2 = (L/R)^2 - (2L/R)cos(alpha) + 1
// cos(alpha) = (R/2L)(1+(L/R)^2-(r/R)^2)
// cos(beta) = (L/r) - (R/r)cos(alpha)
// and 0 <= alpha or beta <= pi
//
// Rearth    angular radius of the earth as seen at the satellite
// Rsun      angular radius of the sun as seen at the satellite
// dES       angular distance of the sun from the earth
// return    fraction (0 <= f <= 1) of area of sun covered by earth
// units only need be consistent
double shadowFactor(double Rearth, double Rsun, double dES) throw()
{
   if(dES >= Rearth+Rsun) return 0.0;
   if(dES <= std::fabs(Rearth-Rsun)) return 1.0;
   double r=Rsun, R=Rearth, L=dES;
   if(Rsun > Rearth) { r=Rearth; R=Rsun; }
   double cosalpha = (R/L)*(1.0+(L/R)*(L/R)-(r/R)*(r/R))/2.0;
   double cosbeta = (L/r) - (R/r)*cosalpha;
   double sinalpha = ::sqrt(1-cosalpha*cosalpha);
   double sinbeta = ::sqrt(1-cosbeta*cosbeta);
   double alpha = ::asin(sinalpha);
   double beta = ::asin(sinbeta);
   double shadow = r*r*(beta-sinbeta*cosbeta)+R*R*(alpha-sinalpha*cosalpha);
   shadow /= ::acos(-1.0)*Rsun*Rsun;
   return shadow;
}

//------------------------------------------------------------------------------------
// From AA 1990 D46
Position LunarPosition(CommonTime t, double& AR) throw()
{
   // days since J2000
   double N = static_cast<JulianDate>(t).jd-2451545.0;
   // centuries since J2000
   double T = N/36525.0;
   // ecliptic longitude
   double lam = DEG_TO_RAD*(218.32 + 481267.883*T
              + 6.29 * ::sin(DEG_TO_RAD*(134.9+477198.85*T))
              - 1.27 * ::sin(DEG_TO_RAD*(259.2-413335.38*T))
              + 0.66 * ::sin(DEG_TO_RAD*(235.7+890534.23*T))
              + 0.21 * ::sin(DEG_TO_RAD*(269.9+954397.70*T))
              - 0.19 * ::sin(DEG_TO_RAD*(357.5+ 35999.05*T))
              - 0.11 * ::sin(DEG_TO_RAD*(259.2+966404.05*T)));
   // ecliptic latitude
   double bet = DEG_TO_RAD*(5.13 * ::sin(DEG_TO_RAD*( 93.3+483202.03*T))
                          + 0.28 * ::sin(DEG_TO_RAD*(228.2+960400.87*T))
                          - 0.28 * ::sin(DEG_TO_RAD*(318.3+  6003.18*T))
                          - 0.17 * ::sin(DEG_TO_RAD*(217.6-407332.20*T)));
   // horizontal parallax
   double par = DEG_TO_RAD*(0.9508
              + 0.0518 * ::cos(DEG_TO_RAD*(134.9+477198.85*T))
              + 0.0095 * ::cos(DEG_TO_RAD*(259.2-413335.38*T))
              + 0.0078 * ::cos(DEG_TO_RAD*(235.7+890534.23*T))
              + 0.0028 * ::cos(DEG_TO_RAD*(269.9+954397.70*T)));

   // obliquity of the ecliptic
   double eps = (23.439 - 0.00000036 * N) * DEG_TO_RAD;

   // convert ecliptic lon,lat to geocentric lon,lat
   double l = ::cos(bet)*::cos(lam);
   double m = ::cos(eps)*::cos(bet)*::sin(lam) - ::sin(eps)*::sin(bet);
   double n = ::sin(eps)*::cos(bet)*::sin(lam) + ::cos(eps)*::sin(bet);

   // convert to right ascension and declination,
   // (referred to mean equator and equinox of date)
   double RA = ::atan2(m,l) * RAD_TO_DEG;
   double DEC = ::asin(n) * RAD_TO_DEG;

   // compute the hour angle of the vernal equinox = GMST and convert RA to lon
   double lon = ::fmod(RA-GMST(t),360.0);
   if(lon < -180.0) lon += 360.0;
   if(lon >  180.0) lon -= 360.0;

   double lat = DEC;

   // apparent semidiameter of moon (in radians)
   AR = 0.2725 * par;
   // moon distance in meters
   double R = 1.0 / ::sin(par);
   R *= 6378137.0;

   // ECEF vector in direction Earth to moon
   double x = R*std::cos(lat*DEG_TO_RAD)*std::cos(lon*DEG_TO_RAD);
   double y = R*std::cos(lat*DEG_TO_RAD)*std::sin(lon*DEG_TO_RAD);
   double z = R*std::sin(lat*DEG_TO_RAD);

   Position EM;
   EM.setECEF(x,y,z);

   return EM;
}

//------------------------------------------------------------------------------------
} // end namespace gpstk
