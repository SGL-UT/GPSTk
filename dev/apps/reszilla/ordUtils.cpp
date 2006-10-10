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

#include "BCEphemerisStore.hpp"

#include "util.hpp"

#include "ordUtils.hpp"

#include "LinearClockModel.cpp"

#include "RobustLinearEstimator.hpp"
#include "BivarStats.hpp"

using namespace std;

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void computeOrds(ORDEpochMap& oem,
                 const RODEpochMap& rem,
                 const gpstk::RinexObsHeader& roh,
                 const gpstk::EphemerisStore& eph,
                 const gpstk::WxObsData& wod,
                 bool svTime,
                 bool keepUnhealthy,
                 bool keepWarts,
                 const string& ordModeStr)
{
   bool dualFreq=false;
   RinexObsType p1,p2;

   if      (ordModeStr=="p1p2" || ordModeStr=="c1p2") dualFreq=true;

   if      (ordModeStr=="p1p2") p1=P1, p2=P2;
   else if (ordModeStr=="c1p2") p1=C1, p2=P2;
   else if (ordModeStr=="c1")   p1=C1;
   else if (ordModeStr=="p1")   p1=P1;
   else if (ordModeStr=="p2")   p1=P2;
   else
   {
      cout << "Unknown ORD computation requested, mode=" << ordModeStr << endl;
      return;
   }

   if (verbosity>1)
      cout << "Computing observed range deviations." << endl;

   if (gpstk::RSS(roh.antennaPosition[0],
           roh.antennaPosition[1],
           roh.antennaPosition[2]) < 1)
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
   gpstk::ObsClockModel* cm;
   if (true)
      cm = new(gpstk::EpochClockModel);
   else
      cm = new(gpstk::LinearClockModel);
   cm->setSigmaMultiplier(1.5);
   cm->setElevationMask(10);

   if (keepUnhealthy)
      cm->setPRNMode(gpstk::ObsClockModel::ALWAYS);
   else
      cm->setPRNMode(gpstk::ObsClockModel::HEALTHY);

   if (verbosity>4)
      gpstk::ObsRngDev::debug = true;

   gpstk::GPSGeoid gm;
   gpstk::ECEF ap(roh.antennaPosition);
   gpstk::Geodetic geo(ap, &gm);
   gpstk::NBTropModel tm(geo.getAltitude(), geo.getLatitude(), roh.firstObs.DOYday());

   RODEpochMap::const_iterator i;
   for (i=rem.begin(); i!=rem.end(); i++)
   {
      const gpstk::DayTime& t = i->first;
      const gpstk::RinexObsData& rod = i->second;

      gpstk::ORDEpoch& oe = oem[t];
      oe.time = t;

      // Now set up our trop model for this epoch
      const gpstk::WxObservation wx = wod.getMostRecent(t);
      if (verbosity>3)
         cout << "wx: " << wx << endl;
      if (wx.isAllValid())
         tm.setWeather(wx.temperature, wx.pressure, wx.humidity);

      // Walk over all prns in this epoch
      gpstk::RinexObsData::RinexSatMap::const_iterator rpi;
      for (rpi=rod.obs.begin(); rpi!=rod.obs.end(); rpi++)
      {
         short prn = rpi->first.id;

         const gpstk::RinexObsData::RinexObsTypeMap& rotm = rpi->second;
         gpstk::RinexObsData::RinexObsTypeMap::const_iterator itr;
         double obs1, obs2;

         // first we need to make sure the observation data (in rotm) has
         // the data we require.
         itr = rotm.find(p1);
         if (itr == rotm.end())
            continue;
         else
            obs1 = itr->second.data;

         if (dualFreq)
         {
            itr = rotm.find(p2);
            if (itr == rotm.end())
               continue;
            else
               obs2 = itr->second.data;
         }

         // Now to look for indications that this data is suspect
         if (!keepWarts)
         {
            // A gross check on the pseudorange
            if (obs1 < 15e6 || (dualFreq && obs2 < 15e6))
                continue;
            
            itr = rotm.find(p1);
            if (itr->second.lli)
               continue;

            itr = rotm.find(p2);
            if (itr->second.lli)
               continue;

            // Now make sure we have a valid C/A pseudorange
            itr = rotm.find(C1);
            if (itr == rotm.end() || itr->second.data < 15e6 || itr->second.lli)
               continue;
         }

         try
         {
            if (dualFreq)
               oe.ords[prn] = gpstk::ObsRngDev(obs1, obs2, prn, t, ap, eph, gm, tm, svTime);
            else
               oe.ords[prn] = gpstk::ObsRngDev(obs1, prn, t, ap, eph, gm, tm, svTime);
         }
         catch (gpstk::EphemerisStore::NoEphemerisFound& e)
         {
            if (verbosity>2)
               cout << e.getText() << endl;
         }
      } // end looping over each SV in this epoch

      cm->addEpoch(oe);
      if (verbosity>3)
         cout << "clk: " << *cm << endl;

      if (verbosity>1 && !cm->isOffsetValid(t))
      {
         cout << "Could not estimate clock for epoch at " << t << endl;
         oem.erase(oem.find(t));
      }
      else
      {
         oe.applyClockModel(*cm);

         gpstk::ORDEpoch::ORDMap::const_iterator pi;
         bool wonky=false;
         for (pi = oe.ords.begin(); pi != oe.ords.end(); pi++)
         {
            const double ord = pi->second.getORD();
            short prn = pi->first;
            if (pi->second.getHealth().is_valid() && 
                pi->second.getHealth() && !keepUnhealthy)
               continue;

            if (std::abs(ord) > 1e3)
               wonky = true;
         }

         if (wonky && verbosity)
         {
            cout << "wonky stuff found." << endl;
            rod.dump(cout);
            cout << oe  << endl
                 << *cm << endl;
         }
         else if (verbosity>3)
            cout << "oe: " << oe;
      }
   } // end looping over all epochs

   // Now go through and delete all the epochs with invalid clocks
   ORDEpochMap::iterator itr;
   for (itr=oem.begin(); itr != oem.end();)
   {
      ORDEpochMap::iterator j=itr;
      itr++;
      if (!j->second.validClock)
         oem.erase(j);
   }

   delete cm;
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
   int zeroCount=0;

   gpstk::Stats<double> fp;
   ORDEpochMap::const_iterator ei;
   for (ei = oem.begin(); ei != oem.end(); ei++)
   {
      const gpstk::DayTime& t = ei->first;
      gpstk::ORDEpoch::ORDMap::const_iterator pi;
      bool wonky=false;
      for (pi = ei->second.ords.begin(); pi != ei->second.ords.end(); pi++)
      {
         const float el = pi->second.getElevation();
         const double ord = pi->second.getORD();
         if (el>minElevation && el<maxElevation)
            fp.Add(ord);
      }
   }

   double strip = sigmam * fp.StdDev();
   gpstk::Stats<double> good, bad;
   for (ei = oem.begin(); ei != oem.end(); ei++)
   {
      const gpstk::DayTime& t = ei->first;
      gpstk::ORDEpoch::ORDMap::const_iterator pi;
      for (pi = ei->second.ords.begin(); pi != ei->second.ords.end(); pi++)
      {
         const float el = pi->second.getElevation();
         const double ord = pi->second.getORD();
         if (el>minElevation && el<maxElevation)
         {
            double mag=std::abs(ord);
            if (mag < strip && mag>1e-6)
               good.Add(ord);
            else if (mag>=strip)
               bad.Add(ord);
            else
               zeroCount++;
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
      cout << "Estimating linear clock" << endl;

   DoubleDoubleVec clocks;
   ORDEpochMap::const_iterator ei;
   for (ei = oem.begin(); ei != oem.end(); ei++)
   {
      double mjd = ei->first.MJDdate();
      double clk = ei->second.clockOffset;
      if (std::abs(clk) < 1e-6 || !ei->second.validClock)
         continue;
      std::pair<double, double> pr(mjd, clk);
      clocks.push_back(pr);
   }

   rle.process(clocks);
   gpstk::DayTime t0(clocks.begin()->first);
   gpstk::DayTime t1(clocks.rbegin()->first);
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
      const gpstk::DayTime& t = ei->first;
      string time=t.printf(timeFormat);
      gpstk::ORDEpoch::ORDMap::const_iterator pi;
      const gpstk::ORDEpoch& e = ei->second;
      
      for (pi = e.ords.begin(); pi != e.ords.end(); pi++)
      {
         const short& prn = pi->first;
         const gpstk::ObsRngDev& ord = pi->second;
         
         s << left << setw(20) << time << right
           << " " << setw(2) << prn
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
      const gpstk::DayTime& t = ei->first;
      const gpstk::ORDEpoch& e = ei->second;
      if (!e.validClock)
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

   gpstk::DayTime t0(oem.begin()->first);
   gpstk::DayTime t1(oem.rbegin()->first);

   const int N=8;
   for (int i=0; i<=N; i++)
   {
      gpstk::DayTime t = t0 + i*(t1-t0)/N;
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
