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
#include <vector>
#include <map>

#include "CommonTime.hpp"
#include "CivilTime.hpp"
#include "SatID.hpp"
#include "SP3SatID.hpp"
#include "XvtStore.hpp"
#include "Xvt.hpp"
#include "Xvt.hpp"
#include "GNSSconstants.hpp"
#include "MiscMath.hpp"

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
      ///    reason, this is thrown. The teXvt may have additional
      ///    information as to why the request failed.
      virtual Xvt getXvt( const SatID& sat,
                          const CommonTime& t ) const
         throw( InvalidRequest )
      { Xvt res; return res; }


      /// A debugging function that outputs in human readable form,
      ///  all data stored in this object.
      /// 
      /// @param[in] s the stream to receive the output; defaults to cout
      /// @param[in] detail the level of detail to provide
      virtual void dump( std::ostream& s = std::cout,
                         short detail = 0             ) const
         throw()
      {

        s << "Dump of TabularEphemerisStore:" << std::endl;

        if(detail >= 0)
        {
          const char *fmt="%4Y/%02m/%02d %2H:%02M:%02S (%P)";
          typename EphMap::const_iterator it;

          s << " Data stored for " << pe.size() << " satellites, over time span "
            << static_cast<CivilTime>(initialTime).printf(fmt) << " to "
            << static_cast<CivilTime>(finalTime).printf(fmt) << "." << std::endl;

          if (detail == 0) return;

          s << " Gap interval = " << gapInterval
             << "; checking gaps is " << (checkDataGap ? "on" : "off") << endl;
          s << " Max interval = " << maxInterval
             << "; checking interval is " << (checkInterval ? "on" : "off") << endl;
          s << " Interpolation order is " << interpOrder << endl;

          if(detail == 1) return;

          for (it = pe.begin(); it != pe.end(); it++)
          {
            s << "  Satellite map for sat " << SP3SatID(it->first) << " : "
              << it->second.size() << " records.";
            if (detail == 1) { s << std::endl; continue; }
            s << "  Data:" << std::endl;
            typename SvEphMap::const_iterator jt;

            for (jt = it->second.begin(); jt != it->second.end(); jt++)
            {
              s << " " << static_cast<CivilTime>(jt->first).printf(fmt) << " P "
                << std::fixed << std::setprecision(6)
                << std::setw(13) << jt->second.x[0] << " "
                << std::setw(13) << jt->second.x[1] << " "
                << std::setw(13) << jt->second.x[2] << " "
                << std::setw(13) << jt->second.clkbias
                << " V "
                << std::setw(13) << jt->second.v[0] << " "
                << std::setw(13) << jt->second.v[1] << " "
                << std::setw(13) << jt->second.v[2] << " "
                << std::setw(13) << jt->second.clkdrift
                << std::endl;
            }

          }

        }  // End of 'if(detail >= 0)...'

        s << "End Dump of TabularEphemerisStore." << std::endl;

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
        CommonTime ttmin(tmin),ttmax(tmax);

        if(ttmax < ttmin) { CommonTime swap=ttmin; ttmin=ttmax; ttmax=swap; }

        if(ttmax < initialTime || finalTime < ttmin) {
          clear();
          return;
        }

        typename EphMap::iterator kt;
        typename SvEphMap::iterator jt;

        kt = pe.begin();
        while(kt != pe.end()) {
          bool del=false;
          SvEphMap& theMap=kt->second;

          jt = theMap.lower_bound(ttmin);
          // jt points to first element with key >= ttmin

          if(jt == theMap.end()) {
            // ttmin is > last value - delete entirely
            del = true;
            if(ttmin > initialTime) initialTime = ttmin;
          }
          else if(jt != theMap.begin()) {
            if(jt->first > initialTime) initialTime = jt->first;
            theMap.erase(kt->second.begin(),jt);
          }

          if(!del) {
            jt = theMap.upper_bound(ttmax);
            // jt points to first element with key > ttmax

            if(jt == theMap.begin()) {
              // ttmax is < first value - delete entirely
              del = true;
              if(ttmax < finalTime) finalTime = ttmax;
            }
            else if(jt != theMap.end()) {
              jt--;
              if(jt->first < finalTime) finalTime = jt->first;
              theMap.erase(jt,kt->second.end());
            }

          }

          if(del) pe.erase(kt++);
          else ++kt;
        }

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

         /// Get number of satellite maps
      int nsats(void) throw() { return pe.size(); }
      
         /// Get total number of ephemerides, all sats
      int neph(void) throw() {
         int n(0);
         typename EphMap::const_iterator it(pe.begin());
         while(it != pe.end()) {
            n += it->second.size();
            ++it;
         }
         return n;
      }

      //---------------------------------------------------------------
      // Below are interfaces that are unique to this class (i.e. not
      // in the parent class)
      //---------------------------------------------------------------

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

      /// Order of Lagrange interpolation used in getXvt() and getXvt(),
      /// should be even. Usually for 15 minute data, this is the default, 10. 
      unsigned int interpOrder;

         /// Flag to reject satellites with bad or absent positional values
      bool rejectBadPosFlag;

         /// Flag to reject satellites with bad or absent clock values
      bool rejectBadClockFlag;

   }; // end class TabularEphemerisStore

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_TABULAR_EPHEMERIS_STORE_HPP
