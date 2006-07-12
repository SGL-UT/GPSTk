#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/DDEpoch.cpp#1 $"

#include "DDEpoch.hpp"

using namespace std;


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
std::map<RinexObsType, double> DDEpoch::singleDifference(
   const gpstk::RinexObsData::RinexObsTypeMap& rx1obs,
   const gpstk::RinexObsData::RinexObsTypeMap& rx2obs)
{
   map<RinexObsType, double> diff;

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
         cout << rx1.time.printf(timeFormat) << " No master SV selected.  Skipping epoch." << endl;
      return;
   }

   clockOffset = rx1.clockOffset - rx2.clockOffset;
   if (std::abs(clockOffset)>2.1e-3 || std::abs(clockOffset)<1e-10)
   {
      if (verbosity>2)
         cout << rx1.time.printf(timeFormat)
              << " Rx1-Rx2 clock offset is " << 1e3*clockOffset << " ms. Skipping epoch."
              << endl;
      return;
   }

   gpstk::RinexObsData::RinexPrnMap::const_iterator oi1, oi2;
   oi1 = rx1.obs.find(masterPrn);
   oi2 = rx2.obs.find(masterPrn);
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
   double minElevation = 15.0;

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


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
bool phaseDisc(const ROTDM& curr, const ROTDM& prev)
{
   typedef std::set<RinexObsType> RinexObsTypeSet;
   RinexObsTypeSet phaseObsTypes;
   phaseObsTypes.insert(L1);
   phaseObsTypes.insert(L2);

   for (ROTDM::const_iterator i=curr.begin(); i != curr.end(); i++)
   {
      const RinexObsType& rot = i->first;
      if (phaseObsTypes.find(rot) == phaseObsTypes.end())
         continue;
      
      ROTDM::const_iterator j = prev.find(rot);
      if (j == prev.end())
         continue;

      // Replace with double lamda = rot.lamda();
      double lamda = ((rot==L1) ? gpstk::C_GPS_M/gpstk::L1_FREQ : gpstk::C_GPS_M/gpstk::L2_FREQ);
      double td = (i->second - j->second) / lamda;
      if (std::abs(td) > 0.9)
         return true;
   }

   // No problems were found.
   return false;
}

