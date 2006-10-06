#pragma ident "$Id$"

//
// Compute observed range deviations (ORDs) and double
// difference residuals from rinex obs files.
//

#include <fstream>
#include <BCEphemerisStore.hpp>
#include <CommandOptionWithTimeArg.hpp>

#include "readers.hpp"
#include "util.hpp"
#include "ordUtils.hpp"
#include "PhaseCleaner.hpp"
#include "RobustLinearEstimator.hpp"

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
"   50 - computed clock, difference from estimate, strip\n"
"   51 - linear clock estimate, abdev \n"
"Double difference types:\n"
"   10 - c1     20 - c2\n"
"   11 - p1     21 - p2\n"
"   12 - l1     22 - l2\n"
"   13 - d1     23 - d2\n"
"   14 - s1     24 - s2 (Why? Because I can!)\n"
"\n"
"Misc notes:\n"
"\n"
"The criteria min-arc-time and min-arc-length are both required to be met\n"
"for a arc to be valid in double difference mode.\n"
"\n"
"Example command to compute ORDs on an ICD-GPS-211 formated smoothed\n"
"measurement data file:\n"
"   reszilla --omode=p1 --svtime --msc=mscoords.cfg -m 85401\n"
"      -o asm2004.138 -e s011138a.04n\n"
"\n"
"All output quantities (stddev, min, max, ord, clock, double differnce, ...)\n"
"are in meters.\n"
;

