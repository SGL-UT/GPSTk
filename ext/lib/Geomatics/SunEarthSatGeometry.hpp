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

/// @file SunEarthSatGeometry.hpp
/// Include file for various routines related to Sun-Earth-Satellite geometry,
/// including satellite attitude, XYZ->UEN rotation, and (elevation,azimuth) as
/// seen at the satellite. Used by PhaseWindup and PreciseRange.
/// NB Use class SolarSystem or SolarPosition() to get Sun's position as needed.

#ifndef SUN_EARTH_SATELLITE_INCLUDE
#define SUN_EARTH_SATELLITE_INCLUDE

#include "Matrix.hpp"
#include "Position.hpp"

namespace gpstk
{
   /// Given a Position, compute unit vectors in ECEF coordinates in the North, East
   /// and Up directions at that position. Use either geodetic coordinates, i.e. so
   /// that 'up' is perpendicular to the geoid, not from center of Earth, or
   /// geocentric coordinates, so that the vertical meets the center of the Earth.
   /// Return the vectors in the form of a 3x3 Matrix<double>, this is in fact the
   /// rotation matrix that will take an ECEF (XYZ) vector into 'north,east,up'.
   /// Individual unit vectors can be defined from this rotation matrix R by
   /// @code
   ///    Vector<double> N = R.rowCopy(0);
   ///    Vector<double> E = R.rowCopy(1);
   ///    Vector<double> U = R.rowCopy(2);
   /// @endcode
   /// @param P  Position at which the rotation matrix will be defined.
   /// @param geoc If true, use geocentric, else geodetic coordinates (default false).
   /// @return   3x3 rotation matrix that will transform an ECEF vector into the
   ///             local topocentric or North,East,Up frame at the position P.
   Matrix<double> NorthEastUp(Position& P, bool geoc=false) throw(Exception);

   /// Same as NorthEastUp(P,true).
   /// @param P  Position at which the rotation matrix will be defined.
   /// @return   3x3 rotation matrix that will transform an ECEF vector into the
   ///             local topocentric or North,East,Up frame at the position P.
   Matrix<double> NorthEastUpGeocentric(Position& P) throw(Exception);
   
   /// Same as NorthEastUp(P,false).
   /// @param P  Position at which the rotation matrix will be defined.
   /// @return   3x3 rotation matrix that will transform an ECEF vector into the
   ///             local topocentric or North,East,Up frame at the position P.
   Matrix<double> NorthEastUpGeodetic(Position& P) throw(Exception);

   /// Given a Position, compute unit vectors in ECEF coordinates in the Up, East and
   /// North directions at that position. Use either geodetic coordinates, i.e. so
   /// that 'up' is perpendicular to the geoid, not from center of Earth, or
   /// geocentric coordinates, so that the vertical meets the center of the Earth.
   /// Return the vectors in the form of a 3x3 Matrix<double>, this is in fact the
   /// rotation matrix that will take an ECEF (XYZ) vector into 'up,east,north'.
   /// Individual unit vectors can be defined from this rotation matrix R by
   /// @code
   ///    Vector<double> U = R.rowCopy(0);
   ///    Vector<double> E = R.rowCopy(1);
   ///    Vector<double> N = R.rowCopy(2);
   /// @endcode
   /// @param P  Position at which the rotation matrix will be defined.
   /// @param geoc If true, use geocentric, else geodetic coordinates (default false).
   /// @return  3x3 rotation matrix that will transform an ECEF vector into the
   ///             local topocentric or North,East,Up frame at the position P.
   Matrix<double> UpEastNorth(Position& P, bool geoc=false) throw(Exception);

   /// Same as UpEastNorth, but using geocentric coordinates, so that the -Up
   /// direction will meet the center of Earth.
   /// @param P  Position at which the rotation matrix will be defined.
   /// @return  3x3 rotation matrix that will transform an ECEF vector into the
   ///             local topocentric or North,East,Up frame at the position P.
   Matrix<double> UpEastNorthGeocentric(Position& P) throw(Exception);
   
   /// Same as UpEastNorth, but using geodetic coordinates
   /// @param P  Position at which the rotation matrix will be defined.
   /// @return  3x3 rotation matrix that will transform an ECEF vector into the
   ///             local topocentric or North,East,Up frame at the position P.
   Matrix<double> UpEastNorthGeodetic(Position& P) throw(Exception);

   /// Compute the fraction of the Sun covered by the earth as seen from a satellite,
   /// given the angular radii of both, and their angular separation. See code for
   /// more documentation, including derivations.
   /// @param AngRadEarth    angular radius of the earth as seen at the satellite
   /// @param AngRadSun      angular radius of the sun as seen at the satellite
   /// @param AngSeparation  angular distance of the sun from the earth
   /// @return  fraction (0 <= f <= 1) of area of sun covered by earth
   /// NB. units only need be consistent.
   double ShadowFactor(double AngRadEarth, double AngRadSun, double AngSeparation)
      throw(Exception);
   
   /// Compute the fraction of the Sun covered by the earth as seen from a satellite,
   /// given Sun and satellite positions. Cf. the other version of this function.
   /// @param SV  Position (input) Satellite position
   /// @param Sun Position (input) Sun position
   /// @return  fraction (0 <= f <= 1) of area of sun covered by earth
   double ShadowFactor(const Position& SV, const Position& Sun)
      throw(Exception);

