#include <fstream>
#include "SatMetaDataStore.hpp"
#include "StringUtils.hpp"
#include "YDSTime.hpp"

using namespace std;

namespace gpstk
{
   bool SatMetaDataStore ::
   loadData(const std::string& sourceName)
   {
      bool rv = true;
      std::ifstream ins(sourceName);
      if (!ins)
      {
         // std::cerr << "Couldn't open " << sourceName << std::endl;
         return false;
      }
      while (ins)
      {
         std::string txt;
         std::getline(ins, txt);
            // skip comments and blank lines
         if ((txt[0] == '#') || txt.empty())
            continue;
         std::vector<std::string> vals = StringUtils::split(txt, ',');
         for (unsigned i = 0; i < vals.size(); i++)
         {
            gpstk::StringUtils::strip(vals[i]);
         }
         if (vals.size() == 0)
         {
               // this could still happen if there are lines with no commas
            continue;
         }
         if (gpstk::StringUtils::upperCase(vals[0]) == "SAT")
         {
            if (!addSat(vals))
               return false;
         }
         else if (gpstk::StringUtils::upperCase(vals[0]) == "SIG")
         {
            if (!addSignal(vals))
               return false;
         }
         else
         {
            cerr << "Invalid record type: " << vals[0] << endl;
            rv = false;
            continue;
         }
      }
      return rv;
   }


   bool SatMetaDataStore ::
   addSat(const std::vector<std::string>& vals)
   {
      if (vals.size() != 27)
      {
         return false;
      }
      SatMetaData sat;
         // simple way to index the columns without having to change
         // all the numbers with every little change.
      unsigned i = 1;
      sat.sys = SatID::convertStringToSatelliteSystem(vals[i++]);
      sat.svn = vals[i++];
      if (StringUtils::isDigitString(vals[i]))
      {
         sat.prn = StringUtils::asUnsigned(vals[i]);
      }
      i++;
      if (StringUtils::isDigitString(vals[i]))
      {
         sat.norad = StringUtils::asInt(vals[i]);
      }
      i++;
      if (StringUtils::isDigitString(vals[i]))
      {
         sat.chl = StringUtils::asInt(vals[i]);
      }
      i++;
      if (StringUtils::isDigitString(vals[i]))
      {
         sat.slotID = StringUtils::asUnsigned(vals[i]);
      }
      i++;
      unsigned long y,doy;
      double sod;
         // Set all time systems to any for now, the dozen or so
         // seconds offset between time systems really isn't
         // likely to amount to anything in this context.
      y = StringUtils::asUnsigned(vals[i++]);
      doy = StringUtils::asUnsigned(vals[i++]);
      sod = StringUtils::asDouble(vals[i++]);
      sat.launchTime = YDSTime(y,doy,sod,gpstk::TimeSystem::Any);
      y = StringUtils::asUnsigned(vals[i++]);
      doy = StringUtils::asUnsigned(vals[i++]);
      sod = StringUtils::asDouble(vals[i++]);
      sat.startTime = YDSTime(y,doy,sod,gpstk::TimeSystem::Any);
      y = StringUtils::asUnsigned(vals[i++]);
      doy = StringUtils::asUnsigned(vals[i++]);
      sod = StringUtils::asDouble(vals[i++]);
      sat.endTime = YDSTime(y,doy,sod,gpstk::TimeSystem::Any);
      sat.plane = vals[i++];
      sat.slot = vals[i++];
      sat.type = vals[i++];
      sat.signals = vals[i++];
      sat.mission = vals[i++];
      sat.status = SatMetaData::asStatus(vals[i++]);
      for (unsigned j = 0; j < SatMetaData::NUMCLOCKS; j++)
      {
         sat.clocks[j] = SatMetaData::asClockType(vals[i++]);
      }
      sat.activeClock = StringUtils::asUnsigned(vals[i]);
      satMap[sat.sys].insert(sat);
      return true;
   }


   bool SatMetaDataStore ::
   addSignal(const std::vector<std::string>& vals)
   {
      if (vals.size() != 5)
      {
         return false;
      }
      Signal sig;
         // simple way to index the columns without having to change
         // all the numbers with every little change.
      unsigned i = 1;
      std::string name = vals[i++];
      std::string carrier = vals[i++];
      std::string code = vals[i++];
      std::string nav = vals[i++];
         /** @todo implement the rest of this when we have some
          * from/to string translation methods for the enumerations
          * used in Signal. */
      return true;
   }


   bool SatMetaDataStore ::
   findSat(SatID::SatelliteSystem sys, uint32_t prn,
           const gpstk::CommonTime& when,
           SatMetaData& sat)
      const
   {
      SatMetaMap::const_iterator sysIt = satMap.find(sys);
      if (sysIt == satMap.end())
      {
         // std::cerr << "no system" << std::endl;
         return false;
      }
         // Unfortunately we have to do a linear search because
         // different systems have different methods of
         // identification.
      for (SatSet::const_iterator rv = sysIt->second.begin();
           rv != sysIt->second.end();
           rv++)
      {
         if (rv->prn < prn)
         {
            // cerr << "< prn" << endl;
            continue;
         }
         if (rv->prn > prn)
         {
            // cerr << "> prn" << endl;
            return false;
         }
         // cerr << "= prn" << endl;
            // same prn at this point
         if (when < rv->startTime)
         {
            // cerr << "< startTime" << endl;
            continue;
         }
         if (when < rv->endTime)
         {
            // std::cerr << "found it" << std::endl;
            // cerr << *rv << endl;
            sat = *rv;
            return true;
         }
      } // for (SatSet::const_iterator rv = sysIt->second.begin();
      // cerr << "giving up" << endl;
      return false;
   } // findSat()


   bool SatMetaDataStore ::
   findSatBySVN(SatID::SatelliteSystem sys, const std::string& svn,
                const gpstk::CommonTime& when,
                SatMetaData& sat)
      const
   {
      SatMetaMap::const_iterator sysIt = satMap.find(sys);
      if (sysIt == satMap.end())
      {
         // std::cerr << "no system" << std::endl;
         return false;
      }
         // This is a bit different than the PRN search because the
         // map is sorted by PRN and not SVN, so we have to search
         // until we either hit the end of the map or we find a match,
         // there's no short-cut failures.
      for (SatSet::const_iterator rv = sysIt->second.begin();
           rv != sysIt->second.end();
           rv++)
      {
         if ((rv->svn == svn) &&
             (when >= rv->startTime) &&
             (when < rv->endTime))
         {
            // std::cerr << "found it" << std::endl;
            // cerr << *rv << endl;
            sat = *rv;
            return true;
         }
      } // for (SatSet::const_iterator rv = sysIt->second.begin();
      // cerr << "giving up" << endl;
      return false;
   } // findSat()


   bool SatMetaDataStore ::
   getSVN(SatID::SatelliteSystem sys, uint32_t prn,
          const gpstk::CommonTime& when,
          std::string& svn)
      const
   {
      SatMetaData sat;
      if (findSat(sys, prn, when, sat))
      {
         svn = sat.svn;
         return true;
      }
      return false;
   }


   bool SatMetaDataStore ::
   getPRN(SatID::SatelliteSystem sys, const std::string& svn,
          const gpstk::CommonTime& when,
          uint32_t& prn)
      const
   {
      SatMetaData sat;
      if (findSatBySVN(sys, svn, when, sat))
      {
         prn = sat.prn;
         return true;
      }
      return false;
   }
}
