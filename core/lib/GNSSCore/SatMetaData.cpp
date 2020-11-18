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

#include "SatMetaData.hpp"
#include "TimeString.hpp"

namespace gpstk
{
   SatMetaData ::
   SatMetaData()
         : prn(0),
           svn(""),
           norad(0),
           chl(0),
           slotID(0),
           sys(SatelliteSystem::Unknown),
           launchTime(gpstk::CommonTime::END_OF_TIME),
           startTime(gpstk::CommonTime::END_OF_TIME),
           endTime(gpstk::CommonTime::END_OF_TIME),
           plane(""),
           slot(""),
           type(""),
           mission(""),
           status(Status::Unknown),
           activeClock(0)
   {
      for (unsigned i = 0; i < NUMCLOCKS; i++)
      {
         clocks[i] = ClockType::Unknown;
      }
   }


   std::string SatMetaData ::
   asString(SatMetaData::Status s)
   {
      switch (s)
      {
         case Status::Unknown:
            return "Unknown";
         case Status::Operational:
            return "Operational";
         case Status::Decommissioned:
            return "Decommissioned";
         case Status::Test:
            return "Test";
         default:
            return "???";
      }
   }


   SatMetaData::Status SatMetaData ::
   asStatus(const std::string& s)
   {
      if (s == "Operational")
         return Status::Operational;
      if (s == "Decommissioned")
         return Status::Decommissioned;
      if (s == "Test")
         return Status::Test;
      return Status::Unknown;
   }


   std::string SatMetaData ::
   asString(SatMetaData::ClockType s)
   {
      switch (s)
      {
         case ClockType::Unknown:
            return "Unknown";
         case ClockType::Crystal:
            return "Crystal";
         case ClockType::Cesium:
            return "Cesium";
         case ClockType::Rubidium:
            return "Rubidium";
         case ClockType::Hydrogen:
            return "Hydrogen";
         case ClockType::USNO:
            return "USNO";
         default:
            return "???";
      }
   }


   SatMetaData::ClockType SatMetaData ::
   asClockType(const std::string& s)
   {
      if (s == "Crystal")
         return ClockType::Crystal;
      if (s == "Cesium")
         return ClockType::Cesium;
      if (s == "Rubidium")
         return ClockType::Rubidium;
      if (s == "Hydrogen")
         return ClockType::Hydrogen;
      if (s == "USNO")
         return ClockType::USNO;
      return ClockType::Unknown;
   }


   std::ostream& operator<<(std::ostream& s, const SatMetaData& smd)
   {
      s << smd.prn << ","
        << smd.svn << ","
        << smd.norad << ","
        << smd.chl << ","
        << smd.slotID << ","
        << static_cast<int>(smd.sys) << ","
        << gpstk::printTime(smd.launchTime, "%Y,%j,%s,")
        << gpstk::printTime(smd.startTime, "%Y,%j,%s,")
        << gpstk::printTime(smd.endTime, "%Y,%j,%s,")
        << smd.plane << ","
        << smd.slot << ","
        << smd.type << ","
        << smd.mission << ","
        << SatMetaData::asString(smd.status) << ","
        << (unsigned)smd.activeClock;
      for (unsigned i = 0; i < SatMetaData::NUMCLOCKS; i++)
         s << "," << SatMetaData::asString(smd.clocks[i]);
      s << std::endl;
      return s;
   }
}
