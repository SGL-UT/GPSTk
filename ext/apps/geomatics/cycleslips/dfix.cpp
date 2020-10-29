//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/// @file dfix.cpp Discontinuity detection and correction (cycleslip fixer) using
///                  class gdc.

//------------------------------------------------------------------------------------
// system includes
#include <ctime>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>
// GPSTk
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "Epoch.hpp"
#include "RinexSatID.hpp"
#include "Position.hpp"
#include "RinexUtilities.hpp"
#include "EphemerisRange.hpp"
#include "singleton.hpp"
#include "GNSSconstants.hpp"
#include "msecHandler.hpp"
#include "stl_helpers.hpp"
// geomatics
#include "expandtilde.hpp"
#include "logstream.hpp"
#include "SatPass.hpp"
#include "SatPassUtilities.hpp"
#include "Rinex3ObsFileLoader.hpp"
// dfix
#include "CommandLine.hpp"
#include "gdc.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// prototypes
/// Read and process command line
/// @throw Exception
int GetCommandLine(int argc, char **argv);

/// Validate input -- check that files exist, append path, quit if user chose validate
/// @throw Exception
int ValidateInput(void);

/// Load ephemeris stores
/// @throw Exception
int Initialize(void);

/// Choose R3ObsIDs given user input, read RINEX file(s) into SatPass list
/// @return 0 success, <0 error code
/// @throw Exception
int ReadRinexFiles(void);

/// Remove milliseconds, mark low elevations
/// @throw Exception
int PreProcess(void);

/// Call GDC for each pass and output
/// @throw Exception
int Process(void);

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
/// Class GlobalData (a singleton) encapsulates global static data as well as
/// command line definition and processing
class GlobalData : public Singleton<GlobalData> {
public:
   //// Default and only constructor, sets defaults.
   GlobalData() throw() { SetDefaults(); }

   // prgm housekeeping
   static const string Version;  ///< version string - see below
   string PrgmName;              ///< this program name
   string Title;                 ///< name, version and runtime
   string logfile;               ///< name of log file
   ofstream oflog;               ///< output log file stream

   // command line input ----------------------------------------------------------
   bool inputIsValid;            ///< if true, quit after Validate()

   /// strings used by GetCommandLine - usage, errors, etc
   string cmdlineErrors,cmdlineDump,cmdlineUsage,cmdlineExtras;
   vector<string> cmdlineUnrecognized;

   string logpath,obspath;       ///< paths for log and obs files
   vector<string> obsfiles;      ///< input obs files - required
   string obsout, cmdout;        ///< output obs and command files

   vector<string> wantedObsIDs;  ///< 4-char obs types wanted in the data store
   unsigned int def_wanted_in;   ///< the number of default wantedObsIDs elements
   vector<string> syscode_in;    ///< systems:codes input
   unsigned int def_syscode_in;  ///< the number of default syscode_in elements

   Epoch startTime, stopTime;    ///< start and stop times for data
   double decdt;                 ///< decimate data to this timestep (sec)
   map<RinexSatID,int> GLOfreqCh;///< input freq channel - overrides eph input

   vector<string> DCcmds;        ///< GDC editing cmds - written to cmdout
   vector<RinexSatID> exSat;     ///< ignore these sats
   vector<RinexSatID> onlySat;   ///< process only these sats
   vector<int> exPass;           ///< ignore these passes
   vector<int> onlyPass;         ///< process only these passes
   bool fixMS;

   // ephemeris and refpos input
   vector<string> SP3files;      ///< SP3 ephemeris file names
   vector<string> RNavfiles;     ///< RINEX nav file names
   string ephpath;               ///< path for nav/ephemeris files
   SP3EphemerisStore SP3EphList; ///< Store of SP3 ephemeris data
   GPSEphemerisStore BCEphList;  ///< Store of NAV ephemeris data
   XvtStore<SatID> *pEph;        ///< Pointer to chosen ephemeris store
   Position Rx;                  ///< Receiver position provided by user
   double elevLimit;             ///< Elevation angle lower limit (deg)
   bool doElev;                  ///< set true if elevation screening is to be done

   vector<string> outlabels;     ///< tell GDC to output labels
   bool typehelp;                ///< print all RINEX3 ObsIDs, then quit
   bool DChelp;                  ///< help for the GDC
   bool DChelpall;               ///< help for the GDC with advanced options
   bool validate;                ///< validate the input, then quit
   string timefmt;               ///< output format for GDC
   // end command line input ------------------------------------------------------

   bool verbose;                 ///< flag handled by CommandLine
   int debug;                    ///< int handled by CommandLine

   double nomdt;                 ///< nominal timestep of data
   vector<SatPass> SPList;       ///< the SatPass list
   vector<string> obstypes;      ///< list of 4 obstypes e.g. L1 L2 P1 P2
   /// map<sys,vector<string>> of SatPass obstypes each system parallel to obstypes
   map<char, vector<string> > SPsysobs;
   /// map<sys,vector<string>> of R3 obsID each system parallel to SPsysobs & obstypes
   map<char, vector<string> > R3sysobs;

   // these parallel : they come from --syscode s:codes,s
   vector<string> Syss;          ///< systems, 1-char strings
   vector<string> Codes;         ///< preferred codes, in order of "quality"

   Rinex3ObsHeader header;       ///< save for RINEX output

   gdc GDC;                      ///< the GDC object

   vector<string> EditCmds;      ///< editing commands returned by GDC - write cmdout
   string longfmt;               ///< times in loader, error messages, etc.

private:

