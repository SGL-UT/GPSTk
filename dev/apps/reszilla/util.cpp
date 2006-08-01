#pragma ident "$Id$"


#include "util.hpp"

using namespace std;

const RinexObsType& L1=gpstk::RinexObsHeader::L1;
const RinexObsType& L2=gpstk::RinexObsHeader::L2;
const RinexObsType& P1=gpstk::RinexObsHeader::P1;
const RinexObsType& P2=gpstk::RinexObsHeader::P2;
const RinexObsType& C1=gpstk::RinexObsHeader::C1;
const RinexObsType& C2=gpstk::RinexObsHeader::C2;
const RinexObsType& D1=gpstk::RinexObsHeader::D1;
const RinexObsType& D2=gpstk::RinexObsHeader::D2;
const RinexObsType& S1=gpstk::RinexObsHeader::S1;
const RinexObsType& S2=gpstk::RinexObsHeader::S2;


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void computeDD2(const RODEpochMap& rx1,
               const RODEpochMap& rx2,
               DD2EpochMap& ddem)
{
   if (verbosity)
      cout << "Computing 2nd differences residuals across codes." << endl;

   // We use the data from rx1 walk us through the data
   // loop over all epochs for this station
   RODEpochMap::const_iterator ei1, ei2;
   for (ei1=rx1.begin(); ei1!=rx1.end(); ei1++)
   {
      // first make sure we have data from the other receiver for this
      // epoch...
      const gpstk::DayTime& t = ei1->first;
      ei2 = rx2.find(t);
      if (ei2 == rx2.end())
      {
         if (verbosity>2)
            cout << "Epoch with no data in second file at " << t << endl;
         continue;
      }

      gpstk::RinexObsData e1 = ei1->second;
      gpstk::RinexObsData e2 = ei2->second;

      DD2Epoch dde;
      if (dde.compute(e1, e2))
         ddem[t] = dde;
   }
} // end of computeDD2()


