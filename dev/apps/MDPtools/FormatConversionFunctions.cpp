#pragma ident "$Id$"


/** @file Translates between various similiar objects */

//lgpl-license START
//lgpl-license END

#include "StringUtils.hpp"

#include "FormatConversionFunctions.hpp"

using namespace std;

namespace gpstk
{
   short snr2ssi(float x)
   {
      // These values were obtained from the comments in a RINEX obs file that was
      // generated from a TurboBinary file recorded on an AOA Benchmark  receiver
      if (x>316) return 9;
      if (x>100) return 8;
      if (x>31.6) return 7;
      if (x>10) return 6;
      if (x>3.2) return 5;
      if (x>0) return 4;
      return 0;
   }

   gpstk::RinexObsData::RinexObsTypeMap makeRinexObsTypeMap(
      const MDPObsEpoch& moe)
   {
      gpstk::RinexObsData::RinexObsTypeMap rotm;
      MDPObsEpoch::ObsMap ol=moe.obs;
      MDPObsEpoch::ObsMap::const_iterator j;

      // The C1 Rinex obs is easy
      j = ol.find(MDPObsEpoch::ObsKey(ccL1,rcCA));
      if (j!=ol.end())
      {
         rotm[gpstk::RinexObsHeader::C1].data = j->second.pseudorange;
         rotm[gpstk::RinexObsHeader::C1].lli = j->second.lockCount ? 1 : 0;
         rotm[gpstk::RinexObsHeader::C1].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::L1].data = j->second.phase;
         rotm[gpstk::RinexObsHeader::L1].lli = j->second.lockCount ? 1 : 0;
         rotm[gpstk::RinexObsHeader::L1].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::D1].data = j->second.doppler;
         rotm[gpstk::RinexObsHeader::D1].lli = j->second.lockCount ? 1 : 0;
         rotm[gpstk::RinexObsHeader::D1].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::S1].data = j->second.snr;
      }

      // Now get the P1, L1, D1, S1 obs
      j = ol.find(MDPObsEpoch::ObsKey(ccL1, rcYcode));
      if (j == ol.end())
         j = ol.find(MDPObsEpoch::ObsKey(ccL1, rcPcode));
      if (j == ol.end())
         j = ol.find(MDPObsEpoch::ObsKey(ccL1, rcCodeless));
      if (j != ol.end())
      {
         rotm[gpstk::RinexObsHeader::P1].data = j->second.pseudorange;
         rotm[gpstk::RinexObsHeader::P1].lli = j->second.lockCount ? 1 : 0;
         rotm[gpstk::RinexObsHeader::P1].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::L1].data = j->second.phase;
         rotm[gpstk::RinexObsHeader::L1].lli = j->second.lockCount ? 1 : 0;
         rotm[gpstk::RinexObsHeader::L1].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::D1].data = j->second.doppler;
         rotm[gpstk::RinexObsHeader::D1].lli = j->second.lockCount ? 1 : 0;
         rotm[gpstk::RinexObsHeader::D1].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::S1].data = j->second.snr;
      }
      
      // Now get the P2, L2, D2, S2 obs
      j = ol.find(MDPObsEpoch::ObsKey(ccL2, rcYcode));
      if (j == ol.end())
         j = ol.find(MDPObsEpoch::ObsKey(ccL2, rcPcode));
      if (j == ol.end())
         j = ol.find(MDPObsEpoch::ObsKey(ccL2, rcCodeless));
      if (j != ol.end())
      {
         rotm[gpstk::RinexObsHeader::P2].data = j->second.pseudorange;
         rotm[gpstk::RinexObsHeader::P2].lli = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::P2].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::L2].data = j->second.phase;
         rotm[gpstk::RinexObsHeader::L2].lli = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::L2].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::D2].data = j->second.doppler;
         rotm[gpstk::RinexObsHeader::D2].lli = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::D2].ssi = snr2ssi(j->second.snr);

         rotm[gpstk::RinexObsHeader::S2].data = j->second.snr;
      }

      // Now get the C2
      j = ol.find(MDPObsEpoch::ObsKey(ccL2, rcCM));
      if (j == ol.end())
         j = ol.find(MDPObsEpoch::ObsKey(ccL2, rcCL));
      if (j == ol.end())
         j = ol.find(MDPObsEpoch::ObsKey(ccL2, rcCMCL));
      if (j != ol.end())
      {
         rotm[gpstk::RinexObsHeader::C2].data = j->second.pseudorange;
         rotm[gpstk::RinexObsHeader::C2].lli = j->second.lockCount ? 0 : 1;
         rotm[gpstk::RinexObsHeader::C2].ssi = snr2ssi(j->second.snr);
      }
      return rotm;
   }

   void makeRinexObsData(
      gpstk::RinexObsHeader& roh,
      gpstk::RinexObsData& rod,
      const gpstk::MDPEpoch& mdp)
   {
      rod.obs.clear();
      rod.clockOffset=0;
      rod.numSvs = mdp.size();
      rod.epochFlag = 0;
      rod.time = mdp.begin()->second.time;

      for (MDPEpoch::const_iterator i=mdp.begin(); i!=mdp.end(); i++)
      {
         const MDPObsEpoch& moe = i->second;
         gpstk::RinexPrn prn(moe.prn, gpstk::systemGPS);
         rod.obs[prn] = makeRinexObsTypeMap(moe);
      }
   }

   // Try to convert the given pages into an EngAlmanc object. Returns true
   // upon success. This algorithm will only work for a receiver that outputs
   // all 4/5 subframes from a code/carrier. Basically it looks for a 12.5 minute
   // cycle that starts with page 1 from subframe 4.
   // It makes sure that there hasn't been a cutover during it by checking that
   // all sv pages (i.e. svid 1-32) have the same toa as the last page 25
   // (svid 51).
   // In the IS-GPS-200D, see pages 72-79, 82, 105
   bool makeEngAlmanac(EngAlmanac& alm, const AlmanacPages& pages)
   {
      if (pages.size()!=50)
         return false;

      AlmanacPages::const_iterator i, firstPage, lastPage;
      firstPage = pages.find(SubframePage(4, 1));
      lastPage = pages.find(SubframePage(5,25));
      long dt = lastPage->second.getHOWTime() - firstPage->second.getHOWTime();

      // 25 pairs of pages every 30 seconds = 750 seconds minus the 24 seconds
      // that the three ephemeris pages take gives us 726 seconds
      if (dt != 726)
         return false;

      int week = firstPage->second.time.GPSfullweek();
      long sfa[10];
      long long_sfa[10];
      for (i = pages.begin(); i != pages.end(); i++)
      {
         i->second.fillArray(sfa);
         copy( &sfa[0], &sfa[10], long_sfa);
         if (!alm.addSubframe(long_sfa, week))
            return false;
      }
      cout << endl;
      
      double p51Toa=alm.getToa();
      double svToa;
      for (int prn=1; prn<=32; prn++)
      {
         try {
            svToa = alm.getToa(gpstk::SatID(prn, SatID::systemGPS));
            if (svToa != p51Toa)
               return false;
         }
         catch (EngAlmanac::SVNotPresentException& e)
         {}
      }

      return true;
   }

   // Try to convert the given pages into an EngEphemeris object. Returns true
   // upon success.
   bool makeEngEphemeris(EngEphemeris& eph, const EphemerisPages& pages)
   {
      EphemerisPages::const_iterator sf[4];

      sf[1] = pages.find(1);
      if (sf[1] == pages.end())
         return false;
      
      sf[2] = pages.find(2);
      if (sf[2] == pages.end())
         return false;

      sf[3] = pages.find(3);
      if (sf[3] == pages.end())
         return false;

      long t1 = sf[1]->second.getHOWTime();
      long t2 = sf[2]->second.getHOWTime();
      long t3 = sf[3]->second.getHOWTime();
      if (t2 != t1+6 || t3 != t1+12)
         return false;

      int prn = sf[1]->second.prn;
      int week = sf[1]->second.time.GPSfullweek();
      long sfa[10];
      long long_sfa[10];

      for (int i=1; i<=3; i++)
      {
         sf[i]->second.fillArray(sfa);
         for( int j = 0; j < 10; j++ )
            long_sfa[j] = static_cast<long>( sfa[j] );
         if (!eph.addSubframe(long_sfa, week, prn, 0))
            return false;
      }

      if (eph.isData(1) && eph.isData(2) && eph.isData(3))
         return true;

      return false;
   }

} // end of namespace gpstk
