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
 * @file GeodeticFrames.hpp
 * Include file defining the GeodeticFrames class.
 * class gpstk::GeodeticFrames encapsulates frame transformations between the
 * conventional terrestrial frame and the conventional inertial frame, as defined
 * by the reference. It implements models of precession and nutation of
 * Earth's axis, as well as the precise rotation of Earth and its 'wobble'
 * as given by the Earth orientation parameters (see class EarthOrientation).
 * Reference: IERS Technical Note 21, IERS Conventions (1996),
 * Dennis D. McCarthy, U.S. Naval Observatory, especially Chapter 5. 
 */

//------------------------------------------------------------------------------------
#ifndef CLASS_GEODETICFRAMES_INCLUDE
#define CLASS_GEODETICFRAMES_INCLUDE

//------------------------------------------------------------------------------------
// GPSTk includes
#include "Exception.hpp"
#include "DayTime.hpp"
#include "Matrix.hpp"

//------------------------------------------------------------------------------------
namespace gpstk
{

   /** Class GeodeticFrames encapsulates frame transformations between the
     * conventional terrestrial frame and the conventional inertial frame, as defined
     * by the reference. It implements models of precession and nutation (IERS 1996)
     * of Earth's axis, as well as the precise rotation of Earth and its 'wobble'
     * as given by the Earth orientation parameters (see class EarthOrientation).
     *
     * Reference: IERS Technical Note 21, IERS Conventions (1996),
     * Dennis D. McCarthy, U.S. Naval Observatory.
     *
     * The conventional terrestrial system (CTS) or Earth-centered,
     * Earth-fixed frame (ECEF), is related to the conventional inertial system
     * (CIS) by four things: a) precession and b) nutation of the Earth and
     * c) Earth rotation and d) polar motion. The transformation between a vector
     * X(CTS) in the terrestrial (ECEF) frame and the vector X(CIS) in the inertial
     * frame is
     *
     * <pre>
     * X(CIS) = P * N * R * W * X(CTS)
     *    where
     * W is the transformation using Earth Orientation Parameters
     *    xp, yp (pole coordinates obtained from the IERS bulletin),
     * R is the effect of both Earth rotation and
     *    precession and nutation in the right ascension,
     * N is the nutation matrix,
     * P is the precession matrix.
     *
     * Reference: IERS Technical Note 21, IERS Conventions (1996), Chapter 5.
     *
     * IF
     *   R1(a) =  [ 1    0      0    ]
     *            [ 0  cos(a) sin(a) ]
     *            [ 0 -sin(a) cos(a) ]
     *
     *   R2(b) =  [ cos(b) 0 -sin(b) ]
     *            [   0    1    0    ]
     *            [ sin(b) 0  cos(b) ]
     *
     *   R3(c) =  [  cos(c) sin(c) 0 ]
     *            [ -sin(c) cos(c) 0 ]
     *            [    0      0    1 ]
     *
     * and if
     *
     *   T = (t-t0)(in days)/36525.0 days
     *   where
     *   t0 = J2000 = January 1 2000 12h UT = 2451545.0JD
     *
     * THEN ----------------------------------------------------------------
     *  [PRECESSION IAU76]
     *
     *   P = R3(zeta)*R2(-theta)*R3(z)
     *  where
     *   zeta  = 2306.2181*T + 0.30188*T^2 + 0.017998*T^3 seconds of arc
     *   theta = 2004.3109*T - 0.42665*T^2 - 0.041833*T^3 seconds of arc
     *   z     = 2306.2181*T + 1.09468*T^2 + 0.018203*T^3 seconds of arc
     *
     * AND -----------------------------------------------------------------
     *  [NUTATION IAU76]
     *
     *   N = R1(-eps)*R3(dpsi)*R1(eps+deps)
     *  where
     *   eps  = obliquity of the ecliptic
     *   deps = nutation in obliquity
     *   dpsi = nutation in longitude (counted in the ecliptic)
     *
     *   eps = (84381.448 - 46.8150*T - 0.00059*T^2 +0.001813*T^3) seconds of arc
     *
     *  and the principal terms in the series for the other two parameters are
     *  ( " denotes seconds of arc)
     *
     *  (IAU76)
     *   deps   =
     *     ( 9.205356 + 0.000886*T)*cos(Omega)"       + 0.001553*sin(Omega)"
     *   + ( 0.573058 - 0.000306*T)*cos(2F-2D+2Omega)"- 0.000464*sin(2F-2D+2Omega)"
     *   + ( 0.097864 - 0.000048*T)*cos(2F+2Omega)"   + 0.000136*sin(2F+2Omega)"
     *   + (-0.089747 + 0.000047*T)*cos(2Omega)"      - 0.000029*sin(2Omega)"
     *   + ( 0.007388 - 0.000019*T)*cos(-L')"         + 0.000198*sin(-L')"
     *   + ( 0.022440 - 0.000068*T)*cos(Larg)"        - 0.000018*sin(Larg)"
     *   + (-0.000687 + 0.000000*T)*cos(L)"           - 0.000039*sin(L)"
     *   etc...
     *   dpsi   =     
     *     (-17.206277- 0.017419*T)*sin(Omega)"       + 0.003645*cos(Omega)"
     *   + (-1.317014 - 0.000156*T)*sin(2F-2D+2Omega)"- 0.001400*cos(2F-2D+2Omega)"
     *   + (-0.227720 - 0.000023*T)*sin(2F+2Omega)"   + 0.000269*cos(2F+2Omega)"
     *   + ( 0.207429 + 0.000021*T)*sin(2Omega)"      - 0.000071*cos(2Omega)"
     *   + (-0.147538 + 0.000364*T)*sin(-L')"         + 0.001121*cos(-L')"
     *   + (-0.051687 + 0.000123*T)*sin(Larg)"        - 0.000054*cos(Larg)"
     *   + ( 0.071118 + 0.000007*T)*sin(L)"           - 0.000094*cos(L)"
     *   etc...
     * OR
     *
     *  (IERS 1980)
     *   deps   =   ( 9.2025 + 0.00089*T)*cos(Omega)"
     *            + ( 0.5736 - 0.00031*T)*cos(2F-2D+2Omega)"
     *            + ( 0.0977 - 0.00005*T)*cos(2F+2Omega)"
     *            + (-0.0895 + 0.00005*T)*cos(2Omega)"
     *            + ( 0.0054 - 0.00001*T)*cos(-L')"
     *            + (-0.0007 + 0.00000*T)*cos(L)"
     *   etc...
     *   dpsi   =   (-17.1996- 0.01742*T)*sin(Omega)"
     *            + (-1.3187 - 0.00016*T)*sin(2F-2D+2Omega)"
     *            + (-0.2274 - 0.00002*T)*sin(2F+2Omega)"
     *            + ( 0.2062 + 0.00002*T)*sin(2Omega)"
     *            + (-0.1426 + 0.00034*T)*sin(-L')"
     *            + ( 0.0712 + 0.00001*T)*sin(L)"
     *   etc...
     *  with
     *
     *   Larg    = L'+2F-2D+2Omega
     *
     *   Omega   = mean longitude of the lunar ascending node
     *           = 125.04455501 degrees - 6962890.2665"*T + 7.4722"*T^2
     *               + 0.007702"*T^3 - 0.00005939"*T^4
     *
     *   D       = mean elongation of the moon from the sun
     *           = 297.85019547 degrees + 1602961601.2090"*T - 6.3706"*T^2
     *               + 0.006593"*T^3 - 0.00003169"*T^4
     *
     *   F       = mean longitude of the moon - Omega
     *           = 93.27209062 degrees + 1739527262.8478"*T - 12.7512"*T^2
     *               - 0.001037"*T^3 + 0.00000417"*T^4
     *
     *   L'      = mean anomaly of the sun
     *           = 357.52910918 degrees + 129596581.0481"*T - 0.5532"*T^2
     *               + 0.000136"*T^3 - 0.00001149"*T^4
     *
     *   L       = mean anomaly of the moon
     *           = 134.96340251 degrees + 1717915923.2178"*T + 31.8792"*T^2
     *               +0.051635"*T^3 -0.00024470"*T^4
     *
     * AND -----------------------------------------------------------------
     *
     *   R = R3(-GAST)
     *
     *   GAST = Greenwich hour angle of the true vernal equinox
     *   GAST = Greenwich Apparent Sidereal Time
     *   GAST = GMST + dpsi*cos(eps) + 0.00264"*sin(Omega) +0.000063"*sin(2*Omega)
     *      (these terms account for the accumulated precession and nutation in
     *         right ascension and minimize any discontinuity in UT1)
     *
     *   GMST = Greenwich hour angle of the mean vernal equinox
     *        = Greenwich Mean Sidereal Time
     *        = GMST0 + r*[(UT1-UTC)+UTC]
     *   r    = is the ratio of universal to sidereal time
     *        = 1.002737909350795 + 5.9006E-11*T' - 5.9e-15*T'^2
     *   T'   = days'/36525
     *   days'= number of days elapsed since t0 = +/-(integer+0.5)
     *      and
     *   (UT1-UTC) is taken from the IERS bulletin (seconds)
     *
     *   GMST0 = GMST at 0h UT1
     *        = 6h 41m (50.54841+8640184.812866*T'+0.093104*T'^2-6.2E-6*T'^3)s
     *
     * AND -----------------------------------------------------------------
     *
     *   W = R1(yp)*R2(xp)
     *   where xp and yp are the polar coordinates of the celestial ephemeris pole,
     *      taken from the IERS bulletin. (NB in the bulletin they are in units
     *      of arcseconds, and they must be converted to radians by multiplying
     *      by pi/180/3600.)
     *
     * </pre>
     */
   class GeodeticFrames
   {
      //------------------------------------------------------------------------------
      // constants
      /// Epoch for the coordinate transformation time, used throughout the formulas
      /// = J2000 = January 1 2000 12h UT
      static const long JulianEpoch;

