#pragma ident "$Id: TimeSystem.hpp 1649 2009-01-27 23:53:20Z raindave $"

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
#include <string>

namespace gpstk
{
   /// This class encapsulates time systems, including std::string I/O
   class TimeSystem
   {
   public:

      /// list of time systems supported by this class
      enum SystemsEnum
      {
         Unknown = 0, ///< unknown time frame; for legacy code compatibility
         Any,         ///< wildcard; allows comparison with any other type
         GPS,         ///< GPS system time
         GLO,         ///< GLONASS system time
         GAL,         ///< Galileo system time
         UTC,         ///< Coordinated Universal Time (e.g., from NTP)
         TAI          ///< International Atomic Time

         // Add new GNSS systems *before* UTC, or other new time systems
         // before TAI, then modify Strings[] in TimeSystem.cpp.
      };

      /// Constructor, including empty constructor
      TimeSystem(SystemsEnum sys = Unknown)
         throw()
      {
         if(sys < 0 || sys > TAI)
            system = Unknown;
         else
            system = sys;
      }

      /// constructor from int
      TimeSystem(int i)
         throw()
      {
         if(i < 0 || i > TAI)
            system = Unknown;
         else
            system = static_cast<SystemsEnum>(i);
      }

      // (copy constructor and operator= are defined by compiler)
      
      void setTimeSystem( const SystemsEnum& sys )
         throw();

      SystemsEnum getTimeSystem() const
         throw()
      { return system; }

      /// Return a std::string for each system (these strings are const and static).
      /// @return the std::string
      std::string asString() const
         throw()
      { return Strings[system]; }

      /// define system based on input string
      /// @param str input string, expected to match output string for given system
      void fromString(const std::string str)
         throw();
      
      /// boolean operator==
      bool operator==(const TimeSystem& right) const
         throw()
      { return system == right.system; }

      /// boolean operator!=
      bool operator!=(const TimeSystem& right) const
         throw()
      { return system != right.system; }

      /// boolean operator< (used by STL to sort)
      bool operator<(const TimeSystem& right) const
         throw()
      { return system < right.system; }


   private:

      /// time system (= element of SystemsEnum enum) for this object
      SystemsEnum system;

      /// set of string labels for elements of SystemsEnum
      static const std::string Strings[];

   };   // end class TimeSystem

}   // end namespace

#endif // GPSTK_TIMESYSTEM_HPP
