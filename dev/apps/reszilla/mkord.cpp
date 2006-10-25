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

//
// Compute observed range deviations (ORDs) and double
// difference residuals from rinex obs files.
//

#include <fstream>

#include <BCEphemerisStore.hpp>
#include <CommandOptionWithTimeArg.hpp>
#include <TropModel.hpp>
#include <EpochClockModel.hpp>

#include "readers.hpp"
#include "ObsReader.hpp"
#include "FFIdentifier.hpp"
#include "util.hpp"
#include "ordUtils.hpp"
#include "RobustLinearEstimator.hpp"
#include "OrdEngine.hpp"

using namespace std;

int verbosity;
string timeFormat;
ElevationRangeList elr;

using namespace gpstk::StringUtils;

char* verboseHelp =
"\n"
"Verbosity values:\n"
"  0: nothing but the results\n"
"  1: Output status before potentially time consuming operations (default)\n"
"  2: more details about each step and the options chosen\n"
"  3: add the reasons for editing data\n"
"  4: dump intermediate values for each epoch (can be QUITE verbose)\n"
"\n"
"Types in the raw output files:\n"
"   0 - c1p2 observed range deviation\n"
"   50 - computed clock, difference from estimate\n"
"   51 - linear clock estimate, abdev \n"
"\n"
"Misc notes:\n"
"\n"
"Example command to compute ORDs on an ICD-GPS-211 formated smoothed\n"
"measurement data file:\n"
"   reszilla --omode=p1 --svtime --msc=mscoords.cfg -m 85401\n"
"      -o asm2004.138 -e s011138a.04n\n"
"\n"
"All output quantities (stddev, min, max, ord, clock, ...)\n"
"are in meters.\n"
;

