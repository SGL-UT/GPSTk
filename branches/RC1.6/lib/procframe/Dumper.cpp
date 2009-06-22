#pragma ident "$Id$"

/**
 * @file Dumper.hpp
 * This class dumps the values inside a GNSS Data Structure.
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


#include "Dumper.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int Dumper::classIndex = 8200000;


      // Returns an index identifying this object.
   int Dumper::getIndex() const
   { return index; }


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
                                + StringUtils::asString( getIndex() ) + ":"
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
               *outStr << gData.header.epoch.year() << " "
                       << gData.header.epoch.DOY() << " "
                       << gData.header.epoch.DOYsecond() << " ";
            }

               // Second, print SourceID information (if enabled)
            if( printTime )
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
                                + StringUtils::asString( getIndex() ) + ":"
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
