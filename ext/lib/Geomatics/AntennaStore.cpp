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

/// @file AntennaStore.cpp
/// Store antenna phase center offset information, in AntexData objects, with
/// receiver/satellite name.
/// Access using name (receivers), or name and time (satellites); compute compute PCOs
/// at any (elevation, azimuth).
 
#include "AntennaStore.hpp"
#include "Position.hpp"
#include "Matrix.hpp"
#include "SolarPosition.hpp"
#include "SunEarthSatGeometry.hpp"

using namespace std;

namespace gpstk
{

   // Add the given name, AntexData pair. If the name already exists in the store,
   // replace the data for it with the input object.
   // throw if the AntexData is invalid.
   void AntennaStore::addAntenna(string name, AntexData& antdata) throw(Exception)
   {
      if(!antdata.isValid()) {
         Exception e("Invalid AntexData object");
         GPSTK_THROW(e);
      }

      // is the name already in the store?
      map<string, AntexData>::iterator it;
      it = antennaMap.find(name);
      if(it != antennaMap.end())       // erase it
         antennaMap.erase(it);

      // add the new data
      antennaMap[name] = antdata;
   }

   // Get the antenna data for the given name from the store.
   // return true if successful, false if input name was not found in the store
   bool AntennaStore::getAntenna(string name, AntexData& antdata) throw()
   {
      map<string, AntexData>::iterator it;
      it = antennaMap.find(name);
      if(it != antennaMap.end()) {
         antdata = it->second;
         return true;
      }
      return false;
   }

