//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
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

#ifndef GPSTK_SYSTEMTIME_HPP
#define GPSTK_SYSTEMTIME_HPP

#include "UnixTime.hpp"
#include <ctime>

namespace gpstk
{
      /// @ingroup TimeHandling
      //@{

      /**
       * This class reads the current system time and stores it in
       * a UnixTime.
       */
   class SystemTime
      : public UnixTime
   {
   public:
         /**
          * @name SystemTime Basic Operations
          */
         //@{

         /**
          * Default Constructor.
          * Reads the system clock for the current system time.
          */
      SystemTime()
      {
         setTimeSystem(TimeSystem::UTC);
         update();
      }

         /// Virtual Destructor.
      virtual ~SystemTime()
      {}

         /**
          * Copy Constructor.
          * @param right a reference to the SystemTime object to copy.
          */
      SystemTime( const SystemTime& right )
            : UnixTime( right )
      {}

         //@}

         /**
          * Update this object to the current system time.
          * @return a reference to this SystemTime
          */
      SystemTime& update();
   };

      //@}

}

#endif // GPSTK_SYSTEMTIME_HPP
