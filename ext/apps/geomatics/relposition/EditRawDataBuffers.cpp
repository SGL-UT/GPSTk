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
 * @file EditRawDataBuffers.cpp
 * Edit raw data in the buffers, part of program DDBase.
 */

//------------------------------------------------------------------------------------
// system includes

// GPSTk

// DDBase
#include "DDBase.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// prototypes -- this module only
int OutputRawData(void) throw(Exception);                     // DataOutput.cpp

//------------------------------------------------------------------------------------
int EditRawDataBuffers(void) throw(Exception)
{
try {
   size_t i;
   map<string,Station>::iterator kt;
   map<GSatID,RawData>::iterator it;

   if(CI.Verbose) oflog << "BEGIN EditRawDataBuffers()"
      << " at total time " << fixed << setprecision(3)
      << double(clock()-totaltime)/double(CLOCKS_PER_SEC) << " seconds."
      << endl;

   // find the largest value of Count seen in the raw data (same will be done for DD)
   maxCount = 0;
   for(kt=Stations.begin(); kt != Stations.end(); kt++) {
      vector<GSatID> Emptys;
      Station& st = kt->second;

      // first find and remove empty RawData's
      for(it=st.RawDataBuffers.begin(); it != st.RawDataBuffers.end(); it++) {
         if(it->second.elev.size() == 0)
            Emptys.push_back(it->first);
         else { // find the max count
            if(it->second.count.size() > 0 &&
               it->second.count[int(it->second.count.size())-1] > maxCount)
                  maxCount = it->second.count[int(it->second.count.size())-1];
         }
      }
         // remove empty buffers
      for(i=0; i<Emptys.size(); i++)
         st.RawDataBuffers.erase(Emptys[i]);    // erase map

         // remove isolated points (single points with gaps > CI.MaxGap on both sides
      for(it=st.RawDataBuffers.begin(); it != st.RawDataBuffers.end(); it++) {
         RawData& rd=it->second;
         vector<int>::iterator cit;
         vector<double>::iterator ditL1=rd.L1.begin();
         vector<double>::iterator ditL2=rd.L2.begin();
         vector<double>::iterator ditP1=rd.P1.begin();
         vector<double>::iterator ditP2=rd.P2.begin();
         vector<double>::iterator ditS1=rd.S1.begin();
         vector<double>::iterator ditS2=rd.S2.begin();
         vector<double>::iterator ditER=rd.ER.begin();
         vector<double>::iterator ditEL=rd.elev.begin();
         vector<double>::iterator ditAZ=rd.az.begin();
         cit = rd.count.begin();
         while(cit != rd.count.end()) {
            if(rd.count.size() == 1 ||       // single point
                                             // or isolated point at begin
               (cit == rd.count.begin() && *(cit+1) - *cit > CI.MaxGap) ||
                                             // or isolated point at end
               (cit+1 == rd.count.end() && *cit - *(cit-1) > CI.MaxGap) ||
                                             // or isolated point not at either end
               (cit+1 != rd.count.end() && cit != rd.count.begin() &&
                  *(cit+1) - *cit > CI.MaxGap && *cit - *(cit-1) > CI.MaxGap))
            {
               if(CI.Debug) {
                  oflog << "Found isolated point with ";
                  if(cit != rd.count.begin())
                     oflog << *cit - *(cit-1) << " pt gap before and ";
                  else
                     oflog << "begin pt before and ";
                  if(cit+1 != rd.count.end())
                     oflog << *(cit+1) - *cit << " pt gap after, ";
                  else
                     oflog << "end pt after, ";
                  oflog << "at " << *cit << endl;
               }
               cit = rd.count.erase(cit);    // cit now pts to the following element
               ditL1 = rd.L1.erase(ditL1);
               ditL2 = rd.L2.erase(ditL2);
               ditP1 = rd.P1.erase(ditP1);
               ditP2 = rd.P2.erase(ditP2);
               ditS1 = rd.S1.erase(ditS1);
               ditS2 = rd.S2.erase(ditS2);
               ditER = rd.ER.erase(ditER);
               ditEL = rd.elev.erase(ditEL);
               ditAZ = rd.az.erase(ditAZ);
            }
            else {
               cit++;
               ditL1++;
               ditL2++;
               ditP1++;
               ditP2++;
               ditS1++;
               ditS2++;
               ditER++;
               ditEL++;
               ditAZ++;
            }
         }
      }

         // find the largest value of count
      for(it=st.RawDataBuffers.begin(); it != st.RawDataBuffers.end(); it++) {
         if(it->second.count.size() > 0 &&
            it->second.count[int(it->second.count.size())-1] > maxCount)
               maxCount = it->second.count[int(it->second.count.size())-1];
      }
   }

   if(maxCount <= 0) {
      oflog << "..No raw data found after EditRawDataBuffers()! Abort." << endl;
      return 1;
   }
   if(CI.Verbose) oflog << "Max count is " << maxCount << endl;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int OutputRawDataBuffers(void) throw(Exception)
{
try {
   int j,k;
   size_t i;
   map<string,Station>::iterator kt;
   map<GSatID,RawData>::const_iterator it;

   if(CI.Verbose) oflog << "BEGIN OutputRawDataBuffers()"
      << " at total time " << fixed << setprecision(3)
      << double(clock()-totaltime)/double(CLOCKS_PER_SEC) << " seconds."
      << endl;

   oflog << "Raw buffered data summary : n SITE sat npts span (count,gap size) (..)"
      << endl;
   if(CI.Screen)
      cout << "Raw buffered data summary : n SITE sat npts span (count,gap size) (..)"
      << endl;
   //CommonTime tt;
   for(kt=Stations.begin(); kt != Stations.end(); kt++) {
      Station& st = kt->second;
      for(k=1,it=st.RawDataBuffers.begin(); it != st.RawDataBuffers.end(); it++,k++) {
         oflog << " " << setw(2) << k << " " << kt->first
            << " " << it->first
            << " " << setw(5) << it->second.count.size();
         if(CI.Screen)
            cout << " " << setw(2) << k << " " << kt->first
            << " " << it->first
            << " " << setw(5) << it->second.count.size();
         //tt = FirstEpoch + it->second.count[0]*CI.DataInterval;
         //oflog << " " << tt;
         //tt += double(CI.DataInterval*it->second.count[it->second.count.size()-1]);
         //oflog << " " << tt;
         if(it->second.count.size() > 0) {
            oflog << " " << setw(5) << it->second.count[0]
                  << " - " << setw(5) << it->second.count[it->second.count.size()-1];
            if(CI.Screen)
               cout << " " << setw(5) << it->second.count[0]
                  << " - " << setw(5) << it->second.count[it->second.count.size()-1];
            for(i=0; i<it->second.count.size()-1; i++) {
               j = it->second.count.at(i+1) - it->second.count.at(i);
               if(j > 1) {
                  oflog << " (" << it->second.count.at(i)+1 << ":" << j-1 << ")";
                  if(CI.Screen)
                     cout << " (" << it->second.count.at(i)+1 << ":" << j-1 << ")";
               }
            }
         }
         oflog << endl;
         if(CI.Screen)
            cout << endl;
      }
   }

      // output the raw data itself
   // move to after Synchronize() OutputRawData();

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(std::exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
