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
            return GLOstore.addEphemeris(GloEphemeris(Rdata));
            break;
         case SatID::systemGalileo:
            return GALstore.addEphemeris(GalEphemeris(Rdata));
            break;
         case SatID::systemGeosync:
            //return GEOstore.addEphemeris(GeoEphemeris(Rdata));
            break;
         case SatID::systemCompass:
            //return COMstore.addEphemeris(ComEphemeris(Rdata));
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
      throw(gpstk::Exception)
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
            what = string("Failed to read header of file ") + filename
               + string(" : ") + e.getText();
            return -2;
         }
         if(dump) Rhead.dump(s);

         // add to FileStore
         try {NavFiles.addFile(filename, Rhead); }
         catch(InvalidRequest& ir) {
           cout << " Exception caught from FileStore addFile in Rinex3EphemerisStore, line 122 "
                << endl << " Invoking dump of filestore: " << endl;
      NavFiles.dump(cout, 1);
      GPSTK_RETHROW(ir);
         }

         // add to mapTimeCorr
         if(Rhead.mapTimeCorr.size() > 0) {
            map<string, TimeSystemCorrection>::const_iterator it;
            for(it=Rhead.mapTimeCorr.begin(); it!=Rhead.mapTimeCorr.end(); ++it)
               addTimeCorr(it->second);
         }

         while(1) {
            // read the record
            try { strm >> Rdata; }
            catch(Exception& e) {
               what = string("Failed to read data in file ") + filename
                  + string(" : ") + e.getText();
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

            bool added;
            added = addEphemeris(Rdata);
         }
         return nread;
      }

      catch(Exception& e) {

         GPSTK_RETHROW(e);
      }

   } // end Rinex3EphemerisStore::loadFile

   // Utility routine for getXvt to test time systems and convert.
   // Convert ttag to the target time system, using the first appropriate correction
   // in the list, and return it. If no correction is found, ttag is unchanged and
   // an exception is thrown.
   CommonTime correctTimeSystem(const CommonTime ttag,
                                const TimeSystem targetSys,
                                const map<string, TimeSystemCorrection>& theMap)
      throw(InvalidRequest)
   {
      CommonTime toReturn(ttag);

      // is a conversion necessary?
      if(ttag.getTimeSystem() == targetSys)
         return toReturn;

      // the corrected timetag: now only the system, not the value, matters
      toReturn.setTimeSystem(targetSys);

      // look up the TimeSystemCorr in list, and do the conversion
      map<string, TimeSystemCorrection>::const_iterator it;
      for(it = theMap.begin(); it != theMap.end(); ++it) {
         if(it->second.convertSystem(ttag, toReturn))
            return toReturn;
      }

      // failure
      InvalidRequest e("Unable to convert time system "
         + ttag.getTimeSystem().asString() + " to satellite system "
         + targetSys.asString());
      GPSTK_THROW(e);

      return ttag;      // never reached, satisfy some compilers
   }

   // Returns the position, velocity, and clock offset of the indicated
   // object in ECEF coordinates (meters) at the indicated time.
   // @param[in] sat the satellite of interest
   // @param[in] ttag the time to look up
   // @return the Xvt of the object at the indicated time
   // @throw InvalidRequest If the request can not be completed for any
   //    reason, this is thrown. The text may have additional
   //    information as to why the request failed.
   Xvt Rinex3EphemerisStore::getXvt(const SatID& sat, const CommonTime& inttag)
      const throw(InvalidRequest)
   {
      try {
         Xvt xvt;
         CommonTime ttag;

         switch(sat.system) {
            case SatID::systemGPS:
               ttag = correctTimeSystem(inttag, TimeSystem::GPS, mapTimeCorr);
               xvt = GPSstore.getXvt(sat,ttag);
               break;
            case SatID::systemGlonass:
               ttag = correctTimeSystem(inttag, TimeSystem::GLO, mapTimeCorr);
               xvt = GLOstore.getXvt(sat,ttag);
               break;
            case SatID::systemGalileo:
               ttag = correctTimeSystem(inttag, TimeSystem::GAL, mapTimeCorr);
               xvt = GALstore.getXvt(sat,ttag);
               break;
            //case SatID::systemGeosync:
            //   ttag = correctTimeSystem(inttag, TimeSystem::GEO, mapTimeCorr);
            //   xvt = GEOstore.getXvt(sat,ttag);
            //   break;
            //case SatID::systemCompass:
            //   ttag = correctTimeSystem(inttag, TimeSystem::COM, mapTimeCorr);
            //   xvt = COMstore.getXvt(sat,ttag);
            //   break;
            default:
               InvalidRequest e("Unsupported satellite system");
               GPSTK_THROW(e);
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

         // CommonTime does not allow comparisons unless TimeSystems agree,
         // or if one is "Any"
         
         if (GPSstore.size()) {
           time = GPSstore.getInitialTime();
           retTime.setTimeSystem(TimeSystem::GPS);
           if(time < retTime) {
            retTime = time;
           }
         }
         if (GLOstore.size()) {
           time = GLOstore.getInitialTime();
           retTime.setTimeSystem(TimeSystem::GLO);
           if(time < retTime) {
            retTime = time;
           }
         }
         if (GALstore.size()) {
           time = GALstore.getInitialTime();
           retTime.setTimeSystem(TimeSystem::GAL);
           if(time < retTime) {
            retTime = time;
           }
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
         
         if (GPSstore.size()) { 
           time = GPSstore.getFinalTime();
           retTime.setTimeSystem(TimeSystem::GPS);
           if(time > retTime) {
              retTime = time;
           }
         }
         if (GLOstore.size()) {
           time = GLOstore.getFinalTime();
           retTime.setTimeSystem(TimeSystem::GLO);
           if(time > retTime) {
            retTime = time;
           }
         }
         if (GALstore.size()) {
           time = GALstore.getFinalTime();
           retTime.setTimeSystem(TimeSystem::GAL);
           if(time > retTime) {
            retTime = time;
           }
         }
         //time = GEOstore.getFinalTime();
         //if(time > retTime) {
         //   retTime = time;
         //   retTime.setTimeSystem(TimeSystem::Any);
         //}
         //time = COMstore.getFinalTime();
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
      if(sysSat.system==SatID::systemMixed || sysSat.system==SatID::systemGlonass) {
         list<GloEphemeris> GLOlist;
         n += GLOstore.addToList(GLOlist);

         list<GloEphemeris>::const_iterator it;
         for(it=GLOlist.begin(); it != GLOlist.end(); ++it)
            thelist.push_back(Rinex3NavData(*it));
      }
      if(sysSat.system==SatID::systemMixed || sysSat.system==SatID::systemGalileo) {
         list<GalEphemeris> GALlist;
         n += GALstore.addToList(GALlist);

         list<GalEphemeris>::const_iterator it;
         for(it=GALlist.begin(); it != GALlist.end(); ++it)
            thelist.push_back(Rinex3NavData(*it));
      }
      /*
      if(sysSat.system==SatID::systemMixed || sysSat.system==SatID::systemGeosync) {
         list<GeoRecord> GEOlist;
         n += GEOstore.addToList(GEOlist);

         list<GeoRecord>::const_iterator it;
         for(it=GEOlist.begin(); it != GEOlist.end(); ++it)
            thelist.push_back(Rinex3NavData(*it));
      }
      if(sysSat.system==SatID::systemMixed || sysSat.system==SatID::systemCompass) {
         list<ComRecord> COMlist;
         n += COMstore.addToList(COMlist);

         list<ComRecord>::const_iterator it;
         for(it=COMlist.begin(); it != COMlist.end(); ++it)
            thelist.push_back(Rinex3NavData(*it));
      }
      */
      return n;
   }

}  // namespace gpstk
