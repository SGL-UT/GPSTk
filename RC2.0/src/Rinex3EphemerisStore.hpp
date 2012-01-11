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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

#include "CommonTime.hpp"
#include "SatID.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"
#include "Xvt.hpp"

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

      /// string containing what() of exceptions caught by loadFile()
      std::string what;

   // member functions:

      Rinex3EphemerisStore() throw()
      { }

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
         NavFiles.dump(os, detail);
         GPSstore.dump(os, detail);
         GLOstore.dump(os, detail);
         GALstore.dump(os, detail);
         //GEOstore.dump(os, detail);
         //COMstore.dump(os, detail);
         os << "End dump Rinex3EphemerisStore." << std::endl;
      }

      /// Edit the dataset, removing data outside the indicated time interval
      /// @param[in] tmin defines the beginning of the time interval
      /// @param[in] tmax defines the end of the time interval
      virtual void edit(const CommonTime& tmin, 
                        const CommonTime& tmax = CommonTime::END_OF_TIME) throw()
      {
         GPSstore.edit(tmin, tmax);
         GLOstore.edit(tmin, tmax);
         GALstore.edit(tmin, tmax);
         //GEOstore.edit(tmin, tmax);
         //COMstore.edit(tmin, tmax);
      }

      /// Clear the dataset, meaning remove all data
      virtual void clear(void) throw()
      {
         GPSstore.clear();
         GLOstore.clear();
         GALstore.clear();
         //GEOstore.clear();
         //COMstore.clear();
      }

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
      void addFile(const std::string& filename, const Rinex3NavHeader& head) throw()
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
      int loadFile(const std::string& filename, bool dump=false, ostream& s=std::cout)
         throw(Exception);

      /// use to access the data records in the store in bulk
      int addToList(list<Rinex3NavData>& theList,
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
         /*
         if(sys==SatID::systemMixed || sys==SatID::systemGalileo)
            n += GALstore.size();
         if(sys==SatID::systemMixed || sys==SatID::systemGeosync)
            n += GEOstore.size();
         if(sys==SatID::systemMixed || sys==SatID::systemCompass)
            n += COMstore.size();
         */
         return n;
      }

   }; // end class Rinex3EphemerisStore

}  // namespace

#endif // GPSTK_RINEX3EPHEMERISSTORE_HPP
