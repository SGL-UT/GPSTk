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

#include <fstream>
#include "SatMetaDataStore.hpp"
#include "StringUtils.hpp"
#include "YDSTime.hpp"

using namespace std;

namespace gpstk
{
   SatMetaDataStore::SVNID ::
   SVNID()
         : system(SatelliteSystem::Unknown)
   {
   }


   SatMetaDataStore::SVNID ::
   SVNID(SatelliteSystem sys, const std::string& svn)
         : system(sys), id(svn)
   {
   }


   bool SatMetaDataStore::SVNID ::
   operator<(const SVNID& right) const
   {
      if (static_cast<int>(system) < static_cast<int>(right.system))
      {
         return true;
      }
      if (static_cast<int>(system) > static_cast<int>(right.system))
      {
         return false;
      }
      return id < right.id;
   }


   bool SatMetaDataStore ::
   loadData(const std::string& sourceName)
   {
      bool rv = true;
      std::ifstream ins(sourceName);
      unsigned long lineNo = 0;
      if (!ins)
      {
            // std::cerr << "Couldn't open " << sourceName << std::endl;
         return false;
      }
      while (ins)
      {
         std::string txt;
         std::getline(ins, txt);
         lineNo++;
            // skip comments and blank lines
         if ((txt[0] == '#') || txt.empty())
            continue;
         std::vector<std::string> vals = StringUtils::split(txt, ',');
         try
         {
            for (unsigned i = 0; i < vals.size(); i++)
            {
               gpstk::StringUtils::strip(vals[i]);
            }
            if (vals.size() == 0)
            {
                  // this could still happen if there are lines with no commas
               continue;
            }
            string key = gpstk::StringUtils::upperCase(vals[0]);
            if (key == "SAT")
            {
               if (!addSat(vals, lineNo))
               {
                  rv = false;
                  continue;
               }
            }
            else if (key == "SIG")
            {
               if (!addSignal(vals, lineNo))
               {
                  rv = false;
                  continue;
               }
            }
            else if (key == "CLOCK")
            {
               if (!addClock(vals, lineNo))
               {
                  rv = false;
                  continue;
               }
            }
            else if (key == "LAUNCH")
            {
               if (!addLaunch(vals, lineNo))
               {
                  rv = false;
                  continue;
               }
            }
            else if (key == "NORAD")
            {
               if (!addNORAD(vals, lineNo))
               {
                  rv = false;
                  continue;
               }
            }
            else
            {
               cerr << "Invalid record type: " << vals[0] << " on line "
                    << lineNo << endl;
               rv = false;
               continue;
            }
         }
         catch (gpstk::Exception& exc)
         {
            cerr << "Exception while processing line " << lineNo << ":" << endl
                 << exc << endl;
            rv = false;
         }
         catch (std::exception& exc)
         {
            cerr << "Exception while processing line " << lineNo << ": "
                 << exc.what() << endl;
            rv = false;
         }
         catch (...)
         {
            cerr << "Unknown exception processing line " << lineNo << endl;
            rv = false;
         }
      }
      return rv;
   }


