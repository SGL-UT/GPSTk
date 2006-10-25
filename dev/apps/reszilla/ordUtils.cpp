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

#include "ordUtils.hpp"

#include "LinearClockModel.cpp"

#include "RobustLinearEstimator.hpp"
#include "BivarStats.hpp"

using namespace std;
using namespace gpstk;

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void computeOrds(ORDEpochMap& ordEpochMap,
                 const ObsEpochMap& obsEpochMap,
                 const Triple& ap,
                 const EphemerisStore& eph,
                 const WxObsData& wod,
                 bool svTime,
                 bool keepUnhealthy,
                 bool keepWarts,
                 const string& ordModeStr)
{
   bool dualFreq = false;
   ObsID oid1;
   ObsID oid2;

   if (ordModeStr=="p1p2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcP);
      oid2 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcP);
      dualFreq = true;
   }
   else if (ordModeStr=="c1p2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcP);
      oid2 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcP);
      dualFreq = true;
   }
   else if (ordModeStr=="y1y2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcY);
      oid2 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcY);
      dualFreq = true;
   }
   else if (ordModeStr=="c1")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcCA);
   }
   else if (ordModeStr=="p1")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1,   ObsID::tcP);
   }
   else if (ordModeStr=="c2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcC2LM);
   }
   else if (ordModeStr=="p2")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL2,   ObsID::tcP);
   }
   else if (ordModeStr=="smo")
   {
      oid1 = ObsID(ObsID::otRange,   ObsID::cbL1L2,   ObsID::tcP);
   }
   else if (ordModeStr=="smart")
   {
      const ObsEpoch& oe = obsEpochMap.begin()->second;
      const SvObsEpoch& soe = oe.begin()->second;
      SvObsEpoch::const_iterator itr;
      for (itr = soe.begin(); itr != soe.end(); itr++)
      {
         const ObsID& oid = itr->first;
         if (oid.type != ObsID::otRange)
            continue;
         if (oid.band == ObsID::cbL1)
            oid1 = oid;
         if (oid.band == ObsID::cbL2)
         {
            oid2 = oid;
            dualFreq = true;
         }
         if (oid.band == ObsID::cbL1L2)
         {
            oid1 = oid;
            dualFreq = false;
            svTime = true;
         }
      }
   }
   else
   {
      cout << "Unknown ORD computation requested, mode=" << ordModeStr << endl;
      exit(-1);
   }

   if (verbosity>1)
      cout << "Computing observed range deviations." << endl;

   if (verbosity>1)
   {
      cout << "Using " << oid1;
      if (dualFreq)
         cout << " and " << oid2;
      cout << endl;
   }

   if (RSS(ap[0], ap[1], ap[2]) < 1)
   {
      cout << "Warning! The antenna appears to be within one meter of the" << endl
           << "center of the geoid. This program is not capable of" << endl
           << "accurately estimating the propigation of GNSS signals" << endl
           << "through solids such as a planetary crust or magma. Also," << endl
           << "if this location is correct, your antenna is probally" << endl
           << "no longer in the best of operating condition." << endl;
      return;
   }

   // This is obviously planning for the future.
   ObsClockModel* cm;
   if (true)
      cm = new(EpochClockModel);
   else
      cm = new(LinearClockModel);
   cm->setSigmaMultiplier(1.5);
   cm->setElevationMask(10);

   if (keepUnhealthy)
      cm->setSvMode(ObsClockModel::ALWAYS);
   else
      cm->setSvMode(ObsClockModel::HEALTHY);

   if (verbosity>4)
      ObsRngDev::debug = true;

   GPSGeoid gm;
   ECEF ecef(ap);
   Geodetic geo(ecef, &gm);
   NBTropModel tm(geo.getAltitude(), geo.getLatitude(), obsEpochMap.begin()->first.DOYday());

   ObsEpochMap::const_iterator i;
   for (i=obsEpochMap.begin(); i!=obsEpochMap.end(); i++)
   {
      try
      {
         const DayTime& t = i->first;
         const ObsEpoch& obsEpoch = i->second;

         ORDEpoch& ordEpoch = ordEpochMap[t];
         ordEpoch.time = t;

         // Now set up our trop model for this epoch
         const WxObservation wx = wod.getMostRecent(t);
         if (wx.isAllValid())
            tm.setWeather(wx.temperature, wx.pressure, wx.humidity);

         if (verbosity>3)
            cout << "wx: " << wx << endl
                 << "obs: " << endl << obsEpoch;

         // Walk over all prns in this epoch
         ObsEpoch::const_iterator j;
         for (j=obsEpoch.begin(); j != obsEpoch.end(); j++)
         {
            const SatID svid = j->first;
            const SvObsEpoch& svObsEpoch = j->second;

            SvObsEpoch::const_iterator k;
            double obs1, obs2;

            // first we need to make sure the observation data (in rotm) has
            // the data we require.
            k = svObsEpoch.find(oid1);
            if (k == svObsEpoch.end())
               continue;
            else
               obs1 = k->second;
            
            if (dualFreq)
            {
               k = svObsEpoch.find(oid2);
               if (k == svObsEpoch.end())
                  continue;
               else
                  obs2 = k->second;
            }

            // Now to look for indications that this data is suspect
            if (!keepWarts)
            {
               // A gross check on the pseudorange
               if (obs1 < 15e6)
                  continue;

               // If there is a LLI on any of the data, ignore the whole obs
               bool wonky=false;
               for (k=svObsEpoch.begin(); !wonky && k != svObsEpoch.end(); k++)
                  if (k->first.type == ObsID::otLLI)
                     wonky=true;
               if (wonky)
                  continue;

               if (dualFreq)
               {
                  if (obs2 < 15e6)
                     continue;

                  // Now make sure we have a valid C/A pseudorange
                  k = svObsEpoch.find(C1);
                  if (k == svObsEpoch.end() || k->second < 15e6)
                     continue;
               }
            }

            try
            {
               if (dualFreq)
                  ordEpoch.ords[svid] = ObsRngDev(
                     obs1, obs2, svid, t, ap, eph, gm, tm, svTime);
               else
                  ordEpoch.ords[svid] = ObsRngDev(
                     obs1, svid, t, ap, eph, gm, tm, svTime);
            }
            catch (EphemerisStore::NoEphemerisFound& e)
            {
               if (verbosity>2)
                  cout << e << endl;
            }
         } // end looping over each SV in this epoch

         if (verbosity>3)
            cout << ordEpoch;

         cm->addEpoch(ordEpoch);
         if (verbosity>3)
            cout << "clk: " << *cm << endl;

         if (!cm->isOffsetValid(t))
         {
            if (verbosity>2)
               cout << "Could not estimate clock for epoch at " << t << endl;
            ordEpochMap.erase(ordEpochMap.find(t));
         }
         else
         {
            ordEpoch.applyClockModel(*cm);

            ORDEpoch::ORDMap::iterator pi;
            for (pi = ordEpoch.ords.begin(); pi != ordEpoch.ords.end();)
            {
               const ObsRngDev& ord = pi->second;
               ORDEpoch::ORDMap::iterator pi2=pi;
               pi++;

               if (!keepUnhealthy && ord.getHealth().is_valid() && ord.getHealth())
               {
                  ordEpoch.ords.erase(pi2);
                  if (verbosity>3)
                     cout << "Tossing ord from an unhealty SV." << endl;
                  continue;
               }
               if (keepWarts)
                  continue;
               if (std::abs(ord.getTrop()) > 100 || 
                   ord.getElevation() <= 0.05)
               {
                  ordEpoch.ords.erase(pi2);
                  if (verbosity>1)
                     cout << "Tossing wonky ord: " << ord << endl;
                  continue;
               }
            } // end deleting bad stuff

         } // end check for valid clock offset 
      }
      catch (Exception& e)
      {
         if (verbosity)
            cout << e;
      }

   } // end looping over all epochs

   delete cm;

   if (verbosity>1)
      cout << "Done computing observed range deviations." << endl;
} // end of compute_ords()


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void dumpStats(
   const ORDEpochMap& oem, 
   const string& ordMode, 
   const double sigmam)
{
   cout << endl
        << "ord        elev   stddev    mean      # obs    # bad    max    strip" << endl
        << "---------- -----  --------  --------  -------  ------  ------  ------" << endl;

   string desc = ordMode + " ord  ";

   for (ElevationRangeList::const_iterator i = elr.begin(); i != elr.end(); i++)
      computeStats(desc, oem, *i, sigmam);
}

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void computeStats(
   const string desc,
   const ORDEpochMap& oem,
   const ElevationRange er,
   const double sigmam)
{
   float minElevation = er.first;
   float maxElevation = er.second;

   Stats<double> fp;
   ORDEpochMap::const_iterator ei;
   for (ei = oem.begin(); ei != oem.end(); ei++)
   {
      const DayTime& t = ei->first;
      ORDEpoch::ORDMap::const_iterator pi;
      for (pi = ei->second.ords.begin(); pi != ei->second.ords.end(); pi++)
      {
         const float el = pi->second.getElevation();
         const double ord = pi->second.getORD();
         if (el>minElevation && el<maxElevation)
            fp.Add(ord);
      }
   }

   double strip = sigmam * fp.StdDev();
   Stats<double> good, bad;
   for (ei = oem.begin(); ei != oem.end(); ei++)
   {
      const DayTime& t = ei->first;
      ORDEpoch::ORDMap::const_iterator pi;
      for (pi = ei->second.ords.begin(); pi != ei->second.ords.end(); pi++)
      {
         const float el = pi->second.getElevation();
         const double ord = pi->second.getORD();
         if (el>minElevation && el<maxElevation)
         {
            double mag=std::abs(ord);
            if (mag < strip)
               good.Add(ord);
            else
               bad.Add(ord);
         }
      }
   }

   char b1[200];
   char zero = good.Average() < good.StdDev()/sqrt((float)good.N())?'0':' ';
   double max = std::max(std::abs(good.Maximum()), std::abs(good.Minimum()));
   sprintf(b1, "c1p2 ord   %2d-%2d  %8.5f  %8.3f  %7d  %6d  %6.2f  %6.2f",
           (int)minElevation, (int)maxElevation,
           good.StdDev()/sqrt((float)2), good.Average(),
           good.N(), bad.N(), max, strip);

   cout << b1 << endl;
}

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void estimateClock(const ORDEpochMap& oem, RobustLinearEstimator& rle)
{
   if (verbosity>1)
      cout << "Estimating linear clock with " << oem.size()
           << " epochs of data." <<  endl;

   DoubleDoubleVec clocks;
   ORDEpochMap::const_iterator ei;
   for (ei = oem.begin(); ei != oem.end(); ei++)
   {
      double mjd = ei->first.MJDdate();
      double clk = ei->second.clockOffset;
      if (std::abs(clk) < 1e-6 || !ei->second.clockOffset.is_valid())
         continue;
      std::pair<double, double> pr(mjd, clk);
      clocks.push_back(pr);
   }

   rle.process(clocks);
   DayTime t0(clocks.begin()->first);
   DayTime t1(clocks.rbegin()->first);
   cout << fixed << setprecision(3);
   if (verbosity)
      cout << "RLE clock offset at " << t0.printf(timeFormat)
           << " is "  << rle.eval(t0.MJDdate()) << " meters." << endl
           << "RLE clock offset at " << t1.printf(timeFormat)
           << " is "  << rle.eval(t1.MJDdate()) << " meters." << endl;
   if (verbosity>1)
      cout << "RLE intercept is "  << rle.a << " meters." << endl
           << "RLE stripped Y data at " << rle.medianY << " +/- " << rle.stripY << endl;

   cout << "RLE clock drift rate is " << rle.b << " meters/day." << endl
        << "RLE clock deviation is " << rle.abdev << " meters." << endl;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void dumpOrds(std::ostream& s, const ORDEpochMap& oem)
{
   if (verbosity>1)
      cout << "Writing raw observed ranges deviations." << endl;

   s.setf(ios::fixed, ios::floatfield);
   s << "# time              PRN type  elev     ord/clk(m)  iodc  health"
     << endl;
   s << setfill(' ');

   ORDEpochMap::const_iterator ei;
   for (ei = oem.begin(); ei != oem.end(); ei++)
   {
      const DayTime& t = ei->first;
      string time=t.printf(timeFormat);
      ORDEpoch::ORDMap::const_iterator pi;
      const ORDEpoch& e = ei->second;
      
      for (pi = e.ords.begin(); pi != e.ords.end(); pi++)
      {
         const SatID& svid = pi->first;
         const ObsRngDev& ord = pi->second;
         
         s << left << setw(20) << time << right
           << " " << setw(2) << svid.id
           << " " << setw(4) << 0 // type
           << " " << setprecision(1) << setw(5)  << ord.getElevation()
           << " " << setprecision(5) << setw(14) << ord.getORD()
           << hex
           << " " << setw(5) << ord.getIODC()
           << " " << setw(7) << ord.getHealth()
           << dec
           << endl;
      }
   }
}  // end dumpOrds()


void dumpClock(
   std::ostream& s, 
   const ORDEpochMap& oem,
   const RobustLinearEstimator& clock)
{
   bool gotEstimate = clock.a != 0;

   ORDEpochMap::const_iterator ei;
   s << setfill(' ');
   for (ei = oem.begin(); ei != oem.end(); ei++)
   {
      const DayTime& t = ei->first;
      const ORDEpoch& e = ei->second;
      if (!e.clockOffset)
         continue;
      double clk = e.clockOffset;
      double err = 0;
      bool strip = false;
      if (gotEstimate)
      {
         err = clk - clock.eval(t.MJDdate());
         strip = std::abs(err) > 5*clock.stripY;
      }
      else
         strip = std::abs(clk) > 300000 || std::abs(clk) < 1e-3;

      s << left << setw(20) << t.printf(timeFormat) << right
        << setfill(' ')
        << " " << setw(2) << 0
        << " " << setw(4) << 50 //type
        << " " << setprecision(1) << setw(5)  << 0
        << " " << setprecision(3) << setw(14)  << clk
        << " " << setprecision(3) << setw(8) << err
        << " " << strip
        << endl;
   }

   if (!gotEstimate)
      return;

   DayTime t0(oem.begin()->first);
   DayTime t1(oem.rbegin()->first);

   const int N=8;
   for (int i=0; i<=N; i++)
   {
      DayTime t = t0 + i*(t1-t0)/N;
      s << left << setw(20) << t.printf(timeFormat) << right
        << " " << setw(2) << 0 // prn
        << " " << setw(4) << 51 //type
        << " " << setprecision(1) << setw(5)  << 0 // elevation
        << " " << setprecision(3) << setw(14)  << clock.eval(t.MJDdate())
        << " " << setprecision(3) << setw(8)  << clock.abdev
        << " " << 0
        << endl;
   }
}
