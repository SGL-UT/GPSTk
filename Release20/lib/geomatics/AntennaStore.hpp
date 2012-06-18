#pragma ident "$Id: AntennaStore.hpp 4 2008-12-09 18:18:41Z BrianTolman $"

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
// This software developed by Applied Research Laboratories at the University
// of Texas at Austin, under contract to an agency or agencies within the U.S. 
// Department of Defense. The U.S. Government retains all rights to use,
// duplicate, distribute, disclose, or release this software. 
//
// Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file AntennaStore.hpp
 * Store antenna phase center offset information, in AntexData objects, with
 * receiver/satellite name.
 * Access using name (receivers), or name and time (satellites); compute compute PCOs
 * at any (elevation, azimuth).
 */
 
#ifndef GPSTK_ANTENNA_STORE_INCLUDE
#define GPSTK_ANTENNA_STORE_INCLUDE

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "AntexHeader.hpp"
#include "AntexData.hpp"
#include "AntexStream.hpp"
#include "DayTime.hpp"

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
      void addAntenna(std::string name, AntexData& antdata) throw(Exception)
      {
         if(!antdata.isValid()) {
            Exception e("Invalid AntexData object");
            GPSTK_THROW(e);
         }

         // is the name already in the store?
         std::map<std::string, AntexData>::iterator it;
         it = antennaMap.find(name);
         if(it != antennaMap.end())       // erase it
            antennaMap.erase(it);

         // add the new data
         antennaMap[name] = antdata;
      }

      /// Get the antenna data for the given name from the store.
      /// @return true if successful, false if input name was not found in the store
      bool getAntenna(std::string name, AntexData& antdata) throw()
      {
         std::map<std::string, AntexData>::iterator it;
         it = antennaMap.find(name);
         if(it != antennaMap.end()) {
            antdata = it->second;
            return true;
         }
         return false;
      }

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
                               bool inputPRN=true) throw()
      {
         std::map<std::string, AntexData>::iterator it;
         for(it = antennaMap.begin(); it != antennaMap.end(); it++) {
            if(it->second.isRxAntenna) continue;
            if(it->second.systemChar != sys) continue;
            if(inputPRN && it->second.PRN == n) {
               name = it->first;
               data = it->second;
               return true;
            }
            if(!inputPRN && it->second.SVN == n) {
               name = it->first;
               data = it->second;
               return true;
            }
         }
         return false;
      }

      /// Get a vector of all antenna names in the store
      void getNames(std::vector<std::string>& names) throw()
      {
         names.clear();
         std::map<std::string, AntexData>::iterator it;
         for(it = antennaMap.begin(); it != antennaMap.end(); it++)
            names.push_back(it->first);
      }

      /// Get a vector of all receiver antenna names in the store
      void getReceiverNames(std::vector<std::string>& names) throw()
      {
         names.clear();
         std::map<std::string, AntexData>::iterator it;
         for(it = antennaMap.begin(); it != antennaMap.end(); it++) {
            if(it->second.isRxAntenna)
               names.push_back(it->first);
         }
      }

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
      void includeReceivers(std::vector<std::string>& names) throw()
      {
         namesToInclude = names;
         if(antennaMap.size() == 0) return;

         int j;
         std::map<std::string, AntexData>::iterator it;
         std::vector<std::string> rejects;
         for(it = antennaMap.begin(); it != antennaMap.end(); it++) {
            if(it->second.isRxAntenna) {
               bool ok=false;
               for(j=0; j<namesToInclude.size(); j++) {
                  if(it->first == namesToInclude[j]) {
                     ok = true;
                     break;
                  }
               }
               if(!ok) rejects.push_back(it->first);
            }
         }
         for(j=0; j<rejects.size(); j++)
            antennaMap.erase(rejects[j]);
      }

      /// Open and read an ANTEX format file with the given name, and read it.
      /// Add to the store all the receivers with names in the given std::vector,
      /// if it has been provided in a previous call to includeReceivers(),
      /// otherwise include all receiver antennas found.
      /// NB. call includeSats() or includeGPSsats() to include satellite antennas,
      /// before calling this routine.
      /// @param filename the name of the ANTEX file to read.
      /// @param time     the time (any) of interest, used to chose valid satellites
      /// @return the number of antennas added.
      /// @throw any exception caught during reading the file.
      int addANTEXfile(std::string filename,
                       DayTime time = DayTime::BEGINNING_OF_TIME)
         throw(Exception)
      {
         try {
            int i,n=0;
            AntexHeader anthdr;
            AntexData antdata;
            AntexStream antstrm, asout;
            DayTime time1,time2;

            // test for validity within a few days of time
            time1 = time2 = time;
            if(time > DayTime::BEGINNING_OF_TIME) {
               time1 += double(2 * 86400);
               time2 -= double(2 * 86400);
            }
   
            // open the input file
            antstrm.open(filename.c_str(),std::ios::in);
            if(!antstrm) {
               Exception e("Could not open file " + filename);
               GPSTK_THROW(e);
            }
            antstrm.exceptions(std::fstream::failbit);

            // read the header
            try {
               antstrm >> anthdr;
               if(!anthdr.isValid()) {
                  Exception e("Header is not valid");
                  GPSTK_THROW(e);
               }
            }
            catch(Exception& e) { GPSTK_RETHROW(e); }
            catch(std::exception& e) {
               Exception ge(std::string("Std exception: ") + e.what());
               GPSTK_THROW(ge);
            }

            // read the data
            while(1) {
               try { antstrm >> antdata; }
               catch(Exception& e) { GPSTK_RETHROW(e); }

               // ignore invalid data
               if(!antdata.isValid()) {
                  if(!antstrm) break;
                  continue;
               }
         
               // name it
               std::string name = antdata.name();

               // find receiver antenna name
               if(antdata.isRxAntenna && namesToInclude.size()) {
                  for(i=0; i<namesToInclude.size(); i++) {
                     if(name == namesToInclude[i]) {
                        addAntenna(name, antdata);
                        n++;
                        break;
                     }
                  }
               }
               // else include it
               else if(antdata.isRxAntenna || includeSats > 1
                     || (includeSats == 1 && antdata.systemChar == 'G')) {
                  if(antdata.isValid(time1) || antdata.isValid(time2)) {
                     addAntenna(name, antdata);
                     n++;
                  }
               }

               // break on EOF
               if(!antstrm) break;
            }

            return n;
         }
         catch(Exception& e) { GPSTK_RETHROW(e); }
      }

      /// dump the store
      void dump(std::ostream& s = std::cout, short detail = 0)
      {
         s << "Dump (" << (detail==0 ? "low" : (detail==1 ? "medium":"high"))
            << " detail) of AntennaStore (" << antennaMap.size() << ") antennas\n";
         std::map<std::string, AntexData>::iterator it;
         for(it = antennaMap.begin(); it != antennaMap.end(); it++) {
            if(detail > 0) s << std::endl;
            it->second.dump(s,detail);
         }
         s << "End of dump of AntennaStore\n";
      }

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
