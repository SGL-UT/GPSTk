/// @file SatPassIterator.cpp
/// Iterate over a vector of SatPass in time order.

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

#include "SatPassIterator.hpp"
#include "logstream.hpp"

using namespace std;
using namespace gpstk::StringUtils;
namespace gpstk {

// -------------------------------------------------------------------------------
// only constructor
SatPassIterator::SatPassIterator(vector<SatPass>& splist, bool rev, bool dbug)
   throw(Exception) : SPList(splist), timeReverse(rev), debug(dbug)
{
   if(SPList.size() == 0) {
      Exception e("Empty list");
      GPSTK_THROW(e);
   }

   int i,j;

   // ensure time order
   sort(SPList);

   // copy the list of obs types, and check that each is registered
   vector<string> otlist;
   for(i=0; i<SPList[0].labelForIndex.size(); i++) {
      otlist.push_back(SPList[0].labelForIndex[i]);
      if(RinexObsHeader::convertObsType(SPList[0].labelForIndex[i])
            == RinexObsHeader::UN)
      {
         Exception e("Unregistered observation type : " + SPList[0].labelForIndex[i]);
         GPSTK_THROW(e);
      }
   }

   // copy the data from the first SatPass in the list, for comparison with the rest
   DT = SPList[0].dt;
   FirstTime = SPList[0].getFirstTime();
   LastTime = SPList[0].getLastTime();

   // loop over the list
   for(i=0; i<SPList.size(); i++) {
      // check for consistency of dt
      if(SPList[i].dt != DT) {
         Exception e("Inconsistent time intervals: " + asString(SPList[i].dt)
            + " != " + asString(DT));
         GPSTK_THROW(e);
      }

      // find the earliest and latest time
      if(SPList[i].getFirstTime() < FirstTime) FirstTime = SPList[i].getFirstTime();
      if(SPList[i].getLastTime() > LastTime) LastTime = SPList[i].getLastTime();

   }  // end loop over the list

   reset(timeReverse,debug);
}

// -------------------------------------------------------------------------------
// restart the iteration
void SatPassIterator::reset(bool rev, bool dbug) throw()
{
   timeReverse = rev;
   debug = dbug;
   // clear out the old
   currentN = 0;
   listIndex.clear();
   dataIndex.clear();
   countOffset.clear();
   indexStatus = vector<int>(SPList.size(),-1);
   // loop over the list
   int i = (timeReverse ? SPList.size()-1 : 0);
   while((timeReverse && i >= 0) || (!timeReverse && i<SPList.size())) {

      for(;;) {
         // ignore passes with negative Status
         if(SPList[i].Status < 0) break;

         // define latest epoch when time reversed
         if(timeReverse && currentN == 0)
            currentN = int((SPList[i].firstTime - FirstTime)/DT + 0.5)
                                 + SPList[i].spdvector[SPList[i].size()-1].ndt;

         // (re)build the maps
         if(listIndex.find(SPList[i].sat) == listIndex.end()) {
            indexStatus[i] = 0;
            listIndex[SPList[i].sat] = i;
            dataIndex[SPList[i].sat] = (timeReverse ? SPList[i].size()-1 : 0);
            countOffset[SPList[i].sat]
               = int((SPList[i].firstTime - FirstTime)/DT + 0.5);
            LOG(DEBUG4) << "reset - define map " << i <<" for sat " << SPList[i].sat
               << " at time " << SPList[i].firstTime.printf("%4F %10.3g")
               << " offset " << countOffset[SPList[i].sat];
         }
         else {
            indexStatus[i] = -1;
            LOG(DEBUG4)<< "reset - turn off pass "<< i <<" for sat " << SPList[i].sat
               << " at time " << SPList[i].firstTime.printf("%4F %10.3g");
         }

         break;      // mandatory
      }

      if(timeReverse) i--; else i++;
   }  // end loop over the list

}

// -------------------------------------------------------------------------------
// return 1 for success, 0 at end of data
// Access (all of) the data for the next epoch. As long as this function
// returns zero, there is more data to be accessed. Ignore passes with
// Status less than zero.
// @param indexMap  map<unsigned int, unsigned int> defined so that all the
//                  data in the current iteration is found at
//                  SatPassList[i].data(j) where indexMap[i] = j.
// @throw if time tags are out of order.
int SatPassIterator::next(map<unsigned int, unsigned int>& indexMap) throw(Exception)
{
   int i,j,k,numsvs;
   RinexSatID sat;

   numsvs = 0;
   indexMap.clear();
   nextIndexMap.clear();

   if(debug) LOG(INFO) << "SPIterator::next(map) - time "
      << (FirstTime+currentN*DT).printf("%4F %10.3g")
      << " size of listIndex " << listIndex.size();

   while(numsvs == 0) {
      if(listIndex.size() == 0) {
         if(debug) LOG(INFO) << "Return 0 from next()";
         return 0;
      }

      // loop over active SatPass's
      map<RinexSatID,int>::iterator kt;

      // debugging - dump the listIndex

      if(debug) for(kt = listIndex.begin(); kt != listIndex.end(); kt++)
         LOG(INFO) << "   listIndex: " << kt->first << " " << kt->second;

      kt = listIndex.begin();
      while(kt != listIndex.end()) {
         sat = kt->first;
         i = kt->second;
         j = dataIndex[sat];
         if(debug) LOG(INFO) << "Loop over listIndex: " << sat << " " << i <<" "<< j;

         if(SPList[i].Status < 0) {
            listIndex.erase(kt++);  // erasing a map - do exactly this and no more
            if(debug) LOG(INFO) << " Erase this pass for bad status: index "
               << i << " sat " << sat << " size is now " << listIndex.size();
            continue;
         }

         if(countOffset[sat] + SPList[i].spdvector[j].ndt == currentN) {
            // found active sat at this count - add to map
            nextIndexMap[i] = j;
            numsvs++;
            if(debug) LOG(INFO) << "SPIterator::next(map) found sat " << sat
               << " at index " << i;

            // increment data index
            if((timeReverse && --j < 0) ||
               (!timeReverse && ++j == SPList[i].spdvector.size()))
            {
               if(debug) LOG(INFO) << " This pass for sat " << sat << " is done ...";
               indexStatus[i] = 1;

               // find the next pass for this sat
               k = i + (timeReverse ? -1 : 1);
               while((timeReverse && k>=0) || (!timeReverse && k<SPList.size()))
               //for(k=i+1; k<SPList.size(); k++)
               {
                  if(debug) LOG(INFO) << " ... consider next pass " << k
                     << " " << SPList[k].sat
                     << " " << SPList[k].firstTime.printf("%4F %10.3g");
                  bool found(false);
                  for(;;) {
                     if(SPList[k].Status < 0)      // bad pass
                        break;
                     if(SPList[k].sat != sat)      // wrong sat
                        break;
                     if(indexStatus[k] > 0)        // already done
                        break;

                     // take this one
                     indexStatus[k] = 0;
                     i = listIndex[sat] = k;
                     dataIndex[sat] = (timeReverse ? SPList[i].size()-1 : 0);
                     countOffset[sat] = int((SPList[i].firstTime-FirstTime)/DT + 0.5);
                     found = true;
                     break;   // mandatory
                  }
                  if(found) break;
                  if(timeReverse) k--; else k++;
               }  // end while loop over next passes

               if(indexStatus[i] == 0) {
                  if(debug) LOG(INFO) << " ... new pass for sat " << SPList[i].sat
                  << " at index " << i << " and time "
                  << SPList[i].firstTime.printf("%4F %10.3g");
               }
            }
            else {
               dataIndex[sat] = j;
            }

         }  // end if found active sat at this count

         // increment the iterator
         if(indexStatus[i] > 0) {                // a new one was not found
            listIndex.erase(kt++);  // erasing a map - do exactly this and no more
            if(debug) LOG(INFO) << " Erase this pass: index " << i << " sat " << sat
               << " size is now " << listIndex.size();
         }
         else kt++;

      }  // end while loop over active SatPass's
      if(debug) LOG(INFO) << "End while loop over active SatPasses";

      //if(robs.numSvs == 0) cout << "Gap at " << currentN << endl;
      if(timeReverse) currentN--; else currentN++;

   }  // end while robs.numSvs==0

   indexMap = nextIndexMap;
   if(debug) LOG(INFO) << "Return 1 from next()";

   return 1;
}

// -------------------------------------------------------------------------------
// return 1 for success, 0 at end of data
// NB This assumes all the passes have the same obstypes in the same order, AND
//   it knows nothing of the obstypes in the header....
// TD perhaps better design is to pass this (SatPassIterator) a vector of obstypes
//   from the header and have it fill the robs parallel to that, inserting 0 as nec.
int SatPassIterator::next(RinexObsData& robs) throw(Exception)
{
   if(listIndex.size() == 0) return 0;

   map<unsigned int, unsigned int> indexMap;
   map<unsigned int, unsigned int>::const_iterator kt;
   int iret = next(indexMap);
   if(iret == 0) return iret;

   robs.obs.clear();
   robs.epochFlag = 0;
   robs.clockOffset = 0.0;
   robs.numSvs = 0;

   // get the time tag.
   // NB there is an assumption here, that all that SatPass'es in indexMap are
   // consistent w.r.t. time tag - clearly ok if SPList was created in the usual ways.
   kt = indexMap.begin();
   robs.time = SPList[kt->first].time(kt->second);

   // loop over the map
   for(kt = indexMap.begin(); kt != indexMap.end(); kt++) {
      int i = kt->first;
      int j = kt->second;
      RinexSatID sat = SPList[i].getSat();
      //LOG(DEBUG2) << "SPIterator::next(robs) found sat " << sat
      //   << " at index " << i << " and time " << SPList[i].time(j);

      bool found = false;
      bool flag = (SPList[i].spdvector[j].flag != SatPass::BAD);
      for(int k=0; k<SPList[i].labelForIndex.size(); k++) {
         RinexObsType ot;
         ot = RinexObsHeader::convertObsType(SPList[i].labelForIndex[k]);
         if(ot == RinexObsHeader::UN) {
            ; //LOG(DEBUG1) << " Error - this sat has UN obstype"; // TD warn?
         }
         else {
            found = true;
            // NO some obs may be zero b/c they are not collected (e.g. C2) -> bad
            //robs.obs[sat][ot].data = flag ? SPList[i].spdvector[j].data[k] : 0.;
            //robs.obs[sat][ot].lli  = flag ? SPList[i].spdvector[j].lli[k] : 0;
            //robs.obs[sat][ot].ssi  = flag ? SPList[i].spdvector[j].ssi[k] : 0;
            robs.obs[sat][ot].data = SPList[i].spdvector[j].data[k];
            robs.obs[sat][ot].lli  = SPList[i].spdvector[j].lli[k];
            robs.obs[sat][ot].ssi  = SPList[i].spdvector[j].ssi[k];
         }
      }
      if(found) robs.numSvs++;
   }

   return 1;
}

}  // end namespace

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
