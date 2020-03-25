#include "SatMetaData.hpp"
#include "TimeString.hpp"

namespace gpstk
{
   SatMetaData ::
   SatMetaData()
         : prn(0),
           svn(0),
           norad(0),
           chl(0),
           slotID(0),
           sys(SatID::systemUnknown),
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
        << smd.sys << ","
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
