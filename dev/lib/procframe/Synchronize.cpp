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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2011
//
//============================================================================


#include "Synchronize.hpp"


namespace gpstk
{

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
      CommonTime time = dynamic_cast<gnssRinex*>(pgRov1)->header.epoch;
      Process(time,gData);

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
      CommonTime time(pgRov1->header.epoch);
      
      gnssRinex gRin;
      Process(time,gRin);
      
      gData.header = gRin.header;
      gData.body = gRin.body;

      return gData;

   }  // End of method 'Synchronize::Process()'


   gnssRinex& Synchronize::Process(CommonTime time, gnssRinex& gData)
      throw(SynchronizeException)
   {
      
      if (firstTime)
      {
         (*pRinexRef) >> gData;      // Get data out of ref station RINEX file

         gnssRinexBuffer.clear();
         gnssRinexBuffer.push_back(gData);

         firstTime = false;          // Mark that first data batch was read
      }

      gData = gnssRinexBuffer.front();

      if( (gData.header.epoch > time) && 
         (std::abs( gData.header.epoch - time ) > tolerance ))
      {
         // If synchronization is not possible, we issue an exception
         SynchronizeException e( "Unable to synchronize data at epoch "
            + time.asString() );
         GPSTK_THROW(e);
      }

      // Check that the reference data time stamp is not less than gData's,
      // and that tolerance is within limits. If not, keep reading.
      // Note that if reference data time stamp is bigger, it will not
      // enter here, "waiting" for gData to catch up.

      while ( ( gData.header.epoch < time ) &&
         (std::abs( gData.header.epoch - time ) > tolerance ) )
      {
         (*pRinexRef) >> gData;   // Get data out of ref station RINEX file
      }


      // If we couldn't synchronize data streams (i.e.: "tolerance"
      // is not met), skip this epoch.
      if ( std::abs( gData.header.epoch - time ) > tolerance )
      {
         // If synchronization is not possible, we issue an exception
         SynchronizeException e( "Unable to synchronize data at epoch "
            + time.asString() );
         GPSTK_THROW(e);
      }


      return gData;
   }  // End of method 'Synchronize::Process(CommonTime time, gnssRinex& gData)'

}  // End of namespace gpstk
