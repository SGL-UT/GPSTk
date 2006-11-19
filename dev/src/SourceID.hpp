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
//  Dagoberto Salazar - gAGE. 2006
//
//============================================================================


#ifndef GPSTK_SOURCEID_HPP
#define GPSTK_SOURCEID_HPP

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>

/**
 * @file SourceID.hpp
 * gpstk::SourceID - Simple index to represent source of the data.
 */

namespace gpstk
{
    /** @addtogroup DataStructures */

   class SourceID
   {
   public:
      /// The type of source.
      enum SourceType
      {
         stUnknown,
         stGPS,     /// GPS data
         stDGPS,    /// Differential GPS data
         stRTK,     /// Real Time Kinematic data
         stINS,     /// Inertial System data
         stLast,    /// used to extend this...
         stPlaceholder = stLast+1000
      };

      /// empty constructor, creates an unknown source data object
      SourceID()
         : type(stUnknown), sourceName("") {};

      /// Explicit constructior
      SourceID(SourceType st, std::string name)
         : type(st), sourceName(name) {};

      /// Equality requires all fields to be the same
      virtual bool operator==(const SourceID& right) const;

      /// Ordering is arbitrary but required to be able to use a SourceID
      /// as an index to a std::map. If an application needs
      /// some other ordering, inherit and override this function.
      virtual bool operator<(const SourceID& right) const;

      bool operator!=(const SourceID& right) const
      { return !(operator==(right)); }

      bool operator>(const SourceID& right) const
      {  return (!operator<(right) && !operator==(right)); }

      bool operator<=(const SourceID& right) const
      { return (operator<(right) || operator==(right)); }

      bool operator>=(const SourceID& right) const
      { return !(operator<(right)); }

      /// Destructor
      virtual ~SourceID() {}

      static SourceType newSourceType(const std::string& s);

      SourceType  type;
      std::string sourceName;

   private:
      static std::map< SourceType, std::string > stStrings;

   public:
      class Initializer
      {
      public:
         Initializer();
      };

      static Initializer singleton;

   }; // class SourceID

   namespace StringUtils
   {
      /// convert this object to a string representation
      std::string asString(const SourceID& p);
   }
   
   /// stream output for SourceID
   std::ostream& operator<<(std::ostream& s, const SourceID& p);

} // namespace gpstk
#endif