      //------------------------------------------------------------------------------
      // functions used internally
      //------------------------------------------------------------------------------
      /// Compute the 'coordinate transformation time', which is used throughout the
      /// class, and is essentially the time since J2000 in centuries.
      /// @param t DayTime time of interest.
      /// @return coordinate transformation time at t.
      static double CoordTransTime(DayTime t)
         throw();

      //------------------------------------------------------------------------------
      /// Compute the mean longitude of lunar ascending node, in degrees,
      /// given T, the CoordTransTime at the time of interest.
      /// @param T coordinate transformation time.
      /// @return Omega in degrees.
      static double Omega(double T)
         throw();

      //------------------------------------------------------------------------------
      /// Compute the mean longitude of the moon minus Omega, in degrees,
      /// given T, the CoordTransTime at the time of interest.
      /// @param T coordinate transformation time.
      /// @return F in degrees.
      static double F(double T)
         throw();

      //------------------------------------------------------------------------------
      /// Compute the mean elongation of the moon from the sun, in degrees,
      /// given T, the CoordTransTime at the time of interest.
      /// @param T coordinate transformation time.
      /// @return D in degrees.
      static double D(double T)
         throw();

      //------------------------------------------------------------------------------
      /// Compute the mean anomaly of the moon, in degrees,
      /// given T, the CoordTransTime at the time of interest.
      /// @param T coordinate transformation time.
      /// @return L in degrees.
      static double L(double T)
         throw();

