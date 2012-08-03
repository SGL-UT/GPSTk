#pragma ident "$Id$"

/**
 * @file Rinex3EphemerisStore.hpp
 * Read and store RINEX formated navigation message (Rinex3Nav) data.
 */

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

#ifndef GPSTK_RINEX3EPHEMERISSTORE_HPP
#define GPSTK_RINEX3EPHEMERISSTORE_HPP

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <algorithm>

#include "Exception.hpp"
#include "CommonTime.hpp"
#include "SatID.hpp"
#include "Xvt.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"
#include "TimeSystemCorr.hpp"

#include "FileStore.hpp"
#include "GPSEphemerisStore.hpp"
#include "GloEphemerisStore.hpp"
#include "GalEphemerisStore.hpp"
//#include "GeoEphemerisStore.hpp"
//#include "COMEphemerisStore.hpp"

namespace gpstk
{
   /** @addtogroup ephemstore */
   //@{

   class Rinex3EphemerisStore : public XvtStore<SatID>
   {
   // member data:
   private:

      /// FileStore for the Rinex3Nav input files
      FileStore<Rinex3NavHeader> NavFiles;

      /// Ephemeris store for GPS nav messages (stored as EngEphemeris)
      GPSEphemerisStore GPSstore;

      /// Ephemeris store for Glonass nav messages (stored as GloRecord)
      GloEphemerisStore GLOstore;

      /// Ephemeris store for Galileo nav messages (stored as GalRecord)
      GalEphemerisStore GALstore;

      /// Ephemeris store for Geosync nav messages (stored as GeoRecord)
      //GeoEphemerisStore GEOstore;

      /// Ephemeris store for COMPASS nav messages (stored as ComRecord)
      //ComEphemerisStore COMstore;

   public:

      /// Rinex file header last read by loadFile()
      Rinex3NavHeader Rhead;

      /// Rinex file data last read by loadFile()
      Rinex3NavData Rdata;

      /// Map of time system corrections, similar to mapTimeCorr in Rinex3NavHeader,
      /// and taken from either loadFile() (RinexNavHeader) or user input.
      /// key = TimeSystemCorrection::asString4().
      /// User may add to the list with addTimeCorr()
      std::map<std::string, TimeSystemCorrection> mapTimeCorr;

      /// string containing what() of exceptions caught by loadFile()
      std::string what;

   // member functions:

      Rinex3EphemerisStore() throw()
      {
         bool flag = GPSstore.getOnlyHealthyFlag();    // default is GPS default
         GLOstore.setCheckHealthFlag(flag);
         GALstore.setOnlyHealthyFlag(flag);
      }

      /// destructor
      virtual ~Rinex3EphemerisStore()
      { }
      
   // XvtStore interface:
      /// Returns the position, velocity, and clock offset of the indicated
      /// object in ECEF coordinates (meters) at the indicated time.
      /// @param[in] sat the satellite of interest
      /// @param[in] ttag the time to look up
      /// @return the Xvt of the object at the indicated time
      /// @throw InvalidRequest If the request can not be completed for any
      ///    reason, this is thrown. The text may have additional
      ///    information as to why the request failed.
      virtual Xvt getXvt(const SatID& sat, const CommonTime& ttag)
         const throw(InvalidRequest);

