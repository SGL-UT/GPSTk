#pragma ident "$Id$"

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

/**
 * @file TabularEphemerisStore.hpp
 * Store a tabular list of Xvt data (such as a table of precise ephemeris data
 * in an SP3 file) and compute Xvt from this table. A Lagrange interpolation
 * is used to compute the Xvt for times that are not in the table but do have
 * sufficient data.
 */

#ifndef GPSTK_TABULAR_EPHEMERIS_STORE_HPP
#define GPSTK_TABULAR_EPHEMERIS_STORE_HPP

#include <iostream>
#include <iomanip>
#include <map>

#include "CommonTime.hpp"
#include "SatID.hpp"
#include "XvtStore.hpp"
#include "Xt.hpp"
#include "SP3Data.hpp"
#include "icd_200_constants.hpp"
#include "MiscMath.hpp"

using namespace gpstk::StringUtils;

namespace gpstk
{
   /** @addtogroup ephemstore */
   //@{

   /// Store a tabular list of Xvt data (such as a table of precise
   /// ephemeris data in an SP3 file) and compute Xvt from this table.
   /// A Lagrange interpolation is used to compute the Xvt for times that
   /// are not in the table but do have sufficient data.
   template <class DataRecord>
   class TabularEphemerisStore : public XvtStore<SatID>
   {
   public:

      /// Default constructor
      TabularEphemerisStore()
         throw()
         : haveVelocity(true), initialTime(CommonTime::END_OF_TIME),
           finalTime(CommonTime::BEGINNING_OF_TIME), checkDataGap(false),
           gapInterval(901.0), checkInterval(false), maxInterval(8105.0),
           interpOrder(10), rejectBadPosFlag(true), rejectBadClockFlag(true)
      {};

