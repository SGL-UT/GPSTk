#pragma ident "$Id: $"

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
 * @file ElevationMask.cpp
 * Implement an elevation mask function for program DDBase.
 */

//------------------------------------------------------------------------------------
// system includes

// GPSTk
#include "geometry.hpp"                // for DEG_TO_RAD

// DDBase
#include "DDBase.hpp"
#include "PhaseWindup.hpp"             // for SingleAxisRotation()

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// prototypes -- this module only
bool DefaultElevationMask(double elevation, double azimuth, double ElevCutoff);
bool RotatedAntennaElevationMask(double elevation, double azimuth, double ElevCutoff);
double RotatedAntennaElevation(double elevation, double azimuth);

//------------------------------------------------------------------------------------
// return 'true' if satellite data at elevation and azimuth (both in degrees)
// is accepted.
bool ElevationMask(double elevation, double azimuth)
{
try {
   if(DefaultElevationMask(elevation,azimuth,CI.MinElevation))
      return RotatedAntennaElevationMask(elevation, azimuth,CI.MinElevation);
   else
      return false;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end ElevationMask

//------------------------------------------------------------------------------------
inline bool DefaultElevationMask(double elevation, double azimuth, double ElevCutoff)
{
   return (elevation >= ElevCutoff);
}

//------------------------------------------------------------------------------------
bool RotatedAntennaElevationMask(double elevation, double azimuth, double ElevCutoff)
{
try {
   return ( RotatedAntennaElevation(elevation, azimuth) >= ElevCutoff );
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Return the elevation of the input direction (elevation and azimuth) in a frame
// which is rotated by angles RotateElev and RotateAzimuth. All angles in degrees.
double RotatedAntennaElevation(double elevation, double azimuth)
{
try {
   if(CI.RotatedAntennaElevation > 0.0 || CI.RotatedAntennaAzimuth > 0.0) {
      Matrix<double> R;
      Vector<double> rhat(3),rotated_rhat;

      elevation *= DEG_TO_RAD;
      azimuth *= DEG_TO_RAD;
      double RotateElev = CI.RotatedAntennaElevation * DEG_TO_RAD;
      double RotateAzimuth = CI.RotatedAntennaAzimuth * DEG_TO_RAD;

      // construct the rotation matrix
      R = SingleAxisRotation(-RotateElev,2) * SingleAxisRotation(RotateAzimuth,3);

      // unit vector, Rx to SV, in ordinary geodetic frame
      rhat(0) = ::cos(azimuth) * ::cos(elevation);
      rhat(1) = ::sin(azimuth) * ::cos(elevation);
      rhat(2) =                  ::sin(elevation);

      // rotate into the frame of the (rotated) antenna
      rotated_rhat = R * rhat;

      elevation = atan2(rotated_rhat(2),RSS(rotated_rhat(0),rotated_rhat(1)));
      elevation *= RAD_TO_DEG;
   }

   return elevation;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end RotatedAntennaElevation

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
