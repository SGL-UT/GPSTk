#pragma ident "$Id$"

/**
 * @file ComputeIURAWeights.cpp
 * This class computes satellites weights based on URA Index and is
 * meant to be used with GNSS data structures.
 */

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007
//
//============================================================================


#include "ComputeIURAWeights.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int ComputeIURAWeights::classIndex = 4000000;


      // Returns an index identifying this object.
   int ComputeIURAWeights::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string ComputeIURAWeights::getClassName() const
   { return "ComputeIURAWeights"; }


      /* Returns a satTypeValueMap object, adding the new data generated
       * when calling this object.
       *
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ComputeIURAWeights::Process( const DayTime& time,
                                                 satTypeValueMap& gData )
   {

         // By default set a very small value to weight
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

      }

         // Remove satellites with missing data
      gData.removeSatID(satRejectedSet);

      return gData;

   }


      /* Method to set the default ephemeris to be used with GNSS
       * data structures.
       *
       * @param ephem     EphemerisStore object to be used
       */
   void ComputeIURAWeights::setDefaultEphemeris(XvtStore<SatID>& ephem)
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
         pTabEphemeris = dynamic_cast<TabularEphemerisStore*>(&ephem);
      }

   }


      /* Method to really get the weight of a given satellite.
       *
       * @param sat           Satellite
       * @param time          Epoch
       * @param preciseEph    Precise ephemerisStore object to be used
       */
   double ComputeIURAWeights::getWeight( const SatID& sat,
                                         const DayTime& time,
                                    const TabularEphemerisStore* preciseEph )
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

   }


      /* Method to really get the weight of a given satellite.
       *
       * @param sat       Satellite
       * @param time      Epoch
       * @param bcEph     Broadcast EphemerisStore object to be used
       */
   double ComputeIURAWeights::getWeight( const SatID& sat,
                                         const DayTime& time,
                                         const GPSEphemerisStore* bcEph )
      throw(InvalidWeights)
   {

         // Set by default a very big value
      int iura(1000000);

      double sigma(1000000.0);

      EngEphemeris engEph;

      try
      {
            // Look if this satellite is present in ephemeris
         engEph = bcEph->findEphemeris(sat, time);

            // If so, get the IURA
         iura = engEph.getAccFlag();

      }
      catch(...)
      {
         InvalidWeights eWeight("Satellite not found.");
         GPSTK_THROW(eWeight);
      }

         // Compute and return the weight
      sigma = gpstk::ura2nominalAccuracy(iura);

      return ( 1.0 / (sigma*sigma) );

   }


} // end namespace gpstk
