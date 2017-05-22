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

/// @file EarthOrientation.hpp
/// Include file defining the EarthOrientation class.
/// class EarthOrientation encapsulates the Earth orientation parameters or EOPs.
/// Also, this class includes several static functions that implement the models in
/// the IERS Conventions, many of which involve the EOPs.
/// Cf. classes EOPStore and SolarSystem.

#ifndef CLASS_EARTHORIENT_INCLUDE
#define CLASS_EARTHORIENT_INCLUDE

//------------------------------------------------------------------------------------
// system includes
#include <iostream>
#include <iomanip>
#include <vector>
// GPSTk
#include "Exception.hpp"
#include "Triple.hpp"
#include "Position.hpp"
#include "Matrix.hpp"
// geomatics
#include "EphTime.hpp"
#include "IERSConvention.hpp"

//------------------------------------------------------------------------------------
namespace gpstk {

   /// class EarthOrientation encapsulates the Earth orientation parameters or EOPs,
   /// which consist of the polar motion angle xp and yp and the time offset UT1-UTC.
   /// These three parameters are determined by interpolating measured values managed
   /// by class EOPStore. Also, this class includes several static functions that
   /// implement the models in the IERS Conventions (1996, 2003 or 2010), many of
   /// which involve the EOPs. This includes the frame transformation between the
   /// conventional terrestrial (i.e. ECEF) frame and the conventional celestial
   /// (inertial) frame.
   /// The class implements models of precession and nutation of Earth's axis,
   /// as well as the precise rotation of Earth and its 'wobble' as given by the EOPs.
   /// The class requires time to be in one of two systems: UTC or TT; class EphTime
   /// is a simple class that enforces this requirement (plus TDB), transformation to
   /// and from CommonTime should be automatic. (TT is terrestrial and TDB is
   /// barycentric dynamic time, which is the time of SolarSystemEphemeris.)
   /// Also cf. classes EOPStore and SolarSystem.
   /// References:
   /// IERS1996: IERS Technical Note 21, "IERS Conventions (1996),"
   ///   Dennis D. McCarthy, U.S. Naval Observatory, 1996.
   /// IERS2003: IERS Technical Note 32, "IERS Conventions (2003),"
   ///   Dennis D. McCarthy and Gerard Petit eds., U.S. Naval Observatory and
   ///   Bureau International des Poids et Mesures, 2004.
   /// IERS2010: IERS Technical Note 36, "IERS Conventions (2010),"
   ///   Gerard Petit and Brian Luzum eds., Bureau International des Poids et Mesures
   ///   and U.S. Naval Observatory, 2010.
   class EarthOrientation
   {
   public:

      /// Polar motion angle xp, in arcseconds
      double xp;

      /// Polar motion angle yp, in arcseconds
      double yp;

      /// Time offset UT1 minus UTC, in seconds
      double UT1mUTC;

      /// IERS convention appropriate for this data
      IERSConvention convention;

      /// Constructor
      EarthOrientation() : xp(0),yp(0),UT1mUTC(0),convention(IERSConvention::NONE)
         { }

      /// append to output stream
      friend std::ostream& operator<<(std::ostream& s, const EarthOrientation&);

      //------------------------------------------------------------------------------
      // constants

      /// Epoch for the coordinate transformation time, used throughout the formulas
      /// = J2000 = January 1 2000 12h UT but use MJD for this constant
      static const double JulianEpoch;

      /// integer MJD J2000 epoch for use in maximizing precision of CoordTransTime()
      static const int intJulianEpoch;

      /// pi, 2*pi and pi/2
      static const double PI, TWOPI, HALFPI;

      /// convert degrees to radians and back
      static const double DEG_TO_RAD, RAD_TO_DEG;

      /// convert arc seconds to radians
      static const double ARCSEC_TO_RAD;

      /// how many arcseconds in 360 degrees?
      static const double ARCSEC_PER_CIRCLE;

