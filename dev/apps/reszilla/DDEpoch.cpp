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

#include <limits>

#include "BCEphemerisStore.hpp"

#include "DDEpoch.hpp"

using namespace std;


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
OIDM DDEpoch::singleDifference(
   const gpstk::SvObsEpoch& rx1obs,
   const gpstk::SvObsEpoch& rx2obs)
{
   OIDM diff;

   gpstk::SvObsEpoch::const_iterator d1_itr = rx1obs.find(D1);
   if (d1_itr == rx1obs.end())
      return diff;

   // clock offset correction
   double coc = clockOffset * d1_itr->second * gpstk::C_GPS_M/gpstk::L1_FREQ; 
   gpstk::SvObsEpoch::const_iterator roti1, roti2;
   for (roti1 = rx1obs.begin(); roti1 != rx1obs.end(); roti1++)
   {
      const gpstk::ObsID& oid = roti1->first;

      // Make sure we have an obs from the other receiver
      roti2 = rx2obs.find(oid);
      if (roti2 == rx2obs.end())
         continue;

      // Compute the first difference
      diff[oid] = roti1->second - roti2->second;

      // Need to convert the phase/doppler observables to meters
      if (oid == L1 || oid == D1)
         diff[oid] *=  gpstk::C_GPS_M/gpstk::L1_FREQ;
      if (oid == L2 || oid == D2)
         diff[oid] *=  gpstk::C_GPS_M/gpstk::L2_FREQ;

      // Then pull off the clock correction
      diff[oid] -= coc;
   }

   return diff;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void DDEpoch::doubleDifference(
   const gpstk::ObsEpoch& rx1,
   const gpstk::ObsEpoch& rx2)
{
   valid = false;
   dd.clear();
   if (masterPrn.id < 0)
   {
      if (verbosity>2)
         cout << rx1.time.printf(timeFormat)
              << " No master SV selected. Skipping epoch." << endl;
      return;
   }

   double c1 = rx1.rxClock;
   double c2 = rx2.rxClock;
   clockOffset = c1 - c2;
   double eps = 10*std::numeric_limits<double>().epsilon();
   if (std::abs(clockOffset) > 2.1e-3 ||
       std::abs(c1) < eps || std::abs(c2) < eps)
   {
      if (verbosity>2)
         cout << rx1.time.printf(timeFormat)
              << " Insane clock offset (" << 1e3*clockOffset
              << " ms). Skipping epoch." << endl;
      return;
   }

   gpstk::ObsEpoch::const_iterator oi1, oi2;
   oi1 = rx1.find(masterPrn);
   oi2 = rx2.find(masterPrn);

   if (oi1 == rx1.end() || oi2 == rx2.end())
      return;

   const gpstk::SvObsEpoch& rx1obs = oi1->second;
   const gpstk::SvObsEpoch& rx2obs = oi2->second;
   
   OIDM masterDiff = singleDifference(rx1obs, rx2obs);
   if (masterDiff.size() == 0)
      return;

   // Now walk through all prns in track
   for (oi1=rx1.begin(); oi1!=rx1.end(); oi1++)
   {
      gpstk::SatID prn = oi1->first;
      oi2 = rx2.find(prn);
      if (oi2 == rx2.end())
         continue;

      OIDM otherDiff;

      if (prn != masterPrn)
         otherDiff = singleDifference( oi1->second,  oi2->second);

      // Now compute the double differences
      // Note that for the master this will be a single diff
      OIDM::const_iterator i;
      for (i = masterDiff.begin(); i != masterDiff.end(); i++)
         dd[prn][i->first] = i->second - otherDiff[i->first];
   }

   valid = true;
}


// ---------------------------------------------------------------------
// Criteria for the masterPrn
//   it has an elevation > the min, 
//   it it on the way up (i.e. doppler>0),
//   there is a record for it on the other receiver
// ---------------------------------------------------------------------
void DDEpoch::selectMasterPrn(
   const gpstk::ObsEpoch& rx1, 
   const gpstk::ObsEpoch& rx2,
   SvElevationMap& pem)
{
   const double minElevation = 15.0;

   // If there is already one selected, try to keep using that one...
   if (masterPrn.id >0)
   {
      gpstk::ObsEpoch::const_iterator i = rx1.find(masterPrn);
      gpstk::ObsEpoch::const_iterator j = rx2.find(masterPrn);
      if (i != rx1.end() && j != rx2.end() &&
          pem[rx1.time][masterPrn] > minElevation)
         return;
   }

   gpstk::SatID prn;
   gpstk::ObsEpoch::const_iterator i;
   for (i=rx1.begin(); i != rx1.end(); i++)
   {
      prn = i->first;
      gpstk::ObsEpoch::const_iterator j = rx2.find(prn);
      gpstk::SvObsEpoch obs = i->second;
      if (j != rx2.end() && obs[D1] >= 0 &&
          pem[rx1.time][i->first] > minElevation)
      {
         masterPrn = prn;
         break;
      }
   }
}


//-----------------------------------------------------------------------------
// Similiar to computeDD but does a triple difference to look for cycle slips
//-----------------------------------------------------------------------------
void computeDDEpochMap(
   gpstk::ObsEpochMap& rx1,
   gpstk::ObsEpochMap& rx2,
   SvElevationMap& pem,
   const gpstk::EphemerisStore& eph,
   DDEpochMap& ddem)
{
   if (verbosity>1)
      cout << "Computing 2nd differences residuals." << endl;

   DDEpoch prev;

   // We use the data from rx1 walk us through the data
   // loop over all epochs for this station
   gpstk::ObsEpochMap::const_iterator ei1;
   for (ei1=rx1.begin(); ei1!=rx1.end(); ei1++)
   {
      // first make sure we have data from the other receiver for this
      // epoch...
      gpstk::DayTime t = ei1->first;
      gpstk::ObsEpochMap::const_iterator ei2 = rx2.find(t);
      if (ei2 == rx2.end())
      {
         if (verbosity>2)
            cout << t.printf(timeFormat)
                 << " No data in second set. Skipping epoch." << endl;
         continue;
      }
      
      DDEpoch curr;

      // Try to keep using the previous master PRN
      if (prev.valid)
         curr.masterPrn = prev.masterPrn;

      curr.selectMasterPrn(ei1->second, ei2->second, pem);
      curr.doubleDifference(ei1->second, ei2->second);

      if (curr.valid)
      {
         ddem[t] = curr;
         prev = curr;
      }
   } // end of looping over all epochs in the first set.

   // Here we need to remove the double differences for the master PRN
   for (DDEpochMap::iterator i = ddem.begin(); i != ddem.end(); i++)
   {
      DDEpoch& dde = i->second;
      SvOIDM::iterator j = dde.dd.find(dde.masterPrn);
      if (j != dde.dd.end())
         dde.dd.erase(j);
   }

   // Here we add the SV health info to the DDEpochs
   if (typeid(eph) == typeid(gpstk::BCEphemerisStore))
   {
      const gpstk::BCEphemerisStore& bce = 
         dynamic_cast<const gpstk::BCEphemerisStore&>(eph);

      for (DDEpochMap::iterator i = ddem.begin(); i != ddem.end(); i++)
      {
         const gpstk::DayTime& t=i->first;
         DDEpoch& dde = i->second;
         for (SvOIDM::iterator j = dde.dd.begin(); j != dde.dd.end(); j++)
         {
            const gpstk::SatID prn=j->first;
            try
            {
               const gpstk::EngEphemeris& prn_eph = bce.findEphemeris(prn.id, t);
               dde.health[prn] = prn_eph.getHealth();
            } 
            catch (gpstk::EphemerisStore::NoEphemerisFound& e)
            {
               if (verbosity>1)
                  cout << t.printf(timeFormat) << " prn " << prn.id << " no eph " << endl;
            }
         }
      }
   }
}  // end of computeDDEpochMap()


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void dumpStats(
   DDEpochMap& ddem,
   const CycleSlipList& csl,
   SvElevationMap& pem)
{
   cout << endl
        << "ord        elev   stddev    mean      # obs    # bad   # unk  max good  slips" << endl
        << "---------- -----  --------  --------  -------  ------  ------  --------  -----" << endl;

   for (ElevationRangeList::const_iterator i = elr.begin(); i != elr.end(); i++)
   {
      cout << "c1 dd res  " << computeStats(C1, ddem,  *i, pem) << "    " << endl;
      cout << "p1 dd res  " << computeStats(P1, ddem,  *i, pem) << "    " << endl;
      cout << "l1 dd res  " << computeStats(L1, ddem,  *i, pem) << "    " 
           << computeStats(csl, *i, L1) << endl;
      cout << "d1 dd res  " << computeStats(D1, ddem,  *i, pem) << "    " << endl;
      cout << "s1 dd res  " << computeStats(S1, ddem,  *i, pem) << "    " << endl;
      cout << endl;
   }
   cout << "------------------------------------------------------------------------ " << endl;

   for (ElevationRangeList::const_iterator i = elr.begin(); i != elr.end(); i++)
   {
      cout << "p2 dd res  " << computeStats(P2, ddem,  *i, pem) << "    " << endl;
      cout << "l2 dd res  " << computeStats(L2, ddem,  *i, pem) << "    " 
           << computeStats(csl, *i, L2) << endl;
      cout << "d2 dd res  " << computeStats(D2, ddem,  *i, pem) << "    " << endl;
      cout << "s1 dd res  " << computeStats(S2, ddem,  *i, pem) << "    " << endl;
      cout << endl;
   }
   cout << "------------------------------------------------------------------------ " << endl;
}


// ---------------------------------------------------------------------
// Returns a string containing a statistical summary of the double difference
// residuals for the specified obs type within the given elevation range.
// ---------------------------------------------------------------------
string computeStats(
   const gpstk::ObsID oid,
   DDEpochMap& ddem,
   const ElevationRange er,
   SvElevationMap& pem)
{
   ostringstream oss;
   float minElevation = er.first;
   float maxElevation = er.second;
   double strip=1000;
   int zeroCount=0;

   gpstk::Stats<double> good, bad;
   DDEpochMap::iterator ei;
   for (ei = ddem.begin(); ei != ddem.end(); ei++)
   {
      const gpstk::DayTime& t = ei->first;
      SvOIDM::iterator pi;
      for (pi = ei->second.dd.begin(); pi != ei->second.dd.end(); pi++)
      {
         const gpstk::SatID& prn = pi->first;
         OIDM& ddr = pi->second;

         if (pem[t][prn]<minElevation || pem[t][prn]>maxElevation)
            continue;
         if (ddr.find(oid) == ddr.end())
            zeroCount++;
         else
         {
            double mag=std::abs(ddr[oid]);
            if (mag < strip)
               good.Add(ddr[oid]);
            else
               bad.Add(ddr[oid]);
         }
      }
   }
   
   char b1[200];
   char zero = good.Average() < good.StdDev()/sqrt((float)good.N())?'0':' ';
   double maxDD = std::max(std::abs(good.Minimum()), std::abs(good.Maximum()));
   sprintf(b1, "%2d-%2d  %8.5f  %8.3f  %7d  %6d  %6d  %6.2f",
           (int)minElevation, (int)maxElevation,
           good.StdDev()/sqrt((float)2), good.Average(),
           good.N(), bad.N(), zeroCount, maxDD);

   oss << b1;
   return oss.str();
}

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void dump(
   std::ostream& s, 
   DDEpochMap& ddem, 
   SvElevationMap& pem)
{
   if (verbosity>1)
      cout << "Writing raw double differences." << endl;

   s.setf(ios::fixed, ios::floatfield);
   s << "# time              PRN type  elev      ddr/clk(m)       health"
     << endl;

   DDEpochMap::iterator ei;
   SvOIDM::iterator pi;
   for (ei = ddem.begin(); ei != ddem.end(); ei++)
   {
      const gpstk::DayTime& t = ei->first;
      double clk = ei->second.clockOffset;
      string time=t.printf(timeFormat);
      gpstk::SatID& masterPrn = ei->second.masterPrn;
      for (pi = ei->second.dd.begin(); pi != ei->second.dd.end(); pi++)
      {
         const gpstk::SatID& prn = pi->first;
         OIDM& ddr = pi->second;
         for (OIDM::const_iterator ti = ddr.begin(); ti != ddr.end(); ti++)
         {
            const gpstk::ObsID& rot = ti->first;
            double dd = ti->second;
            
            if (std::abs(dd) < 1e-9)
               continue;

            int type=0;
            if      (rot == C1) type=10;
            else if (rot == P1) type=11;
            else if (rot == L1) type=12;
            else if (rot == D1) type=13;
            else if (rot == S1) type=14;
            else if (rot == C2) type=20;
            else if (rot == P2) type=21;
            else if (rot == L2) type=22;
            else if (rot == D2) type=23;
            else if (rot == S2) type=24;

            s << left << setw(20) << time << right
              << setfill(' ')
              << " " << setw(2) << prn.id
              << " " << setw(4) << type
              << " " << setprecision(1) << setw(5)  << pem[t][prn]
              << " " << setprecision(6) << setw(14) << dd
              << hex
              << " " << setw(5) << 0
              << " " << setw(7) << ei->second.health[prn]
              << dec 
              << endl;
         }
      }
   }
   s << endl;
}  // end dump()

