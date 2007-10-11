
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
//  Dagoberto Salazar - gAGE. 2007
//
//============================================================================

#include "SourceID.hpp"


/**
 * @file SourceID.cpp
 * gpstk::SourceID - Simple index to represent source of the data.
 */

namespace gpstk
{
   std::map< SourceID::SourceType, std::string > SourceID::stStrings;

   SourceID::Initializer SourceIDsingleton;

   SourceID::Initializer::Initializer()
   {
      stStrings[Unknown] = "UnknownSource";
      stStrings[GPS]      = "GPS";
      stStrings[DGPS]     = "DGPS";
      stStrings[RTK]      = "RTK";
      stStrings[INS]      = "INS";
   }

   std::ostream& SourceID::dump(std::ostream& s) const
   {
      s << SourceID::stStrings[type] << " "
        << sourceName;

      return s;
   } // SourceID::dump()

   bool SourceID::isValid() const
   {
      return !(type==Unknown || sourceName=="");
   }

   SourceID::SourceType SourceID::newSourceType(const std::string& s)
   {
      SourceType newId = 
         static_cast<SourceType>(SourceID::stStrings.rbegin()->first + 1);
      SourceID::stStrings[newId] = s;
      return newId;
   }

   bool SourceID::operator==(const SourceID& right) const
   { return (type==right.type && sourceName==right.sourceName); }

   bool SourceID::operator<(const SourceID& right) const
   {
      if (type == right.type)
         return sourceName < right.sourceName;
      else
         return type < right.type;
   }

   namespace StringUtils
   {
      /// convert this object to a string representation
      std::string asString(const SourceID& p)      
      {
         std::ostringstream oss;
         p.dump(oss);
         return oss.str();
      }
   }
   
   /// stream output for SourceID
   std::ostream& operator<<(std::ostream& s, const SourceID& p)
   {
      p.dump(s);
      return s;
   }

}