      //------------------------------------------------------------------------------
      // interpolation of data from EOPStore to get EOPs
      /// Given parallel arrays of length four containing the values from EOPStore
      /// for time (int MJD) and EOPs xp, yp, and UT1mUTC, where the time of interest
      /// ttag lies within the values of the time array, interpolate and apply
      /// corrections to determine the EOPs at ttag, using the algorithm prescribed
      /// by the given IERS convention.
      /// @param ttag EphTime at which to compute EOPs
      /// @param time vector of length 4 of consecutive MJDs from EOPStore;
      ///                      ttag must lie within this timespan.
      /// @param X vector of length 4 of consecutive xp from EOPStore.
      /// @param Y vector of length 4 of consecutive yp from EOPStore.
      /// @param dT vector of length 4 of consecutive UT1mUTC from EOPStore.
      /// @param conv the IERSConvention to be used.
      void interpolateEOP(const EphTime& ttag,
                          const std::vector<double>& time,
                          const std::vector<double>& X,
                          const std::vector<double>& Y,
                          std::vector<double>& dT,
                          const IERSConvention& conv)
         throw(InvalidRequest);

      //------------------------------------------------------------------------------
      /// 'coordinate transformation time', which is used throughout the
      /// class, defined as the terrestrial time (TT) since J2000, in centuries.
      /// Valid for IERS1996, IERS2003, IERS2010
      /// @param t EphTime time of interest.
      /// @return coordinate transformation time at t.
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      static double CoordTransTime(EphTime t)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// mean longitude of lunar ascending node, in radians,
      /// given T, the CoordTransTime at the time of interest.
      /// Valid for IERS1996, IERS2003, IERS2010
      /// @param T coordinate transformation time.
      /// @return Omega in radians.
      static double Omega(double T)
         throw();

      //------------------------------------------------------------------------------
      /// mean longitude of lunar ascending node, in radians,
      /// given T, the CoordTransTime at the Epoch of interest.
      /// valid for IERS 2003, 2010
      /// @param T coordinate transformation time.
      /// @return Omega in radians.
      static double Omega2003(double T)
         throw();

      //------------------------------------------------------------------------------
      /// mean longitude of the moon minus Omega, in radians,
      /// given T, the CoordTransTime at the time of interest.
      /// Valid for IERS1996, IERS2003, IERS2010
      /// @param T coordinate transformation time.
      /// @return F in radians.
      static double F(double T)
         throw();

      //------------------------------------------------------------------------------
      /// mean elongation of the moon from the sun, in radians,
      /// given T, the CoordTransTime at the time of interest.
      /// Valid for IERS1996, IERS2003, IERS2010
      /// @param T coordinate transformation time.
      /// @return D in radians.
      static double D(double T)
         throw();

      //------------------------------------------------------------------------------
      /// mean anomaly of the moon, in radians,
      /// given T, the CoordTransTime at the time of interest.
      /// Valid for IERS1996, IERS2003, IERS2010
      /// @param T coordinate transformation time.
      /// @return L in radians.
      static double L(double T)
         throw();

      //------------------------------------------------------------------------------
      /// mean anomaly of the sun, in radians,
      /// given T, the CoordTransTime at the time of interest.
      /// Valid for IERS1996, IERS2003, IERS2010
      /// @param T coordinate transformation time.
      /// @return Lp in radians.
      static double Lp(double T)
         throw();

      //------------------------------------------------------------------------------
      /// mean longitude of Mercury, in radians, given T, the
      /// CoordTransTime at the time of interest. Valid for IERS2003, IERS2010
      /// @param T coordinate transformation time.
      /// @return LMe in radians.
      static double LMe(double T)
         throw();

      //------------------------------------------------------------------------------
      /// mean longitude of Venus, in radians, given T, the CoordTransTime
      /// at the time of interest. Valid for IERS2003, IERS2010
      /// @param T coordinate transformation time.
      /// @return LV in radians.
      static double LV(double T)
         throw();

      //------------------------------------------------------------------------------
      /// mean longitude of Earth, in radians, given T, the CoordTransTime
      /// at the time of interest. Valid for IERS2003, IERS2010
      /// @param T coordinate transformation time.
      /// @return LE in radians.
      static double LE(double T)
         throw();

