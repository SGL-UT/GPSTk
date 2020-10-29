//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================


#ifndef GCAT_TROP_MODEL_HPP
#define GCAT_TROP_MODEL_HPP

#include "TropModel.hpp"

namespace gpstk
{
      /** Tropospheric model implemented in "GPS Code Analysis Tool" (GCAT)
       *  software.
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
   class GCATTropModel : public TropModel
   {
   public:


         /// Empty constructor
      GCATTropModel(void)
      { valid = false; };


         /// Constructor to create a GCAT trop model providing  the height
         /// of the receiver above mean sea level (as defined by ellipsoid
         /// model).
         ///
         /// @param ht Height of the receiver above mean sea level, in meters.
      GCATTropModel(const double& ht);


         /// Return the name of the model
      virtual std::string name(void)
         { return std::string("GCAT"); }

         /** Compute and return the full tropospheric delay. The receiver
          *  height must has been provided before, whether using the
          *  appropriate constructor or with the setReceiverHeight() method
          *
          * @param elevation  Elevation of satellite as seen at receiver, in
          *                   degrees
          * @throw InvalidTropModel
          */
      virtual double correction(double elevation) const;


         /** Compute and return the full tropospheric delay, given the
          *  positions of receiver and satellite. This version is most useful
          *  within positioning algorithms, where the receiver position may
          *  vary; it computes the elevation (and other receiver location
          *  information as height) and passes them to setReceiverHeight()
          *  method and correction(elevation) method.
          *
          * @param RX  Receiver position
          * @param SV  Satellite position
          * @throw InvalidTropModel
          */
      virtual double correction( const Position& RX,
                                 const Position& SV );


         /** Compute and return the full tropospheric delay, given the
          *  positions of receiver and satellite and the time tag. This version
          *  is most useful within positioning algorithms, where the receiver
          *  position and timetag may vary; it computes the elevation (and
          *  other receiver location information as height) and passes them to
          *  setReceiverHeight() method and correction(elevation) method.
          *
          * @param RX  Receiver position
          * @param SV  Satellite position
          * @param tt  Time. In this model, tt is a dummy parameter kept just
          *            for consistency
          * @throw InvalidTropModel
          */
      virtual double correction( const Position& RX,
                                 const Position& SV,
                                 const CommonTime& tt )
      { return correction(RX, SV); };


         /** \deprecated
          * Compute and return the full tropospheric delay, given the positions
          * of receiver and satellite and the time tag. This version is most
          * useful within positioning algorithms, where the receiver position
          * and timetag may vary; it computes the elevation (and other receiver
          * location information as height) and passes them to
          * setReceiverHeight() method and correction(elevation) method.
          *
          * @param RX  Receiver position in ECEF cartesian coordinates (meters)
          * @param SV  Satellite position in ECEF cartesian coordinates
          *            (meters)
          * @param tt  Time. In this model, tt is a dummy parameter kept just
          *            for consistency
          * @throw InvalidTropModel
          */
      virtual double correction( const Xvt& RX,
                                 const Xvt& SV,
                                 const CommonTime& tt );


         /** Compute and return the zenith delay for dry component of the
          *  troposphere.
          * @throw InvalidTropModel
          */
      virtual double dry_zenith_delay(void) const;


         /** Compute and return the zenith delay for wet component of the
          *  troposphere.
          * @throw InvalidTropModel
          */
      virtual double wet_zenith_delay(void) const
      { return 0.1; };


         /** Compute and return the mapping function for both components of
          *  the troposphere.
          *
          * @param elevation  Elevation of satellite as seen at receiver, in
          *                   degrees
          * @throw InvalidTropModel
          */
      virtual double mapping_function(double elevation) const;


         /** Compute and return the mapping function for dry component
          *  of the troposphere.
          * @param elevation  Elevation of satellite as seen at receiver, in
          *                   degrees
          * @throw InvalidTropModel
          */
      virtual double dry_mapping_function(double elevation) const
      { return mapping_function(elevation); };


         /** Compute and return the mapping function for wet component
          *  of the troposphere.
          *
          * @param elevation  Elevation of satellite as seen at receiver, in
          *                   degrees
          * @throw InvalidTropModel
          */
      virtual double wet_mapping_function(double elevation) const
      { return mapping_function(elevation); };


         /** In GCAT tropospheric model, this is a dummy method kept here just
          *  for consistency.
          * @throw InvalidParameter
          */
      virtual void setWeather( const double& T,
                               const double& P,
                               const double& H )
      {}


         /** In GCAT tropospheric model, this is a dummy method kept here just
          *  for consistency.
          * @throw InvalidParameter
          */
      virtual void setWeather(const WxObservation& wx)
      {}


         /** Define the receiver height; this is required before calling
          * correction() or any of the zenith_delay routines.
          *
          * @param ht   Height of the receiver above mean sea level, in meters.
          */
      virtual void setReceiverHeight(const double& ht);


   private:

         /// Receiver height
      double gcatHeight;
   };
}
#endif
