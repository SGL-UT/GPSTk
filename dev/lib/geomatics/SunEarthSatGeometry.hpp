#pragma ident "$Id: SunEarthSatGeometry.hpp 200 2009-06-19 20:07:03Z BrianTolman $"

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
 * @file SunEarthSatGeometry.hpp
 * Include file for various routines related to Sun-Earth-Satellite geometry,
 * including satellite attitude, XYZ->UEN rotation, and (elevation,azimuth) as
 * seen at the satellite. Used by PhaseWindup and PreciseRange.
 */

#ifndef SUN_EARTH_SATTELITE_INCLUDE
#define SUN_EARTH_SATTELITE_INCLUDE

#include "DayTime.hpp"
#include "Matrix.hpp"
#include "Position.hpp"
#include "SolarSystem.hpp"
#include "EarthOrientation.hpp"

namespace gpstk
{
   /// Given a Position, compute unit vectors in ECEF coordinates in the Up, East and
   /// North directions at that position. Use geodetic coordinates, i.e. 'up' is
   /// perpendicular to the geoid, not from center of Earth (UpEastNorthGeocentric()).   /// Return the vectors in the form of a 3x3 Matrix<double>, this is in fact the
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

   /// Same as UpEastNorth(), but with rows re-ordered.
   Matrix<double> NorthEastUp(Position& P, bool geoc=false) throw(Exception);

   /// Same as UpEastNorthGeocentric(), but with rows re-ordered.
   Matrix<double> NorthEastUpGeocentric(Position& P) throw(Exception);

   /// Generate a 3x3 rotation Matrix, for direct rotations about one axis
   /// (for XYZ, axis=123), given the rotation angle in radians;
   /// @param angle in radians.
   /// @param axis 1,2,3 as rotation about X,Y,Z.
   /// @return Rotation matrix (3x3).
   /// @throw InvalidInput if axis is anything other than 1, 2 or 3.
   Matrix<double> SingleAxisRotation(double angle, int axis) throw(Exception);

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
   /// @param DayTime tt           Time of interest
   /// @param Position SV          Satellite position
   /// @param SolarSystem SSEph    Solar system ephemeris
   /// @param EarthOrientation EO  Earth orientation parameters appropriate for time
   /// @param double sf            Shadow factor: 0 <= sf <= 1 fraction of sun visible
   /// @return Matrix<double>(3,3) Rotation matrix from XYZ to Satellite body frame.
   /// @throw if SolarSystem is invalid.
   Matrix<double> SatelliteAttitude(const DayTime& tt, const Position& SV,
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
   /// @param DayTime tt           Time of interest
   /// @param Position SV          Satellite position
   /// @param double sf            Shadow factor: 0 <= sf <= 1 fraction of sun visible
   /// @return Matrix<double>(3,3) Rotation matrix from XYZ to Satellite body frame.
   Matrix<double> SatelliteAttitude(const DayTime& tt, const Position& SV, double& sf)
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

}  // end namespace gpstk

#endif // SUN_EARTH_SATTELITE_INCLUDE
