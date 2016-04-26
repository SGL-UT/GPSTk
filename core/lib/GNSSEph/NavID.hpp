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

#ifndef GPSTK_NAVID_HPP
#define GPSTK_NAVID_HPP

#include <iostream>
#include <iomanip>
#include <sstream>
#include "gps_constants.hpp"

#include "SatID.hpp"
#include "ObsID.hpp"

/**
 * @file NavID.hpp
 * gpstk::NavID - navigation message-independent representation of a satellite.
 */

namespace gpstk
{
      // forward declarations
   class NavID;

      /// @ingroup GNSSEph
      //@{

   class NavID
   {
   public:
         /// Supported navigation types
         //***NOTE***//
         //If adding a new NavType enumerator, also add to string
         //input constructor and convertNavTypeToString constructor.
      enum NavType
      {
         ntGPSLNAV,
         ntGPSCNAVL2,
         ntGPSCNAVL5,
         ntGPSMNAV,
         ntBeiDou_D1,
         ntBeiDou_D2,
         ntGloCivilF,
         ntGloCivilC,
         ntGalOS,
         ntUnknown
      };
   
         /// empty constructor, creates an invalid object
      NavID() { navType=ntUnknown; }

         /// explicit constructor, no defaults
      NavID( const SatID& sidr, const ObsID& oidr );

      NavID( const NavType nt) { navType = nt; }

      NavID( const std::string& s );

         /// Convenience method used by dump().
      std::string convertNavTypeToString( const NavType& s ) const
       {
          //define retVal for safety return
         std::string retVal = "";
         switch(s)
         {
            case ntGPSLNAV:      {retVal = NavTypeStrings[0];     break;}
            case ntGPSCNAVL2:    {retVal = NavTypeStrings[1];     break;}
            case ntGPSCNAVL5:    {retVal = NavTypeStrings[2];     break;}
            case ntGPSMNAV:      {retVal = NavTypeStrings[3];     break;}
            case ntBeiDou_D1:    {retVal = NavTypeStrings[4];     break;}
            case ntBeiDou_D2:    {retVal = NavTypeStrings[5];     break;}
            case ntGloCivilF:    {retVal = NavTypeStrings[6];     break;}
            case ntGloCivilC:    {retVal = NavTypeStrings[7];     break;}
            case ntGalOS:        {retVal = NavTypeStrings[8];     break;}
            default: ntUnknown:  {retVal = NavTypeStrings[9];     break;}
         };
         //return retVal in case switch isn't reached
        return retVal;
      }

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
      static const std::string NavTypeStrings[];

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
