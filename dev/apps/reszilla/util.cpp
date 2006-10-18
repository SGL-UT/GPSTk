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

#include "util.hpp"

using namespace std;
using namespace gpstk;

const ObsID C1(ObsID::otRange,   ObsID::cbL1,   ObsID::tcCA);
const ObsID P1(ObsID::otRange,   ObsID::cbL1,   ObsID::tcP);
const ObsID L1(ObsID::otPhase,   ObsID::cbL1,   ObsID::tcP);
const ObsID D1(ObsID::otDoppler, ObsID::cbL1,   ObsID::tcP);
const ObsID S1(ObsID::otSNR,     ObsID::cbL1,   ObsID::tcP);
const ObsID C2(ObsID::otRange,   ObsID::cbL2,   ObsID::tcC2LM);
const ObsID P2(ObsID::otRange,   ObsID::cbL2,   ObsID::tcP);
const ObsID L2(ObsID::otPhase,   ObsID::cbL2,   ObsID::tcP);
const ObsID D2(ObsID::otDoppler, ObsID::cbL2,   ObsID::tcP);
const ObsID S2(ObsID::otSNR,     ObsID::cbL2,   ObsID::tcP);


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void computeDD2(const ObsEpochMap& rx1,
                const ObsEpochMap& rx2,
                DD2EpochMap& ddem)
{
   if (verbosity)
      cout << "Computing 2nd differences residuals across codes." << endl;

   // We use the data from rx1 walk us through the data
   // loop over all epochs for this station
   ObsEpochMap::const_iterator ei1, ei2;
   for (ei1=rx1.begin(); ei1!=rx1.end(); ei1++)
   {
      // first make sure we have data from the other receiver for this
      // epoch...
      const DayTime& t = ei1->first;
      ei2 = rx2.find(t);
      if (ei2 == rx2.end())
      {
         if (verbosity>2)
            cout << "Epoch with no data in second file at " << t << endl;
         continue;
      }

      ObsEpoch e1 = ei1->second;
      ObsEpoch e2 = ei2->second;

      DD2Epoch dde;
      if (dde.compute(e1, e2))
         ddem[t] = dde;
   }
} // end of computeDD2()


