/// @file RinSum.cpp
/// Read Rinex observation files (version 2 or 3) and output a summary of the content.

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
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

// system
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>

// GPSTK
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "GNSSconstants.hpp"

#include "singleton.hpp"
#include "expandtilde.hpp"
#include "stl_helpers.hpp"
#include "logstream.hpp"
#include "CommandLine.hpp"

#include "CommonTime.hpp"
#include "CivilTime.hpp"
#include "Epoch.hpp"
#include "TimeString.hpp"

#include "RinexSatID.hpp"
#include "RinexObsID.hpp"
#include "Rinex3ObsStream.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"
#include "RinexUtilities.hpp"

#include "msecHandler.hpp"

//-----------------------------------------------------------------------------
using namespace std;
using namespace gpstk;
using namespace StringUtils;

//-----------------------------------------------------------------------------
string Version(string("4.1 8/26/15"));

//-----------------------------------------------------------------------------
// Object for command line input and global data
class Configuration : public Singleton<Configuration>
{

public:

      // Default and only constructor
   Configuration() throw() { SetDefaults(); }

      // Create, parse and process command line options and user input
   int ProcessUserInput(int argc, char **argv) throw();

      // Design the command line
   string BuildCommandLine(void) throw();

      // Open the output file, and parse the strings used on the command line
      // return -4 if log file could not be opened
   int ExtraProcessing(string& errors, string& extras) throw();

private:

      // Define default values
   void SetDefaults(void) throw()
   {
      defaultstartStr = string("[Beginning of dataset]");
      defaultstopStr = string("[End of dataset]");
      beginTime = CommonTime::BEGINNING_OF_TIME;
      endTime = CommonTime::END_OF_TIME;
      beginTime.setTimeSystem(TimeSystem::Any);
      endTime.setTimeSystem(TimeSystem::Any);
      userfmt = gpsfmt;
      help = verbose = brief = nohead = notab = gpstime = sorttime = vistab
         = dogaps = doms = ycode = quiet = false;
      debug = -1;
      dt = -1.0;
      vres = 0;
   }  // end Configuration::SetDefaults()

public:

      // member data
   CommandLine opts;             // command line options and syntax page
   static const string PrgmName; // program name
   string Title;                 // id line printed to screen and log

      // start command line input
   bool help, verbose, brief, nohead, notab, gpstime, sorttime, dogaps, doms,
      vistab, ycode, quiet;
   int debug, vres;
   double dt;
   string cfgfile, userfmt;

   vector<string> InputObsFiles; // RINEX obs file names
   string Obspath;               // paths
   string LogFile;               // output log file (optional)

      // times derived from --start and --stop
   string defaultstartStr,startStr;
   string defaultstopStr,stopStr;
   CommonTime beginTime,endTime;
   vector<RinexSatID> exSats;
   vector<RinexSatID> onlySats;

      // end of command line input

   vector<int> gapcount;               // for counting gaps
   string msg;
   static const string calfmt,gpsfmt,longfmt;
   ofstream logstrm;

      // for milliseconds
   msecHandler msh;

}; // end class Configuration

//-----------------------------------------------------------------------------
// const members of Configuration
const string Configuration::PrgmName = string("RinSum");
const string Configuration::calfmt = string("%04Y/%02m/%02d %02H:%02M:%02S");
const string Configuration::gpsfmt = string("%4F %w %10.3g %P");
const string Configuration::longfmt = calfmt + " = " + gpsfmt;

//-----------------------------------------------------------------------------
// struct used to store SAT/Obs table
struct TableData
{
   RinexSatID sat;
   vector<int> nobs;                   // number of data for each obs type
   vector<int> gapcount;               //
   double prevC1, prevP1, prevL1;
   CommonTime begin, end;

      // c'tor
   TableData(const SatID& p, const int& n)
   {
      sat = RinexSatID(p);
      nobs = vector<int>(n);
      prevC1 = 0;
      prevP1 = 0;
      prevL1 = 0;
   }
      // needed for find()
   inline bool operator==(const TableData& d) { return d.sat == sat; }
};

// needed for sort()
class TableSATLessThan
{
public:
   bool operator()(const TableData& d1, const TableData& d2)
   { return d1.sat < d2.sat; }
};

class TableBegLessThan
{
public:
   bool operator()(const TableData& d1, const TableData& d2)
   { return d1.begin < d2.begin; }
};

//-----------------------------------------------------------------------------
// prototypes
int Initialize(string& errors) throw(Exception);
int ProcessFiles(void) throw(Exception);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
      // get the (single) instance of the configuration
   Configuration& C(Configuration::Instance());

   try
   {
      int iret;
      clock_t totaltime(clock());
      Epoch wallclkbeg;
      wallclkbeg.setLocalTime();

         // build title = first line of output
      C.Title = "# " + C.PrgmName + ", part of the GPS Toolkit, Ver " +
         Version + ", Run " + printTime(wallclkbeg,C.calfmt);

      for(;;)
      {
            // get information from the command line
            // iret -2 -3 -4
         if((iret = C.ProcessUserInput(argc, argv)) != 0)
            break;

            // read stores, check input etc
         string errs;
         if((iret = Initialize(errs)) != 0)
         {
            LOG(ERROR) << "------- Input is not valid: ----------\n" << errs
                       << "\n------- end errors -----------";
            break;
         }
         if(!errs.empty())
            LOG(INFO) << errs;  // Warnings are here too

         ProcessFiles();
         iret = 0; // successful completion.

         break;      // mandatory
      }

      if(iret >= 0 && !C.brief && !C.quiet)
      {
            // print elapsed time
         totaltime = clock()-totaltime;
         Epoch wallclkend;
         wallclkend.setLocalTime();
         ostringstream oss;
         oss << C.PrgmName << " timing: processing " << fixed
             << setprecision(3) << double(totaltime)/double(CLOCKS_PER_SEC)
             << " sec, wallclock: " << setprecision(0)
             << (wallclkend-wallclkbeg) << " sec.";
         LOG(INFO) << oss.str();
      }

      return iret;
   }
   catch(FFStreamError& e)
   {
      cerr << "FFStreamError: " << e.what();
   }
   catch(Exception& e)
   {
      cerr << "Exception: " << e.what();
   }
   catch (...)
   {
      cerr << "Unknown exception.  Abort." << endl;
   }
   return 1;

}  // end main()

//-----------------------------------------------------------------------------
// return -5 if input is not valid
int Initialize(string& errors) throw(Exception)
{
   try
   {
      bool isValid(true);
      Configuration& C(Configuration::Instance());
      errors = string("");

         // add path to filenames, and expand tilde (~)
      include_path(C.Obspath, C.InputObsFiles);

         // sort input obs files on time
      if(C.InputObsFiles.size() > 1)
      {
         C.msg = sortRinex3ObsFiles(C.InputObsFiles);
         if(!C.msg.empty())
            LOG(ERROR) << C.msg;
      }

         // initialize millisecond handler
      if(C.doms)
         C.msh.setDT(C.dt);

         // -------- save errors and output
         //errors = oss.str();
         //stripTrailing(errors,'\n');
         //replaceAll(errors,"\n","\n# ");

      if(!isValid)
         return -5;
      return 0;
   }
   catch(Exception& e)
   {
      GPSTK_RETHROW(e);
   }
}  // end Initialize()

