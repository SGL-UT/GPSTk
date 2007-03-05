#pragma ident "$Id$"


/*
  Think, navdmp for mdp, with bonus output that you get data from all code/carrier
  combos.
*/

#include "Geodetic.hpp"
#include "NavProc.hpp"

#include "RinexConverters.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


//-----------------------------------------------------------------------------
MDPNavProcessor::MDPNavProcessor(gpstk::MDPStream& in, std::ofstream& out)
   : MDPProcessor(in, out),
     firstNav(true), almOut(false), ephOut(false),
     badNavSubframeCount(0), navSubframeCount(0)
{
   timeFormat = "%4Y/%03j/%02H:%02M:%02S";

   binByElevation = true;
   if (binByElevation)
   {
      double binSize=5;
      for (double x=0; x<90; x+=binSize)
         bins.push_back(Histogram::BinRange(x, x+binSize));
   }
   else
   {
      bins.push_back(Histogram::BinRange(0, 30));
      double binSize=3;
      for (double x=30; x<60; x+=binSize)
         bins.push_back(Histogram::BinRange(x, x+binSize));
      bins.push_back(Histogram::BinRange(60, 99));
   }
}


//-----------------------------------------------------------------------------
MDPNavProcessor::~MDPNavProcessor()
{
   using gpstk::RangeCode;
   using gpstk::CarrierCode;
   using gpstk::StringUtils::asString;
   
   out << "Done processing data." << endl << endl;

   out << endl << "Navigation Subframe message summary:" << endl;
   if (firstNav)
      out << "  No Navigation Subframe messages processed." << endl;
   else
   {
      out << "  navSubframeCount: " << navSubframeCount << endl;
      out << "  badNavSubframeCount: " << badNavSubframeCount << endl;
   }

   cout << "Parity Errors" << endl;
   cout << "# snr ";
   std::map<RangeCarrierPair, Histogram>::const_iterator peh_itr;
   for (peh_itr = peHist.begin(); peh_itr != peHist.end(); peh_itr++)
   {
      const RangeCarrierPair& rcp=peh_itr->first;
      cout << "    " << asString(rcp.second)
           << "-"    << leftJustify(asString(rcp.first), 2);
   }
   cout << endl;

   Histogram::BinRangeList::const_iterator brl_itr;
   for (brl_itr = bins.begin(); brl_itr != bins.end(); brl_itr++)
   {
      const Histogram::BinRange& br = *brl_itr ;
      std::cout << right << setw(2) << br.first << "-"
                << left  << setw(2) << br.second << ":";

      for (peh_itr = peHist.begin(); peh_itr != peHist.end(); peh_itr++)
      {
         const RangeCarrierPair& rcp=peh_itr->first;
         Histogram h=peh_itr->second;
         cout << right << setw(9) << h.bins[br];
      }

      cout << endl;
   }

   // Whoever would write a reference like this should be shot...
   cout << right << setw(2) << peHist.begin()->second.bins.begin()->first.first
        << "-" << left  << setw(2) << peHist.begin()->second.bins.rbegin()->first.second
        << ":";

   for (peh_itr = peHist.begin(); peh_itr != peHist.end(); peh_itr++)
      cout << right <<  setw(9) << peh_itr->second.total;
      
   out << endl;
}


