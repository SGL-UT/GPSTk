#pragma ident "$Id$"

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

/**
 * @file GSatID.hpp
 * Define a satellite id class for geomatic lib, inheriting SatID.
 */

#ifndef CLASS_GEOMATIC_SATELLITE_ID_INCLUDE
#define CLASS_GEOMATIC_SATELLITE_ID_INCLUDE

#include "Exception.hpp"
#include "SatID.hpp"

namespace gpstk {
   class GSatID : public SatID
   {
   public:

      /// empty constructor, creates an invalid object
      GSatID() throw() { id=-1; system=systemGPS; }

      /// explicit constructor, no defaults
      GSatID(int p, SatelliteSystem s) throw()
      {
         id = p; system = s;
         switch(system) {
            case systemGPS:
            case systemGalileo:
            case systemGlonass:
            case systemGeosync:
            case systemLEO:
            case systemTransit: break;
            default:
               system = systemGPS;
               id = -1;
         }
      }

      /// constructor from string
      GSatID(std::string& str) throw(Exception)
      try { this->fromString(str); }
      catch(Exception& e) { GPSTK_RETHROW(e); }

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

      /// return the single-character system descriptor
      char systemChar() const throw()
      {
         switch(system) {
            case systemGPS: return 'G';
            case systemGalileo: return 'E';
            case systemGlonass: return 'R';
            case systemGeosync: return 'S';
            case systemTransit: return 'T';
            case systemLEO: return 'L';
            default: return '?';
         }
      };

      /// return string describing system
      std::string systemString() const throw()
      {
         switch(system) {
            case systemGPS:     return "GPS";
            case systemGalileo: return "Galileo";
            case systemGlonass: return "Glonass";
            case systemGeosync: return "Geosync";
            case systemTransit: return "Transit";
            case systemLEO: return "LEO";
            default:            return "Unknown";
         }
      };

      /// read from string
      /// @note GPS is default system (no or unknown system char)
      void fromString(const std::string s) throw(Exception)
      {
         char c;
         std::istringstream iss(s);

         id = -1; system = systemGPS;  // default
         if(s.find_first_not_of(std::string(" \t\n"), 0) == std::string::npos)
            return;                    // all whitespace yields the default

         iss >> c;                     // read one character (non-whitespace)
         switch(c)
         {
                                       // no leading system character
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
               iss.putback(c);
               system = SatID::systemGPS;
               break;
            case 'R': case 'r':
               system = SatID::systemGlonass;
               break;
            case 'T': case 't':
               system = SatID::systemTransit;
               break;
            case 'S': case 's':
               system = SatID::systemGeosync;
               break;
            case 'E': case 'e':
               system = SatID::systemGalileo;
               break;
            case 'L': case 'l':
               system = SatID::systemLEO;
               break;
            case ' ': case 'G': case 'g':
               system = SatID::systemGPS;
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
