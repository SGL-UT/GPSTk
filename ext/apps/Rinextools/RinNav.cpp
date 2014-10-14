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

/// @file RinNav.cpp
/// Read RINEX navigation files (version 2 or 3) and write out as a single RINEX nav
/// file; optionally write a summary of the contents to the screen.

// system
#include <string>
#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <fstream>

// GPSTK
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "singleton.hpp"
#include "expandtilde.hpp"
#include "logstream.hpp"
#include "stl_helpers.hpp"
#include "CommandLine.hpp"
#include "CommonTime.hpp"
#include "Epoch.hpp"
#include "TimeString.hpp"

#include "RinexSatID.hpp"

#include "Rinex3NavBase.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3NavStream.hpp"
#include "Rinex3EphemerisStore.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
string Version(string("2.2 10/31/13"));

//------------------------------------------------------------------------------------
// Object for command line input and global data
class Configuration : public Singleton<Configuration> {

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
   void SetDefaults(void) throw();

public:

// member data
   CommandLine opts;             // command line options and syntax page
   static const string PrgmName; // program name
   string Title;                 // id line printed to screen and log

   // start command line input
   bool help, verbose, outver2;
   int debug;
   string cfgfile;

   vector<string> InputNavFiles; // RINEX nav file names
   vector<string> outputStrs;    // strings of the form [sys,]filename
   vector<string> output2Strs;   // strings of the form [sys,]filename

   string Navpath;               // paths

   // times derived from --start and --stop
   string defaultstartStr,startStr;
   string defaultstopStr,stopStr;
   CommonTime beginTime,endTime;

   // exclude sats
   vector<RinexSatID> exclSat;

   string userfmt;            // user's time format for output

   // end of command line input

   // output file names for each system, use blank for "all output"
   map<string, string> mapSysOutputFile;  // map< "G" , "test.nav">
   map<string, string> mapSysOutput2File; // map< "G" , "test.nav"> version 2 output

   // Store all the nav data
   Rinex3EphemerisStore NavStore;

   string msg;
   static const string calfmt,gpsfmt,longfmt;

}; // end class Configuration

//------------------------------------------------------------------------------------
// const members of Configuration
const string Configuration::PrgmName = string("RinNav");
const string Configuration::calfmt = string("%04Y/%02m/%02d %02H:%02M:%02S");
const string Configuration::gpsfmt = string("%4F %10.3g");
const string Configuration::longfmt = calfmt + " = " + gpsfmt;

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// prototypes
int Initialize(string& errors) throw(Exception);
int ProcessFiles(void) throw(Exception);

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
   // get the (single) instance of the configuration
   Configuration& C(Configuration::Instance());

try {
   int iret;
   clock_t totaltime(clock());
   Epoch wallclkbeg;
   wallclkbeg.setLocalTime();

   // build title = first line of output
   C.Title = "# " + C.PrgmName + ", part of the GPS Toolkit, Ver " + Version
      + ", Run " + printTime(wallclkbeg,C.calfmt);

   for(;;) {
      cout << C.Title << endl;
      // get information from the command line
      // iret -2 -3 -4
      if((iret = C.ProcessUserInput(argc, argv)) != 0) break;

      // read stores, check input etc
      string errs;
      if((iret = Initialize(errs)) != 0) {
         LOG(ERROR) << "------- Input is not valid: ----------\n" << errs
                    << "\n------- end errors -----------";
         break;
      }
      if(!errs.empty()) LOG(INFO) << errs;         // Warnings are here too

      iret = ProcessFiles();                       // iret == number of files

      break;      // mandatory
   }

   if(iret >= 0) {
      // print elapsed time
      totaltime = clock()-totaltime;
      Epoch wallclkend;
      wallclkend.setLocalTime();
      ostringstream oss;
      oss << C.PrgmName << " timing: processing " << fixed << setprecision(3)
         << double(totaltime)/double(CLOCKS_PER_SEC) << " sec, wallclock: "
         << setprecision(0) << (wallclkend-wallclkbeg) << " sec.";
      LOG(INFO) << oss.str();
   }

   return iret;
}
catch(FFStreamError& e) { cerr << "FFStreamError: " << e.what(); }
catch(Exception& e) { cerr << "Exception: " << e.what(); }
catch (...) { cerr << "Unknown exception.  Abort." << endl; }
   return 1;

}  // end main()

