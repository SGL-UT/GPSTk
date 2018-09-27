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

/// @file SolarSystem.hpp
/// This class provides the functionality of SolarSytemEphemeris in the Earth-centered
/// Earth-fixed (ECEF) frame, and makes use of this to implement models for solid
/// Earth tides (SETs) as well as geometry-related functions involving satellite, Sun,
/// Moon and Earth, e.g. satellite attitude.
///
/// The class publicly inherits two large classes:
///    class SolarSystemEphemeris (the JPL solar system ephemeris) and
///    class EOPStore (storage and retrieval of Earth orientiation parameters).
/// The motivation for this design is that the fundamental routine of the ephemeris,
/// SolarSystem::ECEFPositionVelocity(planet,time), always requires simultaneous
/// EarthOrientiation data, and this class enforces that requirement.
/// This design allows the class to retrieve EOPs and with them implement the
/// transformation from the inertial (celestial) frame of the solar system ephemeris
/// to the ECEF (terrestrial) frame (using class EarthOrientation).
/// 
/// The class must first be initialized by initializing both SolarSystemEphemeris and
/// EOPStore. SolarSystemEphemeris is initialized by calling
/// initializeWithBinaryFile(filename), passing the name of a SolarSystem binary file
/// (cf. the convertSSEph app that read JPL ASCII files and creates a binary file).
/// EOPStore is initialized by calling addIERSFile(filename), passing it the
/// finals2000A.data or similar file obtained, e.g. from USNO.
/// (See documentation for both SolarSystemEphemeris and EOPStore).
///
/// The IERS convention (1996, 2003 or 2010) is also included in the class; it is
/// required by EarthOrientation but must be consistent with the SolarSystemEphemeris.

//------------------------------------------------------------------------------------
#ifndef SOLAR_SYSTEM_INCLUDE
#define SOLAR_SYSTEM_INCLUDE

//------------------------------------------------------------------------------------
// includes
// system
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

// GPSTk
#include "Exception.hpp"
#include "EphTime.hpp"
#include "Position.hpp"
#include "XvtStore.hpp"

// geomatics
#include "EOPStore.hpp"
#include "SolarSystemEphemeris.hpp"
#include "IERSConvention.hpp"
#include "EarthOrientation.hpp"
#include "SunEarthSatGeometry.hpp"
#include "SolidEarthTides.hpp"
#include "logstream.hpp"

namespace gpstk {

//------------------------------------------------------------------------------------
/// Class SolarSystem provides the functionality of SolarSytemEphemeris in the
/// Earth-centered Earth-fixed (ECEF) frame, and makes use of this to implement
/// models for solid Earth tides (SETs) as well as geometry-related functions
/// involving satellite, Sun, Moon and Earth, e.g. satellite attitude.
///
/// The class publicly inherits two large classes:
///    class SolarSystemEphemeris (the JPL solar system ephemeris) and
///    class EOPStore (storage and retrieval of Earth orientiation parameters).
/// The motivation for this design is that the fundamental routine of the ephemeris,
/// SolarSystem::ECEFPositionVelocity(planet,time), always requires simultaneous
/// EarthOrientiation data, and this class enforces that requirement.
/// This design allows the class to retrieve EOPs and with them implement the
/// transformation from the inertial (celestial) frame of the solar system ephemeris
/// to the ECEF (terrestrial) frame (using class EarthOrientation).
/// 
/// The class must first be initialized by initializing both SolarSystemEphemeris and
/// EOPStore. SolarSystemEphemeris is initialized by calling
/// initializeWithBinaryFile(filename), passing the name of a SolarSystem binary file
/// (cf. the convertSSEph app that read JPL ASCII files and creates a binary file).
/// EOPStore is initialized by calling addIERSFile(filename), passing it the
/// finals2000A.data or similar file obtained, e.g. from USNO; see EOPStore.
/// The IERS convention (1996, 2003 or 2010) may be set at this time; is also included
/// in the class, it is required by EarthOrientation but must be consistent with the
/// SolarSystemEphemeris.
///
/// <pre>
/// Class design:
///                        SolarSystem
///                           - calls with EphTime (limits to UTC,TT,TDB only)
///                           - member IERSconvention - keeps SSEph and EO consistent
///                           - has all the functionality of the inherited classes,
///                              plus, for convenience, SolidEarthTides, poleTides,
///                          /   Sun-Earth-Sat (SunEarthSat.hpp) geometry functions
///                         /     (all using the high-accuracy SolarSystemEphemeris)
///                        /           \
///                       /  (inherits) \
///                      /               \
/// SolarSystemEphemeris:                 EOPStore:
///  - from JPL data (-> binary file)      - load 'IERS' files e.g. USNO finals.data
///  - times: MJD(TDB) only                - times: MJD(UTC) only
///  - Planets+Sun+Moon position(t)        - simple store of <MJD, EarthOrientation>
///                      o                o
///                       o              o
///                        o   (uses)   o
///                         o          o
///                          o        o
///                      EarthOrientation:
///                       - EOP = (polar motion x,y + UT1-UTC)
///                       - calls with EphTime (using UTC,TT only)
///                       - many static functions implementing IERS conventions
///                         (1996, 2003, 2010); e.g. Precession, Nutation, GMST,
///                         Terrestrial <=> Inertial frame transformations.
/// </pre>

class SolarSystem : public SolarSystemEphemeris, public EOPStore
{

public:
   /// Empty and only constructor. The IERS convention should be consistent with the
   /// SolarSystemEphemeris file when initializeWithBinaryFile() is called, otherwise
   /// a warning is issued.
   SolarSystem(IERSConvention inputiers=IERSConvention::NONE) throw()
      { iersconv = inputiers; }

