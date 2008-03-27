#pragma ident "$Id$"

/**
 * @file SimpleFilter.cpp
 * This class filters out satellites with observations grossly out of bounds.
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


#include "SimpleFilter.hpp"


namespace gpstk
{

      // Returns a satTypeValueMap object, filtering the target observables.
      //
      // @param gData     Data object holding the data.
      //
   satTypeValueMap& SimpleFilter::Process(satTypeValueMap& gData)
   {

      SatIDSet satRejectedSet;

         // Check all the indicated TypeID's
      TypeIDSet::const_iterator pos;
      for (pos = filterTypeSet.begin(); pos != filterTypeSet.end(); ++pos)
      {

         double value(0.0);

            // Loop through all the satellites
         satTypeValueMap::iterator it;
         for (it = gData.begin(); it != gData.end(); ++it) 
         {
            try
            {
                  // Try to extract the values
               value = (*it).second(*pos);

                  // Now, check that the value is within bounds
               if ( !( checkValue(value) ) )
               {
                     // If value is out of bounds, then schedule this
                     // satellite for removal
                   satRejectedSet.insert( (*it).first );
               }
            }
            catch(...)
            {
                  // If some value is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*it).first );
            }
         }

            // Before checking next TypeID, let's remove satellites with data
            // out of bounds
         gData.removeSatID(satRejectedSet);
      }

      return gData;

   }  // end SimpleFilter::Process()



      // Returns a gnnsSatTypeValue object, filtering the target observables.
      //
      // @param gData    Data object holding the data.
      //
   gnssSatTypeValue& SimpleFilter::Process(gnssSatTypeValue& gData)
   {
      Process(gData.body);
      return gData;
   }


      // Returns a gnnsRinex object, filtering the target observables.
      //
      // @param gData    Data object holding the data.
      //
   gnssRinex& SimpleFilter::Process(gnssRinex& gData)
   {
      Process(gData.body);
      return gData;
   }


      // Index initially assigned to this class
   int SimpleFilter::classIndex = 1000000;


      // Returns an index identifying this object.
   int SimpleFilter::getIndex() const
   { return (*this).index; }


      // Returns a string identifying this object.
   std::string SimpleFilter::getClassName() const
   { return "SimpleFilter"; }


} // end namespace gpstk
