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

#include "RinexSatID.hpp"

/**
 * @file RinexSatID.cpp
 * gpstk::RinexSatID - navigation system-independent representation of
 * a satellite as defined by the RINEX specification.
 */

namespace gpstk
{
   char RinexSatID::fillchar = '0';

   RinexSatID ::
   RinexSatID(int p, SatelliteSystem s)
      throw()
         : SatID(p, s)
   {
      validate();
   }


   char RinexSatID ::
   systemChar() const
      throw()
   {
      switch(system)
      {
         case SatelliteSystem::GPS:     return 'G';
         case SatelliteSystem::Galileo: return 'E';
         case SatelliteSystem::Glonass: return 'R';
         case SatelliteSystem::Geosync: return 'S';
         case SatelliteSystem::Transit: return 'T';
         case SatelliteSystem::QZSS:    return 'J';
         case SatelliteSystem::BeiDou:  return 'C';
         case SatelliteSystem::IRNSS:   return 'I';
         default:                       return '?';
      }
   }


   std::string RinexSatID ::
   systemString() const
      throw()
   {
      switch(system)
      {
         case SatelliteSystem::GPS:     return "GPS";
         case SatelliteSystem::Galileo: return "Galileo";
         case SatelliteSystem::Glonass: return "GLONASS";
         case SatelliteSystem::Geosync: return "Geosync";
         case SatelliteSystem::Transit: return "Transit";
         case SatelliteSystem::QZSS:    return "QZSS";
         case SatelliteSystem::BeiDou:  return "BeiDou";
         case SatelliteSystem::IRNSS:   return "IRNSS";
         default:                       return "Unknown";
      }
   }


   std::string RinexSatID ::
   systemString3() const
      throw()
   {
      switch(system)
      {
         case SatelliteSystem::GPS:     return "GPS";
         case SatelliteSystem::Galileo: return "GAL";
         case SatelliteSystem::Glonass: return "GLO";
         case SatelliteSystem::Geosync: return "GEO";
         case SatelliteSystem::Transit: return "TRN";     // RINEX ver 2
         case SatelliteSystem::QZSS:    return "QZS";
         case SatelliteSystem::BeiDou:  return "BDS";
         case SatelliteSystem::IRNSS:   return "IRN";      // RINEX ver 3.03
         default:                       return "Unk";
      }
   }


   void RinexSatID ::
   fromString(const std::string& s)
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
         case 'M': case 'm':
            system = SatelliteSystem::Mixed;
            break;
         case ' ': case 'G': case 'g':
            system = SatelliteSystem::GPS;
            break;
         case 'J': case 'j':
            system = SatelliteSystem::QZSS;
            break;
         case 'I': case 'i':
            system = SatelliteSystem::IRNSS;
            break;
         case 'C': case 'c':
            system = SatelliteSystem::BeiDou;
            break;
         default:                   // non-RINEX system character
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
            // do the kludging that RINEX does for PRNs > 99
         switch (system)
         {
            case SatelliteSystem::Geosync:
               id += 100;
               break;
            case SatelliteSystem::QZSS:
               if (id < 83)
               {
                     // PRN codes in the range of 193-197
                  id += 192;
               }
               else
               {
                     // PRN codes in the range of 183-187
                  id += 100;
               }
               break;
         }
      }
   }


   std::string RinexSatID ::
   toString() const
      throw()
   {
      std::ostringstream oss;
      oss.fill(fillchar);
      int rinexID = id;
         // do the kludging that RINEX does for PRNs > 99
         // id of -1 is a special case we use to represent "none"
      if (id != -1)
      {
         switch (system)
         {
            case SatelliteSystem::Geosync:
               rinexID -= 100;
               break;
            case SatelliteSystem::QZSS:
               if (rinexID >= 193)
               {
                     // PRN codes in the range of 193-197
                  rinexID -= 192;
               }
               else
               {
                     // PRN codes in the range of 183-187
                  rinexID -= 100;
               }
               break;
         }
      }
      oss << systemChar() << std::setw(2) << rinexID;
      return oss.str();
   }


   void RinexSatID ::
   validate()
   {
      switch(system)
      {
         case SatelliteSystem::GPS:
         case SatelliteSystem::Galileo:
         case SatelliteSystem::Glonass:
         case SatelliteSystem::Geosync:
         case SatelliteSystem::Transit:
         case SatelliteSystem::QZSS:
         case SatelliteSystem::BeiDou:
         case SatelliteSystem::IRNSS:
         case SatelliteSystem::Mixed:
            break;
               // Invalidate anything non-RINEX.
         default:
            system = SatelliteSystem::Unknown;
            id = -1;
      }
   }
}
