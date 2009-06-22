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
 * @file PhaseWindup.cpp
 * Implement computations of phase windup, solar ephemeris, satellite attitude
 * and eclipse at the satellite.
 */
 
// -----------------------------------------------------------------------------------
// GPSTk includes
#include "StringUtils.hpp"
#include "geometry.hpp"             // DEG_TO_RAD
#include "icd_200_constants.hpp"    // TWO_PI
//
#include "PhaseWindup.hpp"

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk {

// -----------------------------------------------------------------------------------
// prototypes for functions used in this module only
void SolarPosition(DayTime t, double& lat, double& lon, double& R, double& AR);
Matrix<double> SatelliteAttitude(DayTime& tt, Position& SV, double& sf);
double shadowFactor(double Rearth, double Rsun, double dES);
static double GMST(DayTime t);

// -----------------------------------------------------------------------------------
// Given a Position, compute unit (ECEF) vectors in the Up, East and North directions
// at that position. Use geodetic coordinates, i.e. 'up' is perpendicular to the
// geoid. Return the vectors in the form of a 3x3 Matrix<double>, this is in fact the
// rotation matrix that will take an ECEF vector into an 'up-east-north' vector.
Matrix<double> UpEastNorth(Position& P) throw(Exception)
{
try {
   Matrix<double> R(3,3);
   P.transformTo(Position::Geodetic);

   double lat = P.getGeodeticLatitude() * DEG_TO_RAD;      // rad N
   double lon = P.getLongitude() * DEG_TO_RAD;             // rad E
   double ca = ::cos(lat);
   double sa = ::sin(lat);
   double co = ::cos(lon);
   double so = ::sin(lon);

   // This is the rotation matrix which will take X=(x,y,z) into (R*X)(up,east,north)
   R(0,0) =  ca*co;  R(0,1) =  ca*so;  R(0,2) = sa;
   R(1,0) =    -so;  R(1,1) =     co;  R(1,2) = 0.;
   R(2,0) = -sa*co;  R(2,1) = -sa*so;  R(2,2) = ca;

   // The rows of R are also the unit vectors, in ECEF, of up,east,north;
   //  R = (U && E && N) = transpose(U || E || N).
   //Vector<double> U = R.rowCopy(0);
   //Vector<double> E = R.rowCopy(1);
   //Vector<double> N = R.rowCopy(2);

   return R;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

// -----------------------------------------------------------------------------------
// Generate a 3x3 rotation Matrix, for direct rotations about one axis
// (for XYZ, axis=123), given the rotation angle in radians;
// @param angle in radians.
// @param axis 1,2,3 as rotation about X,Y,Z.
// @return Rotation matrix (3x3).
// @throw InvalidInput if axis is anything other than 1, 2 or 3.
Matrix<double> SingleAxisRotation(double angle,
                                  int axis)
   throw(Exception)
{
try {
   if(axis < 1 || axis > 3) {
      Exception e(string("Invalid axis (1,2,3 <=> X,Y,Z): ") + asString(axis));
      GPSTK_THROW(e);
   }
   Matrix<double> R(3,3,0.0);

   int i1=axis-1;                      // axis = 1 : 0,1,2
   int i2=i1+1; if(i2 == 3) i2=0;      // axis = 2 : 1,2,0
   int i3=i2+1; if(i3 == 3) i3=0;      // axis = 3 : 2,0,1

   R(i1,i1) = 1.0;
   R(i2,i2) = R(i3,i3) = ::cos(angle);
   R(i3,i2) = -(R(i2,i3) = ::sin(angle));

   return R;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

// -----------------------------------------------------------------------------------
// Compute the satellite attitude, given the time and the satellite position SV.
// Return a 3x3 Matrix which contains, as rows, the unit (ECEF) vectors X,Y,Z in the
// body frame of the satellite, namely
//    Z = along the boresight (i.e. towards Earth center),
//    Y = perpendicular to both Z and the satellite-sun direction, and
//    X completing the orthonormal triad. X will generally point toward the sun.
// Also return the shadow factor = fraction of sun's area not visible to satellite.
Matrix<double> SatelliteAttitude(DayTime& tt, Position& SV, double& sf)
{
try {
   int i;
   double d,svrange,lat,lon,DistSun,Radsun,Radearth,dES;
   Position X,Y,Z,T;
   Matrix<double> R(3,3);

   // Z points from satellite to Earth center - along the antenna boresight
   Z = SV;
   Z.transformTo(Position::Cartesian);
   svrange = Z.mag();
   d = -1.0/Z.mag();
   Z = d * Z;                                // reverse and normalize Z

   // T points from satellite to sun
   SolarPosition(tt, lat, lon, DistSun, Radsun);
   Radsun *= DEG_TO_RAD;                     // angular radius of sun at satellite
   Radearth = ::asin(6378137.0/svrange);     // angular radius of earth at sat

   T.setGeocentric(lat,lon,DistSun);         // vector earth to sun
   T.transformTo(Position::Cartesian);
   T = T - SV;                               // sat to sun=(E to sun)-(E to sat)
   d = 1.0/T.mag();
   T = d * T;                                // normalize T

   dES = ::acos(Z.dot(T));                   // apparent angular distance, earth
                                             // to sun, as seen at satellite

   sf = shadowFactor(Radearth, Radsun, dES); // is satellite in eclipse?
   //if(sf > 0.999) { ;    // total eclipse }
   //else if(sf > 0.0) { ; // partial eclipse }
   //else { ;              // no eclipse }

   // Y is perpendicular to Z and T, such that ...
   Y = Z.cross(T);
   d = 1.0/Y.mag();
   Y = d * Y;                                // normalize Y

   // ... X points generally in the direction of the sun
   X = Y.cross(Z);                           // X will be unit vector
   if(X.dot(T) < 0) {                        // need to reverse X, hence Y also
      X = -1.0 * X;
      Y = -1.0 * Y;
   }

   // fill the matrix and return it
   for(i=0; i<3; i++) {
      R(0,i) = X[i];
      R(1,i) = Y[i];
      R(2,i) = Z[i];
   }

   return R;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

// -----------------------------------------------------------------------------------
// Compute the phase windup, in cycles, given the time, the unit vector from receiver
// to transmitter, and the west and north unit vectors at the receiver, all in ECEF.
// YR is the West unit vector, XR is the North unit vector, at the receiver.
// shadow is the fraction of the sun's area not visible at the satellite.
double PhaseWindup(DayTime& tt,        // epoch of interest
                   Position& SV,       // satellite position
                   Position& Rx2Tx,    // unit vector from receiver to satellite
                   Position& YR,       // west unit vector at receiver
                   Position& XR,       // north unit vector at receiver
                   double& shadow)     // fraction of sun not visible at satellite
{
try {
   double d,windup=0.0;
   Position DR,DT;
   Position TR = -1.0 * Rx2Tx;         // transmitter to receiver

   // get satellite attitude
   Position XT,YT,ZT;
   Matrix<double> Att = SatelliteAttitude(tt, SV, shadow);
   XT = Position(Att(0,0),Att(0,1),Att(0,2));      // Cartesian is default
   YT = Position(Att(1,0),Att(1,1),Att(1,2));
   ZT = Position(Att(2,0),Att(2,1),Att(2,2));

   // compute effective dipoles at receiver and transmitter
   DR = XR - TR * TR.dot(XR) + Position(TR.cross(YR));
   DT = XT - TR * TR.dot(XT) - Position(TR.cross(YT));

   // normalize
   d = 1.0/DR.mag();
   DR = d * DR;
   d = 1.0/DT.mag();
   DT = d * DT;

   windup = ::acos(DT.dot(DR)) / TWO_PI;

   return windup;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Solar ephemeris, in ECEF coordinates.
// Accuracy is about 1 arcminute, when t is within 2 centuries of 2000.
// Ref. Astronomical Almanac pg C24, as presented on USNO web site.
// input
//    t             epoch of interest
// output
//    lat,lon,R     latitude, longitude and distance (deg,deg,m in ECEF) of sun at t.
//    AR            apparent angular radius of sun as seen at Earth (deg) at t.
void SolarPosition(DayTime t, double& lat, double& lon, double& R, double& AR)
{
try {
   //const double mPerAU = 149598.0e6;
   double D;     // days since J2000
   double g,q;
   double L;     // sun's geocentric apparent ecliptic longitude (deg)
   //double b=0; // sun's geocentric apparent ecliptic latitude (deg)
   double e;     // mean obliquity of the ecliptic (deg)
   //double R;   // sun's distance from Earth (m)
   double RA;    // sun's right ascension (deg)
   double DEC;   // sun's declination (deg)
   //double AR;  // sun's apparent angular radius as seen at Earth (deg)

   D = t.JD() - 2451545.0;
   g = (357.529 + 0.98560028 * D) * DEG_TO_RAD;
   q = 280.459 + 0.98564736 * D;
   L = (q + 1.915 * ::sin(g) + 0.020 * ::sin(2*g)) * DEG_TO_RAD;

   e = (23.439 - 0.00000036 * D) * DEG_TO_RAD;
   RA = atan2(::cos(e)*::sin(L),::cos(L)) * RAD_TO_DEG;
   DEC = ::asin(::sin(e)*::sin(L)) * RAD_TO_DEG;

   //equation of time = apparent solar time minus mean solar time
   //= [q-RA (deg)]/(15deg/hr)

   // compute the hour angle of the vernal equinox = GMST and convert RA to lon
   lon = fmod(RA-GMST(t),360.0);
   if(lon < -180.0) lon += 360.0;
   if(lon >  180.0) lon -= 360.0;

   lat = DEC;

   // ECEF unit vector in direction Earth to sun
   //xhat = ::cos(lat*DEG_TO_RAD)*::cos(lon*DEG_TO_RAD);
   //yhat = ::cos(lat*DEG_TO_RAD)*::sin(lon*DEG_TO_RAD);
   //zhat = ::sin(lat*DEG_TO_RAD);

   // R in AU
   R = 1.00014 - 0.01671 * ::cos(g) - 0.00014 * ::cos(2*g);
   // apparent angular radius in degrees
   AR = 0.2666/R;
   // convert to meters
   R *= 149598.0e6;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Consider the sun and the earth as seen from the satellite. Let the sun be a circle
// of angular radius r, center in direction s, and the earth be a (larger) circle
// of angular radius R, center in direction e. The circles overlap if |e-s| < R+r;
// complete overlap if |e-s| < R-r. The satellite is in penumbra if R-r < |e-s| < R+r,// it is in umbra if |e-s| < R-r.
//    Let L == |e-s|. What is the area of overlap in penumbra : R-r < L < R+r ?
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
double shadowFactor(double Rearth, double Rsun, double dES)
{
try {
   if(dES >= Rearth+Rsun) return 0.0;
   if(dES <= fabs(Rearth-Rsun)) return 1.0;
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
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
double GMST(DayTime t)
{
try {
      // days' since epoch = +/-(integer+0.5)
   double days = t.JD() - 2451545;
   int d=int(days);
   if(d < 0 && days==double(d)) d++;
   days = d + (days<0.0 ? -0.5 : 0.5);
   double Tp = days/36525.0;

      // Compute GMST
   double G;
   //G = 24060.0 + 50.54841 + 8640184.812866*Tp;  // seconds (24060s = 6h 41min)
   //G /= 86400.0; // instead, divide the above equation by 86400.0 manually...
   G = 0.27847222 + 0.00058505104167 + 100.0021390378009*Tp;
   G += (0.093104 - 6.2e-6*Tp)*Tp*Tp/86400.0;      // seconds/86400 = circles
   double r=1.002737909350795 + (5.9006e-11 - 5.9e-15*Tp)*Tp;
   G += r*t.secOfDay()/86400.0;                   // circles
   G *= 360.0;                                    // degrees
   //G = fmod(G,360.0);
   //if(G < -180.0) G += 360.0;
   //if(G >  180.0) G -= 360.0;

   return G;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

} // end namespace gpstk
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
