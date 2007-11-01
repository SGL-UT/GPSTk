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

#include <fstream>
#include <string>

#include <BasicFramework.hpp>
#include <MSCData.hpp>
#include <MSCStream.hpp>
#include <EpochClockModel.hpp>

#include "OrdEngine.hpp"
#include "ObsReader.hpp"
#include "EphReader.hpp"

#include "DDEpoch.hpp"
#include "PhaseCleaner.hpp"
#include "CycleSlipList.hpp"
#include "SvElevationMap.hpp"
#include "ElevationRange.hpp"
#include "GPSEphemerisStore.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


class DDGen : public gpstk::BasicFramework
{
public:
   DDGen() throw();
   
   bool initialize(int argc, char *argv[]) throw();

protected:
   virtual void spinUp();
   virtual void process();

private:

   string ddMode;
   double minArcGap;        // seconds
   double minArcTime;       // seconds
   unsigned long minArcLen; // epochs
   unsigned long msid;
   unsigned long window;    // seconds
   unsigned long minSNR;    // dB
   Triple antennaPos;
   
   ObsEpochMap obs1, obs2;
   CommandOptionWithAnyArg obs1FileOption, obs2FileOption, ephFileOption;
   ElevationRangeList elr;
   bool computeStats, computeAll, removeUnhealthy;
   EphReader healthSrcER;
   
   void readObsFile(const CommandOptionWithAnyArg& obsFileOption,
                    const XvtStore<SatID>& eph,
                    ObsEpochMap &oem);
   
   void filterUnhealthyObs(const XvtStore<SatID>& eph,
                           ObsEpochMap &oem);
   
};

//-----------------------------------------------------------------------------
// The constructor basically just sets up all the command line options
//-----------------------------------------------------------------------------
DDGen::DDGen() throw()
   : BasicFramework("ddGen", "Computes double-difference residuals from raw observations."),
     ddMode("all"), minArcGap(60), minArcTime(60), minArcLen(5), msid(0), window(0), minSNR(20),
     computeStats(false), computeAll(false), removeUnhealthy(false),

     obs1FileOption('1', "obs1", 
                    "Where to get the first receiver's obs data.", true),
     obs2FileOption('2', "obs2", 
                    "Where to get the second receiver's obs data.", true),
     ephFileOption('e', "eph",  "Where to get the ephemeris data. Can be "
                    "RINEX, FIC, or SP3.", true)
{}

