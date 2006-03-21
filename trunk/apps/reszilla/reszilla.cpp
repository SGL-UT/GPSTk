#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/reszilla.cpp#13 $"
//
// Compute observed range deviations (ORDs) and double
// difference residuals from rinex obs files.
//

#include <fstream>
#include <BCEphemerisStore.hpp>

#include "readers.hpp"
#include "util.hpp"
#include "ordUtils.hpp"
#include "PhaseCleaner.hpp"

using namespace std;

int verbosity;
string timeFormat;
ElevationRangeList elr;

int main(int argc, char *argv[])
{
   string hmsFmt="%Y %3j %02H:%02M:%02S";
   string sodFmt="%Y %3j %7.1s";
   string ordMode="c1p2";
   string clkMode="epoch";
   string ddMode="sv";
   unsigned long msid=0;
   double sigmaMask=6;

   timeFormat = hmsFmt;
   verbosity = 1;

   try
   {
      gpstk::CommandOptionNoArg
         nearOption('n', "search-near", "Use BCEphemeris.searchNear()"),
         helpOption('h', "help", "Print usage. Repeat for more info. "),
         clkOption('c', "clock-from-rinex", "Use the receiver clock offset from the rinex obs data."),
         svTimeOption('\0', "svtime", "Observation data is in SV time frame. The default is RX time frame."),
         checkObsOption('\0',"check-obs", "Report data rate, order of data, data present, data gaps"),
         statsOption('s', "no-stats", "Don't compute & output the statistics"),
         cycleSlipOption('\0', "cycle-slips", "Output a list of cycle slips");
   
      gpstk::CommandOptionWithAnyArg
         rawOutputOption('r', "raw-output", "Dump the computed residuals/ords into specified file. If '-' is given as the file name, output is sent to stdout. The default is to no otput the raw residuals."),
         timeFmtOption('t', "timeFormat", "Daytime format specifier used for the timestamps in the raw output. The default is \"" + timeFormat + "\". If this option is specified with the format as \"s\", the format \"" + sodFmt + "\" is used."),
         ordModeOption('\0', "omode", "ORD mode: P1P2, C1P2, C1, P1, P2. The default is " + ordMode),
         clkModeOption('\0', "cmode", "Clock computation mode: epoch, linear. The default is " + clkMode + "."),
         ddModeOption('\0', "ddmode", "Double difference residual mode: none, sv, or c1p2. The default is " + ddMode + "."),
         mscFileOption('\0', "msc", "Station coordinate file"),
         ephFileOption('e', "ephemeris", "Ephemeris data file name (either broadcast in RINEX nav, broadcast in FIC, or precise in SP3)", false),
         obs1FileOption('o', "obs1", "Observation data file name. If this option is specified more than once the contents of all files will be used.", true),
         obs2FileOption('2', "obs2", " Second receiver's observation data file name. Only used when computing a double difference. If this option is specified more than once the contents of all the files will be used."),
         metFileOption('w', "weather", "Weather data file name (RINEX met format only)"),
         elevBinsOption('b', "elev-bin", "A range of elevations, used in computing the statistical summaries. Repeat to specify multiple bins. The default is \"-b 0-10 -b 10-20 -b 20-60 -b 10-90\".");

      gpstk::CommandOptionWithNumberArg
         msidOption('m', "msid", "Station to process data for. Used to select a station from smoothed data files."),
         sigmaOption('\0', "sigma", "Multiplier for sigma stripping used in computation of statistics on the raw residuals. The default is " + gpstk::StringUtils::asString((int)sigmaMask) + "."),
         verbosityOption('v', "verbosity", "How much detail to provide  about intermediate steps. The default is 1. Specify -hh for more info.");

      string appDesc("Computes various residuals from GPS observations.");
      gpstk::CommandOptionParser cop(appDesc);
      cop.parseOptions(argc, argv);

      if (helpOption.getCount() || cop.hasErrors())
      {
         if (cop.hasErrors() && helpOption.getCount()==0)
         {
            cop.dumpErrors(cout);
            cout << "use -h for help" << endl;
         }
         else
         {
            cop.displayUsage(cout);
            if (helpOption.getCount() > 1)
               cout << endl
                    << "Verbosity values:" << endl
                    << "  0: nothing but the results" << endl
                    << "  1: Output status before potentially time consuming operations (default)" << endl
                    << "  2: more details about each step and the options chosen" << endl
                    << "  3: add the reasons for editing data" << endl
                    << "  4: dump intermediate values for each epoch" << endl
                    << endl
                    << "The -ddmode=slipper is a test mode version of the sv double difference with a more" << endl
                    << "complex debiasing algorithm. The intent of this mode is to do a better job at detecting" << endl
                    << "real cycle slips as opposed to single point outliers." << endl
                    << endl
                    << "Example command to compute ORDs on an ICD-GPS-211 formated smoothed" << endl
                    << "measurement data file:" << endl
                    << "  reszilla --omode=p1 --svtime --msc=mscoords.cfg -m 85401 -o asm2004.138 -e s011138a.04n" << endl
                    << endl;
         }
         exit(0);
      }

      if (verbosityOption.getCount())
         verbosity = gpstk::StringUtils::asInt(verbosityOption.getValue()[0]);

      if (ddModeOption.getCount())
         ddMode = gpstk::StringUtils::lowerCase(ddModeOption.getValue().front());
      
      if (ordModeOption.getCount())
         ordMode=gpstk::StringUtils::lowerCase(ordModeOption.getValue()[0]);
      
      bool svTime = svTimeOption.getCount()>0;

      if (clkModeOption.getCount())
         clkMode=gpstk::StringUtils::lowerCase(clkModeOption.getValue().front());

      if (msidOption.getCount())
         msid=gpstk::StringUtils::asUnsigned(msidOption.getValue().front());

      if (sigmaOption.getCount())
         sigmaMask=gpstk::StringUtils::asDouble(sigmaOption.getValue().front());

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
            float minElev = gpstk::StringUtils::asFloat(pr);
            gpstk::StringUtils::stripFirstWord(pr, '-');
            float maxElev = gpstk::StringUtils::asFloat(pr);
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
         cout << "Double difference mode: " << ddMode << endl
              << "Format to use for time in raw output: " << timeFormat << endl
              << "Data time tag: " << (svTime?"sv":"rx") << endl
              << "Clock computation mode: " << clkMode << endl
              << "Double difference computation mode: " << ddMode << endl
              << "Sigma stripping multiplier: " << sigmaMask << endl
              << "Verbosity: " << verbosity << endl;
         cout << "Elevation bins: ";
         for (ElevationRangeList::const_iterator i=elr.begin(); i!= elr.end(); i++)
            cout << i->first << "-" << i->second << " ";
         cout << endl;
         if (msid)
         {
            cout << "msid: " << msid << endl;
            if (mscFileOption.getCount()>0)
               cout << "msc file: " << mscFileOption.getValue().front() << endl;
         }
         if (rawOutputOption.getCount())
            cout << "Raw output file: " << outputFileName << endl;
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
      read_obs_data(obs1FileOption, msid, rem1, roh1);
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
         compute_ords(oem1, rem1, roh1, eph, wod, 
                      svTimeOption, ordMode, clkMode);

         // Now, output statistics to stdout
         if (statsOption.getCount()==0)
            dumpStats(oem1, ordMode, sigmaMask);
         
         // Save the raw ORDs to a file
         if (rawOutputOption.getCount())
            dump(ofs, oem1);
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
         read_obs_data(obs2FileOption, msid, rem2, roh2);

         // copy the position from the other file if it was taken from the msc
         if (msid && mscFileOption.getCount()>0)
            roh2.antennaPosition = roh1.antennaPosition;

            // we need the rx clock offset for this double differece
            // If we haven't been told to use the receiver clock offset in
            // the rinex data, compute the ords just to get a clock offset.
         if (ddMode != "c1p2" && !clkOption.getCount())
         {
            ORDEpochMap oem1,oem2;
            compute_ords(oem1, rem1, roh1, eph, wod, svTimeOption, ordMode, clkMode);
            compute_ords(oem2, rem2, roh2, eph, wod, svTimeOption, ordMode, clkMode);
            add_clock_to_rinex(rem1, oem1);
            add_clock_to_rinex(rem2, oem2);
         }

         if (ddMode == "sv")
         {
            DDEpochMap ddem;

            computeDDEpochMap(rem1, rem2, pem, ddem);
            
            PhaseCleaner pc;

            pc.addData(rem1, rem2, ddem);
            pc.debias();
            pc.getPhaseDD(ddem);

            CycleSlipList sl;
            pc.getSlips(sl);
            
            if (statsOption.getCount()==0)
               dumpStats(ddem, sl, pem);

            if (cycleSlipOption.getCount())
               dump(cout, sl);

            if (rawOutputOption.getCount())
               dump(ofs, ddem, pem);   
         }

         else if (ddMode=="c1p2")
         {
            DD2EpochMap ddem;
            computeDD2(rem1, rem2, ddem);
            if (statsOption.getCount()==0)
               dumpStats(ddem, pem);

            if (rawOutputOption.getCount())
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