//-----------------------------------------------------------------------------
int Configuration::ProcessUserInput(int argc, char **argv) throw()
{
   string PrgmDesc,cmdlineUsage, cmdlineErrors, cmdlineExtras;
   vector<string> cmdlineUnrecognized;

      // build the command line
   opts.DefineUsageString(PrgmName + " [options]");
   PrgmDesc = BuildCommandLine();

      // let CommandLine parse options; write all errors, etc to the passed strings
   int iret = opts.ProcessCommandLine(argc, argv, PrgmDesc,
                                      cmdlineUsage, cmdlineErrors, cmdlineUnrecognized);

      // handle return values
   if(iret == -2)
      return iret;      // bad alloc
   if(iret == -3)
      return iret;      // invalid command line

      // help: print syntax page and quit
   if(opts.hasHelp())
   {
      LOG(INFO) << Title;
      LOG(INFO) << cmdlineUsage;
      return 1;
   }

      // extra parsing (perhaps add to cmdlineErrors, cmdlineExtras)
   iret = ExtraProcessing(cmdlineErrors, cmdlineExtras);
   if(iret == -4)
      return iret;      // log file could not be opened

      // pull out file name without --obs
   if(cmdlineUnrecognized.size() > 0)
   {
      for(int i=cmdlineUnrecognized.size()-1; i >= 0; i--)
      {
         try
         {
            string filename(cmdlineUnrecognized[i]);
            ifstream ifstrm(filename.c_str());
            if(ifstrm.is_open())
            {
               LOG(DEBUG) << "# Deduce filename >" << filename << "<";
               InputObsFiles.push_back(cmdlineUnrecognized[i]);
               cmdlineUnrecognized.erase(cmdlineUnrecognized.begin()+i);
               ifstrm.close();
               continue;
            }
            else
            {
               include_path(Obspath, filename);
               ifstrm.open(filename.c_str());
               if(ifstrm.is_open())
               {
                  LOG(DEBUG) << "# Deduce filename >" << filename << "<";
                  InputObsFiles.push_back(cmdlineUnrecognized[i]);
                  cmdlineUnrecognized.erase(cmdlineUnrecognized.begin()+i);
                  ifstrm.close();
                  continue;
               }
            }
         }
         catch(Exception& e) { ; }
      }
   }

      // output warning / error messages
   if(cmdlineUnrecognized.size() > 0)
   {
      LOG(WARNING) << "Warning - unrecognized arguments:";
      for(size_t j=0; j<cmdlineUnrecognized.size(); j++)
         LOG(WARNING) << "  " << cmdlineUnrecognized[j];
      LOG(WARNING) << "End of unrecognized arguments";
   }

      // fatal errors
   if(!cmdlineErrors.empty())
   {
      stripTrailing(cmdlineErrors,'\n');
      replaceAll(cmdlineErrors,"\n","\n ");
      LOG(ERROR) << "Errors found on command line:\n " << cmdlineErrors
                 << "\nEnd of command line errors.";
      return 1;
   }

      // success: dump configuration summary
   if(verbose)
   {
      ostringstream oss;
      oss << "------ Summary of " << PrgmName
          << " command line configuration ------\n";
      opts.DumpConfiguration(oss);
         //if(!cmdlineExtras.empty()) oss << "# Extra Processing:\n" << cmdlineExtras;
      oss << "------ End configuration summary ------";
      LOG(VERBOSE) << oss.str();
   }
   if(!cmdlineExtras.empty())
   {
      stripTrailing(cmdlineExtras,'\n');
      LOG(INFO) << cmdlineExtras;
   }

   return 0;

}  // end Configuration::CommandLine()

//-----------------------------------------------------------------------------
string Configuration::BuildCommandLine(void) throw()
{
      // Program description will appear at the top of the syntax page
   string PrgmDesc = " Program " + PrgmName + " reads one or more RINEX (v.2+) "
      + "observation files and prints a summary of content.\n Options:";
   opts.DefineUsageString("RinSum <file> [options]");

      // options to appear on the syntax page, and to be accepted on command line
      //opts.Add(char, opt, arg, repeat?, required?, &target, pre-desc, desc);
      // NB cfgfile is a dummy, but it must exist when cmdline is processed.
   opts.Add('f', "file", "fn", true, false, &cfgfile,
            "# Input via configuration file:",
            "Name of file with more options [#->EOL = comment]");

   opts.Add(0, "obs", "file", true, false, &InputObsFiles,
            "# Required input [file names may appear w/o --obs if unambiguous]",
            "Input RINEX observation file name");
   opts.Add(0, "obspath", "p", false, false, &Obspath,
            "# Paths of input files (optional):",
            "Path of input RINEX observation file(s)");

   startStr = defaultstartStr;
   stopStr = defaultstopStr;
   opts.Add(0, "start", "t[:f]", false, false, &startStr,
            "# Editing (t,f are strings: time t; format f "
            "defaults to wk,sow OR yr,mon,day,h,m,s",
            "Start processing data at this epoch");
   opts.Add(0, "stop", "t[:f]", false, false, &stopStr, "",
            "Stop processing data at this epoch");
   opts.Add(0, "exSat", "sat", true, false, &exSats, "",
            "Exclude satellite (or system) <sat> e.g. G24,R");
   opts.Add(0, "onlySat", "sat", true, false, &onlySats, "",
            "Include ONLY satellites (or systems) <sat> e.g. G,R");

   opts.Add(0, "timefmt", "fmt", false, false, &userfmt, "# Output:",
            "Format for time tags (see GPSTK::Epoch::printf) in output");
   opts.Add('b', "brief", "", false, false, &brief, "",
            "Produce a brief output");
   opts.Add(0, "nohead", "", false, false, &nohead, "",
            "Omit header from output");
   opts.Add(0, "notable", "", false, false, &notab, "",
            "Omit sat/obs table from output");
   opts.Add(0, "dt", "sec", false, false, &dt, "",
            "Nominal time step of data (sec); required only for gaps and millisec");
   opts.Add(0, "milli", "", false, false, &doms, "",
            "Find millisecond clock adjusts; req's --dt");
   opts.Add(0, "gaps", "", false, false, &dogaps, "",
            "Print a table of gaps in the data; req's --dt");
   opts.Add(0, "vis", "n", false, false, &vres, "",
            "Print graphical visibility, resolution <n> [n~20 @ 30s; req's --gaps]");
   opts.Add(0, "vtab", "", false, false, &vistab, "",
            "Print tabular visibility [req's --gaps and --vis]");

   opts.Add(0, "ycode", "", false, false, &ycode, "# Other:",
            "Assume v2.11 P mean Y");
   opts.Add(0, "verbose", "", false, false, &verbose, "",
            "Print extra output information");
   opts.Add(0, "debug", "", false, false, &debug, "",
            "Print debug output at level 0 [debug<n> for level n=1-7]");
   opts.Add(0, "help", "", false, false, &help, "",
            "Print this syntax page, and quit");
   opts.Add('q', "quiet", "", false, false, &quiet, "",
            "Make output a little quieter");

      // deprecated (old,new)
      //opts.Add_deprecated("--SP3","--eph");

   return PrgmDesc;

}  // end Configuration::BuildCommandLine()

