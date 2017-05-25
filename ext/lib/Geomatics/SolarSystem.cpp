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

/// @file SolarSystem.cpp
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

//------------------------------------------------------------------------------------
#include "SolarSystem.hpp"

//------------------------------------------------------------------------------------
using namespace std;

namespace gpstk
{
   //---------------------------------------------------------------------------------
   // Compute the ECEF (terrestrial frame, relative to Earth's center) position of a
   // Solar System body at the input time, with units meters.
   // param body  SolarSystem::Planet of interest (input)
   // param time  Time of interest (input)
   // return ECEF Position of the body in meters.
   Position SolarSystem::ECEFPosition(const SolarSystemEphemeris::Planet body,
                                      const EphTime time)
      throw(Exception)
   {
      try {
         Position Pos, Vel;
         ECEFPositionVelocity(body, time, Pos, Vel);
         return Pos;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   //---------------------------------------------------------------------------------
   // Compute the ECEF (terrestrial frame, relative to Earth's center) position and
   // velocity of a Solar System body at the input time, with units meters and m/s.
   // param body  SolarSystem::Planet of interest (input)
   // param time  Time of interest, in system TDB (input)
   // return double PV[6] containing position XYZ components (PV[0-2]) in meters
   //  and velocity XYZ components (PV[3-5]) in m/sec.
   void SolarSystem::ECEFPositionVelocity(const SolarSystemEphemeris::Planet body,
                                          const EphTime time,
                                          Position& Pos, Position& Vel)
      throw(Exception)
   {
      try {
         int i;
         double PV[6];

         // get inertial frame position and velocity relative to Earth
         EphTime ttag(time);
         ttag.convertSystemTo(TimeSystem::TDB);
         RelativeInertialPositionVelocity(ttag.dMJD(), body, idEarth, PV);// km,km/day

         // copy into 3-vectors
         Vector<double> iPos(3),iVel(3),tPos(3),tVel(3);
         for(i=0; i<3; i++) {
            iPos(i) = PV[i];
            iVel(i) = PV[i+3];
         }

         // get EOP at time
         ttag.convertSystemTo(TimeSystem::UTC);
         EarthOrientation eo = EOPStore::getEOP(ttag.dMJD(), iersconv);

         // get transformation i-to-t = transpose(terrestrial-to-inertial)
         Matrix<double> Rot = transpose(eo.ECEFtoInertial(time));

         // transform inertial to terrestrial
         tPos = Rot * iPos;
         tVel = Rot * iVel;

         // change units
         tPos *= 1000.0;                                 // convert km to meters
         tVel *= 1000.0/86400.0;                         // convert km/day to m/s

         // copy out
         Pos = Position(tPos(0),tPos(1),tPos(2),Position::Cartesian);
         Vel = Position(tVel(0),tVel(1),tVel(2),Position::Cartesian);

         return;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
      catch(exception& e) {
         Exception E("std except: " + string(e.what()));
         GPSTK_THROW(E);
      }
      catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
   }

}  // end namespace gpstk
