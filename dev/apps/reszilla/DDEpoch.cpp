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
#include <set>

#include <StringUtils.hpp>
#include <Stats.hpp>

#include "DDEpoch.hpp"
#include "ObsID.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

unsigned DDEpoch::debugLevel;
unsigned DDEpochMap::debugLevel;
bool DDEpochMap::useMasterSV;

// ---------------------------------------------------------------------------
// Computes a single difference between two sets of obs
// ---------------------------------------------------------------------------
OIDM DDEpoch::singleDifference(
   const SvObsEpoch& rx1obs,
   const SvObsEpoch& rx2obs,
   double rangeRate)
{
   OIDM diff;

   // clock offset correction
   double coc = clockOffset * rangeRate;
   SvObsEpoch::const_iterator roti1, roti2;
   for (roti1 = rx1obs.begin(); roti1 != rx1obs.end(); roti1++)
   {
      const ObsID& oid = roti1->first;

      // only compute double differences for range, phase, and doppler
      if (!(oid.type == ObsID::otRange
            || oid.type == ObsID::otPhase
            || oid.type == ObsID::otDoppler))
         continue;

      // Make sure we have an obs from the other receiver
      roti2 = rx2obs.find(oid);
      if (roti2 == rx2obs.end())
         continue;

      // Compute the first difference
      diff[oid] = roti1->second - roti2->second;

      // Need to convert the phase/doppler observables to meters
      if (oid.type == ObsID::otPhase || oid.type == ObsID::otDoppler)
      {
         if (oid.band == ObsID::cbL1)
            diff[oid] *=  C_GPS_M/L1_FREQ;
         else
            diff[oid] *=  C_GPS_M/L2_FREQ;
      }
      // Then pull off the clock correction
      diff[oid] -= coc;
   }

   return diff;
} // end of DDEpoch::singleDifference()

// ---------------------------------------------------------------------------
// Sets the valid flag true if successfull
// also sets the masterPrn to the one actually used
// ---------------------------------------------------------------------------
void DDEpoch::doubleDifference(
   const ObsEpoch& rx1,
   const ObsEpoch& rx2)
{
   valid = false;
   
   if (DDEpochMap::useMasterSV)
      ddSvOIDM.clear();
   else
      ddPrOIDM.clear();
      
   if (DDEpochMap::useMasterSV && (masterPrn.id < 0))
   {
      if (debugLevel>1)
         cout << "# " << rx1.time
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
      if (debugLevel)
         cout << "# " << rx1.time
              << " Insane clock offset (" << 1e3*clockOffset
              << " ms). Skipping epoch." << endl;
      return;
   }
   
   if (DDEpochMap::useMasterSV)
   {
      ObsEpoch::const_iterator oi1, oi2;
      oi1 = rx1.find(masterPrn);
      oi2 = rx2.find(masterPrn);

      if (oi1 == rx1.end() || oi2 == rx2.end())
         return;

      const SvObsEpoch& rx1obs = oi1->second;
      const SvObsEpoch& rx2obs = oi2->second;
   
      OIDM masterDiff = singleDifference(rx1obs, rx2obs, rangeRate[masterPrn]);
      if (masterDiff.size() == 0)
      {
         if (debugLevel)
            cout << "# No masterDiff" << endl;
         return;
      }

      // Now walk through all prns in track
      for (oi1=rx1.begin(); oi1!=rx1.end(); oi1++)
      {
         SatID prn  = oi1->first; 
         oi2 = rx2.find(prn);
         if (oi2 == rx2.end())
            continue;
         
         if (prn == masterPrn)
            continue;
            
         OIDM otherDiff = singleDifference( oi1->second,  oi2->second,
                                            rangeRate[prn]);

         // Now compute the double differences
         // Note that for the master this will be a single diff
         OIDM::const_iterator i;
         for (i = masterDiff.begin(); i != masterDiff.end(); i++)
            ddSvOIDM[prn][i->first] = i->second - otherDiff[i->first];
      }
      valid = true;
   }
   else
   {
      for (ObsEpoch::const_iterator oi0 = rx1.begin(); oi0 != rx1.end(); oi0++)
      {
         const SatID sv1 = oi0->first;
         ObsEpoch::const_iterator oi2 = rx2.find(sv1);
         if (oi2 == rx2.end())
            continue;

         const SvObsEpoch& rx1obs = oi0->second;
         const SvObsEpoch& rx2obs = oi2->second;
   
         OIDM d1 = singleDifference(rx1obs, rx2obs, rangeRate[sv1]);
         if (d1.size() == 0)
         {
            if (debugLevel)
               cout << "# DDEpoch::doubleDifferece(): empty d1" << endl;
            continue;
         }

         // Now walk through all other SVs in track
         for (ObsEpoch::const_iterator oi1=oi0; oi1!=rx1.end(); oi1++)
         {
            SatID sv2 = oi1->first;
   
            if (sv1 == sv2)
               continue;
   
            oi2 = rx2.find(sv2);
            if (oi2 == rx2.end())
               continue;
   
            OIDM d2 = singleDifference( oi1->second, oi2->second,
                                        rangeRate[sv2]);
   
            // Now compute the double differences
            SatIdPair pr(sv1, sv2);
            OIDM::const_iterator i;
            for (i = d1.begin(); i != d1.end(); i++)
               ddPrOIDM[pr][i->first] = i->second - d2[i->first];
         }
      }   
      valid = true;   
   }
} // end of DDEpoch::doubleDifference()

