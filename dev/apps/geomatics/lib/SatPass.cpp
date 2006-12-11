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
#include <ostream>
#include <string>
#include "SatPass.hpp"
#include "icd_200_constants.hpp"    // OSC_FREQ,L1_MULT,L2_MULT
#include "Stats.hpp"

using namespace std;

// for use in smooth()
const double CFF=gpstk::C_GPS_M/gpstk::OSC_FREQ;
const double F1=gpstk::L1_MULT;   // 154.0;
const double F2=gpstk::L2_MULT;   // 120.0;
// wavelengths
const double wl1=CFF/F1;                        // 19.0cm
const double wl2=CFF/F2;                        // 24.4cm
// ionospheric constant
const double alpha = ((F1/F2)*(F1/F2) - 1.0);
// transformation matrix
const double D11 = (alpha+2.)/alpha;
const double D12 = -2./alpha;
const double D21 = (2*alpha+2.)/alpha;
const double D22 = -D11;

//------------------------------------------------------------------------------------
namespace gpstk {

// note that flag & LL1 = true for all L1 discontinuities
//           flag & LL2 = true for all L2 discontinuities
const unsigned short SatPass::OK  = 1; // good data, no discontinuity
const unsigned short SatPass::BAD = 0; // used by caller and DC to mark bad data
const unsigned short SatPass::LL1 = 2; // discontinuity on L1 only
const unsigned short SatPass::LL2 = 4; // discontinuity on L2 only
const unsigned short SatPass::LL3 = 6; // discontinuity on L1 and L2
double SatPass::maxGap = 1800;         // maximum gap (seconds) allowed within pass
string SatPass::outFormat = string("%4F %10.3g");  // GPS week, seconds of week

// Default constructor
SatPassData::SatPassData(void)
{
   flag = SatPass::OK;
   ndt = 0;
   L1 = L2 = P1 = P2 = 0.0;
   indicators = 0;
}

// Destructor
SatPassData::~SatPassData(void) { }

// Default constructor
SatPass::SatPass(RinexSatID insat, double indt)
{
   sat = insat;
   dt = indt;
   ngood = 0;
   status = 0;
}

// Destructor
SatPass::~SatPass(void) { }

// operator =
SatPass& SatPass::operator=(const SatPass& sp)
{
   sat = sp.sat;
   dt = sp.dt;
   ngood = sp.ngood;
   status = sp.status;
   firstTime = sp.firstTime;
   lastTime = sp.lastTime;
   data.resize(sp.data.size());
   for(int i=0; i<sp.data.size(); i++) {
      data[i] = sp.data[i];
   }
   return *this;
}

// add data (P1,P2,L1,L2) to the arrays at timetag tt, optionally flag it BAD
bool SatPass::push_back(const DayTime tt, SatPassData& spd)
{
   int n;

      // if this is the first point, save first time
   if(data.size() == 0) {
      firstTime = lastTime = tt;
      n = 0;
   }
   else {
      if(tt < lastTime) {
         // throw
      }
         // compute count for this point
      n = int((tt-firstTime)/dt + 0.5);

         // test size of gap
      if( (n - data[data.size()-1].ndt) * dt > maxGap)
         return false;

      lastTime = tt;
   }

      // add it
   if(spd.flag & OK) ngood++;
   spd.ndt = n;
   data.push_back(spd);

   return true;
}

// return the time corresponding to the given index in the data array
DayTime SatPass::time(unsigned int i) const throw(Exception)
{
   if(i > data.size()) {
      Exception e("invalid in time() " + StringUtils::asString(i));
      GPSTK_THROW(e);
   }
   return (firstTime + data[i].ndt * dt);
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

// get one element of the data array of this SatPass
SatPassData SatPass::getData(unsigned int i) const throw(Exception)
{
   if(i >= data.size()) {
      Exception e("invalid in getData() " + StringUtils::asString(i));
      GPSTK_THROW(e);
   }
   return data[i];
}

// get one element of the count array of this SatPass
unsigned int SatPass::getCount(unsigned int i) const throw(Exception)
{
   if(i >= data.size()) {
      Exception e("invalid in getCount() " + StringUtils::asString(i));
      GPSTK_THROW(e);
   }
   return data[i].ndt;
}

// smooth pseudorange and debias phase; replace the data only if the
// corresponding input flag is 'true'.
// call this ONLY after cycleslips have been removed.
void SatPass::smooth(bool smoothPR, bool smoothPH, ostream& os)
{
   // PB = D * L - R   pure biases = constants for continuous phase
   // RB = D * PB      real biases = wavelength * N
   // dbL = L - RB     debiased phase
   // smR = D * dbL    smoothed range
   //      1 [ a+2     -2  ]
   // D = -- [             ]
   //      a [ 2a+2 -(a+2) ]

   int i;
   double RB1,RB2,dbL1,dbL2;
   Stats<double> PB1,PB2;

   // get the average pure bias
   for(i=0; i<data.size(); i++) {
      if(!(data[i].flag & OK)) continue;        // skip bad data
      PB1.Add(D11*wl1*data[i].L1 + D12*wl2*data[i].L2 - data[i].P1);
      PB2.Add(D21*wl1*data[i].L1 + D22*wl2*data[i].L2 - data[i].P2);
   }
   // real biases / wavelength
   RB1 = (D11*(PB1.Average()) + D12*(PB2.Average()))/wl1;
   RB2 = (D21*(PB1.Average()) + D22*(PB2.Average()))/wl2;

   os << "SMT" << fixed << setprecision(1)
      << " " << sat
      << " " << firstTime.printf(outFormat)
      << " " << lastTime.printf(outFormat)
      << " " << setw(5) << PB1.N()
      << " " << setw(5) << PB1.Average()
      << " " << setw(5) << PB1.StdDev()
      << " " << setw(5) << PB1.Minimum()
      << " " << setw(5) << PB1.Maximum()
      << " " << setw(5) << PB2.N()
      << " " << setw(5) << PB2.Average()
      << " " << setw(5) << PB2.StdDev()
      << " " << setw(5) << PB2.Minimum()
      << " " << setw(5) << PB2.Maximum()
      << " " << setw(6) << RB1
      << " " << setw(6) << RB2
      << endl;

   if(!smoothPH && !smoothPR) return;

   for(i=0; i<data.size(); i++) {
      if(!(data[i].flag & OK)) continue;        // skip bad data

      dbL1 = data[i].L1 - RB1;
      dbL2 = data[i].L2 - RB2;
      // debias the phase
      if(smoothPH) {
         data[i].L1 = dbL1;
         data[i].L2 = dbL2;
      }
      // smooth the range
      if(smoothPR) {
         data[i].P1 = D11*wl1*dbL1 + D12*wl2*dbL2;
         data[i].P2 = D21*wl1*dbL1 + D22*wl2*dbL2;
      }
   }
}

// dump all the data in the pass, one line per timetag;
// put message msg at beginning of each line.
void SatPass::dump(ostream& os, string msg1, string msg2) const throw()
{
   os << '#' << msg1 << " " << *this << " " << msg2 << endl;

   DayTime tt;
   for(int i=0; i<data.size(); i++) {
      tt = firstTime + data[i].ndt * dt;
      os << msg1
         << " " << setw(3) << i
         << " " << sat
         << " " << setw(3) << data[i].ndt
         << " " << setw(2) << data[i].flag
         << " " << tt.printf(SatPass::outFormat)
         << fixed << setprecision(3)
         << " " << setw(13) << data[i].L1
         << " " << setw(13) << data[i].L2
         << " " << setw(13) << data[i].P1
         << " " << setw(13) << data[i].P2
         << endl;
   }
}

// output SatPass to ostream
ostream& operator<<(ostream& os, const SatPass& sp )
{
   os << setw(3) << sp.data.size()
      << " " << sp.sat
      << " " << setw(3) << sp.ngood
      << " " << setw(2) << sp.status
      << " " << sp.firstTime.printf(SatPass::outFormat)
      << " " << sp.lastTime.printf(SatPass::outFormat)
      << " " << fixed << setprecision(1) << sp.dt;

   return os;
}

}  // end namespace gpstk
