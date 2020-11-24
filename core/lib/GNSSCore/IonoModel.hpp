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

/**
 * @file IonoModel.cpp
 * Implementation of the IS-GPS-200 Ionosphere model (20.3.3.5.2.5).
 */

#ifndef GPSTK_IONOMODEL_HPP
#define GPSTK_IONOMODEL_HPP

#include "CommonTime.hpp"
#include "CarrierBand.hpp"
#include "EngAlmanac.hpp"
#include "Position.hpp"

namespace gpstk
{
      /// @ingroup GPSsolutions
      //@{

      /**
       * Simple model of the ionosphere ("Klobuchar"), specified in the GPS IS.
       * It is used to compute the satellite signal ionospheric delay seen at
       * the receiver by a single-band user.
       *
       * See ICD-GPS-200, section 20.3.3.5.2.5 and  Figure 20-4.
       *
       * Verbatim from the above section:
       * "It is estimated that the use of this model will provide at least
       * a 50% reduction in the single-frequency user's RMS error due to
       * ionospheric propagation effects. During extended operations, or for
       * the Block IIR SVs in the Autonav mode if the CS is unable to upload
       * the SVs, the use of this model will yield unpredictable results."
       */
   class IonoModel
   {
   public:
        
         /// Exception, thrown when attempting to use a model for which not all
         /// the necessary parameters have been specified.
         /// @ingroup exceptiongroup
      NEW_EXCEPTION_CLASS(InvalidIonoModel, gpstk::Exception);
 
         /// Default constructor, creates an invalid model for lack of parameters.
      IonoModel() throw() : valid(false) {}
      
         /// Destructor.
      virtual ~IonoModel() throw() {}
      
         /**
          * Ionosphere model constructor.
          * Creates a valid model with satellite transmitted alpha and beta
          * (Klobuchar) parameters provided by the user.
          * @param a An array containing the four alpha terms.
          * @param b An array containing the four beta terms.
          * @param semicircle_units A boolean indicating params are in
          *                         semicircles (T, default) or radians (F).
          * Note that the IS-GPS-200 defines the algorithm and parameters
          * in terms of semi-circles, not radians; but that the GPSTk for
          * historical reasons extracts parameters from a GPS Nav message
          * in power of inverse radians.  Hence the need for the boolean flag.
          */
      IonoModel(const double a[4], const double b[4], const bool semicircle_units = true) throw();
      
         /**
          * EngAlmanac constructor.
          * Creates a valid model from and EngAlmanac object.
          * @param engalm An EngAlmanac object.
          */
      IonoModel(const EngAlmanac& engalm) throw();
      
         /**
          * Method to feed the model with satellite-transmitted alpha and beta
          * parameters from the passed almanac.
          * See the IS-GPS-200, 20.3.3.3.3.2.
          * @param a An array containing the four alpha terms.
          * @param b An array containing the four beta terms.
          */
      void setModel(const double a[4], const double b[4], const bool semicircle_units = true) throw();
      
         /**
          * returns the validity of the model.
          * @return model validity
          */
      bool isValid() const throw() { return valid; }
      
         /**
          * Get the ionospheric correction value.
          * @param time The time of the observation.
          * @param rxgeo The WGS84 geodetic position of the receiver.
          * @param svel The elevation angle between the rx and SV (degrees).
          * @param svaz The azimuth angle between the rx and SV (degrees).
          * @param band The GPS frequency band the observation was made from.
          * @return The ionospheric correction (meters).
          * @throw InvalidIonoModel
          */
      double getCorrection(const CommonTime& time,
                           const Position& rxgeo,
                           double svel,
                           double svaz,
                           CarrierBand band = CarrierBand::L1) const;

         /// Equality operator
      bool operator==(const IonoModel& right) const throw();

         /// Inequality operator
      bool operator!=(const IonoModel& right) const throw();     

   private:

      double alpha[4];
      double beta[4];

      bool valid;
   };
      //@}
}

#endif