      //------------------------------------------------------------------------------
      /// Compute the mean anomaly of the sun, in degrees,
      /// given T, the CoordTransTime at the time of interest.
      /// @param T coordinate transformation time.
      /// @return Lp in degrees.
      static double Lp(double T)
         throw();

      //------------------------------------------------------------------------------
      /// Compute the obliquity of the ecliptic, in degrees,
      /// given T, the CoordTransTime at the time of interest.
      /// @param T coordinate transformation time.
      /// @return eps in degrees.
      static double Obliquity(double T)
         throw();

      //------------------------------------------------------------------------------
      /// Nutation of the obliquity (deps) and of the longitude (dpsi), IERS 1996
      /// model (ref pg 26), given
      /// @param T,    the coordinate transformation time at the time of interest
      /// @param deps, nutation of the obliquity (output)
      /// @param dpsi, nutation of the longitude (output)
      static void NutationAngles(double T,
                                 double& deps,
                                 double& dpsi)
         throw();

      //------------------------------------------------------------------------------
      /// Zonal tide terms for corrections of UT1mUTC when that quantity does not
      /// include tides (e.g. NGA EOP), ref. IERS 1996 Ch. 8, table 8.1 pg 74.
      /// @param T,    the coordinate transformation time at the time of interest
      /// @param UT1mUT1R, the correction to UT1mUTC (seconds)
      /// @param dlodR, the correction to the length of day (seconds)
      /// @param domegaR, the correction to the Earth rotation rate, rad/second.
      static void UT1mUTCTidalCorrections(double T,
                                          double& UT1mUTR,
                                          double& dlodR,
                                          double& domegaR)
         throw();