   bool SatMetaDataStore ::
   addSat(const std::vector<std::string>& vals, unsigned long lineNo)
   {
         // simple way to index the columns without having to change
         // all the numbers with every little change.
      unsigned i = 1;
      if (vals.size() != 17)
      {
         cerr << "Invalid SAT record on line " << lineNo << " size!=17" << endl;
         return false;
      }
      SatMetaData sat;
      sat.sys = convertStringToSatelliteSystem(vals[i++]);
      sat.svn = vals[i++];
      if (StringUtils::isDigitString(vals[i]))
      {
         sat.prn = StringUtils::asUnsigned(vals[i]);
      }
      else
      {
         cerr << "Invalid PRN on line " << lineNo << endl;
         return false;
      }
      i++;
      if (StringUtils::isDigitString(vals[i]))
      {
         sat.chl = StringUtils::asInt(vals[i]);
      }
      else
      {
         cerr << "Invalid FDMA channel on line " << lineNo << endl;
         return false;
      }
      i++;
      if (StringUtils::isDigitString(vals[i]))
      {
         sat.slotID = StringUtils::asUnsigned(vals[i]);
      }
      else
      {
         cerr << "Invalid FDMA slot on line " << lineNo << endl;
         return false;
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
      try
      {
         sat.startTime = YDSTime(y,doy,sod,gpstk::TimeSystem::Any);
      }
      catch (gpstk::Exception& exc)
      {
         exc.addText("Processing startTime");
         GPSTK_RETHROW(exc);
      }
      y = StringUtils::asUnsigned(vals[i++]);
      doy = StringUtils::asUnsigned(vals[i++]);
      sod = StringUtils::asDouble(vals[i++]);
      try
      {
         sat.endTime = YDSTime(y,doy,sod,gpstk::TimeSystem::Any);
      }
      catch (gpstk::Exception& exc)
      {
         exc.addText("Processing endTime");
         GPSTK_RETHROW(exc);
      }
      sat.plane = vals[i++];
      sat.slot = vals[i++];
      sat.signals = vals[i++];
      sat.status = SatMetaData::asStatus(vals[i++]);
      sat.activeClock = StringUtils::asUnsigned(vals[i]);
         // cross-reference check and fill
      SVNID svn(sat.sys, sat.svn);
      if (noradMap.find(svn) == noradMap.end())
      {
         cerr << "Missing NORAD mapping for SVN " << svn << " on line "
              << lineNo << endl;
         return false;
      }
      sat.norad = noradMap[svn];
      if (launchMap.find(svn) == launchMap.end())
      {
         cerr << "Missing LAUNCH record for SVN " << svn << " on line "
              << lineNo << endl;
         return false;
      }
      sat.launchTime = launchMap[svn].launchTime;
      sat.type = launchMap[svn].type;
      sat.mission = launchMap[svn].mission;
      SystemBlock sysBlock;
      sysBlock.sys = sat.sys;
      sysBlock.blk = launchMap[svn].type;
      if (clkMap.find(sysBlock) == clkMap.end())
      {
         cerr << "Missing CLOCK record for " << sysBlock << " on line "
              << lineNo << endl;
         return false;
      }
         // note: no checks for clock vector size!
      const ClockVec& cv(clkMap[sysBlock]);
      for (unsigned cn = 0; cn < SatMetaData::NUMCLOCKS; cn++)
      {
         sat.clocks[cn] = cv[cn];
      }
         // add the complete record
      satMap[sat.sys].insert(sat);
      return true;
   }


   bool SatMetaDataStore ::
   addSignal(const std::vector<std::string>& vals, unsigned long lineNo)
   {
         // simple way to index the columns without having to change
         // all the numbers with every little change.
      unsigned i = 1;
      if (vals.size() != 5)
      {
         cerr << "Invalid SIG record on line " << lineNo << " size!=5" << endl;
         return false;
      }
      Signal sig;
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
   addClock(const std::vector<std::string>& vals, unsigned long lineNo)
   {
         // simple way to index the columns without having to change
         // all the numbers with every little change.
      unsigned i = 1;
      if (vals.size() != 7)
      {
         cerr << "Invalid CLOCK record on line " << lineNo << " size!=7"
              << endl;
         return false;
      }
      SystemBlock key;
      key.sys = convertStringToSatelliteSystem(vals[i++]);
      key.blk = vals[i++];
      if (clkMap.find(key) != clkMap.end())
      {
            // enforce no duplicates
         cerr << "Duplicate CLOCK " << StringUtils::asString(key.sys) << " "
              << key.blk << " on line " << lineNo << endl;
         return false;
      }
         // currently support up to four clocks.
      clkMap[key].resize(SatMetaData::NUMCLOCKS);
      for (unsigned j = 0; j < SatMetaData::NUMCLOCKS; j++)
      {
         clkMap[key][j] = SatMetaData::asClockType(vals[i++]);
      }
      return true;
   }


   bool SatMetaDataStore ::
   addLaunch(const std::vector<std::string>& vals, unsigned long lineNo)
   {
         // simple way to index the columns without having to change
         // all the numbers with every little change.
      unsigned i = 1;
      if (vals.size() != 8)
      {
         cerr << "Invalid LAUNCH record on line " << lineNo << " size!=8"
              << endl;
         return false;
      }
      SVNID svn;
      svn.system = convertStringToSatelliteSystem(vals[i++]);
      svn.id = vals[i++];
      if (launchMap.find(svn) != launchMap.end())
      {
            // enforce no duplicates
         cerr << "Duplicate LAUNCH " << svn << " on line " << lineNo << endl;
         return false;
      }
      launchMap[svn].svn = svn;
      unsigned y = StringUtils::asUnsigned(vals[i++]);
      unsigned doy = StringUtils::asUnsigned(vals[i++]);
      double sod = StringUtils::asDouble(vals[i++]);
      try
      {
         launchMap[svn].launchTime = YDSTime(y,doy,sod,gpstk::TimeSystem::Any);
      }
      catch (gpstk::Exception& exc)
      {
         exc.addText("Processing launchTime");
         GPSTK_RETHROW(exc);
      }
      launchMap[svn].type = vals[i++];
      launchMap[svn].mission = vals[i++];
      return true;
   }


   bool SatMetaDataStore ::
   addNORAD(const std::vector<std::string>& vals, unsigned long lineNo)
   {
         // simple way to index the columns without having to change
         // all the numbers with every little change.
      unsigned i = 1;
      if (vals.size() != 4)
      {
         cerr << "Invalid NORAD record on line " << lineNo << " size!=4"
              << endl;
         return false;
      }
      SVNID svn;
      svn.system = convertStringToSatelliteSystem(vals[i++]);
      svn.id = vals[i++];
      if (noradMap.find(svn) != noradMap.end())
      {
            // enforce no duplicates
         cerr << "Duplicate NORAD " << svn << " on line " << lineNo << endl;
         return false;
      }
      unsigned long noradID = StringUtils::asUnsigned(vals[i++]);
      noradMap[svn] = noradID;
      return true;
   }


   bool SatMetaDataStore ::
   findSat(SatelliteSystem sys, uint32_t prn,
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
   findSatBySVN(SatelliteSystem sys, const std::string& svn,
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


   bool SatMetaDataStore::
   findSatBySlotFdma(uint32_t slotID,
                     int32_t channel,
                     const gpstk::CommonTime& when,
                     SatMetaData& sat)
         const
   {
      SatelliteSystem sys = SatelliteSystem::Glonass;
      SatMetaMap::const_iterator sysIt = satMap.find(sys);
      if (sysIt == satMap.end())
      {
         // std::cerr << "no system" << std::endl;
         return false;
      }
         // This is a bit different than the PRN search because the
         // map is sorted by PRN and not slotID, so we have to search
         // until we either hit the end of the map or we find a match,
         // there's no short-cut failures.
      for (SatSet::const_iterator rv = sysIt->second.begin();
           rv != sysIt->second.end();
           rv++)
      {
         if ((rv->slotID == slotID)  &&
             (rv->chl    == channel) &&
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
   } // findSatByFdmaSlot()


   bool SatMetaDataStore ::
   getSVN(SatelliteSystem sys, uint32_t prn,
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
   getPRN(SatelliteSystem sys, const std::string& svn,
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
