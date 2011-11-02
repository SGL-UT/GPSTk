#pragma ident "$Id$"

/**
 * @file Rinex3EphemerisStore.cpp
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

#include <string>
#include <list>

#include "Rinex3EphemerisStore.hpp"
#include "Rinex3NavStream.hpp"
#include "Rinex3NavData.hpp"

#include "EngEphemeris.hpp"

using namespace std;

namespace gpstk
{
   // add a Rinex3NavData to the store
   // @param Rinex3NavData Rdata data to be added
   // @return true if data was added, false otherwise
   bool Rinex3EphemerisStore::addEphemeris(const Rinex3NavData& Rdata) throw()
   {
      switch(Rdata.sat.system) {
         case SatID::systemGPS:
            return GPSstore.addEphemeris(EngEphemeris(Rdata));
            break;
         case SatID::systemGlonass:
            //return GLOstore.addEphemeris(GloRecord(Rdata));
            GLOstore.addEphemeris(Rdata);
            return true;
            break;
         case SatID::systemGalileo:
            //return GALstore.addEphemeris(GalRecord(Rdata));
            break;
         case SatID::systemGeosync:
            //return GEOstore.addEphemeris(GeoRecord(Rdata));
            break;
         case SatID::systemCompass:
            //return COMstore.addEphemeris(ComRecord(Rdata));
            break;
         default:
            break;
      }
      return false;
   }

   // load the given Rinex file
   // return -1 failed to open file,
   //        -2 failed to read header (this->Rhead),
   //        -3 failed to read data (this->Rdata),
   //       >=0 number of nav records read
   int Rinex3EphemerisStore::loadFile(const string& filename, bool dump, ostream& s)
      throw(Exception)
   {
      try {
         int nread(0);
         Rinex3NavStream strm;
         what = string();
         
         strm.open(filename.c_str(), ios::in);
         if(!strm.is_open()) {
            what = string("File ") + filename + string(" could not be opened.");
            return -1;
         }
         strm.exceptions(ios::failbit);

         try { strm >> Rhead; }
         catch(Exception& e) {
            what = string("Failed to read header of file ") + filename;
            return -2;
         }
         if(dump) Rhead.dump(s);

         // add to FileStore
         NavFiles.addFile(filename, Rhead);

         while(1) {
            // read the record
            try { strm >> Rdata; }
            catch(Exception& e) {
               what = string("Failed to read data in file ") + filename;
               return -3;
            }
            catch(exception& e) {
               what = string("std excep: ") + e.what();
               return -3;
            }
            catch(...) {
               what = string("Unknown exception while reading data of file ")
                  + filename;
               return -3;
            }

            if(!strm.good() || strm.eof()) break;

            nread++;
            if(dump) Rdata.dump(s);

            addEphemeris(Rdata);
         }

         return nread;
      }
      catch(Exception& e) {
         GPSTK_RETHROW(e);
      }

   } // end Rinex3EphemerisStore::loadFile

   // Returns the position, velocity, and clock offset of the indicated
   // object in ECEF coordinates (meters) at the indicated time.
   // @param[in] sat the satellite of interest
   // @param[in] ttag the time to look up
   // @return the Xvt of the object at the indicated time
   // @throw InvalidRequest If the request can not be completed for any
   //    reason, this is thrown. The text may have additional
   //    information as to why the request failed.
   Xvt Rinex3EphemerisStore::getXvt(const SatID& sat, const CommonTime& ttag)
      const throw(InvalidRequest)
   {
      try {
         Xvt xvt;

         switch(sat.system) {
            case SatID::systemGPS:
               xvt = GPSstore.getXvt(sat,ttag);
               break;
            case SatID::systemGlonass:
               xvt = GLOstore.getXvt(sat,ttag);
               break;
            case SatID::systemGalileo:
               xvt = GALstore.getXvt(sat,ttag);
               break;
            //case SatID::systemGeosync:
            //   xvt = GEOstore.getXvt(sat,ttag);
            //   break;
            //case SatID::systemCompass:
            //   xvt = COMstore.getXvt(sat,ttag);
            //   break;
            default:
               break;
         }

         return xvt;
      }
      catch(InvalidRequest& ir) { GPSTK_RETHROW(ir); }
   }

   // Determine the earliest time for which this object can successfully 
   // determine the Xvt for any object.
   // @return the earliest time in the table
   // @throw InvalidRequest if the object has no data.
   CommonTime Rinex3EphemerisStore::getInitialTime() const throw(InvalidRequest)
   {
      try {
         CommonTime retTime(CommonTime::END_OF_TIME),time;

         // CommonTime does not allow comparisions unless TimeSystems agree,
         // or if one is "Any"
         retTime.setTimeSystem(TimeSystem::Any);
         
         time = GPSstore.getInitialTime();
         if(time < retTime) {
            retTime = time;
            retTime.setTimeSystem(TimeSystem::Any);
         }
         time = GLOstore.getInitialTime();
         if(time < retTime) {
            retTime = time;
            retTime.setTimeSystem(TimeSystem::Any);
         }
         time = GALstore.getInitialTime();
         if(time < retTime) {
            retTime = time;
            retTime.setTimeSystem(TimeSystem::Any);
         }
         //time = GEOstore.getInitialTime();
         //if(time < retTime) {
         //   retTime = time;
         //   retTime.setTimeSystem(TimeSystem::Any);
         //}
         //time = COMstore.getInitialTime();
         //if(time < retTime) {
         //   retTime = time;
         //   retTime.setTimeSystem(TimeSystem::Any);
         //}

         return retTime;
      }
      catch(InvalidRequest& ir) { GPSTK_RETHROW(ir); }
   }

   // Determine the latest time for which this object can successfully 
   // determine the Xvt for any object.
   // @return the latest time in the table
   // @throw InvalidRequest if the object has no data.
   CommonTime Rinex3EphemerisStore::getFinalTime() const throw(InvalidRequest)
   {
      try {
         CommonTime retTime(CommonTime::BEGINNING_OF_TIME),time;

         // CommonTime does not allow comparisions unless TimeSystems agree,
         // or if one is "Any"
         retTime.setTimeSystem(TimeSystem::Any);
         
         time = GPSstore.getInitialTime();
         if(time > retTime) {
            retTime = time;
            retTime.setTimeSystem(TimeSystem::Any);
         }
         time = GLOstore.getInitialTime();
         if(time > retTime) {
            retTime = time;
            retTime.setTimeSystem(TimeSystem::Any);
         }
         time = GALstore.getInitialTime();
         if(time > retTime) {
            retTime = time;
            retTime.setTimeSystem(TimeSystem::Any);
         }
         //time = GEOstore.getInitialTime();
         //if(time > retTime) {
         //   retTime = time;
         //   retTime.setTimeSystem(TimeSystem::Any);
         //}
         //time = COMstore.getInitialTime();
         //if(time > retTime) {
         //   retTime = time;
         //   retTime.setTimeSystem(TimeSystem::Any);
         //}

         return retTime;
      }
      catch(InvalidRequest& ir) { GPSTK_RETHROW(ir); }
   }

   // use to access the data records in the store in bulk
   int Rinex3EphemerisStore::addToList(list<Rinex3NavData>& thelist, SatID sysSat)
      const throw()
   {
      int i,n(0);
      if(sysSat.system==SatID::systemMixed || sysSat.system==SatID::systemGPS) {
         list<EngEphemeris> GPSlist;
         n += GPSstore.addToList(GPSlist);

         list<EngEphemeris>::const_iterator it;
         for(it=GPSlist.begin(); it != GPSlist.end(); ++it)
            thelist.push_back(Rinex3NavData(*it));
      }
      /*
      if(sysSat.system==SatID::systemMixed || sysSat.system==SatID::systemGlonass) {
         list<GloRecord> GLOlist;
         n += GLOstore.addToList(list);

         list<GloRecord>::const_iterator it;
         for(it=GLOlist.begin(); it != GLOlist.end(); ++it)
            thelist.push_back(Rinex3NavData(*it));
      }
      if(sysSat.system==SatID::systemMixed || sysSat.system==SatID::systemGalileo) {
         list<GalRecord> GALlist;
         n += GALstore.addToList(list);

         list<GalRecord>::const_iterator it;
         for(it=GALlist.begin(); it != GALlist.end(); ++it)
            thelist.push_back(Rinex3NavData(*it));
      }
      if(sysSat.system==SatID::systemMixed || sysSat.system==SatID::systemGeosync) {
         list<GeoRecord> GEOlist;
         n += GEOstore.addToList(list);

         list<GeoRecord>::const_iterator it;
         for(it=GEOlist.begin(); it != GEOlist.end(); ++it)
            thelist.push_back(Rinex3NavData(*it));
      }
      if(sysSat.system==SatID::systemMixed || sysSat.system==SatID::systemCompass) {
         list<ComRecord> COMlist;
         n += COMstore.addToList(list);

         list<ComRecord>::const_iterator it;
         for(it=COMlist.begin(); it != COMlist.end(); ++it)
            thelist.push_back(Rinex3NavData(*it));
      }
      */
      return n;
   }

}  // namespace gpstk
