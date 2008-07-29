#pragma ident "$Id$"

/**
 * @file ProcessingList.cpp
 * This is a class to store ProcessingClass objects in a list.
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


#include "ProcessingList.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int ProcessingList::classIndex = 10000000;


      // Returns an index identifying this object.
   int ProcessingList::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string ProcessingList::getClassName() const
   { return "ProcessingList"; }



      /* Processing method. It returns a gnnsSatTypeValue object.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& ProcessingList::Process(gnssSatTypeValue& gData)
      throw(ProcessingException)
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
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ProcessingList::Process()'



      /* Processing method. It returns a gnnsRinex object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& ProcessingList::Process(gnssRinex& gData)
      throw(ProcessingException)
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
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ProcessingList::Process()'



}  // End of namespace gpstk