   // Get the antenna data for the given satellite from the store.
   // Satellites are identified by two things:
   // system character: G or blank GPS, R GLONASS, E GALILEO, M MIXED
   // and integer PRN or SVN number.
   // NB. PRNs apply to GLONASS as well as GPS
   // NB. Typically there is more than one antenna per satellite in ANTEX files;
   // after calling include...Satellites(), when the store is loaded using
   // addANTEXfile(), a time tag should be passed as input; this will load only
   // the satellites valid at that time tag - most likely exactly one per sys/PRN.
   // param        char sys  System character for the satellite: G,R,E or M
   // param           int n  PRN (or SVN) of the satellite
   // param    string& name  Output antenna (ANTEX) name for the given satellite
   // param AntexData& data  Output antenna data for the given satellite
   // param   bool inputPRN  If false, parameter n is SVN not PRN (default true).
   // return true if successful, false if satellite was not found in the store
   bool AntennaStore::getSatelliteAntenna(const char sys, const int n,
                            string& name, AntexData& data,
                            bool inputPRN) const throw()
   {
      map<string, AntexData>::const_iterator it;
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

   // Get a vector of all antenna names in the store
   void AntennaStore::getNames(vector<string>& names) throw()
   {
      names.clear();
      map<string, AntexData>::iterator it;
      for(it = antennaMap.begin(); it != antennaMap.end(); it++)
         names.push_back(it->first);
   }

   // Get a vector of all receiver antenna names in the store
   void AntennaStore::getReceiverNames(vector<string>& names) throw()
   {
      names.clear();
      map<string, AntexData>::iterator it;
      for(it = antennaMap.begin(); it != antennaMap.end(); it++) {
         if(it->second.isRxAntenna)
            names.push_back(it->first);
      }
   }

   // call to give the store a list of receiver antenna names so that only
   // those names will be included in the store (not applicable to satellites).
   // If there are already other names in the store, they will be removed.
   // NB. call before addAntenna() or addANTEXfile()
   void AntennaStore::includeReceivers(vector<string>& names) throw()
   {
      namesToInclude = names;
      if(antennaMap.size() == 0) return;

      int j;
      map<string, AntexData>::iterator it;
      vector<string> rejects;
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

   // Open and read an ANTEX format file with the given name, and read it.
   // Add to the store all the receivers with names in the given vector,
   // if it has been provided in a previous call to includeReceivers(),
   // otherwise include all receiver antennas found.
   // NB. call includeSats() or includeGPSsats() to include satellite antennas,
   // before calling this routine.
   // param filename the name of the ANTEX file to read.
   // param time     the time (any) of interest, used to choose valid satellites
   // return the number of antennas added.
   // throw any exception caught during reading the file.
   int AntennaStore::addANTEXfile(string filename,
                    CommonTime time)
      throw(Exception)
   {
      try {
         int i,n=0;
         AntexHeader anthdr;
         AntexData antdata;
         AntexStream antstrm, asout;
         CommonTime time1,time2;

         // test for validity within a few days of time
         time.setTimeSystem(TimeSystem::Any);
         time1 = time2 = time;
         if(time > CommonTime::BEGINNING_OF_TIME) {
            time1 += double(2 * 86400);
            time2 -= double(2 * 86400);
         }

         // open the input file
         antstrm.open(filename.c_str(),ios::in);
         if(!antstrm.is_open()) {
            Exception e("Could not open file " + filename);
            GPSTK_THROW(e);
         }
         antstrm.exceptions(fstream::failbit);

         // read the header
         try {
            antstrm >> anthdr;
            if(!anthdr.isValid()) {
               Exception e("Header is not valid");
               GPSTK_THROW(e);
            }
         }
         catch(Exception& e) { GPSTK_RETHROW(e); }
         catch(exception& e) {
            Exception ge(string("Std exception: ") + e.what());
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
            string name = antdata.name();

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

   // Compute the vector from the SV Center of Mass (COM) to
   // the phase center of the antenna. 
   // Satellites are identified by two things:
   // system character: G or blank GPS, R GLONASS, E GALILEO, C BeiDou
   // and integer PRN or SVN number.
   // NB. PRNs apply to GLONASS as well as GPS
   // NB. Typically there is more than one antenna per satellite in ANTEX files;
   // after calling include...Satellites(), when the store is loaded using
   // addANTEXfile(), a time tag should be passed as input; this will load only
   // the satellites valid at that time tag - most likely exactly one per sys/PRN.
   // param        char sys  System character for the satellite: G,R,E or M
   // param           int n  PRN (or SVN) of the satellite
   // param Triple satVector Vector from center of Earth to SV. 
   // param   bool inputPRN  If false, parameter n is SVN not PRN (default true).
   // return vector from COM to PC
   // throw InvalidRequest if no data available
   Triple AntennaStore::ComToPcVector(const char sys,
                                      const int n,
                                      const CommonTime& ct,
                                      const Triple& satVector, 
                                      bool inputPRN) const
      throw(Exception)
   {
      AntexData antenna;
      string name;
      try
      {
         if (getSatelliteAntenna(sys, n, name, antenna))
         {

            // tracking, and future expansion. 
            double fact1, fact2;
            string freq1, freq2;
            switch (sys)
            {
               case 'G':
               {
                  fact1 = 2.5458;   // (alpha+1)/alpha
                  fact2 = -1.5458;  // -1/alpha
                  freq1 = "G01";
                  freq2 = "G02";
                  break;
               }
               case 'R':
               {
                  fact1 = 2.53125;
                  fact2 = -1.52125;
                  freq1 = "R01";
                  freq2 = "R02";
                  break;
               }
               case 'C':
               {
                  fact1 = 2.53125;
                  fact2 = -1.52125;
                  freq1 = "C01";
                  freq2 = "C02";
                  break;
               }

               case 'E':
               {
                  double alpha = ((154*154) / (116.5*116.5)) -1.0;   // E1 and E5a
                  fact1 = (alpha+1.0) / alpha;   
                  fact2 = -1.0 / alpha; 
                  freq1 = "E01";
                  freq2 = "E05";
                  break;
               }

               default:
               {
                  stringstream ss;
                  ss << "Invalid satellite system " << sys << " PRN " << n; 
                  ss << " for AntennaStore::ComToPcVector.";
                  InvalidRequest ir(ss.str());
                  GPSTK_THROW(ir); 
               }
            }
             
           // Rotation matrix from satellite attitude.
           // Rot * [XYZ] = [body frame]
           Matrix<double> SVAtt;

           // Use low accuracy Sun vector from SunEarthSatGeometry
           double AR;
           Position Sun=SolarPosition(ct,AR);
           Position Rx=Position(satVector);
           SVAtt = SatelliteAttitude(Rx,Sun);

           // phase center offset vector in body frame (at L1)
           Triple pco1 = antenna.getPhaseCenterOffset(freq1);
           Triple pco2 = antenna.getPhaseCenterOffset(freq2);
           Vector<double> PCO(3);
           for(int i=0; i<3; i++)            // body frame, mm -> m, iono-free combo
              PCO(i) = (fact1*pco1[i]+fact2*pco2[i])/1000.0;

           // PCO vector (from COM to PC) in ECEF XYZ frame, m
           Vector<double> SatPCOXYZ(3);
           SatPCOXYZ = transpose(SVAtt) * PCO;
           Triple pcoxyz = Triple(SatPCOXYZ(0), SatPCOXYZ(1), SatPCOXYZ(2));

           return pcoxyz; 

         }
         else
         {
            stringstream ss;
            ss << "AntennaStore::ComToPcVector.  No satellite data found for "
               << sys << " PRN " << n;
            InvalidRequest ir(ss.str());
            GPSTK_THROW(ir); 
         }
      }
      catch(Exception exc)
      {
         GPSTK_RETHROW(exc);
      }
   }

   // Same as above except with different calling sequence for convenience
   Triple AntennaStore::ComToPcVector(const SatID& sidr, 
                                      const CommonTime& ct, 
                                      const Triple& satVector) const
      throw(Exception)
   {
      char sys = ' ';
      Triple tp;
      switch (sidr.system)
      {
         case SatID::systemGPS:     {sys='G'; break;}
         case SatID::systemGalileo: {sys='E'; break;}  
         case SatID::systemGlonass: {sys='R'; break;}
         case SatID::systemBeiDou:  {sys='C'; break;}
         default:
         {
            stringstream ss;
            ss << "Invalid satellite system " << sidr; 
            ss << " for AntennaStore::ComToPcVector.";
            InvalidRequest ir(ss.str());
            GPSTK_THROW(ir); 
         }
      }

      try
      {
         tp = ComToPcVector(sys, sidr.id, ct, satVector);             
      }
      catch(Exception exc)
      {
         GPSTK_RETHROW(exc);
      }
      return tp; 
   }


   // dump the store
   void AntennaStore::dump(ostream& s, short detail)
   {
      s << "Dump (" << (detail==0 ? "low" : (detail==1 ? "medium":"high"))
         << " detail) of AntennaStore (" << antennaMap.size() << ") antennas\n";
      map<string, AntexData>::iterator it;
      for(it = antennaMap.begin(); it != antennaMap.end(); it++) {
         if(detail > 0) s << endl;
         it->second.dump(s,detail);
      }
      s << "End of dump of AntennaStore\n";
   }

}  // end namespace