   /// Define default values, called by c'tor
   void SetDefaults(void) throw()
   {
      PrgmName = string("dfix");
      logfile = string();

      // command line input -------------------------------------------
      // input control
      startTime = CommonTime::BEGINNING_OF_TIME;
      stopTime = CommonTime::END_OF_TIME;

      // default obs types - GPS only, ignored if user provides input
      wantedObsIDs.push_back("GC1*");
      wantedObsIDs.push_back("GC2*");
      wantedObsIDs.push_back("GL1*");
      wantedObsIDs.push_back("GL2*");
      def_wanted_in = wantedObsIDs.size();

      // default tracking codes, ignored if user provides input
      // NB since GPS L2C (code X,L) is not constellation-wide, suggest G:PYWCXL
      syscode_in.push_back("G:PYWCXL");
      syscode_in.push_back("R:PC");
      def_syscode_in = syscode_in.size();

      // editing
      decdt = -1.0;
      fixMS = doElev = false;
      elevLimit = 0.0;

      // output
      DChelp = DChelpall = typehelp = validate = false;
      timefmt = string("%4F %10.3g");
      //timefmt = string("%.6Q");         // 1.5/86400 = 1.7e-5

      // end command line input ---------------------------------------

      longfmt = string("%04F %10.3g %04Y/%02m/%02d %02H:%02M:%06.3f %P");
   }

}; // end class GlobalData

//------------------------------------------------------------------------------------
/// version string
const string GlobalData::Version(string("3.0 6/20/19"));

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
   string PrgmName;        // for catch only
