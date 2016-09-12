/// @file SatPassUtilities.cpp
/// Various utilities using SatPass

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

#include "Stats.hpp"
#include "stl_helpers.hpp"
#include "logstream.hpp"

#include "SatPassUtilities.hpp"

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk {

// ---------------------------- sort, read and write SatPass lists ---------------
// NB uses SatPass::operator<()
void sort(vector<SatPass>& SPList) throw()
{
   std::sort(SPList.begin(), SPList.end());
}

// -------------------------------------------------------------------------------
void Dump(vector<SatPass>& SatPassList, ostream& os, bool rev, bool dbug)
   throw(Exception)
{
   try {
      int i,j,nep;
      Epoch ttag;
      map<unsigned int,unsigned int> indexMap;
      map<unsigned int,unsigned int>::const_iterator kt;
      vector<string> obstypes;
      ostringstream oss;

      // loop over all data in time order, and dump contents
      // --------------------------------------------------------------------
      SatPassIterator SPit(SatPassList,rev,dbug);
      ttag = SPit.getFirstGoodTime();
      os << "SPL Dump SatPass list " << printTime(ttag," from " + SatPass::longfmt);
      ttag = SPit.getLastGoodTime();
      os << printTime(ttag," to " + SatPass::longfmt)
         << " timeRevers is " << (rev ? "T":"F")
         << " and debug is " << (dbug ? "T":"F") << endl;

      // iterate over the data in time order
      nep = 0;
      while(SPit.next(indexMap)) {
         // NB. next() will never return an empty map with non-zero return value
         ttag = SatPassList[indexMap.begin()->first].time(indexMap.begin()->second);
         if(nep == 0) {
            obstypes = SatPassList[indexMap.begin()->first].getObsTypes();
         }
         nep++;

         // loop over satellites returned in indexMap - pull out data
         oss.str("");
         oss << "SPL " << printTime(ttag,SatPass::longfmt);

         // iterate over the data in time order
         for(kt=indexMap.begin(); kt != indexMap.end(); kt++) {
            int ii = kt->first;              // index pass
            int jj = kt->second;             // index count (epoch) in the pass

            os << oss.str() << " " << SatPassList[ii].status()
               << " " << RinexSatID(SatPassList[ii].getSat())
               << " " << SatPassList[ii].getFlag(jj);
            os << fixed << setprecision(3);

            for(size_t i=0; i<obstypes.size(); i++)
               os << " " << obstypes[i]
                  << " " << setw(13) << SatPassList[ii].data(jj,obstypes[i])
                  << " " << SatPassList[ii].LLI(jj,obstypes[i])
                  << " " << SatPassList[ii].SSI(jj,obstypes[i]);
            os << endl;

         }  // end loop over indexMap
      }  // end while loop over SPit.next()
      os << "SPL End Dump of SatPass list" << endl;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end Dump()

// -------------------------------------------------------------------------------
// Find millisecond adjusts of the time tag, pseudoranges C1 C2 P1 P2, phases L1 L2.
// User the handler to print messages, etc.
// @param  input SatPass list for analysis
// @param  return millisecond handler
// @return number of ms adjusts found
int FindMilliseconds(vector<SatPass>& SPList, msecHandler& msh) throw(Exception)
{
try {
   int i,ii,jj;
   double data;
   Epoch ttag;
   map<unsigned int,unsigned int> indexMap;
   map<unsigned int,unsigned int>::iterator kt;
   SatPassIterator SPit(SPList);

   msh.setDT(SPit.getDT());

   while(SPit.next(indexMap)) {
      // get the current timetag
      kt = indexMap.begin();
      ttag = SPList[kt->first].time(kt->second);

      // add all the data at this epoch
      for(kt = indexMap.begin(); kt != indexMap.end(); ++kt) {
         ii = kt->first; jj = kt->second;
         SatID sat = SPList[ii].getSat();
         vector<string> ots = SPList[ii].getObsTypes();
         // loop over obs types in this SP
         for(i=0; i<ots.size(); i++) {
            if(SPList[ii].hasType(ots[i])) {
               data = SPList[ii].data(jj,ots[i]);
               msh.add(ttag, sat, ots[i], data);
            }
         }
      }
   }

   // finalize the algorithm
   msh.afterAddbeforeFix();

   return msh.getNMS();
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end FindMilliseconds()

// -------------------------------------------------------------------------------
// Remove millisecond adjusts of the time tags and pseudoranges (C1 C2 P1 P2)
// and phases (L1 L2), given handler passed to earlier call to FindMilliseconds().
// User the handler to print messages, etc.
// @param  input SatPass list for modification
// @param  message returned from FindMilliseconds()
void RemoveMilliseconds(std::vector<SatPass>& SPList, msecHandler& msh)
   throw(Exception)
{
try {
   int i,ii,jj;
   double data;
   Epoch ttag;
   CommonTime ttagdum;
   map<unsigned int,unsigned int> indexMap;
   map<unsigned int,unsigned int>::iterator kt;

   SatPassIterator SPit(SPList);
   while(SPit.next(indexMap)) {
      // get the current timetag
      kt = indexMap.begin();
      ttag = SPList[kt->first].time(kt->second);

      // add all the data at this epoch
      int n(0);
      double deltfix(0.0);
      for(kt = indexMap.begin(); kt != indexMap.end(); ++kt) {
         ii = kt->first; jj = kt->second;
         SatID sat = SPList[ii].getSat();
         vector<string> ots = SPList[ii].getObsTypes();
         // loop over obs types in this SP
         for(i=0; i<ots.size(); i++) {
            if(SPList[ii].hasType(ots[i])) {
               data = SPList[ii].data(jj,ots[i]);
               // tricky - don't keep correcting ttag
               ttagdum = static_cast<CommonTime>(ttag);
               msh.fix(ttagdum, sat, ots[i], data);
               SPList[ii].data(jj,ots[i]) = data;
               if(++n == 1) deltfix = (ttagdum-ttag);    // only once
            }
         }
         // correct time tag for this SP
         if(n > 0 && deltfix != 0.0)
            SPList[ii].timeoffset(jj) += deltfix;
      }
   }

   //LOG(INFO) << msh.getFixMessage(verbose);
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end RemoveMilliseconds()

// -------------------------------------------------------------------------------
// prototype is in SatPass.hpp as a friend
int SatPassFromRinexFiles(vector<string>& filenames,
                          vector<string>& obstypes,
                          double dtin,
                          vector<SatPass>& SPList,
                          vector<RinexSatID> exSats,
                          bool lenient,
                          Epoch beginTime, Epoch endTime)
   throw(Exception)
{
try {
   if(filenames.size() == 0) return -1;

   // sort the file names on the begin time in the header
   if(filenames.size() > 1) sortRinexObsFiles(filenames);

   int i,j,nfiles(0),nepochs(0);
   ostringstream oss;
   unsigned short flag;
   vector<double> data(obstypes.size(),0.0);
   vector<unsigned short> ssi(obstypes.size(),0);
   vector<unsigned short> lli(obstypes.size(),0);
   map<RinexSatID,int> indexForSat;
   map<RinexSatID,int>::const_iterator satit;
   RinexObsHeader header;
   RinexObsData obsdata;
   const string timfmt(string("%F %10.3g = %04Y/%02m/%02d %02H:%02M:%02S"));

   // must make larger than 1millisec, but small enough to catch e.g. 1sec data
   const double dttol(0.01);                          // TD ??

   // estimate the data timestep
   const int estN(9);
   const double esttol(0.01);
   int estn[estN]={-1,-1,-1,-1,-1,-1,-1,-1,-1};
   double dt,estdt[estN];
   Epoch prevtime(CommonTime::BEGINNING_OF_TIME);
   // records out of time order
   bool onOrder(false),onShort(false);
   vector<int> nOrder,nShort;
   vector<Epoch> timeOrder,timeShort;

   // sort existing list on begin time
   sort(SPList);

   // fill the index array using SatPass's already there
   // assumes SPList is in time order - later ones overwrite earlier
   for(i=0; i<SPList.size(); i++)
      indexForSat[SPList[i].getSat()] = i;

   // loop over file names
   for(int nfile=0; nfile<filenames.size(); nfile++) {
      string filename = filenames[nfile];

      // does the file exist?
      RinexObsStream RinFile(filename.c_str());
      if(filename.empty() || !RinFile) {
         //cerr << "Error: input file " << filename << " does not exist.\n";
         continue;
      }
      RinFile.exceptions(fstream::failbit);
      //RinFile.beLenient(lenient);

      // is it a Rinex Obs file? ... read the header
      try { RinFile >> header; }
      catch(Exception& e) {
         //cerr << "Error: input file " << filename << " is not a Rinex obs file\n";
         continue;
      }

      // to return the number of files read
      nfiles++;

      // warn if lenient RINEX did something
      //if(!header.whatLenient.empty())
      //   oss << " Warning - changes were made by lenient RINEX reader: "
      //         << header.whatLenient << endl;

      // check that obs types are in header - first file only
      if(obstypes.size() == 0) {
         for(j=0; j<header.obsTypeList.size(); j++) {
            obstypes.push_back(RinexObsHeader::convertObsType(header.obsTypeList[j]));
         }
         data = vector<double>(obstypes.size(),0.0);
         ssi = vector<unsigned short>(obstypes.size(),0);
         lli = vector<unsigned short>(obstypes.size(),0);
      }
      // NB do not change obstypes past this, but may create newobstypes

      // loop over epochs in the file
      while(1) {
         try { RinFile >> obsdata; }
         catch(Exception& e) {
            LOG(ERROR) << "Reading RINEX obs threw exception " << e.what();
            GPSTK_RETHROW(e);
         }

         if(RinFile.eof() || !RinFile.good()) break;

         RinexObsData::RinexSatMap::const_iterator it;
         RinexObsData::RinexObsTypeMap::const_iterator jt;

         // test time limits
         if(obsdata.time < beginTime) continue;
         if(obsdata.time > endTime) break;

         // lenient readers
         //if(!obsdata.whatLenient.empty())
         //   oss << " Warning - lenient RINEX reader at "
         //      << printTime(obsdata.time,"%04Y/%02m/%02d %02H:%02M:%02S: ")
         //      << obsdata.whatLenient << endl;

         // skip auxiliary header, etc
         if(obsdata.epochFlag != 0 && obsdata.epochFlag != 1) continue;

         if(prevtime != CommonTime::BEGINNING_OF_TIME) {
            // compute time since the last epoch
            dt = obsdata.time - prevtime;

            if(dt > dttol) {        // positive dt only
               if(::fabs(::fmod(dt,dtin)) > dttol) {
                  if(lenient) {
                     // NB this is just decimation ...
                     if(!onShort) {
                        nShort.push_back(0);
                        timeShort.push_back(prevtime);
                        onShort = true;
                     }
                     nShort[nShort.size()-1]++;
                     continue;
                  }
                  else
                     GPSTK_THROW(Exception(string("Invalid time step: expected ")
                        + asString<double>(dtin) + string(" seconds but found ")
                        + asString<double>(dt) + string(" at time ")
                        + printTime(obsdata.time,timfmt)));
               }

               for(j=0; j<estN; j++) {
                  if(estn[j] <= 0) { estdt[j]=dt; estn[j]=1; break; }   // first one
                  if(::fabs(dt-estdt[j]) < esttol) { estn[j]++; break; }// matches j
                  if(j == estN-1) {                      // running out of room
                     int jj,kk(0),nleast(estn[0]);
                     for(jj=1; jj<estN; jj++) {          // find the least common dt
                        if(estn[jj] <= nleast) { kk = jj; nleast = estn[jj]; }
                     }
                     estn[kk] = 1; estdt[kk] = dt;       // replace it
                  }
               }
            }
            else if(dt < dttol) {         // negative, and positive but tiny (< dttol)
               if(lenient) {
                  if(!onOrder) {
                     nOrder.push_back(0);
                     timeOrder.push_back(prevtime);
                     onOrder = true;
                  }
                  nOrder[nOrder.size()-1]++;
                  continue;
               }
               else GPSTK_THROW(Exception(string("Invalid time step: expected ")
                  + asString<double>(dtin) + string(" seconds but found ")
                  + asString<double>(dt) + string(" at time ")
                  + printTime(obsdata.time,"%4F %10.3g")));
            }
         }
         onOrder = onShort = false;
         prevtime = obsdata.time;

         // loop over satellites
         for(it=obsdata.obs.begin(); it != obsdata.obs.end(); ++it) {
            RinexSatID sat = it->first;
            flag = SatPass::OK;
            
            // exclude sats
            if(vectorindex(exSats,sat) != -1) continue;
            if(vectorindex(exSats,RinexSatID(-1,sat.system)) != -1) continue;

            // loop over obs
            for(j=0; j<obstypes.size(); j++) {
               if((jt=it->second.find(RinexObsHeader::convertObsType(obstypes[j])))
                     == it->second.end()) {
                  data[j] = 0.0;
                  lli[j] = ssi[j] = 0;
                  // don't do this b/c SatPass may have empty obs types
                  //flag = SatPass::BAD;
               }
               else {
                  data[j] = jt->second.data;
                  lli[j] = jt->second.lli;
                  ssi[j] = jt->second.ssi;
                  // NB - some obstypes are missing on some sats;
                  // thus ngood applies to ALL obstypes
                  if(data[j] == 0.0) flag = SatPass::BAD;
               }
            }  // end loop over obs

            // find the current SatPass for this sat
            satit = indexForSat.find(sat);

            // if there is not a pass for this satellite, create one
            if(satit == indexForSat.end()) {
               SatPass newSP(sat,dtin,obstypes);
               SPList.push_back(newSP);
               indexForSat[sat] = SPList.size()-1;
               satit = indexForSat.find(sat);
            }
            
            // add the data to the SatPass
            do {
               i = SPList[satit->second].addData(obsdata.time,obstypes,
                                                 data,lli,ssi,flag);
               if(i == -1) {        // gap
                  SatPass newSP(sat,dtin,obstypes);
                  SPList.push_back(newSP);
                  indexForSat[sat] = SPList.size()-1;
                  satit = indexForSat.find(sat);
                  // repeat
               }

               // handle most above, but still may be problems eg between files
               else if(i == -2) {   // time tag out of order
                  Exception e("Timetags out of order in RINEX file " + filename
                     + " at time " + printTime(obsdata.time,timfmt)
                     + (lenient ? " - Error, this should not happen!" : ""));
                  GPSTK_THROW(e);
               }

               //else if(i == -3) {   // sat not found (RinexObsData form only)
               //}

            } while(i == -1);

         } // end loop over satellites
         nepochs++;

         if(timeShort.size() > 50 && timeShort.size() > nepochs/2) {
            for(i=0; i<timeOrder.size(); i++)
               LOG(WARNING) << "Warning - " << setw(4) << nOrder[i]
                  << " data records following epoch "<< printTime(timeOrder[i],timfmt)
                  << " are out of time order";
            LOG(ERROR) << "ERROR - too many 'short timestep' warnings - "
               << "decimate the data file first.";
            GPSTK_THROW(Exception("Too many short timesteps - decimate instead"));
         }

      } // end loop over obs data in file

      RinFile.close();

   }  // end loop over RINEX files

   // find the most common timestep
   for(j=0,i=1; i<estN; i++) if(estn[i] > estn[j]) j=i;
   dt = estdt[j];

   // is there disagreement? throw if there is; SatPass must have correct dt
   if(::fabs(dt-dtin) > esttol)
      GPSTK_THROW(Exception("Input time step (" + asString(dtin,2)
                + ") does not match computed (" + asString(dt,2) + ")"));

   string msg = oss.str();
   if(!msg.empty()) {
      stripTrailing(msg,'\n');
      stripTrailing(msg,'\r');
      LOG(WARNING) << msg;
   }
   if(timeShort.size() > 0) {
      for(i=0; i<timeShort.size(); i++)
         LOG(WARNING) << "Warning - " << setw(4) << nShort[i]
            << " data records following epoch " << printTime(timeShort[i],timfmt)
            << " have short (<" << dtin << "sec) timestep";
   }
   if(timeOrder.size() > 0) {
      for(i=0; i<timeOrder.size(); i++)
         LOG(WARNING) << "Warning - " << setw(4) << nOrder[i]
            << " data records following epoch " << printTime(timeOrder[i],timfmt)
            << " are out of time order";
   }

   return nfiles;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

// -------------------------------------------------------------------------------
// TD no this only works if the passes all have the same OTs in the same order....
int SatPassToRinexFile(string filename,
                       RinexObsHeader& header,
                       vector<SatPass>& SPList) throw(Exception)
{
   try {
      if(filename.empty()) return 0;

      int i,j,ii,jj,ngood;
      double data;
      vector<string> obstypes, ots;
      map<unsigned int, unsigned int> indexMap;
      map<unsigned int, unsigned int>::const_iterator kt;
      RinexObsData robs;
      //RinexObsData::RinexSatMap::const_iterator it;
      //RinexObsData::RinexObsTypeMap::const_iterator jt;

      // open file
      RinexObsStream rstrm(filename.c_str(), ios::out);
      if(!rstrm) return -1;
      rstrm.exceptions(fstream::failbit);

      // create a master list of obstypes - union of all passes
      for(i=0; i<SPList.size(); i++) {
         ots = SPList[i].getObsTypes();
         for(j=0; j<ots.size(); j++)
            if(vectorindex(obstypes, ots[j]) == -1)
               obstypes.push_back(ots[j]);
      }

      // is there a change?
      bool change(obstypes.size() != header.obsTypeList.size());
      if(!change)
         for(i=0; i<obstypes.size(); i++)
            if(obstypes[i] != RinexObsHeader::convertObsType(header.obsTypeList[i]))
               { change = true; break; }

      // put these obstypes in the header
      if(change) {
         header.obsTypeList.clear();
         for(i=0; i<obstypes.size(); i++)
            header.obsTypeList.push_back(RinexObsHeader::convertObsType(obstypes[i]));

         // must remove the table
         header.numObsForSat.clear();
         header.valid ^= RinexObsHeader::prnObsValid;
      }

      // create the iterator
      SatPassIterator spit(SPList);

      // put obs types, first time and interval in header
      header.firstObs = spit.getFirstTime();
      header.lastObs = spit.getLastTime();
      header.interval = spit.getDT();
      header.valid |= RinexObsHeader::firstTimeValid;
      header.valid |= RinexObsHeader::lastTimeValid;
      header.valid |= RinexObsHeader::intervalValid;

      rstrm << header;

      while(spit.next(indexMap)) {
         robs.obs.clear();
         robs.numSvs = 0;
         robs.clockOffset = 0.0;

         kt = indexMap.begin();
         robs.time = SPList[kt->first].time(kt->second);

         for(kt=indexMap.begin(); kt != indexMap.end(); ++kt) {
            ii = kt->first; jj = kt->second;
            if(SPList[ii].status() == -1) continue;
            SatID sat = SPList[ii].getSat();
            RinexObsData::RinexObsTypeMap rotm;
            for(ngood=0,j=0; j<header.obsTypeList.size(); j++) {
               RinexDatum rd;

               if(SPList[ii].getFlag(jj) != SatPass::BAD &&
                  SPList[ii].hasType(obstypes[j]))
               {
                  rd.data = SPList[ii].data(jj,obstypes[j]);
                  ngood++;
               }
               // else rd is all zeros

               rotm.insert(
                  map<RinexObsType, RinexDatum>
                     ::value_type(header.obsTypeList[j], rd));
            }
            if(ngood > 0) {
               robs.obs.insert(
                  map<SatID, RinexObsData::RinexObsTypeMap>::value_type(sat,rotm));
               robs.numSvs++;
            }
         }

         if(robs.numSvs == 0) continue;

         robs.epochFlag = 0;
         rstrm << robs;
      }

      rstrm.close();
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }

   return 0;
}

}  // end namespace

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
