#pragma ident "$Id$"

#ifndef GPSTK_TIMESYSTEM_HPP
#define GPSTK_TIMESYSTEM_HPP

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
#include <iomanip>

namespace TimeSystem
{

   class TimeSystem
   {
   public:

      enum Systems
      {
         Unknown,    /**< unknown time frame; mostly for legacy code compatibility */
         Any,        /**< wildcard; allows comparison with any other type */
         GPS,        /**< GPS system time */
         UTC         /**< Coordinated Universal Time (e.g., from NTP) */
      };

         //Constructor
      TimeSystem(Systems sys = Unknown)
         throw()
      {
         if (sys < 0 || sys > 3)
         {
            ts = Unknown;
         }
         else
         {
            ts = sys;
         }
      }

         //Copy Constructor
      TimeSystem( const TimeSystem& right )
         throw()
         : ts(right.ts)
      {}

        //Assignment Opperator
      TimeSystem& operator=( const TimeSystem& right )
         throw()
      {
        return *this;
      }

      std::string printf() const
         throw();

      Systems getTimeSystem() const
         throw()
      { return ts; }

      void setTimeSystem( const Systems& sys )
         throw()
      { ts = sys; }
      
   private:

      Systems ts;

   };//class

}//namespace


#endif // GPSTK_TIMESYSTEM_HPP
