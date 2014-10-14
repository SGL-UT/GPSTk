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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2006, 2007, 2008, 2011
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
 * @file ComputeIURAWeights.cpp
 * This class computes satellites weights based on URA Index and is
 * meant to be used with GNSS data structures.
 */

#include "ComputeIURAWeights.hpp"


namespace gpstk
{

      // Returns a string identifying this object.
   std::string ComputeIURAWeights::getClassName() const
   { return "ComputeIURAWeights"; }



      /* Returns a satTypeValueMap object, adding the new data generated
       * when calling this object.
       *
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ComputeIURAWeights::Process( const CommonTime& time,
                                                 satTypeValueMap& gData )
      throw(ProcessingException)
   {

      try
      {

            // By default set the wight as a very small value
         double weight(0.000001);

         SatIDSet satRejectedSet;

               // Loop through all the satellites
         satTypeValueMap::iterator it;
         for( it = gData.begin(); it != gData.end(); ++it )
         {

            try
            {

                  // Try to extract the weight value
               if( pBCEphemeris != NULL )
               {
                  weight = getWeight( ((*it).first), time, pBCEphemeris );
               }
               else
               {

                  if( pTabEphemeris != NULL )
                  {
                     weight = getWeight( ((*it).first), time, pTabEphemeris );
                  }
               }
            }
            catch(...)
            {

                  // If some value is missing, then schedule this
                  // satellite for removal
               satRejectedSet.insert( (*it).first );

               continue;

            }

               // If everything is OK, then get the new value inside
               // the GDS structure
            (*it).second[TypeID::weight] = weight;

         }  // End of 'for( it = gData.begin(); it != gData.end(); ++it )'


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

   }  // End of method 'ComputeIURAWeights::Process()'



      /* Method to set the default ephemeris to be used with GNSS
       * data structures.
       *
       * @param ephem     EphemerisStore object to be used
       */
   ComputeIURAWeights& ComputeIURAWeights::setDefaultEphemeris(
                                                   XvtStore<SatID>& ephem )
   {

         // Let's check what type ephem belongs to
      if( dynamic_cast<GPSEphemerisStore*>(&ephem) )
      {
         pBCEphemeris = dynamic_cast<GPSEphemerisStore*>(&ephem);
         pTabEphemeris = NULL;
      }
      else
      {
         pBCEphemeris = NULL;
         pTabEphemeris = dynamic_cast<SP3EphemerisStore*>(&ephem);
      }

      return (*this);

   }  // End of method 'ComputeIURAWeights::setDefaultEphemeris()'



      /* Method to really get the weight of a given satellite.
       *
       * @param sat           Satellite
       * @param time          Epoch
       * @param preciseEph    Precise ephemerisStore object to be used
       */
   double ComputeIURAWeights::getWeight( const SatID& sat,
                                         const CommonTime& time,
                                         const SP3EphemerisStore* preciseEph )
      throw(InvalidWeights)
   {

      try
      {
            // Look if this satellite is present in ephemeris
         preciseEph->getXvt(sat, time);
      }
      catch(...)
      {
         InvalidWeights eWeight("Satellite not found.");
         GPSTK_THROW(eWeight);
      }

         // An URA of 0.1 m is assumed for all satellites, 
         // so sigma = 0.1*0.1 = 0.01 m^2
      return 100.0;

   }  // End of method 'ComputeIURAWeights::getWeight()'



      /* Method to really get the weight of a given satellite.
       *
       * @param sat       Satellite
       * @param time      Epoch
       * @param bcEph     Broadcast EphemerisStore object to be used
       */
   double ComputeIURAWeights::getWeight( const SatID& sat,
                                         const CommonTime& time,
                                         const GPSEphemerisStore* bcEph )
      throw(InvalidWeights)
   {

         // Set by default a very big value
      int iura(1000000);

      double sigma(1000000.0);

      try
      {
            // Look if this satellite is present in ephemeris
         const GPSEphemeris& engEph = bcEph->findEphemeris(sat, time);

            // If so, get the IURA
         //iura = engEph.getAccFlag();
         iura = engEph.accuracyFlag;

      }
      catch(...)
      {
         InvalidWeights eWeight("Satellite not found.");
         GPSTK_THROW(eWeight);
      }

         // Compute and return the weight
      sigma = gpstk::ura2nominalAccuracy(iura);

      return ( 1.0 / (sigma*sigma) );

   }  // End of method 'ComputeIURAWeights::getWeight()'



}  // End of namespace gpstk
