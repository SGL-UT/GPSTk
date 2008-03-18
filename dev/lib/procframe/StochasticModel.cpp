#pragma ident "$Id: $"

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
      double variance(qprime*(currentTime - previousTime));

         // Update previous epoch
      setPreviousTime(currentTime);

         // Return variance
      return variance;

   }



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
   }



      /* This method provides the stochastic model with all the available
       * information and takes appropriate actions. By default, it does
       *  nothihg.
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
   }



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

   }



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

   }



      /* This method provides the stochastic model with all the available
       * information and takes appropriate actions.
       *
       * @param type       Type of variable.
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
   void PhaseAmbiguityModel::Prepare( const TypeID& type,
                                      const SatID& sat,
                                      gnssSatTypeValue& gData )
   {
      try
      {
            // Check if there was a cycle slip
         if (gData(sat)(type) > 0.0)
         {
            setCS(true);
         }
         else
         {
            setCS(false);
         }
      }
      catch(Exception& e)
      {
         setCS(true);
      }

      return;
   }



      /* This method provides the stochastic model with all the available
       * information and takes appropriate actions. By default, it does
       *  nothihg.
       *
       * @param type       Type of variable.
       * @param sat        Satellite.
       * @param gData      Data object holding the data.
       *
       */
   void PhaseAmbiguityModel::Prepare( const TypeID& type,
                                      const SatID& sat,
                                      gnssRinex& gData )
   {
      try
      {
            // Check if there was a cycle slip
         if (gData(sat)(type) > 0.0)
         {
            setCS(true);
         }
         else
         {
            setCS(false);
         }
      }
      catch(Exception& e)
      {
         setCS(true);
      }

      return;
   }



} // end namespace gpstk
