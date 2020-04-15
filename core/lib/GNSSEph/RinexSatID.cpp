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
   RinexSatID(int p, const SatelliteSystem& s)
      throw()
         : SatID(p, s)
   {
      switch(s)
      {
         case systemGPS:
         case systemGalileo:
         case systemGlonass:
         case systemGeosync:
         case systemTransit:
         case systemQZSS:
         case systemBeiDou:
         case systemIRNSS:
         case systemMixed:
            break;
               // Invalidate anything non-RINEX.
         default:
            system = systemUnknown;
            id = -1;
      }
   }


   char RinexSatID ::
   systemChar() const
      throw()
   {
      switch(system)
      {
         case systemGPS:     return 'G';
         case systemGalileo: return 'E';
         case systemGlonass: return 'R';
         case systemGeosync: return 'S';
         case systemTransit: return 'T';
         case systemQZSS:    return 'J';
         case systemBeiDou:  return 'C';
         case systemIRNSS:   return 'I';
         default:            return '?';
      }
   }


   std::string RinexSatID ::
   systemString() const
      throw()
   {
      switch(system)
      {
         case systemGPS:     return "GPS";
         case systemGalileo: return "Galileo";
         case systemGlonass: return "GLONASS";
         case systemGeosync: return "Geosync";
         case systemTransit: return "Transit";
         case systemQZSS:    return "QZSS";
         case systemBeiDou:  return "BeiDou";
         case systemIRNSS:   return "IRNSS";
         default:            return "Unknown";
      }
   }


   std::string RinexSatID ::
   systemString3() const
      throw()
   {
      switch(system)
      {
         case systemGPS:     return "GPS";
         case systemGalileo: return "GAL";
         case systemGlonass: return "GLO";
         case systemGeosync: return "GEO";
         case systemTransit: return "TRN";     // RINEX ver 2
         case systemQZSS:    return "QZS";
         case systemBeiDou:  return "BDS";
         case systemIRNSS:   return "IRN";      // RINEX ver 3.03
         default:            return "Unk";
      }
   }


   void RinexSatID ::
   fromString(const std::string& s)
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
         case 'M': case 'm':
            system = SatID::systemMixed;
            break;
         case ' ': case 'G': case 'g':
            system = SatID::systemGPS;
            break;
         case 'J': case 'j':
            system = SatID::systemQZSS;
            break;
         case 'I': case 'i':
            system = SatID::systemIRNSS;
            break;
         case 'C': case 'c':
            system = SatID::systemBeiDou;
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
            case SatID::systemGeosync:
               id += 100;
               break;
            case SatID::systemQZSS:
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
      switch (system)
      {
         case SatID::systemGeosync:
            rinexID -= 100;
            break;
         case SatID::systemQZSS:
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
      oss << systemChar() << std::setw(2) << rinexID;
      return oss.str();
   }
}
