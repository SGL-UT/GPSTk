#pragma ident "$Id$"

/*
  Intended to look for various things in the nav bit stream. See MDPNavProc for general
  processing of the nav data in engineering units.
*/

#include "SubframeProc.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


//-----------------------------------------------------------------------------
MDPSubframeProcessor::MDPSubframeProcessor(MDPStream& in, std::ofstream& out)
   : MDPProcessor(in, out),
     badNavSubframeCount(0), navSubframeCount(0)
{
   timeFormat = "%4Y %03j %02H:%02M:%02S %6.0g";

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
MDPSubframeProcessor::~MDPSubframeProcessor()
{
   using gpstk::RangeCode;
   using gpstk::CarrierCode;
   using gpstk::StringUtils::asString;
   
   out << "Done processing data." << endl << endl;

   out << endl
       << "Navigation Subframe message summary:" << endl
       << "  navSubframeCount: " << navSubframeCount << endl
       << "  badNavSubframeCount: " << badNavSubframeCount << endl
       << "  percent bad: " << setprecision(3)
       << 100.0 * badNavSubframeCount/navSubframeCount << " %" << endl;

   if (badNavSubframeCount==0)
       return;

   out << "Parity Errors" << endl;
   out << "# elev";
   std::map<RangeCarrierPair, Histogram>::const_iterator peh_itr;
   for (peh_itr = peHist.begin(); peh_itr != peHist.end(); peh_itr++)
   {
      const RangeCarrierPair& rcp=peh_itr->first;
      out << "    " << asString(rcp.second)
           << "-"    << leftJustify(asString(rcp.first), 2);
   }
   out << endl;

   Histogram::BinRangeList::const_iterator brl_itr;
   for (brl_itr = bins.begin(); brl_itr != bins.end(); brl_itr++)
   {
      const Histogram::BinRange& br = *brl_itr ;
      out << setprecision(0)
          << right << setw(2) << br.first << "-"
          << left  << setw(2) << br.second << ":";

      for (peh_itr = peHist.begin(); peh_itr != peHist.end(); peh_itr++)
      {
         const RangeCarrierPair& rcp=peh_itr->first;
         Histogram h=peh_itr->second;
         out << right << setw(9) << h.bins[br];
      }

      out << endl;
   }

   // Whoever would write a reference like this should be shot...
   out << right << setw(2) << peHist.begin()->second.bins.begin()->first.first
        << "-" << left  << setw(2) << peHist.begin()->second.bins.rbegin()->first.second
        << ":";

   for (peh_itr = peHist.begin(); peh_itr != peHist.end(); peh_itr++)
      out << right <<  setw(9) << peh_itr->second.total;
      
   out << endl;
}


//-----------------------------------------------------------------------------
void MDPSubframeProcessor::process(const MDPNavSubframe& msg)
{
   navSubframeCount++;

   RangeCarrierPair rcp(msg.range, msg.carrier);
   NavIndex ni(rcp, msg.prn);
   unsigned long sow = msg.time.GPSsow();
   int sfIndex = sow % 750;
   int sfNumber = 1 + (sow%30)/6;

   ostringstream oss;
   oss << msg.time.printf(timeFormat)
       << "  PRN:" << setw(2) << msg.prn
       << " " << asString(msg.carrier)
       << ":" << setw(2) << left << asString(msg.range)
       << " SFID:" << setw(2) << sfNumber;
   string msgPrefix = oss.str();

   // Look up the previous subframe that is comparable to the current one
   // For pages 1-3 this means 30 seconds ago. For pages 4&5 this means
   // 750 seconds ago.
   MDPNavSubframe& prev = sfHistory[sfIndex][ni];
   int dt = msg.time - prev.time;
   bool prevGood = abs(dt) <= 750 && prev.parityGood;
   if (sfNumber<4)
   {
      int i = (sow-30) % 750;
      if (i<0)
         i+=750;
      prev = sfHistory[i][ni];
      dt = msg.time - prev.time;
      prevGood = abs(dt) <= 30 && prev.parityGood;
   }
   if (verboseLevel>2)
      out << msgPrefix << " sfIndex:" << sfIndex << " dt:" << dt << " prg:" << prevGood << endl;

   MDPNavSubframe cmsg = msg;
   cmsg.cookSubframe();
   if (verboseLevel>3 && cmsg.neededCooking)
      out << msgPrefix << "Subframe required cooking" << endl;

   if (!cmsg.parityGood)
   {
      badNavSubframeCount++;
      if (verboseLevel)
         out << msgPrefix << "Parity error"
             << " SNR:" << fixed << setprecision(1) << snr[ni]
             << " EL:" << el[ni]
             << endl;
      if (!prevGood)
         out << "No previous subframe to compare to" << endl;
   }

   if (prevGood)
   {
      vector<uint32_t> diff(11),expected(prev.subframe);
      uint32_t tow = (sow+6)/6;
      uint32_t how = (tow << 13) | (cmsg.subframe[2] & 0x000001FFF);
      expected[2] = how;
      uint32_t diff_sum=0;
      for (int i=1; i < cmsg.subframe.size(); i++)
      {
         diff[i] = cmsg.subframe[i] xor expected[i];
         diff_sum |= diff[i];
      }

      if (diff_sum)
      {
         out << hex << setfill('0') << uppercase;
         out << msgPrefix << "Bit difference detected:" << endl;
         for (int i=1; i < diff.size(); i++)
            out << setw(8) << diff[i] << "  ";
         out << endl << dec;
         if (debugLevel)
         {
            out << "prev:" << endl;
            prev.dump(out);
            out << "cmsg:" << endl;
            cmsg.dump(out);
            out << "msg:" << endl;
            msg.dump(out);
         }
      }
   }
   
   int i = sfIndex == 0 ? 749 : sfIndex-1;
   prev = sfHistory[i][ni];
   if (prev.parityGood && 
       prev.inverted != cmsg.inverted && 
       cmsg.time - prev.time <= 12)
   {
      if (verboseLevel)
         out << msgPrefix << "Polarity inversion"
             << " SNR:" << fixed << setprecision(1) << snr[ni]
             << " EL:" << el[ni]
             << endl;
   }

   // Save off this message
   sfHistory[sfIndex][ni] = cmsg;
}  // end of process()


void  MDPSubframeProcessor::process(const MDPObsEpoch& msg)
{
   if (!msg)
      return;

   for (MDPObsEpoch::ObsMap::const_iterator i = msg.obs.begin();
        i != msg.obs.end(); i++)
   {
      const MDPObsEpoch::Observation& obs=i->second;      
      NavIndex ni(RangeCarrierPair(obs.range, obs.carrier), msg.prn);
      snr[ni] = obs.snr;
      el[ni] = msg.elevation;
   }
}
