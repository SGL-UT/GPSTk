#pragma ident "$Id$"

/**
 * @file GloEphemerisStore.cpp
 * Get GLONASS broadcast ephemeris data information
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2011
//
//============================================================================


#include "GloEphemerisStore.hpp"
#include "TimeString.hpp"

namespace gpstk
{


      // Add ephemeris information from a Rinex3NavData object.
   void GloEphemerisStore::addEphemeris(const Rinex3NavData& data)
      throw()
   {

         // If enabled, check SV health before entering here (health = 0 -> OK)
      if( (data.health == 0) || (!checkHealthFlag) )
      {

            // Get a GloEphemeris object from Rinex3NavData object
         GloEphemeris gloEphem(data);
         CommonTime t( data.time );
         SatID sat( data.sat );
         pe[sat][t] = gloEphem; // find or add entry

         if (t < initialTime)
            initialTime = t;
         else if (t > finalTime)
            finalTime = t;

      }  // End of 'if( (data.health == 0) || (!checkHealthFlag) )'

      return;

   }  // End of method 'GloEphemerisStore::addEphemeris()'


      /* Returns the position, velocity and clock offset of the indicated
       * satellite in ECEF coordinates (meters) at the indicated time,
       * in the PZ-90 ellipsoid.
       *
       *  @param[in] sat   Satellite's identifier
       *  @param[in] epoch Time to look up
       *
       *  @return the Xvt of the object at the indicated time
       *
       *  @throw InvalidRequest If the request can not be completed for any
       *  reason, this is thrown. The text may have additional information
       *  as to why the request failed.
       */
   Xvt GloEphemerisStore::getXvt( const SatID& sat,
                                  const CommonTime& epoch ) const
      throw( InvalidRequest )
   {

         // Check that the given epoch is within the available time limits.
         // We have to add a margin of 15 minutes (900 seconds).
      if ( epoch <  (initialTime - 900.0) ||
           epoch >= (finalTime   + 900.0)   )
      {
         InvalidRequest e( "Requested time is out of boundaries for satellite "
                          + StringUtils::asString(sat) );
         GPSTK_THROW(e);
      }

         // Look for the satellite in the 'pe' (EphMap) data structure.
      GloEphMap::const_iterator svmap = pe.find(sat);

         // If satellite was not found, issue an exception
      if (svmap == pe.end())
      {
         InvalidRequest e( "Ephemeris for satellite  "
                           + StringUtils::asString(sat) + " not found." );
         GPSTK_THROW(e);
      }

         // Let's take the second part of the EphMap
      const TimeGloMap& sem = svmap->second;

         // Look for the exact epoch
      TimeGloMap::const_iterator i = sem.find(epoch);

         // Values to be returned will be stored here
      Xvt sv;

         // 'i' will be the first element whose key >= epoch.
      i = sem.lower_bound(epoch);

         // If we reached the end, the requested time is beyond the last
         // ephemeris record, but it may still be within the allowable time
         // span, so we can use the last record.
      if ( i == sem.end() )
      {
         i = --i;
      }

         // If key > (epoch+900), we must use the previous record if possible.
      if ( ( i->first > (epoch+900.0) ) && ( i != sem.begin() ) )
      {
         i = --i;
      }

         // Check that the given epoch is within the available time limits for
         // this specific satellite, with a margin of 15 minutes (900 seconds).
      if ( epoch <  (i->first - 900.0) ||
           epoch >= (i->first   + 900.0)   )
      {
         InvalidRequest e( "Requested time is out of boundaries for satellite "
                          + StringUtils::asString(sat) );
         GPSTK_THROW(e);
      }

         // We now have the proper reference data record. Let's use it
      GloEphemeris data( i->second );

         // Compute the satellite position, velocity and clock offset
      sv = data.svXvt( epoch );

         // We are done, let's return
      return sv;

   }; // End of method 'GloEphemerisStore::getXvt()'


      /* A debugging function that outputs in human readable form,
       * all data stored in this object.
       *
       * @param[in] s      The stream to receive the output; defaults to cout
       * @param[in] detail The level of detail to provide
       *
       * @warning GLONASS position, velocity and acceleration information are
       * given in km, km/s and km/(s*s), respectively.
       */
   void GloEphemerisStore::dump( std::ostream& s, short detail ) const
      throw()
   {
      static const string fmt("%4F %10.3g = %04Y/%02m/%02d %02H:%02M:%02S %P");
      s << "Dump of GloEphemerisStore:\n";
      s << "week   sow      = year/mn/dy hr:mi:sc Sys Sat   "
         << "X                   Y                   Z                   "
         << "VX                  VY                  VZ                  "
         << "AX                  AY                  AZ                  "
         << "TauN                GammaN            MFtime Hlth fNo AgeInfo\n";

         // Iterate through all items in the 'pe' GloEphMap
      for( GloEphMap::const_iterator it = pe.begin();
           it != pe.end();
           ++it )
      {

            // Then, iterate through corresponding 'TimeGloMap'
         for( TimeGloMap::const_iterator tgmIter = (*it).second.begin();
              tgmIter != (*it).second.end();
              ++tgmIter )
         {

            //   // Declare a 'YDSTime' object to ease printing
            //YDSTime time( (*tgmIter).first );
            //   // First, print year, Day-Of-Year and Seconds of Day
            //s << time.year << " "
            //  << time.doy << " "
            //  << time.sod << " ";
            s << printTime(tgmIter->first,fmt) << " ";

               // Second, print SatID information
            s << RinexSatID((*it).first) << " ";

               // Third, print satellite ephemeris data
            GloEphemeris data( (*tgmIter).second );
               // Get the satellite's acceleration
            Triple a( data.getAcc() );

            s << scientific << setprecision(12);
            s << setw(19) << data.x[0] << " "
              << setw(19) << data.x[1] << " "
              << setw(19) << data.x[2] << " "
              << setw(19) << data.v[0] << " "
              << setw(19) << data.v[1] << " "
              << setw(19) << data.v[2] << " "
              << setw(19) << a[0] << " "
              << setw(19) << a[1] << " "
              << setw(19) << a[2] << " "
              << setw(19) << data.getTauN() << " "
              << setw(19) << data.getGammaN() << " "
              << setw(6) << data.getMFtime() << " "
              << setw(3) << data.getHealth() << " "
              << setw(3) << data.getfreqNum() << " "
              << setprecision(2) << setw(5) << data.getAgeOfInfo();

               // Add end-of-line
            s << endl;

         }  // End of 'for( TimeGloMap::const_iterator tgmIter = ...'

      }  // End of 'for( GloEphMap::const_iterator it = pe.begin(); ...'

      return;

   }; // End of method 'GloEphemerisStore::dump()'


      /* Edit the dataset, removing data outside the indicated time interval
       *
       * @param[in] tmin   Defines the beginning of the time interval
       * @param[in] tmax   Defines the end of the time interval
       */
   void GloEphemerisStore::edit( const CommonTime& tmin,
                                 const CommonTime& tmax )
      throw()
   {

         // Create a working copy
      GloEphMap bak;

         // Reset the initial and final times
      initialTime = CommonTime::END_OF_TIME;
      finalTime   = CommonTime::BEGINNING_OF_TIME;

         // Iterate through all items in the 'bak' GloEphMap
      for( GloEphMap::const_iterator it = pe.begin();
           it != pe.end();
           ++it )
      {

            // Then, iterate through corresponding 'TimeGloMap'
         for( TimeGloMap::const_iterator tgmIter = (*it).second.begin();
              tgmIter != (*it).second.end();
              ++tgmIter )
         {

            CommonTime t( (*tgmIter).first );

               // Check if the current record is within the given time interval
            if( ( tmin <= t ) && ( t <= tmax ) )
            {

                  // If we are within the proper boundaries, let's add the data
               GloEphemeris data( (*tgmIter).second );
            
               SatID sat( (*it).first );
               bak[sat][t] = data;     // Add entry

                  // Update 'initialTime' and 'finalTime', if necessary
               if (t < initialTime)
                  initialTime = t;
               else if (t > finalTime)
                  finalTime = t;

            }  // End of 'if ( ( (*tgmIter).first >= tmin ) && ...'

         }  // End of 'for( TimeGloMap::const_iterator tgmIter = ...'

      }  // End of 'for( GloEphMap::const_iterator it = pe.begin(); ...'

         // Update the data map before returning
      pe = bak;

      return;
      
   }; // End of method 'GloEphemerisStore::edit()'


      // Determine the earliest time for which this object can successfully
      // determine the Xvt for any object.
      // @return The initial time
      // @throw InvalidRequest This is thrown if the object has no data.
   CommonTime GloEphemerisStore::getInitialTime() const
      throw(InvalidRequest)
   {

         // Check if the data map is empty
      if( pe.empty() )
      {
         InvalidRequest e( "GloEphemerisStore object has no data." );
         GPSTK_THROW(e);
      }
      
      return initialTime;

   }; // End of method 'GloEphemerisStore::getInitialTime()'


      // Determine the latest time for which this object can successfully
      // determine the Xvt for any object.
      // @return The final time
      // @throw InvalidRequest This is thrown if the object has no data.
   CommonTime GloEphemerisStore::getFinalTime() const
      throw(InvalidRequest)
   {

         // Check if the data map is empty
      if( pe.empty() )
      {
         InvalidRequest e( "GloEphemerisStore object has no data." );
         GPSTK_THROW(e);
      }

      return finalTime;

   }; // End of method 'GloEphemerisStore::getFinalTime()'


      // Return true if the given SatID is present in the store
   bool GloEphemerisStore::isPresent(const SatID& id) const throw()
   {

         // Look for the satellite in the 'pe' (GloEphMap) data structure.
      GloEphMap::const_iterator svmap = pe.find(id);

         // If satellite was not found return false, else return true
      if (svmap == pe.end())
      {
         return false;
      }
      else
      {
         return true;
      }

   }; // End of method 'GloEphemerisStore::isPresent(const SatID& id)'


}  // End of namespace gpstk