      //------------------------------------------------------------------------------
      /// mean longitude of Mars, in radians, given T, the CoordTransTime
      /// at the time of interest. Valid for IERS2003, IERS2010
      /// @param T coordinate transformation time.
      /// @return LE in radians.
      static double LMa(double T)
         throw();

      //------------------------------------------------------------------------------
      /// mean longitude of Jupiter, in radians, given T, the
      /// CoordTransTime at the time of interest. Valid for IERS2003, IERS2010
      /// @param T coordinate transformation time.
      /// @return LE in radians.
      static double LJ(double T)
         throw();

      //------------------------------------------------------------------------------
      /// mean longitude of Saturn, in radians, given T, the
      /// CoordTransTime at the time of interest. Valid for IERS2003, IERS2010
      /// @param T coordinate transformation time.
      /// @return LE in radians.
      static double LS(double T)
         throw();

      //------------------------------------------------------------------------------
      /// mean longitude of Uranus, in radians, given T, the
      /// CoordTransTime at the time of interest. Valid for IERS2003, IERS2010
      /// @param T coordinate transformation time.
      /// @return LE in radians.
      static double LU(double T)
         throw();

      //------------------------------------------------------------------------------
      /// mean longitude of Neptune, in radians, given T, the
      /// CoordTransTime at the time of interest. Valid for IERS2003, IERS2010
      /// @param T coordinate transformation time.
      /// @return LE in radians.
      static double LN(double T)
         throw();

      //------------------------------------------------------------------------------
      /// general precession in longitude, in radians, given T, the
      /// CoordTransTime at the time of interest. Valid for IERS2003, IERS2010
      /// @param T coordinate transformation time.
      /// @return Pa in radians.
      static double Pa(double T)
         throw();

      //------------------------------------------------------------------------------
      /// obliquity of the ecliptic, in radians,
      /// given T, the CoordTransTime at the time of interest.
      /// Valid for IERS1996 IERS2003 IERS2010
      /// @param T coordinate transformation time.
      /// @return eps in radians.
      /// @throw if convention is not defined
      double Obliquity(double T)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Compute Greenwich Mean Sidereal Time, or the Greenwich hour angle of
      /// the mean vernal equinox (radians), given the coordinate time of interest,
      /// using this->UT1mUTC (sec), which comes from the IERS bulletin.
      /// @param t EphTime epoch of the rotation.
      /// @param reduced, bool true when UT1mUTC is 'reduced', meaning assumes
      ///                 'no tides', as is the case with the NGA EOPs (default=F).
      /// @return GMST in radians
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      /// @throw if convention is not defined
      double GMST(const EphTime& t, bool reduced=false)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Compute Greenwich Apparent Sidereal Time, or the Greenwich hour angle of
      /// the true vernal equinox (radians), given the coordinate time of interest,
      /// and this object's UT1-UTC (sec), which comes from the IERS bulletin.
      /// @param t EphTime epoch of the rotation.
      /// @param reduced, bool true when UT1mUTC is 'reduced', meaning assumes
      ///                 'no tides', as is the case with the NGA EOPs (default=F).
      /// @return GAST in radians
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      /// @throw if convention is not defined
      double GAST(const EphTime& t, bool reduced=false) throw(Exception);

      //------------------------------------------------------------------------------
      /// Generate transformation matrix (3X3 rotation) due to this object's EOPs
      /// polar motion angles xp and yp (arcseconds), as found in the IERS bulletin;
      /// @param t EphTime epoch of the rotation.
      /// @throw if convention is not defined
      Matrix<double> PolarMotionMatrix(const EphTime& t) throw(Exception);

      //------------------------------------------------------------------------------
      /// Generate an Earth Precession Matrix (3X3 rotation) at the given EphTime.
      /// @param t EphTime epoch of the rotation.
      /// @return precession matrix Matrix<double>(3,3)
      /// @throw if convention is not defined
      Matrix<double> PrecessionMatrix(const EphTime& t) throw(Exception);

      //------------------------------------------------------------------------------
      ///// Nutation of the obliquity (deps) and of the longitude (dpsi)
      ///// @param T,    the coordinate transformation time at the time of interest
      ///// @param deps, nutation of the obliquity (output) in radians
      ///// @param dpsi, nutation of the longitude (output) in radians
      ///// @throw if convention is not defined
      //void NutationAngles(double T, double& deps, double& dpsi)
      //   throw(Exception);

