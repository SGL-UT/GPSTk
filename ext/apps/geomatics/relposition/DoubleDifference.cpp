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

/**
 * @file DoubleDifference.cpp
 * Form double differences and buffer them, for program DDBase.
 */

//------------------------------------------------------------------------------------
// TD DoubleDifference.cpp make small limit on DD buff size an input parameter
// TD DoubleDifference.cpp do we allow 'gaps' in ref sat's data?

//------------------------------------------------------------------------------------
// system includes
#include "TimeString.hpp"
// GPSTk

// DDBase
#include "DDBase.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
// prototypes -- this module only
void ComputeSingleDifferences(string baseline, map<SDid,RawData>& SDmap)
   throw(Exception);
int ComputeDoubleDifferences(map<SDid,RawData>& SDmap) throw(Exception);

//------------------------------------------------------------------------------------
// other prototypes
// ElevationMask.cpp
bool ElevationMask(double elevation, double azimuth) throw(Exception);

//------------------------------------------------------------------------------------
int DoubleDifference(void) throw(Exception)
{
try {
   int j,k;
   size_t i,n;
      // map to hold all buffered single differences for one baseline
   map<SDid,RawData> SDmap;

   if(CI.Verbose) oflog << "BEGIN DoubleDifference()"
      << " at total time " << fixed << setprecision(3)
      << double(clock()-totaltime)/double(CLOCKS_PER_SEC) << " seconds."
      << endl;

      // clear any existing DDs
   DDDataMap.clear();

      // loop over baselines
   for(n=0; n<Baselines.size(); n++) {

         // ----------------------------------------------------------
         // for this baseline, compute all SDs, then DDs, and buffer them
      if(CI.Verbose) oflog << "DoubleDifference() for baseline "
         << Baselines[n] << endl;

         // clear the SD map
      SDmap.clear();

         // ----------------------------------------------------------
         // compute all single differences for this baseline
         // give it same ordering as Baseline
      ComputeSingleDifferences(Baselines[n],SDmap);

         // loop over SD data, edit small ones and dump summary
      if(CI.Verbose) oflog << "Single difference summary for baseline "
          << Baselines[n] << endl;

      vector<SDid> Remove;    // these will be small dataset to delete later

      map<SDid,RawData>::const_iterator kt;
      for(k=1,kt=SDmap.begin(); kt != SDmap.end(); k++,kt++) {

         if(CI.Verbose) {
            oflog << " " << setw(2) << k << " " << kt->first
                  << " " << setw(5) << kt->second.count.size();
            if(kt->second.count.size() > 0)
               oflog << " " << setw(5) << kt->second.count.at(0) << " - "
                     << setw(5) << kt->second.count.at(kt->second.count.size()-1);
            else
               oflog << "    na -    na";

               // gaps - (count : number of pts)
            if(kt->second.count.size() > 0) {      // gcc needs this ...
               for(i=0; i<kt->second.count.size()-1; i++) {
                  j = kt->second.count.at(i+1) - kt->second.count.at(i);
                  if(j > 1) oflog
                     << " (" << kt->second.count.at(i)+1 << ":" << j-1 << ")";
               }
            }
         }

            // ignore small datasets
         if(kt->second.count.size() < 10) {   // TD make input parameter
            Remove.push_back(kt->first);
            if(CI.Verbose) oflog << " **Rejected";
         }

         if(CI.Verbose) oflog << endl;

      }  // end summary loop

         // delete marked SD buffers
      for(i=0; i<Remove.size(); i++) SDmap.erase(Remove[i]);
   
         // ----------------------------------------------------------
         // now compute double differences - according to timetable
      if(ComputeDoubleDifferences(SDmap)) return 1;

         // check that there are non-zero double differences

   }  // end loop over baselines

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end DoubleDifference()

//------------------------------------------------------------------------------------
// Compute all single differences 'site1' - 'site2', using the RawDataBuffers in
// Stations[site], and store the results in the given map<SDid,RawData>.
void ComputeSingleDifferences(string baseline, map<SDid,RawData>& SDmap)
   throw(Exception)
{
try {
   int beg,end;
   size_t i,j;
   GSatID sat;

      // decompose the baseline
   string site1=word(baseline,0,'-');
   string site2=word(baseline,1,'-');

      // find the beginning and ending *counts* of good data for this baseline
   if(QueryTimeTable(baseline,beg,end)) {
      oflog << "ERROR - baseline " << baseline
         << " not found in timetable. No single differences computed." << endl;
      return;
   }

      // find satellites in common
   map<GSatID,RawData>::const_iterator it1,it2;

      // loop over satellites at first site
   for(it1 = Stations[site1].RawDataBuffers.begin();
       it1 != Stations[site1].RawDataBuffers.end(); it1++) {

      sat = it1->first;
      // it1->second is RawData={ L1,L2,P1,P2,elev,az,count buffers = vector<> }

         // does this sat have data at the other station?
      it2 = Stations[site2].RawDataBuffers.find(sat);
      if(it2 == Stations[site2].RawDataBuffers.end()) continue;    // no

         // compute single differences for this satellite
         // here is where you define the ordering of sites: first(1) - second(2)
      SDid sdid(site1,site2,sat);
      RawData sddata;

         // loop over epochs, finding common data. start and stop the loop
         // at times determined by the timetable, NOT by the raw data buffers.
      i = j = 0;
      while(i < it1->second.count.size() && j < it2->second.count.size()) {

            // impose limits from timetable
              if(it1->second.count[i] > end) break;
         else if(it2->second.count[j] > end) break;
         else if(it1->second.count[i] < beg) i++;
         else if(it2->second.count[j] < beg) j++;
            // i and j are the same count (epoch)
         else if(it1->second.count[i] == it2->second.count[j]) {
               // reject data below MinElevation here
            //if(it1->second.elev[i] > CI.MinElevation &&
               //it2->second.elev[j] > CI.MinElevation) {
            if(ElevationMask(it1->second.elev[i],it1->second.az[i]) &&
               ElevationMask(it2->second.elev[j],it2->second.az[j])) {
            
                  // buffer the differences
               sddata.count.push_back(it1->second.count[i]);
               sddata.L1.push_back(it1->second.L1[i] - it2->second.L1[j]);
               sddata.L2.push_back(it1->second.L2[i] - it2->second.L2[j]);
               sddata.P1.push_back(it1->second.P1[i] - it2->second.P1[j]);
               sddata.P2.push_back(it1->second.P2[i] - it2->second.P2[j]);
               sddata.ER.push_back(it1->second.ER[i] - it2->second.ER[j]);
               sddata.elev.push_back(it1->second.elev[i]);

            }  // end if elevation is ok

               // next epoch
            i++;
            j++;
         }
            // i is behind j in time(count)
         else if(it1->second.count[i] < it2->second.count[j])
            i++;
            // i is ahead of j in time(count)
         else
            j++;

      }  // end while

         // save it in the map
      SDmap[sdid] = sddata;

   }  // end loop over satellites at first site

}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Assume SDmap is all for the same baseline
int ComputeDoubleDifferences(map<SDid,RawData>& SDmap) throw(Exception)
{
try {
   bool frst,ok;
   int indx,count = 0,ddsign;
   long nn1,nn2;
   double ddL1,ddL2,ddER,ddP1,ddP2,dd,db1,db2;
   CommonTime tt,ttnext;   // ttnext is the time of the next reference satellite switch
   //SDid sid,ref;        // SDid of the current satellite and reference satellite
   map<SDid,int> Inext; // index in count (all) buffers which is to be processed next
   map<SDid,RawData>::const_iterator it;

   if(SDmap.size() == 0) return 0;

      // initialize the 'next index' map to zero
      // find the smallest (earliest) count
   frst = true;
   for(it=SDmap.begin(); it != SDmap.end(); it++) {
      int jj=0;
      Inext[it->first] = jj;  
      if(frst || it->second.count[0] < count) {
         count = it->second.count[0];
         frst = false;
      }
   }
   
      // ref will be the SDid for the reference satellite
   SDid ref = SDmap.begin()->first;        // ref.sat is TBD by timetable

      // loop over epochs in the SDs
   ttnext = CommonTime::BEGINNING_OF_TIME;
   while(1) {
         // time at this count
      tt = FirstEpoch + count * CI.DataInterval;

         // get the reference satellite at this time
      if(tt > ttnext) {
         ttnext = tt;
         if(QueryTimeTable(ref, ttnext)) {         // error - timetable failed
            oflog << "DD: Error - failed to find reference from timetable at "
               << printTime(tt,"%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g") << " count "
               << count << " for baseline " << ref.site1 << "-" << ref.site2 << endl;
            return 1;
         }
         if(CI.Verbose) oflog << "DD: reference is set to " << ref << " at "
            << printTime(tt,"%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g")
            << " count " << count << endl;
      }

         // does reference satellite have data at this count?
      if(SDmap[ref].count[Inext[ref]] != count) {
         oflog << "Error - failed to find reference data " << ref << " at "
            << printTime(tt,"%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g") << endl;
            // TD return here, or just skip the epoch?
            // question is do we allow 'holes' in ref sat's data?
         return 1;
      }

         // compute DDs
      for(it=SDmap.begin(); it != SDmap.end(); it++) {

            // sid is the SDid for the current satellite
         SDid sid = it->first;
         indx = Inext[sid];

            // end of buffer has been reached
         if(Inext[sid] >= int(SDmap[sid].count.size())) continue;

         if(sid == ref) continue;                     // ignore the reference

            // no data for this satellite at this count
         if(SDmap[sid].count[indx] != count) continue;

            // compute DD phases and DD nominal range
         ddL1 = wl1 * (SDmap[sid].L1[indx] - SDmap[ref].L1[Inext[ref]]);
         ddL2 = wl2 * (SDmap[sid].L2[indx] - SDmap[ref].L2[Inext[ref]]);
         ddP1 = SDmap[sid].P1[indx] - SDmap[ref].P1[Inext[ref]];
         ddP2 = SDmap[sid].P2[indx] - SDmap[ref].P2[Inext[ref]];
         ddER = (SDmap[sid].ER[indx] - SDmap[ref].ER[Inext[ref]]);

            // get the appropriate DDData from the map, or create a new one
         map<DDid,DDData>::iterator jt;
         DDid ddid((ref.ssite == 1 ? ref.site1 : ref.site2),
                   (ref.ssite == 1 ? ref.site2 : ref.site1),sid.sat,ref.sat);
         if(DDDataMap.find(ddid) == DDDataMap.end()) {
               // create a new DDData
            DDData tddb;
            dd = (-ddL1+ddER)/wl1;
            nn1 = int(dd + (dd > 0 ? 0.5 : -0.5));
            tddb.L1bias = wl1 * nn1;
            dd = (-ddL2+ddER)/wl2;
            nn2 = int(dd + (dd > 0 ? 0.5 : -0.5));
            tddb.L2bias = wl2 * nn2;
            oflog << " Phase bias (initial) on " << ddid
               << " at " << setw(4) << count << " "
               << printTime(tt,"%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g");
            if(CI.Frequency != 2) oflog << " L1: " << setw(10) << nn1;
            if(CI.Frequency != 1) oflog << " L2: " << setw(10) << nn2;
            oflog << endl;
            //tddb.lastresetcount = count;
            tddb.resets.push_back(tddb.count.size());    // always one at beginning
            tddb.prevL1 = (ddL1-ddER)+tddb.L1bias;
            tddb.prevL2 = (ddL2-ddER)+tddb.L2bias;
            DDDataMap[ddid] = tddb;
         }
               
            // get the current DDData structure, and relative sign
         jt = DDDataMap.find(ddid); // never fail...
         ddsign = DDid::compare(ddid,jt->first);
         DDData& ddb=jt->second;
         ok = true;                 // if ok, buffer this DDData = ddb

            // reset bias?
         db1 = (ddsign*(ddL1-ddER) + ddb.L1bias - ddb.prevL1)/wl1;
         db2 = (ddsign*(ddL2-ddER) + ddb.L2bias - ddb.prevL2)/wl2;
         if((CI.Frequency != 2 && fabs(db1) > CI.PhaseBiasReset) ||
            (CI.Frequency != 1 && fabs(db2) > CI.PhaseBiasReset)) {
            long ndb1 = long(db1 + (db1 > 0 ? 0.5 : -0.5));
            long ndb2 = long(db2 + (db2 > 0 ? 0.5 : -0.5));
            oflog << " Phase bias (reset  ) on " << ddid
               << " at " << setw(4) << count << " "
               << printTime(tt,"%Y/%02m/%02d %2H:%02M:%6.3f=%F/%10.3g");
            if(CI.Frequency != 2) oflog << " L1: " << setw(10) << ndb1;
            if(CI.Frequency != 1) oflog << " L2: " << setw(10) << ndb2;
            oflog << endl;
            ddb.L1bias -= wl1 * ndb1;
            ddb.L2bias -= wl2 * ndb2;
            //ddb.lastresetcount = count;
            ddb.resets.push_back(ddb.count.size());
         }

            // remove the bias from the data
         ddL1 += ddsign * ddb.L1bias;
         ddL2 += ddsign * ddb.L2bias;

            // save for next time
         ddb.prevL1 = ddsign*(ddL1-ddER);
         ddb.prevL2 = ddsign*(ddL2-ddER);

            // buffer the debiased DDs
         ddb.DDL1.push_back(ddsign*ddL1);
         ddb.DDL2.push_back(ddsign*ddL2);
         ddb.DDP1.push_back(ddsign*ddP1);
         ddb.DDP2.push_back(ddsign*ddP2);
         ddb.DDER.push_back(ddsign*ddER);
         ddb.count.push_back(count);

            // increment Inext
         Inext[sid]++;

      }  // end loop over SD's in SDmap

      Inext[ref]++;
      
         // find the next count
         // quit when all Inext are at end
      frst = true;
      ok = false;
      for(it=SDmap.begin(); it != SDmap.end(); it++) {
         if(Inext[it->first] < int(SDmap[it->first].count.size())) {
            if(frst || it->second.count[Inext[it->first]] < count) {
               count = it->second.count[Inext[it->first]];
               frst = false;
            }
            ok = true;
         }
      }

      if(!ok) break;

   }  // end while loop over epochs

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
