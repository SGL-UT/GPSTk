//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#ifndef GPSTK_NAVID_HPP
#define GPSTK_NAVID_HPP

#include <iostream>
#include <iomanip>
#include <sstream>
#include "gps_constants.hpp"

#include "SatID.hpp"
#include "ObsID.hpp"
#include "NavType.hpp"

/**
 * @file NavID.hpp
 * gpstk::NavID - navigation message-independent representation of a satellite.
 */

namespace gpstk
{
      /// @ingroup GNSSEph
      //@{

   class NavID
   {
   public:
         /// empty constructor, creates an invalid object
      NavID() { navType=NavType::Unknown; }

         /// explicit constructor, no defaults
         /// WARNING: This constructor has proven insufficient
         /// for BeiDou.  The BDS ICD requires that PRN 1-5 
         /// use format D2 and PRN 6-30 use format D1.  That
         /// appears to not be followed in all cases.   Therefore
         /// users need to differentiate D1/D2 outside NavID 
         /// and use the explicit constructor
         ///      NavID( NavType::<xxxxx> )
         /// to instatiate a BeiDou-related NavID. 
      NavID( const SatID& sidr, const ObsID& oidr );

      NavID( const NavType nt) { navType = nt; }

      NavID( const std::string& s );

         /// Convenience output method.
      void dump(std::ostream& s) const
      {
         s << convertNavTypeToString(navType);
      }

         /// operator == for NavID
      bool operator==(const NavID& right) const
      { return (navType == right.navType); }

         /// operator != for NavID
      bool operator!=(const NavID& right) const
      { return !(operator==(right)); }

         /// operator < for NavID : order by system, then number
      bool operator<(const NavID& right) const
      {  return (navType<right.navType); }

         /// operator > for NavID
      bool operator>(const NavID& right) const
      {  return (!operator<(right) && !operator==(right)); }

         /// operator <= for NavID
      bool operator<=(const NavID& right) const
      { return (operator<(right) || operator==(right)); }

         /// operator >= for NavID
      bool operator>=(const NavID& right) const
      { return !(operator<(right)); }

      NavType navType;   ///< navType for this satellite
   }; // class NavID

      /// stream output for NavID
   inline std::ostream& operator<<(std::ostream& s, const NavID& p)
   {
      p.dump(s);
      return s;
   }

      //@}

   namespace StringUtils
   {
         /// @ingroup StringUtils
      inline std::string asString(const NavID& p)
      {
         std::ostringstream oss;
         p.dump(oss);
         return oss.str();
      }
   }

} // namespace gpstk

#endif
