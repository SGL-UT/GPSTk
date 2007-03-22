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

#include <StringUtils.hpp>
#include <Stats.hpp>

#include "DDEpoch.hpp"
#include "ObsID.hpp"

using namespace std;
using namespace gpstk;

unsigned DDEpoch::debugLevel;
unsigned DDEpochMap::debugLevel;

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
OIDM DDEpoch::singleDifference(
   const SvObsEpoch& rx1obs,
   const SvObsEpoch& rx2obs,
   double range)
{
   OIDM diff;

   // clock offset correction
   double coc = clockOffset * range;
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
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void DDEpoch::doubleDifference(
   const ObsEpoch& rx1,
   const ObsEpoch& rx2)
{
   valid = false;
   dd.clear();
   if (masterPrn.id < 0)
   {
      if (debugLevel>1)
         cout << rx1.time
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
         cout << rx1.time
              << " Insane clock offset (" << 1e3*clockOffset
              << " ms). Skipping epoch." << endl;
      return;
   }

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
         cout << "No masterDiff" << endl;
      return;
   }

   // Now walk through all prns in track
   for (oi1=rx1.begin(); oi1!=rx1.end(); oi1++)
   {
      SatID prn = oi1->first;
      oi2 = rx2.find(prn);
      if (oi2 == rx2.end())
         continue;

      OIDM otherDiff;

      if (prn != masterPrn)
         otherDiff = singleDifference( oi1->second,  oi2->second, rangeRate[prn]);

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
}


void DDEpoch::dump(ostream& s) const
{
   s << "master:" << masterPrn
     << ", clockOffset:" << clockOffset
     << endl;

   SvOIDM::const_iterator pi;
   for (pi = dd.begin(); pi != dd.end(); pi++)
   {
      const SatID& prn = pi->first;
      const OIDM& ddr = pi->second;
      for (OIDM::const_iterator ti = ddr.begin(); ti != ddr.end(); ti++)
      {
         const ObsID& rot = ti->first;
         double dd = ti->second;
            
         s << setw(2) << prn.id << " " << setw(4) << rot
           << " " << setprecision(6) << setw(14) << dd
           << endl;
      }
   }
}

//-----------------------------------------------------------------------------
// compute the double difference of all common epochs
//-----------------------------------------------------------------------------
void DDEpochMap::compute(
   const ObsEpochMap& rx1,
   const ObsEpochMap& rx2,
   SvElevationMap& pem)
{
   DDEpoch prev;

   DDEpochMap& ddem=*this;
   DDEpoch::debugLevel = debugLevel;

   if (debugLevel)
      cout << "DDEpochMap::compute(" << rx1.size()
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
            cout << "Epoch with no match" << endl;
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
            if (j->first.type == ObsID::otDoppler && j->first.band == ObsID::cbL1)
            {
               curr.rangeRate[prn] = j->second * C_GPS_M/L1_FREQ;
               break;
            }
         curr.elevation[prn] = pem[t][prn];
      }

      // Try to keep using the previous master PRN
      if (prev.valid)
         curr.masterPrn = prev.masterPrn;

      curr.selectMasterPrn(e1, e2);
      curr.doubleDifference(e1, e2);

      if (curr.valid)
      {
         ddem[t] = curr;
         prev = curr;
      }
      else if (debugLevel)
         cout << "invalid DDEpoch" << endl;
   } // end of looping over all epochs in the first set.

   // Here we need to remove the double differences for the master PRN
   for (DDEpochMap::iterator i = ddem.begin(); i != ddem.end(); i++)
   {
      DDEpoch& dde = i->second;
      SvOIDM::iterator j = dde.dd.find(dde.masterPrn);
      if (j != dde.dd.end())
         dde.dd.erase(j);
   }
}  // end of computeDDEpochMap()



// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void DDEpochMap::dump(
   std::ostream& s) 
{
   DDEpochMap& ddem=*this;

   s.setf(ios::fixed, ios::floatfield);
   s << "# time              PRN    type      mstr  elev     ddr(m)          clk(s)   h"
     << endl;

   DDEpochMap::const_iterator ei;
   for (ei = ddem.begin(); ei != ddem.end(); ei++)
   {
      const DayTime& t = ei->first;
      const DDEpoch& dde = ei->second;

      string time=t.printf("%4Y %3j %02H:%02M:%04.1f");
      const SatID& masterPrn = dde.masterPrn;

      SvOIDM::const_iterator pi;
      for (pi = dde.dd.begin(); pi != dde.dd.end(); pi++)
      {
         const SatID& prn = pi->first;
         const OIDM& ddr = pi->second;
         for (OIDM::const_iterator ti = ddr.begin(); ti != ddr.end(); ti++)
         {
            string rot = StringUtils::asString(ti->first);
            double dd = ti->second;
            
            s << left << setw(20) << time << right
              << setfill(' ')
              << " " << setw(2) << prn.id
              << " " << left << setw(14) << rot << right
              << " " << setw(2) << masterPrn.id
              << " " << setprecision(1) << setw(5)  << dde.elevation[prn]
              << " " << setprecision(6) << setw(14) << dd
              << " " << setprecision(8) << setw(12) << dde.clockOffset
              << hex
              << " " << setw(2) << dde.health[prn]
              << dec 
              << endl;
         }
      }
   }
   s << endl;
}  // end dump()


//-----------------------------------------------------------------------------
// Returns a string containing a statistical summary of the double difference
// residuals for the specified obs type within the given elevation range.
//-----------------------------------------------------------------------------
string DDEpochMap::computeStats(
   const gpstk::ObsID oid,
   const ElevationRange& er,
   SvElevationMap& pem) const
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
      for (pi = ei->second.dd.begin(); pi != ei->second.dd.end(); pi++)
      {
         const gpstk::SatID& prn = pi->first;
         const OIDM& ddr = pi->second;

         if (pem[t][prn]<minElevation || pem[t][prn]>maxElevation)
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

   oss << b1;
   return oss.str();
}