int main(int argc, char *argv[])
{
   string hmsFmt="%Y %3j %02H:%02M:%04.1f";
   string hmsFmt2="%Y %3j %02H:%02M:%02S";
   string sodFmt="%Y %3j %7.1s";
   string ordMode="p1p2";
   string ddMode="sv";
   unsigned long msid=0;
   double sigmaMask=6;

   double minArcGap = 60; // seconds
   double minArcTime = 60; // seconds
   long minArcLen=5; // epochs

   bool rawOutput=false;
   bool keepUnhealthy=false;


   gpstk::DayTime startTime(gpstk::DayTime::BEGINNING_OF_TIME);
   gpstk::DayTime stopTime(gpstk::DayTime::END_OF_TIME);

   timeFormat = hmsFmt;
   verbosity = 1;

   try
   {
      gpstk::CommandOptionWithAnyArg
         obs1FileOption('o', "obs1", "Observation data file name. If this "
                        "option is specified more than once the contents of "
                        "all files will be used.", true);

      gpstk::CommandOptionWithAnyArg
         obs2FileOption('2', "obs2", " Second receiver's observation data "
                        "file name. Only used when computing a double "
                        "difference. If this option is specified more than "
                        "once the contents of all the files will be used.");

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
         metFileOption('w', "weather", "Weather data file name (RINEX met "
                       "format only).");

      gpstk::CommandOptionNoArg
         nearOption('n', "search-near", "Use BCEphemeris.searchNear()");
      
      gpstk::CommandOptionNoArg
         clkOption('c', "clock-from-rinex", "Use the receiver clock offset "
                   "from the rinex obs data.");

      gpstk::CommandOptionNoArg
         svTimeOption('\0', "svtime", "Observation data is in SV time frame. "
                      "The default is RX time frame.");

      gpstk::CommandOptionNoArg
         checkObsOption('\0',"check-obs", "Report data rate, order of data, "
                        "data present, data gaps");

      gpstk::CommandOptionNoArg
         keepUnhealthyOption('\0',"keep-unhealthy", "Use unhealthy SVs in the "
                             "clock computition and statistics, the default "
                             "is to toss.");

      gpstk::CommandOptionNoArg
         statsOption('s', "no-stats", "Don't compute & output the statistics");

      gpstk::CommandOptionNoArg
         cycleSlipOption('\0', "cycle-slips", "Output a list of cycle slips");
   
      gpstk::CommandOptionWithAnyArg
         rawOutputOption('r', "raw-output", "Dump the computed residuals/ords "
                         "into specified file. If '-' is given as the file "
                         "name, the output is sent to standard output. The "
                         "default is to not otput the raw residuals.");

      gpstk::CommandOptionWithTimeArg
         startTimeOption('\0', "start-time", "%4Y/%03j/%02H:%02M:%05.2f",
                         "Ignore obs data prior to this time in the "
                         "analysis. The time is specified using the format "
                         "%4Y/%03j/%02H:%02M:%05.2f. The default value is to "
                         "start with the first data found.");
      
      gpstk::CommandOptionWithTimeArg
         stopTimeOption('\0', "stop-time",  "%4Y/%03j/%02H:%02M:%05.2f",
                        "Ignore obs data after to this time in the "
                        "analysis. The time is specified using the format "
                        "%4Y/%03j/%02H:%02M:%05.2f. The default value is to "
                        "process all data.");
      
      gpstk::CommandOptionWithAnyArg
         timeFmtOption('t', "time-format", "Daytime format specifier used for "
                       "the timestamps in the raw output. The default is \"" 
                       + timeFormat + "\". If this option is specified with "
                       "the format as \"s\", the format \"" + sodFmt + "\" "
                       "is used. If this option is specified with the format "
                       "as \"s\", the format \"" + hmsFmt2 + "\" is used.");

      gpstk::CommandOptionWithAnyArg
         ordModeOption('\0', "omode", "ORD mode: P1P2, C1P2, C1, P1, P2. The "
                       "default is " + ordMode);

      gpstk::CommandOptionNoArg
         clkAnalOption('\0', "clock-est", "Compute a linear clock estimate");

      gpstk::CommandOptionWithAnyArg
         ddModeOption('\0', "ddmode", "Double difference residual mode: none, "
                      "sv, or c1p2. The default is " + ddMode + ".");

      gpstk::CommandOptionWithAnyArg
         minArcTimeOption('\0', "min-arc-time", "The minimum length of time "
                          "(in seconds) that a sequence of observations must "
                          "span to be considered as an arc. The default "
                          "value is " + asString(minArcTime, 1) + " seconds.");

      gpstk::CommandOptionWithAnyArg
         minArcGapOption('\0', "min-arc-gap", "The minimum length of time "
                         "(in seconds) between two arcs for them to be "
                         "considered separate arcs. The default value "
                         "is " + asString(minArcGap, 1) + " seconds.");

      gpstk::CommandOptionWithNumberArg
         minArcLenOption('\0', "min-arc-length", "The minimum number of "
                         "epochs that can be considered an arc. The "
                         "default value is " + asString(minArcLen) +
                         " epochs.");

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

      string appDesc("Computes various residuals from GPS observations.");
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

      if (ddModeOption.getCount())
         ddMode = lowerCase(ddModeOption.getValue().front());
      
      if (ordModeOption.getCount())
         ordMode = lowerCase(ordModeOption.getValue()[0]);
      
      bool svTime = svTimeOption.getCount()>0;

      if (msidOption.getCount())
         msid = asUnsigned(msidOption.getValue().front());

      if (sigmaOption.getCount())
         sigmaMask = asDouble(sigmaOption.getValue().front());

      if (minArcTimeOption.getCount())
         minArcTime = asDouble(minArcTimeOption.getValue().front());

      if (minArcLenOption.getCount())
         minArcLen = asUnsigned(minArcLenOption.getValue().front());

      if (minArcGapOption.getCount())
         minArcGap = asDouble(minArcGapOption.getValue().front());

      if (keepUnhealthyOption.getCount())
         keepUnhealthy=true;

      if (timeFmtOption.getCount())
      {
         if ((timeFmtOption.getValue())[0] == "s")
            timeFormat = sodFmt;
         else
            timeFormat = (timeFmtOption.getValue())[0];
      }

      if (startTimeOption.getCount())
         startTime = startTimeOption.getTime()[0];
      if (stopTimeOption.getCount())
         stopTime = stopTimeOption.getTime()[0];

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

      if (ddMode=="none" && obs2FileOption.getCount())
      {
         cout << "Specifying two sets of obs data requires a ddmode other than 'none'." << endl;
         exit(1);
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

      if (verbosity>1)
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
            

         if (obs2FileOption.getCount())
            cout << "Double difference mode: " << ddMode << endl
                 << "Minimum arc time: " << minArcTime << " seconds" << endl
                 << "Minimum arc length: " << minArcLen << " epochs" << endl
                 << "Minimum gap length: " << minArcGap << " seconds" << endl;

         cout << "--------------------------------------------------------------" << endl;
      }

      // -------------------------------------------------------------------
      // End of processing/checking command line arguments, now on to the
      // data processing portion. First we get all the data into memory.
      // -------------------------------------------------------------------

      // Get the ephemeris data
      gpstk::EphemerisStore& eph = read_eph_data(ephFileOption);
      if (nearOption.getCount())
      {
         gpstk::BCEphemerisStore& bce = dynamic_cast<gpstk::BCEphemerisStore&>(eph);
         bce.SearchNear();
      }

      gpstk::WxObsData& wod = read_met_data(metFileOption);

      // Get the obs data
      RODEpochMap rem1;
      gpstk::RinexObsHeader roh1;
      if (verbosity>1)
         cout << "Reading obs1 data." << endl;
      read_obs_data(obs1FileOption, msid, rem1, roh1, startTime, stopTime);
      if (checkObsOption.getCount())
         check_data(roh1, rem1);

      // If a msid & msc file is specified, then get & use the msc file
      // to overwrite the position in the rinex obs header
      if (msid && mscFileOption.getCount()>0)
      {
         string mscfn = (mscFileOption.getValue())[0];
         read_msc_data(mscfn, msid, roh1);
      }

      // If we are given a second set of obs data, don't compute separate ords
      // since this is a double-difference run
      if (ordMode!="none" && !obs2FileOption.getCount())
      {
         // Compute the ords
         ORDEpochMap oem1;
         computeOrds(oem1, rem1, roh1, eph, wod, svTimeOption, 
                     keepUnhealthy, ordMode);

         // Now, output statistics to stdout
         if (statsOption.getCount()==0)
            dumpStats(oem1, ordMode, sigmaMask, keepUnhealthy);

         // Save the raw ORDs to a file
         if (rawOutput)
            dumpOrds(ofs, oem1);

         RobustLinearEstimator rle;
         if (clkAnalOption.getCount())
            estimateClock(oem1, rle);

         if (rawOutput)
            dumpClock(ofs, oem1, rle);
      }

      // Now compute double difference residuals on the obs2 data
      if (obs2FileOption.getCount())
      {
         PrnElevationMap pem = elevation_map(rem1, roh1, eph);

         RODEpochMap rem2;
         rem2.clear();
         gpstk::RinexObsHeader roh2;
         if (verbosity>1)
            cout << "Reading obs data from receiver 2." << endl;
         read_obs_data(obs2FileOption, msid, rem2, roh2, startTime, stopTime);
         if (checkObsOption.getCount())
            check_data(roh2, rem2);

         // copy the position from the other file if it was taken from the msc
         if (msid && mscFileOption.getCount()>0)
            roh2.antennaPosition = roh1.antennaPosition;

            // we need the rx clock offset for this double differece
            // If we haven't been told to use the receiver clock offset in
            // the rinex data, compute the ords just to get a clock offset.
         if (ddMode != "c1p2" && !clkOption.getCount())
         {
            ORDEpochMap oem1,oem2;
            computeOrds(oem1, rem1, roh1, eph, wod, svTimeOption, keepUnhealthy, ordMode);
            computeOrds(oem2, rem2, roh2, eph, wod, svTimeOption, keepUnhealthy, ordMode);
            add_clock_to_rinex(rem1, oem1);
            add_clock_to_rinex(rem2, oem2);
         }

         if (ddMode == "sv")
         {
            DDEpochMap ddem;

            computeDDEpochMap(rem1, rem2, pem, eph, ddem);

            PhaseCleaner pc(minArcLen, minArcTime, minArcGap);

            pc.addData(rem1, rem2);
            pc.debias(pem);
            pc.getPhaseDD(ddem);

            CycleSlipList sl;
            pc.getSlips(sl, pem);
            
            if (statsOption.getCount()==0)
               dumpStats(ddem, sl, pem, keepUnhealthy);

            if (cycleSlipOption.getCount())
               dump(cout, sl);

            if (rawOutput)
               dump(ofs, ddem, pem);   
         }

         else if (ddMode=="c1p2")
         {
            DD2EpochMap ddem;
            computeDD2(rem1, rem2, ddem);
            if (statsOption.getCount()==0)
               dumpStats(ddem, pem);

            if (rawOutput)
               dump(ofs, ddem, pem);            
         }
         else
            cout << "Unknow ddMode:" << ddMode << endl;
      } // end for()
   }
   catch (gpstk::Exception& e)
   {
      cerr << "Caught Excption: " << typeid(e).name() << endl
           << "Terminating." << endl;
      exit(0);
   }
   exit(0);
}
