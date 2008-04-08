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

/**
 * @file SatPass.cpp
 * Data for one complete satellite pass overhead.
 */

//------------------------------------------------------------------------------------
// system
#include <ostream>
#include <string>
#include <vector>
#include <algorithm>
// gpstk
#include "SatPass.hpp"
#include "icd_200_constants.hpp"    // OSC_FREQ,L1_MULT,L2_MULT
#include "Stats.hpp"
#include "StringUtils.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"
#include "RinexUtilities.hpp"
//#include "logstream.hpp"

using namespace std;
using namespace gpstk::StringUtils;

//------------------------------------------------------------------------------------
namespace gpstk {

// ------------------ configuration --------------------------------
// note that flag & LL1 = true for all L1 discontinuities
//           flag & LL2 = true for all L2 discontinuities
const unsigned short SatPass::OK  = 1; // good data, no discontinuity
const unsigned short SatPass::BAD = 0; // used by caller to mark bad data
const unsigned short SatPass::LL1 = 2; // discontinuity on L1 only
const unsigned short SatPass::LL2 = 4; // discontinuity on L2 only
const unsigned short SatPass::LL3 = 6; // discontinuity on L1 and L2
double SatPass::maxGap = 1800;         // maximum gap (seconds) allowed within pass
string SatPass::outFormat = string("%4F %10.3g");  // GPS week, seconds of week

// constructors
SatPass::SatPass(GSatID insat, double indt) throw()
{
   vector<string> defaultObsTypes;
   defaultObsTypes.push_back("L1");
   defaultObsTypes.push_back("L2");
   defaultObsTypes.push_back("P1");
   defaultObsTypes.push_back("P2");

   init(insat, indt, defaultObsTypes);
}

SatPass::SatPass(GSatID insat, double indt, vector<string> obstypes) throw()
{
   init(insat, indt, obstypes);
}

void SatPass::init(GSatID insat, double indt, vector<string> obstypes) throw()
{
   sat = insat;
   dt = indt;
   //firstTime = DayTime::BEGINNING_OF_TIME;
   //lastTime = DayTime::END_OF_TIME;
   ngood = 0;
   Status = 0;

   for(int i=0; i<obstypes.size(); i++) {
      indexForLabel[obstypes[i]] = i;
      labelForIndex[i] = obstypes[i];
   }
}

int SatPass::addData(const DayTime tt, vector<string>& ots, vector<double>& data)
   throw(Exception)
{
   vector<unsigned short> lli(data.size(),0),ssi(data.size(),0);
   try { return addData(tt, ots, data, lli, ssi); }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// return -2 time tag out of order, data not added
//        -1 gap is larger than MaxGap, data not added
//       >=0 (success) index of the added data
int SatPass::addData(const DayTime tt,
                         vector<string>& obstypes,
                         vector<double>& data,
                         vector<unsigned short>& lli,
                         vector<unsigned short>& ssi,
                         unsigned short flag) throw(Exception)
{
   // check that data, lli and ssi have the same length - throw
   if(data.size() != lli.size() || data.size() != ssi.size()) {
      Exception e("Dimensions do not match in addData()"
                  + StringUtils::asString(data.size()) + ","
                  + StringUtils::asString(lli.size()) + ","
                  + StringUtils::asString(ssi.size()));
      GPSTK_THROW(e);
   }
   if(spdvector.size() > 0 && spdvector[0].data.size() != data.size()) {
      Exception e("Error - addData passed different dimension that earlier!"
                   + StringUtils::asString(data.size()) + " != "
                   + StringUtils::asString(spdvector[0].data.size()));
      GPSTK_THROW(e);
   }

   // create a new SatPassData
   SatPassData spd(data.size());
   spd.flag = flag;
   for(int k=0; k<data.size(); k++) {
      int i = indexForLabel[obstypes[k]];
      spd.data[i] = data[k];
      spd.lli[i] = lli[k];
      spd.ssi[i] = ssi[k];
   }

   // push_back defines count and
   // returns : >=0 index of added data (ok), -1 gap, -2 tt out of order
   return push_back(tt, spd);
}

// return -3 sat not found, data not added
//        -2 time tag out of order, data not added
//        -1 gap is larger than MaxGap, data not added
//       >=0 (success) index of the added data
int SatPass::addData(const RinexObsData& robs) throw()
{
   if(robs.epochFlag != 0 && robs.epochFlag != 1) return false;
   RinexObsData::RinexSatMap::const_iterator it;
   RinexObsData::RinexObsTypeMap::const_iterator jt;
   map<string,unsigned int>::const_iterator kt;
   SatPassData spd(indexForLabel.size());

   // loop over satellites
   for(it=robs.obs.begin(); it != robs.obs.end(); it++) {
      if(it->first == sat) {
         // loop over obs
         for(kt=indexForLabel.begin(); kt != indexForLabel.end(); kt++) {
            if((jt=it->second.find(RinexObsHeader::convertObsType(kt->first)))
                  == it->second.end()) {
               spd.data[kt->second] = 0.0;
               spd.lli[kt->second] = 0;
               spd.ssi[kt->second] = 0;
            }
            else {
               spd.data[kt->second] = jt->second.data;
               spd.lli[kt->second] = jt->second.lli;
               spd.ssi[kt->second] = jt->second.ssi;
            }
         }  // end loop over obs

         spd.flag = OK;             // default

         return push_back(robs.time,spd);
      }
   }
   return -3;
}

// smooth pseudorange and debias phase; replace the data only if the
// corresponding input flag is 'true'.
// call this ONLY after cycleslips have been removed.
void SatPass::smooth(bool smoothPR, bool debiasPH, string& msg) throw(Exception)
{
   // test for L1, L2, P1, P2
   map<string, unsigned int>::const_iterator it;
   if(indexForLabel.find("L1") == indexForLabel.end() ||
      indexForLabel.find("L2") == indexForLabel.end() ||
      indexForLabel.find("P1") == indexForLabel.end() ||
      indexForLabel.find("P2") == indexForLabel.end()) {
      Exception e("Obs types L1 L2 P1 P2 required for smooth()");
      GPSTK_THROW(e);
   }

   //static const double CFF=C_GPS_M/OSC_FREQ;
   static const double F1=L1_MULT;   // 154.0;
   static const double F2=L2_MULT;   // 120.0;
   // wavelengths
   static const double wl1=L1_WAVELENGTH; //CFF/F1;                        // 19.0cm
   static const double wl2=L2_WAVELENGTH; //CFF/F2;                        // 24.4cm
   // ionospheric constant
   static const double alpha = ((F1/F2)*(F1/F2) - 1.0);

   // transformation matrix
   // PB = D * L - P   pure biases = constants for continuous phase
   // RB = D * PB      real biases = wavelength * N
   // but DD=1 so **( RB = DDL-DP = L-DP )**
   // dbL = L - RB     debiased phase
   // smR = D * dbL    smoothed range
   //      1 [ a+2     -2  ]
   // D = -- [             ]
   //      a [ 2a+2 -(a+2) ]
   static const double D11 = (alpha+2.)/alpha;
   static const double D12 = -2./alpha;
   static const double D21 = (2*alpha+2.)/alpha;
   static const double D22 = -D11;

   bool first;
   int i;
   double RB1,RB2,dbL1,dbL2;
   Stats<double> PB1,PB2;

   // get the biases B = L - DP
   for(first=true,i=0; i<spdvector.size(); i++) {
      if(!(spdvector[i].flag & OK)) continue;        // skip bad data
      double P1 = spdvector[i].data[indexForLabel["P1"]];
      double P2 = spdvector[i].data[indexForLabel["P2"]];
      RB1 = wl1*spdvector[i].data[indexForLabel["L1"]] - D11*P1 - D12*P2;
      RB2 = wl2*spdvector[i].data[indexForLabel["L2"]] - D21*P1 - D22*P2;
      if(first) { dbL1 = RB1; dbL2 = RB2; first = false; }
      PB1.Add(RB1-dbL1);
      PB2.Add(RB2-dbL2);
   }
   // real biases in cycles
   RB1 = (dbL1 + PB1.Average())/wl1;
   RB2 = (dbL2 + PB2.Average())/wl2;

   ostringstream oss;
   oss << "SMT" << fixed << setprecision(2)
       << " " << sat
       << " " << getFirstGoodTime().printf(outFormat)
       << " " << getLastGoodTime().printf(outFormat)
       << " " << setw(5)  << PB1.N()
       << " " << setw(12) << PB1.Average()+dbL1
       << " " << setw(5)  << PB1.StdDev()
       << " " << setw(12) << PB1.Minimum()+dbL1
       << " " << setw(12) << PB1.Maximum()+dbL1
       << " " << setw(5)  << PB2.N()
       << " " << setw(12) << PB2.Average()+dbL2
       << " " << setw(5)  << PB2.StdDev()
       << " " << setw(12) << PB2.Minimum()+dbL2
       << " " << setw(12) << PB2.Maximum()+dbL2
       << " " << setw(13) << RB1
       << " " << setw(13) << RB2;
   msg = oss.str();

   if(!debiasPH && !smoothPR) return;

   for(i=0; i<spdvector.size(); i++) {
      if(!(spdvector[i].flag & OK)) continue;        // skip bad data

      // compute the debiased phase
      dbL1 = spdvector[i].data[indexForLabel["L1"]] - RB1;
      dbL2 = spdvector[i].data[indexForLabel["L2"]] - RB2;

      // replace the phase with the debiased phase
      if(debiasPH) {
         spdvector[i].data[indexForLabel["L1"]] = dbL1;
         spdvector[i].data[indexForLabel["L2"]] = dbL2;
      }
      // smooth the range - replace the pseudorange with the smoothed pseudorange
      if(smoothPR) {
         spdvector[i].data[indexForLabel["P1"]] = D11*wl1*dbL1 + D12*wl2*dbL2;
         spdvector[i].data[indexForLabel["P2"]] = D21*wl1*dbL1 + D22*wl2*dbL2;
      }
   }
}

// -------------------------- get and set routines ----------------------------
double& SatPass::data(unsigned int i, std::string type) throw(Exception)
{
   if(i >= spdvector.size()) {
      Exception e("Invalid index in data() " + asString(i));
      GPSTK_THROW(e);
   }
   map<string, unsigned int>::const_iterator it;
   if((it = indexForLabel.find(type)) == indexForLabel.end()) {
      Exception e("Invalid obs type in data() " + type);
      GPSTK_THROW(e);
   }
   return spdvector[i].data[it->second];
}

unsigned short& SatPass::LLI(unsigned int i, std::string type) throw(Exception)
{
   if(i >= spdvector.size()) {
      Exception e("Invalid index in LLI() " + asString(i));
      GPSTK_THROW(e);
   }
   map<string, unsigned int>::const_iterator it;
   if((it = indexForLabel.find(type)) == indexForLabel.end()) {
      Exception e("Invalid obs type in LLI() " + type);
      GPSTK_THROW(e);
   }
   return spdvector[i].lli[it->second];
}

unsigned short& SatPass::SSI(unsigned int i, std::string type) throw(Exception)
{
   if(i >= spdvector.size()) {
      Exception e("Invalid index in SSI() " + asString(i));
      GPSTK_THROW(e);
   }
   map<string, unsigned int>::const_iterator it;
   if((it = indexForLabel.find(type)) == indexForLabel.end()) {
      Exception e("Invalid obs type in SSI() " + type);
      GPSTK_THROW(e);
   }
   return spdvector[i].ssi[it->second];
}

// ---------------------------------- set routines ----------------------------
void SatPass::setFlag(unsigned int i, unsigned short f) throw(Exception)
{
   if(i >= spdvector.size()) {
      Exception e("Invalid index in setFlag() " + asString(i));
      GPSTK_THROW(e);
   }

   if(spdvector[i].flag != BAD && f == BAD) ngood--;
   if(spdvector[i].flag == BAD && f != BAD) ngood++;
   spdvector[i].flag = f;
}

// ---------------------------------- get routines ----------------------------
// get value of flag at one index
unsigned short SatPass::getFlag(unsigned int i) throw(Exception)
{
   if(i >= spdvector.size()) {
      Exception e("Invalid index in getFlag() " + asString(i));
      GPSTK_THROW(e);
   }
   return spdvector[i].flag;
}

// get one element of the count array of this SatPass
unsigned int SatPass::getCount(unsigned int i) const throw(Exception)
{
   if(i >= spdvector.size()) {
      Exception e("invalid in getCount() " + asString(i));
      GPSTK_THROW(e);
   }
   return spdvector[i].ndt;
}

// ---------------------------------- utils -----------------------------------
// return the time corresponding to the given index in the data array
DayTime SatPass::time(unsigned int i) const throw(Exception)
{
   if(i > spdvector.size()) {
      Exception e("invalid in time() " + asString(i));
      GPSTK_THROW(e);
   }
   // computing toff first is necessary to avoid a rare bug in DayTime..
   double toff = spdvector[i].ndt * dt + spdvector[i].toffset;
   //return (firstTime + spdvector[i].ndt*dt + spdvector[i].toffset);
   return (firstTime + toff);
}

// return true if the input time could lie within the pass
bool SatPass::includesTime(const DayTime& tt) const throw()
{
   if(tt < firstTime) {
      if((firstTime-tt) > maxGap) return false;
   }
   else if(tt > lastTime) {
      if((tt-lastTime) > maxGap) return false;
   }
   return true;
}

// dump all the data in the pass, one line per timetag;
// put message msg at beginning of each line.
void SatPass::dump(ostream& os, string msg1, string msg2) throw()
{
   int i,j;
   DayTime tt;
   os << '#' << msg1 << " " << *this << " " << msg2 << endl;
   os << '#' << msg1 << "  n Sat cnt flg     time      ";
   for(j=0; j<indexForLabel.size(); j++)
      os << "            " << labelForIndex[j] << " L S";
   os << endl;

   for(i=0; i<spdvector.size(); i++) {
      tt = time(i);
      os << msg1
         << " " << setw(3) << i
         << " " << sat
         << " " << setw(3) << spdvector[i].ndt
         << " " << setw(2) << spdvector[i].flag
         << " " << tt.printf(SatPass::outFormat)
         << fixed << setprecision(3);
      for(j=0; j<indexForLabel.size(); j++)
         os << " " << setw(13) << spdvector[i].data[j]
            << " " << spdvector[i].lli[j]
            << " " << spdvector[i].ssi[j];
      os << endl;
   }
}

// output SatPass to ostream
ostream& operator<<(ostream& os, SatPass& sp )
{
   os << setw(4) << sp.spdvector.size()
      << " " << sp.sat
      << " " << setw(4) << sp.ngood
      << " " << setw(2) << sp.Status
      << " " << sp.firstTime.printf(SatPass::outFormat)
      << " " << sp.lastTime.printf(SatPass::outFormat)
      << " " << fixed << setprecision(1) << sp.dt;
   for(int i=0; i<sp.labelForIndex.size(); i++) os << " " << sp.labelForIndex[i];

   return os;
}

// ---------------------------- private SatPassData functions --------------------
// add data to the arrays at timetag tt (private)
// return >=0 ok (index of added data), -1 gap, -2 timetag out of order
int SatPass::push_back(const DayTime tt, SatPassData& spd) throw()
{
   unsigned int n;
      // if this is the first point, save first time
   if(spdvector.size() == 0) {
      firstTime = lastTime = tt;
      n = 0;
   }
   else {
      if(tt < lastTime)
         return -2;
         // compute count for this point - prev line means n is >= 0
      n = countForTime(tt);
         // test size of gap
      if( (n - spdvector[spdvector.size()-1].ndt) * dt > maxGap)
         return -1;
      lastTime = tt;
   }

      // add it
   //spd.flag = 1;
   ngood++;  // ngood is useless unless it's changed whenever any flag is...
   spd.ndt = n;
   spd.toffset = tt - firstTime - n*dt;
   spdvector.push_back(spd);
   return (spdvector.size()-1);
}

// get one element of the data array of this SatPass (private)
struct SatPass::SatPassData SatPass::getData(unsigned int i) const
   throw(Exception)
{
   if(i >= spdvector.size()) {         // TD ?? keep this - its private
      Exception e("invalid in getData() " + asString(i));
      GPSTK_THROW(e);
   }
   return spdvector[i];
}

// -------------------------------------------------------------------------------
// ---------------------------- iterate over a SatPass list ----------------------
// only constructor
SatPassIterator::SatPassIterator(vector<SatPass>& splist)
   throw(Exception) : SPList(splist)
{
   if(SPList.size() == 0) {
      Exception e("Empty list");
      GPSTK_THROW(e);
   }

   // ensure time order
   sort(SPList);

   int i,j;
   vector<string> otlist;

   // copy the data from the first SatPass in the list, for comparison with the rest
   DT = SPList[0].dt;
   FirstTime = SPList[0].firstTime;
   LastTime = SPList[0].lastTime;
   // copy the list of obs types, and check that each is registered
   for(i=0; i<SPList[0].labelForIndex.size(); i++) {
      otlist.push_back(SPList[0].labelForIndex[i]);
      if(RinexObsHeader::convertObsType(SPList[0].labelForIndex[i])
            == RinexObsHeader::UN) {
         Exception e("Unregistered observation type : " + SPList[0].labelForIndex[i]);
         GPSTK_THROW(e);
      }
   }

   // loop over the list
   for(i=0; i<SPList.size(); i++) {
      // check for consistency of dt
      if(SPList[i].dt != DT) {
         Exception e("Inconsistent time intervals");
         GPSTK_THROW(e);
      }
      // check for consistency of obs types
      for(j=0; j<otlist.size(); j++) {
         if(SPList[i].indexForLabel.find(otlist[j]) ==
            SPList[i].indexForLabel.end()) {
               Exception e("Inconsistent observation types");
               GPSTK_THROW(e);
         }
      }
      // TD check for increasing time order?

      // find the earliest and latest time
      if(SPList[i].firstTime < FirstTime) FirstTime = SPList[i].firstTime;
      if(SPList[i].lastTime > LastTime) LastTime = SPList[i].lastTime;

   }  // end loop over the list

   reset();
}

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
   GSatID sat;

   numsvs = 0;
   indexMap.clear();
   nextIndexMap.clear();

   //LOG(DEBUG1) << "SPIterator::next(map) - time "
   //   << (FirstTime+currentN*DT).printf("%4F %10.3g");

   while(numsvs == 0) {
      if(listIndex.size() == 0) return 0;

      // loop over active SatPass's
      map<GSatID,int>::iterator kt = listIndex.begin();

      // debugging - dump the listIndex
      //for(kt = listIndex.begin(); kt != listIndex.end(); kt++) {
      //   //LOG(DEBUG3) << "Dump listIndex: " << kt->first << " " << kt->second;
      //}

      kt = listIndex.begin();
      while(kt != listIndex.end()) {
         sat = kt->first;
         i = kt->second;
         j = dataIndex[sat];
         //LOG(DEBUG4) << "Loop over listIndex: " << sat << " " << i << " " << j;

         if(SPList[i].Status < 0) continue;     // should never happen

         if(countOffset[sat] + SPList[i].spdvector[j].ndt == currentN) {
            // found active sat at this count - add to map
            nextIndexMap[i] = j;
            numsvs++;
            //LOG(DEBUG2) << "SPIterator::next(map) found sat " << sat
            //   << " at index " << i;

            // increment data index
            j++;
            if(j == SPList[i].spdvector.size()) {     // this pass is done
               //LOG(DEBUG2) << " This pass for sat " << sat << " is done ...";
               indexStatus[i] = 1;

               // find the next pass for this sat
               for(k=i+1; k<SPList.size(); k++) {
                  //LOG(DEBUG3) << " ... consider next pass " << k
                  //   << " " << SPList[k].sat
                  //   << " " << SPList[k].firstTime.printf("%4F %10.3g");

                  if(SPList[k].Status < 0)      // bad pass
                     continue;
                  if(SPList[k].sat != sat)      // wrong sat
                     continue;
                  if(indexStatus[k] > 0)        // already done
                     continue;

                  // take this one
                  indexStatus[k] = 0;
                  i = listIndex[sat] = k;
                  dataIndex[sat] = 0;
                  countOffset[sat]
                     = int((SPList[i].firstTime - FirstTime)/DT + 0.5);
                  break;
               }
               if(indexStatus[i] == 0) {
                  //LOG(DEBUG2) << " ... new pass for sat " << SPList[i].sat
                  //<< " at index " << i << " and time "
                  //<< SPList[i].firstTime.printf("%4F %10.3g");
               }
            }
            else {
               dataIndex[sat] = j;
            }

         }  // end if found active sat at this count

         // increment the iterator
         if(indexStatus[i] > 0) {                // a new one was not found
            //LOG(DEBUG2) << " Erase this pass: index " << i << " sat " << sat;
            listIndex.erase(kt++);  // erasing a map - do exactly this and no more
         }
         else kt++;

      }  // end while loop over active SatPass's
      //LOG(DEBUG4) << "End while loop over active SatPasses";

      //if(robs.numSvs == 0) cout << "Gap at " << currentN << endl;
      currentN++;

   }  // end while robs.numSvs==0
   //LOG(DEBUG1) << "Return from next()";

   indexMap = nextIndexMap;

   return 1;
}

// return 1 for success, 0 at end of data
int SatPassIterator::next(RinexObsData& robs) throw(Exception)
{
   if(listIndex.size() == 0) return 0;

   map<unsigned int, unsigned int> indexMap;
   int iret = next(indexMap);
   if(iret == 0) return iret;

   robs.obs.clear();
   robs.epochFlag = 0;
   robs.time = FirstTime + (currentN-1) * DT;      // careful
   //LOG(DEBUG2) << "SPIterator::next(robs) found time " << robs.time;
   robs.clockOffset = 0.0;
   robs.numSvs = 0;

   // loop over the map
   map<unsigned int, unsigned int>::const_iterator kt;
   for(kt = indexMap.begin(); kt != indexMap.end(); kt++) {
      int i = kt->first;
      int j = kt->second;
      GSatID sat = SPList[i].getSat();
      //LOG(DEBUG2) << "SPIterator::next(robs) found sat " << sat
      //   << " at index " << i << " and time " << SPList[i].time(j);

      bool found = false;
      bool flag = (SPList[i].spdvector[j].flag != SatPass::BAD);
      for(int k=0; k<SPList[i].labelForIndex.size(); k++) {
         RinexObsHeader::RinexObsType ot;
         ot = RinexObsHeader::convertObsType(SPList[i].labelForIndex[k]);
         if(ot == RinexObsHeader::UN) {
            //LOG(DEBUG1) << " Error - this sat has UN obstype";
            // warn?
         }
         else {
            found = true;
            robs.obs[sat][ot].data = flag ? SPList[i].spdvector[j].data[k] : 0.;
            robs.obs[sat][ot].lli  = flag ? SPList[i].spdvector[j].lli[k] : 0;
            robs.obs[sat][ot].ssi  = flag ? SPList[i].spdvector[j].ssi[k] : 0;
         }
      }
      if(found) robs.numSvs++;
   }

   return 1;
}

// restart the iteration
void SatPassIterator::reset(void) throw()
{
   // clear out the old
   currentN = 0;
   listIndex.clear();
   dataIndex.clear();
   countOffset.clear();
   indexStatus = vector<int>(SPList.size(),-1);

   // loop over the list
   for(int i=0; i<SPList.size(); i++) {
      // ignore passes with negative Status
      if(SPList[i].Status < 0) continue;

      // (re)build the maps
      if(listIndex.find(SPList[i].sat) == listIndex.end()) {
         indexStatus[i] = 0;
         listIndex[SPList[i].sat] = i;
         dataIndex[SPList[i].sat] = 0;
         countOffset[SPList[i].sat]
            = int((SPList[i].firstTime - FirstTime)/DT + 0.5);
         //LOG(DEBUG4) << "reset - define map " << i << " for sat " << SPList[i].sat
         //   << " at time " << SPList[i].firstTime.printf("%4F %10.3g")
         //   << " offset " << countOffset[SPList[i].sat];
      }
      else {
         indexStatus[i] = -1;
         //LOG(DEBUG4)<< "reset - turn off pass " << i << " for sat " << SPList[i].sat
         //   << " at time " << SPList[i].firstTime.printf("%4F %10.3g");
      }
   }  // end loop over the list
}

// -------------------------------------------------------------------------------
// ---------------------------- sort, read and write SatPass lists ------------
// NB uses SatPass::operator<()
void sort(vector<SatPass>& SPList) throw()
{
   std::sort(SPList.begin(), SPList.end());
}

int SatPassFromRinexFiles(vector<string>& filenames,
                          vector<string>& obstypes,
                          double dt,
                          vector<SatPass>& SPList) throw(Exception)
{
   if(filenames.size() == 0) return -1;

   // sort the file names on the begin time in the header
   if(filenames.size() > 1) sortRinexObsFiles(filenames);

   int i,j,nfiles = 0;
   vector<double> data(obstypes.size(),0.0);
   vector<unsigned short> ssi(obstypes.size(),0);
   vector<unsigned short> lli(obstypes.size(),0);
   map<GSatID,int> indexForSat;
   map<GSatID,int>::const_iterator satit;
   RinexObsHeader header;
   RinexObsData obsdata;

   // sort existing list on begin time
   sort(SPList);

   // fill the index array using SatPass's already there
   // assumes SPList is in time order - later ones overwrite earlier
   for(i=0; i<SPList.size(); i++) {
      indexForSat[SPList[i].sat] = i;
   }

   for(int nfile=0; nfile<filenames.size(); nfile++) {
      string filename = filenames[nfile];

      // does the file exist?
      RinexObsStream RinFile(filename.c_str());
      if(filename.empty() || !RinFile) {
         //cerr << "Error: input file " << filename << " does not exist.\n";
         continue;
      }
      RinFile.exceptions(fstream::failbit);

      // is it a Rinex Obs file? ... read the header
      try { RinFile >> header; }
      catch(Exception& e) {
         //cerr << "Error: input file " << filename << " is not a Rinex obs file\n";
         continue;
      }

      // to return the number of files read
      nfiles++;

      // check that obs types are in header - first file only
      if(obstypes.size() == 0) {
         for(j=0; j<header.obsTypeList.size(); j++) {
            obstypes.push_back(RinexObsHeader::convertObsType(header.obsTypeList[j]));
         }
         data = vector<double>(obstypes.size(),0.0);
         ssi = vector<unsigned short>(obstypes.size(),0);
         lli = vector<unsigned short>(obstypes.size(),0);
      }

      // loop over epochs in the file
      while(RinFile >> obsdata) {
         RinexObsData::RinexSatMap::const_iterator it;
         RinexObsData::RinexObsTypeMap::const_iterator jt;

         // skip auxiliary header, etc
         if(obsdata.epochFlag != 0 && obsdata.epochFlag != 1) continue;

         // loop over satellites
         for(it=obsdata.obs.begin(); it != obsdata.obs.end(); ++it) {
            GSatID sat = it->first;

            // loop over obs
            for(j=0; j<obstypes.size(); j++) {
               if((jt=it->second.find(RinexObsHeader::convertObsType(obstypes[j])))
                     == it->second.end()) {
                  data[j] = 0.0;
                  lli[j] = ssi[j] = 0;
               }
               else {
                  data[j] = jt->second.data;
                  lli[j] = jt->second.lli;
                  ssi[j] = jt->second.ssi;
               }
            }  // end loop over obs

            // find the current SatPass for this sat
            satit = indexForSat.find(sat);

            // if there is not one, create one
            if(satit == indexForSat.end()) {
               SatPass newSP(sat,dt,obstypes);
               SPList.push_back(newSP);
               indexForSat[sat] = SPList.size()-1;
               satit = indexForSat.find(sat);
            }
            
            // add the data to the SatPass
            do {
               i = SPList[satit->second].addData(obsdata.time,obstypes,data,lli,ssi);
               if(i < 0) {             // failure
                  if(i == -1) {        // gap
                     SatPass newSP(sat,dt,obstypes);
                     SPList.push_back(newSP);
                     indexForSat[sat] = SPList.size()-1;
                     satit = indexForSat.find(sat);
                  }
                  else if(i == -2) {   // time tag out of order
                     Exception e("Time tags out of order at time "
                           + obsdata.time.printf("%4F %10.3g"));
                     GPSTK_THROW(e);
                  }
                  //else if(i == -3) {   // sat not found (RinexObsData form only)
                  //}
               }
            } while(i < 0);

         } // end loop over satellites

      } // end loop over obs data in file

      RinFile.close();
   }

   return nfiles;
}

int SatPassToRinexFile(string filename,
                       RinexObsHeader& header,
                       vector<SatPass>& SPList) throw(Exception)
{
   try {
      // create iterator
      SatPassIterator spit(SPList);

      // open file
      RinexObsStream rstrm(filename.c_str(), ios::out);
      if(!rstrm) return -1;
      rstrm.exceptions(fstream::failbit);

      // put obs types, first time and interval in header
      header.obsTypeList.clear();
      for(int i=0; i<SPList[0].labelForIndex.size(); i++)
         header.obsTypeList.push_back(
            RinexObsHeader::convertObsType(SPList[0].labelForIndex[i]));
      header.firstObs = spit.getFirstTime();
      header.lastObs = spit.getLastTime();
      header.interval = spit.getDT();
      header.valid |= RinexObsHeader::firstTimeValid;
      header.valid |= RinexObsHeader::lastTimeValid;
      header.valid |= RinexObsHeader::intervalValid;

      rstrm << header;

      RinexObsData robs;
      RinexObsData::RinexSatMap::const_iterator it;
      RinexObsData::RinexObsTypeMap::const_iterator jt;
      //cout << "#RNX  wk  sec_of_wk sat";
      //for(int i=0; i<SPList[0].labelForIndex.size(); i++)
      //   cout << "            " << SPList[0].labelForIndex[i] << " L S";
      //cout << endl;

      while(spit.next(robs)) {
         //cout << "RNX " << robs.time.printf(SatPass::outFormat)
         //      << fixed << setprecision(3);
         if(robs.epochFlag != 0 || robs.obs.size() == 0) {
            //cout << endl;
            continue;
         }

         rstrm << robs;

         //for(it=robs.obs.begin(); it != robs.obs.end(); it++) {
         //   if(it != robs.obs.begin()) cout << "RNX                ";
         //   cout << " " << GSatID(it->first);
         //   for(jt=it->second.begin(); jt !=it->second.end(); jt++) {
         //      cout << " " << setw(13) << jt->second.data
         //           << " " << jt->second.lli
         //           << " " << jt->second.ssi;
         //   }
         //   cout << endl;
         //}
      }

      rstrm.close();
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }

   return 0;
}

}  // end namespace gpstk
