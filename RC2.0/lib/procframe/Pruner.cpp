#pragma ident "$Id$"

/**
 * @file Pruner.hpp
 * This class examines a GNSS Data Structure (GDS) and prunes specific
 * values according to their TypeIDs.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2009
//
//============================================================================


#include "Pruner.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int Pruner::classIndex = 8400000;


      // Returns an index identifying this object.
   int Pruner::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string Pruner::getClassName() const
   { return "Pruner"; }



      /* Prunes data from a satTypeValueMap object.
       *
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& Pruner::Process( satTypeValueMap& gData )
      throw(ProcessingException)
   {

      try
      {

            // Call appropriate GDS method
         if( deleteTypeSet.size() > 0 )
         {
            gData.removeTypeID( deleteTypeSet );
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

   }  // End of method 'Pruner::Process()'



      /* Method to add a set of TypeIDs to be deleted.
       *
       * @param deleteSet     TypeIDSet of data values to be added to the
       *                      ones being deleted.
       */
   Pruner& Pruner::addTypeSet( const TypeIDSet& deleteSet )
   {

         // Iterate over 'deleteSet' and add its components to 'deleteTypeSet'
      for( TypeIDSet::const_iterator pos = deleteSet.begin();
           pos != deleteSet.end();
           ++pos )
      {
         deleteTypeSet.insert(*pos);
      }

      return (*this);

   }  // End of method 'Pruner::addTypeSet()'



}  // End of namespace gpstk
