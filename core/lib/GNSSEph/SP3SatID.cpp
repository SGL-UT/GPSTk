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
         case SatelliteSystem::GPS:     return 'G';
         case SatelliteSystem::Galileo: return 'E';
         case SatelliteSystem::Glonass: return 'R';
         case SatelliteSystem::LEO:     return 'L';
         case SatelliteSystem::BeiDou:  return 'C';
         case SatelliteSystem::QZSS:    return 'J';
         case SatelliteSystem::Mixed:   return 'M';
               // non-SP3
         default: return '?';
      }
   }


   std::string SP3SatID ::
   systemString() const throw()
   {
      switch (system)
      {
         case SatelliteSystem::GPS:     return "GPS";
         case SatelliteSystem::Galileo: return "Galileo";
         case SatelliteSystem::Glonass: return "Glonass";
         case SatelliteSystem::LEO:     return "LEO";
         case SatelliteSystem::BeiDou:  return "BeiDou";
         case SatelliteSystem::QZSS:    return "QZSS";
         case SatelliteSystem::Mixed:   return "Mixed";
         default:                       return "Unknown";
      }
   }


   void SP3SatID ::
   fromString(const std::string s)
   {
      char c;
      std::istringstream iss(s);

      id = -1;
      system = SatelliteSystem::GPS;  // default
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
         case ' ': case 'G': case 'g':
            system = SatelliteSystem::GPS;
            break;
         case 'R': case 'r':
            system = SatelliteSystem::Glonass;
            break;
         case 'E': case 'e':
            system = SatelliteSystem::Galileo;
            break;
         case 'L': case 'l':
            system = SatelliteSystem::LEO;
            break;
         case 'C': case 'c':
            system = SatelliteSystem::BeiDou;
            break;
         case 'J': case 'j':
            system = SatelliteSystem::QZSS;
            break;
         case 'M': case 'm':
            system = SatelliteSystem::Mixed;
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
            case SatelliteSystem::QZSS:
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
         case SatelliteSystem::QZSS:
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
         case SatelliteSystem::GPS:
         case SatelliteSystem::Glonass:
         case SatelliteSystem::Galileo:
         case SatelliteSystem::LEO: 
         case SatelliteSystem::BeiDou:
         case SatelliteSystem::QZSS: 
         case SatelliteSystem::Mixed: break;
               // invalidate anything non-SP3
         default:
            system = SatelliteSystem::Unknown;
            id = -1;
      }
   }
}