// ---------------------------------------------------------------------
// Compute a double difference for each prn in track on both receivers
// (C1(prn1)-P2(prn1)) - (C1(prn2)-P2(prn2))
// ---------------------------------------------------------------------
bool DD2Epoch::compute(gpstk::RinexObsData rx1,
                       gpstk::RinexObsData rx2)
{
   gpstk::RinexObsData::RinexPrnMap::iterator oi1, oi2;
   for (oi1=rx1.obs.begin(); oi1!=rx1.obs.end(); oi1++)
   {
      gpstk::RinexPrn prn = oi1->first;
      oi2 = rx2.obs.find(prn);
      if (oi2 == rx2.obs.end())
         continue;

      gpstk::RinexObsData::RinexObsTypeMap& rx1obs = oi1->second;
      gpstk::RinexObsData::RinexObsTypeMap& rx2obs = oi2->second;

      // Now compute the C1-P2 for each receiver
      double d1 = rx1obs[C1].data - rx1obs[P2].data;
      double d2 = rx2obs[C1].data - rx2obs[P2].data;

      // Now compute the double differences
      res[prn] = d1 - d2;
   }

   return true;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
PrnElevationMap elevation_map(const RODEpochMap& rem,
                              const gpstk::RinexObsHeader& roh,
                              const gpstk::EphemerisStore& eph)
{
   PrnElevationMap pem;
   if (verbosity>1)
      cout << "Computing elevation map." << endl;

   gpstk::ECEF rxpos(roh.antennaPosition);

   RODEpochMap::const_iterator i;
   for (i=rem.begin(); i!=rem.end(); i++)
   {
      const gpstk::DayTime& t = i->first;
      const gpstk::RinexObsData& rod = i->second;
      gpstk::RinexObsData::RinexPrnMap::const_iterator rpi;
      for (rpi=rod.obs.begin(); rpi!=rod.obs.end(); rpi++)
         try
         {
            short prn = rpi->first.prn;
            gpstk::Xvt svpos = eph.getPrnXvt(prn, t);
            pem[t][rpi->first] = rxpos.elvAngle(svpos.x);
         }
         catch (gpstk::EphemerisStore::NoEphemerisFound& e)
         {
            if (verbosity>2)
               cout << e.getText() << endl;
         }
   }
   return pem;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
PrnElevationMap elevation_map(const ORDEpochMap& oem)
{
   PrnElevationMap pem;
   if (verbosity>1)
      cout << "Computing elevation map." << endl;
   ORDEpochMap::const_iterator i;
   for (i=oem.begin(); i!=oem.end(); i++)
   {
      const gpstk::DayTime& t=i->first;
      const gpstk::ORDEpoch& epoch=i->second;
      gpstk::ORDEpoch::ORDMap::const_iterator j;
      for (j=epoch.ords.begin(); j!=epoch.ords.end(); j++)
      {
         gpstk::ObsRngDev ord = j->second;
         gpstk::RinexPrn prn(j->first, gpstk::systemGPS);
         pem[t][prn] = ord.getElevation();
      }
   }
   return pem;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void add_clock_to_rinex(RODEpochMap& rem, const ORDEpochMap& oem)
{
   const gpstk::GPSGeoid gm;
   RODEpochMap::iterator i;
   for (i=rem.begin(); i!=rem.end(); i++)
   {
      gpstk::RinexObsData& rod = i->second;
      ORDEpochMap::const_iterator j = oem.find(rod.time);
      if (j==oem.end() || !j->second.validClock)
      {
         if (verbosity>2)
            cout << "Epoch has no clock " << rod.time << endl;
         continue;
      }
      rod.clockOffset = j->second.clockOffset/gm.c();
   }
} // end of add_clock_to_rinex()


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void check_data(const gpstk::RinexObsHeader& roh, const RODEpochMap& rem)
{
   double rate=-1;
   int gapCount(0), rateCount(0);

   RODEpochMap::const_iterator i = rem.begin();
   while (i != rem.end())
   {
      const gpstk::RinexObsData& prev = i->second;
      i++;
      if (i == rem.end())
         break;
      const gpstk::RinexObsData& curr = i->second;
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
      else if (dt < 0)
      {
         gapCount++;
         cout << "Data time backed up from " 
              << prev.time <<" to " << curr.time << endl;
      }
   }
   
   cout << "Data had " << gapCount << " gaps." << endl;
   cout << "Data had " << rateCount << " rate changes." << endl;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void dumpStats(DD2EpochMap& ddem, PrnElevationMap& pem)
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
   PrnElevationMap& pem)
{
   float minElevation = er.first;
   float maxElevation = er.second;

   gpstk::Stats<double> good, bad;
   DD2EpochMap::iterator ei;
   for (ei = ddem.begin(); ei != ddem.end(); ei++)
   {
      const gpstk::DayTime& t = ei->first;
      DD2Epoch::DD2ResidualMap::iterator pi;
      for (pi = ei->second.res.begin(); pi != ei->second.res.end(); pi++)
      {
         const gpstk::RinexPrn& prn = pi->first;
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
   const RinexObsType& rot)
{
   ostringstream oss;
   int slipCount=0;

   float minElev = er.first;
   float maxElev = er.second;
   CycleSlipList::const_iterator i;
   for (i=csl.begin(); i!=csl.end(); i++)
   {
      const CycleSlipRecord& cs = *i;
      if (cs.rot == rot && minElev < cs.elevation && cs.elevation <= maxElev)
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
      if (i->rot == L1)
         l1++;
      else if (i->rot == L2)
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
        << "  " << right << setw(2) << cs.prn.prn
        << " " << cs.rot.type
        << " " << setprecision(3) << setw(14) << cs.cycles
        << "  " << std::setprecision(2) << setw(5) << cs.elevation
        << "  " << setw(5) << cs.preCount
        << "  " << setw(5) << cs.postCount
        << "  " << setw(5) << setprecision(1) << cs.preGap
        << "  " << setw(2) << cs.masterPrn.prn
        << endl;
   }
   s << endl;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void dump(std::ostream& s,
          DD2EpochMap& ddem,
          PrnElevationMap& pem)
{
   s.setf(ios::fixed, ios::floatfield);
   s << "# time, PRN, RES(m), SV_EL" << endl;

   DD2EpochMap::iterator ei;
   DD2Epoch::DD2ResidualMap::iterator pi;
   for (ei = ddem.begin(); ei != ddem.end(); ei++)
   {
      const gpstk::DayTime& t = ei->first;
      string time=t.printf(timeFormat);
      for (pi = ei->second.res.begin(); pi != ei->second.res.end(); pi++)
      {
         const gpstk::RinexPrn& prn = pi->first;
         double ddr = pi->second;
         s << time
           << setfill(' ')
           << " " << setw(2)  << prn.prn
           << setprecision(6)
           << " " << setw(11) << ddr
           << setprecision(1)
           << " " << setw(4) << pem[t][prn]
           << endl;
      }
   }
}  // end dump()
