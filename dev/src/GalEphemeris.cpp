/// @file GalEphemeris.cpp Encapsulates the GPS legacy broadcast ephemeris and clock.
/// Inherits OrbitEph, which does most of the work; this class adds health and
/// accuracy information, fit interval, ionospheric correction terms and data
/// flags.

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

#include <string>
#include "Exception.hpp"
#include "SVNumXRef.hpp"
#include "GALWeekSecond.hpp"

#include "GalEphemeris.hpp"

using namespace std;

namespace gpstk
{
   // Returns true if the time, ct, is within the period of validity of
   // this OrbitEph object.
   // @throw Invalid Request if the required data has not been stored.
   bool GalEphemeris::isValid(const CommonTime& ct) const
   {
      try {
         if(ct >= beginValid && ct <= endValid) return true;
         return false;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   // This function returns the health status of the SV.
   bool GalEphemeris::isHealthy(void) const
   {
      try {
         OrbitEph::isHealthy();     // ignore the return value; for dataLoaded check

         // from RINEX 3.02 spec, A8:
         if(health & 0x5)           // E1b, E5b and both Tgds are valid
            return true;
         if(health & 0x2)           // E5a and Tgba are valid
            return true;

         // but there seems to be contradictory information:
         //if(health & 0x1)   // b1)         // E1b DVS is good
         //if(health & 0x6)   // b110)       // E1b HS is good
         //if(health & 0x8)   // b1000)      // E5a DVS is good
         //if(health & 0x30)  // b110000)    // E5a HS is good
         //if(health & 0x40)  // b1000000)   // E5b DVS is good
         //if(health & 0x180) // b110000000) // E5b HS is good

         return false;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   // adjustBeginningValidity determines the beginValid and endValid times.
   // @throw Invalid Request if the required data has not been stored.
   void GalEphemeris::adjustValidity(void)
   {
      try {
         OrbitEph::adjustValidity();   // for dataLoaded check
         beginValid = ctToe - fitDuration*1800.0;     // hours*3600/2
         endValid = ctToe + fitDuration*1800.0;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }
      
   // Dump the orbit, etc information to the given output stream.
   // @throw Invalid Request if the required data has not been stored.
   void GalEphemeris::dumpBody(std::ostream& os) const
   {
      try {
         OrbitEph::dumpBody(os);

         os << "           Galileo-SPECIFIC PARAMETERS\n"
            << scientific << setprecision(8)
            << "Tgd (E5a/E1) : " << setw(16) << Tgda << " meters" << endl
            << "Tgd (E5b/E1) : " << setw(16) << Tgdb << " meters" << endl
            << "HOW time     : " << setw(6) << HOWtime << " (sec of GAL week "
            << setw(4) << static_cast<GALWeekSecond>(ctToe).getWeek() << ")" << endl
            << "TransmitTime : " << OrbitEph::timeDisplay(transmitTime) << endl
            << "IODNav: " << IODnav
            << fixed << setprecision(2) << setfill('0')
            << "  Accuracy : " << getAccuracy() << " meters"
            << "  fitDuration: " << setw(2) << fitDuration << " hours" << endl
            << "Healthy?     : 0x" << hex << setw(2) << health << dec << " :"
            << " [E1b_D " << ((health & 0x1) ? "N":"Y") << "]"
            << " [E1b_H " << ((health & 0x2) ? "N":"Y") << "]"
            << " [E5a_D " << ((health & 0x4) ? "N":"Y") << "]"
            << " [E5a_H " << ((health & 0x18) ? "N":"Y") << "]"
            << " [E5b_D " << ((health & 0x20) ? "N":"Y") << "]"
            << " [E5b_H " << ((health & 0xC0) ? "N":"Y") << "]"
            << endl
            << "Datasources  : " << setw(3) << datasources << " :"
            << ((datasources & 0x1) ? " [I/NAV E1-B]" : "")
            << ((datasources & 0x2) ? " [F/NAV E5a-I]" : "")
            << ((datasources & 0x4) ? " [I/NAV E5b-I]" : "")
            << ((datasources & 0x8) ? " [bit 3 reserved]" : "")
            << ((datasources & 0x10) ? " [bit 4 reserved]" : "")
            << ((datasources & 0x100) ? " [set clk/Toc/acc for E5a,E1]" : "")
            << ((datasources & 0x200) ? " [set clk/Toc/acc for E5b,E1]" : "")
            << endl;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

   // Define this GalEphemeris by converting the given RINEX navigation data.
   // NB this both overrides and calls the OrbitEph version.
   // @param rnd Rinex3NavData
   // @return true if GalEphemeris was defined, false otherwise
   bool GalEphemeris::load(const Rinex3NavData& rnd)
   {
      try {
         if(rnd.satSys != "E") return false;    // OrbitEph::load will also do this...

         // load the OrbitEph parts
         if(!OrbitEph::load(rnd)) return false;

         // now load the Galileo-specific parts
         IODnav = rnd.IODnav;
         health = rnd.health;
         accuracy = rnd.accuracy;
         Tgda = rnd.Tgd;
         Tgdb = rnd.Tgd2;
         datasources = rnd.datasources;
         fitDuration = 4;

         HOWtime = rnd.HOWtime;
         int week = static_cast<GALWeekSecond>(ctToe).getWeek();
         transmitTime = GALWeekSecond(week, static_cast<double>(HOWtime),
            TimeSystem::GAL);

         return true;
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }
   }

} // end namespace
