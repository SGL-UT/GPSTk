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
 * @file PhaseWindup.hpp
 * Include file for the phase windup routines.
 */

#ifndef PHASE_WINDUP_INCLUDE
#define PHASE_WINDUP_INCLUDE

#include "DayTime.hpp"
#include "Matrix.hpp"
#include "Position.hpp"

namespace gpstk {

/// Given a Position, compute unit vectors in ECEF coordinates in the Up, East and
/// North directions at that position. Use geodetic coordinates, i.e. 'up' is
/// perpendicular to the geoid. Return the vectors in the form of a
/// 3x3 Matrix<double>, this is in fact the rotation matrix that will take an
/// ECEF vector into an 'up-east-north' vector. Individual unit vectors can be
/// defined from this rotation matrix R by
/// @code
///    Vector<double> U = R.rowCopy(0);
///    Vector<double> E = R.rowCopy(1);
///    Vector<double> N = R.rowCopy(2);
/// @endcode
/// @param P  Position at which the rotation matrix will be defined.
/// @return   3x3 rotation matrix that will transform an ECEF vector into the
///             Up,East,North frame at the position P.
Matrix<double> UpEastNorth(Position& P) throw(Exception);

/// Generate a 3x3 rotation Matrix, for direct rotations about one axis
/// (for XYZ, axis=123), given the rotation angle in radians;
/// @param angle in radians.
/// @param axis 1,2,3 as rotation about X,Y,Z.
/// @return Rotation matrix (3x3).
/// @throw InvalidInput if axis is anything other than 1, 2 or 3.
Matrix<double> SingleAxisRotation(double angle, int axis) throw(Exception);

/// Compute the phase windup, in cycles, given the time, the unit vector from receiver
/// to transmitter, and the west and north unit vectors at the receiver, all in ECEF.
/// YR is the West unit vector, XR is the North unit vector, at the receiver.
/// shadow is the fraction of the sun's area visible at the satellite.
/// @param DayTime& tt       the epoch of interest (input)
/// @param Position& SV      the satellite position (input)
/// @param Position& Rx2Tx   unit vector from receiver to satellite (input)
/// @param Position& YR      west unit vector at receiver (input)
/// @param Position& XR      north unit vector at receiver (input)
/// @param double& shadow    fraction of sun visible at satellite (output)
double PhaseWindup(DayTime& tt,
                   Position& SV,
                   Position& Rx2Tx,
                   Position& RxW,
                   Position& RxN,
                   double& shadow);

}  // end namespace gpstk
#endif