try {
   // begin counting time
   clock_t totaltime = clock();
   Epoch wallbegin,wallend;
   wallbegin.setLocalTime();

   // get (create) the global data object (a singleton);
   // since this is the first instance, this will also set default values
   GlobalData& GD=GlobalData::Instance();
   PrgmName = GD.PrgmName;

   // Build title
   Epoch ttag;
   ttag.setLocalTime();
   GD.Title = GD.PrgmName + " ver " + GD.Version
                           + ttag.printf(", Run %04Y/%02m/%02d at %02H:%02M:%02S");
 
   // display title on screen
   LOG(INFO) << GD.Title;

   // TEMP, for debugging CommandLine;
   //LOGlevel = ConfigureLOG::Level("DEBUG");

   // process : loop once -----------------------------------------------------
   int iret;
   for(bool go=true; go; go=false)  {

      // process the command line ------------------------------------
      // open log file, dump config
      iret = GetCommandLine(argc,argv);
      if(iret) break;

      // check input, add paths to filenames
      iret = ValidateInput();
      if(iret) break;

      // fill ephemeris store
      iret = Initialize();
      if(iret) break;

      // read files into SatPassList
      iret = ReadRinexFiles();
      if(iret) break;

      // preprocess the data - millisec fix,
      iret = PreProcess();
      if(iret) break;

      // do it
      iret = Process();
      if(iret) break;

   }  // end loop once

   // error condition ---------------------------------------------------------
   // return codes: 0 ok, -3,1..6 from CommandLine - see below
   //               6 decimation invalid
   //               7 failed to read RINEX file(s)
   //               8 no good data
   //LOG(INFO) << "Return code is " << iret;
   if(iret != 0) {
      if(iret != 1) {
         string msg;
         msg = GD.PrgmName + string(" is terminating with code ")
                           + StringUtils::asString(iret);
         LOG(ERROR) << msg;
      }

      if(iret == 1) {                           // help(s) handled in GetCommand
         ;
      }
      else if(iret == 2) { LOG(INFO) << GD.cmdlineErrors; }    // cmd line errors
      else if(iret == 3) { LOG(INFO) << "The user requested input validation."; }
      else if(iret == 4) { LOG(INFO) << "The input is invalid."; }
      else if(iret == 5) { LOG(INFO) << "The log file could not be opened."; }
      else if(iret == 6) { LOG(INFO) << "Decimation was configured incorrectly."; }
      else if(iret == 7) { LOG(INFO) << "Failed to read all RINEX files."; }
      else if(iret == 8) { LOG(INFO) << "No good data was found."; }
      //else if(iret == 9) { LOG(INFO) << "Input read configuration is invalid."; }
      else if(iret == -3) { // cmd line definition invalid
         LOG(INFO) << "The command line definition is invalid.\n" << GD.cmdlineErrors;
      }
      else
         LOG(INFO) << "temp - Some other return code...fix this" << iret;
   }

   // compute and print run time ----------------------------------------------
   if(iret != 1) {
      wallend.setLocalTime();
      totaltime = clock()-totaltime;
      ostringstream oss;
      oss << PrgmName << " timing: " << fixed << setprecision(3)
         << double(totaltime)/double(CLOCKS_PER_SEC)
         << " seconds. (" << (wallend - wallbegin) << " sec)";
      LOG(INFO) << oss.str();
      if(pLOGstrm != &cout) cout << oss.str() << endl;
   }

   return (iret == 0 ? 0 : 1);
}
catch(Exception& e) {
   cerr << PrgmName << " caught Exception:\n" << e.what() << endl;
   // don't use LOG here - causes hangup - don't know why
}
catch (...) {
   cerr << "Unknown error in " << PrgmName << ".  Abort." << endl;
}
   return -1;
}   // end main()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int GetCommandLine(int argc, char **argv)
{
try {
   unsigned int i,j,k;
   GlobalData& GD=GlobalData::Instance();
   vector<string> GLOstrs;                            // input strings GLOsat:n
   const string defaultstartStr("[Beginning of dataset]");
   const string defaultstopStr("[End of dataset]");
   string startStr(defaultstartStr), stopStr(defaultstopStr);
   string str,refPosStr;

   // create list of command line options, and fill it
   // put required options first - they will get listed first anyway
   CommandLine opts;

   // build the options list == syntax page
   string PrgmDesc =
      " Program " + GD.PrgmName + " will read input RINEX obs file(s) and ..."
      "\n Input is on the command line, or of the same format in a file "
      "(see --file below);\n lines in that file which begin with '#' are ignored. "
      "Accepted options are \n shown below, followed by a description, with default "
      "value, if any, in ().";

   // opts.Add(char, opt, arg, repeat?, required?, &target, pre-descript, descript.);
   // required options
   bool req(true);
   opts.Add('i', "obs", "name", true, req, &GD.obsfiles, "\n# Required input",
            "Name of input RINEX observation file(s)");

   // optional args
   req = false;
   string dummy("");         // dummy for --file
   opts.Add('f', "file", "name", true, req, &dummy, "\n# File I/O:",
            "Name of file containing more options [#-EOL = comment]");
   opts.Add('l', "log", "name", false, req, &GD.logfile, "",
            "Name of output log file");
   opts.Add(0, "logpath", "path", false, req, &GD.logpath, "",
            "Path for output log file");
   opts.Add(0, "obspath", "path", false, req, &GD.obspath, "",
            "Path for input RINEX observation file(s)");
   // Flow control
   opts.Add(0, "start", "time", false, req, &startStr, "\n# Flow control "
            "(time = MJD OR GPSweek,SOW OR YYYY,Mon,D,H,Min,S:",
            "Start processing the input data at this time");
   opts.Add(0, "stop", "time", false, req, &stopStr, "",
            "Stop processing the input data at this time");
   // RINEX input obs IDs
   opts.Add(0, "obsID", "ot", true, req, &GD.wantedObsIDs,
            "\n# RINEX3 Data Input (NB ObsIDs for dual-freq PR+phase required;"
            " defaults erased if obsID input is detected):",
            "RINEX3 Obs types (4-char) to read from files");
   opts.Add(0, "syscode", "s[:c]", true, false, &GD.syscode_in, "",
            "Allowed system:tracking_codes s:c, (c's in order); cf --typehelp");
   // Data input and config
   opts.Add(0, "dt", "name", false, req, &GD.decdt, "\n# Data input and config:",
            "Decimate timestep of the data to this in seconds");
   opts.Add(0, "DC", "cmd=val", true, req, &GD.DCcmds,"",
            "Set algorithm configuration parameter (see --DChelp)");
   opts.Add(0, "exSat", "sat", true, req, &GD.exSat, "\n# Editing:",
            "Exclude satellite(s) [e.g. G24 or R14 or R]");
   opts.Add(0, "onlySat", "sat", true, req, &GD.onlySat, "",
            "Process given satellite(s) only");
   opts.Add(0, "exPass", "npass", true, req, &GD.exPass, "",
            "Exclude satellite pass number(s)");
   opts.Add(0, "onlyPass", "npass", true, req, &GD.onlyPass, "",
            "Process given satellite pass number(s) only");
   opts.Add(0, "GLOfreq", "sat:n", true, false, &GLOstrs, "",
            "GLO channel #s for each sat [e.g. R17:-4]");
   opts.Add(0, "fixMS", "", false, false, &GD.fixMS, "",
            "Fix millisecond clock adjusts before processing");
   // Editing
   opts.Add(0, "elev", "deg", false, req, &GD.elevLimit,
            "\n# Exclude low elevation (req's elev>0, ref, and one of eph/nav):",
            "Minimum elevation angle (deg)");
   opts.Add(0, "ref", "X,Y,Z", false, req, &refPosStr, "",
            "Known position (ECEF,m), for computing residuals and ORDs");
   opts.Add(0, "eph", "fn", true, req, &GD.SP3files, "",
            "Input Ephemeris+clock (SP3 format) file name");
   opts.Add(0, "nav", "fn", true, req, &GD.RNavfiles, "",
            "Input RINEX nav file name(s)");
   opts.Add(0, "ephpath", "path", false, req, &GD.ephpath, "",
            "Path for input SP3 or RINEX ephemeris file(s)");

   // Output
   opts.Add(0, "validate", "", false, req, &GD.validate, "\n# Output:",
            "Read input and test its validity, then quit");
   opts.Add('o', "obsout", "name", true, req, &GD.obsout, "",
            "Name of output (edited) RINEX observation file");
   opts.Add(0, "cmdout", "name", true, req, &GD.cmdout, "",
            "Name of output file for RINEX editing commands");
   opts.Add(0, "dump", "label", true, req, &GD.outlabels, "",
            "Tell DC to output 'label' data (or 'all') to log - cf. DChelpall");
   opts.Add(0, "timefmt", "fmt", false, req, &GD.timefmt, "",
            "Output timetags with this format [cf. class Epoch]");
   // Help
   opts.Add(0, "DChelp", "", false, req, &GD.DChelp, "\n# Help",
            "Print list of DC parameters and their defaults, then quit");
   opts.Add(0, "DChelpall", "", false, req, &GD.DChelpall, "",
            "DChelp with advanced options included");
   opts.Add(0, "typehelp", "", false, false, &GD.typehelp, "",
            "Print this syntax page and table of all RINEX3 ObsIDs, and quit");

   // add options that are ignored (true if it has an arg)
   //opts.Add_ignore("--PRSoutput",true);

   // deprecated args
   //opts.Add_deprecated("--HtOffset","--ht");

   // --------------------------------------------------------------------------
   // declare it and parse it; write all errors to string GD.cmdlineErrors
   int iret = opts.ProcessCommandLine(argc,argv,PrgmDesc,
                         GD.cmdlineUsage,GD.cmdlineErrors,GD.cmdlineUnrecognized);
   if(iret == -2) return iret;      // bad alloc
   if(iret == -3) return iret;      // cmd line definition invalid

   // help, debug and verbose handled automatically by CommandLine
   GD.verbose = (LOGlevel >= VERBOSE);
   GD.debug = (LOGlevel - DEBUG);

   // --------------------------------------------------------------------------
   // do extra parsing -- append errors to GD.cmdlineErrors
   string msg;
   vector<string> fields;
   ostringstream oss,ossx;

   // unrecognized arguments are an error
   if(GD.cmdlineUnrecognized.size() > 0) {
      oss << " Error - unrecognized arguments:\n";
      for(i=0; i<GD.cmdlineUnrecognized.size(); i++)
         oss << GD.cmdlineUnrecognized[i] << "\n";
      oss << " End of unrecognized arguments\n";
   }

   // configure the DC
   if(GD.debug > -1) GD.GDC.setParameter("debug",GD.debug);
   if(GD.verbose) GD.GDC.setParameter("verbose",1);
   //GD.GDC.setParameter("DT",GD.dt);    // no - dt comes from SatPass

   for(i=0; i<GD.DCcmds.size(); i++) {
      msg = StringUtils::replaceAll(GD.DCcmds[i],string(" "),string(""));
      if(GD.GDC.setParameter(msg)) {
         if(GD.verbose) ossx << "Set GDC parameter with " << msg << endl;
      }
      else
         ossx << "   Warning - failed to set GDC parameter " << msg << endl;
   }
   for(i=0; i<GD.outlabels.size(); i++) {
      if(StringUtils::lowerCase(GD.outlabels[i]) == string("all")) {
         // NB keep this list updated with gdc; 1 means do, 0 means don't
         GD.GDC.setParameter("RAW=1"); // data (WL,GF) before any processing
         GD.GDC.setParameter("WL1=1"); // results of 1st diff filter on WL
         GD.GDC.setParameter("WLG=1"); // WL after fixing gross slips (after fdif)
         GD.GDC.setParameter("WLW=1"); // results of window filter on WL
         GD.GDC.setParameter("WLF=1"); // WL after fixing
         GD.GDC.setParameter("GF1=1"); // results of 1st diff filter on GF
         GD.GDC.setParameter("GFG=1"); // GF after fixing gross slips (after fdif)
         GD.GDC.setParameter("GFW=1"); // results of window filter on GF
         GD.GDC.setParameter("GFF=1"); // GF after fixing
         LOG(VERBOSE) << "Set GDC output to all data types";
      }
      else {
         msg = GD.outlabels[i]+"=1";
         if(GD.GDC.setParameter(msg)) {
            if(GD.verbose) ossx << "Set GDC output to include data type "
                                 << GD.outlabels[i] << endl;
         }
         else
            ossx << "   Warning - failed to set GDC output to include data type "
                                 << GD.outlabels[i] << endl;
      }
   }

   // RINEX output requested
   if(!GD.obsout.empty()) GD.GDC.setParameter("doFix=1");
   // Editing cmd output requested
   if(!GD.cmdout.empty()) GD.GDC.setParameter("doCmds=1");

   // start and stop times
   for(i=0; i<2; i++) {
      msg = (i==0 ? startStr : stopStr);
      if(msg == (i==0 ? defaultstartStr : defaultstopStr)) continue;

      int n(StringUtils::numWords(msg,','));
      if(n != 1 && n != 2 && n != 6) {
         oss << "Error - invalid argument in --" << (i==0 ? "start" : "stop")
            << " " << (i==0 ? startStr : stopStr) << endl;
         continue;
      }

      string fmtMJD("%Q"),fmtGPS("%F,%g"),fmtCAL("%Y,%m,%d,%H,%M,%S");
      try {
         (i==0 ? GD.startTime:GD.stopTime).scanf(msg,
            (n==1 ? fmtMJD : (n==2 ? fmtGPS : fmtCAL)));
      }
      catch(Exception&) {
         oss << "Error - invalid time in --" << (i==0 ? "start" : "stop")
            << " " << (i==0 ? startStr : stopStr) << endl;
      }
   }

   // GLONASS frequency channels
   for(i=0; i<GLOstrs.size(); i++) {
      msg = GLOstrs[i];
      fields = StringUtils::split(msg,':');
      if(fields.size() != 2) {
         oss << "Error - invalid input in --GLOfreq: " << msg << endl;
      }
      else {
         RinexSatID sat(fields[0]);
         int chan(StringUtils::asInt(fields[1]));      // for Solaris
         GD.GLOfreqCh.insert(make_pair(sat,chan));
      }
   }

   // reference position
   if(!refPosStr.empty()) {
      msg = refPosStr;
      fields.clear();
      while(msg.size() > 0) fields.push_back(StringUtils::stripFirstWord(msg,','));
      if(fields.size() != 3)
         oss << "Error - invalid field in --refPos input: " << refPosStr << endl;
      else {
         try { GD.Rx.setECEF(StringUtils::asDouble(fields[0]),
                             StringUtils::asDouble(fields[1]),
                             StringUtils::asDouble(fields[2]));
         }
         catch(Exception&) {
            oss << "Error - invalid position in --refPos input: "
               << refPosStr << endl;
         }
      }
   }

   // Obs types
   // Rinex3ObsHeader.cpp line ~1820 - mapping R2 -> R3:
   // C1->C1C       C2->C2X         C5->C5X
   // P1->C1W/Y     P2->C2X/W/Y     P5->P5X
   // L1->L1C/Y     L2->L2X/W/Y     L5->L5X
   // D1->D1C/Y     D2->D2X/W/Y     D5->D5X
   // S1->S1C/Y     S2->S2X/W/Y     S5->S5X
   // Y if have P2 and P is really Y code - this can't come from RINEX
   // W if have P2 but P is not Y
   // X if don't have P2
   // NB P[12] never -> tracking code P!

   // obs IDs ---------------------------------------------
   // remove the defaults if user provided inputs
   if(GD.wantedObsIDs.size() > GD.def_wanted_in) {
      vector<string> vtemp(GD.wantedObsIDs);
      GD.wantedObsIDs.clear();
      for(i=4; i< vtemp.size(); i++) 
         GD.wantedObsIDs.push_back(vtemp[i]);
   }

   // systems and codes -----------------------------------
   bool skip;
   string freqs(RinexObsID::validRinexFrequencies);
   // loop over input syscode_in, but note it will start with def_syscode_in default
   // elements, before any user input, and must see if those defaults were overridden
   for(i=0; i<GD.syscode_in.size(); i++) {
      fields = StringUtils::split(GD.syscode_in[i],':');       // system:codes
      string sys(fields[0]);                                // 1-char sys

      // if defaults are over-ridden by user input, skip them;
      // def_syscode_in is the number of def.s
      if(i < GD.def_syscode_in) {
         skip = false;
         for(j=i+1; j<GD.syscode_in.size(); j++)
            if(GD.syscode_in[j][0] == sys[0]) { skip=true; break; }  // user input sys
         if(skip) continue;
      }

      // if there are no "wanted" ObsIDs for this system, skip it
      skip = true;
      for(j=0; j<GD.wantedObsIDs.size(); j++) {
         if(GD.wantedObsIDs[j][0] == '*' || GD.wantedObsIDs[j][0] == sys[0])
            { skip = false; break; }
      }
      if(skip) {
         if(i >= GD.def_syscode_in)       // don't warn about the defaults
            ossx << "   Warning - system " << sys << " was specified (--syscode)"
               << " but no ObsIDs for it were specified (--obsID) - skip this system."
               << endl;
         continue;
      }

      // For now, allow only GPS and GLO
      if(sys != "G" && sys != "R") {
         ossx << "   Warning - only GPS and GLONASS are currently supported." << endl;
         continue;
      }

      // ok, save the system and codes
      GD.Syss.push_back(sys);             // keep parallel to Codes
      if(fields.size() > 1)
         str = fields[1];                 // user-supplied codes
      else {                              // default codes - combine all frequencies
         str = string("");
         for(j=0; j<freqs.size(); j++) {
            // default codes for freq j
            string strf(RinexObsID::validRinexTrackingCodes[sys[0]][freqs[j]]);
            StringUtils::stripTrailing(strf,"* ");
            if(str.empty() && !strf.empty()) str=strf;
            else for(k=0; k<strf.size(); k++) {
               if(str.find_first_of(strf.substr(k,1),0) == string::npos)
                  str += string(1,strf[k]);
            }
         }
      }
      GD.Codes.push_back(str);               // keep parallel to GD.Syss
      ossx << "   Include system:codes " << sys << ":" << str << endl;
   }

   // check wantedObsIDs for invalid/no-system (NB ROFL::loadObsID also checks size)
   for(i=0; i<GD.wantedObsIDs.size(); i++) {
      if(GD.wantedObsIDs[i].size() != 4) {
         oss << "Error : invalid RINEX3 obsID (not 4-char): "
                        << GD.wantedObsIDs[i] << endl;
         continue;
      }
      str = GD.wantedObsIDs[i].substr(0,1);
      if(str != "*" && vectorindex(GD.Syss,str) == -1) {
         oss << "Error : invalid RINEX3 obsID (system not found): "
            << GD.wantedObsIDs[i] << endl;
         continue;
      }
      // test using ROFL
      Rinex3ObsFileLoader rofl;
      if(!rofl.loadObsID(GD.wantedObsIDs[i])) {
         oss << "Error : invalid or duplicate RINEX3 (3-char) obsID: "
            << GD.wantedObsIDs[i] << endl;
         continue;
      }
      ossx << "   Request ObsID " << GD.wantedObsIDs[i] << endl;
   }

   // extra msg for debug<n>
   ossx << "   NB. debug0/1/2/3 <=> --dump WLF,GFF,FIN / +RAW / +WL1,WLG,GF1,GFG"
         << " / +WLW,GFW" << endl;

   // build list of desired obstypes - C may replace P within the loader
   GD.obstypes.push_back(string("L1"));
   GD.obstypes.push_back(string("L2"));
   GD.obstypes.push_back(string("P1"));
   GD.obstypes.push_back(string("P2"));

   // --------------------------------------------------------------------------
   // append errors
   GD.cmdlineErrors += oss.str();
   GD.cmdlineExtras += ossx.str();

   LOG(DEBUG) << GD.cmdlineUsage;  // this will contain list of args

   // dump it
   oss.str("");         // clear it
   oss << "#------ Summary of " << GD.PrgmName
      << " command line configuration --------" << endl;
   opts.DumpConfiguration(oss);

   // dump the 'extra parsing' things
   oss << "\n# Extra Processing:\n" << GD.cmdlineExtras;
   if(GD.verbose && GD.GLOfreqCh.size() > 0) {
      oss << "#\n# GLO frequency channels:";
      map<RinexSatID,int>::const_iterator it(GD.GLOfreqCh.begin());
      i = 0;
      for( ; it != GD.GLOfreqCh.end(); ++it) {
         oss << " " << it->first << ":" << it->second;
         if((++i % 9)==0) { i=0; oss << endl << "#                        "; }
      }
      oss << endl;
   }
   oss << "#------ End configuration summary --------";

   GD.cmdlineDump = oss.str();

   if(opts.hasHelp() || GD.DChelp || GD.DChelpall || GD.typehelp) {
      LOG(INFO) << GD.cmdlineUsage;
      // handle DChelp
      if(GD.DChelpall || GD.DChelp) {
         LOG(INFO) << "";
         GD.GDC.DisplayParameterUsage(LOGstrm,"#",GD.DChelpall);
      }
      if(GD.typehelp) dumpAllRinex3ObsTypes(LOGstrm);
      return 1;
   }
   if(opts.hasErrors()) return 2;
   if(!GD.cmdlineErrors.empty()) {
      //LOG(INFO) << "RETURNING invalid b/c of errors:\n" << GD.cmdlineErrors;
      return 2;
   }

   // --------------------------------------------------------------------
   // Open log file, if it exists
   if(!GD.logfile.empty()) {
      GD.oflog.open(GD.logfile.c_str(),ios_base::out);
      if(!GD.oflog.is_open()) {
         cerr << "Failed to open log file " << GD.logfile << endl;
         return 5;
      }
      LOG(INFO) << "Output directed to log file " << GD.logfile;
      pLOGstrm = &GD.oflog; // ConfigureLOG::Stream() = &GD.oflog;
      LOG(INFO) << GD.Title;
   }

   // configure log stream
   ConfigureLOG::ReportLevels() = false;
   ConfigureLOG::ReportTimeTags() = false;
   // debug and verbose handled earlier in GetCommandLine/PreProcessArgs
   if(GD.debug > -1)
      ; // handled in CommandLine::PreProcessArgs()
   else if(GD.verbose)
      LOGlevel = ConfigureLOG::Level("VERBOSE");

   // dump configuration
   if(GD.debug > -1) {
      LOG(INFO) << "Found debug switch at level " << GD.debug;
      LOG(INFO) << "\n" << GD.cmdlineUsage;  // this will contain list of args
      // NB debug turns on verbose
   }

   LOG(VERBOSE) << GD.cmdlineDump;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
// Check input: read RINEX and generate a complete header, saving the data.
// if not correct, set bool GD.inputIsValid
int ValidateInput(void)
{
try {
   int i;
   string msg;
   GlobalData& GD=GlobalData::Instance();
   GD.inputIsValid = true;

   if(GD.validate) LOG(INFO) << " ---- Validate configuration ----";

   // where else to do this?
   include_path(GD.logpath,GD.logfile);
   expand_filename(GD.logfile);

   if(GD.obsfiles.size() == 0) {
      LOG(ERROR) << "Error - No input file.";
      GD.inputIsValid = false;
   }
   else {
      for(i=0; i<GD.obsfiles.size(); i++) {
         include_path(GD.obspath,GD.obsfiles[i]);
         expand_filename(GD.obsfiles[i]);
      }

      // sort filenames on time
      if(GD.obsfiles.size() > 1) {
         msg = sortRinexObsFiles(GD.obsfiles);
         if(!msg.empty()) {
            LOG(ERROR) << msg;
            GD.inputIsValid = false;
         }
      }

      for(i=0; i<GD.SP3files.size(); i++) {
         include_path(GD.ephpath,GD.SP3files[i]);
         expand_filename(GD.SP3files[i]);
      }
      for(i=0; i<GD.RNavfiles.size(); i++) {
         include_path(GD.ephpath,GD.RNavfiles[i]);
         expand_filename(GD.RNavfiles[i]);
      }
   }

   if(GD.validate) {
      LOG(INFO) << " ---- Input is "
         << (GD.inputIsValid ? "" : "NOT ") << "valid ----";
      return 3;
   }

   if(!GD.inputIsValid) return 4;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int Initialize(void)
{
try {
   int n;
   GlobalData& GD=GlobalData::Instance();

   // exclude small elevation --------------------------------------
   // get nav files and build EphemerisStore
   if(GD.SP3files.size() > 0) {
      n = FillEphemerisStore(GD.SP3files, GD.SP3EphList, GD.BCEphList);
      if(GD.verbose) {
         LOG(VERBOSE) << "Added " << n << " SP3 ephemeris files to store.";
      }
   }
   else if(GD.RNavfiles.size() > 0) {
      n = FillEphemerisStore(GD.RNavfiles, GD.SP3EphList, GD.BCEphList);
      if(GD.verbose) {
         LOG(VERBOSE) << "Added " << n << " nav ephemeris files to store.";
      }
   }

   if(GD.SP3files.size() > 0 && GD.RNavfiles.size() > 0)
      LOG(WARNING) << " Warning - SP3 ephemeris used; RINEX nav ignored.";

   if(GD.SP3EphList.ndata() > 0) {
      // set gap and interval checking, based on nominal timestep
      // take default GD.SP3EphList.setPositionInterpOrder(order);
      int order = GD.SP3EphList.getPositionInterpOrder();
      GD.SP3EphList.setClockLinearInterp();

      vector<SatID> sats(GD.SP3EphList.getSatList());
      double dt(GD.SP3EphList.getClockTimeStep(sats[0]));
      GD.SP3EphList.setClockGapInterval(dt+1);
      GD.SP3EphList.setClockMaxInterval((order-1)*dt+1);
      dt = GD.SP3EphList.getPositionTimeStep(sats[0]);
      GD.SP3EphList.setPosGapInterval(dt+1);
      GD.SP3EphList.setPosMaxInterval((order-1)*dt+1);
      if(GD.debug >= 0) GD.SP3EphList.dump(LOGstrm,1);
      else if(GD.verbose) GD.SP3EphList.dump(LOGstrm,0);
      GD.pEph = &GD.SP3EphList;
   }
   else if(GD.BCEphList.size() > 0) {
      if(GD.verbose) GD.BCEphList.dump(LOGstrm,1);
      GD.pEph = &GD.BCEphList;
   }
   else if(GD.elevLimit > 0.0) {
      GD.elevLimit = 0.0;
      LOG(WARNING) << " Warning - unable to build ephemeris store; ignore elevations";
   }

   // is it there?
   if(GD.pEph != NULL) {
      if(GD.Rx.getCoordinateSystem() != Position::Unknown && GD.elevLimit > 0.0)
         GD.doElev = true;
      else if(GD.Rx.getCoordinateSystem() != Position::Unknown) {
         LOG(WARNING) << " Warning - Excluding low elevation requires --elev";
      }
      else {
         LOG(WARNING) << " Warning - Excluding low elevation requires --ref";
      }
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
/// Utility for use by ReadRinexFiles()
/// find the index in loader obs ids for the given system and obstype, accepting codes
/// from the string codes, in order. If totals for this code is zero, try for another.
/// return -2 if obstype is not one of L1,L2,P1,P2,
///     or -1 if it is, but no R3ObsID was found
int findIndex(const vector<string>& allR3ObsID, const char sys,
                  const string& obs, const string& codes, const vector<int>& totals)
{
   if(obs[0] != 'P' && obs[0] != 'L') return -2;
   if(obs[1] != '1' && obs[1] != '2') return -2;
   char type(obs[0]=='P' ? 'C' : 'L');
   char freq(obs[1]);
   //const string tkcodes(RinexObsID::validRinexTrackingCodes[sys][freq]);
   string test3("123"); test3[0]=sys; test3[1]=type; test3[2]=freq;

   int ind(-1);
   unsigned int i,j,bestj;
   for(i=0; i<allR3ObsID.size(); i++) {
      const string oid(allR3ObsID[i]);
      if(test3 == oid.substr(0,3)) {
         for(j=0; j<codes.size(); j++) {
            if(oid[3] == codes[j]) {
               if(totals[i] == 0) {
                  LOG(WARNING) << " Warning - no data found for " << sys << " "
                     << obs << " in " << allR3ObsID[i] << " - skip this R3ObsID.";
                  continue;
               }
               if(ind == -1 || j < bestj) { ind = i; bestj = j; }
               break;
            }
         }
      }
   }

   return ind;
}

//------------------------------------------------------------------------------------
// Reads RINEX 2|3 Obs file(s) into SatPass list SPList
int ReadRinexFiles(void)
{
try {
   int n,iret;
   unsigned int i,j;
   string str,msg;
   GlobalData& GD=GlobalData::Instance();

   LOG(INFO) << "\nLoad the RINEX files using Rinex3ObsFileLoader -------";

   // declare the ROFL, giving it all the filenames
   // NB could also loop over files here, defining an ROFL for each one...
   Rinex3ObsFileLoader rofl(GD.obsfiles);

   // configure the loader
   // NB this already done in ExtraProcessing but leave it here anyway
   for(i=0; i<GD.wantedObsIDs.size(); i++) { // pass in desired obs IDs
      if(!rofl.loadObsID(GD.wantedObsIDs[i])) {
         LOG(WARNING) << " Warning - ignore invalid or duplicate ObsID request: "
                     << GD.wantedObsIDs[i];
      }
   }

   // NB do this in Edit() so the pass can be marked as excluded
   //for(i=0; i<GD.exSats.size(); i++)   // excluded sats (not systems)
   //   rofl.excludeSat(GD.exSats[i]);
   rofl.saveTheData(true);             // no sense to have wantedObsIDs without save
   if(GD.decdt > 0.0)                  // decimate
      rofl.setDecimation(GD.decdt);
   rofl.setStartTime(GD.startTime);    // start and stop times
   rofl.setStopTime(GD.stopTime);
   rofl.setTimeFormat(GD.longfmt);     // time format

   // load the files
   iret = rofl.loadFiles(str,msg);
   if(iret < 0 || !str.empty()) {
      LOG(ERROR) << " Error - Loader failed: returned " << iret
                     << " with message " << str;
      return -1;
   }
   if(!msg.empty()) LOG(INFO) << msg;
   LOG(INFO) << "Loader read " << iret << " file" << (iret>1?"s":"")
               << " successfully " << endl;

   // dump a summary of the whole thing, including the sat/obs counts table
   LOG(INFO) << rofl.asString();

   // dump the headers
   for(i=0; i<GD.obsfiles.size(); i++) {
      LOG(INFO) << "\nHeader for file " << GD.obsfiles[i];
      GD.header = rofl.getFullHeader(i);
      GD.header.dump(LOGstrm);
   }

   // dump the data - NB setTimeFormat()
   // could also get data store and dump each element
   //const vector<Rinex3ObsData>& datastore(rofl.getStore());
   //if(DUMP(DATA)) rofl.dumpStoreData(LOGstrm);

   // now write to SatPass -------------------------------------------
   LOG(INFO) << "\nWrite to SatPass -----------------------------";

   // ------------------------------------------------
   // get vector of ObsIDs actually stored by loader
   const vector<string> loadR3ObsIDs(rofl.getWantedObsTypes());
   // get vector of total epoch counts for loadR3ObsIDs
   const vector<int> totcounts(rofl.getTotalObsCounts());
   // indexes[sys][i] = index in loadR3ObsIDs for GD.obstypes[i] in system sys
   map<char, vector<int> > indexes;

   // loop over all systems, all obstypes: find corresponding R3 ObsID in loader's
   // output, and construct a new obstype for SatPass.
   for(i=0; i<GD.Syss.size(); i++) {
      char sys(GD.Syss[i][0]);         // Syss[i] is 1-char string

      string codes(GD.Codes[i]);       // users prioritized list
      vector<int> v;
      indexes[sys] = v;          // initialize indexes[sys] with empty vector
      vector<string> SPot;       // SatPass obstype P: P-code PR, C: C/A PR, L: phase
      vector<string> R3ot;       // R3 obstype from loader for SPot
      for(j=0; j<GD.obstypes.size(); j++) {
         // find the R3ObsID in loader
         n = findIndex(loadR3ObsIDs,sys,GD.obstypes[j],codes,totcounts);
         if(n == -1) {
            LOG(ERROR) << " Error - loader found no R3ObsID for system " << sys
                           << " obstype " << GD.obstypes[j] << ". Abort.";
            return -2;
         }
         indexes[sys].push_back(n);

         // add to map SPsysobs, use the tracking code to decide on pseudorange P|C
         // for purposes of determining when to apply differential code biases.
         string ot(GD.obstypes[j]);
         if(ot[0] == 'P') {                  // obstypes above and findIndex() use 'P'
            char tc(loadR3ObsIDs[n][3]);     // tracking code
            if(sys=='G') {                   // GPS
               switch(tc) {
                  case 'P': case 'Y': case 'W':
                  case 'I': case 'M': case 'Q': case 'D':      // all P-code, right?
                     ot[0] = 'P'; break;
                  case 'C': case 'L': case 'X': case 'S':      // all C/A
                     ot[0] = 'C'; break;
                  default: break;
               }
            }
            if(sys=='R') {                   // GLO
               ot[0] = tc;                   // P or C are only choices
            }
         }
         SPot.push_back(n >= 0 ? ot : GD.obstypes[j]);
         R3ot.push_back(n >= 0 ? loadR3ObsIDs[n] : "-NA-");
      }

      GD.SPsysobs[sys] = SPot;
      GD.R3sysobs[sys] = R3ot;
   }

   // print obs types assignment SP <=> R3
   LOG(INFO) << " Assign RINEX3-ObsIDs to SatPass obstypes for each system :";
   ostringstream oss;

   map<char, vector<string> >::const_iterator rit = GD.R3sysobs.begin();
   while(rit != GD.R3sysobs.end()) {
      oss.str("");
      oss << " System " << rit->first << " ("
        << RinexObsID::map1to3sys[string(1,rit->first)] << "): SatPass obstypes = [";
      for(i=0; i<rit->second.size(); i++)
         oss << (i==0 ? "":",") << rit->second[i];
      LOG(INFO) << oss.str() << "]";
      rit++;
   }

   // define dt
   GD.nomdt = rofl.getDT();
   LOG(VERBOSE) << fixed << setprecision(2)
      << " The input data interval is " << GD.nomdt << " seconds.";

   // -------------------------------------------------------------
   // NB all passes have the same SP obs types (per sys)
   n = rofl.WriteSatPassList(GD.SPsysobs, indexes, GD.SPList);

   LOG(INFO) << " WriteSatPassList returned " << n << " passes.";
   LOG(INFO) << " Dump the passes:";
   for(i=0; i<GD.SPList.size(); i++) {
      LOG(INFO) << "SPL " << setw(3) << i+1 << " " << GD.SPList[i];
   }

   rofl.reset();

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end ReadRinexFiles()

//------------------------------------------------------------------------------------
int PreProcess(void)
{
try {
   unsigned int i,j;
   string msg;
   GlobalData& GD=GlobalData::Instance();

   msecHandler msh;
   i = FindMilliseconds(GD.SPList, msh);
   LOG(INFO) << "\n" << msh.getFindMessage(GD.fixMS);    // flag is verbose

   if(GD.fixMS && i) {
      RemoveMilliseconds(GD.SPList, msh);
      LOG(INFO) << msh.getFixMessage(GD.verbose);
   }

   // mark low elevation data bad
   if(GD.doElev) {
      CorrectedEphemerisRange CER;
      for(i=0; i<GD.SPList.size(); i++) {
         if(GD.SPList[i].status() == -1) continue;

         RinexSatID sat = GD.SPList[i].getSat();
         for(j=0; j<GD.SPList[i].size(); j++) {
            Epoch ttag = GD.SPList[i].time(j);
            try {
               //double ER =
               CER.ComputeAtReceiveTime(ttag, GD.Rx, sat, *GD.pEph);
               if(CER.elevation >= GD.elevLimit) continue;
            }
            catch(InvalidRequest&) {
               // do not exclude the sat here; PRSolution will...
               LOG(DEBUG) << "CER did not find ephemeris for "
                  << sat << " at time " << ttag.printf(GD.timefmt);
               // fall through
            }

            // mark it bad
            GD.SPList[i].setFlag(j,SatPass::BAD);

         }  // end loop over data in SPList[i]
      }  // end loop over SatPasses
   }  // end if elev mask

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int Process(void)
{
try {
   int iret=-666,i=-666,GLOn=-666;
   string msg;
   ostringstream oss;
   map<RinexSatID,int>::const_iterator gloit;
   GlobalData& GD=GlobalData::Instance();

   // dump the configuration
   LOG(INFO) << "\n# GDC configuration:";
   GD.GDC.DisplayParameterUsage(LOGstrm, "#", true);
   LOG(INFO) << "# End of GDC configuration.\n";

   // call the GDC
   string retmsg;
   for(i=0; i<GD.SPList.size(); i++) {
      // configure SatPass SPList[i]
      GD.SPList[i].setOutputFormat(GD.timefmt);       // nround?

      RinexSatID sat(GD.SPList[i].getSat());

      // exclude sats
      if(vectorindex(GD.exSat,sat) != -1) {
         LOG(VERBOSE) << "DFX " << setw(3) << i+1 << " " << sat << " sat excluded.";
         continue;
      }
      if(GD.onlySat.size() > 0 && vectorindex(GD.onlySat,sat) == -1) {
         LOG(VERBOSE) << "DFX " << setw(3) << i+1 << " " << sat << " not only sat.";
         continue;
      }

      // exclude passes
      if(GD.onlyPass.size() > 0 && vectorindex(GD.onlyPass,i+1) == -1) {
         LOG(VERBOSE) << "DFX " << setw(3) << i+1 << " " << sat << " pass excluded.";
         continue;
      }

      // no good data
      if(GD.SPList[i].getNgood() == 0) {
         LOG(VERBOSE) << "DFX " << setw(3) << i+1 << " " << sat << " no good data.";
         continue;
      }

      // get the GLOn
      if(sat.system == SatelliteSystem::Glonass) {
         gloit = GD.GLOfreqCh.find(sat);

         // if GLONASS frequency channel not given, try to find it
         if(gloit != GD.GLOfreqCh.end()) {
            GLOn = gloit->second;
         }
         else {
            if(!GD.SPList[i].getGLOchannel(GLOn, msg)) {
               LOG(WARNING) << " Warning - unable to compute GLO channel for sat "
                  << sat << " - skip pass : " + msg;
            }
            else {
               LOG(VERBOSE) << "# GLO frequency channel for " << sat
                           << " was computed from data, = " << GLOn << "; " << msg;
               GD.GLOfreqCh[sat] = GLOn;
            }
         }
      }

      // make the unique number == pass number == i+1, always
      GD.GDC.ForceUniqueNumber(i);     // NB it will be incremented in DC call

      // call GDC
      iret = GD.GDC.DiscontinuityCorrector(GD.SPList[i], retmsg, GD.EditCmds, GLOn);
      // TD is iret<0 handled by retmsg?
      int unique(GD.GDC.getUniqueNumber());        // == i+1 here

      // save the GLO freq channel
      if(sat.system == SatelliteSystem::Glonass &&
         GD.GLOfreqCh.find(sat) == GD.GLOfreqCh.end())
            GD.GLOfreqCh[sat] = GLOn;

      // add tag to lines in the retmsg
      oss.str(""); oss << "DFX " << setw(3) << unique << " " << sat;
      msg = oss.str();
      // add tag == msg to all the lines in retmsg
      StringUtils::change(retmsg,"\n","\n"+msg+" ");
      retmsg = msg + " " + retmsg;
      LOG(INFO) << retmsg;
   }

   // write editing commands
   if(!GD.cmdout.empty()) {
      ofstream ofs;
      ofs.open(GD.cmdout.c_str(),ios_base::out);
      if(!GD.oflog.is_open()) {
         LOG(ERROR) << " Error - failed to open file " << GD.cmdout;
         GD.cmdout = string();
      }
      else {
         for(i=0; i<GD.EditCmds.size(); i++)
            ofs << GD.EditCmds[i] << endl;
         ofs.close();
      }
   }

   // write to RINEX
   if(!GD.obsout.empty()) {
      LOG(INFO) << "Write to RINEX file " << GD.obsout;

      // strict RINEX, and we may have computed them
      GD.header.glonassFreqNo.clear();
      for(gloit = GD.GLOfreqCh.begin(); gloit != GD.GLOfreqCh.end(); ++gloit)
         GD.header.glonassFreqNo[gloit->first] = gloit->second;
      GD.header.valid |= Rinex3ObsHeader::validGlonassSlotFreqNo;
      // strict RINEX
      GD.header.valid |= Rinex3ObsHeader::validGlonassCodPhsBias;

      GD.header.commentList.push_back(string("Edited by ")+GD.Title);

      i = SatPassToRinex3File(GD.obsout, GD.header, GD.R3sysobs, GD.SPList);
      LOG(VERBOSE) << "SatPassToRinex3File returned " << i;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
