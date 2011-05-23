#pragma ident "$Id"

/**
 * @file GloBrcEphemerisStore.cpp
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


#include "GloBrcEphemerisStore.hpp"
#include "ECEF.hpp"

namespace gpstk
{


      // Add ephemeris information from a RinexGloNavData object.
   void GloBrcEphemerisStore::addEphemeris(const RinexGloNavData& data)
      throw()
   {

         // If enabled, check SV health before entering here (health = 0 -> OK).
      if( (data.health == 0) || (!checkHealthFlag) )
      {

         CommonTime t = data.time;
         SatID sat = data.sat;
         GloRecord& glorecord = pe[sat][t]; // find or add entry

         glorecord.x = ECEF(data.px,data.py,data.pz);
         glorecord.v = ECEF(data.vx,data.vy,data.vz);
         glorecord.a = ECEF(data.ax,data.ay,data.az);

         glorecord.dtime     = data.TauN;
         glorecord.ddtime    = data.GammaN;
         glorecord.MFtime    = data.MFtime;
         glorecord.health    = data.health;
         glorecord.freqNum   = data.freqNum;
         glorecord.ageOfInfo = data.ageOfInfo;

         if (t < initialTime)
            initialTime = t;
         else if (t > finalTime)
            finalTime = t;

      }  // End of 'if( (data.health == 0) || (!checkHealthFlag) )'

   }  // End of method 'GloBrcEphemerisStore::addEphemeris()'


      /* Returns the position and clock offset of the indicated
       * satellite in ECEF coordinates (meters) at the indicated time,
       * in the PZ-90 ellipsoid.
       *
       *  @param[in] sat   Satellite's identifier
       *  @param[in] epoch Time to look up
       *
       *  @return the Xt of the object at the indicated time
       *
       *  @throw InvalidRequest If the request can not be completed for any
       *  reason, this is thrown. The text may have additional information
       *  as to why the request failed.
       */
   Xt GloBrcEphemerisStore::getXt( const SatID& sat,
                                   const CommonTime& epoch ) const
      throw( InvalidRequest )
   {

         // Results will be stored here.
      Xt res;

         // We will take advantage of 'getXvt()' method.
      res = getXvt( sat, epoch );

         // Return
      return res;

   }; // End of method 'GloBrcEphemerisStore::getXt()'


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
   Xvt GloBrcEphemerisStore::getXvt( const SatID& sat,
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
      EphMap::const_iterator svmap = pe.find(sat);

         // If satellite was not found, issue an exception
      if (svmap == pe.end())
      {
         InvalidRequest e( "Ephemeris for satellite  "
                           + StringUtils::asString(sat) + " not found." );
         GPSTK_THROW(e);
      }

         // Let's take the second part of the EphMap: map<CommonTime,GloRecord>
      const SvEphMap& sem = svmap->second;

         // Look for the exact epoch
      SvEphMap::const_iterator i = sem.find(epoch);

         // Values to be returned will be stored here
      Xvt sv;

         // If the exact epoch is found, let's return the values
      if (i != sem.end())      // exact match of epoch
      {

         sv = i->second;

         sv.x[0]   *= 1.e3;   // m
         sv.x[1]   *= 1.e3;   // m
         sv.x[2]   *= 1.e3;   // m
         sv.v[0]   *= 1.e3;  // m/sec
         sv.v[1]   *= 1.e3;  // m/sec
         sv.v[2]   *= 1.e3;  // m/sec

            // Please be aware that the satellite clock corrections in GLONASS
            // ephemeris ALWAYS include the relativity corrections.
         return sv;
      }

         // 'i' will be the first element whose key >= epoch.
      i = sem.lower_bound(epoch);

         // If we reached the end, the requested time is beyond the last
         // ephemeris record, but still within the allowable time span,
         // so we can use the last record.
      if ( i == sem.end() )
      {
         i = --i;
      }

         // If key > (epoch+900), we must use the previous record.
      if ( i->first > (epoch+900.0) )
      {
         i = --i;
      }

         // We now have the proper reference data record. Let's use it
      CommonTime refEpoch( i->first );
      GloRecord data( i->second );

         // Initial state matrix
      Matrix<double> initialState(6,1), error(6,1);

         // Get the reference state out of GloRecord data structure
      initialState(0,0)  = (data.x[0]*1000.0);   // Initial x coordinate (m)
      initialState(1,0)  = (data.v[0]*1000.0);   // Initial x velocity   (m/s)
      initialState(2,0)  = (data.x[1]*1000.0);   // Initial y coordinate
      initialState(3,0)  = (data.v[1]*1000.0);   // Initial y velocity
      initialState(4,0)  = (data.x[2]*1000.0);   // Initial z coordinate
      initialState(5,0)  = (data.v[2]*1000.0);   // Initial z velocity

         // Get the luni-solar accelerations for this time period
      const double axls(data.a[0]*1000.0);   // In m/(s*s)
      const double ayls(data.a[1]*1000.0);
      const double azls(data.a[2]*1000.0);

         // Create 'GlonassSatelliteModel' object with initial state and time
      GlonassSatelliteModel gloSV( initialState, 0.0 );

         // Set the luni-solar accelerations. The 'GlonassSatelliteModel' object
         // will internally compute the real accelerations
      gloSV.setAcceleration( axls, ayls, azls );

         // Integrate satellite state to the desired epoch, using the given step
      double rkStep( step );

      if ( (epoch - refEpoch) < 0.0 ) rkStep = step*(-1.0);

      gloSV.integrateTo( (epoch - refEpoch), rkStep );

      sv.x[0] = gloSV.getState()(0,0);   // X coordinate
      sv.v[0] = gloSV.getState()(1,0);   // X velocity
      sv.x[1] = gloSV.getState()(2,0);   // Y coordinate
      sv.v[1] = gloSV.getState()(3,0);   // Y velocity
      sv.x[2] = gloSV.getState()(4,0);   // Z coordinate
      sv.v[2] = gloSV.getState()(5,0);   // Z velocity

         // Please note that in order to keep consistency with the current
         // standards of the GPSTk, the 'dtime' value holds the sum of
         // satellite clock corrections AND relativity corrections (already
         // included in GLONASS ephemeris). This may change in the near future.
      sv.dtime = data.dtime - data.ddtime * (epoch - refEpoch);
      sv.ddtime = data.ddtime;
      sv.frame = ReferenceFrame::PZ90;

         // We are done, let's return
      return sv;

   }; // End of method 'GloBrcEphemerisStore::getXvt()'


}  // End of namespace gpstk
