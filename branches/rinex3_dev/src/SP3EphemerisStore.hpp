#pragma ident "$Id$"

/**
 * @file SP3EphemerisStore.hpp
 * Read and store SP3 formated ephemeris data
 */

#ifndef GPSTK_SP3_EPHEMERIS_STORE_HPP
#define GPSTK_SP3_EPHEMERIS_STORE_HPP

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


#include <iostream>

#include "TabularEphemerisStore.hpp"
#include "FileStore.hpp"

#include "SP3Stream.hpp"
#include "SP3Data.hpp"
#include "SP3Header.hpp"

namespace gpstk
{

      /** @addtogroup ephemstore */
      //@{

      /**
       * This adds the interface to read SP3 files into a TabularEphemerisStore
       */
   class SP3EphemerisStore : public TabularEphemerisStore<Xvt>,
                             public FileStore<SP3Header>
   {
   public:

         /// Constructor.
      SP3EphemerisStore()
         throw()
	: rejectBadPosFlag(true), rejectBadClockFlag(true)
     { TabularEphemerisStore<Xvt>(); };


         /// Destructor.
      virtual ~SP3EphemerisStore() {};


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


         /** Dump the store to cout.
          * @param detail determines how much detail to include in the output
          *   0 list of filenames with their start, stop times.
          *   1 list of filenames with their start, stop times,
          *     other header information and prns/accuracy.
          *   2 above, plus dump all the PVT data (use judiciously).
          */
      virtual void dump( std::ostream& s=std::cout,
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


         /// Load the given SP3 file
      virtual void loadFile(const std::string& filename)
         throw( FileMissingException );


         /// Set if satellites with bad or absent position values will be
         /// rejected. It is false by default when object is constructed.
      void rejectBadPositions(const bool flag) { rejectBadPosFlag = flag; }

         /// Set if satellites with bad or absent clock values will be
         /// rejected. It is false by default when object is constructed.
      void rejectBadClocks(const bool flag) { rejectBadClockFlag = flag; }

         /// Insert a new SP3Data object into the store
      void addEphemeris(const SP3Data& data)
         throw();

         /// Insert position data into the store at time t
         /// @param t   Time of the data
         /// @param sat Satellite id of the data
         /// @param x   X component of position in km
         /// @param y   Y component of position in km
         /// @param z   Z component of position in km
         /// @param c   Clock bias in microsec
      void addPositionData( const CommonTime& t,
                            const SatID& sat,
                            const double& x,
                            const double& y,
                            const double& z,
                            const double& c      )
         throw();

         /// Insert velocity data into the store at time t
         /// @param t   Time of the data
         /// @param sat Satellite id of the data
         /// @param vx  X component of velocity in decimeters/sec
         /// @param vy  Y component of velocity in decimeters/sec
         /// @param vz  Z component of velocity in decimeters/sec
         /// @param vc  Clock drift in 1.e-4 microsec/sec
      void addVelocityData( const CommonTime& t,
                            const SatID& sat,
                            const double& vx,
                            const double& vy,
                            const double& vz,
                            const double& vc     )
         throw();

         /// Insert position and velocity data into the store at time t
         /// @param t   Time of the data
         /// @param sat Satellite id of the data
         /// @param xvt Xvt containing position, velocity, clk bias and drift,
         ///      in the units specified in addPositionData() and addVelocityData()
      void addData(const CommonTime& t, const SatID& sat, const Xvt& xvt)
         throw();

         /// Remove all data
      void clear()
         throw();

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

//   private:

         /// The key to this map is the time
      typedef std::map<CommonTime, Xvt> SvEphMap;

         /// The key to this map is the svid of the satellite (usually the prn)
      typedef std::map<SatID, SvEphMap> EphMap;

         /// the map of SVs and XVTs
      EphMap pe;

   private:

         /// Flag to reject satellites with bad or absent positional values
      bool rejectBadPosFlag;

         /// Flag to reject satellites with bad or absent clock values
      bool rejectBadClockFlag;

   }; // End of class 'SP3EphemerisStore'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_SP3_EPHEMERIS_STORE_HPP
