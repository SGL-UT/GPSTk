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

/// @file GSatID.hpp
/// Define a satellite id class for geomatic lib, inheriting SatID.

#ifndef CLASS_GEOMATIC_SATELLITE_ID_INCLUDE
#define CLASS_GEOMATIC_SATELLITE_ID_INCLUDE

#include "Exception.hpp"
#include "SatID.hpp"

namespace gpstk {
   class GSatID : public SatID
   {
   public:

      /// empty constructor, creates an invalid object
      GSatID() throw() { id=-1; system=SatelliteSystem::GPS; }

      /// explicit constructor, no defaults
      GSatID(int p, SatelliteSystem s) throw()
      {
         id = p; system = s;
         switch(system) {
            case SatelliteSystem::GPS:
            case SatelliteSystem::Galileo:
            case SatelliteSystem::Glonass:
            case SatelliteSystem::Geosync:
            case SatelliteSystem::LEO:
            case SatelliteSystem::Transit:
            case SatelliteSystem::QZSS:
            case SatelliteSystem::BeiDou:
            case SatelliteSystem::IRNSS:
               break;
            default:
               system = SatelliteSystem::GPS;
               id = -1;
         }
      }

         /** constructor from string
          * @throw Exception
          */
      GSatID(std::string& str)
      {
         try
         {
            this->fromString(str);
         }
         catch(Exception& e)
         {
            GPSTK_RETHROW(e);
         }
      }

      /// cast SatID to GSatID
      GSatID(const SatID& sat) throw()
         { *this = GSatID(sat.id,sat.system); }

      /// set the fill character used in output
      /// return the current fill character
      char setfill(char c) throw()
         { char csave=fillchar; fillchar=c; return csave; }

      /// get the fill character used in output
      char getfill() throw()
         { return fillchar; }

      // operator=, copy constructor and destructor built by compiler

      /// operator ==
      bool operator==(const GSatID& right) const throw()
         { return ((system == right.system) && (id == right.id)); }

      /// operator< (used by STL to sort)
      bool operator<(const GSatID& right) const throw()
      {
         if (system==right.system)
            return (id<right.id);
         return (system<right.system);
      }

      // the rest follow from Boolean algebra...
      /// boolean operator!=
      bool operator!=(const GSatID& right) const throw()
         { return !operator==(right); }

      /// boolean operator>=
      bool operator>=(const GSatID& right) const throw()
         { return !operator<(right); }

      /// boolean operator<=
      bool operator<=(const GSatID& right) const throw()
         { return (operator<(right) || operator==(right)); }

      /// boolean operator>
      bool operator>(const GSatID& right) const throw()
         { return (!operator<(right) && !operator==(right)); }

      /// return the single-character system descriptor
      char systemChar() const throw()
      {
         switch(system) {
            case SatelliteSystem::GPS:     return 'G';
            case SatelliteSystem::Galileo: return 'E';
            case SatelliteSystem::Glonass: return 'R';
            case SatelliteSystem::Geosync: return 'S';
            case SatelliteSystem::Transit: return 'T';
            case SatelliteSystem::LEO:     return 'L';
            case SatelliteSystem::QZSS:    return 'J';
            case SatelliteSystem::BeiDou:  return 'C';
            case SatelliteSystem::IRNSS:   return 'I';
            default:                       return '?';
         }
      };

      /// return string describing system
      std::string systemString() const throw()
      {
         switch(system) {
            case SatelliteSystem::GPS:     return "GPS";
            case SatelliteSystem::Galileo: return "Galileo";
            case SatelliteSystem::Glonass: return "Glonass";
            case SatelliteSystem::Geosync: return "Geosync";
            case SatelliteSystem::Transit: return "Transit";
            case SatelliteSystem::LEO:     return "LEO";
            case SatelliteSystem::QZSS:    return "QZSS";
            case SatelliteSystem::BeiDou:  return "BeiDou";
            case SatelliteSystem::IRNSS:   return "IRNSS";
            default:                       return "Unknown";
         }
      };

      /// read from string
      /// @note GPS is default system (no or unknown system char)
      /// @throw Exception
      void fromString(const std::string s)
      {
         char c;
         std::istringstream iss(s);

         id = -1; system = SatelliteSystem::GPS;  // default
         if(s.find_first_not_of(std::string(" \t\n"), 0) == std::string::npos)
            return;                    // all whitespace yields the default

         iss >> c;                     // read one character (non-whitespace)
         switch(c)
         {
                                       // no leading system character
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
               iss.putback(c);
               system = SatelliteSystem::GPS;
               break;
            case 'R': case 'r':
               system = SatelliteSystem::Glonass;
               break;
            case 'T': case 't':
               system = SatelliteSystem::Transit;
               break;
            case 'S': case 's':
               system = SatelliteSystem::Geosync;
               break;
            case 'E': case 'e':
               system = SatelliteSystem::Galileo;
               break;
            case 'L': case 'l':
               system = SatelliteSystem::LEO;
               break;
            case ' ': case 'G': case 'g':
               system = SatelliteSystem::GPS;
               break;
            case 'J': case 'j':
               system = SatelliteSystem::QZSS;
               break;
            case 'C': case 'c':
               system = SatelliteSystem::BeiDou;
               break;
            case 'I': case 'i':
               system = SatelliteSystem::IRNSS;
               break;
            default:                   // invalid system character
               Exception e(std::string("Invalid system character \"")
                           + c + std::string("\""));
               GPSTK_THROW(e);
         }
         iss >> id;
         if(id <= 0) id = -1;
      }

      /// convert to string
      std::string toString() const throw()
      {
         std::ostringstream oss;
         char savechar=oss.fill(fillchar);
         oss << systemChar()
             << std::setw(2) << id
             << std::setfill(savechar);
          return oss.str();
      }

   private:

      static char fillchar;  ///< fill character used during stream output

   }; // class GSatID

   /// stream output for GSatID
   inline std::ostream& operator<<(std::ostream& s, const GSatID& sat) throw()
   {
      s << sat.toString();
      return s;
   }

} // namespace gpstk

#endif
// nothing below this
