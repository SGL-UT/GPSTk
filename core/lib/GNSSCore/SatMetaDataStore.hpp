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

#ifndef GPSTK_SATMETADATASTORE_HPP
#define GPSTK_SATMETADATASTORE_HPP

#include <map>
#include <set>
#include "SatMetaData.hpp"
#include "SatMetaDataSort.hpp"
#include "NavID.hpp"

namespace gpstk
{
      /** Provide a class for reading satellite metadata from a CSV
       * file and provide methods for looking up information in that
       * file.
       */
   class SatMetaDataStore
   {
   public:
         /// Specifies a single GNSS signal.
      struct Signal
      {
         CarrierBand carrier; ///< Carrier frequency.
         gpstk::TrackingCode code;   ///< Tracking code.
         gpstk::NavType nav;         ///< Navigation code.
      };
         /// Key of GNSS and satellite block
      class SystemBlock
      {
      public:
         inline bool operator<(const SystemBlock& right) const;
         SatelliteSystem sys;
         std::string blk;
      };
         /// Like SatID but for SVN which is a string
      class SVNID
      {
      public:
         SVNID();
         SVNID(SatelliteSystem sys, const std::string& svn);
         SatelliteSystem system;
         std::string id;
         bool operator<(const SVNID& right) const;
      };
         /// Launch configuration
      class LaunchConfig
      {
      public:
         SVNID svn;
         gpstk::CommonTime launchTime;
         std::string type;             ///< Typically block number.
         std::string mission;          ///< Mission number.
      };
         /// Set of signals that may be transmitted by a satellite.
      using SignalSet = std::set<Signal>;
         /// Map of signal set name to signal set.
      using SignalMap = std::map<std::string, SignalSet>;
         /// Set of satellites ordered by PRN or channel/slotID.
      using SatSet = std::multiset<SatMetaData, SatMetaDataSort>;
         /// Satellites grouped by system.
      using SatMetaMap = std::map<SatelliteSystem, SatSet>;
         /// Types of clocks on a satellite (hardware-specific positional idx).
      using ClockVec = std::vector<SatMetaData::ClockType>;
         /// Clock configuration information
      using ClockConfigMap = std::map<SystemBlock, ClockVec>;
         /// Map SVN to launch time.
      using LaunchMap = std::map<SVNID, LaunchConfig>;
         /// Map SVN to NORAD ID.
      using NORADMap = std::map<SVNID, unsigned long>;

         /// Nothin doin.
      SatMetaDataStore() = default;

         /** Attempt to load satellite metadata from the store.
          * The format of the input file is CSV, the values being
          *   \li SAT (literal)
          *   \li GNSS name
          *   \li svn
          *   \li prn
          *   \li FDMA channel (0 if n/a)
          *   \li FDMA slot ID (0 if n/a)
          *   \li start time year
          *   \li start time day of year
          *   \li start time seconds of day
          *   \li end time year
          *   \li end time day of year
          *   \li end time seconds of day
          *   \li orbital plane
          *   \li orbital slot
          *   \li signal set name
          *   \li satellite status
          *   \li active clock number
          *
          * Mapping system satellite number to NORAD identifier:
          *   \li NORAD (literal)
          *   \li GNSS name
          *   \li svn
          *   \li NORAD ID
          *
          * Satellite launch time:
          *   \li LAUNCH (literal)
          *   \li GNSS name
          *   \li svn
          *   \li launch time year
          *   \li launch time day of year
          *   \li launch time seconds of day
          *   \li satellite block/type
          *   \li mission number
          *
          * Clock configuration:
          *   \li CLOCK (literal)
          *   \li GNSS name
          *   \li satellite type/block
          *   \li clock type 1
          *   \li clock type 2
          *   \li clock type 3
          *   \li clock type 4
          *
          * Signal sets are defined using multiple SIG records as follows
          *   \li SIG (literal)
          *   \li signal set name
          *   \li carrier band name
          *   \li tracking code name
          *   \li navigation code name
          *
          * @param[in] sourceName The path to the input CSV-format file.
          * @return true if successful, false on error.
          */
      virtual bool loadData(const std::string& sourceName);

         /** Find a satellite in the map by searching by PRN.
          * @param[in] sys The GNSS of the desired satellite.
          * @param[in] prn The pseudo-random number identifying the
          *   desired satellite.
          * @param[in] when The time of interest of the desired satellite.
          * @param[out] sat If found the satellite's metadata.
          * @return true if the requested satellite mapping was found.
          */
      bool findSat(SatelliteSystem sys, uint32_t prn,
                   const gpstk::CommonTime& when,
                   SatMetaData& sat)
         const;

         /** Find a satellite in the map by searching by PRN.
          * @param[in] prn The satellite to find, identified by PRN
          *   (i.e. not FDMA channel/slot).
          * @param[in] when The time of interest of the desired satellite.
          * @param[out] sat If found the satellite's metadata.
          * @return true if the requested satellite mapping was found.
          */
      bool findSat(const SatID& prn,
                   const gpstk::CommonTime& when,
                   SatMetaData& sat)
         const
      { return findSat(prn.system, prn.id, when, sat); }

