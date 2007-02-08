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

#include <iostream>
#include <list>

#include "OrdApp.hpp"
#include "ElevationRange.hpp"
#include "Stats.hpp"
#include "CycleSlipList.hpp"

#include "DDEpoch.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

class DDStats : public BasicFramework
{
public:
   DDStats() throw();
   
   bool initialize(int argc, char *argv[]) throw();

protected:
   virtual void process();

private:
   ElevationRangeList elr;
   double sigmaMult;

   ifstream input;
   ofstream output;

   ObsID makeOID(const string& carrier, const string& code, const string& type) const;

   void outputStats(ofstream& s, DDEpochMap& ddem, SvElevationMap& pem) const;
};


//-----------------------------------------------------------------------------
// The constructor basically just sets up all the command line options
//-----------------------------------------------------------------------------
DDStats::DDStats() throw()
   : BasicFramework("ordStats", "Computes statistics on ddGen output.")
{}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool DDStats::initialize(int argc, char *argv[]) throw()
{
   CommandOptionWithAnyArg
      inputOpt('i', "input",
               "Where to read the ord data. The default is stdin."),

      outputOpt('r', "output",
               "Where to write the output. The default is stdout."),

      elevBinsOption('b', "elev-bin",
                     "A range of elevations, used in  computing"
                     " the statistical summaries. Repeat to specify multiple "
                     "bins. The default is \"-b 0-10 -b 10-20 -b 20-60 -b "
                     "10-90\"."),
      statsFileOption('o',"statsFile","Filename for output of stats only. Stats"
                      " will still be included at the end of the ord file."),

      sigmaOption('s',"sigma","Multiplier for sigma stripping used in "
                  "statistical computations. The default value is 6.");

   if (!BasicFramework::initialize(argc,argv))
      return false;

   if (outputOpt.getCount())
   {
      const string fn=outputOpt.getValue()[0];
      output.open(fn.c_str(), ios::out);
      if (debugLevel)
         cout << "# Sending output to" << fn << endl;
   }
   else
   {
      if (debugLevel)
         cout << "# Sending output to stdout" << endl;
      output.copyfmt(cout);
      output.clear(cout.rdstate());
      output.basic_ios<char>::rdbuf(cout.rdbuf());
   }

   if (inputOpt.getCount())
   {
      const string fn = inputOpt.getValue()[0];
      input.open(fn.c_str(), ios::in);
      if (debugLevel)
         cout << "# Reading double difference data from " << fn << endl;
   }
   else
   {
      if (debugLevel)
         cout << "# Reading double difference data from stdin" << endl;
      input.copyfmt(cin);
      input.clear(cin.rdstate());
      input.basic_ios<char>::rdbuf(cin.rdbuf());
   }

   // get elevation ranges, if specified
   if (elevBinsOption.getCount())
   {
      for (int i=0; i<elevBinsOption.getCount(); i++)
      {
         string pr = elevBinsOption.getValue()[i];
         float minElev = asFloat(pr);
         stripFirstWord(pr, '-');
         float maxElev = asFloat(pr);
         elr.push_back( ElevationRange(minElev, maxElev) );
      }
   }
   else
   {
      elr.push_back( ElevationRange( 0, 10) );
      elr.push_back( ElevationRange(10, 20) );
      elr.push_back( ElevationRange(20, 60) );
      elr.push_back( ElevationRange(60, 90) );
      elr.push_back( ElevationRange(10, 90) );
   }
   if (sigmaOption.getCount())
      sigmaMult = asDouble(sigmaOption.getValue().front());
   else
      sigmaMult = 6;

   std::ofstream extraOutput;   
   if (statsFileOption.getCount())
   {
      const string fn = statsFileOption.getValue()[0];
      extraOutput.open(fn.c_str(), ios::out);
   }  

   return true;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void DDStats::outputStats(ofstream& s, DDEpochMap& ddem, SvElevationMap& pem) const
{
const ObsID C1(ObsID::otRange,   ObsID::cbL1,   ObsID::tcCA);
const ObsID P1(ObsID::otRange,   ObsID::cbL1,   ObsID::tcP);
const ObsID L1(ObsID::otPhase,   ObsID::cbL1,   ObsID::tcP);
const ObsID D1(ObsID::otDoppler, ObsID::cbL1,   ObsID::tcP);
const ObsID S1(ObsID::otSNR,     ObsID::cbL1,   ObsID::tcP);
const ObsID C2(ObsID::otRange,   ObsID::cbL2,   ObsID::tcC2LM);
const ObsID P2(ObsID::otRange,   ObsID::cbL2,   ObsID::tcP);
const ObsID L2(ObsID::otPhase,   ObsID::cbL2,   ObsID::tcP);
const ObsID D2(ObsID::otDoppler, ObsID::cbL2,   ObsID::tcP);
const ObsID S2(ObsID::otSNR,     ObsID::cbL2,   ObsID::tcP);

   s << endl
     << "ord        elev   stddev    mean      # obs    # bad   # unk  max good  slips" << endl
     << "---------- -----  --------  --------  -------  ------  ------  --------  -----" << endl;

   for (ElevationRangeList::const_iterator i = elr.begin(); i != elr.end(); i++)
   {
      s << "c1 dd res  " << ddem.computeStats(C1, *i, pem) << "    " << endl;
      s << "p1 dd res  " << ddem.computeStats(P1, *i, pem) << "    " << endl;
      s << "l1 dd res  " << ddem.computeStats(L1, *i, pem) << "    " << endl;
      s << "d1 dd res  " << ddem.computeStats(D1, *i, pem) << "    " << endl;
      s << "s1 dd res  " << ddem.computeStats(S1, *i, pem) << "    " << endl;
      s << endl;
   }
   s << "------------------------------------------------------------------------ " << endl;

   for (ElevationRangeList::const_iterator i = elr.begin(); i != elr.end(); i++)
   {
      s << "p2 dd res  " << ddem.computeStats(P2, *i, pem) << "    " << endl;
      s << "l2 dd res  " << ddem.computeStats(L2, *i, pem) << "    " << endl;
      s << "d2 dd res  " << ddem.computeStats(D2, *i, pem) << "    " << endl;
      s << "s1 dd res  " << ddem.computeStats(S2, *i, pem) << "    " << endl;
      s << endl;
   }
   s << "------------------------------------------------------------------------ " << endl;
}


//-----------------------------------------------------------------------------
void DDStats::process()
{   
   // Read in all the double difference data
   DDEpochMap ddem;
   SvElevationMap pem;
   using namespace StringUtils;
   while (input)
   {
      string readBuffer;
      getline(input, readBuffer);
      strip(readBuffer);
      if (readBuffer.size() < 24 || readBuffer[0] == '#')
         continue;

      DayTime time;
      time.setToString(readBuffer.substr(0,19), "%4Y %3j %02H:%02M:%04.1f");

      istringstream iss(readBuffer.substr(20, string::npos));
      unsigned p, m, h;
      string carrier, code, type;
      double el, dd, clock;
      iss >> p >> carrier >> code >> type >> m >> el >> dd >> clock >> hex >> h >> dec;

      ObsID oid = makeOID(carrier, code, type);
      SatID prn(p, SatID::systemGPS);
      SatID master(m, SatID::systemGPS);

      DDEpoch& e = ddem[time];
      e.masterPrn = master;
      e.clockOffset = clock;
      e.dd[prn][oid] = dd;
      e.health[prn] = h;
      pem[time][prn] = el;
   }

   if (debugLevel)
      cout << "Done reading the double difference data ("
           << ddem.size() << " epochs)" << endl;
   if (debugLevel>1)
      ddem.dump(output, pem);
   
   outputStats(output, ddem, pem);
}


ObsID DDStats::makeOID(
   const string& carrier,
   const string& code,
   const string& type) const
{
   map< ObsID::CarrierBand, string >::const_iterator cbi;
   for (cbi = ObsID::cbStrings.begin(); cbi != ObsID::cbStrings.end(); cbi++)
      if (cbi->second == carrier)
         break;
   if (cbi == ObsID::cbStrings.end())
      cout << "Uh, we have a cb problem" << endl;

   map< ObsID::TrackingCode, string >::const_iterator tci;
   for (tci = ObsID::tcStrings.begin(); tci != ObsID::tcStrings.end(); tci++)
      if (tci->second == code)
         break;
   if (tci == ObsID::tcStrings.end())
      cout << "Uh, we have a tc problem" << endl;
         
   map< ObsID::ObservationType, string >::const_iterator oti;
   for (oti = ObsID::otStrings.begin(); oti != ObsID::otStrings.end(); oti++)
      if (oti->second == type)
         break;
   if (oti == ObsID::otStrings.end())
      cout << "Uh, we have a ot problem" << endl;

   return ObsID(oti->first, cbi->first, tci->first);
}


//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      DDStats crap;
      if (!crap.initialize(argc, argv))
         exit(0);
      crap.run();
   }
   catch (gpstk::Exception &exc)
   { cout << exc << endl; }
   catch (std::exception &exc)
   { cerr << "Caught std::exception " << exc.what() << endl; }
   catch (...)
   { cerr << "Caught unknown exception" << endl; }
}