      //------------------------------------------------------------------------------
      /// Compute the Greenwich hour angle of the true vernal equinox, or
      /// Greenwich Apparent Sidereal Time, in radians, given the (UT) time of
      /// interest t, and, where T = CoordTransTime(t),
      /// om  = Omega(T) = mean longitude of lunar ascending node, in degrees,
      /// ep = Obliquity(T) = the obliquity of the ecliptic, in degrees,
      /// dp = nutation in longitude (counted in the ecliptic),
      ///                in seconds of arc.
      ///
      /// GAST = Greenwich hour angle of the true vernal equinox
      ///      = Greenwich apparent sidereal time
      ///      = GMST + dpsi*cos(eps) + 0.00264"*sin(Omega) + 0.000063"*sin(2*Omega)
      ///    (these terms account for the accumulated precession and nutation in
      ///       right ascension and minimize any discontinuity in UT1)
      ///
      /// GMST = Greenwich hour angle of the mean vernal equinox
      ///      = Greenwich mean sidereal time
      ///      = GMST0 + r*[UTC + (UT1-UTC)]
      /// r    = is the ratio of universal to sidereal time
      ///      = 1.002737909350795 + 5.9006E-11*T' - 5.9e-15*T'^2
      /// T'   = days'/36525
      /// days'= number of days elapsed since t0 = +/-(integer+0.5)
      ///    and
      /// (UT1-UTC) (seconds) is taken from the IERS bulletin 
      ///
      /// GMST0 = GMST at 0h UT1
      ///      = 6h 41m (50.54841+8640184.812866*T'+0.093104*T'^2-6.2E-6*T'^3)s
      ///
      /// @param t DayTime time of interest.
      /// @param om, Omega(T), mean longitude of lunar ascending node, in degrees,
      /// @param eps, Obliquity(T), the obliquity of the ecliptic, in degrees,
      /// @param dpsi, nutation in longitude (counted in the ecliptic),
      ///                       in seconds of arc
      /// @param UT1mUTC,  UT1-UTC in seconds, as found in the IERS bulletin.
      static double gast(DayTime t,
                         double om,
                         double eps,
                         double dpsi,
                         double UT1mUTC)
         throw();

      //------------------------------------------------------------------------------
      /// Compute the precession matrix, a 3x3 rotation matrix, given
      /// @param T, the coordinate transformation time at the time of interest
      /// @return precession matrix Matrix<double>(3,3)
      static Matrix<double> PrecessionMatrix(double T)
         throw(InvalidRequest);

      //------------------------------------------------------------------------------
      /// Nutation model IAU76 (ref McCarthy), compute the nutation matrix,
      /// a 3x3 rotation matrix, given
      /// @param ep, Obliquity(T), the obliquity of the ecliptic, in degrees,
      /// @param dp, dpsi(T), the nutation in longitude (counted in the ecliptic),
      ///        in seconds of arc, and
      /// @param de, the nutation in obliquity, in seconds of arc.
      /// @return nutation matrix Matrix<double>(3,3)
      static Matrix<double> NutationMatrix(double eps,
                                           double dpsi,
                                           double deps)
         throw(InvalidRequest);

   public:

      //------------------------------------------------------------------------------
      // public functions
      //------------------------------------------------------------------------------
      /// Compute Greenwich Mean Sidereal Time, or the Greenwich hour angle of
      /// the mean vernal equinox, given the coordinate time of interest,
      /// and UT1-UTC (sec), which comes from the IERS bulletin.
      /// @param t DayTime epoch of the rotation.
      /// @param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
      /// @param reduced, bool true when UT1mUTC is 'reduced', meaning assumes
      ///                 'no tides', as is the case with the NGA EOPs (default=F).
      static double GMST(DayTime t,
                         double UT1mUTC,
                         bool reduced=false)
         throw();

