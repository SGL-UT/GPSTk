#pragma ident "$Id$"

/**
 * @file ComputeCombination.cpp
 * This is the base class to ease computing combination of data for
 * GNSS data structures.
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


#include "ComputeCombination.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int ComputeCombination::classIndex = 2000000;


      // Returns an index identifying this object.
   int ComputeCombination::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string ComputeCombination::getClassName() const
   { return "ComputeCombination"; }



      /* Returns a satTypeValueMap object, adding the new data generated
       * when calling this object.
       *
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ComputeCombination::Process(satTypeValueMap& gData)
      throw(ProcessingException)
   {

      try
      {

         double value1(0.0);
         double value2(0.0);

         SatIDSet satRejectedSet;

            // Loop through all the satellites
         satTypeValueMap::iterator it;
         for( it = gData.begin(); it != gData.end(); ++it ) 
         {

            try
            {
                  // Try to extract the values
               value1 = (*it).second(type1);
               value2 = (*it).second(type2);
            }
            catch(...)
            {
                  // If some value is missing, schedule this satellite
                  // for removal
               satRejectedSet.insert( (*it).first );
               continue;
            }

               // If everything is OK, then get the new value inside
               // the structure
            (*it).second[resultType] = getCombination(value1, value2);

         }

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ComputeCombination::Process()'


} // End of namespace gpstk
