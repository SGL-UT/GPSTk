#pragma ident "$Id$"

/**
 * @file StochasticModel.cpp
 * Base class to define stochastic models, plus implementations
 * of common ones.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
//
//============================================================================


#include "StochasticModel.hpp"


namespace gpstk
{


      // Get element of the process noise matrix Q
   double RandomWalkModel::getQ()
   {

         // Compute current variance
      double variance(qprime*std::abs(currentTime - previousTime));

         // Return variance
      return variance;

   }  // End of method 'PhaseAmbiguityModel::getQ()'



      /* This method provides the stochastic model with all the available
       * information and takes appropriate actions.
       *
       * @param type       Type of variable.
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
   void RandomWalkModel::Prepare( const TypeID& type,
                                  const SatID& sat,
                                  gnssSatTypeValue& gData )
   {

         // Update previous epoch
      setPreviousTime(currentTime);

      setCurrentTime(gData.header.epoch);

      return;

   }  // End of method 'RandomWalkModel::Prepare()'



      /* This method provides the stochastic model with all the available
       * information and takes appropriate actions. By default, it does
       * nothing.
       *
       * @param type       Type of variable.
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
   void RandomWalkModel::Prepare( const TypeID& type,
                                  const SatID& sat,
                                  gnssRinex& gData )
   {

         // Update previous epoch
      setPreviousTime(currentTime);

      setCurrentTime(gData.header.epoch);

      return;

   }  // End of method 'RandomWalkModel::Prepare()'



      // Get element of the state transition matrix Phi
   double PhaseAmbiguityModel::getPhi()
   {

         // Check if there is a cycle slip
      if(cycleSlip)
      {
         return 0.0;
      }
      else
      {
         return 1.0;
      }

   }  // End of method 'PhaseAmbiguityModel::getPhi()'



      // Get element of the process noise matrix Q
   double PhaseAmbiguityModel::getQ()
   {

         // Check if there is a cycle slip
      if(cycleSlip)
      {
         return variance;
      }
      else
      {
         return 0.0;
      }

   }  // End of method 'PhaseAmbiguityModel::getQ()'



      /* This method checks if a cycle slip happened.
       *
       * @param type       Type of variable.
       * @param sat        Satellite.
       * @param data       Object holding the data.
       * @param source     Object holding the source of data.
       *
       */
   void PhaseAmbiguityModel::checkCS( const TypeID& type,
                                      const SatID& sat,
                                      satTypeValueMap& data,
                                      SourceID& source )
   {

      try
      {

            // By default, assume there is no cycle slip
         setCS(false);

            // Check if satellite is present at this epoch
         if( data.find(sat) == data.end() )
         {
            // If satellite is not present, declare CS and exit
            setCS(true);

            return;
         }


         if (!watchSatArc)
         {
               // In this case, we only use cycle slip flags
               // Check if there was a cycle slip
            if (data(sat)(type) > 0.0)
            {
               setCS(true);
            }

         }
         else
         {
               // Check if this satellite has previous entries
            if( satArcMap[ source ].find(sat) == satArcMap[ source ].end() )
            {
                  // If it doesn't have an entry, insert one
               satArcMap[ source ][ sat ] = 0.0;
            };

               // Check if arc number is different than arc number in storage
            if ( data(sat)(TypeID::satArc) != satArcMap[ source ][ sat ] )
            {
               setCS(true);
               satArcMap[ source ][ sat ] = data(sat)(TypeID::satArc);
            }

         }


      }
      catch(Exception& e)
      {
         setCS(true);
      }

      return;

   } // End of 'PhaseAmbiguityModel::checkCS()'



}  // End of namespace gpstk
