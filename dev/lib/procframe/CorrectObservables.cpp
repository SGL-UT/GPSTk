#pragma ident "$Id: $"

/**
 * @file CorrectObservables.cpp
 * This class corrects observables from effects such as antenna excentricity,
 * difference in phase centers, offsets due to tide effects, etc.
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


#include "CorrectObservables.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int CorrectObservables::classIndex = 1400000;


      // Returns an index identifying this object.
   int CorrectObservables::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string CorrectObservables::getClassName() const
   { return "CorrectObservables"; }



      /* Returns a satTypeValueMap object, adding the new data generated 
       * when calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& CorrectObservables::Process(const DayTime& time,
                                                satTypeValueMap& gData)
   {

         // Compute station latitude and longitude
      double lat(nominalPos.geodeticLatitude());
      double lon(nominalPos.longitude());

         // Define station position as a Triple, in ECEF
      Triple staPos(nominalPos.getX(), nominalPos.getY(), nominalPos.getZ());

         // Compute displacement vectors for L1 and L2, in meters [UEN]
      Triple dispL1(extraBiases + monumentVector + L1PhaseCenter);
      Triple dispL2(extraBiases + monumentVector + L2PhaseCenter);

         // Object to store satellite Xvt
      Xvt svPosVel;

      SatIDSet satRejectedSet;

         // Loop through all the satellites
      satTypeValueMap::iterator it;
      for (it = gData.begin(); it != gData.end(); ++it) 
      {

            // Use this if satellite position is not already computed
            // NOTE: This may change in the future, if some other class
            // already provides satellite position, improving performance
         if(true)
         {
               // Try to get satellite position if it is not already computed
            try
            {
                  // For our purposes, position at receive time is fine enough
               svPosVel = ephemeris.getXvt( (*it).first, time );
            }
            catch(...)
            {
                  // If satellite is missing, then schedule it for removal
               satRejectedSet.insert( (*it).first );
               continue;
             }
         }

            // If everything is OK, then continue processing.
            // Create a Triple with satellite position, in ECEF
         Triple svPos( svPosVel.x.theArray[0],
                       svPosVel.x.theArray[1],
                       svPosVel.x.theArray[2] );

            // Compute vector station-satellite, in ECEF
         Triple ray(svPos - staPos);

            // Rotate vector ray to UEN reference frame
         ray = ray.R3(lon).R2(-lat);

            // Convert ray to an unitary vector
         ray = ray.unitVector();

            // Compute corrections = displacement vectors components 
            // along ray direction.
         double corrL1(dispL1.dot(ray));
         double corrL2(dispL2.dot(ray));


            // Find which observables are present, and then apply corrections

            // Look for C1
         if( (*it).second.find(TypeID::C1) != (*it).second.end() )
         {
            (*it).second[TypeID::C1] = (*it).second[TypeID::C1] + corrL1;
         };
            // Look for P1
         if( (*it).second.find(TypeID::P1) != (*it).second.end() )
         {
            (*it).second[TypeID::P1] = (*it).second[TypeID::P1] + corrL1;
         };
            // Look for L1
         if( (*it).second.find(TypeID::L1) != (*it).second.end() )
         {
            (*it).second[TypeID::L1] = (*it).second[TypeID::L1] + corrL1;
         };

            // Look for C2
         if( (*it).second.find(TypeID::C2) != (*it).second.end() )
         {
            (*it).second[TypeID::C2] = (*it).second[TypeID::C2] + corrL2;
         };
            // Look for P2
         if( (*it).second.find(TypeID::P2) != (*it).second.end() )
         {
            (*it).second[TypeID::P2] = (*it).second[TypeID::P2] + corrL2;
         };
            // Look for L2
         if( (*it).second.find(TypeID::L2) != (*it).second.end() )
         {
            (*it).second[TypeID::L2] = (*it).second[TypeID::L2] + corrL2;
         };

      }

         // Remove satellites with missing data
      gData.removeSatID(satRejectedSet);

      return gData;

   } // End CorrectObservables::Process()


} // end namespace gpstk