//-----------------------------------------------------------------------------
// Here the command line options parsed and used to configure the program
//-----------------------------------------------------------------------------
bool DDGen::initialize(int argc, char *argv[]) throw()
{
   CommandOptionWithAnyArg
      ddModeOption('\0', "ddmode", "Specifies what observations are used to "
                    "compute the double difference residuals. Valid values are:"
                    " all. The default is " + ddMode),
      minArcTimeOption('\0', "min-arc-time", "The minimum length of time "
                    "(in seconds) that a sequence of observations must "
                    "span to be considered as an arc. The default "
                    "value is " + asString(minArcTime, 1) + " seconds."),
      minArcGapOption('\0', "min-arc-gap", "The minimum length of time "
                    "(in seconds) between two arcs for them to be "
                    "considered separate arcs. The default value "
                    "is " + asString(minArcGap, 1) + " seconds."),
      minArcLenOption('\0', "min-arc-length", "The minimum number of "
                    "epochs that can be considered an arc. The "
                    "default value is " + asString(minArcLen) +
                    " epochs."),
      elevBinsOption('b', "elev-bin",
                    "A range of elevations, used in  computing"
                    " the statistical summaries. Repeat to specify multiple "
                    "bins. The default is \"-b 0-10 -b 10-20 -b 20-60 -b "
                    "10-90\"."),
      mscFileOption('c', "msc", "Station coordinate file."),
      antennaPosOption('p', "pos", "Location of the antenna in meters ECEF."),
      ephHealthSource('E',"health-src","Do not use data from unhealthy SVs "
                    "as determined using this ephemeris source.  Can be "
                    "RINEX navigation or FIC file(s). ");

   CommandOptionWithNumberArg 
      msidOption('m', "msid", "Station to process data for. Used to "
                    "select a station position from the msc file or data "
                    "from a SMODF file."),
                 
      timeSpanOption('w',"window","Compute mean values of the double "
                    "differences over this time span (seconds). (15 min = 900)"), 
      SNRoption('S',"SNR","Only included observables with a raw signal strength, "
                    "or SNR, of at least this value, in dB. The default is 20 dB.");
                    
   CommandOptionNoArg 
      statsOption('s', "stats", "Compute stats on the double differences."),
      allComboOption('a', "all-combos", "Compute all combinations, don't just "
                    "use one master SV."),
      cycleSlipOption('\0', "cycle-slips", "Output a list of cycle slips");

   if (!BasicFramework::initialize(argc,argv)) 
      return false;
      
   if (timeSpanOption.getCount() && allComboOption.getCount())
   {
      cerr << "\n\n You cannot set up the tool to compute averages while computing all\n "
           << "SV combos.  It doesn't make sense to do so because the arcs are so\n "
           << "small. Sorry. Exiting....\n\n";
      return false;
   }
   
   if (mscFileOption.getCount() && !msidOption.getCount())
   {
      cerr << "\n\n You gave a station coordinate file but didn't specify\n "
           << "the station ID. Exiting....\n\n";
      return false;
   }

   if (timeSpanOption.getCount() && (asDouble(timeSpanOption.getValue()[0])<= 0 ))
   {
      cerr << "\n\n Please enter a positive value for the time span window.\n "
           << "Exiting....\n\n";
      return false;
   }
   
   if ((SNRoption.getCount() && asDouble(SNRoption.getValue()[0])<= 0 ))
   {
      cerr << "\n\n Please enter a SNR value >= 0 dB.\n "
           << "Exiting....\n\n";
      return false;
   }
   
   if (msidOption.getCount())
      msid = asUnsigned(msidOption.getValue().front());

   // Get the station position
   if (antennaPosOption.getCount())
   {
      string aps = antennaPosOption.getValue()[0];
      if (numWords(aps) != 3)
      {
         cerr << "Please specify three coordinates in the antenna postion.\n"
              << "Exiting....\n\n";
         return false;
      }
      else
         for (int i=0; i<3; i++)
            antennaPos[i] = asDouble(word(aps, i));
   }
   else if (msid && mscFileOption.getCount())
   {
      string mscfn = (mscFileOption.getValue())[0];
      MSCStream msc(mscfn.c_str(), ios::in);
      MSCData mscd;
      while (msc >> mscd && mscd.station != msid)
         ;
      if (mscd.station == msid)
         antennaPos = mscd.coordinates;
   }
   else
   {
      string fn = (obs1FileOption.getValue())[0];
      ObsReader obsReader(fn, verboseLevel);
      if (obsReader.inputType == FFIdentifier::tRinexObs)
         antennaPos = obsReader.roh.antennaPosition;
   }

   if (RSS(antennaPos[0], antennaPos[1], antennaPos[2]) < 1)
   {
      cerr << "Warning! The antenna appears to be within one meter of the" << endl
           << "center of the geoid. This program is not capable of" << endl
           << "accurately estimating the propigation of GNSS signals" << endl
           << "through solids such as a planetary crust or magma. Also," << endl
           << "if this location is correct, your antenna is probably" << endl
           << "no longer in the best of operating condition." << endl;
      return false;
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
   
   if (ephHealthSource.getCount())
   {
      healthSrcER.verboseLevel = verboseLevel;
      for (int i=0; i<ephHealthSource.getCount(); i++)
         healthSrcER.read(ephHealthSource.getValue()[i]);
      gpstk::XvtStore<SatID>& ephStoreTemp = *healthSrcER.eph; 
      if (typeid(ephStoreTemp)!=typeid(GPSEphemerisStore))
      {
         cerr << "You provided an eph source that was not broadcast ephemeris.\n"
                 "(Precise ephemeris does not contain health info and can't be \n"
                 " used with the \"-E\" option.) Exiting... \n";
         return false;
      }
      removeUnhealthy = true;
   }
   
   if (ddModeOption.getCount())
      ddMode = lowerCase(ddModeOption.getValue()[0]);

   if (minArcTimeOption.getCount())
      minArcTime = asDouble(minArcTimeOption.getValue().front());
   
   if (minArcLenOption.getCount())
      minArcLen = asUnsigned(minArcLenOption.getValue().front());

   if (minArcGapOption.getCount())
      minArcGap = asDouble(minArcGapOption.getValue().front());

   if (statsOption.getCount())
      computeStats = true;
   
   if (allComboOption.getCount())
      computeAll = true;
      
   if (timeSpanOption.getCount())
      window = asUnsigned(timeSpanOption.getValue().front());
      
   if (SNRoption.getCount())
      minSNR = asUnsigned(SNRoption.getValue().front());

   return true;
}

//-----------------------------------------------------------------------------
// General program setup
//-----------------------------------------------------------------------------
void DDGen::spinUp()
{
   if (verboseLevel)
   {
      cout << "# Double difference mode: " << ddMode << endl
           << "# Minimum arc time: " << minArcTime << " seconds" << endl
           << "# Minimum arc length: " << minArcLen << " epochs" << endl
           << "# Minimum gap length: " << minArcGap << " seconds" << endl
           << "# Antenna Position: " << setprecision(8) << antennaPos << endl;
      if (msid)
         cout << "# msid: " << msid << endl;

      if (computeAll)
         cout << "# Using all SV combinations." << endl;
      else
         cout << "# Using one master SV combinations." << endl;
         
      if (window)
         cout << "# Computing mean values for " << window << " second windows" << endl;
   }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void DDGen::process()
{
   EphReader ephReader;
   ephReader.verboseLevel = verboseLevel;
   for (int i=0; i<ephFileOption.getCount(); i++)
      ephReader.read(ephFileOption.getValue()[i]);

   ObsEpochMap oem1, oem2;

   if (debugLevel)
      cout << "# Reading obs from Rx1" << endl;
   readObsFile(obs1FileOption, *ephReader.eph, oem1);
   
   if (removeUnhealthy)
      filterUnhealthyObs(*healthSrcER.eph, oem1);
   
   if (verboseLevel)
      cout << "# Reading obs from Rx2" << endl;
   readObsFile(obs2FileOption, *ephReader.eph, oem2);

   if (removeUnhealthy)
      filterUnhealthyObs(*healthSrcER.eph, oem2);

   SvElevationMap pem = elevation_map(oem1, antennaPos, *ephReader.eph);
   DDEpochMap ddem;
   ddem.debugLevel = debugLevel;
   ddem.windowLength = window;
   
   if (computeAll)
      ddem.useMasterSV = false;
   else
      ddem.useMasterSV = true;
      
   ddem.compute(oem1, oem2, pem);
   
   // Here we compute a phase double difference that is Better(TM)    
   if (computeAll)
   {
      PhaseCleanerA pc(minArcLen, minArcTime, minArcGap);
      pc.debugLevel = debugLevel;
      pc.addData(oem1, oem2, minSNR);
      pc.debias(pem);
      pc.getPhaseDD(ddem);          
   }
   else
   {
      PhaseCleaner pc(minArcLen, minArcTime, minArcGap);
      pc.debugLevel = debugLevel;
      pc.addData(oem1, oem2, minSNR);
      pc.debias(pem);
      pc.getPhaseDD(ddem);
      CycleSlipList sl;
      pc.getSlips(sl, pem);
      dump(cout, sl);
   }
    
   if (window)
   {
      // this option was only allowed for the master SV method, i.e. !computeAll
      if (verboseLevel)
         cout << "# Computing averages for windows of " << window << " seconds.\n";
      ddem.outputAverages(cout);   
   }
   
   if (computeAll && computeStats)
      ddem.outputStatsForAllCombos(cout, elr);
   else if (computeStats)
      ddem.outputStats(cout, elr);
   else
      ddem.dump(cout);
}

//-----------------------------------------------------------------------------
// Read a single file of observation data, computing receiver clock offsets along
// the way.
//-----------------------------------------------------------------------------
void DDGen::readObsFile(
   const CommandOptionWithAnyArg& obsFileOption, 
   const XvtStore<SatID>& eph,
   ObsEpochMap &oem)
{
   // Just a placeholder
   gpstk::WxObsData wod;

   // Use a New Brunswick trop model.
   NBTropModel tm;

   // Now set up the function object that is used to compute the ords.
   OrdEngine ordEngine(eph, wod, antennaPos, "smart", tm);
   ordEngine.verboseLevel = verboseLevel;
   ordEngine.debugLevel = debugLevel;

   // Set up a simple epoch clock model.
   EpochClockModel cm(1.5, 10, ObsClockModel::HEALTHY);
   const GPSGeoid gm;

   // Walk through each obs file, reading and computing ords along the way.
   for (int i=0; i<obsFileOption.getCount(); i++)
   {
      string fn = (obsFileOption.getValue())[i];
      ObsReader obsReader(fn, verboseLevel);
      obsReader.msid = msid;

      while (obsReader())
      {
         ObsEpoch obs(obsReader.getObsEpoch());
         if (!obsReader())
            break; 

         ORDEpoch oe = ordEngine(obs);
         
         cm.addEpoch(oe);

         if (cm.isOffsetValid())
         {
            // Need to keep clock offset in seconds
            obs.rxClock = cm.getOffset() / gm.c();
            oem[obs.time] = obs;
         }
         else
         {
            if (verboseLevel>2)
               cout << "# Could not estimate clock for epoch at " << obs.time << endl;
         }
      }
   }
}

void DDGen::filterUnhealthyObs(const XvtStore<SatID>& eph, ObsEpochMap &oem)
{
   ObsEpochMap::iterator oemIter;   
   for (oemIter=oem.begin(); oemIter!=oem.end(); oemIter++)
   {
      const GPSEphemerisStore& bce = dynamic_cast<const GPSEphemerisStore&>(eph);
      const DayTime& t = oemIter->first;
      ObsEpoch& obsEpoch = oemIter->second;
      
      ObsEpoch::iterator oeIter;
      for(oeIter=obsEpoch.begin(); oeIter!=obsEpoch.end();)
      {
         const SatID& svid = oeIter ->first;
                 
         try
         {
            EngEphemeris ephTemp = bce.findEphemeris(svid, t);
            short health =  ephTemp.getHealth();
            if (health != 0)
              obsEpoch.erase(oeIter++);
            else
              oeIter++;
         }
         catch (gpstk::Exception &exc)
         { 
            if (verboseLevel || debugLevel)
               cout << "# DDGen::filterUnhealthyObs - probably missing eph data\n";
         }
      }
   }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      DDGen crap;
      if (!crap.initialize(argc, argv))
         exit(0);
      crap.run();
   }
   catch (gpstk::Exception &exc)
   { cerr << exc << endl; }
   catch (std::exception &exc)
   { cerr << "Caught std::exception " << exc.what() << endl; }
   catch (...)
   { cerr << "Caught unknown exception" << endl; }
}
