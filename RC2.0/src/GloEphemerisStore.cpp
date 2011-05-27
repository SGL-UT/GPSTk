#pragma ident "$Id"

/**
 * @file GloEphemerisStore.cpp
 * Get GLONASS ephemeris data information
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
//  Copyright 2004, The University of Texas at Austin
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

#include "GloEphemerisStore.hpp"
#include "ECEF.hpp"

namespace gpstk
{
   void GloEphemerisStore::addEphemeris(const Rinex3NavData& data)
      throw()
   {
      CommonTime t = data.time;
      SatID sat = data.sat;
      GloRecord& glorecord = pe[sat][t]; // find or add entry

      glorecord.x = ECEF(data.px,data.py,data.pz);
      glorecord.v = ECEF(data.vx,data.vy,data.vz);
      glorecord.a = ECEF(data.ax,data.ay,data.az);

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
   }

   /// Returns the position and clock offset of the indicated
   /// object in ECEF coordinates (meters) at the indicated time.
   /// Uses Lagrange interpolation; call setInterpolationOrder() to change
   /// the order.
   /// 
   /// @param[in] id the object's identifier
   /// @param[in] t the time to look up
   /// 
   /// @return the Xt of the object at the indicated time
   /// 
   /// @throw InvalidRequest If the request can not be completed for any
   ///    reason, this is thrown. The text may have additional
   ///    information as to why the request failed.
   Xt GloEphemerisStore::getXt( const SatID& sat,
                                const CommonTime& t ) const
     throw( InvalidRequest )
   {
     EphMap::const_iterator svmap = pe.find(sat);
     if (svmap == pe.end())
     {
       InvalidRequest e("Ephemeris for satellite  "
                        + StringUtils::asString(sat) + " not found.");
       GPSTK_THROW(e);
     }

     const SvEphMap& sem = svmap->second;
     SvEphMap::const_iterator i = sem.find(t);
     Xt sv;
     if (i != sem.end())      // exact match of t
     {
       sv = i->second;
       return sv;
     }

     // Note that the order of the Lagrange interpolation is twice this value.
     const int half = interpOrder / 2;

     //  i will be the lower bound, j the upper (in time).
     i = sem.lower_bound(t); // i points to first element with key >= t

     SvEphMap::const_iterator j = i;

     if (i == sem.begin() || --i == sem.begin())
     {
       InvalidRequest e("Inadequate data before requested time, satellite "
                        + StringUtils::asString(sat));
       GPSTK_THROW(e);
     }
     if (j == sem.end())
     {
       InvalidRequest e("Inadequate data after requested time, satellite "
                        + StringUtils::asString(sat));
       GPSTK_THROW(e);
     }

     // "t" is now just between "i" and "j"; therefore, it is time to check
     // for data gaps ("checkDataGap" must be enabled for this).
     if ( checkDataGap                      &&
          ABS( t - i->first ) > gapInterval &&
          ABS( j->first - t ) > gapInterval    )
     {
       // There was a data gap
       InvalidRequest e( "Data gap too wide detected for satellite "
                         + StringUtils::asString(sat) );
       GPSTK_THROW(e);
     }

     for (int k = 0; k < half - 1; k++)
     {
       i--;

       // if k==half-2, this is last iteration
       if (i == sem.begin() && k < half-2)
       {
         InvalidRequest e("Inadequate data before requested time, satellite "
                          + StringUtils::asString(sat));
         GPSTK_THROW(e);
       }
       j++;
       if ( j == sem.end() && k < half - 2 )
       {
         InvalidRequest e("Inadequate data after requested time, satellite "
                          + StringUtils::asString(sat));
         GPSTK_THROW(e);
       }
     }

     // Now that we have fully defined the i-j interval, let's check if the
     // interpolation interval is too wide ("checkInterval" must be enabled
     // for this).
     if ( checkInterval                            &&
          ABS( j->first - i->first ) > maxInterval    )
     {
       // There was a data gap
       InvalidRequest e( "Interpolation interval too wide detected for SV "
                         + StringUtils::asString(sat) );
       GPSTK_THROW(e);
     }

     // Pull data and interpolate.
     SvEphMap::const_iterator itr;
     CommonTime t0 = i->first;
     double dt = t-t0,err;
     std::vector<double> times,X,Y,Z,T;

     for (itr = i; itr != sem.end(); itr++)
     {
       times.push_back(itr->first - t0);

       X.push_back(itr->second.x[0]);
       Y.push_back(itr->second.x[1]);
       Z.push_back(itr->second.x[2]);
       T.push_back(itr->second.clkbias);

       if(itr == j) break;
     }

     LagrangeInterpolation(times,X,dt,sv.x[0]);
     LagrangeInterpolation(times,Y,dt,sv.x[1]);
     LagrangeInterpolation(times,Z,dt,sv.x[2]);
     LagrangeInterpolation(times,T,dt,sv.clkbias);

     // Again, there is a problem here b/c the relativity correction
     // cannot be computed without the velocity.  See note above.

     return sv;

   }; // end getXt


   /// Returns the position, velocity, and clock offset of the indicated
   ///  object in ECEF coordinates (meters) at the indicated time.
   /// Uses Lagrange interpolation; call setInterpolationOrder() to change
   /// the order.
   /// 
   /// @param[in] id the object's identifier
   /// @param[in] t the time to look up
   /// 
   /// @return the Xvt of the object at the indicated time
   /// 
   /// @throw InvalidRequest If the request can not be completed for any
   ///    reason, this is thrown. The text may have additional
   ///    information as to why the request failed.
   Xvt GloEphemerisStore::getXvt( const SatID& sat,
                                  const CommonTime& t ) const
     throw( InvalidRequest )
   {
     EphMap::const_iterator svmap = pe.find(sat);
     if (svmap == pe.end())
     {
       InvalidRequest e("Ephemeris for satellite  " + StringUtils::asString(sat)
                        + " not found.");
       GPSTK_THROW(e);
     }

     const SvEphMap& sem = svmap->second;
     SvEphMap::const_iterator i = sem.find(t);
     Xvt sv;

     if (i != sem.end() && haveVelocity)      // exact match of t
     {
       sv = i->second;

       sv.relcorr = -2*(sv.x[0]/C_GPS_M)*(sv.v[0]/C_GPS_M)
                    -2*(sv.x[1]/C_GPS_M)*(sv.v[1]/C_GPS_M)
                    -2*(sv.x[2]/C_GPS_M)*(sv.v[2]/C_GPS_M);
       return sv;
     }

     // Note that the order of the Lagrange interpolation is twice this value.
     const int half = interpOrder / 2;

     //  i will be the lower bound, j the upper (in time).
     i = sem.lower_bound(t); // i points to first element with key >= t

     SvEphMap::const_iterator j = i;

     if (i == sem.begin() || --i == sem.begin())
     {
       InvalidRequest e("Inadequate data before requested time, satellite "
                        + StringUtils::asString(sat));
       GPSTK_THROW(e);
     }
     if (j == sem.end())
     {
       InvalidRequest e("Inadequate data after requested time, satellite "
                        + StringUtils::asString(sat));
       GPSTK_THROW(e);
     }

     // "t" is now just between "i" and "j"; therefore, it is time to check
     // for data gaps ("checkDataGap" must be enabled for this).
     if ( checkDataGap                      &&
          ABS( t - i->first ) > gapInterval &&
          ABS( j->first - t ) > gapInterval    )
     {
       // There was a data gap
       InvalidRequest e( "Data gap too wide detected for satellite "
                         + StringUtils::asString(sat) );
       GPSTK_THROW(e);
     }

     for (int k = 0; k < half-1; k++)
     {
       i--;

       // if k==half-2, this is last iteration
       if (i == sem.begin() && k<half-2)
       {
         InvalidRequest e("Inadequate data before requested time, satellite "
                          + StringUtils::asString(sat));
         GPSTK_THROW(e);
       }
       j++;
       if (j == sem.end() && k<half-2)
       {
         InvalidRequest e("Inadequate data after requested time, satellite "
                          + StringUtils::asString(sat));
         GPSTK_THROW(e);
       }
     }

     // Now that we have fully defined the i-j interval, let's check if
     // the interpolation interval is too wide ("checkInterval" must be
     // enabled for this).
     if ( checkInterval                            &&
          ABS( j->first - i->first ) > maxInterval    )
     {
       // There was a data gap.
       InvalidRequest e( "Interpolation interval too wide detected for SV "
                         + StringUtils::asString(sat) );
       GPSTK_THROW(e);
     }

     // Pull data and interpolate.
     SvEphMap::const_iterator itr;
     CommonTime t0 = i->first;
     double dt = t-t0,err;
     std::vector<double> times,X,Y,Z,T,VX,VY,VZ,F;

     for (itr = i; itr != sem.end(); itr++)
     {
       times.push_back(itr->first - t0);

       X.push_back(itr->second.x[0]);
       Y.push_back(itr->second.x[1]);
       Z.push_back(itr->second.x[2]);
       T.push_back(itr->second.clkbias);

       VX.push_back(itr->second.v[0]);
       VY.push_back(itr->second.v[1]);
       VZ.push_back(itr->second.v[2]);
       F.push_back(itr->second.clkdrift);

       if (itr == j) break;
     }

     if (haveVelocity)
     {
       sv.x[0]   = LagrangeInterpolation(times,X,dt,err);
       sv.x[1]   = LagrangeInterpolation(times,Y,dt,err);
       sv.x[2]   = LagrangeInterpolation(times,Z,dt,err);
       sv.clkbias  = LagrangeInterpolation(times,T,dt,err);

       sv.v[0]   = LagrangeInterpolation(times,VX,dt,err);
       sv.v[1]   = LagrangeInterpolation(times,VY,dt,err);
       sv.v[2]   = LagrangeInterpolation(times,VZ,dt,err);
       sv.clkdrift = LagrangeInterpolation(times,F,dt,err);
     }
     else
     {
       LagrangeInterpolation(times,X,dt,sv.x[0],sv.v[0]);
       LagrangeInterpolation(times,Y,dt,sv.x[1],sv.v[1]);
       LagrangeInterpolation(times,Z,dt,sv.x[2],sv.v[2]);
       LagrangeInterpolation(times,T,dt,sv.clkbias,sv.clkdrift);
     }

     // Compute relativity correction
     // dtr = -2*dot(R,V)/(c*c) = -4.4428e-10 * ecc * sqrt(A(m))*sinE
     // (do it this way for numerical reasons).
     sv.relcorr = -2*(sv.x[0]/C_GPS_M)*(sv.v[0]/C_GPS_M)
                  -2*(sv.x[1]/C_GPS_M)*(sv.v[1]/C_GPS_M)
                  -2*(sv.x[2]/C_GPS_M)*(sv.v[2]/C_GPS_M);

     return sv;

   }; // end getXvt


} // namespace gpstk
