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
 * @file Dumper.hpp
 * This class dumps the values inside a GNSS Data Structure.
 */

#include "Dumper.hpp"


namespace gpstk
{

      // Returns a string identifying this object.
   std::string Dumper::getClassName() const
   { return "Dumper"; }



      /* Dumps data from a satTypeValueMap object.
       *
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& Dumper::Process( satTypeValueMap& gData )
      throw(ProcessingException)
   {

      try
      {

            // Iterate through all items in the GNSS Data Structure
         for( satTypeValueMap::const_iterator it = gData.begin();
              it!= gData.end();
              it++ )
         {

               // First, print satellite (system and PRN)
            *outStr << (*it).first << " ";

               // Now, print TypeIDs
            printTypeID( (*it).second );

               // Print end of line
            *outStr << std::endl;

         }  // End of 'for( satTypeValueMap::const_iterator it = ...'

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'Dumper::Process()'



      /* Dumps data from a gnnsRinex object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& Dumper::Process( gnssRinex& gData )
      throw(ProcessingException)
   {

      try
      {

            // Iterate through all items in the GNSS Data Structure
         for( satTypeValueMap::const_iterator it = gData.body.begin();
              it!= gData.body.end();
              it++ )
         {

               // First, print year, Day-Of-Year and Seconds of Day (if enabled)
            if( printTime )
            {
                  // Declare a 'YDSTime' object to ease printing
               YDSTime time( gData.header.epoch );

               *outStr << time.year << " "
                       << time.doy << " "
                       << time.sod << " ";
            }

               // Second, print SourceID information (if enabled)
            if( printStation )
            {
               *outStr << gData.header.source << " ";
            }

               // Then, print satellite (system and PRN)
            *outStr << (*it).first << " ";

               // Now, print TypeIDs
            printTypeID( (*it).second );

               // Print end of line
            *outStr << std::endl;

         }  // End of 'for( satTypeValueMap::const_iterator it = ...'

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'Dumper::Process()'



      /* Method to add a set of TypeIDs to be printed.
       *
       * @param printSet      TypeIDSet of data values to be added to the
       *                      ones being printed.
       */
   Dumper& Dumper::addTypeSet( const TypeIDSet& printSet )
   {

         // Iterate over 'printSet' and add its components to 'printTypeSet'
      for( TypeIDSet::const_iterator pos = printSet.begin();
           pos != printSet.end();
           ++pos )
      {
         printTypeSet.insert(*pos);
      }

      return (*this);

   }  // End of method 'Dumper::addTypeSet()'



      // Print TypeIDs information.
   void Dumper::printTypeID( const typeValueMap& tvMap )
   {

         // Iterate through all 'tvMap'
      for( typeValueMap::const_iterator itObs = tvMap.begin();
           itObs != tvMap.end();
           itObs++ )
      {

            // Check if we have specific TypeIDs to be printed or not
         if( printTypeSet.size() > 0 )
         {

               // Check if current TypeID is in 'printTypeSet'
            if( printTypeSet.find((*itObs).first) != printTypeSet.end() )
            {
               if( printType )
               {
                  *outStr << (*itObs).first << " ";
               }

               *outStr << (*itObs).second << " ";
            }

         }
         else
         {
               // No specific TypeIDs, so lets print them all
            if( printType )
            {
               *outStr << (*itObs).first << " ";
            }

            *outStr << (*itObs).second << " ";

         }  // End of 'if( printTypeSet.size() > 0 )'

      }  // End of 'for( typeValueMap::const_iterator itObs = ... )'

      return;

   }  // End of method 'Dumper::printTypeID()'


}  // End of namespace gpstk