   /// Compute the satellite attitude, given the time, the satellite position SV, and
   /// a Sun position. NB. Use either class SolarSystem (high accuracy) or
   /// module SolarPosition (low accuracy) to get the Sun position; however note that
   /// SolarSystem::SatelliteAttitude(tt, SV, shadow, SunSVangle) makes both calls.
   /// Return a 3x3 Matrix which contains, as rows, the unit (ECEF) vectors X,Y,Z
   /// in the body frame of the satellite, namely
   ///    Z = along the boresight (i.e. towards Earth center),
   ///    Y = perpendicular to both Z and the satellite-sun direction, and
   ///    X = completing the orthonormal triad. X will generally point toward the sun.
   /// Thus this rotation matrix R transforms an ECEF XYZ vector into the body frame
   /// of the satellite, so R * (ECEF XYZ vector) = components in body frame.
   /// Also, transpose(R) * (sat. body. frame vector) = ECEF XYZ components.
   /// Also return the shadow factor, which is the fraction of the sun's area not
   /// visible to satellite; thus sf == 1 means the satellite is in eclipse.
   /// @param pos Position (input) Satellite position at tt
   /// @param Sun Position (input) Sun position at tt
   /// @return Matrix<double>(3,3) Rotation matrix from XYZ to Satellite body frame.
   Matrix<double> SatelliteAttitude(const Position& pos, const Position& Sun)
      throw(Exception);

   /// Compute the satellite attitude, given the satellite position P and velocity V,
   /// assuming an orbit-normal attitude.
   /// Return a 3x3 Matrix which contains, as rows, the unit (ECEF) vectors X,Y,Z
   /// in the body frame of the satellite, namely
   ///    Z = along the boresight (i.e. towards Earth center),
   ///    Y = perpendicular to orbital plane, pointing opposite the angular momentum vector
   ///    X = along-track (same direction as velocity vector for circular orbit)
   /// Thus this rotation matrix R transforms an ECEF XYZ vector into the body frame
   /// of the satellite, so R * (ECEF XYZ vector) = components in body frame.
   /// Also, R.transpose() * (sat. body. frame vector) = ECEF XYZ components.
   /// @param Position P           Satellite position
   /// @param Position V           Satellite velocity
   /// @return Matrix<double>(3,3) Rotation matrix from XYZ to Satellite body frame.
   Matrix<double> OrbitNormalAttitude(const Position& P, const Position& V)
      throw(Exception);

   /// Compute the azimuth and nadir angle, in the satellite body frame,
   /// of receiver Position RX as seen at the satellite Position SV. The nadir angle
   /// is measured from the Z axis, which points to Earth center, and azimuth is
   /// measured from the X axis.
   /// @param SV Position           Satellite position
   /// @param RX Position           Receiver position
   /// @param Rot Matrix<double>    Rotation matrix (3,3), output of SatelliteAttitude
   /// @param nadir double          Output nadir angle in degrees
   /// @param azimuth double        Output azimuth angle in degrees
   /// @throw if rotation matrix has wrong dimentions.
   void SatelliteNadirAzimuthAngles(const Position& SV,
                                    const Position& RX,
                                    const Matrix<double>& Rot,
                                    double& nadir,
                                    double& azimuth)
      throw(Exception);

   /// Compute the angle from satellite to Earth to Sun; that is the angular
   /// separation of the satellite and the Sun, as seen from the center of Earth.
   /// This angle lies between zero and pi, and it reaches zero (pi)
   /// only when the Sun lies exactly in the orbit plane at noon (midnight).
   /// NB. Use either class SolarSystem (high accuracy) or module SolarPosition
   /// (low accuracy) to get the Sun position.
   /// Return the angle in radians.
   /// @param SV Position        Satellite position
   /// @param Sun Position       Sun position at tt
   /// @return angle double      Angle in radians satellite-Earth-Sun
   double SatelliteEarthSunAngle(const Position& SV, const Position& Sun)
      throw(Exception);

   /// Compute the angle between the Sun and the plane of the orbit of the satellite,
   /// given the satellite position and velocity and Sun position, all at one time.
   /// Return the angle in radians; it lies between +-PI/2 and has the sign of RxV.
   /// That is, the angle is positive if the Sun is out of the orbit plane in the
   /// direction of R cross V; then Sun "sees" the orbit motion as counter-clockwise.
   /// Also return phi, the angle, in the plane of the orbit, from midnight to the
   /// satellite; this lies between 0 and 2PI and increases in the direction of Vel.
   /// NB. Use either class SolarSystem (high accuracy) or module SolarPosition
   /// (lower accuracy) to get the Sun position.
   /// @param Pos Position    Satellite position at time of interest
   /// @param Vel Position    Satellite velocity at time of interest
   /// @param Sun Position    Sun position at time of interest
   /// @param phi double      Return angle in orbit plane, midn to satellite (radians)
   /// @param beta double     Return angle sun to plane of satellite orbit (radians)
   /// NB. phi, beta and sesa, the satellite-earth-sun angle, form a right spherical
   /// triangle with sesa opposite the right angle. Thus cos(sesa)=cos(beta)*cos(phi).
   void SunOrbitAngles(const Position& Pos, const Position& Vel, const Position& Sun,
                       double& beta, double& phi)
      throw(Exception);

   /// Compute the nominal yaw angle of the satellite given the satellite position and
   /// velocity and the Sun position at the given time, plus a flag for GPS Block IIR
   /// and IIF satellites. Return the nominal yaw angle in radians, and the yaw rate
   /// in radians/second.
   /// @param P Position     Satellite position at time of interest
   /// @param V Position     Satellite velocity at time of interest (Cartesian, m/s)
   /// @param Sun Position   Sun position at time of interest
   /// @param blkIIRF bool   True if the satellite is GPS block IIR or IIF
   /// @param yawrate double Return yaw rate in radians/second
   /// @return double yaw    Satellite yaw angle in radians
   double SatelliteYawAngle(const Position& P, const Position& V, const Position& Sun,
                            const bool& blkIIRF, double& yawrate)
      throw(Exception);

}  // end namespace gpstk

#endif // SUN_EARTH_SATELLITE_INCLUDE