// ---------------------------------------------------------------------------
// Criteria for the masterPrn:
//   it has an elevation > the min, 
//   it it on the way up (i.e. doppler>0),
//   t4 is a record for it on the other receiver
// ---------------------------------------------------------------------------
void DDEpoch::selectMasterPrn(
   const ObsEpoch& rx1, 
   const ObsEpoch& rx2)
{
   const double minElevation = 15.0;

   // If there is already one selected, try to keep using that one...
   if (masterPrn.id >0)
   {
      ObsEpoch::const_iterator i = rx1.find(masterPrn);
      ObsEpoch::const_iterator j = rx2.find(masterPrn);
      if (i != rx1.end() && j != rx2.end() &&
          elevation[masterPrn] > minElevation)
         return;
   }

   SatID prn;
   ObsEpoch::const_iterator i;
   for (i=rx1.begin(); i != rx1.end(); i++)
   {
      prn = i->first;
      ObsEpoch::const_iterator j = rx2.find(prn);
      SvObsEpoch obs = i->second;
      if (j != rx2.end() && rangeRate[prn] >= 0 &&
          elevation[i->first] > minElevation)
      {
         masterPrn = prn;
         break;
      }
   }
} // end of DDEpoch::selectMasterPrn()


//----------------------------------------------------------------------------
// compute the double difference of all common epochs
//----------------------------------------------------------------------------
void DDEpochMap::compute(
   const ObsEpochMap& rx1,
   const ObsEpochMap& rx2,
   SvElevationMap& pem)
{
   DDEpoch prev;

   DDEpochMap& ddem=*this;
   DDEpoch::debugLevel = debugLevel;

   if (debugLevel)
      cout << "# DDEpochMap::compute(" << rx1.size()
           << ", " << rx2.size() << " epochs)" << endl;

   // We use the data from rx1 walk us through the data
   // loop over all epochs for this station
   ObsEpochMap::const_iterator ei1;
   for (ei1=rx1.begin(); ei1!=rx1.end(); ei1++)
   {
      // first make sure we have data from the other receiver for this epoch...
      DayTime t = ei1->first;
      ObsEpochMap::const_iterator ei2 = rx2.find(t);
      if (ei2 == rx2.end())
      {
         if (debugLevel>1)
            cout << "# Epoch with no match" << endl;
         continue;
      }
      const ObsEpoch& e1 = ei1->second;
      const ObsEpoch& e2 = ei2->second;
      
      DDEpoch curr;

      // We need to have a range rate but it doesn't really matter where from.
      // So here we find a doppler for each of the SVs
      // Also we fill in the elevation while we are walking through
      // the SVs.
      for (ObsEpoch::const_iterator i=e1.begin(); i != e1.end(); i++)
      {
         const SatID& prn = i->first;
         const SvObsEpoch& obs = i->second;
         for(SvObsEpoch::const_iterator j=obs.begin(); j != obs.end(); j++)
            if (j->first.type == ObsID::otDoppler && 
                j->first.band == ObsID::cbL1)
            {
               curr.rangeRate[prn] = j->second * C_GPS_M/L1_FREQ;
               break;
            }
         curr.elevation[prn] = pem[t][prn];
      }

      if (useMasterSV)
      {
         // Try to keep using the previous master PRN
         if (prev.valid)
            curr.masterPrn = prev.masterPrn;

         curr.selectMasterPrn(e1, e2);
      }
      
      curr.doubleDifference(e1, e2);

      if (curr.valid)
      {
         ddem[t] = curr;
         prev = curr;
      }
      else if (debugLevel)
         cout << "# Invalid DDEpoch" << endl;
   } // end of looping over all epochs in the first set.

   if (useMasterSV)
   {
      // Here we need to remove the double differences for the master PRN
      for (DDEpochMap::iterator i = ddem.begin(); i != ddem.end(); i++)
      {
         DDEpoch& dde = i->second;
         SvOIDM::iterator j = dde.ddSvOIDM.find(dde.masterPrn);
         if (j != dde.ddSvOIDM.end())
            dde.ddSvOIDM.erase(j);
      }
   }
}  // end of DDEpochMap::compute()

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void DDEpochMap::dump(std::ostream& s) const
{
   const DDEpochMap& ddem=*this;

   s.setf(ios::fixed, ios::floatfield);
   s << "# time               obs type       SV1 SV2   EL1     EL2"
     << "           ddr(m)  h1h2"
     << endl;

   DDEpochMap::const_iterator ei;
   for (ei = ddem.begin(); ei != ddem.end(); ei++)
   {
      const DayTime& t = ei->first;
      const DDEpoch& dde = ei->second;

      string time=t.printf("%4Y %3j %02H:%02M:%04.1f");
      
      if (useMasterSV)
      {
         const SatID& masterPrn = dde.masterPrn;
         
         SvOIDM::const_iterator pi;
         for (pi = dde.ddSvOIDM.begin(); pi != dde.ddSvOIDM.end(); pi++)
         {
            const SatID& prn = pi->first;
            const OIDM& ddr = pi->second;
            for (OIDM::const_iterator ti = ddr.begin(); ti != ddr.end(); ti++)
            {
               string rot = StringUtils::asString(ti->first);
               double dd = ti->second;
               
               // don't output single differnce b/w master and itself.
               // this is excluded from the stats computation as well
               if (masterPrn.id != prn.id)
               {
                  s << left << setw(20) << time << right
                    << setfill(' ') << setprecision(2)
                    << " " << left << setw(14) << rot << right
                    << " " << setw(3) << masterPrn.id
                    << " " << setw(3) << prn.id
                    << "   " << setw(5) << dde.elevation[masterPrn] << "  "
                    << " " << setw(5) << dde.elevation[prn]
                    << " " << setprecision(6) << setw(14) << dd
                    << hex
                    << " " << setw(2) << dde.health[masterPrn] 
                    << dde.health[prn]
                    << dec 
                    << endl;
               }
               
               if (debugLevel && (dde.health[masterPrn] || dde.health[prn]))
                  cout << "# Unhealthy SV. Master/SV2: " << hex << setw(2) 
                       << dde.health[masterPrn] << dde.health[prn] << dec 
                       << endl;    
            }
         }
      }
      else
      {
         PrOIDM::const_iterator pi;
         for (pi = dde.ddPrOIDM.begin(); pi != dde.ddPrOIDM.end(); pi++)
         {
            const SatIdPair& pr = pi->first;
            const SatID& sv1 = pr.first;
            const SatID& sv2 = pr.second;
            const OIDM& ddr = pi->second;
            for (OIDM::const_iterator ti = ddr.begin(); ti != ddr.end(); ti++)
            {
               string rot = StringUtils::asString(ti->first);
               double dd = ti->second;
               
               s << left << setw(20) << time << right
                 << setfill(' ') << setprecision(2)
                 << " " << left << setw(14) << rot << right
                 << " " << setw(3) << sv1.id 
                 << " " << setw(3) << sv2.id
                 << "   " << setw(5) << dde.elevation[sv1] << "  "
                 << " " << setw(5) << dde.elevation[sv2]
                 << " " << setprecision(6) << setw(14) << dd
                 << hex
                 << " " << setw(2) << dde.health[sv1] << dde.health[sv2]
                 << dec
                 << endl;
            }
         }
      }
   }
}  // end of DDEpochMap::dump()