      /// Destructor
      virtual ~TabularEphemerisStore()
      {};

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
      virtual Xt getXt( const SatID sat,
                        const CommonTime& t )
         const throw( gpstk::InvalidRequest )
      {
         typename EphMap::const_iterator svmap = pe.find(sat);
         if (svmap == pe.end())
         {
           InvalidRequest e("Ephemeris for satellite  " + asString(sat)
                            + " not found.");
           GPSTK_THROW(e);
         }

         const SvEphMap& sem = svmap->second;
         typename SvEphMap::const_iterator i = sem.find(t);

         Xt sv;

         if (i != sem.end() && haveVelocity)      // exact match of t
         {
           sv = i->second;

           sv.x[0]  *= 1.e3;    // m
           sv.x[1]  *= 1.e3;    // m
           sv.x[2]  *= 1.e3;    // m
           sv.dtime *= 1.e-6;   // sec

           return sv;
         }

         // Note that the order of the Lagrange interpolation
         // is twice this value
         const int half = 5;

         //  i will be the lower bound, j the upper (in time).
         i = sem.lower_bound(t); // i points to first element with key >= t

         typename SvEphMap::const_iterator j = i;

         if (i == sem.begin() || --i == sem.begin())
         {
           InvalidRequest e("Inadequate data before requested time, satellite "
                            + asString(sat));
           GPSTK_THROW(e);
         }
         if (j == sem.end())
         {
           InvalidRequest e("Inadequate data after requested time, satellite "
                            + asString(sat));
           GPSTK_THROW(e);
         }

         // "t" is now just between "i" and "j"; therefore, it is time to check
         // for data gaps ("checkDataGap" must be enabled for this).
         if ( checkDataGap                               &&
              ( std::abs( t - i->first ) > gapInterval ) &&
              ( std::abs( j->first - t ) > gapInterval )    )
         {
           // There was a data gap
           InvalidRequest e( "Data gap too wide detected for satellite "
                             + asString(sat) );
           GPSTK_THROW(e);
         }

         for (int k=0; k<half-1; k++)
         {
           i--;

           // if k==half-2, this is last iteration
           if (i == sem.begin() && k < half-2)
           {
             InvalidRequest e("Inadequate data before requested time, satellite "
                              + asString(sat));
             GPSTK_THROW(e);
           }
           j++;
           if (j == sem.end() && k < half-2)
           {
             InvalidRequest e("Inadequate data after requested time, satellite "
                              + asString(sat));
             GPSTK_THROW(e);
           }
         }

         // Now that we have fully defined the i-j interval, let's check if
         // the interpolation interval is too wide ("checkInterval" must be
         // enabled for this).
         if ( checkInterval                                     &&
              ( std::abs( j->first - i->first ) > maxInterval )    )
         {
           // There was a data gap
           InvalidRequest e( "Interpolation interval too wide detected for SV "
                             + asString(sat) );
           GPSTK_THROW(e);
         }

         // pull data and interpolate
         typename SvEphMap::const_iterator itr;
         CommonTime t0 = i->first;
         double dt = t-t0,err;
         std::vector<double> times,X,Y,Z,T;

         for (itr = i; itr != sem.end(); itr++)
         {
           times.push_back(itr->first - t0);      // sec

           X.push_back(itr->second.x[0]);         // km
           Y.push_back(itr->second.x[1]);         // km
           Z.push_back(itr->second.x[2]);         // km
           T.push_back(itr->second.dtime);        // microsec

           if(itr == j) break;
         }

         LagrangeInterpolation(times,X,dt,sv.x[0]);
         LagrangeInterpolation(times,Y,dt,sv.x[1]);
         LagrangeInterpolation(times,Z,dt,sv.x[2]);
         LagrangeInterpolation(times,T,dt,sv.dtime);

         sv.x[0]  *= 1.e3;    // m
         sv.x[1]  *= 1.e3;    // m
         sv.x[2]  *= 1.e3;    // m
         sv.dtime *= 1.e-6;   // sec

         return sv;

     };  // end Xvt TabularEphemerisStore::getXt


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
      virtual Xvt getXvt( const SatID sat,
                          const CommonTime& t )
         const throw( gpstk::InvalidRequest )
      {
        typename EphMap::const_iterator svmap = pe.find(sat);
        if (svmap == pe.end())
        {
            InvalidRequest e("Ephemeris for satellite  " + asString(sat)
                             + " not found.");
            GPSTK_THROW(e);
        }

        const SvEphMap& sem = svmap->second;
        typename SvEphMap::const_iterator i = sem.find(t);

        Xvt sv;

        if (i != sem.end() && haveVelocity)      // exact match of t
        {
          sv = i->second;

          sv.x[0]   *= 1.e3;   // m
          sv.x[1]   *= 1.e3;   // m
          sv.x[2]   *= 1.e3;   // m
          sv.dtime  *= 1.e-6;  // sec
          sv.v[0]   *= 1.e-1;  // m/sec
          sv.v[1]   *= 1.e-1;  // m/sec
          sv.v[2]   *= 1.e-1;  // m/sec
          sv.ddtime *= 1.e-10; // sec/sec

          sv.dtime += -2*(sv.x[0]/C_GPS_M)*(sv.v[0]/C_GPS_M)
                      -2*(sv.x[1]/C_GPS_M)*(sv.v[1]/C_GPS_M)
                      -2*(sv.x[2]/C_GPS_M)*(sv.v[2]/C_GPS_M);
          return sv;
        }

        // Note that the order of the Lagrange interpolation
        // is twice this value
        const int half = 5;

        //  i will be the lower bound, j the upper (in time).
        i = sem.lower_bound(t); // i points to first element with key >= t

        typename SvEphMap::const_iterator j = i;

        if (i == sem.begin() || --i == sem.begin())
        {
          InvalidRequest e("Inadequate data before requested time, satellite "
                           + asString(sat));
          GPSTK_THROW(e);
        }
        if (j == sem.end())
        {
          InvalidRequest e("Inadequate data after requested time, satellite "
                           + asString(sat));
          GPSTK_THROW(e);
        }

        // "t" is now just between "i" and "j"; therefore, it is time to check
        // for data gaps ("checkDataGap" must be enabled for this).
        if ( checkDataGap                               &&
             ( std::abs( t - i->first ) > gapInterval ) &&
             ( std::abs( j->first - t ) > gapInterval )    )
        {
          // There was a data gap
          InvalidRequest e( "Data gap too wide detected for satellite "
                            + asString(sat) );
          GPSTK_THROW(e);
        }

        for(int k = 0; k < half-1; k++)
        {
          i--;

          // if k==half-2, this is last iteration
          if(i == sem.begin() && k<half-2)
          {
            InvalidRequest e("Inadequate data before requested time, satellite "
                             + asString(sat));
            GPSTK_THROW(e);
          }
          j++;
          if(j == sem.end() && k<half-2)
          {
            InvalidRequest e("Inadequate data after requested time, satellite "
                             + asString(sat));
            GPSTK_THROW(e);
          }
        }

        // Now that we have fully defined the i-j interval, let's check if
        // the interpolation interval is too wide ("checkInterval" must be
        // enabled for this).
        if ( checkInterval                                     &&
             ( std::abs( j->first - i->first ) > maxInterval )    )
        {
          // There was a data gap
          InvalidRequest e( "Interpolation interval too wide detected for SV "
                            + asString(sat) );
          GPSTK_THROW(e);
        }

        // pull data and interpolate
        typename SvEphMap::const_iterator itr;
        CommonTime t0 = i->first;
        double dt = t-t0,err;
        std::vector<double> times,X,Y,Z,T,VX,VY,VZ,F;

        for (itr = i; itr != sem.end(); itr++)
        {
          times.push_back(itr->first - t0);      // sec

          X.push_back(itr->second.x[0]);         // km
          Y.push_back(itr->second.x[1]);         // km
          Z.push_back(itr->second.x[2]);         // km
          T.push_back(itr->second.dtime);        // microsec
          VX.push_back(itr->second.v[0]);        // decimeters/sec
          VY.push_back(itr->second.v[1]);        // decimeters/sec
          VZ.push_back(itr->second.v[2]);        // decimeters/sec
          F.push_back(itr->second.ddtime);       // 1.e-4 microsec/sec

          if(itr == j) break;
        }

        if (haveVelocity)
        {
          sv.x[0]   = LagrangeInterpolation(times,X,dt,err);
          sv.x[1]   = LagrangeInterpolation(times,Y,dt,err);
          sv.x[2]   = LagrangeInterpolation(times,Z,dt,err);
          sv.dtime  = LagrangeInterpolation(times,T,dt,err);
          sv.v[0]   = LagrangeInterpolation(times,VX,dt,err);
          sv.v[1]   = LagrangeInterpolation(times,VY,dt,err);
          sv.v[2]   = LagrangeInterpolation(times,VZ,dt,err);
          sv.ddtime = LagrangeInterpolation(times,F,dt,err);
        }
        else
        {
          LagrangeInterpolation(times,X,dt,sv.x[0],sv.v[0]);
          LagrangeInterpolation(times,Y,dt,sv.x[1],sv.v[1]);
          LagrangeInterpolation(times,Z,dt,sv.x[2],sv.v[2]);
          LagrangeInterpolation(times,T,dt,sv.dtime,sv.ddtime);
          sv.v[0]   *= 1.e4;            // decimeters/sec
          sv.v[1]   *= 1.e4;            // decimeters/sec
          sv.v[2]   *= 1.e4;            // decimeters/sec
          sv.ddtime *= 1.e4;            // 1.e-4 microsec/sec
        }

        sv.x[0]   *= 1.e3;   // m
        sv.x[1]   *= 1.e3;   // m
        sv.x[2]   *= 1.e3;   // m
        sv.dtime  *= 1.e-6;  // sec
        sv.v[0]   *= 1.e-1;  // m/sec
        sv.v[1]   *= 1.e-1;  // m/sec
        sv.v[2]   *= 1.e-1;  // m/sec
        sv.ddtime *= 1.e-10; // sec/sec

        // add relativity correction to dtime
        // this only for consistency with GPSEphemerisStore::getSatXvt ....
        // dtr = -2*dot(R,V)/(c*c) = -4.4428e-10 * ecc * sqrt(A(m))*sinE
        // (do it this way for numerical reasons)
        sv.dtime += -2*(sv.x[0]/C_GPS_M)*(sv.v[0]/C_GPS_M)
                    -2*(sv.x[1]/C_GPS_M)*(sv.v[1]/C_GPS_M)
                    -2*(sv.x[2]/C_GPS_M)*(sv.v[2]/C_GPS_M);

        return sv;

      };  // end Xvt TabularEphemerisStore::getXvt


