#pragma ident "$Id$"

/**
 * @file Synchronize.cpp
 * This class synchronizes two GNSS Data Structures data streams.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008
//
//============================================================================


#include "Synchronize.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int Synchronize::classIndex = 8100000;


      // Returns an index identifying this object.
   int Synchronize::getIndex(void) const
   { return index; }


      // Returns a string identifying this object.
   std::string Synchronize::getClassName() const
   { return "Synchronize"; }



      /* Sets tolerance, in seconds.
       *
       * @param tol                 Tolerance, in seconds.
       */
   Synchronize& Synchronize::setTolerance(const double tol)
   {

         // Make sure that tolerance is positive
      if( tol >= 0.0 )
      {
         tolerance = tol;
      }

      return (*this);

   }  // End of method 'Synchronize::setTolerance()'



      /* Returns a gnnsRinex object, adding the new data generated
       * when calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& Synchronize::Process(gnssRinex& gData)
      throw(SynchronizeException)
   {

      DayTime rtime( (*pgRov1).header.epoch );

      if (firstTime)
      {
         (*pRinexRef) >> gData;      // Get data out of ref station RINEX file
         firstTime = false;          // Mark that first data batch was read

      }

         // Check that the reference data time stamp is not less than gData's,
         // and that tolerance is within limits. If not, keep reading.
         // Note that if reference data time stamp is bigger, it will not
         // enter here, "waiting" for gData to catch up.

      while ( ( gData.header.epoch < rtime ) &&
              (std::abs( gData.header.epoch - rtime ) > tolerance ) )
      {
         (*pRinexRef) >> gData;   // Get data out of ref station RINEX file
      }


         // If we couldn't synchronize data streams (i.e.: "tolerance"
         // is not met), skip this epoch.
      if ( std::abs( gData.header.epoch - rtime ) > tolerance )
      {
            // If synchronization is not possible, we issue an exception
         SynchronizeException e( "Unable to synchronize data at epoch "
                                 + gData.header.epoch.asString() );
         GPSTK_THROW(e);
      }


      return gData;

   }  // End of method 'Synchronize::Process()'



      /* Returns a gnnsSatTypeValue object, adding the new data
       * generated when calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& Synchronize::Process(gnssSatTypeValue& gData)
      throw(SynchronizeException)
   {

      DayTime rtime( (*pgRov2).header.epoch );

      if (firstTime)
      {
         (*pRinexRef) >> gData;      // Get data out of ref station RINEX file
         firstTime = false;          // Mark that first data batch was read

      }

         // Check that the reference data time stamp is not less than gData's,
         // and that tolerance is within limits. If not, keep reading.
         // Note that if reference data time stamp is bigger, it will not
         // enter here, "waiting" for gData to catch up.

      while ( ( gData.header.epoch < rtime ) &&
              (std::abs( gData.header.epoch - rtime ) > tolerance ) )
      {
         (*pRinexRef) >> gData;   // Get data out of ref station RINEX file
      }


         // If we couldn't synchronize data streams (i.e.: "tolerance"
         // is not met), skip this epoch.
      if ( std::abs( gData.header.epoch - rtime ) > tolerance )
      {
            // If synchronization is not possible, we issue an exception
         SynchronizeException e( "Unable to synchronize data at epoch "
                                 + gData.header.epoch.asString() );
         GPSTK_THROW(e);
      }


      return gData;

   }  // End of method 'Synchronize::Process()'



}  // End of namespace gpstk
