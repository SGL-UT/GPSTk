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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#ifndef GPSTK_SATID_HPP
#define GPSTK_SATID_HPP

#include <iostream>
#include <iomanip>
#include <sstream>
#include "gps_constants.hpp"

/**
 * @file SatID.hpp
 * gpstk::SatID - navigation system-independent representation of a satellite.
 */

namespace gpstk
{
      // forward declarations
   class SatID;
//   std::istream& operator<<(std::istream& s, SatID& p);

      /// @ingroup GNSSEph
      //@{

      /// Satellite identifier consisting of a satellite number (PRN, etc.)
      /// and a satellite system. For GLONASS (systemGlonass), the identifier
      /// is the slot number as per section 3.5 of the RINEX 3 spec.
      /// For SBAS (systemGeosync), the id is the PRN-100.
   class SatID
   {
   public:
         /** Supported satellite systems
          * @note any additions to this enumeration list should also
          *   be added to convertSatelliteSystemToString() and
          *   convertStringToSatelliteSystem() and to SatID_T. */
      enum SatelliteSystem
      {
         systemGPS = 1,
         systemGalileo,
         systemGlonass,
         systemGeosync,
         systemLEO,
         systemTransit,
         systemBeiDou,
         systemQZSS,
         systemIRNSS,
         systemMixed,
         systemUserDefined,
         systemUnknown
      };

         /// empty constructor, creates an invalid object
      SatID() { id=-1; system=systemGPS; }

         /// explicit constructor, no defaults
         /// @note if s is given a default value here,
         /// some compilers will silently cast int to SatID.
      SatID(int p, const SatelliteSystem& s) { id=p; system=s; }

         // operator=, copy constructor and destructor built by compiler

         /** Translate system enumeration to its string representation.
          * @note The string representation is being used in file
          *   formats, e.g. RawNavCSVHeader.  The string values should
          *   not be changed if at all possible, as that would break
          *   the ability to read older files.
          * @note Any new systems should not contain spaces in the
          *   string values.
          * @note The translations here should precisely match those
          *   in convertStringToSatelliteSystem.
          * @param[in] s The system to get the string name of.
          * @return A space-free string containing the name of the GNSS.
          */
      static inline std::string convertSatelliteSystemToString(
         const SatelliteSystem s);

         /** Translate GNSS names as strings into system enumeration
          * equivalents.
          * @see convertSatelliteSystemToString
          * @param[in] s The GNSS name to convert to enumeration.
          * @return An enumeration equivalent of the given string.
          *   systemUnknown is returned for any names that do not
          *   exactly match known values.
          */
      static inline SatelliteSystem convertStringToSatelliteSystem(
         const std::string& s);

         /// Convenience output method.
      void dump(std::ostream& s) const
      {
         s << convertSatelliteSystemToString(system) << " " << id;
      }

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

      int id;                   ///< satellite identifier, e.g. PRN
      SatelliteSystem system;   ///< system for this satellite

   }; // class SatID

      /// stream output for SatID
   inline std::ostream& operator<<(std::ostream& s, const SatID& p)
   {
      p.dump(s);
      return s;
   }

   std::string SatID ::
   convertSatelliteSystemToString(SatelliteSystem s)
   {
      switch(s)
      {
         case systemGPS:         return "GPS";           break;
         case systemGalileo:     return "Galileo";       break;
         case systemGlonass:     return "GLONASS";       break;
         case systemGeosync:     return "Geostationary"; break;
         case systemLEO:         return "LEO";           break;
         case systemTransit:     return "Transit";       break;
         case systemBeiDou:      return "BeiDou";        break;
         case systemQZSS:        return "QZSS";          break;
         case systemIRNSS:       return "IRNSS";         break;
         case systemMixed:       return "Mixed";         break;
         case systemUserDefined: return "UserDefined";   break;
         case systemUnknown:     return "Unknown";       break;
         default:                return "??";            break;
      }
   }

   SatID::SatelliteSystem SatID ::
   convertStringToSatelliteSystem(const std::string& s)
   {
      if (s == "GPS")
         return systemGPS;
      if (s == "Galileo")
         return systemGalileo;
      if (s == "GLONASS")
         return systemGlonass;
      if (s == "Geostationary")
         return systemGeosync;
      if (s == "LEO")
         return systemLEO;
      if (s == "Transit")
         return systemTransit;
      if (s == "BeiDou")
         return systemBeiDou;
      if (s == "QZSS")
         return systemQZSS;
      if (s == "IRNSS")
         return systemIRNSS;
      if (s == "Mixed")
         return systemMixed;
      if (s == "UserDefined")
         return systemUserDefined;
      return systemUnknown;
   }

      //@}

   namespace StringUtils
   {
         /// @ingroup StringUtils
         //@{

         /// SatID as a string
      inline std::string asString(const SatID& p)
      {
         std::ostringstream oss;
         p.dump(oss);
         return oss.str();
      }


         /// SatelliteSystem as a string
      inline std::string asString(const SatID::SatelliteSystem& s)
      {
         SatID sat(-1,s);
         std::string str(asString(sat));
         std::string::size_type pos = str.find(' ',0);
         return (std::string(str,0,pos));
      }
   }

} // namespace gpstk

#endif