      /// Dump information about the store to an ostream.
      /// @param[in] os ostream to receive the output; defaults to std::cout
      /// @param[in] detail integer level of detail to provide; allowed values are
      ///    0: number of satellites, time step and time limits
      ///    1: above plus flags, gap and interval values, and number of data/sat
      ///    2: above plus all the data tables
      virtual void dump(std::ostream& os=std::cout, short detail=0)
         const throw()
      {
         // may have to re-write this...
         os << "Dump Rinex3EphemerisStore:" << std::endl;
         // dump the time system corrections
         std::map<std::string,TimeSystemCorrection>::const_iterator tcit;
         for(tcit=mapTimeCorr.begin(); tcit != mapTimeCorr.end(); ++tcit) {
            os << "Time correction for " << tcit->second.asString4() << " : "
               << tcit->second.asString() << " " << std::scientific << std::setprecision(12);
            switch(tcit->second.type) {
               case TimeSystemCorrection::GPUT:
                    os << ", A0 = " << tcit->second.A0
                        << ", A1 = " << tcit->second.A1
                        << ", RefTime = week/sow " << tcit->second.refWeek
                        << "/" << tcit->second.refSOW;
                  break;
               case TimeSystemCorrection::GAUT:
                    os << ", A0 = " << tcit->second.A0
                        << ", A1 = " << tcit->second.A1
                        << ", RefTime = week/sow " << tcit->second.refWeek
                        << "/" << tcit->second.refSOW;
                  break;
               case TimeSystemCorrection::SBUT:
                    os << ", A0 = " << tcit->second.A0
                        << ", A1 = " << tcit->second.A1
                        << ", RefTime = week/sow " << tcit->second.refWeek
                        << "/" << tcit->second.refSOW
                        << ", provider " << tcit->second.geoProvider
                        << ", UTC ID = " << tcit->second.geoUTCid;
                  break;
               case TimeSystemCorrection::GLUT:
                    os << ", -TauC = " << tcit->second.A0;
                  break;
               case TimeSystemCorrection::GPGA:
                    os << ", A0G = " << tcit->second.A0
                        << ", A1G = " << tcit->second.A1
                        << ", RefTime = week/sow " << tcit->second.refWeek
                        << "/" << tcit->second.refSOW;
                  break;
               case TimeSystemCorrection::GLGP:
                    os << ", TauGPS = " << tcit->second.A0
                        << " = " << tcit->second.A0 * C_MPS
                        << " m, RefTime = yr/mon/day "
                        << tcit->second.refYr
                        << "/" << tcit->second.refMon
                        << "/" << tcit->second.refDay;
                  break;
            }
            os << std::endl;
         }

         NavFiles.dump(os, detail);

         GPSstore.dump(os, detail);
         GLOstore.dump(os, detail);
         GALstore.dump(os, detail);
         //if(GEOstore.size()) GEOstore.dump(os, detail);
         //if(COMstore.size()) COMstore.dump(os, detail);
         os << "End dump Rinex3EphemerisStore." << std::endl;
      }

      /// Edit the dataset, removing data outside the indicated time interval
      /// @param[in] tmin defines the beginning of the time interval
      /// @param[in] tmax defines the end of the time interval
      virtual void edit(const CommonTime& tmin, 
                        const CommonTime& tmax = CommonTime::END_OF_TIME) throw()
      {
         if (GPSstore.size()) GPSstore.edit(tmin, tmax);
         if (GLOstore.size()) GLOstore.edit(tmin, tmax);
         if (GALstore.size()) GALstore.edit(tmin, tmax);
         //GEOstore.edit(tmin, tmax);
         //COMstore.edit(tmin, tmax);
      }

      /// Clear the dataset, meaning remove all data
      virtual void clear(void) throw()
      {
         NavFiles.clear();
         GPSstore.clear();
         GLOstore.clear();
         GALstore.clear();
         //GEOstore.clear();
         //COMstore.clear();
      }

      /// Return time system of this store. NB this is needed only to satisfy the
      /// XvtStore virtual interface; the system stores (GPSstore, GLOstore, etc)
      /// will be used internally to determine time system.
      virtual TimeSystem getTimeSystem(void) const throw()
         { return TimeSystem::Any; }

      /// Determine the earliest time for which this object can successfully 
      /// determine the Xvt for any object.
      /// @return the earliest time in the table
      /// @throw InvalidRequest if the object has no data.
      virtual CommonTime getInitialTime() const throw(InvalidRequest);

      /// Determine the latest time for which this object can successfully 
      /// determine the Xvt for any object.
      /// @return the latest time in the table
      /// @throw InvalidRequest if the object has no data.
      virtual CommonTime getFinalTime() const throw(InvalidRequest);