      /// A debugging function that outputs in human readable form,
      ///  all data stored in this object.
      /// 
      /// @param[in] s the stream to receive the output; defaults to cout
      /// @param[in] detail the level of detail to provide
      virtual void dump( std::ostream& s = std::cout,
                         short detail = 0             )
         const throw()
      {

        s << "Dump of TabularEphemerisStore:" << std::endl;

        if(detail >= 0)
        {
          typename EphMap::const_iterator it;

          s << " Data stored for " << pe.size() << " satellites, over time span "
            << initialTime << " to " << finalTime << "." << std::endl;

          if (detail == 0) return;

          for (it = pe.begin(); it != pe.end(); it++)
          {
            s << "  PRN " << it->first << " : "
              << it->second.size() << " records.";
            if (detail == 1) { s << std::endl; continue; }
            s << "  Data:" << std::endl;
            typename SvEphMap::const_iterator jt;

            for (jt = it->second.begin(); jt != it->second.end(); jt++)
            {
              s << " " << jt->first << " P "
                << std::fixed << std::setprecision(6)
                << std::setw(13) << jt->second.x[0] << " "
                << std::setw(13) << jt->second.x[1] << " "
                << std::setw(13) << jt->second.x[2] << " "
                << std::setw(13) << jt->second.dtime
                << " V "
                << std::setw(13) << jt->second.v[0] << " "
                << std::setw(13) << jt->second.v[1] << " "
                << std::setw(13) << jt->second.v[2] << " "
                << std::setw(13) << jt->second.ddtime
                << std::endl;
            }

          }

        }  // End of 'if(detail >= 0)...'

      };  // End of method 'TabularEphemerisStore::dump()'