      //------------------------------------------------------------------------------
      /// Compute Greenwich Apparent Sidereal Time, or the Greenwich hour angle of
      /// the true vernal equinox, given the coordinate time of interest,
      /// and UT1-UTC (sec), which comes from the IERS bulletin.
      /// @param t DayTime epoch of the rotation.
      /// @param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
      /// @param reduced, bool true when UT1mUTC is 'reduced', meaning assumes
      ///                 'no tides', as is the case with the NGA EOPs (default=F).
      static double GAST(DayTime t,
                         double UT1mUTC,
                         bool reduced=false)
         throw();

      //------------------------------------------------------------------------------
      /// Generate transformation matrix (3X3 rotation) due to the polar motion
      /// angles xp and yp (arcseconds), as found in the IERS bulletin;
      /// see class EarthOrientation.
      /// @param xp, Earth wobble in arcseconds, as found in the IERS bulletin.
      /// @param yp, Earth wobble in arcseconds, as found in the IERS bulletin.
      static Matrix<double> PolarMotion(double xp,
                                        double yp)
         throw(InvalidRequest);

      //------------------------------------------------------------------------------
      /// Generate precise transformation matrix (3X3 rotation) due to Earth rotation
      /// at Greenwich hour angle of the true vernal equinox and which accounts for
      /// precession and nutation in right ascension, given the UT time of interest
      /// and the UT1-UTC correction (in sec), obtained from the IERS bulletin.
      /// @param t DayTime epoch of the rotation.
      /// @param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
      /// @return 3x3 rotation matrix
      static Matrix<double> PreciseEarthRotation(DayTime t,
                                                 double UT1mUTC,
                                                 bool reduced=false)
         throw(InvalidRequest);

      //------------------------------------------------------------------------------
      /// Generate an Earth Precession Matrix (3X3 rotation) at the given DayTime.
      static Matrix<double> Precession(DayTime t)
         throw(InvalidRequest)
         { return PrecessionMatrix(CoordTransTime(t)); }

      //------------------------------------------------------------------------------
      /// Generate an Earth Nutation Matrix (3X3 rotation) at the given DayTime.
      /// @param t DayTime epoch of the rotation.
      /// @return 3x3 rotation matrix
      static Matrix<double> Nutation(DayTime t)
         throw(InvalidRequest);

      //------------------------------------------------------------------------------
      /// Generate the full transformation matrix (3x3 rotation) relating the ECEF
      /// frame to the conventional inertial frame. Input is the time of interest,
      /// the polar motion angles xp and yp (arcseconds), and UT1-UTC (seconds)
      /// (xp,yp and UT1-UTC are just as found in the IERS bulletin;
      /// see class EarthOrientation).
      /// @param t DayTime epoch of the rotation.
      /// @param xp, Earth wobble in arcseconds, as found in the IERS bulletin.
      /// @param yp, Earth wobble in arcseconds, as found in the IERS bulletin.
      /// @param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
      /// @param reduced, bool true when UT1mUTC is 'reduced', meaning assumes
      ///                 'no tides', as is the case with the NGA EOPs (default=F).
      /// @return 3x3 rotation matrix
      static Matrix<double> ECEFtoInertial(DayTime t,
                                           double xp,
                                           double yp,
                                           double UT1mUTC,
                                           bool reduced=false)
         throw(InvalidRequest);

      //------------------------------------------------------------------------------
      /// Given a rotation matrix R (3x3), find the Euler angles (theta,phi,psi) which
      /// produce this rotation, and also determine the magnitude alpha and direction
      /// nhat (= unit 3-vector) of the net rotation.
      /// Throw InvalidRequest if the matrix is not a rotation matrix.
      static void ResolveRotation(const Matrix<double>& R,
                                  double& theta,
                                  double& phi,
                                  double& psi,
                                  double& alpha,
                                  Vector<double>& nhat)
         throw(InvalidRequest);

   }; // end class GeodeticFrames

} // end namespace gpstk

#endif  // nothing below this...
