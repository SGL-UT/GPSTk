//============================================================================
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

/// @file AntennaStore.hpp
/// Store antenna phase center offset information, in AntexData objects, with
/// receiver/satellite name.
/// Access using name (receivers), or name and time (satellites); compute compute PCOs
/// at any (elevation, azimuth).
 
#ifndef GPSTK_ANTENNA_STORE_INCLUDE
#define GPSTK_ANTENNA_STORE_INCLUDE

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "AntexHeader.hpp"
#include "AntexData.hpp"
#include "AntexStream.hpp"
#include "CommonTime.hpp"
#include "SatID.hpp"
#include "Triple.hpp"

namespace gpstk
{
   /// Store antenna phase center offset information, in AntexData objects, in a
   /// std::map with receiver/satellite name.
   ///
   /// An entire ANTEX format file may be added to the store with a call to
   /// the function addANTEXfile(); optional input arguments allow the caller to add
   /// only receiver antennas in a given list (std::vector) and only satellites
   /// that are valid at a given time. Calling the include/exclude functions (e.g.
   /// void includeAllSatellites()) before calling addANTEXfile() controls whether
   /// satellite antennas are excluded from the store.
   ///
   /// Names are formed by the AntexData member function name(); these names are
   /// strictly follow the IGS conventions found in the file rcvr_ant.tab and are
   /// equal to the string AntexData::type, except in the case of satellite names.
   /// Satellite names, which would be ambiguous if only AntexData::type were used,
   /// are defined to be the string AntexData::type + "/" + AntexData::serialNo.
   /// The AntexData serial number for a satellite is typically the system character
   /// followed by the PRN number, e.g. G17 or R24. Thus example names are:
   /// "ASH701945D_M    SCIS"
   /// "ASH701975.01A   NONE"
   /// "JPSREGANT_DD_E  NONE"
   /// "LEIAT504GG      NONE"
   /// "NAVAN2004T      NONE"
   /// "BLOCK IIR-A/G21"
   /// "BLOCK IIR-B/G02"
   /// "BLOCK IIR-M/G12"
   /// "BLOCK IIR-M/G31"
   /// "GLONASS-M/R15"
   /// Note there is no leading or trailing, but there may be embedded, whitespace.
   ///
   class AntennaStore
   {
   public:

      /// Empty constructor
      AntennaStore() : includeSats(0) {}

      /// Destructor
      ~AntennaStore() {}

      /// Add the given name, AntexData pair. If the name already exists in the store,
      /// replace the data for it with the input object.
      /// @throw if the AntexData is invalid.
      void addAntenna(std::string name, AntexData& antdata) throw(Exception);

      /// Get the antenna data for the given name from the store.
      /// @return true if successful, false if input name was not found in the store
      bool getAntenna(std::string name, AntexData& antdata) throw();

      /// Get the antenna data for the given satellite from the store.
      /// Satellites are identified by two things:
      /// system character: G or blank GPS, R GLONASS, E GALILEO, M MIXED
      /// and integer PRN or SVN number.
      /// NB. PRNs apply to GLONASS as well as GPS
      /// NB. Typically there is more than one antenna per satellite in ANTEX files;
      /// after calling include...Satellites(), when the store is loaded using
      /// addANTEXfile(), a time tag should be passed as input; this will load only
      /// the satellites valid at that time tag - most likely exactly one per sys/PRN.
      /// @param        char sys  System character for the satellite: G,R,E or M
      /// @param           int n  PRN (or SVN) of the satellite
      /// @param    string& name  Output antenna (ANTEX) name for the given satellite
      /// @param AntexData& data  Output antenna data for the given satellite
      /// @param   bool inputPRN  If false, parameter n is SVN not PRN (default true).
      /// @return true if successful, false if satellite was not found in the store
      bool getSatelliteAntenna(const char sys, const int n,
                               std::string& name, AntexData& data,
                               bool inputPRN=true) const throw();

      /// Get a vector of all antenna names in the store
      void getNames(std::vector<std::string>& names) throw();

      /// Get a vector of all receiver antenna names in the store
      void getReceiverNames(std::vector<std::string>& names) throw();

      /// get the number of antennas stored
      unsigned int size(void) const throw() { return antennaMap.size(); }

      /// clear the store of all information
      void clear(void) throw() { antennaMap.clear(); }

      /// call to have satellite antennas included in store
      /// NB. call before addAntenna() or addANTEXfile()
      void includeAllSatellites(void) { includeSats = 2; }

      /// call to have satellite antennas included in store
      /// NB. call before addAntenna() or addANTEXfile()
      void includeGPSSatellites(void) { includeSats = 1; }

      /// call to have satellite antennas excluded from store (the default).
      /// NB. call before addAntenna() or addANTEXfile()
      void excludeAllSatellites(void) { includeSats = 0; }

      /// call to give the store a list of receiver antenna names so that only
      /// those names will be included in the store (not applicable to satellites).
      /// If there are already other names in the store, they will be removed.
      /// NB. call before addAntenna() or addANTEXfile()
      void includeReceivers(std::vector<std::string>& names) throw();

      /// Open and read an ANTEX format file with the given name, and read it.
      /// Add to the store all the receivers with names in the given std::vector,
      /// if it has been provided in a previous call to includeReceivers(),
      /// otherwise include all receiver antennas found.
      /// NB. call includeSats() or includeGPSsats() to include satellite antennas,
      /// before calling this routine.
      /// @param filename the name of the ANTEX file to read.
      /// @param time     the time (any) of interest, used to choose valid satellites
      /// @return the number of antennas added.
      /// @throw any exception caught during reading the file.
      int addANTEXfile(std::string filename,
                       CommonTime time = CommonTime::BEGINNING_OF_TIME)
         throw(Exception);

      /// Compute the vector from the SV Center of Mass (COM) to
      /// the phase center of the antenna. 
      /// Satellites are identified by two things:
      /// system character: G or blank GPS, R GLONASS, E GALILEO, M MIXED, C BeiDou
      /// and integer PRN or SVN number.
      /// NB. PRNs apply to GLONASS as well as GPS
      /// NB. Typically there is more than one antenna per satellite in ANTEX files;
      /// after calling include...Satellites(), when the store is loaded using
      /// addANTEXfile(), a time tag should be passed as input; this will load only
      /// the satellites valid at that time tag - most likely exactly one per sys/PRN.
      /// @param        char sys  System character for the satellite: G,R,E or M
      /// @param           int n  PRN (or SVN) of the satellite
      /// @param   bool inputPRN  If false, parameter n is SVN not PRN (default true).
      /// @return vector (m) from COM to PC
      /// @throw InvalidRequest if no data available
      Triple ComToPcVector(const char sys, 
                           const int n,
                           const CommonTime& ct,
                           const Triple& satVector, 
                           bool inputPRN=true) const
         throw(Exception);

      /// Same as above except with different calling sequence for convenience
      Triple ComToPcVector(const SatID& sidr, 
                           const CommonTime& ct,
                           const Triple& satVector) const
         throw(Exception);

      /// dump the store
      void dump(std::ostream& s = std::cout, short detail = 0);

   private:
      /// List of receiver names to include in store
      std::vector<std::string> namesToInclude;

      /// flags determining which types of satellite antennas will be added
      /// 0 = no sats; 1 = GPS sats only; >1 = all sats
      int includeSats;

      /// map from name of antenna to AntexData object
      std::map<std::string, AntexData> antennaMap;
      
   }; // end class AntennaStore
   
} // namespace gpstk
#endif  // GPSTK_ANTENNA_STORE_INCLUDE
