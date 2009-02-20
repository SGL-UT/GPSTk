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

namespace gpstk
{
      /** @addtogroup ephemstore */
      //@{

      /// Store a tabular list of Xvt data (such as a table of precise
      /// ephemeris data in an SP3 file) and compute Xvt from this table.
      /// A Lagrange interpolation is used to compute the Xvt for times that
      /// are not in the table but do have sufficient data.
   class TabularEphemerisStore : public XvtStore<SatID>
   {
   public:

         /// Default constructor
      TabularEphemerisStore()
         throw()
         : haveVelocity(true), initialTime(CommonTime::END_OF_TIME),
           finalTime(CommonTime::BEGINNING_OF_TIME), checkDataGap(false),
           gapInterval(901.0), checkInterval(false), maxInterval(8105.0),
           interpOrder(10)
      {};

         /// Destructor
      virtual ~TabularEphemerisStore() {};

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
      virtual Xvt getXvt( const SatID id,
                          const CommonTime& t )
         const throw( gpstk::InvalidRequest );

         /// A debugging function that outputs in human readable form,
         ///  all data stored in this object.
         /// 
         /// @param[in] s the stream to receive the output; defaults to cout
         /// @param[in] detail the level of detail to provide
      virtual void dump( std::ostream& s = std::cout,
                         short detail = 0 )
         const throw();

         /// Edit the dataset, removing data outside the indicated time
         ///  interval.
         /// 
         /// @param[in] tmin defines the beginning of the time interval
         /// @param[in] tmax defines the end of the time interval
      virtual void edit( const CommonTime& tmin,
                         const CommonTime& tmax = CommonTime::END_OF_TIME )
         throw();

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

         /// Check if this ephemeris contains clock information.
      virtual bool clockIsPresent()
         const throw()
      { return true; };

      //---------------------------------------------------------------
      // Below are interfaces that are unique to this class (i.e. not
      // in the parent class)
      //---------------------------------------------------------------

         /// Insert position data into the store at time t
         /// @param t   Time of the data
         /// @param sat Satellite id of the data
         /// @param x   X component of position in km
         /// @param y   Y component of position in km
         /// @param z   Z component of position in km
         /// @param c   Clock bias in microsec
      void addPositionData(const CommonTime& t,
                           const SatID& sat,
                           const double& x,
                           const double& y,
                           const double& z,
                           const double& c)
         throw();

         /// Insert velocity data into the store at time t
         /// @param t   Time of the data
         /// @param sat Satellite id of the data
         /// @param vx  X component of velocity in decimeters/sec
         /// @param vy  Y component of velocity in decimeters/sec
         /// @param vz  Z component of velocity in decimeters/sec
         /// @param vc  Clock drift in 1.e-4 microsec/sec
      void addVelocityData(const CommonTime& t,
                           const SatID& sat,
                           const double& vx,
                           const double& vy,
                           const double& vz,
                           const double& vc)
         throw();

         /// Insert position and velocity data into the store at time t
         /// @param t   Time of the data
         /// @param sat Satellite id of the data
         /// @param xvt Xvt containing position, velocity, clk bias and drift,
         ///      in the units specified in addPositionData() and addVelocityData()
      void addData(const CommonTime& t, const SatID& sat, const Xvt& xvt)
         throw();

         /// Remove all data
      void clear() throw();

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

         /// Flag indicating that velocity data present in all datasets loaded.
      bool haveVelocity;

   private:

         /// The key to this map is the time
      typedef std::map<CommonTime, Xvt> SvEphMap;

         /// The key to this map is the svid of the satellite (usually the prn)
      typedef std::map<SatID, SvEphMap> EphMap;

         /// the map of SVs and XVTs
      EphMap pe;

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

   }; // end class TabularEphemerisStore

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_TABULAR_EPHEMERIS_STORE_HPP