   /// Choose an IERS Convention. If the input IERS convention is inconsistent with
   /// the loaded ephemeris then a warning is issued.
   void setConvention(const IERSConvention& conv) throw()
   {
      iersconv = conv;
      testIERSvsEphemeris(iersconv, EphNumber());
   }

   /// get the IERS Convention
   IERSConvention getConvention(void) const throw ()
      { return iersconv; }

   /// Overloaded function to load ephemeris file. A check of the ephemeris number
   /// and the IERS convention for this object is made; if the IERS convention is
   /// inconsistent with the ephemeris file then a warning is issued.
   /// Cf. SolarSystemEphemeris::initializeWithBinaryFile(std::string filename).
   int initializeWithBinaryFile(std::string filename) throw(Exception)
   {
      int iret = SolarSystemEphemeris::initializeWithBinaryFile(filename);

      // if not defined, set IERS convention to the default; otherwise test it.
      if(iersconv == IERSConvention::NONE) {
         if(EphNumber() == 403)
            iersconv = IERSConvention::IERS1996;
         else if(EphNumber() == 405)
            iersconv = IERSConvention::IERS2010;         // the default
         else
            LOG(ERROR) << "Unknown ephemeris number " << EphNumber();
      }
      else
         testIERSvsEphemeris(iersconv, EphNumber());

      return iret;
   }

   /// Return the start time of the Solar System ephemeris data
   CommonTime startTime(void) const throw(Exception)
   {
      EphTime t;
      t.setMJD(SolarSystemEphemeris::startTimeMJD());
      t.setTimeSystem(TimeSystem::TDB);
      return static_cast<CommonTime>(t);
   }

   /// Return the end time of the Solar System ephemeris data
   CommonTime endTime(void) const throw(Exception)
   {
      EphTime t;
      t.setMJD(SolarSystemEphemeris::endTimeMJD());
      t.setTimeSystem(TimeSystem::TDB);
      return static_cast<CommonTime>(t);
   }

   /// Overload EOPStore::getEOP() to use the IERS convention of this object
   EarthOrientation getEOP(const double& mjdutc) throw(InvalidRequest)
      { return EOPStore::getEOP(mjdutc, iersconv); }

   /// Return the ECEF (terrestrial frame, relative to Earth's center) position of a
   /// Solar System body at the input time, with units meters.
   /// @param body  SolarSystemEphemeris::Planet of interest (input)
   /// @param tt    Time of interest (input)
   /// @return ECEF Position of the body in meters.
   Position ECEFPosition(const SolarSystemEphemeris::Planet body, const EphTime tt)
      throw(Exception);