//----------------------------------------------------------------------------
// Returns a string containing a statistical summary of the double difference
// residuals for the specified obs type within the given elevation range.
//----------------------------------------------------------------------------
string DDEpochMap::computeStats(
   const gpstk::ObsID oid,
   const ElevationRange& er) const
{
   ostringstream oss;
   float minElevation = er.first;
   float maxElevation = er.second;
   double strip=1000;
   int zeroCount=0;

   gpstk::Stats<double> good, bad;
   for (const_iterator ei = begin(); ei != end(); ei++)
   {
      const gpstk::DayTime& t = ei->first;
      SvOIDM::const_iterator pi;
      const DDEpoch& dde = ei->second;
      for (pi = dde.ddSvOIDM.begin(); pi != dde.ddSvOIDM.end(); pi++)
      {
         const gpstk::SatID& prn = pi->first;
         const OIDM& ddr = pi->second;
         
         if (prn == dde.masterPrn)
            continue;
         
         if (dde.elevation[prn]<minElevation ||
             dde.elevation[prn]>maxElevation)
            continue;

         OIDM::const_iterator ddi = ddr.find(oid);
         if (ddi == ddr.end())
            zeroCount++;
         else
         {
            double dd = ddi->second;
            if (std::abs(dd) < strip)
               good.Add(dd);
            else
               bad.Add(dd);
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

   oss << setw(14) << left << asString(oid) << right << b1 << endl;
   return oss.str();
}  // end of DDEpochMap::computeStats()


//----------------------------------------------------------------------------
// Returns a string containing a statistical summary of the double difference
// residuals for the specified obs type within the given elevation range.
//----------------------------------------------------------------------------
string DDEpochMap::computeStatsForAllCombos(
   const ObsID oid,
   const ElevationRange& er) const
{
   float minElevation = er.first;
   float maxElevation = er.second;
   double strip=1000;
   int zeroCount=0;

   gpstk::Stats<double> good, bad;
   for (const_iterator ei = begin(); ei != end(); ei++)
   {
      const gpstk::DayTime& t = ei->first;
      PrOIDM::const_iterator pi;
      SvDoubleMap& elevation = ei->second.elevation;
      for (pi = ei->second.ddPrOIDM.begin(); 
           pi != ei->second.ddPrOIDM.end(); pi++)
      {
         const SatIdPair& pr = pi->first;
         const gpstk::SatID& sv1 = pr.first;
         const gpstk::SatID& sv2 = pr.second;
         const OIDM& ddr = pi->second;

         if (elevation[sv1]<minElevation || elevation[sv1]>maxElevation ||
             elevation[sv2]<minElevation || elevation[sv2]>maxElevation)
            continue;

         OIDM::const_iterator ddi = ddr.find(oid);
         if (ddi == ddr.end())
            zeroCount++;
         else
         {
            double dd = ddi->second;
            if (std::abs(dd) < strip)
               good.Add(dd);
            else
               bad.Add(dd);
         }
      }
   }
   
   ostringstream oss;
   if (good.N() > 0 || bad.N() >0)
   {
      char b1[200];
      char zero = good.Average() < good.StdDev()/sqrt((float)good.N())?'0':' ';
      double maxDD = std::max(std::abs(good.Minimum()),
                     std::abs(good.Maximum()));
      sprintf(b1, "%2d-%2d  %8.5f  %8.3f  %7d  %6d  %6d  %6.2f",
              (int)minElevation, (int)maxElevation,
              good.StdDev()/sqrt((float)2), good.Average(),
              good.N(), bad.N(), zeroCount, maxDD);
      oss << setw(14) << left << asString(oid) << right << b1 << endl;
   }

   return oss.str();
}  // end of DDEpochMap::computeStatsForAllCombos()

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DDEpochMap::outputStats(ostream& s, const ElevationRangeList elr) const
{
   // First figure out what obs types we have to work with
   if (debugLevel)
      cout << "# Computing obsSet" << endl;

   set<ObsID> obsSet;
   for (const_iterator ei = begin(); ei != end(); ei++)
   {
      const DDEpoch& dde = ei->second;
      for (SvOIDM::const_iterator pi = dde.ddSvOIDM.begin(); 
          pi != dde.ddSvOIDM.end(); pi++)
      {
         const OIDM& ddr = pi->second;
         for (OIDM::const_iterator ti = ddr.begin(); ti != ddr.end(); ti++)
            obsSet.insert(ti->first);
      }
   }

   s << endl
     << "ObsID         elev    stddev     mean    # obs      # bad   "
     << "# unk  max good  slips" 
     << endl
     << "------------- -----  --------  --------  -------    ------  "
     << "------ --------  -----" 
     << endl;
     
   // For convience, group these into L1
   for (ElevationRangeList::const_iterator i = elr.begin(); 
        i != elr.end(); i++)
   {
      for (set<ObsID>::const_iterator j = obsSet.begin(); 
           j != obsSet.end(); j++)
         if (j->band == ObsID::cbL1)
            s << computeStats(*j, *i);
      s << endl;
   }
   s << "------------------------------------"
     << "------------------------------------"
     << endl << endl;

   // and L2
   for (ElevationRangeList::const_iterator i = elr.begin(); 
        i != elr.end(); i++)
   {
      for (set<ObsID>::const_iterator j = obsSet.begin(); 
           j != obsSet.end(); j++)
         if (j->band == ObsID::cbL2)
            s << computeStats(*j, *i);
      s << endl;
   }
   s << "------------------------------------"
     << "------------------------------------"
     << endl << endl;
     
}  // end of DDEpochMap::outputStats()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DDEpochMap::outputStatsForAllCombos(ostream& s, 
                                         const ElevationRangeList elr) const
{
   // First figure out what obs types we have to work with
   if (debugLevel)
      cout << "# Computing obsSet" << endl;

   set<ObsID> obsSet;
   for (const_iterator ei = begin(); ei != end(); ei++)
   {
      const DDEpoch& dde = ei->second;
      for (PrOIDM::const_iterator pi = dde.ddPrOIDM.begin(); 
           pi != dde.ddPrOIDM.end(); pi++)
      {
         const OIDM& ddr = pi->second;
         for (OIDM::const_iterator ti = ddr.begin(); ti != ddr.end(); ti++)
            obsSet.insert(ti->first);
      }
   }

   s << endl
     << "ObsID         elev    stddev     mean    # obs      # bad   "
     << "# unk  max good  slips" << endl
     << "------------- -----  --------  --------  -------    ------  "
     << "------ --------  -----";

   // For convience, group these into L1
   for (ElevationRangeList::const_iterator i = elr.begin(); 
        i != elr.end(); i++)
   {
      for (set<ObsID>::const_iterator j = obsSet.begin(); 
           j != obsSet.end(); j++)
         if (j->band == ObsID::cbL1)
            s << computeStatsForAllCombos(*j, *i);
      s << endl;
   }
   
   s << "------------------------------------"
     << "------------------------------------"
     << endl;

   // and L2
   for (ElevationRangeList::const_iterator i = elr.begin(); 
        i != elr.end(); i++)
   {
      for (set<ObsID>::const_iterator j = obsSet.begin(); 
           j != obsSet.end(); j++)
         if (j->band == ObsID::cbL2)
            s << computeStatsForAllCombos(*j, *i);
      s << endl;
   }
   
   s << "------------------------------------"
     << "------------------------------------" 
     << endl << endl;
     
}  // end of DDEpochMap::outputStatsForAllCombos()


void DDEpochMap::outputAverages(ostream& s) const
{
   if (debugLevel)
      cout << "# Computing averages\n";
      
   s << "# window end time        obs type    # points    mean ddr(m)\n";
      
   // only going to compute averages for range, phase, and doppler
   gpstk::Stats<double> l1CArange,l1Prange,l1Phase,l1Doppler;
   gpstk::Stats<double> l2Prange,l2Phase,l2Doppler;

   gpstk::DayTime windowTempDT;
   
   const_iterator ei = begin();
   const gpstk::DayTime& dataStartDT = ei->first;
   gpstk::DayTime windowEndDT = dataStartDT + windowLength;
   
   for (const_iterator ei = begin(); ei != end(); ei++)
   {
      const gpstk::DayTime& t = ei->first;
      const DDEpoch& dde = ei->second;

      // the time for this DDEpoch
      windowTempDT = t;
      
      if (windowTempDT < windowEndDT)
      {
         // record data for this epoch
         SvOIDM::const_iterator pi;
         for (pi = dde.ddSvOIDM.begin(); pi != dde.ddSvOIDM.end(); pi++)
         {
            const OIDM& ddr = pi->second;
            for (OIDM::const_iterator ti = ddr.begin(); ti != ddr.end(); ti++)
            {
               gpstk::ObsID obsID = ti->first;
               double dd          = ti->second;
              
               if ((obsID.band == ObsID::cbL1) &&
                   (obsID.type == ObsID::otRange) &&
                   (obsID.code == ObsID::tcCA))
                  l1CArange.Add(dd);
               else if ((obsID.band == ObsID::cbL1) &&
                        (obsID.type == ObsID::otRange))
                  l1Prange.Add(dd);
               else if ((obsID.band == ObsID::cbL1) &&
                        (obsID.type == ObsID::otPhase))
                  l1Phase.Add(dd);
               else if ((obsID.band == ObsID::cbL1) &&
                        (obsID.type == ObsID::otDoppler))
                  l1Doppler.Add(dd);
               else if ((obsID.band == ObsID::cbL2) &&
                        (obsID.type == ObsID::otRange))
                  l2Prange.Add(dd);
               else if ((obsID.band == ObsID::cbL2) &&
                        (obsID.type == ObsID::otPhase))
                  l2Phase.Add(dd);
               else if ((obsID.band == ObsID::cbL2) &&
                        (obsID.type == ObsID::otDoppler))   
                  l2Doppler.Add(dd);
            }
         }    
      }
      else
      {
         // reset window end time
         windowEndDT = windowTempDT + windowLength;
         
         // compute and output stats for previous window
         string time = windowTempDT.printf("%4Y %3j %02H:%02M:%04.1f");
         
         s << ">a " << left << setw(20) << time 
           << setfill(' ') << setprecision(2) << " " << setw(16) 
           << "L1 C/A range" << setw(6) << l1CArange.N()
           << " " << right << setprecision(6) << setw(14) 
           << l1CArange.Average() << endl;
         
         s << ">a " << left << setw(20) << time 
           << setfill(' ') << setprecision(2) << " " << setw(16) 
           << "L1 P/Y range" << setw(6) << l1Prange.N()
           << " " <<  right << setprecision(6) << setw(14) 
           << l1Prange.Average() << endl;
         
         s << ">a " << left << setw(20) << time 
           << setfill(' ') << setprecision(2) << " " << setw(16) 
           << "L1 P/Y phase" << setw(6) << l1Phase.N()
           << " " << right << setprecision(6) << setw(14) 
           << l1Phase.Average() << endl;
         
         s << ">a " << left << setw(20) << time
           << setfill(' ') << setprecision(2) << " " << setw(16) 
           << "L1 P/Y doppl" << setw(6) << l1Doppler.N()
           << " " << right << setprecision(6) << setw(14) 
           << l1Doppler.Average() << endl;  
         
         s << ">a " << left << setw(20) << time
           << setfill(' ') << setprecision(2) << " "  << setw(16) 
           << "L2 P/Y range" << setw(6) << l2Prange.N()
           << " " << right << setprecision(6) << setw(14) 
           << l2Prange.Average() << endl;
         
         s << ">a " << left << setw(20) << time
           << setfill(' ') << setprecision(2) << " " << setw(16) 
           << "L2 P/Y phase" << setw(6) << l2Phase.N()
           << " " << right << setprecision(6) << setw(14) 
           << l2Phase.Average() << endl;
         
         s << ">a " << left << setw(20) << time
           << setfill(' ') << setprecision(2) << " " << setw(16) 
           << "L2 P/Y doppl" << setw(6) << l2Doppler.N()
           << " " << right << setprecision(6) << setw(14) 
           << l2Doppler.Average() << endl;
               

         // reset objects to clear them and start recording data 
         // for this new window
         l1CArange.Reset();
         l1Prange.Reset();
         l1Phase.Reset();
         l1Doppler.Reset();
         l2Prange.Reset();
         l2Phase.Reset();
         l2Doppler.Reset();
      }
   
   }
}  // end of DDEpochMap::outputAverages()