      /// Return true if IndexType=SatID is present in the data tables
      virtual bool isPresent(const SatID& sat) const throw()
      {
         switch(sat.system) {
            case SatID::systemGPS:     return GPSstore.isPresent(sat);
            case SatID::systemGlonass: return GLOstore.isPresent(sat);
            case SatID::systemGalileo: return GALstore.isPresent(sat);
            //case SatID::systemGeosync: return GEOstore.isPresent(sat);
            //case SatID::systemCompass: return COMstore.isPresent(sat);
            default: return false;
         }
         return false;
      }

      /// Return true if velocity is present in the data tables
      virtual bool hasVelocity() const throw()
         {  return true; }

   // end of XvtStore interface

      /// add a Rinex3NavData to the store
      /// @param Rinex3NavData Rdata data to be added
      /// @return true if data was added, false otherwise
      bool addEphemeris(const Rinex3NavData& Rdata) throw();

      /// add filename and header to FileStore
      /// @param string filename file name to be added
      /// @param Rinex3NavHeader Rhead header to be added
      void addFile(const std::string& filename, Rinex3NavHeader& head) throw()
      { NavFiles.addFile(filename,head); }

      /// load a RINEX navigation file
      /// @param string filename name of the RINEX navigation file to read
      /// @param bool dump if true, dump header and nav data as read, default false
      /// @param ostream stream to which dump is written, default cout
      /// @return -1 failed to open file,
      ///         -2 failed to read header (this->Rhead),
      ///         -3 failed to read data (this->Rdata),
      ///        >=0 number of nav records read
      /// @throw some other problem
      int loadFile(const std::string& filename, bool dump=false, std::ostream& s=std::cout)
         throw(gpstk::Exception);

      /// use to access the data records in the store in bulk
      int addToList(std::list<Rinex3NavData>& theList,
                    SatID sysSat=SatID(-1,SatID::systemMixed))
         const throw();

      /// get the number of records, optionally by system
      int size(SatID::SatelliteSystem sys = SatID::systemMixed) const throw()
      {
         int n(0);
         if(sys==SatID::systemMixed || sys==SatID::systemGPS)
            n += GPSstore.size();
         if(sys==SatID::systemMixed || sys==SatID::systemGlonass)
            n += GLOstore.size();
         if(sys==SatID::systemMixed || sys==SatID::systemGalileo)
            n += GALstore.size();
         //if(sys==SatID::systemMixed || sys==SatID::systemGeosync)
         //   n += GEOstore.size();
         //if(sys==SatID::systemMixed || sys==SatID::systemCompass)
         //   n += COMstore.size();
         return n;
      }

      /// Add to the map of time system corrections. Overwrite the existing
      /// correction of the same type, if it exists.
      /// @return true if an existing correction was overwritten.
      bool addTimeCorr(const TimeSystemCorrection& tsc) throw()
      {
         // true if this type already exists
         bool overwrite(mapTimeCorr.find(tsc.asString4()) != mapTimeCorr.end());

         // add or overwrite it
         mapTimeCorr[tsc.asString4()] = tsc;

         return overwrite;
      }

      /// Delete from the map of time system corrections.
      /// @param type of TimeSystemCorrection, as a string,
      ///                   i.e. TimeSystemCorrection::asString4()
      /// @return true if an existing correction was deleted.
      bool delTimeCorr(const std::string& typestr) throw()
      {
         std::map<std::string, TimeSystemCorrection>::iterator it;
         it = mapTimeCorr.find(typestr);
         if(it != mapTimeCorr.end()) {
            mapTimeCorr.erase(it);
            return true;
         }
         return false;
      }

