#pragma ident "$Id: SP3EphemerisStore.hpp 1418 2008-10-20 08:41:12Z coandrei $"

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
   class SP3EphemerisStore : public TabularEphemerisStore,
                             public FileStore<SP3Header>
   {
   public:

         /// Constructor.
      SP3EphemerisStore()
         throw()
	: dumpBadPosFlag(false), dumpBadClockFlag(false), haveVelocity(true)
      { TabularEphemerisStore(); };


         /// Destructor.
      virtual ~SP3EphemerisStore() {};


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


         /// Load the given SP3 file
      virtual void loadFile(const std::string& filename)
         throw(FileMissingException);


         /// Set if satellites with bad or absent positional values will be
         /// dumped. It is false by default when object is constructed.
      virtual SP3EphemerisStore& dumpBadPositions(const bool flag)
      { dumpBadPosFlag = true; return (*this); };


         /// Set if satellites with bad or absent clock values will be
         /// dumped. It is false by default when object is constructed.
      virtual SP3EphemerisStore& dumpBadClocks(const bool flag)
      { dumpBadClockFlag = true; return (*this); };


   private:


         /// Flag to dump satellites with bad or absent positional values
      bool dumpBadPosFlag;


         /// Flag to dump satellites with bad or absent clock values
      bool dumpBadClockFlag;


   }; // End of class 'SP3EphemerisStore'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_SP3_EPHEMERIS_STORE_HPP