   /// Return the ECEF (terrestrial frame, relative to Earth's center) position and
   /// velocity of a Solar System body at the input time, with units meters and m/s.
   /// @param body  SolarSystemEphemeris::Planet of interest (input)
   /// @param tt    Time of interest (input)
   /// @param Pos   Position containing result for position in m in XYZ
   /// @param Vel   Position containing result for velocity in m/s in XYZ
   void ECEFPositionVelocity(const SolarSystemEphemeris::Planet body,
                             const EphTime tt,
                             Position& Pos, Position& Vel)
      throw(Exception);

   /// Convenience routine to get the ECEF position of the Sun
   /// @param tt    Time of interest (input)
   /// @return ECEF Position of the Sun in meters.
   Position SolarPosition(const EphTime tt) throw(Exception)
   {
      try { return ECEFPosition(SolarSystemEphemeris::idSun, tt); }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   /// Convenience routine to get the ECEF position of the Moon
   /// @param tt    Time of interest (input)
   /// @return ECEF Position of the Moon in meters.
   Position LunarPosition(const EphTime tt) throw(Exception)
   {
      try { return ECEFPosition(SolarSystemEphemeris::idMoon, tt); }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   /// Convenience routine to get the ECEF position and velocity of the Sun
   /// @param tt    Time of interest (input)
   /// @param Pos   Position containing result for Solar position in m in XYZ
   /// @param Vel   Position containing result for Solar velocity in m/s in XYZ
   void SolarPositionVelocity(const EphTime tt, Position& Pos, Position& Vel)
      throw(Exception)
   {
      try {
         return ECEFPositionVelocity(SolarSystemEphemeris::idSun, tt, Pos, Vel);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   /// Convenience routine to get the ECEF position and velocity of the Moon
   /// @param tt    Time of interest (input)
   /// @param Pos   Position containing result for Lunar position in m in XYZ
   /// @param Vel   Position containing result for Lunar velocity in m/s in XYZ
   void LunarPositionVelocity(const EphTime tt, Position& Pos, Position& Vel)
      throw(Exception)
   {
      try {
         return ECEFPositionVelocity(SolarSystemEphemeris::idMoon, tt, Pos, Vel);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   /// Compute the satellite attitude, given the time and the satellite position SV
   /// Return a 3x3 Matrix which contains, as rows, the unit (ECEF) vectors X,Y,Z
   /// in the body frame of the satellite, namely
   ///    Z = along the boresight (i.e. towards Earth center),
   ///    Y = perpendicular to both Z and the satellite-sun direction, and
   ///    X = completing the orthonormal triad. X will generally point toward the sun.
   /// Thus this rotation matrix R transforms an ECEF XYZ vector into the body frame
   /// of the satellite, so R * [ECEF XYZ Vector] = components in body frame.
   /// Also, R.transpose() * [satellite body frame Vector] = ECEF XYZ components.
   /// Also return the shadow factor, which is the fraction of the sun's area not
   /// visible to satellite; thus sf > 0 means the satellite is in eclipse.
   /// @param SV Position          Satellite position
   /// @return Matrix<double>(3,3) Rotation matrix from XYZ to Satellite body frame.
   Matrix<double> SatelliteAttitude(const EphTime& tt, const Position& SV)
      throw(Exception)
   {
      try {
         Position Sun = SolarSystem::SolarPosition(tt);
         return gpstk::SatelliteAttitude(SV, Sun);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   /// Compute the angle between the Sun and the plane of the orbit of the satellite,
   /// given the time and the satellite position and velocity at that time.
   /// Return the angle in radians; it lies between +-pi/2 and has the sign of RxV.
   /// That is, the angle is positive if the Sun is out of the orbit plane in the
   /// direction of R cross V; then Sun "sees" the orbit motion as counter-clockwise.
   /// Also return, in phi, the angle, in the plane of the orbit, from the Sun to the
   /// satellite; this lies between 0 and 2pi and increases in the direction of Vel.
   /// @param tt EphTime   Time of interest
   /// @param Pos Position Satellite position at tt
   /// @param Vel Position Satellite velocity at tt (Cartesian, m/s)
   /// @param phi double   Return angle in orbit plane, midnight to sat (radians)
   /// @param beta double  Return angle sun to plane of satellite orbit (radians)
   /// NB. phi, beta and sesa, the satellite-earth-sun angle, form a right spherical
   /// triangle with sesa opposite the right angle. Thus cos(sesa)=cos(beta)*cos(phi).
   void SunOrbitAngles(const EphTime& tt, const Position& Pos, const Position& Vel,
                       double& beta, double& phi)
      throw(Exception)
   {
      try {
         Position Sun = SolarSystem::SolarPosition(tt);
         gpstk::SunOrbitAngles(Pos, Vel, Sun, beta, phi);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   /// Compute the site displacement due to solid Earth tides for the given Position
   /// (assumed to be fixed to the solid Earth) at the given time, given the position
   /// of the site of interest; cf. gpstk::computeSolidEarthTides().
   /// Return a Triple containing the site displacement in ECEF XYZ coordinates with
   /// units meters.
   /// Reference IERS Conventions (1996) found in IERS Technical Note 21
   ///       and IERS Conventions (2003) found in IERS Technical Note 32
   ///       and IERS Conventions (2010) found in IERS Technical Note 36.
   /// NB. Currently only the largest terms are implemented, yielding a result
   /// accurate to the millimeter level. Specifically, TN21 pg 61 eq 8 and
   /// TN21 pg 65 eq 17.
   /// @param site Position Nominal position of the site of interest.
   /// @param tt EphTime   Time of interest.
   /// @return Triple      Displacement vector, ECEF XYZ in meters.
   Triple computeSolidEarthTides(const Position site, const EphTime tt)
      throw(Exception)
   {
      try {
         const Position Sun = SolarSystem::SolarPosition(tt);
         const Position Moon = SolarSystem::LunarPosition(tt);
         const double EMRAT = SolarSystem::EarthToMoonMassRatio();
         const double SERAT = SolarSystem::SunToEarthMassRatio();
         return
            gpstk::computeSolidEarthTides(site, tt, Sun, Moon, EMRAT, SERAT,iersconv);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   /// Compute the site displacement due to rotational deformation due to polar motion
   /// for the given Position (assumed to fixed to the solid Earth) at the given time.
   /// Return a Triple containing the site displacement in ECEF XYZ coordinates with
   /// units meters.
   /// Reference IERS Conventions (1996) found in IERS Technical Note 21, ch. 7 pg 67.
   /// @param Position site  Nominal position of the site of interest.
   /// @param EphTime tt     Time of interest.
   /// @return Triple disp   Displacement vector, ECEF XYZ meters.
   Triple computePolarTides(const Position site, const EphTime tt)
      throw(Exception)
   {
      try {
         EphTime ttag(tt);
         ttag.convertSystemTo(TimeSystem::UTC);
         const EarthOrientation eo=EOPStore::getEOP(ttag.dMJD(), iersconv);
         return gpstk::computePolarTides(site, tt, eo.xp, eo.yp, iersconv);
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

private:
   /// IERS convention in use with this instance of the class. This is determined
   /// either by reading the SolarSystemEphemeris number (403 -> IERS1996,
   /// 405 -> IERS2003 or IERS2010, the default), or by assignments; however if the
   /// IERS convention is inconsistent with the ephemeris then a warning will be
   /// issued at the reading of the ephemeris file or when the assignment is made.
   IERSConvention iersconv;

   /// Helper routine to keep the tests in one place
   void testIERSvsEphemeris(const IERSConvention conv, const int ephno) throw()
   {
      if(ephno == -1) return;         // no ephemeris (yet)

      if((ephno == 403 && conv != IERSConvention::IERS1996)
            ||
         (ephno == 405 && conv != IERSConvention::IERS2003
                       && conv != IERSConvention::IERS2010)) {

            LOG(WARNING) << "Warning - IERS convention (" << conv.asString()
                  << ") is inconsistent with SolarSystemEphemeris (" << ephno << ")";
         }
   }

}; // end class SolarSystem

}  // end namespace gpstk

#endif // SOLAR_SYSTEM_INCLUDE
// nothing below this
