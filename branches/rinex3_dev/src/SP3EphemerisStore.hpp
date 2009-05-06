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
#include <string>

#include "TabularEphemerisStore.hpp"
#include "FileStore.hpp"
#include "CommonTime.hpp"
#include "SatID.hpp"
#include "Xvt.hpp"
#include "SP3Data.hpp"
#include "SP3Header.hpp"
#include "SP3Stream.hpp"

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
     { TabularEphemerisStore<Xvt>(); };


         /// Destructor.
      virtual ~SP3EphemerisStore() {};

      virtual void dump( std::ostream& s=std::cout,
                         short detail = 0 )
         const throw();

         /// Load the given SP3 file
      virtual void loadFile(const std::string& filename)
         throw( FileMissingException );


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

   }; // End of class 'SP3EphemerisStore'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_SP3_EPHEMERIS_STORE_HPP