      //------------------------------------------------------------------------------
      /// Generate an Earth Nutation Matrix (3X3 rotation) at the given EphTime.
      /// @param t EphTime epoch of the rotation.
      /// @return nutation matrix Matrix<double>(3,3)
      /// @throw if convention is not defined
      Matrix<double> NutationMatrix(const EphTime& t) throw(Exception);

      //------------------------------------------------------------------------------
      /// Generate precise transformation matrix (3X3 rotation) for Earth motion due
      /// to precession, nutation and frame bias, at the given time of interest.
      /// @param t EphTime epoch of the rotation.
      /// @return 3x3 rotation matrix
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      /// @throw if convention is not defined
      Matrix<double> PreciseEarthRotation(const EphTime& t) throw(Exception);

      //------------------------------------------------------------------------------
      /// Generate the full transformation matrix (3x3 rotation) relating the ECEF
      /// frame to the conventional inertial frame. Input is the time of interest;
      /// use this object's EOPs - the polar motion angles xp and yp (arcseconds),
      /// and UT1-UTC (seconds) (xp,yp and UT1-UTC are as found in the IERS bulletin).
      /// @param t EphTime epoch of the rotation.
      /// @param reduced, bool true when UT1mUTC is 'reduced', meaning assumes
      ///                 'no tides', as is the case with the NGA EOPs (default=F).
      /// @return 3x3 rotation matrix
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      /// @throw if convention is not defined
      Matrix<double> ECEFtoInertial(const EphTime& t, bool reduced=false)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Compute the transformation from ECEF to the J2000 dynamical (inertial)
      /// frame. This differs from the ECEFtoInertial transformation only by the
      /// frame bias matrix. Only available in IERS2010.
      /// *** NB currently not tested. ***
      Matrix<double> ECEFtoJ2000(const EphTime& t, bool reduced=false)
         throw(Exception);

   private:
      //------------------------------------------------------------------------------
      /// locator s which gives the position of the CIO on the equator of
      /// the CIP, given the coordinate transformation time T and the coordinates X,Y
      /// of the CIP.
      /// Consistent with IAU 2000A (IERS2003) precession-nutation, and
      /// P03 precession (IERS2010).
      /// Derived in part from SOFA routine s00.c for IERS2003 and s06.c for IERS2010.
      /// Used for both IERS2003 and IERS2010, but not for IERS1996
      /// @param T, the coordinate transformation time at the time of interest
      /// @param X, the X coordinate of the CIP (input)
      /// @param Y, the Y coordinate of the CIP (input)
      /// @param which, the IERS convention to be used (input)
      /// @return S, the parameter that positions the CIO on the CIP equator.
      static double S(double T, double& X, double& Y,
                                      IERSConvention which=IERSConvention::IERS2003)
         throw();

      //------------------------------------------------------------------------------
      /// The position of the Terrestrial Ephemeris Origin (TEO) on the equator of the
      /// Celestial Intermediate Pole (CIP), as given by the quantity s'.
      /// Also called the Terrestrial Intermediate Origin (TIO).
      /// Ref. IERS Tech Note 32 Chap 5 Eqn 12
      /// @param T Coordinate transformation time T.
      /// @return angle 's prime' in radians
      static double Sprime(double T) throw();

      /// Sprime with EphTime input; cf. Sprime(double T == CoordTransTime(t))
      static double Sprime(EphTime t) throw()
         { return Sprime(CoordTransTime(t)); }

      //------------------------------------------------------------------------------
      /// coordinates X,Y of the Celestial Intermediate Origin (CIO) using
      /// a series based on IAU 2006 precession and IAU 2000A nutation (IERS 2010).
      /// The coordinates form a unit vector that points towards the CIO; they include
      /// the effects of frame bias, precession and nutation. cf. sofa xy06
      /// @param T, the coordinate transformation time at the time of interest
      /// @param X, x coordinate of CIO
      /// @param Y, y coordinate of CIO
      static void XYCIO(double& T, double& X, double& Y)
         throw();

