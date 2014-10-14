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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2011 
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
 * @file ComputeSimpleWeights.cpp
 * This class computes simple satellites weights partially based on the
 * variance model by Kennedy, 2002. It is meant to be used with GNSS data
 * structures.
 */

#include "ComputeSimpleWeights.hpp"


namespace gpstk
{

      // Returns a string identifying this object.
   std::string ComputeSimpleWeights::getClassName() const
   { return "ComputeSimpleWeights"; }



      /* Returns a satTypeValueMap object, adding the new data
       * generated when calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ComputeSimpleWeights::Process( const CommonTime& time,
                                                 satTypeValueMap& gData )
      throw(ProcessingException)
   {

      try
      {

            // If we are using a 5th order Taylor-based differencing filter, the
            // corresponding scale factor to convert from covariance matrix to
            // double-differenced covariance matrix is 1.509551839.
         double scaleFact( 1.509551839 );

            // Declare some important constants
         double tropoVar( 0.0004 );    // (0.02 m)^2
         double multiVar( 0.000025 );  // (0.005 m)^2

            // We need a NBTropModel initialized with dummy values
         NBTropModel tropoObj(0.0, 0.0, 1);

         SatIDSet satRejectedSet;


            // Loop through all the satellites
         for( satTypeValueMap::iterator it = gData.begin();
              it != gData.end();
              ++it )
         {

            double elevP( 0.0 );

            try
            {
               elevP = gData.getValue( (*it).first, TypeID::elevation );
            }
            catch(...)
            {

                  // If some value is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*it).first );

               continue;

            }

               // If everything is OK, then compute the weight value and
               // put it into the GDS structure
            double mt( tropoObj.dry_mapping_function(elevP) );

            double weight( 1.0 / ( scaleFact*( mt*mt*tropoVar + multiVar ) ) );

            (*it).second[TypeID::weight] = weight;

         }

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ComputeSimpleWeightsWeights::Process()'



      /* Returns a gnnsSatTypeValue object, adding the new data
       * generated when calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& ComputeSimpleWeights::Process(gnssSatTypeValue& gData)
      throw(ProcessingException)
   {

      try
      {

         Process(gData.header.epoch, gData.body);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ComputeSimpleWeightsWeights::Process()'



      /* Returns a gnnsRinex object, adding the new data generated
       * when calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& ComputeSimpleWeights::Process(gnssRinex& gData)
      throw(ProcessingException)
   {

      try
      {

         Process(gData.header.epoch, gData.body);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ComputeSimpleWeightsWeights::Process()'


}  // End of namespace gpstk
