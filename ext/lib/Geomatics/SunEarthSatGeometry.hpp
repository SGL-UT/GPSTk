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
 * @file SunEarthSatGeometry.hpp
 * Include file for various routines related to Sun-Earth-Satellite geometry,
 * including satellite attitude, XYZ->UEN rotation, and (elevation,azimuth) as
 * seen at the satellite. Used by PhaseWindup and PreciseRange.
 */

#ifndef SUN_EARTH_SATTELITE_INCLUDE
#define SUN_EARTH_SATTELITE_INCLUDE

#include "CommonTime.hpp"
#include "Matrix.hpp"
#include "Position.hpp"
#include "SolarSystem.hpp"
#include "EarthOrientation.hpp"

namespace gpstk
{
   /// Given a Position, compute unit vectors in ECEF coordinates in the Up, East and
   /// North directions at that position. Use geodetic coordinates, i.e. 'up' is
   /// perpendicular to the geoid, not from center of Earth (UpEastNorthGeocentric()).
   /// Return the vectors in the form of a 3x3 Matrix<double>, this is in fact the
   /// rotation matrix that will take an ECEF vector into an 'up,east,north' vector.
   /// Individual unit vectors can be defined from this rotation matrix R by
   /// @code
   ///    Vector<double> U = R.rowCopy(0);
   ///    Vector<double> E = R.rowCopy(1);
   ///    Vector<double> N = R.rowCopy(2);
   /// @endcode
   /// @param P  Position at which the rotation matrix will be defined.
   /// @param geoc If true, use geocentric, not geodetic (default is false).
   /// @return   3x3 rotation matrix that will transform an ECEF vector into the
   ///             Up,East,North frame at the position P.
   Matrix<double> UpEastNorth(Position& P, bool geoc=false) throw(Exception);

   /// Same as UpEastNorth, but using geocentric coordinates, so that the -Up
   /// direction will meet the center of Earth.
   Matrix<double> UpEastNorthGeocentric(Position& P) throw(Exception);

   /// Same as UpEastNorth, but using geodetic coordinates
   Matrix<double> UpEastNorthGeodetic(Position& P) throw(Exception);

   /// Same as UpEastNorth(), but with rows re-ordered.
   Matrix<double> NorthEastUp(Position& P, bool geoc=false) throw(Exception);

   /// Same as UpEastNorthGeocentric(), but with rows re-ordered.
   Matrix<double> NorthEastUpGeocentric(Position& P) throw(Exception);

   /// Same as UpEastNorthGeodetic(), but with rows re-ordered.
   Matrix<double> NorthEastUpGeodetic(Position& P) throw(Exception);

   /// Generate a 3x3 rotation Matrix, for direct rotations about one axis
   /// (for XYZ, axis=123), given the rotation angle in radians;
   /// @param angle in radians.
   /// @param axis 1,2,3 as rotation about X,Y,Z.
   /// @return Rotation matrix (3x3).
   /// @throw InvalidInput if axis is anything other than 1, 2 or 3.
   Matrix<double> SingleAxisRotation(double angle, const int axis) throw(Exception);

   /// Compute the satellite attitude, given the time, the satellite position SV, and
   /// a SolarSystem ephemeris.
   /// Return a 3x3 Matrix which contains, as rows, the unit (ECEF) vectors X,Y,Z
   /// in the body frame of the satellite, namely
   ///    Z = along the boresight (i.e. towards Earth center),
   ///    Y = perpendicular to both Z and the satellite-sun direction, and
   ///    X = completing the orthonormal triad. X will generally point toward the sun.
   /// Thus this rotation matrix R transforms an ECEF XYZ vector into the body frame
   /// of the satellite, so R * (ECEF XYZ vector) = components in body frame.
   /// Also, R.transpose() * (sat. body. frame vector) = ECEF XYZ components.
   /// Also return the shadow factor, which is the fraction of the sun's area not
   /// visible to satellite; thus sf == 1 means the satellite is in eclipse.
   /// @param CommonTime tt           Time of interest
   /// @param Position SV          Satellite position
   /// @param SolarSystem SSEph    Solar system ephemeris
   /// @param EarthOrientation EO  Earth orientation parameters appropriate for time
   /// @param double sf            Shadow factor: 0 <= sf <= 1 fraction of sun visible
   /// @return Matrix<double>(3,3) Rotation matrix from XYZ to Satellite body frame.
   /// @throw if SolarSystem is invalid.
   Matrix<double> SatelliteAttitude(const CommonTime& tt, const Position& SV,
                                    const SolarSystem& SSEph,
                                    const EarthOrientation& EO,
                                    double& sf)
      throw(Exception);

   /// Version without solar ephemeris - uses lower quality solar position routine
   /// Compute the satellite attitude, given the time, the satellite position SV, and
   /// a SolarSystem ephemeris.
   /// Return a 3x3 Matrix which contains, as rows, the unit (ECEF) vectors X,Y,Z
   /// in the body frame of the satellite, namely
   ///    Z = along the boresight (i.e. towards Earth center),
   ///    Y = perpendicular to both Z and the satellite-sun direction, and
   ///    X = completing the orthonormal triad. X will generally point toward the sun.
   /// Thus this rotation matrix R transforms an ECEF XYZ vector into the body frame
   /// of the satellite, so R * (ECEF XYZ vector) = components in body frame.
   /// Also, R.transpose() * (sat. body. frame vector) = ECEF XYZ components.
   /// Also return the shadow factor, which is the fraction of the sun's area not
   /// visible to satellite; thus sf == 1 means the satellite is in eclipse.
   /// @param CommonTime tt           Time of interest
   /// @param Position SV          Satellite position
   /// @param double sf            Shadow factor: 0 <= sf <= 1 fraction of sun visible
   /// @return Matrix<double>(3,3) Rotation matrix from XYZ to Satellite body frame.
   Matrix<double> SatelliteAttitude(const CommonTime& tt, const Position& SV, double& sf)
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
   /// @param Position SV          Satellite position
   /// @param Position RX          Receiver position
   /// @param Matrix<double> Rot   Rotation matrix (3,3), output of SatelliteAttitude
   /// @param double nadir         Output nadir angle in degrees
   /// @param double azimuth       Output azimuth angle in degrees
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

#endif // SUN_EARTH_SATTELITE_INCLUDE