      //------------------------------------------------------------------------------
      /// Starting with 2003 conventions a new method for computing the transformation
      /// fron ITRS to GCRS is provided by the Celestial Ephemeris Origin (CEO) which
      /// is based on the Earth Rotation Angle, which depends on time(UT1).
      /// Ref. IERS Tech Note 32 Chap 5 Eqn 14
      /// @param t EphTime time of interest
      /// @param UT1mUTC offset UT1-UTC
      /// @return Earth rotation angle in radians
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      static double EarthRotationAngle(const EphTime& t, const double& UT1mUTC)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Equation of the equinoxes complementary terms, IAU 2000 (IERS 2003)
      /// Note that GAST = GMST + EquationOfEquinoxes
      /// @param t EphTime epoch of interest
      /// @return the ee in radians
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      static double EquationOfEquinoxes2003(EphTime t)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Zonal tide terms for corrections of UT1mUTC when that quantity does not
      /// include tides (e.g. NGA EOP), ref. IERS 1996 Ch. 8, table 8.1 pg 74.
      /// @param T,    the coordinate transformation time at the time of interest
      /// @param UT1mUT1R, the correction to UT1mUTC (seconds)
      /// @param dlodR, the correction to the length of day (seconds)
      /// @param domegaR, the correction to the Earth rotation rate, rad/second.
      static void UT1mUTCTidalCorrections(double T, double& UT1mUTR,
                                          double& dlodR, double& domegaR)
         throw();

      //------------------------------------------------------------------------------
      /// obliquity of the ecliptic, in radians,
      /// given T, the CoordTransTime at the time of interest, for IERS1996 (also 03)
      /// @param T coordinate transformation time.
      /// @return eps in radians.
      static double Obliquity1996(double T) throw();

      //------------------------------------------------------------------------------
      /// obliquity of the ecliptic, in radians,
      /// given T, the CoordTransTime at the time of interest, for IERS2010
      /// @param T coordinate transformation time.
      /// @return eps in radians.
      static double Obliquity2010(double T) throw();

      //------------------------------------------------------------------------------
      /// Compute Greenwich Mean Sidereal Time, or the Greenwich hour angle of
      /// the mean vernal equinox (radians), given the coordinate time of interest,
      /// and UT1-UTC (sec), which comes from the IERS bulletin. For IERS1996,03
      /// @param t EphTime epoch of the rotation.
      /// @param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
      /// @param reduced, bool true when UT1mUTC is 'reduced', meaning assumes
      ///                 'no tides', as is the case with the NGA EOPs (default=F).
      /// @return GMST in radians
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      static double GMST1996(EphTime t, double UT1mUTC, bool reduced=false)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Compute Greenwich Mean Sidereal Time, or the Greenwich hour angle of
      /// the mean vernal equinox (radians), given the coordinate time of interest,
      /// and UT1-UTC (sec), which comes from the IERS bulletin. For IERS2003
      /// @param t EphTime epoch of the rotation.
      /// @param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
      /// @return GMST in radians
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      static double GMST2003(EphTime t, double UT1mUTC)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Compute Greenwich Mean Sidereal Time, or the Greenwich hour angle of
      /// the mean vernal equinox (radians), given the coordinate time of interest,
      /// and UT1-UTC (sec), which comes from the IERS bulletin. For IERS2010
      /// @param t EphTime epoch of the rotation.
      /// @param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
      /// @return GMST in radians
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      static double GMST2010(EphTime t, double UT1mUTC)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Helper to compute the Greenwich hour angle of the true vernal equinox, or
      /// Greenwich Apparent Sidereal Time, in radians, for IERS1996, given the (UT)
      /// time of interest t, and, where T = CoordTransTime(t),
      /// om  = Omega(T) = mean longitude of lunar ascending node, in radians,
      /// ep = Obliquity(T) = the obliquity of the ecliptic, in radians,
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
      /// @param t EphTime time of interest.
      /// @param om, Omega(T), mean longitude of lunar ascending node, in radians,
      /// @param eps, Obliquity(T), the obliquity of the ecliptic, in radians,
      /// @param dpsi, nutation in longitude (counted in the ecliptic),
      ///                       in seconds of arc
      /// @param UT1mUTC,  UT1-UTC in seconds, as found in the IERS bulletin.
      /// @return gast in radians
      static double gast1996(EphTime t, double om, double eps, double dpsi,
                                            double UT1mUTC)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Compute Greenwich Apparent Sidereal Time, or the Greenwich hour angle of
      /// the true vernal equinox (radians), given the coordinate time of interest,
      /// and UT1-UTC (sec), which comes from the IERS bulletin.
      /// @param t EphTime epoch of the rotation.
      /// @param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
      /// @return GAST in radians
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      static double GAST1996(EphTime t, double UT1mUTC, bool reduced=false)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Compute Greenwich Apparent Sidereal Time, or the Greenwich hour angle of
      /// the true vernal equinox (radians), given the coordinate time of interest,
      /// and UT1-UTC (sec), which comes from the IERS bulletin.
      /// @param t EphTime epoch of the rotation.
      /// @param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
      /// @return GAST in radians
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      static double GAST2003(EphTime t, double UT1mUTC)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Compute Greenwich Apparent Sidereal Time, or the Greenwich hour angle of
      /// the true vernal equinox (radians), given the coordinate time of interest,
      /// and UT1-UTC (sec), which comes from the IERS bulletin.
      /// @param t EphTime epoch of the rotation
      /// @param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin
      /// @return GAST in radians
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      static double GAST2010(EphTime t, double UT1mUTC)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Generate transformation matrix (3X3 rotation) due to the polar motion
      /// angles xp and yp (arcseconds), as found in the IERS bulletin;
      /// @param xp, Earth wobble in arcseconds, as found in the IERS bulletin.
      /// @param yp, Earth wobble in arcseconds, as found in the IERS bulletin.
      /// @return Matrix<double>(3,3) rotation matrix
      static Matrix<double> PolarMotionMatrix1996(double xp, double yp)
         throw();