      /// Edit the dataset, removing data outside the indicated time
      ///  interval.
      /// 
      /// @param[in] tmin defines the beginning of the time interval
      /// @param[in] tmax defines the end of the time interval
      virtual void edit( const CommonTime& tmin,
                         const CommonTime& tmax = CommonTime::END_OF_TIME )
         throw()
      {
        typename EphMap::iterator kt;

        for (kt = pe.begin(); kt != pe.end(); kt++)
        {
          typename SvEphMap::reverse_iterator jt=(kt->second).rbegin();

          while(jt != (kt->second).rend())
          {
            if (jt->first < tmin || jt->first > tmax)
            {
              (kt->second).erase(jt->first);
            }
            jt ++;
          }
        }

        initialTime = tmin;
        finalTime = tmax;

      };  // End of method 'TabularEphemerisStore::edit()'


      /// Determine the earliest time for which this object can successfully
      ///  determine the Xvt for any object.
      /// 
      /// @return The initial time
      /// 
      /// @throw InvalidRequest This is thrown if the object has no data.
      virtual CommonTime getInitialTime()
         const throw()
      { return initialTime; };


      /// Determine the latest time for which this object can successfully
      ///  determine the Xvt for any object.
      /// 
      /// @return The final time
      /// 
      /// @throw InvalidRequest This is thrown if the object has no data.
      virtual CommonTime getFinalTime()
         const throw()
      { return finalTime; };

      /// Check if this ephemeris contains velocity information in all
      /// datasets loaded.
      virtual bool velocityIsPresent()
         const throw()
      { return haveVelocity; }

         /// Set if satellites with bad or absent position values will be
         /// rejected. It is false by default when object is constructed.
      void rejectBadPositions(const bool flag) { rejectBadPosFlag = flag; }

         /// Set if satellites with bad or absent clock values will be
         /// rejected. It is false by default when object is constructed.
      void rejectBadClocks(const bool flag) { rejectBadClockFlag = flag; }


      //---------------------------------------------------------------
      // Below are interfaces that are unique to this class (i.e. not
      // in the parent class)
      //---------------------------------------------------------------


      /// Insert a new SP3Data object into the store
      void addEphemeris(const SP3Data& data)
         throw()
      {  
        CommonTime t = data.time;
        SatID sat = data.sat;
        Xvt&  xvt = pe[sat][t];

        if (data.RecType == 'P')
        {
          xvt.x = ECEF(data.x[0], data.x[1], data.x[2]);
          xvt.dtime = data.clk;
          haveVelocity = false;
        }
        else if (data.RecType == 'V')
        {
          xvt.v = Triple(data.x[0],data.x[1],data.x[2]);
          xvt.ddtime = data.clk;
          haveVelocity = true;
        }

        if (t < initialTime) initialTime = t;
        else if (t > finalTime) finalTime = t;
      };


