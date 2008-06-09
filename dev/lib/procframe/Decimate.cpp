#pragma ident "$Id$"

/**
 * @file Decimate.cpp
 * This class decimates GNSS Data Structures data given a sampling interval,
 * a tolerance, and a starting epoch.
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


#include "Decimate.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int Decimate::classIndex = 1600000;


      // Returns an index identifying this object.
   int Decimate::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string Decimate::getClassName() const
   { return "Decimate"; }


      /* Sets sampling interval.
       * @param sampleInterval      Sampling interval, in seconds.
       */
   Decimate& Decimate::setSampleInterval(const double sampleInterval)
   {
         // Make sure that sample interval is positive
      if( sampleInterval >= 0.0 )
      {
         sampling = sampleInterval;
      }

      return (*this);
   }


      /* Sets tolerance, in seconds.
       * @param tol                 Tolerance, in seconds.
       */
   Decimate& Decimate::setTolerance(const double tol)
   {
         // Make sure that tolerance is positive
      if( tol >= 0.0 )
      {
         tolerance = tol;
      }

      return (*this);
   }


      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& Decimate::Process( const DayTime& time,
                                         satTypeValueMap& gData)
      throw(DecimateEpoch)
   {

         // Set a threshold
      double threshold( std::abs(sampling - tolerance) );

         // Check if current epoch - lastEpoch is NOT within threshold,
         // implying that it must be decimated
      if ( !(std::abs(time - lastEpoch) > threshold) )
      {
            // If epoch must be decimated, we issue an Exception
         DecimateEpoch e("This epoch must be decimated.");
         GPSTK_THROW(e);
      }

         // Update reference epoch
      lastEpoch = time;

      return gData;

   }  // End of method Decimate::Process()


} // end namespace gpstk
