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

namespace gpstk
{


      // Add ephemeris information from a RinexGloNavData object.
   void GloEphemerisStore::addEphemeris(const RinexGloNavData& data)
      throw()
   {

         // If enabled, check SV health before entering here (health = 0 -> OK).
      if( (data.health == 0) || (!checkHealthFlag) )
      {

         CommonTime t = data.time;
         SatID sat = data.sat;
         GloRecord& glorecord = pe[sat][t]; // find or add entry

         glorecord.x = Triple(data.px,data.py,data.pz);
         glorecord.v = Triple(data.vx,data.vy,data.vz);
         glorecord.a = Triple(data.ax,data.ay,data.az);

         glorecord.clkbias   = data.TauN;
         glorecord.clkdrift  = data.GammaN;
         glorecord.MFtime    = data.MFtime;
         glorecord.health    = data.health;
         glorecord.freqNum   = data.freqNum;
         glorecord.ageOfInfo = data.ageOfInfo;

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

         // Let's take the second part of the EphMap: map<CommonTime,GloRecord>
      const TimeGloMap& sem = svmap->second;

         // Look for the exact epoch
      TimeGloMap::const_iterator i = sem.find(epoch);

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

            // In the GLONASS system, 'clkbias' already includes the
            // relativistic correction, therefore we must substract the late
            // from the former.
         sv.clkbias = sv.clkbias - sv.computeRelativityCorrection();
         sv.frame = ReferenceFrame::PZ90;

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

         // Get the data out of the GloRecord structure
      double  x( data.x[0] );   // X coordinate (km)
      double vx( data.v[0] );   // X velocity   (km/s)
      double ax( data.a[0] );   // X acceleration (km/s^2)
      double  y( data.x[1] );   // Y coordinate
      double vy( data.v[1] );   // Y velocity
      double ay( data.a[1] );   // Y acceleration
      double  z( data.x[2] );   // Z coordinate
      double vz( data.v[2] );   // Z velocity
      double az( data.a[2] );   // Z acceleration

         // We will need some PZ-90 ellipsoid parameters
      PZ90Ellipsoid pz90;
      double we( pz90.angVelocity() );

         // Get sidereal time at Greenwich at 0 hours UT
      double gst( getSidTime(refEpoch) );
      double s0( gst*PI/12.0 );
      YDSTime ytime( refEpoch );
      double numSeconds( ytime.sod );
      double s( s0 + we*numSeconds );
      double cs( std::cos(s) );
      double ss( std::sin(s) );

         // Initial state matrix
      Vector<double> initialState(6), accel(3), dxt1(6), dxt2(6), dxt3(6),
                     dxt4(6), tempRes(6);

         // Get the reference state out of GloRecord data structure. The values
         // must be rotated from PZ-90 to an absolute coordinate system
         // Initial x coordinate (m)
      initialState(0)  = (x*cs - y*ss);
         // Initial y coordinate
      initialState(2)  = (x*ss + y*cs);
         // Initial z coordinate
      initialState(4)  = z;

         // Initial x velocity   (m/s)
      initialState(1)  = (vx*cs - vy*ss - we*initialState(2) );
         // Initial y velocity
      initialState(3)  = (vx*ss + vy*cs + we*initialState(0) );
         // Initial z velocity
      initialState(5)  = vz;


         // Integrate satellite state to the desired epoch, using the given step
      double rkStep( step );

      if ( (epoch - refEpoch) < 0.0 ) rkStep = step*(-1.0);
      CommonTime workEpoch(refEpoch);

      double tolerance( 1e-9 );
      bool done( false );
      while (!done)
      {

            // If we are about to overstep, change the stepsize appropriately
            // to hit our target final time.
         if( rkStep > 0.0 )
         {
            if( (workEpoch + rkStep) > epoch )
               rkStep = (epoch - workEpoch);
         }
         else
         {
            if ( (workEpoch + rkStep) < epoch )
               rkStep = (epoch - workEpoch);
         }

         numSeconds += rkStep;
         s = s0 + we*( numSeconds );
         cs = std::cos(s);
         ss = std::sin(s);
         
            // Accelerations are computed once per iteration
         accel(0) = ax*cs - ay*ss;
         accel(1) = ax*ss + ay*cs;
         accel(2) = az;

         dxt1 = derivative( initialState, accel );
         for( int j = 0; j < 6; ++j )
            tempRes(j) = initialState(j) + rkStep*dxt1(j)/2.0;

         dxt2 = derivative( tempRes, accel );
         for( int j = 0; j < 6; ++j )
            tempRes(j) = initialState(j) + rkStep*dxt2(j)/2.0;

         dxt3 = derivative( tempRes, accel );
         for( int j = 0; j < 6; ++j )
            tempRes(j) = initialState(j) + rkStep*dxt3(j);

         dxt4 = derivative( tempRes, accel );
         for( int j = 0; j < 6; ++j )
            initialState(j) = initialState(j) + rkStep * ( dxt1(j) + 2.0*(dxt2(j)+dxt3(j)) + dxt4(j) )/6.0;

            // If we are within teps of the goal time, we are done.
         workEpoch += rkStep;
         if ( std::fabs(epoch - workEpoch ) < tolerance )
            done = true;

      }  // End of 'while (!done)...'


      x  = initialState(0);
      y  = initialState(2);
      z  = initialState(4);
      vx = initialState(1);
      vy = initialState(3);
      vz = initialState(5);

      sv.x[0] = 1000.0*(  x*cs + y*ss);         // X coordinate
      sv.v[0] = 1000.0*( vx*cs + vy*ss + we*y); // X velocity
      sv.x[1] = 1000.0*( -x*ss + y*cs);         // Y coordinate
      sv.v[1] = 1000.0*(-vx*ss + vy*cs - we*x); // Y velocity
      sv.x[2] = 1000.0*z;                       // Z coordinate
      sv.v[2] = 1000.0*vz;                      // Z velocity


         // In the GLONASS system, 'clkbias' already includes the relativistic
         // correction, therefore we must substract the late from the former.
      sv.clkbias = data.clkbias - data.clkdrift * (epoch - refEpoch)
                                - data.computeRelativityCorrection();
      sv.clkdrift = data.clkdrift;
      sv.relcorr = data.relcorr;
      sv.frame = ReferenceFrame::PZ90;

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

               // Declare a 'YDSTime' object to ease printing
            YDSTime time( (*tgmIter).first );
               // First, print year, Day-Of-Year and Seconds of Day
            s << time.year << " "
              << time.doy << " "
              << time.sod << " ";

               // Second, print SatID information
            s << (*it).first << " ";

               // Third, print satellite ephemeris data
            GloRecord data( (*tgmIter).second );
            s << data.x[0] << " " << data.x[1] << " " << data.x[2] << " "
              << data.v[0] << " " << data.v[1] << " " << data.v[2] << " "
              << data.a[0] << " " << data.a[1] << " " << data.a[2] << " "
              << data.clkbias << " " << data.clkdrift << " "
              << data.MFtime<< " " << data.health << " "
              << data.freqNum << " " << data.ageOfInfo;

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
               GloRecord data( (*tgmIter).second );
            
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


      // Compute true sidereal time  (in hours) at Greenwich at 0 hours UT.
   double GloEphemerisStore::getSidTime( const CommonTime& time ) const
   {

         // The following algorithm is based on the paper:
         // Aoki, S., Guinot,B., Kaplan, G. H., Kinoshita, H., McCarthy, D. D.
         //    and P.K. Seidelmann. 'The New Definition of Universal Time'.
         //    Astronomy and Astrophysics, 105, 359-361, 1982.

         // Get the Julian Day at 0 hours UT (jd)
      YDSTime ytime( time );
      double year( ytime.year );
      int doy( ytime.doy );
      int temp( floor(365.25 * (year - 1.0)) + doy );

      double jd( static_cast<double>(temp)+ 1721409.5 );
      
         // Compute the Julian centuries (36525 days)
      double jc( (jd - 2451545.0)/36525.0 );

         // Compute the sidereal time, in seconds
      double sid( 24110.54841 + jc * ( 8640184.812866
                              + jc * ( 0.093104 - jc * 0.0000062 ) ) );

      sid = sid / 3600.0;
      sid = fmod(sid, 24.0);
      if( sid < 0.0 ) sid = sid + 24.0;

      return sid;
      
   }; // End of method 'GloEphemerisStore::getSidTime()'


      // Function implementing the derivative of GLONASS orbital model.
   Vector<double> GloEphemerisStore::derivative( const Vector<double>& inState,
                                                 const Vector<double>& accel )
      const
   {

         // We will need some important PZ90 ellipsoid values
      PZ90Ellipsoid pz90;
      const double j20( pz90.j20() );
      const double mu( pz90.gm_km() );
      const double ae( pz90.a_km() );

         // Let's start getting the current satellite position and velocity
      double  x( inState(0) );          // X coordinate
      double vx( inState(1) );          // X velocity
      double  y( inState(2) );          // Y coordinate
      double vy( inState(3) );          // Y velocity
      double  z( inState(4) );          // Z coordinate
      double vz( inState(5) );          // Z velocity


      double r2( x*x + y*y + z*z );
      double r( std::sqrt(r2) );
      double xmu( mu/r2 );
      double rho( ae/r );
      double xr( x/r );
      double yr( y/r );
      double zr( z/r );
      double zr2( zr*zr );
      double k1(j20*xmu*1.5*rho*rho);
      double  cm( k1*(1.0-5.0*zr2) );
      double cmz( k1*(3.0-5.0*zr2) );
      double k2(cm-xmu);

      double gloAx( k2*xr + accel(0) );
      double gloAy( k2*yr + accel(1) );
      double gloAz( (cmz-xmu)*zr + accel(2) );

      Vector<double> dxt(6, 0.0);

         // Let's insert data related to X coordinates
      dxt(0) = inState(1);       // Set X'  = Vx
      dxt(1) = gloAx;            // Set Vx' = gloAx

         // Let's insert data related to Y coordinates
      dxt(2) = inState(3);       // Set Y'  = Vy
      dxt(3) = gloAy;            // Set Vy' = gloAy

         // Let's insert data related to Z coordinates
      dxt(4) = inState(5);       // Set Z'  = Vz
      dxt(5) = gloAz;            // Set Vz' = gloAz

      return dxt;

   }  // End of method 'GloEphemerisStore::derivative()'


}  // End of namespace gpstk