//-----------------------------------------------------------------------------
int Configuration::ExtraProcessing(string& errors, string& extras) throw()
{
   int n;
   size_t i;
   vector<string> fld;
   ostringstream oss,ossx;       // oss for Errors, ossx for Warnings and info

      // start and stop times
   for(i=0; i<2; i++)
   {
      static const string fmtGPS("%F,%g"),fmtCAL("%Y,%m,%d,%H,%M,%S");
      msg = (i==0 ? startStr : stopStr);
      if(msg == (i==0 ? defaultstartStr : defaultstopStr))
         continue;

      bool ok(true);
      bool hasfmt(msg.find('%') != string::npos);
      n = numWords(msg,',');
      if(hasfmt)
      {
         fld = split(msg,':');
         if(fld.size() != 2)
         {
            ok = false;
         }
         else
         {
            try
            {
               Epoch ep;
               stripLeading(fld[0]," \t");
               stripLeading(fld[1]," \t");
               ep.scanf(fld[0],fld[1]);
               (i==0 ? beginTime : endTime) = static_cast<CommonTime>(ep);
            }
            catch(Exception& e)
            {
               ok = false;
               LOG(INFO) << "excep " << e.what();
            }
         }
      }
      else if(n == 2 || n == 6)
      {
            // try the defaults
         try
         {
            Epoch ep;
            ep.scanf(msg,(n==2 ? fmtGPS : fmtCAL));
            (i==0 ? beginTime : endTime) = static_cast<CommonTime>(ep);
         }
         catch(Exception& e)
         {
            ok = false;
            LOG(INFO) << "excep " << e.what();
         }
      }

      if(ok)
      {
         msg = printTime((i==0 ? beginTime : endTime),fmtGPS+" = "+fmtCAL);
         if(msg.find("Error") != string::npos) ok = false;
      }

      if(!ok)
         oss << "Error : invalid time or format in --" << (i==0 ? "start" : "stop")
             << " " << (i==0 ? startStr : stopStr) << endl;
      else
      {
         (i==0 ? beginTime : endTime).setTimeSystem(TimeSystem::Any);
         ossx << (i==0 ? "Begin time --begin" : "End time --end") << " is "
              << printTime((i==0 ? beginTime : endTime), fmtGPS+" = "+fmtCAL+"\n");
      }
   }

      // open the log file (so warnings, configuration summary, etc can go there) -----
   if(!LogFile.empty())
   {
      logstrm.open(LogFile.c_str(), ios::out);
      if(!logstrm.is_open())
      {
         LOG(ERROR) << "Error : Failed to open log file " << LogFile;
         return -4;
      }
      LOG(INFO) << "Output redirected to log file " << LogFile;
      pLOGstrm = &logstrm;
   }
   if (!quiet)
      LOG(INFO) << Title;

      // check consistency of exSat and onlySat; note you CAN have --only R --ex R10,R07
   if(exSats.size() > 0 && onlySats.size() > 0)
   {
      for(i=0; i<onlySats.size(); i++)
      {
         RinexSatID sat(onlySats[i]);
         RinexSatID sys(-1,sat.system);
         if((find(exSats.begin(), exSats.end(), sat) != exSats.end()) ||
            (find(exSats.begin(), exSats.end(), sys) != exSats.end()))
            oss << "Error : satellite " << asString(sat)
                << " found in both --exSat and --onlySat" << endl;
      }
   }

      // gaps and vis options
   if(vres < 0)
   {
      ossx << "Warning - Option --vis, must have n positive\n";
      vres = 0;
   }
   if(dt < 0.0 && dt != -1.0)
   {
      ossx << "Warning - Option --dt, must have dt positive\n";
      dt = -1.0;
   }
      // milli requires dt
   if(doms && dt == -1.0)
   {
      ossx << "Warning - Option --milli requires --dt be given\n";
      doms = false;
   }
      // gaps requires dt
   if(dogaps && dt == -1.0)
   {
      ossx << "Warning - Option --gaps requires --dt be given\n";
      dogaps = false;
   }
      // vres requires dt and gaps
   if(vres > 0 && (dt == -1.0 || !dogaps))
   {
      ossx << "Warning - Option --vis <n> requires --gaps and --dt be given\n";
      vres = 0;
   }
   if(vistab && vres == 0)
   {
      ossx << "Warning - Option --vtab requires that --vis <n> be given\n";
      vistab = false;
   }

      // add new errors to the list
   msg = oss.str();
   if(!msg.empty())
      errors += msg;
   msg = ossx.str(); //stripTrailing(msg,'\n');
   if(!msg.empty())
      extras += msg;

   return 0;

} // end Configuration::ExtraProcessing(string& errors) throw()