      //------------------------------------------------------------------------------
      /// Generate transformation matrix (3X3 rotation) due to the polar motion
      /// angles xp and yp (arcseconds), as found in the IERS bulletin;
      /// The returned matrix R transforms the CIP into TRS:  V(TRS) = R * V(CIP).
      /// see sofa pom00. Also valid for IERS2010.
      /// @param t EphTime epoch of the rotation.
      /// @param xp, Earth wobble in arcseconds, as found in the IERS bulletin.
      /// @param yp, Earth wobble in arcseconds, as found in the IERS bulletin.
      /// @return Matrix<double>(3,3) rotation matrix CIP -> TRS
      static Matrix<double> PolarMotionMatrix2003(EphTime t, double xp, double yp)
         throw();

      //------------------------------------------------------------------------------
      /// Compute Fukushima-Williams angles for computing nutation, frame bias and
      /// precession matrices in IERS2010; cf. FukushimaWilliams().
      /// NB. fourth angle is Obliquity.
      static void FukushimaWilliams(double T, double& gamb, double& phib,
                                              double& psib, double& eps)
         throw();

      //------------------------------------------------------------------------------
      /// For IERS 2010, generate any of B = frame bias matrix
      ///                               PB = Precession*Bias matrix
      ///                              NPB = Nutation*Precession*Bias matrix
      /// given the four F-W angles with caviats, as follows.
      /// Get   B by passing the full F-W angles at J2000.
      /// Get  PB by passing the full F-W angles at time time of interest.
      /// Get NPB by passing the full F-W angles at time time of interest with
      ///    nutation angle corrections (NutationAngles2010()). Specifically,
      ///     FukushimaWilliams(T, gamb, phib, psib, eps);
      ///     NutationAngles2010(T, deps, dpsi);
      ///     NPB = FukushimaWilliams(gamb, phib, psib+dpsi, eps+deps);
      /// Thus the precession matrix is computed as PB * transpose(B), and
      ///      the nutation matrix is computed as N = NPB * transpose(PB).
      /// @param gamb F-W angle
      /// @param phib F-W angle
      /// @param psib F-W angle
      /// @param epsa F-W angle, the obliquity
      /// @return 3x3 rotation matrix B, PB or NPB
      static Matrix<double> FukushimaWilliams(double gamb, double phib,
                                              double psib, double epsa)
         throw();

