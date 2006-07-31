#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/DDEpoch.cpp#8 $"

#include <limits>

#include "BCEphemerisStore.hpp"

#include "DDEpoch.hpp"

using namespace std;


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
ROTDM DDEpoch::singleDifference(
   const gpstk::RinexObsData::RinexObsTypeMap& rx1obs,
   const gpstk::RinexObsData::RinexObsTypeMap& rx2obs)
{
   ROTDM diff;

   gpstk::RinexObsData::RinexObsTypeMap::const_iterator d1_itr = rx1obs.find(D1);
   if (d1_itr == rx1obs.end())
      return diff;

   // clock offset correction
   double coc = clockOffset * d1_itr->second.data * gpstk::C_GPS_M/gpstk::L1_FREQ; 
   gpstk::RinexObsData::RinexObsTypeMap::const_iterator roti1, roti2;
   for (roti1 = rx1obs.begin(); roti1 != rx1obs.end(); roti1++)
   {
      const RinexObsType& rot = roti1->first;

      // Make sure we have an obs from the other receiver
      roti2 = rx2obs.find(rot);
      if (roti2 == rx2obs.end())
         continue;

      // Compute the first difference
      diff[rot] = roti1->second.data - roti2->second.data;

      // Need to convert the phase/doppler observables to meters
      if (rot == L1 || rot == D1)
         diff[rot] *=  gpstk::C_GPS_M/gpstk::L1_FREQ;
      if (rot == L2 || rot == D2)
         diff[rot] *=  gpstk::C_GPS_M/gpstk::L2_FREQ;

      // Then pull off the clock correction
      diff[rot] -= coc;
   }

   return diff;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void DDEpoch::doubleDifference(
   const gpstk::RinexObsData& rx1,
   const gpstk::RinexObsData& rx2)
{
   valid = false;
   dd.clear();
   if (masterPrn.prn < 0)
   {
      if (verbosity>2)
         cout << rx1.time.printf(timeFormat)
              << " No master SV selected. Skipping epoch." << endl;
      return;
   }

   double c1 = rx1.clockOffset;
   double c2 = rx2.clockOffset;
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

   gpstk::RinexObsData::RinexPrnMap::const_iterator oi1, oi2;
   oi1 = rx1.obs.find(masterPrn);
   oi2 = rx2.obs.find(masterPrn);

   if (oi1 == rx1.obs.end() || oi2 == rx2.obs.end())
      return;

   const gpstk::RinexObsData::RinexObsTypeMap& rx1obs = oi1->second;
   const gpstk::RinexObsData::RinexObsTypeMap& rx2obs = oi2->second;
   
   ROTDM masterDiff = singleDifference(rx1obs, rx2obs);
   if (masterDiff.size() == 0)
      return;

   // Now walk through all prns in track
   for (oi1=rx1.obs.begin(); oi1!=rx1.obs.end(); oi1++)
   {
      gpstk::RinexPrn prn = oi1->first;
      oi2 = rx2.obs.find(prn);
      if (oi2 == rx2.obs.end())
         continue;

      ROTDM otherDiff;

      if (prn != masterPrn)
         otherDiff = singleDifference( oi1->second,  oi2->second);

      // Now compute the double differences
      // Note that for the master this will be a single diff
      for (ROTDM::const_iterator roti = masterDiff.begin(); roti != masterDiff.end(); roti++)
         dd[prn][roti->first] = roti->second - otherDiff[roti->first];
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
   const gpstk::RinexObsData& rx1, 
   const gpstk::RinexObsData& rx2,
   PrnElevationMap& pem)
{
   const double minElevation = 15.0;

   // If there is already one selected, try to keep using that one...
   if (masterPrn.prn >0)
   {
      RinexPrnMap::const_iterator i = rx1.obs.find(masterPrn);
      RinexPrnMap::const_iterator j = rx2.obs.find(masterPrn);
      if (i != rx1.obs.end() && j != rx2.obs.end() &&
          pem[rx1.time][masterPrn] > minElevation)
         return;
   }

   gpstk::RinexPrn prn;
   for (RinexPrnMap::const_iterator i=rx1.obs.begin(); i != rx1.obs.end(); i++)
   {
      prn = i->first;
      RinexPrnMap::const_iterator j = rx2.obs.find(prn);
      RinexObsTypeMap obs = i->second;
      if (j != rx2.obs.end() && obs[D1].data >= 0 &&
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
   RODEpochMap& rx1,
   RODEpochMap& rx2,
   PrnElevationMap& pem,
   const gpstk::EphemerisStore& eph,
   DDEpochMap& ddem)
{
   if (verbosity>1)
      cout << "Computing 2nd differences residuals." << endl;

   DDEpoch prev;

   // We use the data from rx1 walk us through the data
   // loop over all epochs for this station
   for (RODEpochMap::const_iterator ei1=rx1.begin(); ei1!=rx1.end(); ei1++)
   {
      // first make sure we have data from the other receiver for this
      // epoch...
      gpstk::DayTime t = ei1->first;
      RODEpochMap::const_iterator ei2 = rx2.find(t);
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
      PrnROTDM::iterator j = dde.dd.find(dde.masterPrn);
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
         for (PrnROTDM::iterator j = dde.dd.begin(); j != dde.dd.end(); j++)
         {
            const gpstk::RinexPrn prn=j->first;
            try
            {
               const gpstk::EngEphemeris& prn_eph = bce.findEphemeris(prn.prn, t);
               dde.health[prn] = prn_eph.getHealth();
            } 
            catch (gpstk::EphemerisStore::NoEphemerisFound& e)
            {
               if (verbosity>1)
                  cout << t.printf(timeFormat) << " prn " << prn.prn << " no eph " << endl;
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
   PrnElevationMap& pem,
   bool keepUnhealthy)
{
   cout << endl
        << "ord        elev   stddev    mean      # obs    # bad   # unk  max good  slips" << endl
        << "---------- -----  --------  --------  -------  ------  ------  --------  -----" << endl;

   for (ElevationRangeList::const_iterator i = elr.begin(); i != elr.end(); i++)
   {
      cout << "c1 dd res  " << computeStats(C1, ddem,  *i, pem, keepUnhealthy) << "    " << endl;
      cout << "p1 dd res  " << computeStats(P1, ddem,  *i, pem, keepUnhealthy) << "    " << endl;
      cout << "l1 dd res  " << computeStats(L1, ddem,  *i, pem, keepUnhealthy) << "    " 
           << computeStats(csl, *i, L1) << endl;
      cout << "d1 dd res  " << computeStats(D1, ddem,  *i, pem, keepUnhealthy) << "    " << endl;
      cout << "s1 dd res  " << computeStats(S1, ddem,  *i, pem, keepUnhealthy) << "    " << endl;
      cout << endl;
   }
   cout << "------------------------------------------------------------------------ " << endl;

   for (ElevationRangeList::const_iterator i = elr.begin(); i != elr.end(); i++)
   {
      cout << "p2 dd res  " << computeStats(P2, ddem,  *i, pem, keepUnhealthy) << "    " << endl;
      cout << "l2 dd res  " << computeStats(L2, ddem,  *i, pem, keepUnhealthy) << "    " 
           << computeStats(csl, *i, L2) << endl;
      cout << "d2 dd res  " << computeStats(D2, ddem,  *i, pem, keepUnhealthy) << "    " << endl;
      cout << "s1 dd res  " << computeStats(S2, ddem,  *i, pem, keepUnhealthy) << "    " << endl;
      cout << endl;
   }
   cout << "------------------------------------------------------------------------ " << endl;
}


// ---------------------------------------------------------------------
// Returns a string containing a statistical summary of the double difference
// residuals for the specified obs type within the given elevation range.
// ---------------------------------------------------------------------
string computeStats(
   const RinexObsType rot,
   DDEpochMap& ddem,
   const ElevationRange er,
   PrnElevationMap& pem,
   bool keepUnhealthy)
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
      PrnROTDM::iterator pi;
      for (pi = ei->second.dd.begin(); pi != ei->second.dd.end(); pi++)
      {
         const gpstk::RinexPrn& prn = pi->first;
         ROTDM& ddr = pi->second;

         if (ei->second.health[prn] && !keepUnhealthy)
            continue;
         if (pem[t][prn]<minElevation || pem[t][prn]>maxElevation)
            continue;
         if (ddr.find(rot) == ddr.end())
            zeroCount++;
         else
         {
            double mag=std::abs(ddr[rot]);
            if (mag < strip)
               good.Add(ddr[rot]);
            else
               bad.Add(ddr[rot]);
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
void dump(std::ostream& s, DDEpochMap& ddem, PrnElevationMap& pem)
{
   if (verbosity>1)
      cout << "Writing raw double differences." << endl;

   s.setf(ios::fixed, ios::floatfield);
   s << "# time              PRN type  elev      ddr/clk(m)       health"
     << endl;

   DDEpochMap::iterator ei;
   PrnROTDM::iterator pi;
   for (ei = ddem.begin(); ei != ddem.end(); ei++)
   {
      const gpstk::DayTime& t = ei->first;
      double clk = ei->second.clockOffset;
      string time=t.printf(timeFormat);
      gpstk::RinexPrn& masterPrn = ei->second.masterPrn;
      for (pi = ei->second.dd.begin(); pi != ei->second.dd.end(); pi++)
      {
         const gpstk::RinexPrn& prn = pi->first;
         ROTDM& ddr = pi->second;
         for (ROTDM::const_iterator ti = ddr.begin(); ti != ddr.end(); ti++)
         {
            const RinexObsType& rot = ti->first;
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
              << " " << setw(2) << prn.prn
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