//-----------------------------------------------------------------------------
// Similiar to computeDD but does a triple difference to look for cycle slips
//-----------------------------------------------------------------------------
void computeDDEpochMap(
   RODEpochMap& rx1,
   RODEpochMap& rx2,
   PrnElevationMap& pem,
   DDEpochMap& ddem)
{
   if (verbosity)
      cout << "Computing 2nd differences residuals with new SV master selection using a" << endl
           << "3rd difference cycle slip detection." << endl;

   float minMasterElevation = 15;
   if (verbosity>1)
      cout << "Using a minimum master SV elevation of "
           << minMasterElevation << " degrees." << std::endl;
 
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

      if (!curr.valid)
         continue;

      if (prev.valid && 
          phaseDisc(curr.dd[curr.masterPrn], prev.dd[curr.masterPrn]))
      {
         // since it appears that there is a cycle slip on the current master,
         // we want to back up 7 epochs, select a new master, and reprocess
         // the those epochs. if we can't back up, then skip this epoch
         if (verbosity)
            cout << t.printf(timeFormat)
                 << " Possible cycle slip on the master ("
                 << curr.masterPrn.prn << "). Backing up 7 epochs." << endl;

         int n;
         for (n=0; n<7 && ei1 != rx1.begin(); n++)
            ei1--;

         if (n!=7 && verbosity)
            cout << t.printf(timeFormat) 
                 << " Failed to back up 7 epochs." << endl;
            
         gpstk::DayTime prevTime = ei1->first;
         ei1++;
         t = ei1->first;
         ei2 = rx2.find(t);
            
         gpstk::RinexPrn prn, badPrn = curr.masterPrn;
         for (RinexPrnMap::const_iterator i=ei1->second.obs.begin(); i != ei1->second.obs.end(); i++)
         {
            prn = i->first;
            RinexPrnMap::const_iterator j = ei2->second.obs.find(prn);
            RinexObsTypeMap obs = i->second;
            if (j != ei2->second.obs.end() && obs[D1].data >= 0 &&
                pem[t][prn] > minMasterElevation &&
                prn != badPrn)
               break;
         }
         // Now recompute the double differences
         curr = DDEpoch(); // cheap way to clear this out...
         curr.masterPrn = prn;
         curr.doubleDifference(ei1->second, ei2->second);
         if (!curr.valid)
            continue;
      } // end dealing with a slip on the master SV

      ddem[t] = curr;
      prev = curr;
   } // end of looping over all epochs in the first set.
}  // end of computeDDEpochMap()


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void dumpStats(DDEpochMap& ddem,
               const CycleSlipList& csl,
               PrnElevationMap& pem)
{
   cout << endl
        << "ord        elev   stddev    mean    z   #obs  #del   max   strip   slips" << endl
        << "---------- -----  -------  ----------  ------ ----  ------ ------  -----" << endl;

   for (ElevationRangeList::const_iterator i = elr.begin(); i != elr.end(); i++)
   {
      cout << "c1 dd res " << computeStats(C1, ddem,  *i, pem) << "   " << endl;
      cout << "p1 dd res " << computeStats(P1, ddem,  *i, pem) << "   " << endl;
      cout << "l1 dd res " << computeStats(L1, ddem,  *i, pem) << "  " 
           << computeStats(csl, *i, L1) << endl;
      cout << "d1 dd res " << computeStats(D1, ddem,  *i, pem) << "   " << endl;
      cout << endl;
   }
   cout << "------------------------------------------------------------------------ " << endl;

   for (ElevationRangeList::const_iterator i = elr.begin(); i != elr.end(); i++)
   {
      cout << "p2 dd res " << computeStats(P2, ddem,  *i, pem) << "   " << endl;
      cout << "l2 dd res " << computeStats(L2, ddem,  *i, pem) << "  " 
           << computeStats(csl, *i, L2) << endl;
      cout << "d2 dd res " << computeStats(D2, ddem,  *i, pem) << "   " << endl;
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
   PrnElevationMap& pem)
{
   ostringstream oss;
   float minElevation = er.first;
   float maxElevation = er.second;

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
         if (pem[t][prn]>minElevation && pem[t][prn]<maxElevation)
         {
            if (abs(ddr[rot]) < 1000 )
               good.Add(ddr[rot]);
            else
               bad.Add(ddr[rot]);
         }
      }
   }
   
   oss << right << " " << setprecision(2) << setw(2) <<  minElevation
        << "-" << setw(2) << maxElevation << " ";

   char b1[200];
   char zero = good.Average() < good.StdDev()/sqrt((float)good.N())?'0':' ';
   double maxDD = max(abs(good.Minimum()), abs(good.Maximum()));
   sprintf(b1, "%8.5f  %8.4f %c %7d %4d  %6.2f %6.2f",
           good.StdDev()/sqrt(2), good.Average(), zero,
           good.N(), bad.N(), maxDD, 1000.0);

   oss << b1;
   return oss.str();
}

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void dump(std::ostream& s,
          DDEpochMap& ddem,
          PrnElevationMap& pem)
{
   s.setf(ios::fixed, ios::floatfield);
   s << "# time              PRN type  elev      clk(m)"
     << "    2nd diff(m)"
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

         s << left << setw(20) << time << right
           << setfill(' ')
           << " " << setw(2) << prn.prn
           << " " << setw(4) << 1
           << " " << setprecision(1) << setw(5)  << pem[t][prn]
           << " " << setprecision(3) << setw(12)  << clk

           << " " << setprecision(6) << setw(14) << ddr[L1]
           << endl;

         s << left << setw(20) << time << right
           << setfill(' ')
           << " " << setw(2) << prn.prn
           << " " << setw(4) << 2
           << " " << setprecision(1) << setw(5)  << pem[t][prn]
           << " " << setprecision(3) << setw(12)  << clk

           << " " << setprecision(6) << setw(14) << ddr[L2]
           << endl;
      }
   }
   s << endl;
}  // end dump()

