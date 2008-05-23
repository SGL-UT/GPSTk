#pragma ident "$Id: GravitationalDelay.cpp $"

/**
 * @file GravitationalDelay.cpp
* This class computes the delay in the signal due to changes in gravity field.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008
//
//============================================================================


#include "GravitationalDelay.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int GravitationalDelay::classIndex = 3100000;


      // Returns an index identifying this object.
   int GravitationalDelay::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string GravitationalDelay::getClassName() const
   { return "GravitationalDelay"; }


      // Constant value needed for computation. This value comes from:
      //    K = (1+gamma) muE / (c*c)
      // where:
      // - gamma = 1.0   (general relativity)
      // - muE = 3.986004418e14 m^3/s^2  (std gravitational parameter, Earth)
      // - c = 2.99792458e8 m/s  (speed of light)
   const double K = 0.887005608e-2;


      /* Returns a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param epoch     Time of observations.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& GravitationalDelay::Process( const DayTime& epoch,
                                           satTypeValueMap& gData )
   {
      SatIDSet satRejectedSet;

         // Define a Triple that will hold satellite position, in ECEF
      Triple svPos(0.0, 0.0, 0.0);

         // Loop through all the satellites
      satTypeValueMap::iterator it;
      for (it = gData.begin(); it != gData.end(); ++it) 
      {
            // Check if satellite position is not already computed
         if( ( (*it).second.find(TypeID::satX) == (*it).second.end() ) ||
             ( (*it).second.find(TypeID::satY) == (*it).second.end() ) ||
             ( (*it).second.find(TypeID::satZ) == (*it).second.end() ) )
         {
               // If satellite position is missing, then schedule this 
               // satellite for removal
            satRejectedSet.insert( (*it).first );
            continue;
         }
         else
         {
               // Get satellite position out of GDS
            svPos[0] = (*it).second[TypeID::satX];
            svPos[1] = (*it).second[TypeID::satY];
            svPos[2] = (*it).second[TypeID::satZ];
         }

            // Get magnitude of satellite position vector
         double r2(svPos.mag());

            // Get vector from Earth mass center to receiver
         Triple rxPos(nominalPos.X(), nominalPos.Y(), nominalPos.Z());

            // Compute magnitude of receiver position vector
         double r1(rxPos.mag());

            // Compute the difference between satellite and receiver positions
         Position difPos(svPos - rxPos);

            // Compute magnitude of the diference between rxPos and svPos
         double r12( difPos.mag() );

            // Compute gravitational delay correction
         double gravDel( K*std::log( (r1+r2+r12)/(r1+r2-r12) ) );

            // Get the correction into the GDS
         (*it).second[TypeID::gravDelay] = gravDel;

      }

         // Remove satellites with missing data
      gData.removeSatID(satRejectedSet);

      return gData;

   }


      /* Returns a gnnsRinex object, adding the new data generated when
       *  calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& GravitationalDelay::Process(gnssRinex& gData)
   {
      Process(gData.header.epoch, gData.body);
      return gData;
   }


} // end namespace gpstk