      //------------------------------------------------------------------------------
      /// Nutation of the obliquity (deps) and of the longitude (dpsi), IERS 1996
      /// model (ref pg 26), given
      /// @param T,    the coordinate transformation time at the time of interest
      /// @param deps, nutation of the obliquity (output) in radians
      /// @param dpsi, nutation of the longitude (output) in radians
      /// @param om, longitude mean ascending node of lunar orbit, from mean equinox
      static void NutationAngles1996(double T, double& deps, double& dpsi, double& om)
         throw();

      //------------------------------------------------------------------------------
      /// Nutation of the obliquity (deps) and of the longitude (dpsi), IERS 2003
      /// @param T,    the coordinate transformation time at the time of interest
      /// @param deps, nutation of the obliquity (output) in radians
      /// @param dpsi, nutation of the longitude (output) in radians
      static void NutationAngles2003(double T, double& deps, double& dpsi)
         throw();

      //------------------------------------------------------------------------------
      /// Nutation of the obliquity (deps) and of the longitude (dpsi), IERS 2010
      /// @param T,    the coordinate transformation time at the time of interest
      /// @param deps, nutation of the obliquity (output) in radians
      /// @param dpsi, nutation of the longitude (output) in radians
      static void NutationAngles2010(double T, double& deps, double& dpsi)
         throw();

      //------------------------------------------------------------------------------
      /// nutation matrix, a 3x3 rotation matrix, given
      /// @param eps, Obliquity(T), the obliquity of the ecliptic, in radians,
      /// @param dpsi, the nutation in longitude (counted in the ecliptic) in radians.
      /// @param deps, the nutation in obliquity, in radians.
      /// @return nutation matrix Matrix<double>(3,3)
      static Matrix<double> NutationMatrix(double eps, double dpsi, double deps)
         throw();

      //------------------------------------------------------------------------------
      /// IERS1996 nutation matrix, a 3x3 rotation matrix, given
      /// @param T, the coordinate transformation time at the time of interest
      /// @return nutation matrix Matrix<double>(3,3)
      static Matrix<double> NutationMatrix1996(double T)
         throw();

      //------------------------------------------------------------------------------
      /// IERS2003 nutation matrix, a 3x3 rotation matrix
      /// (including the frame bias matrix), given
      /// @param T, the coordinate transformation time at the time of interest
      /// @return nutation matrix Matrix<double>(3,3)
      static Matrix<double> NutationMatrix2003(double T)
         throw();

      //------------------------------------------------------------------------------
      /// IERS2010 nutation matrix, a 3x3 rotation matrix, given
      /// @param T, the coordinate transformation time at the time of interest;
      /// cf. FukushimaWilliams().
      /// @return nutation matrix Matrix<double>(3,3)
      static Matrix<double> NutationMatrix2010(double T)
         throw();

      //------------------------------------------------------------------------------
      /// IERS1996 precession matrix, a 3x3 rotation matrix, given
      /// @param T, the coordinate transformation time at the time of interest
      /// @return precession matrix Matrix<double>(3,3)
      static Matrix<double> PrecessionMatrix1996(double T)
         throw();

      //------------------------------------------------------------------------------
      /// IERS2003 precession matrix, a 3x3 rotation matrix
      /// (including the frame bias matrix), given
      /// @param T, the coordinate transformation time at the time of interest
      /// @return precession matrix Matrix<double>(3,3)
      static Matrix<double> PrecessionMatrix2003(double T)
         throw();

      //------------------------------------------------------------------------------
      /// IERS2003 precession and obliquity rate corrections, IAU 2000
      /// @param T, the coordinate transformation time at the time of interest
      /// @return precession, obliquity corrections in radians
      static void PrecessionRateCorrections2003(double T, double& dpsi, double& deps)
         throw();

      //------------------------------------------------------------------------------
      /// IERS2010 frame bias matrix, a 3x3 rotation matrix; cf. FukushimaWilliams().
      /// @return frame bias matrix Matrix<double>(3,3)
      static Matrix<double> BiasMatrix2010(void)
         throw();