      /// Fill out the time system corrections "network" by adding corrections that
      /// can be derived from existing corrections. For example,
      ///   given GPUT (GPS to UTC(USNO), from LEAP SECONDS)
      ///     and GLUT (GLO to UTC(SU), from CORR TO SYSTEM TIME)
      /// compute GLGP (GLO to GPS) by assuming all UTC's are equivalent.
      /// @return the number of new TimeSystemCorrection's
      int expandTimeCorrMap(void) throw()
      {
         int n(0);
         std::map<std::string, TimeSystemCorrection>::iterator it,jt;

         // currently there are only two possibilities : GPGA and GLGP
         // GLGP : GLO to GPS
         it = mapTimeCorr.find(std::string("GPUT"));
         jt = mapTimeCorr.find(std::string("GLUT"));
         if(it != mapTimeCorr.end() && jt != mapTimeCorr.end()
            && mapTimeCorr.find(std::string("GLGP")) == mapTimeCorr.end())
         {
            TimeSystemCorrection tc("GLGP");
            tc.A0 = jt->second.A0 - it->second.A0;
            tc.A1 = jt->second.A1 - it->second.A1; // probably zeros
            tc.refYr = jt->second.refYr;           // take ref time from GLO
            tc.refMon = jt->second.refMon;
            tc.refDay = jt->second.refDay;
            tc.refWeek = jt->second.refWeek;
            tc.refSOW = jt->second.refSOW;
            tc.refSOW = jt->second.refSOW;
            tc.geoProvider = jt->second.geoProvider;  // blank
            tc.geoUTCid = 0;                          // NA
            mapTimeCorr[tc.asString4()] = tc;
            n++;
         }

         // GPGA : GPS to GAL
         it = mapTimeCorr.find(std::string("GAUT"));
         jt = mapTimeCorr.find(std::string("GPUT"));
         if(it != mapTimeCorr.end() && jt != mapTimeCorr.end()
            && mapTimeCorr.find(std::string("GPGA")) == mapTimeCorr.end())
         {
            TimeSystemCorrection tc("GPGA");
            tc.A0 = jt->second.A0 - it->second.A0;
            tc.A1 = jt->second.A1 - it->second.A1;
            tc.refYr = it->second.refYr;           // take ref time from GAL
            tc.refMon = it->second.refMon;
            tc.refDay = it->second.refDay;
            tc.refWeek = it->second.refWeek;
            tc.refSOW = it->second.refSOW;
            tc.refSOW = it->second.refSOW;
            tc.geoProvider = it->second.geoProvider;  // blank
            tc.geoUTCid = 0;                          // NA
            mapTimeCorr[tc.asString4()] = tc;
            n++;
         }

         return n;
      }

      /// Get integration step for GLONASS Runge-Kutta algorithm (seconds)
      double getGLOStep(void) const
         { return GLOstore.getIntegrationStep(); }

      /// Set integration step for GLONASS Runge-Kutta algorithm (seconds)
      void setGLOStep(double step)
         { GLOstore.setIntegrationStep(step); }

      /// Get flag that causes unhealthy ephemerides to be excluded (GPS/GLO/GAL)
      bool getOnlyHealthyFlag(void) const
         { return GPSstore.getOnlyHealthyFlag(); }

      /// Set flag that causes unhealthy ephemerides to be excluded (GPS/GLO/GAL)
      void setOnlyHealthyFlag(bool flag)
      {
         GPSstore.setOnlyHealthyFlag(flag);
         GLOstore.setCheckHealthFlag(flag);
         GALstore.setOnlyHealthyFlag(flag);
      }

      /// use findNearEphemeris() in the getSat...() routines (GPS/GAL)
      void SearchNear(void) throw()
      {
         GPSstore.SearchNear();
         GALstore.SearchNear();
      }

      /// use findEphemeris() in the getSat...() routines (the default) (GPS/GAL)
      void SearchPast(void) throw()
      {
         GPSstore.SearchPast();
         GALstore.SearchPast();
      }

   }; // end class Rinex3EphemerisStore

}  // namespace

#endif // GPSTK_RINEX3EPHEMERISSTORE_HPP
