#pragma ident "$Id: NavProc.cpp,v 1.1 2006/07/28 14:31:42 littlej Exp $"

/*
  Think, navdmp for mdp, with bonus output that you get data from all code/carrier
  combos.
*/

#include "Geodetic.hpp"
#include "NavProc.hpp"

#include "FormatConversionFunctions.hpp"

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
   short sfid = msg.getSFID();
   short svid = msg.getSVID();
   bool isAlm = sfid > 3;
   long sow = msg.getHOWTime();
   short page = ((sow-6) / 30) % 25 + 1;

   if (verboseLevel>2)
   {
      out << msg.time.printf(timeFormat)
          << "  prn:" << setw(2) << msg.prn
          << " " << asString(msg.carrier)
          << ":" << setw(6) << left << asString(msg.range)
          << " nav:" << static_cast<int>(msg.nav)
          << " SOW:" << setw(6) << sow
          << " SFID:" << sfid;
      if (isAlm)
         out << " SVID:" << svid
             << " Page:" << page;
      out << endl;
   }

   // Sanity check on the header time versus the HOW time
   short week = msg.time.GPSfullweek();
   if (sow <0 || sow>=604800)
   {
      badNavSubframeCount++;
      if (verboseLevel>1)
         out << msg.time.printf(timeFormat)
             << "  SOW bad: " << sow
             << endl;
      return;
   }
      
   DayTime howTime(week, msg.getHOWTime());
   if (howTime == msg.time)
   {
      // Move this back down to verboseLevel>0 when ITT fixes their code...
      if (verboseLevel>2)
         out << msg.time.printf(timeFormat) << "  header time is HOW time" << endl;
   }
   else if (howTime != msg.time+6)
   {
      badNavSubframeCount++;
      if (verboseLevel>1)
         out << msg.time.printf(timeFormat)
             << "  HOW/header time miscompare " << howTime.printf(timeFormat)
             << endl;
      return;
   }

//   if ((isAlm && !almOut) || (!isAlm && !ephOut))
//      return;

   NavIndex ni(RangeCarrierPair(msg.range, msg.carrier), msg.prn);
   prev[ni] = curr[ni];
   curr[ni] = msg;

   long sfa[10];
   msg.fillArray(sfa);
   long long_sfa[10];
   for( int j = 0; j < 10; j++ )
      long_sfa[j] = static_cast<long>( sfa[j] );

   if (gpstk::EngNav::subframeParity(long_sfa))
   {
      if (isAlm && almOut)
      {
         AlmanacPages& almPages = almPageStore[ni];
         EngAlmanac& engAlm = almStore[ni];
         SubframePage sp(sfid, page);
         almPages[sp] = msg;
         almPages.insert(make_pair(sp, msg));
         
         if (makeEngAlmanac(engAlm, almPages))
         {
            out << msg.time.printf(timeFormat)
                << "  Built complete alm from prn " << setw(2) << ni.second
                << " " << setw(2) << asString(ni.first.second)
                << " " << left << setw(6) <<  asString(ni.first.first)
                << endl;
            if (verboseLevel>1)
               engAlm.dump(out);
            almPages.clear();
            engAlm = EngAlmanac();
         }            
      }
      if (!isAlm && ephOut)
      {
         EphemerisPages& ephPages = ephPageStore[ni];
         ephPages[sfid] = msg;
         EngEphemeris engEph;

         if (makeEngEphemeris(engEph, ephPages))
         {
            out << msg.time.printf(timeFormat)
                << "  Built complete eph from prn " << setw(2) << ni.second
                << " " << setw(2) << asString(ni.first.second)
                << " " << left << setw(6) << asString(ni.first.first)
                << " iocd:0x" << hex << setw(3) << engEph.getIODC() << dec
                << endl;
            if (verboseLevel>1)
               out << engEph;
            ephStore[ni] = engEph;
         }
      }
   }
   else
   {
      badNavSubframeCount++;
      out << msg.time.printf(timeFormat)
          << "  Parity error on prn:" << setw(2) << msg.prn
          << " " << asString(msg.carrier)
             << ":" << setw(6) << left << asString(msg.range)
          << " TOW:" << setw(6) << msg.getHOWTime()
          << " SFID:" << sfid
          << endl;
   }
}  // end of process()