      /// Remove all data
      void clear()
         throw()
      {
        pe.clear();
        initialTime = CommonTime::END_OF_TIME;
        finalTime = CommonTime::BEGINNING_OF_TIME;
      };

      /// Enable checking of data gaps.
      void enableDataGapCheck(void)
      { checkDataGap = true; };

      /// Disable checking of data gaps.
      void disableDataGapCheck(void)
      { checkDataGap = false; };

      /// Get current gap interval.
      double getGapInterval(void)
      { return gapInterval; };

      /// Set gap interval.
      void setGapInterval(double interval)
      { gapInterval = interval; return; };

      /// Enable checking of maximum interval.
      void enableIntervalCheck(void)
      { checkInterval = true; };

      /// Disable checking of maximum interval.
      void disableIntervalCheck(void)
      { checkInterval = false; };

      /// Get current maximum interval.
      double getMaxInterval(void)
      { return maxInterval; };

      /// Set maximum interval.
      void setMaxInterval(double interval)
      { maxInterval = interval; return; };

      /// Get current interpolation order.
      unsigned int getInterpolationOrder(void)
      { return interpOrder; }

      /// Set the interpolation order.
      /// This routine forces the order to be even.
      void setInterpolationOrder(unsigned int order)
      { interpOrder = 2*((order+1)/2); }

   protected:

      /// The key to this map is the time
      typedef std::map<CommonTime, DataRecord> SvEphMap;

      /// The key to this map is the svid of the satellite (usually the prn)
      typedef std::map<SatID, SvEphMap> EphMap;

      /// the map of SVs and XVTs
      EphMap pe;

      /// Flag indicating that velocity data present in all datasets loaded.
      bool haveVelocity;

      /// These give the overall span of time for which this object
      ///  contains data.
      /// 
      /// NB there may be gaps in the data, i.e. the data may not be
      /// continuous.
      CommonTime initialTime, finalTime;

      /// Flag to check for data gaps.
      /// 
      /// If this flag is enabled, data gaps wider than "gapInterval" will
      /// generate an "InvalidRequest" exception when using method "getXvt()".
      /// 
      /// This flag is disabled by default.
      bool checkDataGap;

      /// Maximum interval of time (in seconds) to declare a data gap.
      /// 
      /// Recommended value is (SP3 sample period) + 1, in seconds, which
      /// means 900 s + 1 s = 901 s for a typical 15-minutes-per-sample
      /// SP3 file.
      /// 
      /// This field is useful only if "checkDataGap" is enabled. Use method
      /// "enableDataGapCheck()" for this.
      double gapInterval;

      /// Flag to check for interpolation interval.
      /// 
      /// If this flag is enabled, interpolation intervals wider than
      /// "maxInterval" will generate an "InvalidRequest" exception when
      /// using method "getXvt()".
      /// 
      /// This flag is disabled by default.
      bool checkInterval;

      /// Maximum interval of time (in seconds) allowed to carry out the
      ///  interpolation process.
      /// 
      /// Recommended value is (10 - 1) * (SP3 sample period) + 5, in
      /// seconds, which means 8100 s + 5 s = 8105 s for a typical
      /// 15-minutes-per-sample SP3 file (please note that the order of the
      /// Lagrange interpolation is usually 10).
      /// 
      /// This field is useful only if "checkInterval" is enabled. Use method
      /// "enableIntervalCheck()" for this.
      double maxInterval;

      /// Order of Lagrange interpolation used in getXvt(), should be even.
      /// Usually for 15 minute data, this is 10. 
      unsigned int interpOrder;

         /// Flag to reject satellites with bad or absent positional values
      bool rejectBadPosFlag;

         /// Flag to reject satellites with bad or absent clock values
      bool rejectBadClockFlag;

   }; // end class TabularEphemerisStore

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_TABULAR_EPHEMERIS_STORE_HPP