//-----------------------------------------------------------------------------
// Return 0 ok, >0 number of files successfully read, <0 fatal error
int ProcessFiles(void) throw(Exception)
{
   try
   {
      Configuration& C(Configuration::Instance());
      int iret,ii,k;
      size_t i,j,nfile,nfiles;
      string tag;
      CommonTime lastObsTime, prevObsTime, firstObsTime;
      RinexSatID sat;
      Rinex3ObsStream ostrm;
      ostringstream oss;
         // estimate time step
      const size_t ndtmax=15;
      double dt, bestdt[ndtmax];
      int ndt[ndtmax];
         // cache the out-of-time-order records
      bool cacheon;
      vector<CommonTime> cachetime;
      vector<vector<Rinex3ObsData> > cache;

      for(nfiles=0,nfile=0; nfile<C.InputObsFiles.size(); nfile++)
      {
         Rinex3ObsStream istrm;
         Rinex3ObsHeader Rhead, Rheadout;
         Rinex3ObsData Rdata;

            // If command line specified P1/P2 are to be considered
            // as Y-code, set the Rinex3ObsHeader flag to indicate such.
         if (C.ycode)
         {
            Rhead.PisY = true;
            Rheadout.PisY = true;
         }
      
         cacheon = false;
         cache.clear();
         cachetime.clear();

         string filename(C.InputObsFiles[nfile]);

            // iret is set to 0 ok, or could not: 1 open file, 2 read header, 3 read data
         iret = 0;
         for(i=0; i<ndtmax; i++)
            ndt[i] = -1;

            // open the file ------------------------------------------------
         istrm.open(filename.c_str(),ios::in);
         if(!istrm.is_open())
         {
            LOG(WARNING) << "Warning : could not open file " << filename;
            iret = 1;
            continue;
         }
         istrm.exceptions(ios::failbit);

            // output file name
         if(C.quiet)
         {
            std::string choppedFN(filename);
            choppedFN.erase(0,1+filename.find_last_of("/\\"));
            LOG(INFO) << "+++++++++++++ " << C.PrgmName
                      << " summary of Rinex obs file " << choppedFN
                      << " +++++++++++++";
         }
         else if(!C.brief)
         {
            LOG(INFO) << "+++++++++++++ " << C.PrgmName
                      << " summary of Rinex obs file " << filename
                      << " +++++++++++++";
         }

            // get file size
         istrm.seekg(0,ios::end);
         streampos filesize(istrm.tellg());
         istrm.seekg(0,ios::beg);

            // read the header ----------------------------------------------
         try
         {
            istrm >> Rhead;
         }
         catch(Exception& e)
         {
            LOG(WARNING) << "Warning : Failed to read header: " << e.what()
                         << "\n Header dump follows.";
            Rhead.dump(LOGstrm);
            istrm.close();
            iret = 2;
            continue;
         }
         if(Rhead.lastObs.getTimeSystem() != Rhead.firstObs.getTimeSystem())
            Rhead.lastObs.setTimeSystem(Rhead.firstObs.getTimeSystem());

            // output file name and header
         if(C.brief)
         {
            if(nfile > 0)
               LOG(INFO) << "";
            LOG(INFO) << "File name: " << filename
                      << " (RINEX ver. " << Rhead.version << ")";
            LOG(INFO) << "Marker name: " << Rhead.markerName;
            LOG(INFO) << "Antenna type: " << Rhead.antType;
            LOG(INFO) << "Position (XYZ,m) : " << fixed << setprecision(4)
                      << Rhead.antennaPosition << ".";
            LOG(INFO) << "Antenna offset (UEN,m) : " << fixed << setprecision(4)
                      << Rhead.antennaDeltaHEN << ".";
         }
         else if(!C.nohead)
         {
            LOG(DEBUG) << "RINEX header:";
            Rhead.dump(LOGstrm);
         }

         if(!Rhead.isValid())
         {
            LOG(INFO) << "Abort: header is invalid.";
            if(C.quiet)
            {
               std::string choppedFN(filename);
               choppedFN.erase(0,1+filename.find_last_of("/\\"));
               LOG(INFO) << "\n+++++++++++++ End of RinSum summary of "
                         << choppedFN << " +++++++++++++";
            }
            else if(!C.brief)
            {
               LOG(INFO) << "\n+++++++++++++ End of RinSum summary of "
                         << filename << " +++++++++++++";
            }
            continue;
         }

            // initialize counting -------------------------------------------
         int nepochs(0), ncommentblocks(0), nmaxobs(0);
         vector<TableData> table;            // table of counts per sat,obs
         map<char, vector<int> > totals;     // totals per system,obs

         prevObsTime = CommonTime::BEGINNING_OF_TIME;
         firstObsTime = CommonTime::BEGINNING_OF_TIME;

            // initialize for all systems in the header
         map<std::string,vector<RinexObsID> >::const_iterator sit;   // used below often
         for(sit=Rhead.mapObsTypes.begin(); sit != Rhead.mapObsTypes.end(); ++sit)
         {
               // Initialize the vectors contained in the map
            totals[(sit->first)[0]] = vector<int>((sit->second).size());

            LOG(DEBUG) << "GNSS " << (sit->first) << " is present with "
                       << (sit->second).size() << " observations...";

               // find the max size of obs list
            if(int((sit->second).size()) > nmaxobs)
               nmaxobs = (sit->second).size();
         }

            // initialize millisecond handler with obstypes and wavelengths
         vector<string> msots;
         if(C.doms)
         {
            vector<double> waves;
               // get obs types from header
            for(sit=Rhead.mapObsTypes.begin(); sit != Rhead.mapObsTypes.end(); ++sit)
            {
                  // get the system
               RinexSatID rsid;
               rsid.fromString(sit->first);
               SatID sid(rsid);
                  // TD support only GPS currently
               if(rsid.systemChar() != 'G') continue;
                  // excluded satellites/systems
               if(find(C.exSats.begin(), C.exSats.end(), rsid) != C.exSats.end())
                  continue;
                  // get the obstypes, prepend the system character
               for(i=0; i<sit->second.size(); i++)
               {
                  tag = sit->second[i].asString();       // 3-char obs type
                  if(tag[0] == 'C' || tag[0] == 'L')
                  {
                        // code and phase only
                     msots.push_back(string(1,rsid.systemChar())+tag);
                        // get wavelength ... NB TD Glonass frequency channel not supported
                     if(tag[0] == 'L')
                     {
                        ii = asInt(string(1,tag[1]));
                        waves.push_back(getWavelength(sid, ii));
                     }
                     else 
                        waves.push_back(0.0);
                  }
               }
            }

            C.msh.setObstypes(msots,waves);
            LOG(DEBUG) << "Initialize millisecond handler with obs type, wavelength:";
            for(i=0; i<msots.size(); i++) LOG(DEBUG) << " " << msots[i]
                                                     << fixed << setprecision(6) << " " << waves[i];
         }

         if(pLOGstrm == &cout && !C.brief)
            LOG(INFO) << "\nReading the observation data...";

            // loop over epochs ---------------------------------------------
         while(1)
         {
            try
            {
               istrm >> Rdata;
            }
            catch(Exception& e)
            {
               LOG(WARNING) << " Warning : Failed to read obs data (Exception "
                            << e.getText(0) << "); dump follows.";
               Rdata.dump(LOGstrm,Rhead);
               istrm.close();
               iret = 3;
               break;
            }
            catch(std::exception& e)
            {
               Exception ge(string("Std excep: ") + e.what());
               GPSTK_THROW(ge);
            }
            catch(...)
            {
               Exception ue("Unknown exception while reading RINEX data.");
               GPSTK_THROW(ue);
            }

               // normal EOF
            if(!istrm.good() || istrm.eof())
            {
               iret = 0;
               break;
            }

               // stay within time limits
            if(Rdata.time < C.beginTime)
            {
               LOG(DEBUG) << " RINEX data timetag " << printTime(C.beginTime,C.longfmt)
                          << " is before begin time.";
               continue;
            }
            if(Rdata.time > C.endTime)
            {
               LOG(DEBUG) << " RINEX data timetag " << printTime(C.endTime,C.longfmt)
                          << " is after end time.";
               break;
            }

               // fix time systems
            if(nepochs == 0 &&
               Rdata.time.getTimeSystem() != Rhead.lastObs.getTimeSystem())
            {
               Rhead.lastObs.setTimeSystem(Rdata.time.getTimeSystem());
               Rhead.firstObs.setTimeSystem(Rdata.time.getTimeSystem());
            }
            lastObsTime = Rdata.time;
            lastObsTime.setTimeSystem(Rhead.lastObs.getTimeSystem());
            firstObsTime.setTimeSystem(Rhead.lastObs.getTimeSystem());
            prevObsTime.setTimeSystem(Rhead.lastObs.getTimeSystem());
            if(firstObsTime == CommonTime::BEGINNING_OF_TIME)
               firstObsTime = lastObsTime;

               //LOG(INFO) << "";
            LOG(DEBUG) << " Read RINEX data: flag " << Rdata.epochFlag
                       << ", timetag " << printTime(Rdata.time,C.longfmt);

               // if aux header data, either output or skip
            if(Rdata.epochFlag > 1)
            {
               if(C.debug > -1) for(j=0; j<Rdata.auxHeader.commentList.size(); j++)
                                   LOG(DEBUG) << "Comment: " << Rdata.auxHeader.commentList[j];
               ncommentblocks++;
               continue;
            }

               // debug: dump the RINEX data object
            if(C.debug > -1)
               Rdata.dump(LOGstrm,Rhead);

               // count this epoch
            nepochs++;

               // check for data out of time order
               // use < 1.e-3 not < 0 b/c inline header info (epochFlag > 1) excluded
            if(prevObsTime != CommonTime::BEGINNING_OF_TIME
               && Rdata.time-prevObsTime < 1.e-3)
            {
                  // save it
               if(!cacheon)
               {
                     // new block
                  cachetime.push_back(prevObsTime);
                  cacheon = true;
                  vector<Rinex3ObsData> v;
                  cache.push_back(v);
               }
               cache[cache.size()-1].push_back(Rdata);
               continue;
            }
            cacheon = false;

               // look for gaps in the timetags
            int ncount;
            if(C.dt > 0.0)
            {
               ncount = int(0.5+(lastObsTime-firstObsTime)/C.dt);
                  // update gap count
               if(C.gapcount.size() == 0)
               {
                     // create the list
                  C.gapcount.push_back(ncount);   // start time
                  C.gapcount.push_back(ncount-1); // end time
               }
               i = C.gapcount.size() - 1;
               if(ncount == C.gapcount[i] + 1)    // no gap
                  C.gapcount[i] = ncount;
               else
               {
                     // found a gap
                  C.gapcount.push_back(ncount);   // start time
                  C.gapcount.push_back(ncount);   // end time
               }

                  // TD test after 50 epochs - wrong dt is disasterous
            }

               // loop over satellites -------------------------------------
            Rinex3ObsData::DataMap::const_iterator it;
            for(it=Rdata.obs.begin(); it != Rdata.obs.end(); ++it)
            {
               const RinexSatID& sat(it->first);

                  // is sat included?
               if(C.onlySats.size() > 0 &&
                  find(C.onlySats.begin(), C.onlySats.end(), sat) == C.onlySats.end()
                  && find(C.onlySats.begin(), C.onlySats.end(),
                          RinexSatID(-1,sat.system)) == C.onlySats.end())
                  continue;

                  // is sat excluded?
               if(find(C.exSats.begin(), C.exSats.end(), sat) != C.exSats.end())
                  continue;
                  // check for all sats of this system
               else if(find(C.exSats.begin(), C.exSats.end(),
                            RinexSatID(-1,sat.system)) != C.exSats.end())
                  continue;

               const vector<RinexDatum>& vecData(it->second);

                  // find this sat in the table; add it if necessary
               vector<TableData>::iterator ptab;
               ptab = find(table.begin(),table.end(),TableData(sat,nmaxobs));
               if(ptab == table.end())
               {
                     // add it
                  table.push_back(TableData(sat,nmaxobs));
                  ptab = find(table.begin(),table.end(),TableData(sat,nmaxobs));
                  ptab->begin = lastObsTime;
                  if(C.dt > 0.0)
                  {
                     ptab->gapcount.push_back(ncount);      // start time
                     ptab->gapcount.push_back(ncount-1);    // end time
                  }
               }

                  // update list of gap times
               if(C.dt > 0.0)
               {
                  i = ptab->gapcount.size() - 1;         // index of curr end time
                  if(ncount == ptab->gapcount[i] + 1)    // no gap
                     ptab->gapcount[i] = ncount;
                  else
                  {
                        // found a gap
                     ptab->gapcount.push_back(ncount);   // start time
                     ptab->gapcount.push_back(ncount);   // end time
                  }
               }

                  // set the end time for this satellite to the current epoch
               ptab->end = lastObsTime;
               if(C.debug > -1)
               {
                  oss.str("");
                  oss << "Sat " << setw(2) << sat;
               }

                  // first, find the current system...
               char sysCode = sat.systemChar();
               string sysStr(string(1,sysCode));

                  // update Obs data totals
               for(size_t index=0; index != vecData.size(); index++)
               {
                  if(C.debug > -1)
                     oss << " (" << index << ")";

                     // if this observation is not zero, update it's total count
                  if(vecData[index].data != 0)
                  {
                     (ptab->nobs)[index]++;                 // per obs
                     if(totals[sysCode].size() == 0)
                        totals[sysCode] = vector<int>(vecData.size());
                     totals[sysCode][index]++;              // per system
                  }

                     // if looking for milliseconds, update handler
                  if(C.doms && vecData[index].data != 0)
                  {
                     tag = sysStr + Rhead.mapObsTypes[sysStr][index].asString();
                     if(vectorindex(msots,tag) != -1)
                     {
                        C.msh.add(lastObsTime, sat, tag, vecData[index].data);
                     }
                  }

                  if(C.debug > -1)
                     oss << fixed << setprecision(3)
                         << " " << asString(Rhead.mapObsTypes[sysStr][index])
                         << " " << setw(13) << vecData[index].data
                         << " " << vecData[index].lli
                         << " " << vecData[index].ssi;

               } // end loop over observations

               if(C.debug > -1)
                  LOG(DEBUG) << oss.str();

            }  // end loop over satellites

            if(prevObsTime != CommonTime::BEGINNING_OF_TIME)
            {
               dt = lastObsTime-prevObsTime;
               if(dt > 0.0)
               {
                  for(i=0; i<ndtmax; i++)
                  {
                     if(ndt[i] <= 0)
                     {
                        bestdt[i]=dt;
                        ndt[i]=1;
                        break;
                     }
                     if(fabs(dt-bestdt[i]) < 0.0001)
                     {
                        ndt[i]++;
                        break;
                     }
                     if(i == ndtmax-1)
                     {
                        k = 0;
                        int nleast = ndt[k];
                        for(j=1; j<ndtmax; j++)
                        {
                           if(ndt[j] <= nleast)
                           {
                              k = j;
                              nleast = ndt[j];
                           }
                        }
                        ndt[k] = 1;
                        bestdt[k] = dt;
                     }
                  }
               }
               else if(dt == 0)
               {
                  LOG(WARNING) << "Warning - repeated time tag at "
                               << printTime(lastObsTime,C.longfmt);
               }
               else
               {
                  LOG(WARNING) << "Warning - time tags out of order: "
                               << printTime(prevObsTime,C.longfmt) << " > "
                               << printTime(lastObsTime,C.longfmt);
                     //<< " " << scientific << setprecision(4) << dt;
               }
            }
            prevObsTime = lastObsTime;

         }  // end while loop over epochs

         istrm.close();

            // check that we found some data
         if(nepochs <= 0)
         {
            LOG(INFO) << "File " << filename
                      << " : no data found. Are time limits wrong?";
            continue;
         }

            // Compute interval -------------------------------------------------
         for(i=1,j=0; i < ndtmax; i++)
         {
            if(ndt[i] > ndt[j])
               j = i;
            dt = bestdt[j];
         }

            // Summary info -----------------------------------------------------
         LOG(INFO) << "Computed interval " << fixed << setw(5) << setprecision(2)
                   << dt << " seconds.";
         LOG(INFO) << "Computed first epoch: " << printTime(firstObsTime,C.longfmt);
         LOG(INFO) << "Computed last  epoch: " << printTime(lastObsTime,C.longfmt);
      
            // compute time span of dataset in days/hours/minutes/seconds
         oss.str("");
         oss << "Computed time span: ";
         double secs = lastObsTime - firstObsTime;
         int remainder = int(secs);
         CivilTime delta(firstObsTime);
         delta.day    = remainder / 86400; remainder %= 86400;
         delta.hour   = remainder / 3600;  remainder %= 3600;
         delta.minute = remainder / 60;    remainder %= 60;
         delta.second = remainder;
         if(delta.day > 0)
            oss << delta.day << "d ";

         LOG(INFO) << oss.str() << delta.hour << "h " << delta.minute << "m "
                   << delta.second << "s = " << secs << " seconds.";

         LOG(INFO) << "Computed file size: " << filesize << " bytes.";

            // Reusing secs, as it is equivalent to the original expression
            // i = 1+int(0.5+(lastObsTime-firstObsTime)/dt);
         i = 1+int(0.5 + secs / dt);

         LOG(INFO) << "There were " << nepochs << " epochs ("
                   << fixed << setprecision(2) << double(nepochs*100)/i
                   << "% of " << i << " possible epochs in this timespan) and "
                   << ncommentblocks << " inline header blocks.";

            // Sort table
         if(C.sorttime)
            sort(table.begin(),table.end(),TableBegLessThan());
         else
            sort(table.begin(),table.end(),TableSATLessThan());

            // output table
            // header
         vector<TableData>::iterator tabIt;
         if(table.size() > 0)
            table.begin()->sat.setfill('0');

         if(!C.brief && !C.notab)
         {
               // non-brief output ------------
            LOG(INFO) << "\n      Summary of data available in this file: "
                      << "(Spans are based on times and interval)";
            string fmt(C.gpstime ? C.gpsfmt : C.calfmt);
            j = 0;
            for(sit=Rhead.mapObsTypes.begin(); sit != Rhead.mapObsTypes.end(); ++sit)
            {
               RinexSatID sat(sit->first);

               map<char, vector<int> >::const_iterator totalsIter;
                  // compute grand total first
               totalsIter = totals.find((sit->first)[0]);
               const vector<int>& vec = totalsIter->second;
               for(i=0,k=0; k<vec.size(); k++) i += vec[k];
               if(i == 0)
                  continue;

                  // print the table
               if(++j > 1)
                  LOG(INFO) << "";
               LOG(INFO) << "System " << sit->first <<" = "<< sat.systemString() << ":";
               oss.str("");
               oss << " Sat\\OT:";

                  // print line of RINEX 3 codes
               for(k=0; k < (sit->second).size(); k++)
                     //oss << setw(k==0?4:7) << asString((sit->second)[k]);
                  oss << setw(k==0?4:7) << (sit->second)[k].asString();
               LOG(INFO) << oss.str() << "   Span             Begin time - End time";

                  // print the table
               for(tabIt = table.begin(); tabIt != table.end(); ++tabIt)
               {
                  std::string sysChar;
                  sysChar += (tabIt->sat).systemChar();
                  if((sit->first) == sysChar)
                  {
                     oss.str("");
                     oss << " " << tabIt->sat << " ";
                     size_t obsSize = (Rhead.mapObsTypes.find(sysChar)->second).size();
                     for(k = 0; k < obsSize; k++)
                        oss << setw(7) << tabIt->nobs[k];

                     oss << setw(7) << 1+int(0.5+(tabIt->end-tabIt->begin)/dt);

                     LOG(INFO) << oss.str() << "  " << printTime(tabIt->begin,fmt)
                               << " - " << printTime(tabIt->end,fmt);
                  }
               }

               oss.str("");
               oss << "TOTAL";
               for(k=0; k<vec.size(); k++) oss << setw(7) << vec[k];
               LOG(INFO) << oss.str();
            }
            LOG(INFO) << "";
         }
         else
         {
               // brief output ---------------
               // output satellites
            oss.str(""); oss << "SATs(" << table.size() << "):";
            i = 0;
            for(tabIt = table.begin(); tabIt != table.end(); ++tabIt)
            {
               oss << " " << tabIt->sat;
               if((++i % 20) == 0)
               {
                  LOG(INFO) << oss.str();
                  oss.str(""); i=0;
                  oss << "SATs ...:";
               }
            }
            LOG(INFO) << oss.str();

               // output obs types
            sit = Rhead.mapObsTypes.begin();
            for( ; sit != Rhead.mapObsTypes.end(); ++sit)
            {
               string sysCode = (sit->first);
               vector<RinexObsID>& vec = Rhead.mapObsTypes[sysCode];

                  // is this system found in the list of sats?
               map<char, vector<int> >::const_iterator totalsIter;
               totalsIter = totals.find(sysCode[0]);
               const vector<int>& vectot = totalsIter->second;
               for(i=0,k=0; k<vectot.size(); k++) i += vectot[k];
               if(i == 0)
                  continue;    // no, skip it

               oss.str("");
               oss << "System " << RinexSatID(sysCode).systemString3()
                   << " Obs types(" << vec.size() << "): ";

               for(i=0; i<vec.size(); i++) oss << " " << vec[i].asString();

                  // if RINEX ver. 2, then add ver 2 obstypes in parentheses
                  //map<string, map<string, RinexObsID> > Rinex3ObsHeader::mapSysR2toR3ObsID
                  //Rhead.mapSysR2toR3ObsID[sys][ot2] = OT3;
               if(Rhead.version < 3)
               {
                  oss << " [v2:";
                  for(i=0; i<vec.size(); i++)
                  {
                     map<string,RinexObsID>::iterator it;
                     for(it = Rhead.mapSysR2toR3ObsID[sysCode].begin();
                         it != Rhead.mapSysR2toR3ObsID[sysCode].end(); ++it)
                     {
                        if(it->second == vec[i])
                        {
                           oss << " " << it->first;
                           break;
                        }
                     }
                  }
                  oss << "]";
               }

               LOG(INFO) << oss.str();
            }
         }

            // gaps
         if(C.dogaps)
         {
               // summary of gaps using count
            oss.str("");
            oss << "Summary of gaps (vs count) in the data in this file, "
                << "assuming dt = " << C.dt << " sec.\n";
            if(C.dt != dt)
               oss << " Warning - computed dt does not match input dt\n";
            oss << " First epoch = " << printTime(firstObsTime,C.longfmt)
                << " and last epoch = " << printTime(lastObsTime,C.longfmt) << endl;
            oss << "    Sat    beg - end (count,size) ... "
                << "[count = # of dt's from first epoch]\n";
               // print for timetags = all sats
            k = C.gapcount.size()-1;               // size() is at least 2
            oss << "GAP ALL " << setw(5) << C.gapcount[0]
                << " - " << setw(5) << C.gapcount[k];

               // NB DO NOT make ii size_t
            for(ii=1; ii<=k-2; ii+=2)
               oss << " (" << C.gapcount[ii]+1                          // begin of gap
                   << "," << C.gapcount[ii+1]-C.gapcount[ii]-1 << ")";   // size
            oss << endl;

               // loop over sats
            for(tabIt = table.begin(); tabIt != table.end(); ++tabIt)
            {
               k = tabIt->gapcount.size() - 1;
               oss << "GAP " << tabIt->sat << " " << setw(5) << tabIt->gapcount[0]
                   << " - " << setw(5) << tabIt->gapcount[k];
                  // NB DO NOT make ii size_t
               for(ii=1; ii<=k-2; ii+=2)
                  oss << " (" << tabIt->gapcount[ii]+1 << ","      // begin count of gap
                      << tabIt->gapcount[ii+1]-tabIt->gapcount[ii]-1 << ")";   // size
               oss << endl;
            }

            tag = oss.str(); stripTrailing(tag,"\n");
            LOG(INFO) << tag;

               // summary of gaps using sow
            oss.str("");
            double t(static_cast<GPSWeekSecond>(firstObsTime).sow), d(C.dt);
            oss << "\nSummary of gaps (vs SOW) in the data in this file, assuming dt = "
                << C.dt << " sec.\n";
            if(C.dt != dt)
               oss << " Warning - computed dt does not match input dt\n";
            oss << " First epoch = " << printTime(firstObsTime,C.longfmt)
                << " and last epoch = " << printTime(lastObsTime,C.longfmt) << endl;
            oss << "    Sat      beg -      end (sow,number of missing points)\n";

               // print for timetags = all sats
            k = C.gapcount.size()-1;               // size() is at least 2
            oss << "GAP ALL " << fixed << setprecision(1) << setw(8) << t+d*C.gapcount[0]
                << " - " << setw(8) << t+d*C.gapcount[k];
               // NB DO NOT make ii size_t
            for(ii=1; ii<=k-2; ii+=2)
               oss << " (" << t+d*(C.gapcount[ii]+1)                    // begin of gap
                   << "," << C.gapcount[ii+1]-C.gapcount[ii]-1 << ")";   // size
            oss << endl;

               // loop over sats
            for(tabIt = table.begin(); tabIt != table.end(); ++tabIt)
            {
               k = tabIt->gapcount.size() - 1;
               oss << "GAP " << tabIt->sat << " " << fixed << setprecision(1)
                   << setw(8) << t+d*tabIt->gapcount[0]
                   << " - " << setw(8) << t+d*tabIt->gapcount[k];
                  // NB DO NOT make ii size_t
               for(ii=1; ii<=k-2; ii+=2)
                  oss << " (" << t+d*(tabIt->gapcount[ii]+1) << ","  // begin sow of gap
                      << tabIt->gapcount[ii+1]-tabIt->gapcount[ii]-1 << ")";   // size
               oss << endl;
            }

            tag = oss.str(); stripTrailing(tag,"\n");
            LOG(INFO) << tag;

               // visibility
            if(C.vres > 0)
            {
                  // print visibility graphically, resolution C.vres = counts/character
               double dn(static_cast<double>(C.vres));
               oss.str("");
               oss << "\nVisibility - resolution is " << dn << " epochs = " << dn*C.dt
                   << " seconds.\n";
               oss << " First epoch = " << printTime(firstObsTime,C.longfmt)
                   << " and last epoch = " << printTime(lastObsTime,C.longfmt) << endl;
               oss << "VIS ALL ";
               bool isOn(false);
               for(k=0,i=0; i<C.gapcount.size()-1; i+=2)
               {
                  ii = int(double(C.gapcount[i]/dn));
                  if(ii-k > 0)
                  {
                     oss << string(ii-k,' ');
                     k = ii;
                     isOn = false;
                  }
                  ii = int(double(C.gapcount[i+1]/dn));
                  if(ii-k > 0)
                  {
                     if(isOn)
                     {
                        oss << "x";
                        ii--;
                     }
                     oss << string(ii-k,'X');
                     k = ii;
                     isOn = true;
                  }
               }
               LOG(INFO) << oss.str();

                  // timetable of visibility, resolution dn epochs
                  // to get resolution = 1 epoch, remove isOn, kk and //RES=1
               multimap<int,string> vtab;

                  // loop over sats
                  //ostringstream ossvt;
               for(tabIt = table.begin(); tabIt != table.end(); ++tabIt)
               {
                  oss.str("");
                  oss << "VIS " << tabIt->sat << " ";

                  isOn = false;
                  bool first(true);
                  int jj,kk(static_cast<int>(tabIt->gapcount[0]/dn)); // + 0.5);
                  for(k=0,i=0; i<tabIt->gapcount.size()-1; i+=2)
                  {
                        // satellite 'off'
                     j = int(double(tabIt->gapcount[i]/dn));
                     if(!first)
                     {
                        vtab.insert(multimap<int, string>::value_type(
                                       kk, string("-")+asString(tabIt->sat)));
                        kk = j;
                     }
                     first = false;
                     jj = j-k;
                     if(jj > 0)
                     {
                        isOn = false;
                        oss << string(jj,' ');
                        k = j;
                     }
                        // satellite 'on'
                     j = int(double(tabIt->gapcount[i+1]/dn));
                     vtab.insert(multimap<int, string>::value_type(
                                    kk, string("+")+asString(tabIt->sat)));
                     kk = j;
                     jj = j-k;
                     if(jj > 0)
                     {
                        if(!isOn)
                        {
                           isOn = true;
                        }
                        else
                        {
                           oss << "x";
                           jj--;
                        }
                        oss << string(jj,'X');
                        k = j;
                     }
                  }
                  vtab.insert(multimap<int, string>::value_type(
                                 kk, string("-")+asString(tabIt->sat)));
                  LOG(INFO) << oss.str();
               }

               if(C.vistab)
               {
                  LOG(INFO) << "\n Visibility Timetable - resolution is "
                            << dn << " epochs = " << dn*C.dt << " seconds.\n"
                            << " First epoch = " << printTime(firstObsTime,C.longfmt)
                            << " and last epoch = " << printTime(lastObsTime,C.longfmt) << "\n"
                            << "     YYYY/MM/DD HH:MM:SS = week d secs-of-wk Xtot count  nX  "
                            << "seconds nsats visible satellites";
                  j = k = 0;
                  CommonTime ttag(firstObsTime);
                  vector<string> sats;
                  multimap<int,string>::const_iterator vt;
                  vt = vtab.begin();
                  while(vt != vtab.end())
                  {
                     while(vt != vtab.end() && vt->first == k)
                     {
                        string str(vt->second);
                        if(str[0] == '+')
                        {
                              //LOG(INFO) << "Add " << str.substr(1);
                           sats.push_back(str.substr(1));
                        }
                        else
                        {
                           vector<string>::iterator fsat;
                           fsat = find(sats.begin(),sats.end(),str.substr(1));
                           if(fsat != sats.end())
                           {
                              sats.erase(fsat);
                           }
                        }
                        ++vt;
                     }

                     ttag += (k-j)*C.dt*dn;

                     if(vt == vtab.end())
                        break;

                     sort(sats.begin(),sats.end());

                     oss.str("");
                     oss << "VTAB " << setw(4) << printTime(ttag,C.longfmt)
                         << " " << setw(4) << k
                         << " " << setw(5) << k*C.vres
                         << " " << setw(3) << vt->first - k
                         << fixed << setprecision(1)
                         << " " << setw(8) << (vt->first-k)*C.dt*dn
                         << " " << setw(5) << sats.size();
                     for(i=0; i<sats.size(); i++) oss << " " << sats[i];
                     LOG(INFO) << oss.str();

                     j = k;
                     k = vt->first;
                  }
                  LOG(INFO) << "VTAB " << setw(4) << printTime(ttag,C.longfmt)
                            << " " << setw(4) << k
                            << " " << setw(5) << int(0.5+(ttag-firstObsTime)/C.dt)
                            << " END";
               }

            }  // end if C.vres > 0 (user chose vis output)
         }

            // output milliseconds
         if(C.doms)
         {
            C.msh.afterAddbeforeFix();

               // true b/c no fixing, but false b/c editing commands follow
            LOG(INFO) << C.msh.getFindMessage(false);

            vector<string> cmds = C.msh.getEditCommands();
            for(i=0; i<cmds.size(); i++)
               LOG(INFO) << cmds[i] << " # edit cmd for millisecond clock adjust";
            LOG(INFO) << "";
         }

            // Warnings ------------------------------------------------------------
            // there were records out of time order
         if(cache.size() > 0)
         {
            for(i=0; i<cache.size(); i++)
               LOG(INFO) << " Warning: " << setw(4) << cache[i].size()
                         << " data records following epoch "
                         << printTime(cachetime[i],C.calfmt) << " are out of time order,"
                         << "\n         with epochs " << printTime(cache[i][0].time,C.calfmt)
                         << " to " << printTime(cache[i][cache[i].size()-1].time,C.calfmt)
                         << endl;
         }

         if((Rhead.valid & Rinex3ObsHeader::validInterval)
            && fabs(dt-Rhead.interval) > 1.e-3)
            LOG(INFO) << " Warning - Computed interval is " << setprecision(2)
                      << dt << " sec, while input header has " << setprecision(2)
                      << Rhead.interval << " sec.";

         if(C.beginTime == CommonTime::BEGINNING_OF_TIME
            && fabs(firstObsTime-Rhead.firstObs) > 1.e-8)
            LOG(INFO) << " Warning - Computed first time does not agree with header";

         if(C.endTime == CommonTime::END_OF_TIME
            && (Rhead.valid & Rinex3ObsHeader::validLastTime)
            && fabs(lastObsTime-Rhead.lastObs) > 1.e-8)
            LOG(INFO) << " Warning - Computed last time does not agree with header";

            // look for empty systems
         for(sit=Rhead.mapObsTypes.begin(); sit != Rhead.mapObsTypes.end(); ++sit)
         {
            map<char,vector<int> >::const_iterator totIt(totals.find(sit->first[0]));
            const vector<int>& vec(totIt->second);
            for(i=0,k=0; k<vec.size(); k++)
               i += vec[k];
            if(i == 0)
            {
               RinexSatID sat(sit->first);
               if( (find(C.exSats.begin(), C.exSats.end(),
                         RinexSatID(-1,sat.system)) == C.exSats.end()) // sys not excluded
                   &&
                   (C.onlySats.size() > 0 &&
                    find(C.onlySats.begin(), C.onlySats.end(), // only system
                         RinexSatID(-1,sat.system)) != C.onlySats.end()) )
                  LOG(INFO) << " Warning - System " << sit->first << " = "
                            << sat.systemString() << " should be removed from the header.";
            }
         }
      
            // look for obs types that are completely empty
            // sit declared above map<std::string,vector<RinexObsID> >::const_iterator sit;
         for(sit=Rhead.mapObsTypes.begin(); sit != Rhead.mapObsTypes.end(); ++sit)
         {
               // loop over obs types in header - systems first
            RinexSatID sat(sit->first);
            map<char, vector<int> >::const_iterator totalsIter;
            totalsIter = totals.find((sit->first)[0]);

               // this vector is printed after "TOTAL" above
            const vector<int>& totvec = totalsIter->second;

               // compute grand total first - skip if this system has no data at all
            for(i=0,k=0; k<totvec.size(); k++) i += totvec[k];
            if(i == 0)
               continue;

            for(k=0; k<totvec.size(); k++)
            {
               if(totvec[k] == 0)
               {
                  tag = string();
                  if(Rhead.version < 3)
                  {
                     map<string,RinexObsID>::iterator it;
                     for(it = Rhead.mapSysR2toR3ObsID[sit->first].begin();
                         it != Rhead.mapSysR2toR3ObsID[sit->first].end(); ++it)
                     {
                        if(it->second == sit->second[k])
                        {
                           tag = string(", ") + it->first + string(" in ver.2");
                           break;
                        }
                     }
                  }
                  LOG(INFO) << " Warning - Obs type "
                            << sit->first << asString((sit->second)[k])
                            << " (" << sat.systemString()
                            << " " << asString((sit->second)[k]) << tag
                            << ") should be removed from header";
               }
            }
         }
         
            // failure due to critical error
         if(iret < 0)
            break;

         if(iret == 0)
            nfiles++;

      }  // end loop over files

      if(iret < 0)
         return iret;

      return nfiles;
   }
   catch(Exception& e)
   {
      GPSTK_RETHROW(e);
   }
}  // end ProcessFiles()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
