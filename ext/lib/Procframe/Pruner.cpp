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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2009, 2011
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
 * @file Pruner.hpp
 * This class examines a GNSS Data Structure (GDS) and prunes specific
 * values according to their TypeIDs.
 */

#include "Pruner.hpp"


namespace gpstk
{

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