// ---------------------------------------------------------------------
// Compute a double difference for each prn in track on both receivers
// (C1(prn1)-P2(prn1)) - (C1(prn2)-P2(prn2))
// ---------------------------------------------------------------------
bool DD2Epoch::compute(ObsEpoch rx1,
                       ObsEpoch rx2)
{
   ObsEpoch::iterator oi1, oi2;
   for (oi1=rx1.begin(); oi1!=rx1.end(); oi1++)
   {
      SatID prn = oi1->first;
      oi2 = rx2.find(prn);
      if (oi2 == rx2.end())
         continue;

      SvObsEpoch& rx1obs = oi1->second;
      SvObsEpoch& rx2obs = oi2->second;

      // Now compute the C1-P2 for each receiver
      double d1 = rx1obs[C1] - rx1obs[P2];
      double d2 = rx2obs[C1] - rx2obs[P2];

      // Now compute the double differences
      res[prn] = d1 - d2;
   }

   return true;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
SvElevationMap elevation_map(const ObsEpochMap& oem,
                             const Triple& ap,
                             const EphemerisStore& eph)
{
   SvElevationMap pem;
   if (verbosity>1)
      cout << "Computing elevation map." << endl;

   ECEF rxpos(ap);

   ObsEpochMap::const_iterator oem_itr;
   for (oem_itr=oem.begin(); oem_itr!=oem.end(); oem_itr++)
   {
      const DayTime& t = oem_itr->first;
      const ObsEpoch& oe = oem_itr->second;
      ObsEpoch::const_iterator oe_itr;
      for (oe_itr=oe.begin(); oe_itr!=oe.end(); oe_itr++)
         try
         {
            SatID prn = oe_itr->first;
            Xvt svpos = eph.getPrnXvt(prn.id, t);
            pem[t][prn] = rxpos.elvAngle(svpos.x);
         }
         catch (EphemerisStore::NoEphemerisFound& e)
         {
            if (verbosity>2)
               cout << e.getText() << endl;
         }
   }
   return pem;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
SvElevationMap elevation_map(const ORDEpochMap& oem)
{
   SvElevationMap pem;
   if (verbosity>1)
      cout << "Computing elevation map." << endl;

   ORDEpochMap::const_iterator i;
   for (i=oem.begin(); i!=oem.end(); i++)
   {
      const DayTime& t=i->first;
      const ORDEpoch& epoch=i->second;
      ORDEpoch::ORDMap::const_iterator j;
      for (j=epoch.ords.begin(); j!=epoch.ords.end(); j++)
      {
         const ObsRngDev& ord = j->second;
         const SatID& svid = j->first;
         pem[t][svid] = ord.getElevation();
      }
   }
   return pem;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void add_clock_to_obs(ObsEpochMap& rem, const ORDEpochMap& oem)
{
   const GPSGeoid gm;
   ObsEpochMap::iterator i;
   for (i=rem.begin(); i!=rem.end(); i++)
   {
      ObsEpoch& rod = i->second;
      ORDEpochMap::const_iterator j = oem.find(rod.time);
      if (j==oem.end() || !j->second.clockOffset.is_valid())
      {
         if (verbosity>2)
            cout << "Epoch has no clock " << rod.time << endl;
         continue;
      }
      rod.rxClock = j->second.clockOffset/gm.c();
   }
} // end of add_clock_to_rinex()


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void check_data(const Triple& ap, const ObsEpochMap& rem)
{
   double rate=-1;
   int gapCount(0), rateCount(0);

   ObsEpochMap::const_iterator i = rem.begin();
   while (i != rem.end())
   {
      const ObsEpoch& prev = i->second;
      i++;
      if (i == rem.end())
         break;
      const ObsEpoch& curr = i->second;
      double dt = curr.time - prev.time;
      if (rate < 0)
      {
         rate = dt;
         cout << "Data rate at " 
              << curr.time << " is " << rate << " seconds." << endl;
      }
      else if (std::abs(rate - dt) > 1e-3 && dt > 0 && dt < 300)
      {
         rateCount++;
         cout << "Data rate change at "
              << curr.time <<" from " << rate
              << " to " << dt << " seconds." << endl;
      }
      else if (dt >= 300)
      {
         gapCount++;
         cout << "Data gap from "
              << prev.time <<" to " << curr.time << endl;
      }
   }
   
   cout << "Data had " << gapCount << " gaps." << endl;
   cout << "Data had " << rateCount << " rate changes." << endl;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void dumpStats(DD2EpochMap& ddem, SvElevationMap& pem)
{

   cout << endl
        << "obs       elev    sdev(m)  mean(m) [err]         #obs   #del   min     max    " << endl
        << "--------- ------  -------  --------------------  ------ ----   -------------- " << endl;

   for (ElevationRangeList::const_iterator i = elr.begin(); i != elr.end(); i++)
   {
      cout << "C1-P2 res "; 
      computeStats(ddem,  *i, pem); 
      cout << endl;
   }
   cout << endl;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void computeStats(
   DD2EpochMap& ddem,
   const ElevationRange er,
   SvElevationMap& pem)
{
   float minElevation = er.first;
   float maxElevation = er.second;

   Stats<double> good, bad;
   DD2EpochMap::iterator ei;
   for (ei = ddem.begin(); ei != ddem.end(); ei++)
   {
      const DayTime& t = ei->first;
      DD2Epoch::DD2ResidualMap::iterator pi;
      for (pi = ei->second.res.begin(); pi != ei->second.res.end(); pi++)
      {
         const SatID& prn = pi->first;
         double ddr = pi->second;
         if (pem[t][prn]>minElevation && pem[t][prn]<maxElevation)
         {
            if (abs(ddr) < 1000 )
               good.Add(ddr);
            else
               bad.Add(ddr);
         }
      }
   }
   
   cout << setprecision(2) << setw(2) <<  minElevation
        << "-"
        << setw(2) << maxElevation
        << " ";

   char b1[200];
   sprintf(b1, "%8.5f  %9.2e [%8.2e] %7d %4d  %7.2f %7.2f",
           good.StdDev()/2, good.Average(), good.StdDev()/sqrt((float)good.N()),
           good.N(), bad.N(),
           good.Minimum(), good.Maximum());

   cout << b1;
}

// ---------------------------------------------------------------------
// Returns a string with the number of cycle slips that are in the elevation range
// specififed.
// ---------------------------------------------------------------------
string computeStats(
   const CycleSlipList& csl, 
   const ElevationRange& er, 
   const ObsID& oid)
{
   ostringstream oss;
   int slipCount=0;

   float minElev = er.first;
   float maxElev = er.second;
   CycleSlipList::const_iterator i;
   for (i=csl.begin(); i!=csl.end(); i++)
   {
      const CycleSlipRecord& cs = *i;
      if (cs.oid == oid && minElev < cs.elevation && cs.elevation <= maxElev)
         slipCount++;
   }
   oss << slipCount;
   return oss.str();
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void dump(std::ostream& s, const CycleSlipList& csl)
{
   s << "Total Cycle slips: " << csl.size() << endl;

   CycleSlipList::const_iterator i;
   long l1=0, l2=0;
   for (i=csl.begin(); i!=csl.end(); i++)
      if (i->oid == L1)
         l1++;
      else if (i->oid == L2)
         l2++;

   s << "Cycle slips on L1: " << l1 << endl;
   s << "Cycle slips on L2: " << l2 << endl;

   if (csl.size() == 0 || verbosity<1)
      return;

   s << endl
     << "# time                 prn        cyles     elev     pre   post   gap mstr " << endl;
   s.setf(ios::fixed, ios::floatfield);
   for (i=csl.begin(); i!=csl.end(); i++)
   {
      const CycleSlipRecord& cs=*i;
      s << left << setw(20) << cs.t.printf(timeFormat)
        << "  " << right << setw(2) << cs.prn.id
        << " " << cs.oid
        << " " << setprecision(3) << setw(14) << cs.cycles
        << "  " << std::setprecision(2) << setw(5) << cs.elevation
        << "  " << setw(5) << cs.preCount
        << "  " << setw(5) << cs.postCount
        << "  " << setw(5) << setprecision(1) << cs.preGap
        << "  " << setw(2) << cs.masterPrn.id
        << endl;
   }
   s << endl;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void dump(std::ostream& s,
          DD2EpochMap& ddem,
          SvElevationMap& pem)
{
   s.setf(ios::fixed, ios::floatfield);
   s << "# time, PRN, RES(m), SV_EL" << endl;

   DD2EpochMap::iterator ei;
   DD2Epoch::DD2ResidualMap::iterator pi;
   for (ei = ddem.begin(); ei != ddem.end(); ei++)
   {
      const DayTime& t = ei->first;
      string time=t.printf(timeFormat);
      for (pi = ei->second.res.begin(); pi != ei->second.res.end(); pi++)
      {
         const SatID& prn = pi->first;
         double ddr = pi->second;
         s << time
           << setfill(' ')
           << " " << setw(2)  << prn.id
           << setprecision(6)
           << " " << setw(11) << ddr
           << setprecision(1)
           << " " << setw(4) << pem[t][prn]
           << endl;
      }
   }
}  // end dump()
