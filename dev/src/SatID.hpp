#pragma ident "$Id$"

#ifndef GPSTK_SATID_HPP
#define GPSTK_SATID_HPP

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
#include <sstream>
#include <gps_constants.hpp>

/**
 * @file SatID.hpp
 * gpstk::SatID - navigation system-independent representation of a satellite.
 */

namespace gpstk
{
   // forward declarations
   class SatID;
   std::istream& operator>>(std::istream& s, SatID& p);

   /// Satellite identifier consisting of a satellite number (PRN, etc.)
   /// and a satellite system
   class SatID
   {
   public:
      /// Supported satellite systems
      enum SatelliteSystem
      {
         systemGPS = 1,
         systemGalileo,
         systemGlonass,
         systemGeosync,
         systemLEO,
         systemTransit
      };

      /// empty constructor, creates an invalid object
      SatID() { id=-1; system=systemGPS; }

      /// explicit constructor, no defaults
      /// @note if s is given a default value here,
      /// some compilers will silently cast int to SatID.
      SatID(int p, SatelliteSystem s) { id=p; system=s; }

      // operator=, copy constructor and destructor built by compiler

      /// return a string with system description (no whitespace)
      std::string systemString() const
      {
         switch(system) {
            case systemGPS: return "GPS";
            case systemGalileo: return "Galileo";
            case systemGlonass: return "GLONASS";
            case systemGeosync: return "Geostationary";
            case systemLEO: return "LEO";
            case systemTransit: return "Transit";
            default: return "??";
         }
      };

      /// operator == for SatID
      bool operator==(const SatID& right) const
      { return ((system == right.system) && (id == right.id)); }

      /// operator != for SatID
      bool operator!=(const SatID& right) const
      { return !(operator==(right)); }

      /// operator < for SatID : order by system, then number
      bool operator<(const SatID& right) const
      {
         if (system==right.system)
            return (id<right.id);
         return (system<right.system);
      }

      /// operator > for SatID
      bool operator>(const SatID& right) const
      {  return (!operator<(right) && !operator==(right)); }

      /// operator <= for SatID
      bool operator<=(const SatID& right) const
      { return (operator<(right) || operator==(right)); }

      /// operator >= for SatID
      bool operator>=(const SatID& right) const
      { return !(operator<(right)); }

      /// convert to string
      std::string toString() const
      {
         char ch=' ';
         switch (system)
         {
            case systemGPS:     ch = 'G'; break;
            case systemGalileo: ch = 'E'; break;
            case systemGlonass: ch = 'R'; break;
            case systemGeosync: ch = 'S'; break;
            case systemLEO:     ch = 'L'; break;
            case systemTransit: ch = 'T'; break;
            default:            ch = '?';
         }
         std::ostringstream oss;
         char savechar=oss.fill('0');
         oss << ch << std::setw(2) << id << std::setfill(savechar);
         return oss.str();
      }

      /// return true if this is a valid SatID
      /// @note assumes all id's are positive and less than 100;
      ///     plus GPS id's are less than or equal to MAX_PRN (32).
      /// @note this is not used internally in the gpstk library
      bool isValid() const
      {
         switch(system)
         {
            case systemGPS: return (id > 0 && id <= MAX_PRN);
            //case systemGalileo:
            //case systemGlonass:
            //case systemGeosync:
            //case systemLEO:
            //case systemTransit:
            default: return (id > 0 && id < 100);
         }
      }

      /// return true if this is a valid SatID in the RINEX specification
      /// @note assumes all id's are positive and less than 100;
      ///     plus GPS id's are less than or equal to MAX_PRN (32).
      /// @note this is not used internally in the gpstk library
      bool isValidRinex() const
      {
         switch(system)
         {
            case systemGPS:
            case systemGalileo:
            case systemGlonass:
            case systemGeosync:
            case systemTransit:
               return isValid();
            default:
               return false;
         }
      }

      /// return true if this is a valid SatID in the SP3 specification
      /// @note assumes all id's are positive and less than 100
      /// @note this is not used internally in the gpstk library
      bool isValidSP3() const
      {
         switch(system)
         {
            case systemGPS:
            case systemGalileo:
            case systemGlonass:
            case systemLEO:
               return isValid();
            default:
               return false;
         }
      }

      int id;                   ///< satellite identifier, e.g. PRN
      SatelliteSystem system;   ///< system for this satellite

   }; // class SatID

      /// stream output for SatID
   inline std::ostream& operator<<(std::ostream& s, const SatID& p)
   {
      s << p.toString();
      return s;
   }

} // namespace gpstk

#endif
