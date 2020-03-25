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
         if (vals.size() == 0)
         {
               // this could still happen if there are lines with no commas
            continue;
         }
         if (vals.size() != 25)
         {
            // std::cerr << "size is " << vals.size() << std::endl;
            return false;
         }
         for (unsigned i = 0; i < vals.size(); i++)
         {
            gpstk::StringUtils::strip(vals[i]);
            // std::cerr << "vals[" << i << "] = " << vals[i] << std::endl;
            SatMetaData sat;
            if (StringUtils::isDigitString(vals[0]))
            {
               sat.prn = StringUtils::asUnsigned(vals[0]);
            }
            if (StringUtils::isDigitString(vals[1]))
            {
               sat.svn = StringUtils::asUnsigned(vals[1]);
            }
            if (StringUtils::isDigitString(vals[2]))
            {
               sat.norad = StringUtils::asInt(vals[2]);
            }
            if (StringUtils::isDigitString(vals[3]))
            {
               sat.chl = StringUtils::asInt(vals[3]);
            }
            if (StringUtils::isDigitString(vals[4]))
            {
               sat.slotID = StringUtils::asUnsigned(vals[4]);
            }
            sat.sys = SatID::convertStringToSatelliteSystem(vals[5]);
            unsigned long y,doy;
            double sod;
               // Set all time systems to any for now, the dozen or so
               // seconds offset between time systems really isn't
               // likely to amount to anything in this context.
            y = StringUtils::asUnsigned(vals[6]);
            doy = StringUtils::asUnsigned(vals[7]);
            sod = StringUtils::asDouble(vals[8]);
            sat.launchTime = YDSTime(y,doy,sod,gpstk::TimeSystem::Any);
            y = StringUtils::asUnsigned(vals[9]);
            doy = StringUtils::asUnsigned(vals[10]);
            sod = StringUtils::asDouble(vals[11]);
            sat.startTime = YDSTime(y,doy,sod,gpstk::TimeSystem::Any);
            y = StringUtils::asUnsigned(vals[12]);
            doy = StringUtils::asUnsigned(vals[13]);
            sod = StringUtils::asDouble(vals[14]);
            sat.endTime = YDSTime(y,doy,sod,gpstk::TimeSystem::Any);
            sat.plane = vals[15];
            sat.slot = vals[16];
            sat.type = vals[17];
            sat.mission = vals[18];
            sat.status = SatMetaData::asStatus(vals[19]);
            for (unsigned i = 0; i < SatMetaData::NUMCLOCKS; i++)
            {
               sat.clocks[i] = SatMetaData::asClockType(vals[20+i]);
            }
            sat.activeClock = StringUtils::asUnsigned(
               vals[20+SatMetaData::NUMCLOCKS]);
            satMap[sat.sys].insert(sat);
         }
      }
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
         if (when <= rv->endTime)
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
   findSatBySVN(SatID::SatelliteSystem sys, uint32_t svn,
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
             (when <= rv->endTime))
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
          uint32_t& svn)
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
   getPRN(SatID::SatelliteSystem sys, uint32_t svn,
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
