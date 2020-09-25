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

/// @file GalEphemeris.cpp Encapsulates the GPS legacy broadcast ephemeris and clock.
/// Inherits OrbitEph, which does most of the work; this class adds health and
/// accuracy information, fit interval, ionospheric correction terms and data
/// flags.

#include <string>
#include "Exception.hpp"
#include "TimeString.hpp"
#include "CivilTime.hpp"
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
      if (health==Xvt::Healthy)
         return true;
      return false;
   }

   // adjustBeginningValidity determines the beginValid and endValid times.
   // @throw Invalid Request if the required data has not been stored.
   // 
   // NOTE: The Galileo ICD does not make the same sort of promises about 
   // the relationship of t-sub-oe and beginning time of transmission as
   // GPS.  Therefore, we should NOT adjust the beginValid time to be
   // anything other than the earliest tranmit time we recorded.
   //
   // The end of validity presents a problem also.  There is no guidance in
   // the ICD.  The only guidance in the SDD is "do not use beyond four hours
   // from initial time of transmission".   We don't KNOW that the transmission
   // time we have in hand is the earliest transmission.  Typically, the toe is 
   // about ten minutes prior to the first time of transmission.   Therefore, 
   // we will go with that. 
   //
   void GalEphemeris::adjustValidity(void)
   {
      try {
         OrbitEph::adjustValidity();   // for dataLoaded check
         beginValid = transmitTime;     
         endValid = ctToe + (3600*4);  // Four hours
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
            << "IODNav: " << IODnav << fixed << setprecision(2)
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

   // Dump the overhead information as a string containing a single line.
   // @throw Invalid Request if the required data has not been stored.
   string GalEphemeris::asString(void) const
   {
      if(!dataLoadedFlag)
         GPSTK_THROW(InvalidRequest("Data not loaded"));
      try {
         ostringstream os;
         CivilTime ct;
         os << "EPH E" << setfill('0') << setw(2) << satID.id << setfill(' ');
         ct = CivilTime(beginValid);
         os << printTime(ct," | %4Y %3j %02H:%02M:%02S |");
         ct = CivilTime(ctToe);
         os << printTime(ct," %3j %02H:%02M:%02S |");
         ct = CivilTime(ctToc);
         os << printTime(ct," %3j %02H:%02M:%02S |");
         ct = CivilTime(endValid);
         os << printTime(ct," %3j %02H:%02M:%02S |");
         ct = CivilTime(transmitTime);
         os << printTime(ct," %3j %02H:%02M:%02S | ");
         os << setw(3) << IODnav;
            //<< " | " << fixed << setprecision(2) << getAccuracy()
         os << " | 0x" << hex << setw(3) <<setfill('0')<< health <<dec<<setfill(' ');
         os << " | "
            << ((datasources & 0x5) ? "E15b" : "")       // 1 or 4
            << ((datasources & 0x2) ? "E5a " : "")
            //<< ((datasources & 0x4) ? "E5b " : "")
            << ((datasources & 0x100) ? " a1" : "")
            << ((datasources & 0x200) ? " b1" : "")
            << " |";
         return os.str();
      }
      catch(Exception& e) { GPSTK_RETHROW(e);
      }
   }

   Xvt::HealthStatus GalEphemeris::deriveHealth(const unsigned short SHS, 
                                  const unsigned short DVS, 
                                  const unsigned short SISA )
   {
      if (SHS==1 || SHS==3)
         return Xvt::Unhealthy;

      if (SHS==2)
         return Xvt::Degraded;

      if (SHS==0)
      {
         if (DVS==1)
            return Xvt::Degraded;

         if (DVS==0)
         {
            if (SISA==255)
               return Xvt::Degraded; 
            if (SISA<255)
               return Xvt::Healthy;
         }
      }
      return Xvt::Unknown;
   }

} // end namespace