//-----------------------------------------------------------------------------
void MDPNavProcessor::process(const gpstk::MDPNavSubframe& msg)
{
   if (firstNav)
   {
      firstNav = false;
      if (verboseLevel)
         out << msg.time.printf(timeFormat)
             << "  Received first Navigation Subframe message"
             << endl;
   }

   navSubframeCount++;
   RangeCarrierPair rcp(msg.range, msg.carrier);
   NavIndex ni(rcp, msg.prn);

   gpstk::MDPNavSubframe umsg = msg;

   ostringstream oss;
   oss << umsg.time.printf(timeFormat)
      << "  PRN:" << setw(2) << umsg.prn
      << " " << asString(umsg.carrier)
      << ":" << setw(4) << left << asString(umsg.range);
   string msgPrefix = oss.str();
   if (verboseLevel>2 || debugLevel)
      out << endl << msgPrefix << " Processing" << endl;
   
   // First try the data assuming it is already upright
   umsg.cooked = true;
   bool parityGood = umsg.checkParity();
   if (!parityGood)
   {
      if (verboseLevel>2)
         out << msgPrefix << " Subframe appears raw" << endl;
      umsg.cooked = false;
      umsg.cookSubframe();
      parityGood = umsg.checkParity();
   }
   else
   {
      if (verboseLevel>2)
         out << msgPrefix << " Subframe appears cooked" << endl;
   }

   if (!parityGood)
   {
      badNavSubframeCount++;
      if (verboseLevel)
         out << msgPrefix << " Parity error"
             << " SNR:" << fixed << setprecision(1) << snr[ni]
             << " EL:" << el[ni]
             << endl;

      if (peHist.find(rcp) == peHist.end())
         peHist[rcp].resetBins(bins);

      if (binByElevation)
         peHist[rcp].addValue(el[ni]);
      else
         peHist[rcp].addValue(snr[ni]);

      return;
   }

   short sfid = umsg.getSFID();
   short svid = umsg.getSVID();
   bool isAlm = sfid > 3;
   long sow = umsg.getHOWTime();
   short page = ((sow-6) / 30) % 25 + 1;

   if (verboseLevel>2)
   {
      out << msgPrefix
          << " SOW:" << setw(6) << sow
          << " NC:" << static_cast<int>(umsg.nav)
          << " I:" << umsg.inverted
          << " SFID:" << sfid;
      if (isAlm)
         out << " SVID:" << svid
             << " Page:" << page;
      out << endl;
   }

   // Sanity check on the header time versus the HOW time
   short week = umsg.time.GPSfullweek();
   if (sow <0 || sow>=604800)
   {
      badNavSubframeCount++;
      if (verboseLevel>1)
         out << msgPrefix << "  Bad SOW: " << sow << endl;
      return;
   }
      
   DayTime howTime(week, umsg.getHOWTime());
   if (howTime == umsg.time)
   {
      if (verboseLevel && ! (bugMask & 0x4))
         out << msgPrefix << " Header time==HOW time" << endl;
   }
   else if (howTime != umsg.time+6)
   {
      badNavSubframeCount++;
      if (verboseLevel>1)
         out << msgPrefix << " HOW time != hdr time+6, HOW:"
             << howTime.printf(timeFormat)
             << endl;
      return;
   }

   prev[ni] = curr[ni];
   curr[ni] = umsg;

   if (isAlm && almOut)
   {
      AlmanacPages& almPages = almPageStore[ni];
      EngAlmanac& engAlm = almStore[ni];
      SubframePage sp(sfid, page);
      almPages[sp] = umsg;
      almPages.insert(make_pair(sp, umsg));
         
      if (makeEngAlmanac(engAlm, almPages))
      {
         out << msgPrefix << " Built complete alm" << endl;
         if (verboseLevel>1)
            engAlm.dump(out);
         almPages.clear();
         engAlm = EngAlmanac();
      }            
   }
   if (!isAlm && ephOut)
   {
      EphemerisPages& ephPages = ephPageStore[ni];
      ephPages[sfid] = umsg;
      EngEphemeris engEph;
      try
      {
         if (makeEngEphemeris(engEph, ephPages))
         {
            out << msgPrefix << " Built complete eph, iocd:0x"
                << hex << setw(3) << engEph.getIODC() << dec
                << endl;
            if (verboseLevel>1)
               out << engEph;
            ephStore[ni] = engEph;
         }
      }
      catch (gpstk::Exception& e)
      {
         out << e << endl;
      }
   }

}  // end of process()


void  MDPNavProcessor::process(const gpstk::MDPObsEpoch& msg)
{
   if (!msg)
      return;

   for (gpstk::MDPObsEpoch::ObsMap::const_iterator i = msg.obs.begin();
        i != msg.obs.end(); i++)
   {
      const gpstk::MDPObsEpoch::Observation& obs=i->second;      
      NavIndex ni(RangeCarrierPair(obs.range, obs.carrier), msg.prn);
      snr[ni] = obs.snr;
      el[ni] = msg.elevation;
   }
}
