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
 * @file IonoModelStore.hpp
 * Store GPS Navigation Message based ionospheric models
 */


#ifndef GPSTK_IONOMODELSTORE_HPP
#define GPSTK_IONOMODELSTORE_HPP

#include <map>
#include "CommonTime.hpp"
#include "CarrierBand.hpp"
#include "IonoModel.hpp"

namespace gpstk
{
      /// @ingroup GPSsolutions
      //@{

      /**
       * This class defines an interface to hide how we determine
       * the ionospheric delay as determined from GPS navigation message
       * based models at some point in time
       */
   class IonoModelStore
   {
   public:


         /**
          * Thrown when attempting to get a model that isn't stored.
          * @ingroup exceptiongroup
          */
      NEW_EXCEPTION_CLASS(NoIonoModelFound, gpstk::Exception);


         /// constructor
      IonoModelStore() {}


         /// destructor
      virtual ~IonoModelStore() {}


         /** Get the ionospheric correction value.
          *
          * @param time the time of the observation
          * @param rxgeo the WGS84 geodetic position of the receiver
          * @param svel the elevation angle between the rx and SV (degrees)
          * @param svaz the azimuth angle between the rx and SV (degrees)
          * @param freq the GPS band the observation was made from
          * @return the ionospheric correction (meters)
          * @throw NoIonoModelFound
          */
      virtual double getCorrection(const CommonTime& time,
                                   const Position& rxgeo,
                                   double svel,
                                   double svaz,
                                   CarrierBand band = CarrierBand::L1)
         const;


         /** Add an IonoModel to this collection
          *
          * @param mt the time the model is valid from
          * @param im the IonoModel to add
          * @return true if the model was added, false otherwise
          */
      bool addIonoModel( const CommonTime& mt,
                         const IonoModel& im )
         throw();

         /** Edit the dataset, removing data outside the indicated time interval
          *
          * @param tmin defines the beginning of the time interval
          * @param tmax defines the end of the time interval
          */
      void edit(const CommonTime& tmin, 
                const CommonTime& tmax = CommonTime::END_OF_TIME);

   private:


      typedef std::map<CommonTime, IonoModel> IonoModelMap;

      IonoModelMap ims;


   }; // End of class 'IonoModelStore'
   
      //@}

}  // End of namespace gpstk

#endif   // GPSTK_IONOMODELSTORE_HPP
