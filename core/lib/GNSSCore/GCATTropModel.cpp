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


#include "GCATTropModel.hpp"

namespace gpstk
{
      /* Tropospheric model implemented in "GPS Code Analysis Tool" (GCAT)
       * software.
       *
       * This model is described in the book "GPS Data processing: code and
       * phase Algorithms, Techniques and Recipes" by Hernandez-Pajares, M.,
       * J.M. Juan-Zornoza and Sanz-Subirana, J. See Chapter 5.
       *
       * This book and associated software are freely available at:
       *
       * http://gage152.upc.es/~manuel/tdgps/tdgps.html
       *
       * This is a simple but efective model composed of the wet and dry
       * vertical tropospheric delays as defined in Gipsy/Oasis-II GPS
       * analysis software, and the mapping function as defined by Black and
       * Eisner (H. D. Black, A. Eisner. Correcting Satellite Doppler
       * Data for Tropospheric Effects. Journal of  Geophysical Research.
       * Vol 89. 1984.) and used in MOPS (RTCA/DO-229C) standards.
       *
       * Usually, the caller will set the receiver height using
       * setReceiverHeight() method, and then call the correction() method
       * with the satellite elevation as parameter.
       *
       * @code
       *   GCATTropModel gcatTM();
       *   ...
       *   gcatTM.setReceiverHeight(150.0);
       *   trop = gcatTM.correction(elevation);
       * @endcode
       *
       * Another posibility is to set the receiver height when calling
       * the constructor.
       *
       * @code
       *   GCATTropModel gcatTM(150.0);    // Receiver height is 150.0 meters
       *   ...
       *   trop = gcatTM.correction(elevation);
       * @endcode
       */

      // Constructor to create a GCAT trop model providing  the height of the
      // receiver above mean sea level (as defined by ellipsoid model).
      //
      // @param ht Height of the receiver above mean sea level, in meters.
   GCATTropModel::GCATTropModel(const double& ht)
   {
      setReceiverHeight(ht);
      valid = true;
   }


      /* Compute and return the full tropospheric delay. The receiver height
       * must has been provided before, whether using the appropriate
       * constructor or with the setReceiverHeight() method.
       *
       * @param elevation  Elevation of satellite as seen at receiver, in
       *                   degrees
       */
   double GCATTropModel::correction(double elevation) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();

      if(elevation < 5.0) return 0.0;

      return ( (dry_zenith_delay() + wet_zenith_delay()) *
               mapping_function(elevation));
   }


      /* Compute and return the full tropospheric delay, given the positions of
       * receiver and satellite. This version is most useful within positioning
       * algorithms, where the receiver position may vary; it computes the
       * elevation and the receiver height and passes them to appropriate
       * set...() routines and the correction(elevation) routine.
       *
       * @param RX  Receiver position in ECEF cartesian coordinates (meters)
       * @param SV  Satellite position in ECEF cartesian coordinates (meters)
       */
   double GCATTropModel::correction( const Position& RX,
                                     const Position& SV )
      throw(InvalidTropModel)
   {

      try
      {
         setReceiverHeight( RX.getAltitude() );
      }
      catch(GeometryException& e)
      {
         valid = false;
      }

      if(!valid) throw InvalidTropModel("Invalid model");

      double c;
      try
      {
         c = correction(RX.elevationGeodetic(SV));
      }
      catch(InvalidTropModel& e)
      {
         GPSTK_RETHROW(e);
      }

      return c;

   }


      /* Compute and return the full tropospheric delay, given the positions of
       * receiver and satellite and the time tag. This version is most useful
       * within positioning algorithms, where the receiver position and timetag
       * may vary; it computes the elevation (and other receiver location
       * information) and passes them to appropriate set...() routines and the
       * correction(elevation) routine.
       *
       * @param RX  Receiver position in ECEF cartesian coordinates (meters)
       * @param SV  Satellite position in ECEF cartesian coordinates (meters)
       * @param tt  Time. In this model, tt is a dummy parameter kept only for
       *            consistency
       *
       * This function is deprecated; use the Position version
       */
   double GCATTropModel::correction( const Xvt& RX,
                                     const Xvt& SV,
                                     const CommonTime& tt )
      throw(InvalidTropModel)
   {

      Position R(RX),S(SV);

      return GCATTropModel::correction(R,S);
   }


      /* Compute and return the zenith delay for the dry component of the
       * troposphere.
       */
   double GCATTropModel::dry_zenith_delay(void) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();

      double ddry(2.29951*std::exp((-0.000116 * gcatHeight) ));

      return ddry;
   }


      /* Compute and return the mapping function of the troposphere
       * @param elevation  Elevation of satellite as seen at receiver,
       *                   in degrees
       */
   double GCATTropModel::mapping_function(double elevation) const
      throw(InvalidTropModel)
   {
      THROW_IF_INVALID();

      if(elevation < 5.0) return 0.0;

      double d = std::sin(elevation*DEG_TO_RAD);
      d = SQRT(0.002001+(d*d));

      return (1.001/d);
   }


      /* Define the receiver height; this is required before calling
       * correction() or any of the zenith_delay or mapping_function routines.
       * @param ht Height of the receiver above mean sea level, in meters.
       */
   void GCATTropModel::setReceiverHeight(const double& ht)
   {
      gcatHeight = ht;
      valid = true;
   }

}
