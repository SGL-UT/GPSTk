#pragma ident "$Id$"

/**
 * @file BELogEntry.hpp
 * Record the unique identifying information associated
 * with a Broadcast Ephemeris, allow it to be ordered (i.e. map support)
 * and provide a string output capability. 
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

#ifndef GPSTK_BELOGENTRY_HPP
#define GPSTK_BELOGENTRY_HPP

#include "EngEphemeris.hpp"

namespace gpstk
{
   class BELogEntry
   {
   public:
         /// Default constructor
      BELogEntry( const gpstk::EngEphemeris ee );
      
         /// Destructor
      virtual ~BELogEntry() {}

      std::string getStr() const;
      gpstk::DayTime getHOW() const;
      void increment();
      unsigned long getKey() const;
      static const std::string header;
      
      protected:
         DayTime HOWSF1;
         DayTime Toe;
         int PRN_ID;
         int IODC;
         int count;
         unsigned long key;
         
   }; // class BELogEntry

} // namespace

#endif