//------------------------------------------------------------------------------------
// return -5 if input is not valid
int Initialize(string& errors) throw(Exception)
{
try {
   bool isValid(true);
   Configuration& C(Configuration::Instance());
   errors = string("");

   // add path to filenames, and expand tilde (~)
   include_path(C.Navpath, C.InputNavFiles);
   expand_filename(C.InputNavFiles);

   ostringstream ossE;

   // -------- save errors and output
   errors = ossE.str();
   stripTrailing(errors,'\n');
   replaceAll(errors,"\n","\n# ");

   if(!isValid) return -5;
   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end Initialize()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
void Configuration::SetDefaults(void) throw()
{
   defaultstartStr = string("[Beginning of dataset]");
   defaultstopStr = string("[End of dataset]");
   beginTime = CommonTime::BEGINNING_OF_TIME;
   endTime = CommonTime::END_OF_TIME;

   userfmt = gpsfmt;
   help = verbose = false;
   debug = -1;

}  // end Configuration::SetDefaults()

//------------------------------------------------------------------------------------
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
   if(iret == -2) return iret;      // bad alloc
   if(iret == -3) return iret;      // invalid command line

   // help: print syntax page and quit
   if(opts.hasHelp()) {
      LOG(INFO) << cmdlineUsage;
      return 1;
   }

   // extra parsing (perhaps add to cmdlineErrors, cmdlineExtras)
   iret = ExtraProcessing(cmdlineErrors, cmdlineExtras);
   if(iret == -4) return iret;      // log file could not be opened

   // pull out file name, sats and data tags
   if(cmdlineUnrecognized.size() > 0) {
      for(int i=cmdlineUnrecognized.size()-1; i >= 0; i--) {
         RinexSatID sat;
         string tag(cmdlineUnrecognized[i]);
         upperCase(tag);

         try {
            string filename(cmdlineUnrecognized[i]);
            ifstream ifstrm(filename.c_str());
            if(ifstrm.is_open()) {
               LOG(DEBUG) << "# Deduce filename >" << filename << "<";
               InputNavFiles.push_back(cmdlineUnrecognized[i]);
               cmdlineUnrecognized.erase(cmdlineUnrecognized.begin()+i);
               ifstrm.close();
               continue;
            }
            else {
               include_path(Navpath, filename);
               ifstrm.open(filename.c_str());
               if(ifstrm.is_open()) {
                  LOG(DEBUG) << "# Deduce filename >" << filename << "<";
                  InputNavFiles.push_back(cmdlineUnrecognized[i]);
                  cmdlineUnrecognized.erase(cmdlineUnrecognized.begin()+i);
                  ifstrm.close();
                  continue;
               }
            }
         } catch(Exception& e) { ; }

         // TEMP?
         LOG(DEBUG) << "Unrecognized >" << cmdlineUnrecognized[i] << "<";
      }
   }

   // output warning / error messages
   if(cmdlineUnrecognized.size() > 0) {
      LOG(WARNING) << "Warning - unrecognized arguments:";
      for(size_t i=0; i<cmdlineUnrecognized.size(); i++)
         LOG(WARNING) << "  " << cmdlineUnrecognized[i];
      LOG(WARNING) << "End of unrecognized arguments";
   }

   // fatal errors
   if(!cmdlineErrors.empty()) {
      stripTrailing(cmdlineErrors,'\n');
      replaceAll(cmdlineErrors,"\n","\n ");
      LOG(ERROR) << "Errors found on command line:\n " << cmdlineErrors
         << "\nEnd of command line errors.";
      return 1;
   }

   // success: dump configuration summary
   if(verbose) {
      ostringstream oss;
      oss << "------ Summary of " << PrgmName
         << " command line configuration ------\n";
      opts.DumpConfiguration(oss);
      if(!cmdlineExtras.empty()) oss << "# Extra Processing:\n" << cmdlineExtras;
      oss << "------ End configuration summary ------";
      LOG(VERBOSE) << oss.str();
   }

   return 0;

}  // end Configuration::CommandLine()

//------------------------------------------------------------------------------------
string Configuration::BuildCommandLine(void) throw()
{
   // Program description will appear at the top of the syntax page
   string PrgmDesc = " Program " + PrgmName +
" reads one or more RINEX (v.2+) navigation files and writes the merged\n"
" navigation data to one or more output (ver 2 or 3) files. A summary of\n"
" the ephemeris data may be written to the screen.\n"
"\n"
" Usage: " + PrgmName + " [options] <file>\n"
"   where <file> is an input RINEX navigation file, and options are as follows.\n"
"\n Options:";
   ;

   // options to appear on the syntax page, and to be accepted on command line
   //opts.Add(char, opt, arg, repeat?, required?, &target, pre-desc, desc);
   // NB cfgfile is a dummy, but it must exist when cmdline is processed.
   opts.Add('f', "file", "fn", true, false, &cfgfile,
            "# Input via configuration file:",
            "Name of file with more options [#->EOL = comment]");

   opts.Add(0, "nav", "file", true, false, &InputNavFiles,
            "# Required input [--nav is optional]",
            "Input RINEX navigation file name");
   opts.Add(0, "navpath", "p", false, false, &Navpath, "",
            "Path of input RINEX navigation file(s)");

   startStr = defaultstartStr;
   stopStr = defaultstopStr;
   opts.Add(0, "start", "t[:f]", false, false, &startStr,
            "# Editing (t,f are strings: time t; format f "
               "defaults to wk,sow OR yr,mon,day,h,m,s",
            "Start processing data at this epoch");
   opts.Add(0, "stop", "t[:f]", false, false, &stopStr, "",
            "Stop processing data at this epoch");
   opts.Add(0, "exSat", "sat", true, false, &exclSat, "",
            "Exclude satellite [system] from output [e.g. G17,R]");

   opts.Add(0, "out", "[sys:]fn", true, false, &outputStrs,
            "# Output [sys may be 1(G,R,E,S,C,J: R2 G,R only) or 3(GPS,...)-char]:",
            "Output [system <sys> only] to RINEX ver. 3 file fn");
   opts.Add(0, "out2", "[sys:]fn", true, false, &output2Strs, "",
            "Version 2 output [system <sys> only] to RINEX file fn");
   opts.Add(0, "timefmt", "fmt", false, false, &userfmt, "",
            "Format for time tags (see GPSTK::Epoch::printf) in output");
   opts.Add(0, "ver2", "", false, false, &outver2, "",
            "Write out RINEX version 2");
   opts.Add(0, "verbose", "", false, false, &verbose, "",
            "Print extra output information");
   opts.Add(0, "debug", "", false, false, &debug, "",
            "Print debug output at level 0 [debug<n> for level n=1-7]");
   opts.Add(0, "help", "", false, false, &help, "",
            "Print this syntax page, and quit");

   return PrgmDesc;

}  // end Configuration::BuildCommandLine()

//------------------------------------------------------------------------------------
int Configuration::ExtraProcessing(string& errors, string& extras) throw()
{
   int n;
   size_t i;
   vector<string> fld;
   ostringstream oss,ossx;       // oss for Errors, ossx for Warnings and info

   // start and stop times
   for(i=0; i<2; i++) {
      static const string fmtGPS("%F,%g"),fmtCAL("%Y,%m,%d,%H,%M,%S");
      msg = (i==0 ? startStr : stopStr);
      if(msg == (i==0 ? defaultstartStr : defaultstopStr)) continue;

      bool ok(true);
      bool hasfmt(msg.find('%') != string::npos);
      n = numWords(msg,',');
      if(hasfmt) {
         fld = split(msg,':');
         if(fld.size() != 2) { ok = false; }
         else try {
            Epoch ep;
            stripLeading(fld[0]," \t");
            stripLeading(fld[1]," \t");
            ep.scanf(fld[0],fld[1]);
            (i==0 ? beginTime : endTime) = static_cast<CommonTime>(ep);
         }
         catch(Exception& e) { ok = false; LOG(INFO) << "excep " << e.what(); }
      }
      else if(n == 2 || n == 6) {        // try the defaults
         try {
            Epoch ep;
            ep.scanf(msg,(n==2 ? fmtGPS : fmtCAL));
            (i==0 ? beginTime : endTime) = static_cast<CommonTime>(ep);
         }
         catch(Exception& e) { ok = false; LOG(INFO) << "excep " << e.what(); }
      }

      if(ok) {
         msg = printTime((i==0 ? beginTime : endTime),fmtGPS+" = "+fmtCAL);
         if(msg.find("Error") != string::npos) ok = false;
      }

      if(!ok)
         oss << "Error : invalid time or format in --" << (i==0 ? "start" : "stop")
            << " " << (i==0 ? startStr : stopStr) << endl;
      else
         ossx << (i==0 ? "   Begin time --begin" : "   End time --end") << " is "
            << printTime((i==0 ? beginTime : endTime), fmtGPS+" = "+fmtCAL+"\n");
   }

   // output file names
   for(i=0; i<outputStrs.size(); i++) {
      msg = outputStrs[i];
      fld = split(msg,':');
      if(fld.size() <= 0 || fld.size() > 2) {
         oss << "Error : invalid --out argument : " << msg;
         continue;
      }
      if(fld.size() == 1)
         mapSysOutputFile[string("")] = fld[0];
      else {
         upperCase(fld[0]);
         if(fld[0] == "GPS" || fld[0] == "G") mapSysOutputFile[string("G")] = fld[1];
         if(fld[0] == "GLO" || fld[0] == "R") mapSysOutputFile[string("R")] = fld[1];
         if(fld[0] == "GAL" || fld[0] == "E") mapSysOutputFile[string("E")] = fld[1];
         if(fld[0] == "GEO" || fld[0] == "SBAS" || fld[0] == "S")
                                              mapSysOutputFile[string("S")] = fld[1];
         if(fld[0] == "BDS" || fld[0] == "C") mapSysOutputFile[string("C")] = fld[1];
         if(fld[0] == "QZS" || fld[0] == "J") mapSysOutputFile[string("J")] = fld[1];
         LOG(VERBOSE) << "Input system " << fld[0] << " and output file " << fld[1];
      }
   }
   for(i=0; i<output2Strs.size(); i++) {
      msg = output2Strs[i];
      fld = split(msg,':');
      if(fld.size() <= 0 || fld.size() > 2) {
         oss << "Error : invalid --out2 argument : " << msg;
         continue;
      }
      if(fld.size() == 1)
         mapSysOutput2File[string("")] = fld[0];
      else {
         upperCase(fld[0]);
         if(fld[0] == "GPS" || fld[0] == "G") mapSysOutput2File[string("G")] = fld[1];
         if(fld[0] == "GLO" || fld[0] == "R") mapSysOutput2File[string("R")] = fld[1];
         if(fld[0] == "GAL" || fld[0] == "E") mapSysOutput2File[string("E")] = fld[1];
         if(fld[0] == "GEO" || fld[0] == "SBAS" || fld[0] == "S")
                                              mapSysOutput2File[string("S")] = fld[1];
         if(fld[0] == "BDS" || fld[0] == "C") mapSysOutput2File[string("C")] = fld[1];
         if(fld[0] == "QZS" || fld[0] == "J") mapSysOutput2File[string("J")] = fld[1];
      }
   }

   // add new errors to the list
   msg = oss.str();
   if(!msg.empty()) errors += msg;
   msg = ossx.str();
   if(!msg.empty()) extras += msg;

   return 0;

} // end Configuration::ExtraProcessing(string& errors) throw()

//------------------------------------------------------------------------------------
// Return 0 ok, >0 number of files successfully read, <0 fatal error
int ProcessFiles(void) throw(Exception)
{
try {
   Configuration& C(Configuration::Instance());
   int nread;
   Rinex3NavHeader Rheadout,Rhead;

   size_t nfiles(0);
   for(size_t nfile=0; nfile<C.InputNavFiles.size(); nfile++) {
      
      string filename(C.InputNavFiles[nfile]);

      // load filename
      nread = C.NavStore.loadFile(filename,(C.debug>-1),LOGstrm);
      if(nread == -1) {        // -1 failed to open file
         LOG(WARNING) << C.NavStore.what;
         continue;
      }
      else if(nread == -2) {   // -2 failed to read header
         LOG(WARNING) << "Warning : Failed to read header: " << C.NavStore.what
            << "\n Header dump follows.";
         C.NavStore.Rhead.dump(LOGstrm);
         continue;
      }
      else if(nread == -3) {   // -3 failed to read data
         LOG(WARNING) << " Warning : Failed to read nav data (Exception "
            << C.NavStore.what << "); dump follows.";
         C.NavStore.Rdata.dump(LOGstrm);
         continue;
      }
      else nfiles++;

      LOG(VERBOSE) << "Opened input file " << filename;

      Rhead = C.NavStore.Rhead;

      // save header
      if(nfiles == 1) {
         Rheadout = Rhead; // C.NavStore.Rhead;
         Rheadout.fileProgram = C.PrgmName + string(" ") + Version;
      }

      // merge headers
      else {
         // add Time Correction records from Rhead to Rheadout
         map<string,TimeSystemCorrection>::iterator tcit;
         for(tcit=C.NavStore.Rhead.mapTimeCorr.begin();
               tcit != C.NavStore.Rhead.mapTimeCorr.end(); ++tcit)
         {
            if(Rheadout.mapTimeCorr.find(tcit->first) == Rheadout.mapTimeCorr.end()) {
               Rheadout.mapTimeCorr[tcit->first] = tcit->second;
               Rheadout.valid |= Rinex3NavHeader::validTimeSysCorr;
            }
         }

         // add Iono Correction records from Rhead to Rheadout
         map<string, IonoCorr>::iterator icit;
         for(icit=C.NavStore.Rhead.mapIonoCorr.begin();
               icit != C.NavStore.Rhead.mapIonoCorr.end(); ++icit)
         {
            if(Rheadout.mapIonoCorr.find(icit->first) == Rheadout.mapIonoCorr.end()) {
               Rheadout.mapIonoCorr[icit->first] = icit->second;
               if(icit->second.type == IonoCorr::GPSA
                  || icit->second.type == IonoCorr::GPSB)
                     Rheadout.valid |= Rinex3NavHeader::validIonoCorrGPS;
               else
                  Rheadout.valid |= Rinex3NavHeader::validIonoCorrGal;
            }
         }
      }

   }  // end loop over files

   if(nfiles == 0) {
      LOG(WARNING) << "Warning - Failed to read any files.";
      return -1;
   }
   else
      LOG(INFO) << "Read " << nfiles << " input RINEX Nav files.\n";

   // dump store
   C.NavStore.dump(LOGstrm,(C.debug>-1 ? 1 : 0));

   // stay within time limits
   C.NavStore.edit(C.beginTime,C.endTime);

   // how many do we have?
   int Neph(C.NavStore.size());
   int NGPS(C.NavStore.size(SatID::systemGPS));
   int NGLO(C.NavStore.size(SatID::systemGlonass));
   int NGAL(C.NavStore.size(SatID::systemGalileo));
   int NGEO(C.NavStore.size(SatID::systemGeosync));
   int NBDS(C.NavStore.size(SatID::systemBeiDou));
   int NQZS(C.NavStore.size(SatID::systemQZSS));
   LOG(VERBOSE) << "\nRinNav has stored " << Neph << " navigation records.";
   if(NGPS) LOG(VERBOSE) <<"RinNav has stored " << NGPS << " GPS navigation records.";
   if(NGLO) LOG(VERBOSE) <<"RinNav has stored " << NGLO << " GLO navigation records.";
   if(NGAL) LOG(VERBOSE) <<"RinNav has stored " << NGAL << " GAL navigation records.";
   if(NGEO) LOG(VERBOSE) <<"RinNav has stored " << NGEO << " GEO navigation records.";
   if(NBDS) LOG(VERBOSE) <<"RinNav has stored " << NBDS << " BDS navigation records.";
   if(NQZS) LOG(VERBOSE) <<"RinNav has stored " << NQZS << " QZS navigation records.";

   int Nsys(0);
   if(NGPS > 0) Nsys++;
   if(NGLO > 0) Nsys++;
   if(NGAL > 0) Nsys++;
   if(NGEO > 0) Nsys++;
   if(NBDS > 0) Nsys++;
   if(NQZS > 0) Nsys++;

   // dump merged header
   LOG(VERBOSE) << "\nOutput (merged) header:";
   if(C.verbose) Rheadout.dump(LOGstrm);

   // get full list of Rinex3NavData
   list<Rinex3NavData> theList,theFullList;
   list<Rinex3NavData>::const_iterator listit;
   C.NavStore.addToList(theFullList);

   // N... is what was read; n... will be what is kept
   int neph(0), nGPS(0), nGLO(0), nGAL(0), nGEO(0), nBDS(0), nQZS(0);

   // must edit out any excluded sats
   if(C.exclSat.size() > 0) {
      for(listit = theFullList.begin(); listit != theFullList.end(); ++listit) {
         // skip excluded sats/systems
         if(vectorindex(C.exclSat,listit->sat) != -1 ||
            vectorindex(C.exclSat,RinexSatID(-1,listit->sat.system)) != -1)
               continue;
         // keep it
         theList.push_back(*listit);
         // count it
         neph++;
         switch(listit->sat.system) {
            case SatID::systemGPS: nGPS++; break;
            case SatID::systemGlonass: nGLO++; break;
            case SatID::systemGalileo: nGAL++; break;
            case SatID::systemGeosync: nGEO++; break;
            case SatID::systemBeiDou:  nBDS++; break;
            case SatID::systemQZSS:    nQZS++; break;
            default: break;
         }
      }
   }
   else {
      neph = Neph;
      nGPS = NGPS; nGLO = NGLO; nGAL = NGAL;
      nGEO = NGEO; nBDS = NBDS; nQZS = NQZS;
      theList = theFullList;
   }

   int nsys(0);
   if(nGPS > 0) nsys++;
   if(nGLO > 0) nsys++;
   if(nGAL > 0) nsys++;
   if(nGEO > 0) nsys++;
   if(nBDS > 0) nsys++;
   if(nQZS > 0) nsys++;
   if(nsys == 0 || neph == 0) {
      LOG(WARNING) << "Warning - no data to output.";
      return nfiles;
   }
   else LOG(INFO) << " Found " << nsys << " systems and " << neph << " records.";

   // sort on time, then sat
   theList.sort();

   // dump
   LOG(VERBOSE) << "Dump records to be written";
   if(C.verbose)
      for(listit = theList.begin(); listit != theList.end(); ++listit)
         LOG(VERBOSE) << listit->dumpString();

   if(C.mapSysOutputFile.size() == 0)
      LOG(INFO) << "No output of RINEX 3 Navigation data selected.";

   // output store to file(s) -----------------------------------------------
   // version 3
   map<string, string>::const_iterator it;
   for(it = C.mapSysOutputFile.begin(); it != C.mapSysOutputFile.end(); ++it) {
      string sys(it->first);
      string filename(it->second);
      neph = 0;

      Rinex3NavStream ostrm;
      ostrm.open(filename.c_str(),ios::out);
      if(!ostrm.is_open()) {
         LOG(ERROR) << "Error : could not open output file " << filename;
         return -1;
      }
      ostrm.exceptions(ios::failbit);

      LOG(INFO) << " Opened output RINEX ver 3 file " << filename;

      // prepare header
      Rinex3NavHeader rhead(Rheadout);
      // set version; NB set the version before calling setFileSystem()
      if(rhead.version < 3.02)
         rhead.version = 3.02;  // this necessary? shouldn't Rinex3NavHeader do it?

      // reset the file system
      // set system to mixed if more than one store
      if(sys == "") {                        // check that output really mixed
         if(nsys > 1) sys = string("M");
         else if(nGPS > 0) sys = string("G");
         else if(nGLO > 0) sys = string("R");
         else if(nGAL > 0) sys = string("E");
         else if(nGEO > 0) sys = string("S");
         else if(nBDS > 0) sys = string("C");
         else if(nQZS > 0) sys = string("J");
      }
      rhead.setFileSystem(sys);

      // write the header
      ostrm << rhead;

      LOG(DEBUG) << "Dump records to be written";
      for(listit = theList.begin(); listit != theList.end(); ++listit) {
         if(listit->sat.system != rhead.fileSysSat.system) continue;
         if(C.debug > -1) listit->dump(LOGstrm);     // dump it
         ostrm << *listit;             // write it
         neph++;
      }

      ostrm.close();

      //if(sys == "M") LOG(VERBOSE) << "Had " << neph << " records for all systems";
      if(sys == "G") LOG(VERBOSE) << "Had " << nGPS << " records for GPS";
      if(sys == "R") LOG(VERBOSE) << "Had " << nGLO << " records for GLO";
      if(sys == "E") LOG(VERBOSE) << "Had " << nGAL << " records for GAL";
      if(sys == "S") LOG(VERBOSE) << "Had " << nGEO << " records for GEO";
      if(sys == "C") LOG(VERBOSE) << "Had " << nBDS << " records for BDS";
      if(sys == "J") LOG(VERBOSE) << "Had " << nQZS << " records for QZS";
      LOG(VERBOSE) << "Wrote " << neph << " records to RINEX ver 3 file " << filename;
   }

   if(C.mapSysOutput2File.size() == 0)
      LOG(INFO) << "No output of RINEX 2 Navigation data selected.";

   // version 2
   for(it = C.mapSysOutput2File.begin(); it != C.mapSysOutput2File.end(); ++it) {
      string sys(it->first);
      string filename(it->second);

      Rinex3NavStream ostrm;
      ostrm.open(filename.c_str(),ios::out);
      if(!ostrm.is_open()) {
         LOG(ERROR) << "Error : could not open output file " << filename;
         return -1;
      }
      ostrm.exceptions(ios::failbit);

      LOG(INFO) << "Opened output RINEX ver 2 file " << filename;

      // prepare header
      Rinex3NavHeader rhead(Rheadout);
      // set version; NB set the version before calling setFileSystem()
      rhead.version = 2.11;

      // reset the file system
      // set system to mixed if more than one store
      if(sys == "" && nsys > 1) {
         LOG(WARNING) << "Warning - cannot write a mixed-system RINEX ver 2 file";
         continue;
      }

      if(sys == "") {                        // check that output really mixed
         //if(nsys > 1) sys = string("M");
         if(nGPS > 0) sys = string("G");
         else if(nGLO > 0) sys = string("R");
         else if(nGAL > 0) sys = string("E");
         else if(nGEO > 0) sys = string("S");
         else if(nBDS > 0) sys = string("C");
         else if(nQZS > 0) sys = string("J");
      }
      rhead.setFileSystem(sys);

      // write the header
      ostrm << rhead;

      LOG(VERBOSE) << "Dump records to be written";
      for(listit = theList.begin(); listit != theList.end(); ++listit) {
         if(C.verbose) listit->dump(LOGstrm); // dump it
         ostrm << *listit;                    // write it
      }

      ostrm.close();

      //if(sys == "M") LOG(VERBOSE) << "Wrote " << neph << " records for all systems";
      if(sys == "G") LOG(VERBOSE) << "Wrote " << nGPS << " records for GPS";
      if(sys == "R") LOG(VERBOSE) << "Wrote " << nGLO << " records for GLO";
      if(sys == "E") LOG(VERBOSE) << "Wrote " << nGAL << " records for GAL";
      if(sys == "S") LOG(VERBOSE) << "Wrote " << nGEO << " records for GEO";
      if(sys == "C") LOG(VERBOSE) << "Wrote " << nBDS << " records for BDS";
      if(sys == "C") LOG(VERBOSE) << "Wrote " << nQZS << " records for QZS";
      LOG(VERBOSE) << "Wrote " << neph << " records to RINEX ver 2 file " << filename;
   }

   return nfiles;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end ProcessFiles()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
