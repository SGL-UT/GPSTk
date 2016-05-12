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

/// @file SatPass.cpp
/// Data for one complete satellite pass overhead.

//------------------------------------------------------------------------------------
// system
#include <ostream>
#include <string>
#include <vector>
#include <algorithm>
// gpstk
#include "SatPass.hpp"
#include "Stats.hpp"
#include "StringUtils.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"
#include "RinexUtilities.hpp"
#include "stl_helpers.hpp"
#include "logstream.hpp"

//------------------------------------------------------------------------------------
using namespace std;
namespace gpstk {
using namespace StringUtils;

// ------------------ configuration --------------------------------
// note that flag & LL1 = true for all L1 discontinuities
//           flag & LL2 = true for all L2 discontinuities
const unsigned short SatPass::OK  = 1; // good data, no discontinuity
const unsigned short SatPass::BAD = 0; // used by caller to mark bad data
const unsigned short SatPass::LL1 = 2; // discontinuity on L1 only
const unsigned short SatPass::LL2 = 4; // discontinuity on L2 only
const unsigned short SatPass::LL3 = 6; // discontinuity on L1 and L2
double SatPass::maxGap = 1800;         // maximum gap (seconds) allowed within pass
int SatPass::outRound = 3;             // round frac seconds in outFormat
string SatPass::outFormat = string("%4F %10.3g");  // GPS week, seconds of week
string SatPass::longfmt = string("%04Y/%02m/%02d %02H:%02M:%06.3f = %04F %10.3g");

// constructors
SatPass::SatPass(RinexSatID insat, double indt) throw()
{
   vector<string> defaultObsTypes;
   defaultObsTypes.push_back("L1");
   defaultObsTypes.push_back("L2");
   defaultObsTypes.push_back("P1");
   defaultObsTypes.push_back("P2");

   init(insat, indt, defaultObsTypes);
}

SatPass::SatPass(RinexSatID insat, double indt, vector<string> obstypes) throw()
{
   init(insat, indt, obstypes);
}

void SatPass::init(RinexSatID insat, double indt, vector<string> obstypes) throw()
{
   sat = insat;
   dt = indt;
   //firstTime = CommonTime::BEGINNING_OF_TIME;
   //lastTime = CommonTime::END_OF_TIME;
   ngood = 0;
   Status = 0;

   for(int i=0; i<obstypes.size(); i++) {
      indexForLabel[obstypes[i]] = i;
      labelForIndex[i] = obstypes[i];
   }
}

SatPass& SatPass::operator=(const SatPass& right) throw()
{
   if(&right != this) {
      Status = right.Status;
      dt = right.dt;
      sat = right.sat;
      indexForLabel = right.indexForLabel;
      labelForIndex = right.labelForIndex;
      firstTime = right.firstTime;
      lastTime = right.lastTime;
      ngood = right.ngood;
      spdvector.resize(right.spdvector.size());
      for(int i=0; i<right.spdvector.size(); i++)
         spdvector[i] = right.spdvector[i];
   }

   return *this;
}

int SatPass::addData(const Epoch tt, vector<string>& ots, vector<double>& data)
   throw(Exception)
{
   vector<unsigned short> lli(data.size(),0),ssi(data.size(),0);
   try { return addData(tt, ots, data, lli, ssi); }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

// return -2 time tag out of order, data not added
//        -1 gap is larger than MaxGap, data not added
//       >=0 (success) index of the added data
int SatPass::addData(const Epoch tt,
                     const vector<string>& obstypes,
                     const vector<double>& data,
                     const vector<unsigned short>& lli,
                     const vector<unsigned short>& ssi,
                     const unsigned short flag) throw(Exception)
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

// return -4 robs was not obs data (header info)
//        -3 sat not found, data not added
//        -2 time tag out of order, data not added
//        -1 gap is larger than MaxGap, data not added
//       >=0 (success) index of the added data
int SatPass::addData(const RinexObsData& robs) throw()
{
   if(robs.epochFlag != 0 && robs.epochFlag != 1)
      return -4;

   RinexObsData::RinexSatMap::const_iterator it;
   RinexObsData::RinexObsTypeMap::const_iterator jt;
   map<string,unsigned int>::const_iterator kt;
   SatPassData spd(indexForLabel.size());

   // loop over satellites
   for(it=robs.obs.begin(); it != robs.obs.end(); it++) {
      if(it->first == sat) {      // sat is this->sat
         spd.flag = OK;
         // loop over obs
         for(kt=indexForLabel.begin(); kt != indexForLabel.end(); kt++) {
            if((jt=it->second.find(RinexObsHeader::convertObsType(kt->first)))
                  == it->second.end()) {
               spd.data[kt->second] = 0.0;
               spd.lli[kt->second] = 0;
               spd.ssi[kt->second] = 0;
               //spd.flag = BAD;// don't do this b/c spd may have 'empty' obs types
            }
            else {
               spd.data[kt->second] = jt->second.data;
               spd.lli[kt->second] = jt->second.lli;
               spd.ssi[kt->second] = jt->second.ssi;
               if(jt->second.data == 0.0) spd.flag = BAD;
            }
         }  // end loop over obs

         return push_back(robs.time,spd);
      }
   }
   return -3;        // sat was not found
}

// Truncate all data at and after the given time.
// return -1 if ttag is at or before the start of this pass,
// return +1 if ttag is at or after the end of this pass,
// else return 0
int SatPass::trimAfter(const Epoch ttag) throw(Exception)
{
try {
   if(ttag <= firstTime) return -1;
   if(ttag >= lastTime) return 1;

   // find the count for this time limit
   int count = countForTime(ttag);
   //if(count == -1) return 1;            // 4/16/13
   if(count < 0) return -1;

   unsigned int i, j, n(0);                     // count for ngood
   for(i=0; i<spdvector.size(); i++) {
      if(spdvector[i].ndt >= static_cast<unsigned int>(count)) { j=i; break; }
      if(spdvector[i].flag != SatPass::BAD) n++;
   }
   if(j > -1) {
      spdvector.resize(j+1);
      lastTime = time(j);
      ngood = n;
   }
   else return 1;                      // also should never happen ... handled above

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

// compute the GLO channel
// start at n, then set n before returning; return false if failure
// challenge is at low elevation, L1 is slightly better than L2, but need both
// return true if successful, false if failed; also return string msg, which is
// FINAL sat n week sow(beg) week sow(end) npts stddev slope sl/std stddev(slope) [??]
// NB if "??" appears at end of msg, result is questionable (stddev(slope) is high)
bool SatPass::getGLOchannel(int& n, string& msg) throw(Exception)
{
try {
   if(sat.system != SatID::systemGlonass) return false;

   // make sure L1, L2, C1/P1, P2 are present
   bool useC1=false;
   map<string, unsigned int>::const_iterator it;
   if(indexForLabel.find("L1") == indexForLabel.end() ||
      indexForLabel.find("L2") == indexForLabel.end() ||
      (indexForLabel.find("C1") == indexForLabel.end() &&
       indexForLabel.find("P1") == indexForLabel.end()) ||
      indexForLabel.find("P2") == indexForLabel.end()) {
      Exception e("Obs types L1 L2 C1/P1 P2 required for GLOchannel()");
      GPSTK_THROW(e);
   }
   if(indexForLabel.find("P1") == indexForLabel.end()) useC1=true;

   // transformation matrix
   // PB = D * L - P   pure biases = constants for continuous phase
   // RB = D * PB      real biases = wavelength * N
   // but DD=1 so **( RB = DDL-DP = L-DP )**
   // dbL = L - RB     debiased phase
   // smR = D * dbL    smoothed range
   //      1 [ a+2     -2  ]
   // D = -- [             ]
   //      a [ 2a+2 -(a+2) ]
   static const double alpha = ((9./7.)*(9./7.) - 1.0);     // ionospheric constant
   static const double D11 = (alpha+2.)/alpha;
   static const double D12 = -2./alpha;
   static const double D21 = (2*alpha+2.)/alpha;
   static const double D22 = -D11;

   bool first,done,ok;
   int i,dn,di,sign(0);
   const int N(spdvector.size());
   double pP1,pP2,pL1,pL2,pRB1,pRB2;
   TwoSampleStats<double> dN1,dN2;
   static const double testStdDev(40.0),testSlope(0.1),testRatio(10.0),testSigma(.25);
   vector<int> dnSeen;

   if(n < -7 || n > 7) n=0;         // just in case
   dn = 0;
   di = (N > 50 ? N/50 : 1);        // want about 50 points total

   while(1) {           // loop over possible choices for n
      done = ok = true;
      dN1.Reset();
      dN2.Reset();

      // nominal wavelengths
      double wl1(C_MPS/(1602.0e6 + (n+dn)*562.5e3));
      double wl2(C_MPS/(1246.0e6 + (n+dn)*437.5e3));

      // compute the slope of dBias vs dL: biases B = L - DP
      first = true;
      for(i=0; i<N; i+=di) {
         if(!(spdvector[i].flag & OK)) continue;         // skip bad data

         double P1 = spdvector[i].data[indexForLabel[(useC1 ? "C1" : "P1")]];
         double P2 = spdvector[i].data[indexForLabel["P2"]];
         double L1 = spdvector[i].data[indexForLabel["L1"]];
         double L2 = spdvector[i].data[indexForLabel["L2"]];
         double RB1 = wl1*L1 - D11*P1 - D12*P2;
         double RB2 = wl2*L2 - D21*P1 - D22*P2;

         if(!first && (::fabs(RB1-pRB1) > 2000. || ::fabs(RB2-pRB2) > 2000. ||
                       ::fabs(L1-pL1)/2848. > 1000. || ::fabs(L2-pL2)/2848. > 1000.)){
            first = true;
            continue;
         }

         if(!first) {
            dN1.Add((-L1+pL1)/2848.,RB1-pRB1);     // X,Y
            dN2.Add((-L2+pL2)/2848.,RB2-pRB2);

            LOG(DEBUG) << "GLODMP " << sat
               << " " << setw(2) << n+dn
               << " " << printTime(time(i),outFormat)
               << fixed << setprecision(2)
               << " " << setw(9) << RB1-pRB1
               << " " << setw(9) << -(L1-pL1)/2848.
               << " " << setw(4) << dN1.N()
               << " " << setw(9) << dN1.StdDevY()
               << " " << setw(9) << (dN1.N() > 1 ? dN1.Slope() : 0.0)
               << " " << setw(9) << (dN1.N() > 1 ? dN1.SigmaSlope() : 0.0)
               << " " << setw(9) << RB2-pRB2
               << " " << setw(9) << -(L2-pL2)/2848.
               << " " << setw(4) << dN2.N()
               << " " << setw(9) << dN2.StdDevY()
               << " " << setw(9) << (dN1.N() > 1 ? dN2.Slope() : 0.0)
               << " " << setw(9) << (dN1.N() > 1 ? dN2.SigmaSlope() : 0.0);
         }
         else first=false;

         pL1 = L1; pL2 = L2;
         pP1 = P1; pP2 = P2;
         pRB1 = RB1; pRB2 = RB2;

      } // end for loop over data

      if(dN1.N() == 0) return false;                     // no data

      int m(dn);        // save for LOG stmt

      // ------------------ tests -------------------------------------
      // -slope/Dn is 0.1877 for L1, 0.2413 for L2
      // this fails if SigmaSlope is big >~ 1 //dN1.SigmaSlope() < testSigma &&
      if(dN1.StdDevY() < testStdDev &&
         ::fabs(dN1.Slope()) < testSlope &&
         ::fabs(dN1.Slope())/dN1.SigmaSlope() < testRatio)
      {
         done = true;                                    // success
      }
      else {                                             // haven't found it yet
         done = false;

         // save this dn so its not repeated
         dnSeen.push_back(dn);

         // compute a new dn
         int dm = - int((dN1.Slope() < 0 ? -0.5 : 0.5) + dN1.Slope()/0.1877);
         if(::abs(dm) > 5 || n+dn+dm < -7 || n+dn+dm > 7 || dm==0
                          || dN1.SigmaSlope() > testSigma)
         {
            if(dN1.Slope() < 0) ++dn; else --dn;
         }
         else dn += dm;

         if(n+dn > 7 || n+dn < -7) {                     // failure - n+dn too big
            msg = string("out of range : n+dn=") + asString(n+dn);
            ok = false;
         }
         if(vectorindex(dnSeen,dn) != -1) {
            msg = string("failed to converge : n+dn=") + asString(n+dn);
            ok = false;
         }

      }

      LOG(DEBUG) << "GETGLO " << setw(2) << n+m << " PRELIM " << sat
         << fixed << setprecision(2)
         << " " << setw(2) << dN1.N()
         << " " << setw(9) << dN1.StdDevY()
         << " (" << setprecision(0) << testStdDev << ")" << setprecision(2)
         << " " << setprecision(3) << setw(10) << dN1.Slope()
         << " (" << testSlope << ")" << setprecision(2)
         << " " << setw(9) << dN1.Slope()/dN1.SigmaSlope()
         << " (" << setprecision(0) << testRatio << ")" << setprecision(2)
         << " " << setw(9) << dN1.SigmaSlope()
         << " (" << testSigma << ")"
         //<< " " << setw(9) << dN2.StdDevY()
         //<< " " << setw(9) << dN2.Slope()
         //<< " " << setw(9) << dN2.Slope()/dN1.SigmaSlope()
         << " " << (done ? "DONE":"NOPE");

      if(done || !ok) break;

   }  // end while

   if(!ok) return false;

   ostringstream oss;
   oss << "FINAL" << fixed << setprecision(6)
       << " " << sat
       << " " << setw(2) << n+dn
       << " " << printTime(getFirstGoodTime(),outFormat)
       << " " << printTime(getLastGoodTime(),outFormat)
       << fixed << setprecision(4)
       << " " << setw(2) << dN1.N()
       << " " << setw(8) << dN1.StdDevY()
       << " " << setw(8) << dN1.Slope()
       << " " << setw(8) << dN1.Slope()/dN1.SigmaSlope()
       << " " << setw(8) << dN1.SigmaSlope()
       << " " << (dN1.SigmaSlope() < testSigma ? "" : "??")
       //<< " " << setw(2) << dN2.N()
       //<< " " << setw(8) << dN2.StdDevY()
       //<< " " << setw(8) << dN2.Slope()
       //<< " " << setw(8) << dN2.Slope()/dN2.SigmaSlope()
       ;

   msg = oss.str();
   n = n+dn;

   return true;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

// Smooth pseudorange and debias phase; replace the data only if the corresponding
// input flag is 'true'; use real bias for pseudorange, integer (cycles) for phase.
// Call this ONLY after cycleslips have been removed.
void SatPass::smooth(const bool smoothPR, const bool debiasPH, string& msg,
                     const double& wl1, const double& wl2) throw(Exception)
{
try {
   ostringstream oss;

   // make sure L1, L2, C1/P1, C2/P2 are present
   if(!hasType("L1")) oss << " L1";
   if(!hasType("L2")) oss << " L2";
   if(!hasType("C1") && !hasType("P1")) oss << " C/P1";
   if(!hasType("C2") && !hasType("P2")) oss << " C/P2";
   if(!oss.str().empty())
      GPSTK_THROW(Exception( string("smooth() requires obs types L1 L2 C/P1 C/P2:")
                              + oss.str() + string(" missing.")));

   bool useC1(hasType("C1")),useC2(hasType("C2"));

   // transformation matrix
   // PB = D * L - P   pure biases = constants for continuous phase
   // RB = D * PB      real biases = wavelength * N
   // but DD=1 so **( RB = DDL-DP = L-DP )**
   // dbL = L - RB     debiased phase
   // smR = D * dbL    smoothed range
   //      1 [ a+2     -2  ]
   // D = -- [             ]
   //      a [ 2a+2 -(a+2) ]
   const double alpha = ((wl2/wl1)*(wl2/wl1) - 1.0);     // ionospheric constant
   const double D11 = (alpha+2.)/alpha;
   const double D12 = -2./alpha;
   const double D21 = (2*alpha+2.)/alpha;
   const double D22 = -D11;

   bool first;
   int i;
   double RB1,RB2,dbL1,dbL2,dLB10(0.0),dLB20(0.0);
   long LB1,LB2,LB10,LB20;
   Stats<double> PB1,PB2;

   // get the biases B = L - DP
   for(first=true,i=0; i<spdvector.size(); i++) {
      if(!(spdvector[i].flag & OK)) continue;        // skip bad data

      double P1 = spdvector[i].data[indexForLabel[(useC1 ? "C1" : "P1")]];
      double P2 = spdvector[i].data[indexForLabel[(useC2 ? "C2" : "P2")]];
      double L1 = spdvector[i].data[indexForLabel["L1"]] - dLB10;
      double L2 = spdvector[i].data[indexForLabel["L2"]] - dLB20;

      if(first) {                   // remove the large numerical range
         LB10 = long(L1-P1/wl1);
         LB20 = long(L2-P2/wl2);
         dLB10 = double(LB10);
         dLB20 = double(LB20);
         L1 -= dLB10;
         L2 -= dLB20;
      }

      // Bias = L(m) - D*P
      RB1 = wl1*L1 - D11*P1 - D12*P2;
      RB2 = wl2*L2 - D21*P1 - D22*P2;

      if(first) {
         dbL1 = RB1; dbL2 = RB2;
         first = false;
      }

      PB1.Add(RB1-dbL1);
      PB2.Add(RB2-dbL2);

      LOG(DEBUG) << "SMTDMP " << sat << " " << printTime(time(i),outFormat)
         << fixed << setprecision(3)
         << " " << setw(13) << RB1-dbL1
         << " " << setw(13) << RB2-dbL2
         << " " << setw(13) << L1
         << " " << setw(13) << L2
         << " " << setw(13) << P1
         << " " << setw(13) << P2
         << " " << setw(13) << RB1
         << " " << setw(13) << RB2
         ;
   }

   // real biases in cycles
   RB1 = (dbL1 + PB1.Average())/wl1;
   RB2 = (dbL2 + PB2.Average())/wl2;
   // integer biases (cycles)
   LB1 = LB10 + long(RB1 + (RB1 > 0 ? 0.5 : -0.5));
   LB2 = LB20 + long(RB2 + (RB2 > 0 ? 0.5 : -0.5));

   oss.str("");
   oss << "SMT" << fixed << setprecision(2)
       << " " << sat
       << " " << printTime(getFirstGoodTime(),outFormat)
       << " " << printTime(getLastGoodTime(),outFormat)
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
       << " " << setw(13) << RB2
       << " " << setw(10) << LB1
       << " " << setw(10) << LB2;
   msg = oss.str();

   if(!debiasPH && !smoothPR) return;

   for(i=0; i<spdvector.size(); i++) {
      if(!(spdvector[i].flag & OK)) continue;        // skip bad data

      // replace the phase with the debiased phase, with integer bias (cycles)
      if(debiasPH) {
         spdvector[i].data[indexForLabel["L1"]] -= LB1;
         spdvector[i].data[indexForLabel["L2"]] -= LB2;
      }

      // replace the pseudorange with the smoothed pseudorange
      if(smoothPR) {
         // compute the debiased phase, with real bias
         dbL1 = spdvector[i].data[indexForLabel["L1"]] - RB1;
         dbL2 = spdvector[i].data[indexForLabel["L2"]] - RB2;

         spdvector[i].data[indexForLabel[(useC1 ? "C1" : "P1")]]
                                                = D11*wl1*dbL1 + D12*wl2*dbL2;
         spdvector[i].data[indexForLabel[(useC2 ? "C2" : "P2")]]
                                                = D21*wl1*dbL1 + D22*wl2*dbL2;
      }
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

// -------------------------- get and set routines ----------------------------
// NB may be used as rvalue or lvalue
double& SatPass::data(unsigned int i, string type) throw(Exception)
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

double& SatPass::timeoffset(unsigned int i) throw(Exception)
{
   if(i >= spdvector.size()) {
      Exception e("Invalid index in timeoffset() " + asString(i));
      GPSTK_THROW(e);
   }
   return spdvector[i].toffset;
}

unsigned short& SatPass::LLI(unsigned int i, string type) throw(Exception)
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

unsigned short& SatPass::SSI(unsigned int i, string type) throw(Exception)
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

// @return the earliest time (full, including toffset) in this SatPass data
Epoch SatPass::getFirstTime(void) const throw() { return time(0); }

// @return the latest time (full, including toffset) in this SatPass data
Epoch SatPass::getLastTime(void) const throw() { return time(spdvector.size()-1); }

// these allow you to get e.g. P1 or C1. NB return double not double& as above: rvalue
double SatPass::data(unsigned int i, string type1, string type2) throw(Exception)
{
   if(i >= spdvector.size()) {
      Exception e("Invalid index in data() " + asString(i));
      GPSTK_THROW(e);
   }
   map<string, unsigned int>::const_iterator it;
   if((it = indexForLabel.find(type1)) != indexForLabel.end())
      return spdvector[i].data[it->second];
   else if((it = indexForLabel.find(type2)) != indexForLabel.end())
      return spdvector[i].data[it->second];
   else {
      Exception e("Invalid obs types in data() " + type1 + " " + type2);
      GPSTK_THROW(e);
   }
}

unsigned short SatPass::LLI(unsigned int i, string type1, string type2)
   throw(Exception)
{
   if(i >= spdvector.size()) {
      Exception e("Invalid index in LLI() " + asString(i));
      GPSTK_THROW(e);
   }
   map<string, unsigned int>::const_iterator it;
   if((it = indexForLabel.find(type1)) != indexForLabel.end())
      return spdvector[i].lli[it->second];
   else if((it = indexForLabel.find(type2)) != indexForLabel.end())
      return spdvector[i].lli[it->second];
   else {
      Exception e("Invalid obs types in LLI() " + type1 + " " + type2);
      GPSTK_THROW(e);
   }
}

unsigned short SatPass::SSI(unsigned int i, string type1, string type2)
   throw(Exception)
{
   if(i >= spdvector.size()) {
      Exception e("Invalid index in SSI() " + asString(i));
      GPSTK_THROW(e);
   }
   map<string, unsigned int>::const_iterator it;
   if((it = indexForLabel.find(type1)) == indexForLabel.end())
      return spdvector[i].ssi[it->second];
   else if((it = indexForLabel.find(type2)) == indexForLabel.end())
      return spdvector[i].ssi[it->second];
   else {
      Exception e("Invalid obs types in SSI() " + type1 + " " + type2);
      GPSTK_THROW(e);
   }
}

// ---------------------------------- utils -----------------------------------
// return the time corresponding to the given index in the data array
Epoch SatPass::time(unsigned int i) const throw(Exception)
{
   if(i >= spdvector.size()) {
      Exception e("Invalid index in time() " + asString(i));
      GPSTK_THROW(e);
   }
   // computing toff first is necessary to avoid a rare bug in Epoch..
   double toff = spdvector[i].ndt * dt + spdvector[i].toffset;
   return (firstTime + toff);
}

// return true if the input time could lie within the pass
bool SatPass::includesTime(const Epoch& tt) const throw()
{
   if(tt < firstTime) {
      if((firstTime-tt) > maxGap) return false;
   }
   else if(tt > lastTime) {
      if((tt-lastTime) > maxGap) return false;
   }
   return true;
}

// create a new SatPass from the given one, starting at count N.
// modify this SatPass to end just before N.
// return true if successful.
bool SatPass::split(int N, SatPass &newSP) {
try {
   int i,j,n,oldgood,ilast;
   Epoch tt;

   newSP = SatPass(sat, dt);                       // create new SatPass
   newSP.Status = Status;
   newSP.indexForLabel = indexForLabel;
   newSP.labelForIndex = labelForIndex;

   oldgood = ngood;
   ngood = ilast = 0;
   for(i=0; i<spdvector.size(); i++) {             // loop over all data
      n = spdvector[i].ndt;
      tt = time(i);
      if(n < N) {                                     // keep in this SatPass
         if(spdvector[i].flag != BAD) ngood++;
         ilast = i;
      }
      else {                                          // copy out data into new SP
         if(n == N) {
            newSP.ngood = oldgood-ngood;
            newSP.firstTime = newSP.lastTime = tt;
         }
         j = newSP.countForTime(tt);
         spdvector[i].ndt = j;
         spdvector[i].toffset = tt - newSP.firstTime - j*dt;
         newSP.spdvector.push_back(spdvector[i]);
      }
   }

   // now trim this SatPass
   spdvector.resize(ilast+1);
   lastTime = time(ilast);

   return true;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

void SatPass::decimate(const int N, Epoch refTime) throw(Exception)
{
try {
   if(N <= 1) return;
   if(spdvector.size() < N) { dt = N*dt; return; }
   if(refTime == CommonTime::BEGINNING_OF_TIME) refTime = firstTime;

   // find new firstTime = time(nstart)
   int i,j,nstart=int(0.5+(firstTime-refTime)/dt);
   nstart = nstart % N;
   while(nstart < 0) nstart += N;
   if(nstart > 0) nstart = N-nstart;

   // decimate
   ngood = 0;
   Epoch newfirstTime, tt;
   for(j=0,i=0; i<spdvector.size(); i++) {
      if(spdvector[i].ndt % N != nstart) continue;
      lastTime = time(i);
      if(j==0) {
         newfirstTime = time(i);
         spdvector[i].toffset = 0.0;
         spdvector[i].ndt = 0;
      }
      else {
         tt = time(i);
         spdvector[i].ndt = int(0.5+(tt-newfirstTime)/(N*dt));
         spdvector[i].toffset = tt - newfirstTime - spdvector[i].ndt * N * dt;
      }
      spdvector[j] = spdvector[i];
      if(spdvector[j].flag != BAD) ngood++;
      j++;
   }

   dt = N*dt;
   firstTime = newfirstTime;
   spdvector.resize(j); // trim
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

// dump all the data in the pass, one line per timetag;
// put message msg at beginning of each line.
void SatPass::dump(ostream& os, string msg1, string msg2) throw()
{
   int i,j,last;
   Epoch tt;
   os << '#' << msg1 << " " << *this << " " << msg2 << endl;
   os << '#' << msg1 << "  n Sat cnt flg     time         toffset";
   for(j=0; j<indexForLabel.size(); j++)
      os << "            " << labelForIndex[j] << " L S";
   os << " gap(pts)";
   os << endl;

   for(i=0; i<spdvector.size(); i++) {
      tt = time(i);
      os << msg1
         << " " << setw(3) << i
         << " " << sat
         << " " << setw(3) << spdvector[i].ndt
         << " " << setw(2) << spdvector[i].flag
         << " " << printTime(tt,SatPass::outFormat)
         << fixed << setprecision(6)
         << " " << setw(9) << spdvector[i].toffset
         << setprecision(3);
      for(j=0; j<indexForLabel.size(); j++)
         os << " " << setw(13) << spdvector[i].data[j]
            << " " << spdvector[i].lli[j]
            << " " << spdvector[i].ssi[j];
      if(i==0) last = spdvector[i].ndt;
      if(spdvector[i].ndt - last > 1) os << " " << spdvector[i].ndt-last;
      last = spdvector[i].ndt;
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
      << " " << printTime(sp.firstTime,SatPass::outFormat)
      << " " << printTime(sp.lastTime,SatPass::outFormat)
      << " " << fixed << setprecision(1) << sp.dt;
   for(int i=0; i<sp.labelForIndex.size(); i++) os << " " << sp.labelForIndex[i];

   return os;
}

// ---------------------------- private SatPassData functions --------------------
// add data to the arrays at timetag tt (private)
// return >=0 ok (index of added data), -1 gap, -2 timetag out of order
int SatPass::push_back(const Epoch tt, SatPassData& spd) throw()
{
   unsigned int n;
      // if this is the first point, save first time
   if(spdvector.size() == 0) {
      firstTime = lastTime = tt;
      n = 0;
   }
   else {
      if(tt - lastTime < 1.e-8) return -2;
         // compute count for this point - prev line means n is >= 0
      n = countForTime(tt);
         // test size of gap
      if( (n - spdvector[spdvector.size()-1].ndt) * dt > maxGap)
         return -1;
      lastTime = tt;
   }

      // add it
   //spd.flag = 1;
   // ngood is useless unless it's changed whenever any flag is...
   if(spd.flag != SatPass::BAD) ngood++;
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

}  // end namespace gpstk
