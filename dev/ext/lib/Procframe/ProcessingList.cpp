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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2011
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
 * @file ProcessingList.cpp
 * This is a class to store ProcessingClass objects in a list.
 */

#include "ProcessingList.hpp"


namespace gpstk
{

      // Returns a string identifying this object.
   std::string ProcessingList::getClassName() const
   { return "ProcessingList"; }



      /* Processing method. It returns a gnnsSatTypeValue object.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& ProcessingList::Process(gnssSatTypeValue& gData)
   {

      try
      {

         std::list<ProcessingClass*>::const_iterator pos;
         for (pos = proclist.begin(); pos != proclist.end(); ++pos)
         {
            (*pos)->Process(gData);
         }

         return gData;

      }
      catch(...)
      {

            // This method must throw the same exceptions it may get from
            // the 'ProcessingList' elements, without altering them.
         throw;

      }

   }  // End of method 'ProcessingList::Process()'



      /* Processing method. It returns a gnnsRinex object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& ProcessingList::Process(gnssRinex& gData)
   {

      try
      {

         std::list<ProcessingClass*>::const_iterator pos;
         for (pos = proclist.begin(); pos != proclist.end(); ++pos)
         {
            (*pos)->Process(gData);
         }

         return gData;

      }
      catch(...)
      {

            // This method must throw the same exceptions it may get from
            // the 'ProcessingList' elements, without altering them.
         throw;

      }

   }  // End of method 'ProcessingList::Process()'


}  // End of namespace gpstk
