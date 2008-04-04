#pragma ident "$Id$"

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

/**
 * @file Timetable.cpp
 * Compute reference satellites time table for program DDBase.
 */

//------------------------------------------------------------------------------------
// TD Timetable.cpp  handle week rollover in TimeTable() and ReadTimeTable()
// TD Timetable.cpp  check logic
// TD Timetable.cpp  check output messages
// TD Timetable.cpp  add several limits as input parameters
// TD Timetable.cpp  4. edit TTab, removing segments that do not create gaps

//------------------------------------------------------------------------------------
// includes
// system

// GPSTk
// Geomatics
#include "DDid.hpp"
#include "index.hpp"
// DDBase
#include "DDBase.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
// ElevationMask.cpp
double RotatedAntennaElevation(double elevation, double azimuth) throw(Exception);

//------------------------------------------------------------------------------------
// Segment structure used in deducing time table functions implemented in
// Timetable.cpp
class TTSegment {
public:
   std::string site1,site2;
   gpstk::GSatID sat;
   int start,end;    // starting and ending counts
   int first,last;   // counts to actually use in timetable
   int length;       // length (in data points)
   double minelev;   // minimum elevation in this segment
   double maxelev;   // maximum elevation in this segment

   TTSegment(void) : start(-1),length(0),minelev(0.0),maxelev(0.0) {}

   double metric(void) const
   { return (double(length)/100.0 + 100.0*(minelev+maxelev)/90.0); }

   //bool operator<(const TTSegment& right) const
   //{ return (metric() < right.metric()); }

   //bool operator>(const TTSegment& right) const
   //{ return (metric() > right.metric()); }

   void findElev(void) throw(Exception);

   friend ostream& operator<<(ostream& s, const TTSegment& t) throw(Exception);

   friend bool increasingMetricSort(const TTSegment& left, const TTSegment& right);
   friend bool decreasingMetricSort(const TTSegment& left, const TTSegment& right);
   friend bool startSort(const TTSegment& left, const TTSegment& right);
};

//------------------------------------------------------------------------------------
// local data
list<TTSegment> TimeTable;    // satellite time table
map<SDid,SDData> SDmap;       // map of SD data - not full single differences

//------------------------------------------------------------------------------------
// prototypes -- this module only
int ReadTimeTable(void) throw(Exception);
int ComputeBaselineTimeTable(const string& bl) throw(Exception);
int TTComputeSingleDifferences(const string& bl, const double ElevLimit)
   throw(Exception);
int TimeTableAlgorithm(list<TTSegment>& TTS, list<TTSegment>& TTab)
   throw(Exception);
bool startSort(const TTSegment& left, const TTSegment& right);
bool increasingMetricSort(const TTSegment& left, const TTSegment& right);
bool decreasingMetricSort(const TTSegment& left, const TTSegment& right);