int main(int argc, char *argv[])
{
   string hmsFmt="%Y %3j %02H:%02M:%04.1f";
   string hmsFmt2="%Y %3j %02H:%02M:%02S";
   string sodFmt="%Y %3j %7.1s";
   string ordMode="p1p2";
   unsigned long msid=0;
   double sigmaMask=6;

   bool rawOutput=false;
   bool keepUnhealthy=false;
   bool keepWarts=false;


   gpstk::DayTime startTime(gpstk::DayTime::BEGINNING_OF_TIME);
   gpstk::DayTime stopTime(gpstk::DayTime::END_OF_TIME);

   timeFormat = hmsFmt;
   verbosity = 1;

   try
   {
      gpstk::CommandOptionWithAnyArg
         obsFileOption('o', "obs", "Observation data file name. If this "
                       "option is specified more than once the contents of "
                       "all files will be used.", true);

      gpstk::CommandOptionWithAnyArg
         mscFileOption('\0', "msc", "Station coordinate file");

      gpstk::CommandOptionWithNumberArg
         msidOption('m', "msid", "Station to process data for. Used to select "
                    "a station from smoothed data files.");
         
      gpstk::CommandOptionWithAnyArg
         ephFileOption('e', "ephemeris", "Ephemeris data file name (either "
                       "broadcast in RINEX nav, broadcast in FIC, or precise "
                       "in SP3).", false);

      gpstk::CommandOptionWithAnyArg
         antennaPosOption('p', "pos", "Location of the antenna in meters ECEF."
                          , false);

      gpstk::CommandOptionWithAnyArg
         metFileOption('w', "weather", "Weather data file name (RINEX met "
                       "format only).");

      gpstk::CommandOptionNoArg
         nearOption('n', "search-near", "Use BCEphemeris.searchNear()");
      
      gpstk::CommandOptionNoArg
         svTimeOption('\0', "svtime", "Observation data is in SV time frame. "
                      "The default is RX time frame.");

      gpstk::CommandOptionNoArg
         keepWartsOption('\0', "keep-warts", "Keep any warts that are in "
                         "the data. The defailt is to remove them.");

      gpstk::CommandOptionNoArg
         keepUnhealthyOption('\0',"keep-unhealthy", "Use unhealthy SVs in the "
                             "clock computition and statistics, the default "
                             "is to toss.");

      gpstk::CommandOptionNoArg
         statsOption('s', "stats", "Compute & output the statistics");

      gpstk::CommandOptionWithAnyArg
         rawOutputOption('r', "raw-output", "Dump the computed residuals/ords "
                         "into specified file. If '-' is given as the file "
                         "name, the output is sent to standard output. The "
                         "default is to not otput the raw residuals.");

      gpstk::CommandOptionWithAnyArg
         timeFmtOption('t', "time-format", "Daytime format specifier used for "
                       "the timestamps in the raw output. The default is \"" 
                       + timeFormat + "\". If this option is specified with "
                       "the format as \"s\", the format \"" + sodFmt + "\" "
                       "is used. If this option is specified with the format "
                       "as \"s\", the format \"" + hmsFmt2 + "\" is used.");

      gpstk::CommandOptionWithAnyArg
         ordModeOption('\0', "omode", "ORD mode: p1p2, c1p2, c1, p1, c2, p2, smo. "
                       "Note that the smo mode often requires the --svtime "
                       "option to be specified. The default is " + ordMode);

      gpstk::CommandOptionNoArg
         clkAnalOption('\0', "clock-est", "Compute a linear clock estimate");

      gpstk::CommandOptionWithAnyArg
         elevBinsOption('b', "elev-bin", "A range of elevations, used in "
                        "computing the statistical summaries. Repeat to "
                        "specify multiple bins. The default is \"-b 0-10 "
                        "-b 10-20 -b 20-60 -b 10-90\".");

      gpstk::CommandOptionWithAnyArg
         sigmaOption('\0', "sigma", "Multiplier for sigma stripping used "
                     "in computation of the the statistics "
                     "on the raw residuals. The default value is "
                     + asString((int)sigmaMask) + ".");

      gpstk::CommandOptionNoArg
         helpOption('h', "help", "Print usage. Repeat for more info. ");

      gpstk::CommandOptionWithNumberArg
         verbosityOption('v', "verbosity", "How much detail to provide "
                         "about intermediate steps. The default is 1. "
                         "Specify -hh for more help.");

      string appDesc("Computes observed range deviations from GPS observations.");
      gpstk::CommandOptionParser cop(appDesc);
      cop.parseOptions(argc, argv);

      if (helpOption.getCount() || cop.hasErrors())
      {
         if (cop.hasErrors() && helpOption.getCount()==0)
         {
            cop.dumpErrors(cout);
            cout << "use -h for help, -hh for more help." << endl;
         }
         else
         {
            cop.displayUsage(cout);
            if (helpOption.getCount() > 1)
               cout << verboseHelp << endl;
         }
         exit(0);
      }

      if (verbosityOption.getCount())
         verbosity = asInt(verbosityOption.getValue()[0]);
      if (verbosity>4)
         gpstk::ObsRngDev::debug = true;

      if (ordModeOption.getCount())
         ordMode = lowerCase(ordModeOption.getValue()[0]);
      
      bool svTime = svTimeOption.getCount()>0;

      if (msidOption.getCount())
         msid = asUnsigned(msidOption.getValue().front());

      if (sigmaOption.getCount())
         sigmaMask = asDouble(sigmaOption.getValue().front());
      if (keepUnhealthyOption.getCount())
         keepUnhealthy=true;
      
      if (keepWartsOption.getCount())
         keepWarts=true;

      if (timeFmtOption.getCount())
      {
         if ((timeFmtOption.getValue())[0] == "s")
            timeFormat = sodFmt;
         else
            timeFormat = (timeFmtOption.getValue())[0];
      }

      // set up where the raw data will be written
      ofstream ofs;
      string outputFileName;
      if (rawOutputOption.getCount())
      {
         rawOutput=true;

         outputFileName=(rawOutputOption.getValue())[0];
         if (outputFileName != "-")
            ofs.open(outputFileName.c_str());
         else
         {
            ofs.copyfmt(std::cout);
            ofs.clear(std::cout.rdstate());
            ofs.std::basic_ios<char>::rdbuf(std::cout.rdbuf());
         }
      }

      // Set up the elevation ranges for the various statistical summaries
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

      if (verbosity)
      {
         cout << "--------------------------------------------------------------" << endl
              << "Observed Rage Deviation (ORD) mode: " << ordMode << endl
              << "Format to use for time in raw output: " << timeFormat << endl
              << "Data time tag: " << (svTime?"sv":"rx") << endl
              << "Sigma stripping multiplier: " << sigmaMask << endl
              << "Verbosity: " << verbosity << endl
              << "Elevation bins: ";
         
         for (ElevationRangeList::const_iterator i=elr.begin(); i!= elr.end(); i++)
            cout << i->first << "-" << i->second << " ";
         cout << endl;

         if (msid)
         {
            cout << "msid: " << msid << endl;
            if (mscFileOption.getCount()>0)
               cout << "msc file: " << mscFileOption.getValue().front() << endl;
         }
         
         if (startTime != gpstk::DayTime(gpstk::DayTime::BEGINNING_OF_TIME) ||
             stopTime != gpstk::DayTime(gpstk::DayTime::END_OF_TIME))
            cout << "Start time: " << startTime.printf(timeFormat) << endl
                 << "Stop time: " << stopTime.printf(timeFormat) << endl;

         if (rawOutput)
            cout << "Raw output file: " << outputFileName << endl;
         
         if (keepUnhealthy)
            cout << "Keeping unhealthy SVs in statistics." << endl;
         else
            cout << "Ignoring unhealthy SVs in statistics." << endl;
            
         cout << "--------------------------------------------------------------" << endl;
      }

      // -------------------------------------------------------------------
      // End of processing/checking command line arguments.
      // -------------------------------------------------------------------

      // Get the ephemeris data
      gpstk::EphemerisStore& eph = *read_eph_data(ephFileOption);
      if (nearOption.getCount())
      {
         gpstk::BCEphemerisStore& bce = dynamic_cast<gpstk::BCEphemerisStore&>(eph);
         bce.SearchNear();
      }

      // Get the weather data...
      gpstk::WxObsData& wod = *read_met_data(metFileOption);

      // Get the station position
      gpstk::Triple ap;
      if (antennaPosOption.getCount() > 0)
      {
         string aps = antennaPosOption.getValue()[0];
         if (numWords(aps) != 3)
            cout << "Please specify three coordinates in the antenna postion." << endl;
         else
            for (int i=0; i<3; i++)
               ap[i] = asDouble(word(aps, i));
      }
      else if (msid && mscFileOption.getCount() > 0)
      {
         string mscfn = (mscFileOption.getValue())[0];
         read_msc_data(mscfn, msid, ap);
      }
      else
      {
         string fn = (obsFileOption.getValue())[0];
         gpstk::ObsReader obsReader(fn, verbosity);
         if (obsReader.inputType == gpstk::FFIdentifier::tRinexObs)
            ap = obsReader.roh.antennaPosition;
      }
      
      if (gpstk::RSS(ap[0], ap[1], ap[2]) < 1)
      {
         cout << "Warning! The antenna appears to be within one meter of the" << endl
              << "center of the geoid. This program is not capable of" << endl
              << "accurately estimating the propigation of GNSS signals" << endl
              << "through solids such as a planetary crust or magma. Also," << endl
              << "if this location is correct, your antenna is probally" << endl
              << "no longer in the best of operating condition." << endl;
         exit(-1);
      }

      // Set up our clock model
      gpstk::EpochClockModel cm;
      cm.setSigmaMultiplier(1.5);
      cm.setElevationMask(10);
      if (keepUnhealthy)
         cm.setSvMode(gpstk::ObsClockModel::ALWAYS);

      // Use a New Brunswick trop model.
      gpstk::NBTropModel tm;

      // Now set up the function object that is used to compute the ords.
      OrdEngine ordEngine(eph, wod, ap, tm);
      ordEngine.svTime = svTimeOption;
      ordEngine.keepWarts = keepWarts;
      ordEngine.setMode(ordMode);
      ordEngine.verboseLevel = verbosity;
      ordEngine.keepUnhealthy = keepUnhealthy;
      gpstk::ORDEpochMap ordEpochMap;

      // Walk through each obs file, reading and computing ords along the way.
      for (int i=0; i<obsFileOption.getCount(); i++)
      {
         string fn = (obsFileOption.getValue())[i];
         gpstk::ObsReader obsReader(fn, verbosity);
         obsReader.msid = msid;

         while (obsReader())
         {
            gpstk::ObsEpoch obs(obsReader.getObsEpoch());

            if (!obsReader())
               break;

            if (obs.time < startTime || obs.time > stopTime)
               continue;

            gpstk::ORDEpoch oe = ordEngine(obs);

            cm.addEpoch(oe);
            oe.applyClockModel(cm);
            if (verbosity>3)
               cout << "clk:" << cm << endl << "clk:" << cm.getOffset() << endl;

            // Only add epochs that have a valid clock estimate
            if (!oe.clockOffset.is_valid())
               continue;

            ordEpochMap[obs.time] = oe;
         }
      }

      // Now, output statistics to stdout
      if (statsOption.getCount()==0)
         dumpStats(ordEpochMap, ordMode, sigmaMask);
      
      // Save the raw ORDs to a file
      if (rawOutput)
         dumpOrds(ofs, ordEpochMap);
      
      RobustLinearEstimator rle;
      if (clkAnalOption.getCount())
         estimateClock(ordEpochMap, rle);
      
      if (rawOutput)
         dumpClock(ofs, ordEpochMap, rle);
   }
   catch (gpstk::Exception& e)
   {
      cout << "Caught Excption: " << typeid(e).name() << endl
           << "Terminating." << endl;
   }
}
