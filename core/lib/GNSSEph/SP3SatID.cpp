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

#include "SP3SatID.hpp"

/**
 * @file SP3SatID.cpp
 * gpstk::SP3SatID - navigation system-independent representation of a satellite
 * as defined by the SP3 specification.
 */

namespace gpstk
{
   char SP3SatID::fillchar = '0';

   SP3SatID ::
   SP3SatID(int p, SatelliteSystem s) throw()
         : SatID(p,s)
   {
      validate();
   }


   char SP3SatID ::
   systemChar() const throw()
   {
      switch (system)
      {
         case systemGPS:     return 'G';
         case systemGalileo: return 'E';
         case systemGlonass: return 'R';
         case systemLEO:     return 'L';
         case systemBeiDou:  return 'C';
         case systemQZSS:    return 'J';
         case systemMixed:   return 'M';
               // non-SP3
         default: return '?';
      }
   }


   std::string SP3SatID ::
   systemString() const throw()
   {
      switch (system)
      {
         case systemGPS:     return "GPS";
         case systemGalileo: return "Galileo";
         case systemGlonass: return "Glonass";
         case systemLEO:     return "LEO";
         case systemBeiDou:  return "BeiDou";
         case systemQZSS:    return "QZSS";
         case systemMixed:   return "Mixed";
         default:            return "Unknown";
      }
   }


   void SP3SatID ::
   fromString(const std::string s)
   {
      char c;
      std::istringstream iss(s);

      id = -1;
      system = systemGPS;  // default
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
         case ' ': case 'G': case 'g':
            system = SatID::systemGPS;
            break;
         case 'R': case 'r':
            system = SatID::systemGlonass;
            break;
         case 'E': case 'e':
            system = SatID::systemGalileo;
            break;
         case 'L': case 'l':
            system = SatID::systemLEO;
            break;
         case 'C': case 'c':
            system = SatID::systemBeiDou;
            break;
         case 'J': case 'j':
            system = SatID::systemQZSS;
            break;
         case 'M': case 'm':
            system = SatID::systemMixed;
            break;
         default:                   // non-SP3 system character
            Exception e(std::string("Invalid system character \"")
                        + c + std::string("\""));
            GPSTK_THROW(e);
      }
      iss >> id;
      if(id <= 0)
      {
         id = -1;
      }
      else
      {
            // do the kludging that SP3 does for PRNs > 99
         switch (system)
         {
            case SatID::systemQZSS:
               id += 192;
               break;
         }
      }
   }


   std::string SP3SatID ::
   toString() const throw()
   {
      std::ostringstream oss;
      oss.fill(fillchar);
      int sp3ID = id;
         // do the kludging that SP3 does for PRNs > 99
      switch (system)
      {
         case SatID::systemQZSS:
            sp3ID -=192;
            break;
      }
      oss << systemChar()
          << std::setw(2) << sp3ID;
      return oss.str();
   }


   void SP3SatID ::
   validate()
   {
      switch(system)
      {
         case systemGPS:
         case systemGlonass:
         case systemGalileo:
         case systemLEO: 
         case systemBeiDou:
         case systemQZSS: 
         case systemMixed: break;
               // invalidate anything non-SP3
         default:
            system = systemUnknown;
            id = -1;
      }
   }
}