//------------------------------------------------------------------------------------
// Find the entry in the timetable which applies to the baseline given in sdid and
// the time tt. Set the satellite in sdid to the reference satellite, and set the
// time tt to the time (in the future) when the reference will change again.
// return 0 on success, 1 on failure.
int QueryTimeTable(SDid& sdid, DayTime& tt) throw(Exception)
{
try {
      // loop over the timetable, looking for a match : baseline and time
   list<TTSegment>::iterator ttit;
   for(ttit=TimeTable.begin(); ttit != TimeTable.end(); ttit++) {
      if(((ttit->site1 == sdid.site1 && ttit->site2 == sdid.site2) ||
          (ttit->site1 == sdid.site2 && ttit->site2 == sdid.site1)   ) &&
         FirstEpoch+CI.DataInterval*ttit->first <= tt                  &&
         FirstEpoch+CI.DataInterval*ttit->last  >= tt)
      {                                                  // success
         sdid.sat = ttit->sat;
         tt = FirstEpoch+CI.DataInterval*ttit->last;
         return 0;
      }
   }

   return 1;      // failure
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}
    
//------------------------------------------------------------------------------------
// Find the start and stop counts in the timetable which applies to the given baseline
int QueryTimeTable(string baseline, int& beg, int& end) throw(Exception)
{
try {
   string site1=word(baseline,0,'-');
   string site2=word(baseline,1,'-');
   beg = end = -1;
      // loop over the timetable, looking for a match in baseline
   list<TTSegment>::iterator ttit;
   for(ttit=TimeTable.begin(); ttit != TimeTable.end(); ttit++) {
      if((ttit->site1 == site1 && ttit->site2 == site2) ||
         (ttit->site1 == site2 && ttit->site2 == site1)   )
      {                                                  // success
         if(beg == -1 || ttit->first < beg) beg = ttit->first;
         if(end == -1 || ttit->last  > end) end = ttit->last;
      }
   }
   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int Timetable(void) throw(Exception)
{
try {
   if(CI.Verbose) oflog << "BEGIN Timetable()"
      << " at total time " << fixed << setprecision(3)
      << double(clock()-totaltime)/double(CLOCKS_PER_SEC) << " seconds."
      << endl;

   int ib,iret;
   list<TTSegment>::iterator ttit;

   if(CI.TimeTableFile.size() > 0) {
      iret = ReadTimeTable();
   }
   else if(CI.RefSat.id != -1) {         // user says use this sat only
      // loop over baselines
      for(ib=0; ib<Baselines.size(); ib++) {
         TTSegment ts;
         ts.site1 = word(Baselines[ib],0,'-');
         ts.site2 = word(Baselines[ib],1,'-');
         ts.sat = CI.RefSat;
         ts.start = ts.first = 0;
         ts.end = ts.last = maxCount;
         ts.minelev = ts.maxelev = 0.0;
         ts.length = ts.end - ts.start + 1;
         TimeTable.push_back(ts);
         iret = 0;
      }
   }
   else {
      // loop over baselines
      for(ib=0; ib<Baselines.size(); ib++) {
         iret = ComputeBaselineTimeTable(Baselines[ib]);
         if(iret) break;
      }  // end loop over baselines
   }

   if(iret == 0) {
      // write out timetable to log
      // REF site site sat week use_first use_last data_start data_end
      DayTime tt;
      GSatID sat;
      oflog << "Here is the time table (" << TimeTable.size() << ")" << endl;
      if(CI.Screen)
         cout << "Time table (" << TimeTable.size() << "):" << endl;
      oflog << "# " << Title << endl;
      oflog << "# REF site site sat week use_first use_last data_start data_end\n";
      if(CI.Screen)
         cout << "# REF site site sat week use_first use_last data_start data_end\n";
      for(ttit=TimeTable.begin(); ttit != TimeTable.end(); ttit++) {
         oflog << "REF " << ttit->site1 << " " << ttit->site2 << " " << ttit->sat;
         if(CI.Screen)
            cout << "REF " << ttit->site1 << " " << ttit->site2 << " " << ttit->sat;
         tt = FirstEpoch + CI.DataInterval * ttit->first;
         oflog << tt.printf(" %4F %10.3g");        // TD week rollover!
         if(CI.Screen)
            cout << tt.printf(" %4F %10.3g");        // TD week rollover!
         tt = FirstEpoch + CI.DataInterval * ttit->last;
         oflog << tt.printf(" %10.3g");
         if(CI.Screen)
            cout << tt.printf(" %10.3g");
         tt = FirstEpoch + CI.DataInterval * ttit->start;
         oflog << tt.printf(" %10.3g");
         if(CI.Screen)
            cout << tt.printf(" %10.3g");
         tt = FirstEpoch + CI.DataInterval * ttit->end;
         oflog << tt.printf(" %10.3g");
         if(CI.Screen)
            cout << tt.printf(" %10.3g");
         // TD? ttit->minelev, ttit->maxelev, ttit->length, ttit->metric()
         oflog << " " << fixed << setw(4) << setprecision(1) << ttit->minelev;
         if(CI.Screen)
            cout << " " << fixed << setw(4) << setprecision(1) << ttit->minelev;
         oflog << " " << fixed << setw(4) << setprecision(1) << ttit->maxelev;
         if(CI.Screen)
            cout << " " << fixed << setw(4) << setprecision(1) << ttit->maxelev;
         // write the number of counts for this ref
         oflog << " " << setw(5) << ttit->length;
         if(CI.Screen)
            cout << " " << setw(5) << ttit->length;
         oflog << endl;
         if(CI.Screen)
            cout << endl;

         // for next time
         sat = ttit->sat;
      }
      oflog << "End of time table." << endl;
      if(CI.Screen)
         cout << "End of time table." << endl;
   }

   return iret;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}   // end Timetable()

//------------------------------------------------------------------------------------
// Input the time table from a file
int ReadTimeTable(void) throw(Exception)
{
try {
   int week;
   double sow;
   string line;
   DayTime tt;

   // open an input file for all timetables
   if(CI.Debug) oflog << "Try to open time table file " << CI.TimeTableFile << endl;
   ifstream ttifs(CI.TimeTableFile.c_str());
   if(!ttifs) {
      cerr << "Failed to open input time table file " << CI.TimeTableFile << endl;
      return -3;
   }

   //REF site site sat week use_first use_last data_start data_end
   do {
      getline(ttifs,line);
      stripTrailing(line,'\r');
      if(ttifs.eof() || !ttifs.good()) break;

      if(line.size() <= 0) continue;                              // skip blank lines
      if(line[0] == '#') continue;                                // skip comments
      if(numWords(line) < 9) continue; // TD msg?    // skip bad lines
      if(words(line,0,1) != string("REF")) continue; // only REF lines
      TTSegment ts;
      ts.site1 = words(line,1,1);
      ts.site2 = words(line,2,1);
      ts.sat.fromString(words(line,3,1));

      week = asInt(words(line,4,1));
      sow = asInt(words(line,5,1));
      tt.setGPSfullweek(week,sow);           // TD handle week rollover
      ts.first = int(0.5+(tt-FirstEpoch)/CI.DataInterval);

      sow = asInt(words(line,6,1));
      tt.setGPSfullweek(week,sow);
      ts.last = int(0.5+(tt-FirstEpoch)/CI.DataInterval);

      sow = asInt(words(line,7,1));
      tt.setGPSfullweek(week,sow);
      ts.start = int(0.5+(tt-FirstEpoch)/CI.DataInterval);

      sow = asInt(words(line,8,1));
      tt.setGPSfullweek(week,sow);
      ts.end = int(0.5+(tt-FirstEpoch)/CI.DataInterval);

      //ts.minelev = ts.maxelev = 0.0;
      ts.length = ts.end - ts.start + 1;
      ts.findElev();
      TimeTable.push_back(ts);

   } while(1);
   // close the output timetable file
   ttifs.close();

   oflog << "Read time table from file " << CI.TimeTableFile << endl;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int ComputeBaselineTimeTable(const string& bl) throw(Exception)
{
try {
   int i,j;
   map<SDid,SDData>::const_iterator it;
   list<TTSegment> SegList;

   SDmap.clear();
   i = TTComputeSingleDifferences(bl,40.0);
   if(i) return i;

   // now compute the timetable based on SDmap
   for(it=SDmap.begin(); it != SDmap.end(); it++) {
      TTSegment ts;
      ts.site1 = it->first.site1;
      ts.site2 = it->first.site2;
      ts.sat = it->first.sat;
      ts.start = it->second.count[0];
      ts.minelev = ts.maxelev = 0.0;
      for(i=0; i<it->second.count.size()-1; i++) {
         j = it->second.count.at(i+1) - it->second.count.at(i);
         if(j > 1) {
            TTSegment tts;
            tts = ts;
            tts.end = it->second.count.at(i);
            tts.length = tts.end - tts.start + 1;
            tts.findElev();
            SegList.push_back(tts);
            ts.start = it->second.count.at(i+1);
         }
      }
      ts.end = it->second.count[it->second.count.size()-1];
      ts.length = ts.end - ts.start + 1;
      ts.findElev();
      SegList.push_back(ts);
   }
   
   if(SegList.size() == 0) return -2;

   // figure out the time table from the list of segments
   list<TTSegment> TTable;
   i = TimeTableAlgorithm(SegList, TTable);
   if(i) return i;

   // add this timetable to the master timetable.
   list<TTSegment>::iterator ttit;
   for(ttit=TTable.begin(); ttit != TTable.end(); ttit++)
      TimeTable.push_back(*ttit);

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int TTComputeSingleDifferences(const string& bl, const double ElevLimit)
   throw(Exception)
{
try {
   int i,j,k;
   double elevi,elevj;
   GSatID sat;
   map<GSatID,RawData>::const_iterator it,jt;
   map<SDid,SDData>::const_iterator kt;
   ofstream rawofs;
   format f62(6,2),f133(13,3);

   // loop over buffered raw data of sats common to both
   string est=word(bl,0,'-');
   string fix=word(bl,1,'-');

   for(it=Stations[est].RawDataBuffers.begin();
       it != Stations[est].RawDataBuffers.end(); it++) {

      // consider each satellite
      sat = it->first;
      if(CI.Verbose) oflog << "Single difference " << est << " " << fix << " " << sat;

      // is sat also found at fixed site?
      jt = Stations[fix].RawDataBuffers.find(sat);
      if(jt == Stations[fix].RawDataBuffers.end()) {
         if(CI.Verbose) oflog << " not found on both sites" << endl;
         continue;
      }

      if(CI.Verbose) oflog << " (raw buffers size: " << it->second.count.size()
         << " " << jt->second.count.size() << ")";

      // compute continuous segments of SD data
      // sdd.count is the intersection of the two count vectors
      SDid sdid(fix,est,sat);
      SDData sdd;
      sdd.elevmin = 100.0;
      sdd.elevmax = -1.0;
      i = j = 0;
      do {
         if(it->second.count[i] == jt->second.count[j]) {
            elevi = RotatedAntennaElevation(it->second.elev[i],it->second.az[i]);
            elevj = RotatedAntennaElevation(jt->second.elev[j],jt->second.az[j]);
            if(elevi >= ElevLimit && elevj >= ElevLimit) {
               sdd.count.push_back(it->second.count[i]);
               if(elevi < sdd.elevmin) sdd.elevmin = elevi;
               if(elevi > sdd.elevmax) sdd.elevmax = elevi;
            }
            i++;
            j++;
         }
         else if(it->second.count[i] < jt->second.count[j]) {
            i++;
         }
         else {
            j++;
         }
      } while( i < it->second.count.size() &&
               j < jt->second.count.size() );

      // TD ?
      if(sdd.count.size() < 10) {
         if(CI.Verbose) oflog << " size is too small ("
            << sdd.count.size() << ")" << endl;
         continue;
      }

      // save it in the map
      SDmap[sdid] = sdd;

      if(CI.Verbose) oflog << endl;

   }  // end loop over raw buffered data common to both sites

   // write out a summary of single differences
   oflog << "Single differences summary :" << endl;
   for(k=1,kt=SDmap.begin(); kt != SDmap.end(); kt++,k++) {
      oflog << " " << setw(2) << k << " " << kt->first
         << " " << setw(5) << kt->second.count.size()
         << " " << setw(5) << kt->second.count.at(0)
         << " - " << setw(5) << kt->second.count.at(kt->second.count.size()-1);
         // elevation
      oflog << " elev: "
         << fixed << setw(4) << setprecision(1) << kt->second.elevmin
         << " - " << setw(4) << setprecision(1) << kt->second.elevmax;
         // gaps - (count : number of pts)
      for(i=0; i<kt->second.count.size()-1; i++) {
         j = kt->second.count.at(i+1) - kt->second.count.at(i);
         if(j > 1)
            oflog << " (" << kt->second.count.at(i)+1 << ":" << j-1 << ")";
      }
      oflog << endl;
   }

   if(SDmap.size() == 0) {
      oflog << "Returning error code -1 from TTComputeSingleDifferences()" << endl;
      return -1;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// preprocess the segment list - get first and last counts
int TimeTableAlgorithm(list<TTSegment>& TTS, list<TTSegment>& TTab) throw(Exception)
{
try {
   bool keep;
   int i,beg,end;
   int begcount,endcount;
   list<TTSegment>::iterator ttit,ttjt;
   typedef pair<int,int> seg;
   list<seg> Segs;
   list<seg>::iterator lit,ljt;

   // 0 sort list in order of decreasing metric()
   // 0.5 delete very small segments
   // 0.6 run through the list, finding the smallest begin, and largest end, counts;
   // call these the begin and end counts.
   // 1. run through the sorted list, starting at largest metric(), and pick out
   // first segments which have the begin and/or end counts; add these to TTab
   // and erase from TTS.
   // 2. run through TTS again, starting at the largest metric(); if a segment
   // includes some counts that have not been covered before, than add this
   // to TTab and erase from TTS. Quit when either the entire range from
   // begcount to endcount is covered, a given minimum metric() is reached,
   // or when the end of TTS is reached.
   // 3. if gaps remain in the coverage, these are real gaps in the data and
   // the estimation will have to reset.
   // 4. sort TTab in increasing order. Run through TTab looking for
   // segments which can be removed without generating gaps; remove these.

   // 0. sort in reverse order (largest metric() first)
   // [ list has its own sort sort(TTS.rbegin(),TTS.rend()); ]
   TTS.sort(decreasingMetricSort);

   // 0.5 delete very small segments and output the sorted list
   // 0.6 find begcount and endcount (after deletion)
   begcount = endcount = -1;
   oflog << "Here is the sorted list of segments:" << endl;
   for(i=1,ttit=TTS.begin(); ttit != TTS.end(); i++) {
      oflog << " " << setw(4) << i << *ttit;
      if(ttit->length < 10) {
         oflog << " -- delete this segment: too small";
         ttit = TTS.erase(ttit); // ttit now points to next seg
      }
      else {
         if(begcount < 0 || ttit->start < begcount)
            begcount = ttit->start;
         if(endcount < 0 || ttit->end > endcount)
            endcount = ttit->end;
         ttit++;
      }
      oflog << endl;
   }
   oflog << "End the sorted list; limits : " << begcount << " - " << endcount << endl;

   for(ttit=TTS.begin(); ttit != TTS.end(); ttit++) {
      if(ttit->length < 10) {
      }
   }
   
   // 1.find the begin point
   for(ttit=TTS.begin(); ttit != TTS.end(); ttit++) {
      if(ttit->start == begcount) {
         oflog << "Found the begin time: " << *ttit << endl;
         TTab.push_back(*ttit);
         beg = ttit->end;
         TTS.erase(ttit);
         break;
      }
   }

   if(beg == endcount) {
      // one segment covers it all - done!
      oflog << "One segment covers it all!" << endl;
      end = endcount;
   }
   else {
      // find the end point
      for(ttit=TTS.begin(); ttit != TTS.end(); ttit++) {
         if(ttit->end == endcount) {
            oflog << "Found the   end time: " << *ttit << endl;
            TTab.push_back(*ttit);
            end = ttit->start;
            TTS.erase(ttit);
            break;
         }
      }

      if(TTab.size() != 2) {
         // error, we didn't find the beg and end -- throw?
         return -2;
      }
   }

   // start list of segs with the ones that contain endpoints
   ttit = TTab.begin();
   Segs.push_back(make_pair(ttit->start,ttit->end));
   ttit++;
   if(ttit != TTab.end())
      Segs.push_back(make_pair(ttit->start,ttit->end));

   if(beg >= end) { // two segments cover it all
      if(Segs.size() > 1) {            // TD unsure if the logic is wrong here...
         ljt = lit = Segs.begin();
         ljt++;
         lit->second = ljt->second;
         if(CI.Debug) oflog << "Two segments cover it all: erase seg ("
            << ljt->first << "-" << ljt->second << ")" << endl;
         Segs.erase(ljt);
      }
   }
   else {
      // 2.
      // start with 
      // |======1st Seg======|            gap                 |=====last Seg======|
      //
      // in general, new segs can be added, so Segs looks like this:
      // |===1st Seg===|    gap       |===2nd Seg===|    gap       |===last Seg===|
      //
      // consider 8 cases of new segments from TTS:
      // 1.   |--------------|                               covers end of seg
      // 2.               |-------|                          lies entirely w/in gap
      // 3.                     |--------|                   covers beg of seg
      // 4.         |--------------------|                   covers entire gap
      // 5.                      |----------------------|    covers entire seg
      // 6. covers seg and gap   |-------------------------------------|
      // 7.         |-------------------------------------|  covers gap and seg
      // 8.                               |-----|            lies entirely w/in seg
      //
      // Here is the algorithm:
      // for each new segment extending from b to e
      // for each i in list of segments in list (extending bi to ei) {
      // if(b > ei) skip to the next i
      // if(e > ei) {                              // b <= ei and e > ei
      //    mod seg i so that ei -> e              // case 1,4,7
      //    if(b < bi) mod seg i so that bi -> b   // case 5,6
      //    while (e >= bj) where j > i,
      //       merge segments i and j and delete j // case 4,6,7
      //    keep this segment
      // }
      // else {                                    // b <= ei and e <= ei
      //    if(e >= bi) {
      //       if(b < bi) {
      //          mod segment i so bi -> b         // case 3
      //          keep this segment
      //       }
      //       //else { ignore this segment }      // case 8
      //    }
      //    else {
      //       make a new segment (b,e),
      //          and insert it before segment i   // case 2
      //       keep this segment
      //    }
      // }
      // } // end of loop over segments i
      // if(keep) add this segment to the time table
      // 
      // loop over all segments, in decreasing order of metric()
      for(i=1,ttit=TTS.begin(); ttit != TTS.end(); i++,ttit++) { // i temp

         if(CI.Debug) { // temp
            oflog << "Here is the current time table (" << TTab.size() << ")"
               << endl;
            for(ttjt=TTab.begin(); ttjt != TTab.end(); ttjt++)
               oflog << " " << *ttjt << endl;
         }

         if(CI.Debug) {
            oflog << "and here is the seg list";
            for(lit=Segs.begin(); lit != Segs.end(); lit++)
               oflog << " (" << lit->first << "-" << lit->second << ")";
            oflog << endl;
         }

         // done if one segment covers all
         lit = Segs.begin();
         if(Segs.size() == 1 && lit->first == begcount && lit->second == endcount)
            break;

         // keep this? you don't want metric to become very small -> failure
         if(ttit->metric() <= 100.0) break;        // TD input param

         beg = ttit->start;
         end = ttit->end;
         if(CI.Debug) oflog << "consider new segment ("
            << beg << "-" << end << ")" << endl;

         // loop over the segs
         keep = false;
         lit = Segs.begin();
         while(lit != Segs.end()) {
            if(beg > lit->second) {
               if(CI.Debug) oflog << " skip seg ("
                  << lit->first << "-" << lit->second << ")" << endl;
               lit++;
               continue;
            }
            if(end > lit->second) {
               if(CI.Debug) oflog << " mod 1 seg ("
                  << lit->first << "-" << lit->second << ")";
               lit->second = end;
               if(beg < lit->first) lit->first=beg;
               if(CI.Debug) oflog << " to ("
                  << lit->first << "-" << lit->second << ")" << endl;
               ljt = lit;
               while(++ljt != Segs.end() && end >= ljt->first) {
                  // merge i and j
                  if(CI.Debug) oflog << " merge segs ("
                     << lit->first << "-" << lit->second << ") and ("
                     << ljt->first << "-" << ljt->second << ")";
                  lit->second = ljt->second;
                  if(CI.Debug) oflog << " and erase seg ("
                     << ljt->first << "-" << ljt->second << ")" << endl;
                  Segs.erase(ljt);
                  ljt = lit;
               }
               keep = true;
            }
            else {
               if(end >= lit->first) {
                  if(beg < lit->first) {
                     if(CI.Debug) oflog << " mod 2 seg ("
                        << lit->first << "-" << lit->second << ")";
                     lit->first = beg;
                     keep = true;
                     if(CI.Debug) oflog << " to ("
                        << lit->first << "-" << lit->second << ")" << endl;
                  }
                  //else { keep=false; ignore -- this seg has nothing new }
               }
               else {
                  seg newseg(beg,end);
                  if(CI.Debug) oflog << " add seg ("
                     << newseg.first << "-" << newseg.second << ")" << endl;
                  Segs.insert(lit,newseg);
                  keep = true;
               }
            }
            break;
         }  // end while loop over segs

         if(keep) {
            TTab.push_back(*ttit);
            TTab.sort(startSort);       // temp
         }

         if(CI.Debug) if(i > 100) break;      // temp

      }  // end for loop over segments TTS

   }  // end if(initial gap is non-zero)

   // 3. are there gaps?
   if(Segs.size() != 1) {
      oflog << "There are real gaps in the data; segments with data:" << endl;
      for(lit=Segs.begin(); lit != Segs.end(); lit++)
         oflog << " (" << lit->first << "-" << lit->second << ")";
      oflog << endl;
   }
   else oflog << "There are no gaps in the data" << endl;

   // sort the timetable
   TTab.sort(startSort);

   // TD 4. edit TTab, removing segments that do not create gaps

   // decide on actual transition times
   for(ttit=TTab.begin(); ttit != TTab.end(); ttit++) {
      if(CI.Verbose) oflog << " " << *ttit << endl;

      // compute the count at which to switch
      if(ttit == TTab.begin()) {
         ttit->first = ttit->start;       // first = start for first segment
         ttjt = ttit;                     // initialize ttjt
      }
      else {
         // switch at the mid-point of overlap
         ttit->first = (ttjt->end + ttit->start)/2;
      }
      ttit->last = ttit->end;             // change later, except for last segment
      if(ttit != TTab.begin()) {
         ttjt->last = ttit->first;        // count at the switch point
         ttjt++;                          // ttjt lags behind ttit by 1
      }
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void TTSegment::findElev(void) throw(Exception)
{
   int i,k;
   double elevi;
   RawData& rd=Stations[site1].RawDataBuffers[sat];
   minelev = 99.0;
   maxelev = -1.0;
   k = index(rd.count,start);
   if(k == -1) return;
   for(i=k; i<k+length; i++) {
      elevi = RotatedAntennaElevation(rd.elev.at(i),rd.az.at(i));
      if(elevi > maxelev) maxelev = elevi;
      if(elevi < minelev) minelev = elevi;
   }
}

//------------------------------------------------------------------------------------
// friends of TTSegment, defined there
bool startSort(const TTSegment& left, const TTSegment& right)
{ return (left.start < right.start); }

bool increasingMetricSort(const TTSegment& left, const TTSegment& right)
{ return (left.metric() < right.metric()); }

bool decreasingMetricSort(const TTSegment& left, const TTSegment& right)
{ return (left.metric() > right.metric()); }

//------------------------------------------------------------------------------------
ostream& operator<<(ostream& os, const TTSegment& t) throw(Exception)
{
try {
   os << " " << t.site1
      << " " << t.site2
      << " " << t.sat
      << " " << setw(5) << t.length
      << " " << setw(5) << t.start
      << " - " << setw(5) << t.end
      //<< " (" << t.first << "-" << t.last << ")"
      << " " << fixed << setw(4) << setprecision(1) << t.minelev
      << " - " << fixed << setw(4) << setprecision(1) << t.maxelev
      << " " << setw(7) << setprecision(2) << t.metric();
   return os;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
