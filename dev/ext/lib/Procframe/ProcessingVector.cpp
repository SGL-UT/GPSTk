#pragma ident "$Id$"

/**
 * @file ProcessingVector.cpp
 * This is a class to store ProcessingClass objects in a vector.
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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2011
//
//============================================================================


#include "ProcessingVector.hpp"


namespace gpstk
{

      // Returns a string identifying this object.
   std::string ProcessingVector::getClassName(void) const
   { return "ProcessingVector"; }



      /* Processing method. It returns a gnnsSatTypeValue object.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& ProcessingVector::Process(gnssSatTypeValue& gData)
   {

      try
      {

         std::vector<ProcessingClass*>::const_iterator pos;
         for (pos = procvector.begin(); pos != procvector.end(); ++pos)
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

   }  // End of method 'ProcessingVector::Process()'



      /* Processing method. It returns a gnnsRinex object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& ProcessingVector::Process(gnssRinex& gData)
   {

      try
      {

         std::vector<ProcessingClass*>::const_iterator pos;
         for (pos = procvector.begin(); pos != procvector.end(); ++pos)
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

   }  // End of method 'ProcessingVector::Process()'


}  // End of namespace gpstk
