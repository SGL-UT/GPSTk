#pragma ident "$Id$"

/**
 * @file ComputeLinear.cpp
 * This class computes linear combinations of GDS data.
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


#include "ComputeLinear.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int ComputeLinear::classIndex = 2900000;


      // Returns an index identifying this object.
   int ComputeLinear::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string ComputeLinear::getClassName() const
   { return "ComputeLinear"; }



      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ComputeLinear::Process( const DayTime& time,
                                            satTypeValueMap& gData )
      throw(ProcessingException)
   {

      try
      {

            // Loop through all the satellites
         satTypeValueMap::iterator it;
         for( it = gData.begin(); it != gData.end(); ++it )
         {

               // Loop through all the defined linear combinations
            LinearCombList::const_iterator pos;
            for( pos = linearList.begin(); pos != linearList.end(); ++pos )
            {

               double result(0.0);

                  // Read the information of each linear combination
               typeValueMap::const_iterator iter;
               for(iter = pos->body.begin(); iter != pos->body.end(); ++iter)
               {
                  double temp(0.0);

                  TypeID type(iter->first);

                  if( (*it).second.find(type) != (*it).second.end() )
                  {
                     temp = (*it).second[type];
                  }
                  else
                  {
                     temp = 0.0;
                  }

                  result = result + (*iter).second * temp;
               }

                  // Store the result in the proper place
               (*it).second[pos->header] = result;

            }

         }

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

   }  // End of method 'ComputeLinear::Process()'


} // End of namespace gpstk