      //------------------------------------------------------------------------------
      /// IERS2010 precession matrix, a 3x3 rotation matrix, given
      /// @param T, the coordinate transformation time at the time of interest
      /// Does not include the frame bias matrix; cf. FukushimaWilliams().
      /// @return precession matrix Matrix<double>(3,3)
      static Matrix<double> PrecessionMatrix2010(double T)
         throw();

      //------------------------------------------------------------------------------
      /// Generate precise transformation matrix (3X3 rotation) for Earth motion due
      /// to precession, nutation and frame bias, at the given time of interest.
      /// Valid only for 2010
      /// @param T CoordTransTime(EphTime t) for time of interest
      /// @return 3x3 rotation matrix
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      static Matrix<double> PreciseEarthRotation2003(double T)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Generate precise transformation matrix (3X3 rotation) for Earth motion due
      /// to precession, nutation and frame bias, at the given time of interest.
      /// Valid only for 2010
      /// @param T CoordTransTime(EphTime t) for time of interest
      /// @return 3x3 rotation matrix
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      static Matrix<double> PreciseEarthRotation2010(double T)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Generate the full transformation matrix (3x3 rotation) relating the ECEF
      /// frame to the conventional inertial frame, using IERS 1996 conventions.
      /// Input is the time of interest, the polar motion angles xp and yp (arcsecs),
      /// and UT1-UTC (sec) (xp,yp and UT1-UTC are just as found in the IERS bulletin)
      /// @param t EphTime epoch of the rotation.
      /// @param xp, Earth wobble in arcseconds, as found in the IERS bulletin.
      /// @param yp, Earth wobble in arcseconds, as found in the IERS bulletin.
      /// @param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
      /// @param reduced, bool true when UT1mUTC is 'reduced', meaning assumes
      ///                 'no tides', as is the case with the NGA EOPs (default=F).
      /// @return 3x3 rotation matrix
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      Matrix<double> ECEFtoInertial1996(EphTime t, double xp, double yp,
                           double UT1mUTC, bool reduced=false)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Generate the full transformation matrix (3x3 rotation) relating the ECEF
      /// frame to the conventional inertial frame, using IERS 2003 conventions.
      /// Input is the time of interest, the polar motion angles xp and yp (arcsecs),
      /// and UT1-UTC (sec) (xp,yp and UT1-UTC are just as found in the IERS bulletin)
      /// @param t EphTime epoch of the rotation.
      /// @param xp, Earth wobble in arcseconds, as found in the IERS bulletin.
      /// @param yp, Earth wobble in arcseconds, as found in the IERS bulletin.
      /// @param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
      /// @return 3x3 rotation matrix
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      Matrix<double> ECEFtoInertial2003(EphTime t,double xp,double yp,double UT1mUTC)
         throw(Exception);

      //------------------------------------------------------------------------------
      /// Generate the full transformation matrix (3x3 rotation) relating the ECEF
      /// frame to the conventional inertial frame, using IERS 2010 conventions.
      /// Input is the time of interest, the polar motion angles xp and yp (arcsecs),
      /// and UT1-UTC (sec) (xp,yp and UT1-UTC are just as found in the IERS bulletin)
      /// @param t EphTime epoch of the rotation.
      /// @param xp, Earth wobble in arcseconds, as found in the IERS bulletin.
      /// @param yp, Earth wobble in arcseconds, as found in the IERS bulletin.
      /// @param UT1mUTC, UT1-UTC in seconds, as found in the IERS bulletin.
      /// @param reduced, bool true when UT1mUTC is 'reduced', meaning assumes
      ///                 'no tides', as is the case with the NGA EOPs (default=F).
      /// @return 3x3 rotation matrix
      /// @throw if the TimeSystem conversion fails (if TimeSystem is Unknown)
      Matrix<double> ECEFtoInertial2010(EphTime t,double xp,double yp,double UT1mUTC)
         throw(Exception);

   }; // end class EarthOrientation

}  // end namespace gpstk

#endif // CLASS_EARTHORIENT_INCLUDE