         /** Get the space vehicle number of a satellite in the map by
          * searching by PRN.
          * @param[in] sys The GNSS of the desired satellite.
          * @param[in] prn The pseudo-random number identifying the
          *   desired satellite.
          * @param[in] when The time of interest of the desired satellite.
          * @param[out] svn If found the satellite's vehicle number.
          * @return true if the requested satellite mapping was found.
          */
      bool getSVN(SatelliteSystem sys, uint32_t prn,
                  const gpstk::CommonTime& when,
                  std::string& svn)
         const;

         /** Get the space vehicle number of a satellite in the map by
          * searching by PRN.
          * @param[in] sat The ID of the desired satellite.
          * @param[in] when The time of interest of the desired satellite.
          * @param[out] svn If found the satellite's vehicle number.
          * @return true if the requested satellite mapping was found.
          */
      bool getSVN(const SatID& sat, const gpstk::CommonTime& when,
                  std::string& svn)
         const
      { return getSVN(sat.system, sat.id, when, svn); }

         /** Find a satellite in the map by searching by SVN.
          * @param[in] sys The GNSS of the desired satellite.
          * @param[in] svn The system-unique space vehicle number
          *   identifying the desired satellite.
          * @param[in] when The time of interest of the desired satellite.
          * @param[out] sat If found the satellite's metadata.
          * @return true if the requested satellite mapping was found.
          */
      bool findSatBySVN(SatelliteSystem sys, const std::string& svn,
                        const gpstk::CommonTime& when,
                        SatMetaData& sat)
         const;

         /** Find a GLONASS satellite in the map by searching by
          *  its orbit slotID and FDMA channel.  To be a unique
          *  identification, both are necessary.
          *  This is only applicable to GLONASS FDMA SVs
          * @param[in] slotID The GLONASS orbit slot ID 
          *   identifying the desired satellite.
          * @param[in] channel The FDMA channel
          *   identifying the desired satellite.
          * @param[in] when The time of interest of the desired satellite.
          * @param[out] sat If found the satellite's metadata.
          * @return true if the requested satellite mapping was found.
          */
      bool findSatBySlotFdma(uint32_t slotID,
                              int32_t channel,
                        const gpstk::CommonTime& when,
                        SatMetaData& sat)
         const;

         /** Get the pseudo-random number of a satellite in the map by
          * searching by SVN.
          * @param[in] sys The GNSS of the desired satellite.
          * @param[in] svn The space vehicle number identifying the
          *   desired satellite.
          * @param[in] when The time of interest of the desired satellite.
          * @param[out] prn If found the satellite's pseudo-random number.
          * @return true if the requested satellite mapping was found.
          */
      bool getPRN(SatelliteSystem sys, const std::string& svn,
                  const gpstk::CommonTime& when,
                  uint32_t& prn)
         const;

         /// Storage of all the satellite metadata.
      SatMetaMap satMap;
         /// Map signal set name to the actual signals.
      SignalMap sigMap;
         /// Map satellite block to clock types.
      ClockConfigMap clkMap;
         /// Launch time of satellites.
      LaunchMap launchMap;
         /// Map SVN to NORAD ID.
      NORADMap noradMap;

   protected:
         /** Convert a SAT record to a SatMetaData record and store it.
          * @param[in] vals SAT record in the form of an array of columns.
          * @param[in] lineNo The line number of the input file being processed.
          * @return true if successful, false on error.
          */
      bool addSat(const std::vector<std::string>& vals, unsigned long lineNo);
         /** Convert a SIG record to a Signal object and store it.
          * @param[in] vals SIG record in the form of an array of columns.
          * @param[in] lineNo The line number of the input file being processed.
          * @return true if successful, false on error.
          */
      bool addSignal(const std::vector<std::string>& vals,
                     unsigned long lineNo);
         /** Add a CLOCK record to clkMap.
          * @param[in] vals CLOCK record in the form of an array of columns.
          * @param[in] lineNo The line number of the input file being processed.
          * @return true if successful, false on error.
          */
      bool addClock(const std::vector<std::string>& vals, unsigned long lineNo);
         /** Add a LAUNCH record to launchMap.
          * @param[in] vals LAUNCH record in the form of an array of columns.
          * @param[in] lineNo The line number of the input file being processed.
          * @return true if successful, false on error.
          */
      bool addLaunch(const std::vector<std::string>& vals,
                     unsigned long lineNo);
         /** Add a NORAD record to noradMap.
          * @param[in] vals NORAD record in the form of an array of columns.
          * @param[in] lineNo The line number of the input file being processed.
          * @return true if successful, false on error.
          */
      bool addNORAD(const std::vector<std::string>& vals, unsigned long lineNo);
   }; // class SatMetaDataStore


   bool SatMetaDataStore::SystemBlock ::
   operator<(const SatMetaDataStore::SystemBlock& right)
      const
   {
      if (static_cast<int>(sys) < static_cast<int>(right.sys))
         return true;
      if (static_cast<int>(sys) > static_cast<int>(right.sys))
         return false;
      return blk < right.blk;
   }


   inline std::ostream& operator<<(std::ostream& s,
                                   const SatMetaDataStore::SystemBlock& sblk)
   {
      s << gpstk::StringUtils::asString(sblk.sys) << " " << sblk.blk;
      return s;
   }


   inline std::ostream& operator<<(std::ostream& s,
                                   const SatMetaDataStore::SVNID& svn)
   {
      s << gpstk::StringUtils::asString(svn.system) << " " << svn.id;
      return s;
   }

} // namespace gpstk

#endif // GPSTK_SATMETADATASTORE_HPP
