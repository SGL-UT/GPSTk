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

//------------------------------------------------------------------------------------
// DiscFix.cpp Read a RINEX observation file containing dual frequency
//    pseudorange and phase, separate the data into satellite passes, and then
//    find and estimate discontinuities in the phase (using the GPSTk Discontinuity
//    Corrector (GDC) in DiscCorr.hpp).
//    The corrected data can be written out to another RINEX file, plus there is the
//    option to smooth the pseudorange and/or debias the phase (SatPass::smooth()).
//------------------------------------------------------------------------------------

/// @file DiscFix.cpp
/// Correct phase discontinuities (cycle slips) in dual frequency data in a RINEX
/// observation file, plus optionally smooth the pseudoranges and/or debias the phases

// system
#include <ctime>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
// gpstk
#include "MathBase.hpp"
#include "RinexSatID.hpp"
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "CommonTime.hpp"
#include "CivilTime.hpp"
#include "GPSWeekSecond.hpp"
#include "Epoch.hpp"
#include "TimeString.hpp"
#include "StringUtils.hpp"
// geomatics
#include "logstream.hpp"
#include "stl_helpers.hpp"
#include "expandtilde.hpp"
#include "CommandLine.hpp"
#include "SatPass.hpp"
#include "SatPassUtilities.hpp"
#include "DiscCorr.hpp"

using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// prgm data
static const string DiscFixVersion = string("6.3 2/4/16");
static const string PrgmName("DiscFix");
// a convenience
static const string L1("L1"),L2("L2"),P1("P1"),P2("P2"),C1("C1"),C2("C2");

// all input and global data
typedef struct configuration {
      // input
   string inputPath;
   vector<string> obsfiles;
      // data flow
   double decimate;
   Epoch begTime, endTime;
   double MaxGap;
   //int MinPts;
      // processing
   double dt0,dt;          // data interval in input file, and final (decimated) dt
   bool noCA1,noCA2,useCA1,forceCA1,useCA2,forceCA2,doGLO;
   vector<RinexSatID> exSat;
   RinexSatID SVonly;
      // output files
   string LogFile,OutFile;
   ofstream oflog,ofout;
   string format;
   int round;
      // output
   string OutRinexObs;
   string HDPrgm;         // header of output RINEX file
   string HDRunby;
   string HDObs;
   string HDAgency;
   string HDMarker;
   string HDNumber;
   int NrecOut;
   Epoch FirstEpoch,LastEpoch;
   bool smoothPR,smoothPH,smooth;
   int debug;
   bool verbose,DChelp;
   vector<string> DCcmds;        // all the --DC... on the cmd line
      // estimate dt from data
   double estdt[9];
   int ndt[9];
   // input and/or compute GLONASS frequency channel for each GLO satellite
   map<RinexSatID,int> GLOfreqChannel;

   // summary of cmd line input
   string cmdlineSum;

   string Title,Date;
   Epoch PrgmEpoch;
   RinexObsStream irfstr; //, orfstr;   // input and output RINEX files
   RinexObsHeader rhead;
   int inP1,inP2,inL1,inL2;            // indexes in rhead of C1/P1, P2, L1 and L2
   string P1C1,P2C2;                   // either P1 or C1

   // Data for an entire pass is stored in SatPass object
   // This vector contains all the SatPass's defined so far
   // The parallel vector holds an iterator for use in writing out the data
   vector<SatPass> SPList;

   // list of observation types to be included in each SatPass
   vector<string> obstypes;

   // this is a map relating a satellite to the index in SVPList of the current pass
   vector<unsigned int> SPIndexList;
   map<RinexSatID,int> SatToCurrentIndexMap;

   GDCconfiguration GDConfig;       // the discontinuity corrector configuration
} DFConfig;

// declare (one only) global configuration object
DFConfig cfg;

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// prototypes
/**
 * @throw Exception
 */
int GetCommandLine(int argc, char **argv);
/**
 * @throw Exception
 */
void DumpConfiguration(void);
/**
 * @throw Exception
 */
int Initialize(void);
/**
 * @throw Exception
 */
int ShallowCheck(void);  // called by Initialize()
/**
 * @throw Exception
 */
int WriteToRINEX(void);
void PrintSPList(ostream&, string, vector<SatPass>&);

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
   try {
      clock_t totaltime = clock();
      int i,nread,npass,iret;
      Epoch ttag;
      string msg;
      vector<string> EditCmds;

      // Title and description
      cfg.Title = PrgmName+", part of the GPS ToolKit, Ver "+DiscFixVersion+", Run ";
      cfg.PrgmEpoch.setLocalTime();
      cfg.Date = printTime(cfg.PrgmEpoch,"%04Y/%02m/%02d %02H:%02M:%02S");
      cfg.Title += cfg.Date;
      cout << cfg.Title << endl;

      for(;;) {                           // a convenience
         // -------------------------------- get command line
         iret = GetCommandLine(argc, argv);
         if(iret) break;

         // -------------------------------- initialize
         iret = Initialize();
         if(iret) break;

         // -------------------------------- read in the data
         try {
            nread = SatPassFromRinexFiles(cfg.obsfiles, cfg.obstypes, cfg.dt0,
                              cfg.SPList, cfg.exSat, true, cfg.begTime, cfg.endTime);
            LOG(VERBOSE) << "Successfully read " << nread << " RINEX obs files.";
         }
         catch(Exception &e) {         // time tags out of order or a read error
            string what(e.what());
            string::size_type pos(what.find("Time tags out of order", 0));
            if(pos != string::npos) {
               pos = what.find("\n");
               if(pos != string::npos) what.erase(pos);
               LOG(ERROR) << "Error - " << what;
            }
            else { GPSTK_RETHROW(e); }
         }

         if(nread != cfg.obsfiles.size()) {
            iret = -7;
            break;
         }
         if(cfg.SPList.size() <= 0) {
            LOG(ERROR) << "Error - no data found.";
            iret = -8;
            break;
         }

         // -------------------------------- exclude satellites
         for(npass=0; npass<cfg.SPList.size(); npass++) {
            RinexSatID sat(cfg.SPList[npass].getSat());

            if(cfg.SVonly.id != -1 && sat != cfg.SVonly) {
               cfg.SPList[npass].status() = -1;
               LOG(VERBOSE) << "Exclude pass #" << setw(2) << npass+1 << " (" << sat
                  << ") as only one satellite is to be processed.";
            }
            // done in SatPassFromRinex()
            //else if(vectorindex(cfg.exSat,sat) != -1) {
            //   cfg.SPList[npass].status() = -1;
            //   LOG(VERBOSE) << "Exclude pass #" << setw(2) << npass+1 << " (" << sat
            //      << ") as satellite is excluded explicitly.";
            //}
            //else if((!cfg.doGLO && sat.system != SatelliteSystem::GPS) ||
            //        ( cfg.doGLO && sat.system != SatelliteSystem::GPS
            //                    && sat.system != SatelliteSystem::Glonass))
            //{
            //   cfg.SPList[npass].status() = -1;
            //   LOG(VERBOSE) << "Exclude pass #" << setw(2) << npass+1 << " (" << sat
            //      << ") as satellite system is excluded.";
            //}
            else if(cfg.SPList[npass].size()==0 || cfg.SPList[npass].getNgood()==0) {
               cfg.SPList[npass].status() = -1;
               LOG(VERBOSE) << "Exclude pass #" << setw(2) << npass+1 << " (" << sat
                  << ") as it is empty.";
            }
            //else if(cfg.SPList[npass].getNgood() < minpass) {
            //   cfg.SPList[npass].status() = -1;
            //   LOG(VERBOSE) << "Exclude pass #" << setw(2) << npass+1 << " (" << sat
            //      << ") as it is too small (" << cfg.SPList[npass].getNgood()
            //      << " < " << minpass << ").";
            //}

         }  // end for() over all passes

         // remove the invalid ones
         vector<SatPass>::iterator it(cfg.SPList.begin());
         while(it != cfg.SPList.end()) {
            if(it->status() == -1)
               it = cfg.SPList.erase(it);       // remove it; it points to next pass
            else
               ++it;                            // go to next pass
         }

         // is there anything left?
         if(cfg.SPList.size() <= 0) {
            LOG(ERROR) << "Error - no data found.";
            iret = -9;
            break;
         }

         // -------------------------------- decimate
         // set the data interval, and decimate if the user input is N*raw interval
         if(cfg.decimate < 0.0) {
            LOG(INFO) << PrgmName << ": decimation timestep must be positive";
            iret = -2;
            break;
         }
         else if(cfg.decimate == 0.0) {
            cfg.dt = cfg.dt0;                         // just go with raw interval
         }
         else if(fmod(cfg.decimate,cfg.dt0) < 0.01) { // decimate
            int N(0.5+cfg.decimate/cfg.dt0);
            ttag = cfg.SPList[0].getFirstTime();
            int n(ttag.GPSsow()/cfg.decimate);
            //ttag.setGPSfullweek(ttag.GPSfullweek(), n*cfg.decimate);
            GPSWeekSecond gpst(ttag.GPSweek(), n*cfg.decimate);
            ttag = static_cast<Epoch>(gpst);
            for(npass=0; npass<cfg.SPList.size(); npass++)
               cfg.SPList[npass].decimate(N, ttag);
            cfg.dt = cfg.decimate;
         }
         else {                                       // can't decimate
            LOG(ERROR) << "Error - cannot decimate; input time step ("
               << asString(cfg.decimate,2)
               << ") is not an even multiple of the data rate ("
               << asString(cfg.dt0,2) << ")";
            iret = -10;
            break;
         }

         cfg.GDConfig.setParameter(string("DT:")+asString(cfg.dt,2));
         cfg.GDConfig.setParameter(string("MaxGap:")+asString(cfg.MaxGap,2));
         LOG(INFO) << "\nHere is the current GPSTk DC configuration:";
         cfg.GDConfig.DisplayParameterUsage(LOGstrm,(cfg.DChelp && cfg.verbose));
         LOG(INFO) << "";

         // -------------------------------- call the GDC, output results and smooth
         for(npass=0; npass<cfg.SPList.size(); npass++) {

            LOG(INFO) << "Proc " << setw(2) << npass+1 << " " << cfg.SPList[npass];
            //cfg.SPList[npass].dump(*pLOGstrm,"RAW");      // temp

            msg = "";
            iret=DiscontinuityCorrector(cfg.SPList[npass],cfg.GDConfig,EditCmds,msg);
            if(iret != 0) {
               cfg.SPList[npass].status() = -1;         // failed
               LOG(ERROR) << "GDC failed (" << iret << " "
                  << (iret==-1 ? "Singularity":
                     (iret==-3 ? "DT not set, or memory":
                     (iret==-4 ? "No data":"Bad input")))
                  << ") for pass "
                  << npass+1 << " :\n" << msg;
               continue;
            }
            //if(cfg.verbose && LOGlevel < ConfigureLOG::Level("VERBOSE"))
            LOG(INFO) << msg;

            ttag = cfg.SPList[npass].getFirstGoodTime();
            if(ttag < cfg.FirstEpoch) cfg.FirstEpoch = ttag;
            ttag = cfg.SPList[npass].getLastTime();
            if(ttag > cfg.LastEpoch) cfg.LastEpoch = ttag;

            // output editing commands
            for(i=0; i<EditCmds.size(); i++)
               cfg.ofout << EditCmds[i] << " # pass " << npass+1 << endl;
            EditCmds.clear();

            // smooth pseudorange and debias phase
            if(cfg.smooth) {
               cfg.SPList[npass].smooth(cfg.smoothPR, cfg.smoothPH, msg);
               LOG(INFO) << msg;
            }

         }  // end for() loop over passes

         // -------------------------------- write to RINEX
         iret = WriteToRINEX();
         if(iret) break;

         // -------------------------------- print a summary
         PrintSPList(LOGstrm,"Fine",cfg.SPList);

         break;
      }

      // timing
      totaltime = clock()-totaltime;
      LOG(INFO) << PrgmName << " timing: " << fixed << setprecision(3)
         << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.\n";
      cout << PrgmName << " timing: " << fixed << setprecision(3)
         << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.\n";

      // clean up
      cfg.ofout.close();
      cfg.oflog.close();

      return iret;
   }
   catch(Exception& e) {
      cfg.oflog << e.what();
      cout << e.what();
   }
   catch (...) {
      cfg.oflog << PrgmName << ": Unknown error.  Abort." << endl;
      cout << PrgmName << ": Unknown error.  Abort." << endl;
   }

   return -1;

}   // end main()

//------------------------------------------------------------------------------------
int Initialize(void)
{
try {
   int i;

   // open the log file
   cfg.oflog.open(cfg.LogFile.c_str(),ios::out);
   if(!cfg.oflog.is_open()) {
      cerr << PrgmName << " failed to open log file " << cfg.LogFile << ".\n";
      return -3;
   }

   // last write to screen
   LOG(INFO) << PrgmName << " is writing to log file " << cfg.LogFile;

   // attach LOG to the log file
   pLOGstrm = &cfg.oflog;
   ConfigureLOG::ReportLevels() = false;
   ConfigureLOG::ReportTimeTags() = false;

   // set the DC commands now (setParameter may write to log file)
   for(i=0; i<cfg.DCcmds.size(); i++) {
      cfg.GDConfig.setParameter(cfg.DCcmds[i]);
      if(cfg.DCcmds[i].substr(0,5) == string("Debug")) {
         string msg("DEBUG");
         msg += asString(cfg.DCcmds[i].substr(6));
         LOGlevel = ConfigureLOG::Level(msg);
      }
      else if(cfg.DCcmds[i].substr(0,4) == string("--DC DT=<dt>")) {
         LOG(WARNING) << "Warning - Input of the timestep with --DCDT is ignored.";
      }
   }

   if(cfg.verbose && LOGlevel < ConfigureLOG::Level("VERBOSE"))
      LOGlevel = ConfigureLOG::Level("VERBOSE");

   LOG(INFO) << cfg.Title;
   //LOG(INFO) << "LOG level is at " << ConfigureLOG::ToString(LOGlevel);

   // open input obs files, read header and some of the data
   i = ShallowCheck();
   if(i) return i;

   // allow GDC to output to log file
   cfg.GDConfig.setDebugStream(cfg.oflog);
   if(cfg.P1C1 == C1) cfg.GDConfig.setParameter("useCA1:1");   // Shallow sets P1C1
   if(cfg.P2C2 == C2) cfg.GDConfig.setParameter("useCA2:1");   // Shallow sets P2C2

   cfg.SVonly.setfill('0');                     // set fill char in RinexSatID

   // catch input trap
   if(!cfg.doGLO && cfg.SVonly.system == SatelliteSystem::Glonass) {
      LOG(VERBOSE) << "SVonly is GLONASS - turn on processing of GLONASS";
      cfg.doGLO = true;
   }

   // write to log file
   DumpConfiguration();

   cfg.FirstEpoch = CommonTime::END_OF_TIME;
   cfg.LastEpoch = CommonTime::BEGINNING_OF_TIME;
   // configure SatPass
   {
      cfg.obstypes.push_back(L1);    // DiscFix requires these 4 observables only
      cfg.obstypes.push_back(L2);
      cfg.obstypes.push_back(cfg.P1C1);
      cfg.obstypes.push_back(P2);

      SatPass dummy(cfg.SVonly,cfg.dt0);
      dummy.setMaxGap(cfg.MaxGap);
      dummy.setOutputFormat(cfg.format,cfg.round);
   }

   // open output file
   // output for editing commands - write to this in ProcessSatPass()
   cfg.ofout.open(cfg.OutFile.c_str());
   if(!cfg.oflog.is_open()) {
      cfg.oflog << "Error: " << PrgmName << " failed to open output file "
         << cfg.OutFile << endl;
      return -5;
   }
   else
      LOG(INFO) << PrgmName << " is writing to output file " << cfg.OutFile;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
// open the input files, read the headers and some of the data. Determine dt0 & C1/P1.
int ShallowCheck(void)
{
try {
   bool inputValid(true);
   int i,j;
   string msg;
   vector<bool> fileValid;
   vector<int> fileHasP1C1,fileHasP2C2;
   vector<long> filesize;
   vector<double> fileDT;
   vector<Epoch> fileFirst;

   // open obs files and read few epochs; test validity and determine content
   for(i=0; i<cfg.obsfiles.size(); i++) {
      fileValid.push_back(false);
      fileHasP1C1.push_back(0);           // 0: don't know, 1 C1, 2 P1, 3 both
      fileHasP2C2.push_back(0);           // 0: don't know, 1 C2, 2 P2, 3 both
      filesize.push_back(long(0));
      fileDT.push_back(-1.0);
      fileFirst.push_back(CommonTime::BEGINNING_OF_TIME);

      // open obs files
      RinexObsStream rstrm;
      rstrm.open(cfg.obsfiles[i].c_str(),ios_base::in);
      if(!rstrm.is_open()) {
         LOG(ERROR) << "  Error - Observation file " << cfg.obsfiles[i]
             << " could not be opened.";
         inputValid = false;
      }
      else {
         LOG(DEBUG) << "Opened file " << cfg.obsfiles[i] << flush;
         rstrm.exceptions(ios_base::failbit);

         // get file size
         long begin = rstrm.tellg();
         rstrm.seekg(0,ios::end);
         long end = rstrm.tellg();
         rstrm.seekg(0,ios::beg);
         filesize[i] = end-begin;

         // read header
         bool rinexok=true;
         //RinexObsHeader head;
         try { rstrm >> cfg.rhead; }
         catch(Exception& e) { rinexok = false; }
         catch(exception& e) { rinexok = false; }

         // is header valid?
         LOG(DEBUG) << "Read header for " << cfg.obsfiles[i];
         if(rinexok && !cfg.rhead.isValid()) { rinexok = false; }
         if(!rinexok) {
            LOG(ERROR) << "  Error - Observation file " << cfg.obsfiles[i]
                << " does not contain valid RINEX observations.";
            inputValid = false;
         }
         // look for L1,L2,C1/P1,P2 observations, and antenna height
         else {
            unsigned int found=0;
            for(j=0; j<cfg.rhead.obsTypeList.size(); j++) {
               if(cfg.rhead.obsTypeList[j] == RinexObsHeader::L1) found +=32;
               if(cfg.rhead.obsTypeList[j] == RinexObsHeader::L2) found +=16;  // 48
               if(cfg.rhead.obsTypeList[j] == RinexObsHeader::P1) found += 8;  // 56
               if(cfg.rhead.obsTypeList[j] == RinexObsHeader::P2) found += 4;  // 60
               if(cfg.rhead.obsTypeList[j] == RinexObsHeader::C1) found += 2;  // 62
               if(cfg.rhead.obsTypeList[j] == RinexObsHeader::C2) found += 1;  // 63
            }
            if(found & 8) fileHasP1C1[i] += 2;     // has P1
            if(found & 2) fileHasP1C1[i] += 1;     // has C1
            if(found & 4) fileHasP2C2[i] += 2;     // has P2
            if(found & 1) fileHasP2C2[i] += 1;     // has C2
            if(!(found & 32)) LOG(ERROR) << "  Error - Observation file "
                  << cfg.obsfiles[i] << " has no L1 data.";
            if(!(found & 16)) LOG(ERROR) << "  Error - Observation file "
                  << cfg.obsfiles[i] << " has no L2 data.";
            if(fileHasP1C1[i] == 0) LOG(ERROR) << "  Error - Observation file "
                  << cfg.obsfiles[i] << " has no P1 or C1 data.";
            if(fileHasP2C2[i] == 0) LOG(ERROR) << "  Error - Observation file "
                  << cfg.obsfiles[i] << " has no P2 or C2 data.";

            if(!(found & 48) || fileHasP1C1[i]*fileHasP2C2[i] == 0) {
               inputValid = false;
            }
            else {
               fileFirst[i] = cfg.rhead.firstObs;

               // read a few obs to determine data interval
               const int N=10;      // how many epochs to read?
               begin = rstrm.tellg();
               int jj,kk,nleast,nepochs=0,ndt[9]={-1,-1,-1, -1,-1,-1, -1,-1,-1};
               double dt,bestdt[9];
               Epoch first=CommonTime::END_OF_TIME,prev=CommonTime::END_OF_TIME;
               RinexObsData robs;
               while(1) {
                  try { rstrm >> robs; }
                  catch(Exception& e) { break; }   // simply quit if meet failure
                  if(!rstrm) break;                // or EOF
                  dt = robs.time - prev;
                  if(dt > 0.0) {
                     for(j=0; j<9; j++) {
                        if(ndt[j] <= 0) { bestdt[j]=dt; ndt[j]=1; break; }
                        if(fabs(dt-bestdt[j]) < 0.002) { ndt[j]++; break; }
                        if(j == 8) {
                           kk=0; nleast=ndt[kk];
                           for(jj=1; jj<9; jj++) if(ndt[jj] <= nleast) {
                              kk=jj; nleast=ndt[jj];
                           }
                           ndt[kk]=1; bestdt[kk]=dt;
                        }
                     }
                  }
                  if(++nepochs >= N) break;
                  prev = robs.time;
                  if(first == CommonTime::END_OF_TIME) first = robs.time;
               }

               // save the results
               for(jj=1,kk=0; jj<9; jj++) if(ndt[jj]>ndt[kk]) kk=jj;
               // round to nearest 0.1 second
               fileDT[i] = double(0.1*int(0.5+bestdt[kk]/0.1));
               fileValid[i] = true;

               // dump the results
               LOG(VERBOSE) << " RINEX observation file " << cfg.obsfiles[i]
                  << " starts at "
                  << printTime(first,"%04Y/%02m/%02d %02H:%02M:%02S = %F %10.3g");
               LOG(VERBOSE) << " RINEX observation file " << cfg.obsfiles[i]
                  << " has data interval " << asString(fileDT[i],2) << " sec,"
                  << " size " << filesize[i] << " bytes, and types"
                  << (found & 16 ?  " L1":"")
                  << (found & 8 ?  " L2":"")
                  << (found & 2 ?  " P1":"")
                  << (found & 4 ?  " P2":"")
                  << (found & 1 ?  " C1":"");
            }
         }
         rstrm.clear();
         rstrm.close();
      }  // end reading file

      LOG(DEBUG) << "End reading file " << cfg.obsfiles[i] << flush;
      
   }  // end loop over cfg.obsfiles
   cfg.dt0 = fileDT[0];

   LOG(VERBOSE)
      << "The data interval in input file is " << fixed << setprecision(2) << cfg.dt0;

   // test that obs files agree on data interval, and look for C1/P1
   bool P1missing(false), C1missing(false);
   bool P2missing(false), C2missing(false);
   for(j=0; j<cfg.obsfiles.size(); j++) {
      if(fabs(cfg.dt0 - fileDT[j]) > 0.001) {
         msg = string("  Error - RINEX Obs files data intervals differ: ")
             + StringUtils::asString(fileDT[j],2) + string(" != ")
             + StringUtils::asString(cfg.dt0,2);
         LOG(ERROR) << msg;
         inputValid = false;
      }
      if(!(fileHasP1C1[j] & 1)) C1missing = true;
      if(!(fileHasP1C1[j] & 2)) P1missing = true;
      if(!(fileHasP2C2[j] & 1)) C2missing = true;
      if(!(fileHasP2C2[j] & 2)) P2missing = true;
   }

   // handle C1 vs P1
   if(C1missing && cfg.forceCA1) {
      msg = string("  Error - Found '--forceCA1', but these files have no C1 data:");
      for(j=0; j<cfg.obsfiles.size(); j++)
         if(!(fileHasP1C1[j] & 1)) msg += string("\n    ") + cfg.obsfiles[j];
      LOG(ERROR) << msg;
      inputValid = false;
   }
   // NB 'else' and order of if()'s matter in the next stmt
   else if(P1missing && (!cfg.useCA1 || C1missing)) {
      if(C1missing) {
         msg = string("  Error - Not all obs files have either P1 or C1 data.");
      }
      else {
         msg = string("  Error - '--useCA1' not found, ")
             + string("yet these obs files have no P1 data:");
         for(j=0; j<cfg.obsfiles.size(); j++)
            if(!(fileHasP1C1[j] & 2)) msg += string("\n    ") + cfg.obsfiles[j];
      }
      LOG(ERROR) << msg;
      inputValid = false;
   }
   else {
      if(P1missing || cfg.forceCA1) cfg.P1C1 = C1;
      else                          cfg.P1C1 = P1;
      //LOG(VERBOSE) << "Choose to use " << cfg.P1C1 << " for L1 pseudorange";
   }
   // handle C2 vs P2
   if(C2missing && cfg.forceCA2) {
      msg = string("  Error - Found '--forceCA2', but these files have no C2 data:");
      for(j=0; j<cfg.obsfiles.size(); j++)
         if(!(fileHasP2C2[j] & 1)) msg += string("\n    ") + cfg.obsfiles[j];
      LOG(ERROR) << msg;
      inputValid = false;
   }
   // NB 'else' and order of if()'s matter in the next stmt
   else if(P2missing && (!cfg.useCA2 || C2missing)) {
      if(C2missing) {
         msg = string("  Error - Not all obs files have either P2 or C2 data.");
      }
      else {
         msg = string("  Error - '--useCA2' not found, ")
             + string("yet these obs files have no P2 data:");
         for(j=0; j<cfg.obsfiles.size(); j++)
            if(!(fileHasP2C2[j] & 2)) msg += string("\n    ") + cfg.obsfiles[j];
      }
      LOG(ERROR) << msg;
      inputValid = false;
   }
   else {
      if(P2missing || cfg.forceCA2) cfg.P2C2 = C2;
      else                          cfg.P2C2 = P2;
      //LOG(VERBOSE) << "Choose to use " << cfg.P2C2 << " for L2 pseudorange";
   }

   if(inputValid) return 0;
   return -6;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int WriteToRINEX(void)
{
try {
   if(cfg.OutRinexObs.empty()) return 0;
   LOG(VERBOSE) << "Write the output RINEX file " << cfg.OutRinexObs;

   // copy user input into the last input header
   RinexObsHeader rheadout(cfg.rhead);   

   // change the obs type list to include only P1(C1) P2 L1 L2
   rheadout.obsTypeList.clear();

   rheadout.obsTypeList.push_back(RinexObsHeader::L1);
   rheadout.obsTypeList.push_back(RinexObsHeader::L2);
   if(cfg.P1C1 == C1)
      rheadout.obsTypeList.push_back(RinexObsHeader::C1);
   else
      rheadout.obsTypeList.push_back(RinexObsHeader::P1);
   rheadout.obsTypeList.push_back(RinexObsHeader::P2);

   // fill records in output header
   rheadout.fileProgram = PrgmName + string(" v.") + DiscFixVersion.substr(0,4)
                                 + string(",") + cfg.GDConfig.Version().substr(0,4);
   if(!cfg.HDRunby.empty()) rheadout.fileAgency = cfg.HDRunby;
   if(!cfg.HDObs.empty()) rheadout.observer = cfg.HDObs;
   if(!cfg.HDAgency.empty()) rheadout.agency = cfg.HDAgency;
   if(!cfg.HDMarker.empty()) rheadout.markerName = cfg.HDMarker;
   if(!cfg.HDNumber.empty()) rheadout.markerNumber = cfg.HDNumber;
   rheadout.version = 2.1;
   rheadout.valid |= RinexObsHeader::versionValid;
   rheadout.firstObs = cfg.FirstEpoch;
   rheadout.valid |= RinexObsHeader::firstTimeValid;
   rheadout.interval = cfg.dt;
   rheadout.valid |= RinexObsHeader::intervalValid;
   rheadout.lastObs = cfg.LastEpoch;
   rheadout.valid |= RinexObsHeader::lastTimeValid;
   if(cfg.smoothPR)
      rheadout.commentList.push_back(string("Ranges smoothed by ") + PrgmName
         + string(" v.") + DiscFixVersion.substr(0,4) + string(" ") + cfg.Date);
   if(cfg.smoothPH)
      rheadout.commentList.push_back(string("Phases debiased by ") + PrgmName
         + string(" v.") + DiscFixVersion.substr(0,4) + string(" ") + cfg.Date);
   if(cfg.smoothPR || cfg.smoothPH)
      rheadout.valid |= RinexObsHeader::commentValid;
      // invalidate the table
   if(rheadout.valid & RinexObsHeader::numSatsValid)
      rheadout.valid ^= RinexObsHeader::numSatsValid;
   if(rheadout.valid & RinexObsHeader::prnObsValid)
      rheadout.valid ^= RinexObsHeader::prnObsValid;

   int iret = SatPassToRinex2File(cfg.OutRinexObs,rheadout,cfg.SPList);
   if(iret) return -4;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
void PrintSPList(ostream& os, string msg, vector<SatPass>& v)
{
   int i,j,gap;
   RinexSatID sat;
   map<RinexSatID,int> lastSP;
   map<RinexSatID,int>::const_iterator kt;

   os << "#" << leftJustify(msg,4)
             << "  N gap  tot sat   ok  s      start time        end time   dt"
             << " observation types\n";

   for(i=0; i<v.size(); i++) {
      os << msg;
      sat = v[i].getSat();
      kt = lastSP.find(sat);
      if(kt == lastSP.end())
         gap = 0;
      else {
         j = kt->second;
         gap = int((v[i].getFirstTime() - v[j].getLastTime()) / v[i].getDT() + 0.5);
         lastSP.erase(sat);
      }
      lastSP[sat] = i;
         // n,gap,sat,length,ngood,firstTime,lastTime
      os << " " << setw(2) << i+1 << " " << setw(4) << gap << " " << v[i];
      os << endl;
   }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int GetCommandLine(int argc, char **argv)
{
try {
   size_t i;
      // defaults
   cfg.DChelp = false;
   cfg.verbose = false;
   cfg.decimate = 0.0;
   cfg.begTime = Epoch(CommonTime::BEGINNING_OF_TIME);
   cfg.endTime = Epoch(CommonTime::END_OF_TIME);
   cfg.MaxGap = 600.0;

   cfg.LogFile = string("df.log");
   cfg.OutFile = string("df.out");
   cfg.format = string("%4F %10.3g");
   cfg.round = 3;

   cfg.noCA1 = false;      // if false, use CA code on L1 (C1) if P1 absent
   cfg.useCA1 = true;
   cfg.forceCA1 = false;   // if true, use CA code on L1 even if P1 is present
   cfg.noCA2 = false;      // if false, use CA code on L2 (C2) if P2 absent
   cfg.useCA2 = true;
   cfg.forceCA2 = false;   // if true, use CA code on L2 even if P2 is present
   cfg.doGLO = false;      // if true, process GLONASS sats

   cfg.dt = -1.0;
   
   cfg.HDPrgm = PrgmName + string(" v.") + DiscFixVersion.substr(0,4);
   cfg.HDRunby = string("ARL:UT/SGL/GPSTk");

   cfg.smoothPR = false;
   cfg.smoothPH = false;
   cfg.smooth = false;

   for(i=0; i<9; i++) cfg.ndt[i]=-1;

   cfg.inputPath = string(".");

   // -------------------------------------------------------
   // create list of command line options, and fill it
   // put required options first - they will get listed first anyway
   CommandLine opts;
   string cmdlineUsage, cmdlineErrors;
   vector<string> cmdlineUnrecognized;

   // build the options list == syntax page
   string PrgmDesc = "Prgm " + PrgmName +
   " reads a RINEX observation data file containing GPS or GLO dual frequency\n"
   "   pseudorange and carrier phase measurements, divides the data into\n"
   "   'satellite passes', and finds and fixes discontinuities in the phases for\n"
   "   each pass. Output is a list of editing commands for use with RinexEdit.\n"
   "   " + PrgmName
   + " will (optionally) write the corrected pseudorange and phase data\n"
   "   to a new RINEX observation file. Other options will also smooth the\n"
   "   pseudorange and/or debias the corrected phase.\n\n"
   "   " + PrgmName + " calls the GPSTk Discontinuity Corrector (GDC vers "
   + cfg.GDConfig.Version() + ").\n" +
   "   GDC options (--DC below, and see --DChelp) are passed to GDC,\n"
   "     except --DCDT is ignored; it is computed from the data.";

   // temp variables for input
   bool help=false;
   const string defaultstartStr("[Beginning of dataset]");
   const string defaultstopStr("[End of dataset]");
   string startStr(defaultstartStr);
   string stopStr(defaultstopStr);
   vector<string> GLOfreqStrs;

   // required
   opts.Add(0,"obs","file", true, true, &cfg.obsfiles,"\n# File I/O:", 
      "Input RINEX obs file - may be repeated");

   // optional
   // opts.Add(char, opt, arg, repeat?, required?, &target, pre-descript, descript.);
   string dummy("");         // dummy for --file
   opts.Add('f', "file", "name", true, false, &dummy, "",
            "Name of file containing more options [#-EOL = comment]");
   opts.Add(0, "obspath", "path", false, false, &cfg.inputPath, "",
            "Path for input RINEX obs file(s)");

   opts.Add(0, "start", "time", false, false, &startStr,
            "\n# Times (time = \"GPSweek,SOW\" OR \"YYYY,Mon,D,H,Min,S)\":",
            "Start processing the input data at this time");
   opts.Add(0, "stop", "time", false, false, &stopStr, "",
            "Stop processing the input data at this time");

   opts.Add(0, "decimate", "dt", false, false, &cfg.decimate, "# Data config:",
            "Decimate data to time interval (sec) dt");
   opts.Add(0, "gap", "t", false, false, &cfg.MaxGap, "",
            "Minimum gap (sec) between passes [same as --DCMaxGap] ("
               + asString(int(cfg.MaxGap)) + ")");
   opts.Add(0, "noCA1", "", false, false, &cfg.noCA1, "",
            "Fail if L1 P-code is missing, even if L1 CA-code is present");
   opts.Add(0, "noCA2", "", false, false, &cfg.noCA2, "",
            "Fail if L2 P-code is missing, even if L2 CA-code is present");
   opts.Add(0, "forceCA1", "", false, false, &cfg.forceCA1, "",
            "Use C/A L1 range, even if L1 P-code is present");
   opts.Add(0, "forceCA2", "", false, false, &cfg.forceCA2, "",
            "Use C/A L2 range, even if L2 P-code is present");
   opts.Add(0, "onlySat", "sat", false, false, &cfg.SVonly, "",
            "Process only satellite <sat> (a SatID, e.g. G21 or R17)");
   opts.Add(0, "exSat", "sat", true, false, &cfg.exSat, "",
            "Exclude satellite(s) [e.g. --exSat G22,R]");
   opts.Add(0, "doGLO", "", false, false, &cfg.doGLO, "",
            "Process GLONASS satellites as well as GPS");
   opts.Add(0, "GLOfreq", "sat:n", true, false, &GLOfreqStrs, "",
            "GLO channel #s for each sat [e.g. R17:-4]");

   opts.Add(0, "smoothPR", "", false, false, &cfg.smoothPR,
   "# Smoothing: [NB smoothed pseudorange and debiased phase are not identical.]",
            "Smooth pseudorange and output in place of raw pseudorange");
   opts.Add(0, "smoothPH", "", false, false, &cfg.smoothPH, "",
            "Debias phase and output in place of raw phase");
   opts.Add(0, "smooth", "", false, false, &cfg.smooth, "",
            "Same as (--smoothPR AND --smoothPH)");

   opts.Add(0, "DC", "param=value", true, false, &cfg.DCcmds,
            "# Discontinuity Corrector (DC) - cycle slip fixer - configuration:",
            "Set DC parameter <param> to <value>");
   opts.Add(0, "DChelp", "", false, false, &cfg.DChelp, "",
            "Print list of DC parameters (all if -v) and their defaults, then quit");

   opts.Add(0, "log", "file", false, false, &cfg.LogFile, "# Output:",
            "Output log file name (" + cfg.LogFile + ")");
   opts.Add(0, "cmd", "file", false, false, &cfg.OutFile, "",
            "Output file name (for editing commands) (" + cfg.OutFile + ")");
   opts.Add(0, "format", "fmt", false, false, &cfg.format, "",
            "Output time format (cf. gpstk::" "Epoch) (" + cfg.format + ")");
   opts.Add(0, "round", "n", false, false, &cfg.round, "",
            "Round output time format (--format) to n digits");

   opts.Add(0, "RinexFile", "file", false, false, &cfg.OutRinexObs, "# RINEX output:",
            "RINEX (obs) file name for output of corrected data");
   opts.Add(0, "Prgm", "str", false, false, &cfg.HDPrgm, "",
            "RINEX header 'PROGRAM' string for output");
   opts.Add(0, "RunBy", "str", false, false, &cfg.HDRunby, "",
            "RINEX header 'RUNBY' string for output");
   opts.Add(0, "Observer", "str", false, false, &cfg.HDObs, "",
            "RINEX header 'OBSERVER' string for output");
   opts.Add(0, "Agency", "str", false, false, &cfg.HDAgency, "",
            "RINEX header 'AGENCY' string for output");
   opts.Add(0, "Marker", "str", false, false, &cfg.HDMarker, "",
            "RINEX header 'MARKER' string for output");
   opts.Add(0, "Number", "str", false, false, &cfg.HDNumber, "",
            "RINEX header 'NUMBER' string for output");

   opts.Add(0, "verbose", "", false, false, &cfg.verbose, "# Help:",
            "print extended output information");
   //opts.Add(0, "debug", "", false, false, &cfg.debug, "",
   //         "print debug output at level 0 [debug<n> for level n=1-7]");
   opts.Add(0, "help", "", false, false, &help, "",
            "print this and quit");

   // declare it and parse it; write all errors to string GD.cmdlineErrors
   int iret = opts.ProcessCommandLine(argc, argv, PrgmDesc,
                         cmdlineUsage, cmdlineErrors, cmdlineUnrecognized);
   if(iret == -2) {
      LOG(ERROR) << " Error - command line failed (memory)";
      return iret;
   }

   // ---------------------------------------------------
   // do extra parsing -- append errors to GD.cmdlineErrors
   RinexSatID sat;
   string msg;
   vector<string> fields;
   ostringstream oss;

   // unrecognized arguments are an error
   if(cmdlineUnrecognized.size() > 0) {
      oss << "Error - unrecognized arguments:\n";
      for(i=0; i<cmdlineUnrecognized.size(); i++)
         oss << cmdlineUnrecognized[i] << "\n";
      oss << "End of unrecognized arguments\n";
   }

   // if no GLO, add to exSat
   if(!cfg.doGLO && cfg.SVonly.system != SatelliteSystem::Glonass) {
      sat.fromString("R");
      if(vectorindex(cfg.exSat,sat) == -1) cfg.exSat.push_back(sat);
   }

   // parse GLO freq
   for(i=0; i<GLOfreqStrs.size(); i++) {
      fields = StringUtils::split(GLOfreqStrs[i],':');
      if(fields.size() != 2) {
         oss << "Error - invalid GLO sat:chan pair in --GLOfreq input: "
            << GLOfreqStrs[i] << endl;
      }
      else {
         sat.fromString(fields[0]);
         cfg.GLOfreqChannel.insert(
            map<RinexSatID,int>::value_type(sat,asInt(fields[1])));
      }
   }

   // start and stop times
   for(i=0; i<2; i++) {
      string msg = (i==0 ? startStr : stopStr);
      if(msg == (i==0 ? defaultstartStr : defaultstopStr)) continue;

      int n(StringUtils::numWords(msg,','));
      if(n != 2 && n != 6) {
         oss << "Error - invalid argument in --" << (i==0 ? "start" : "stop")
            << " " << (i==0 ? startStr : stopStr) << endl;
         continue;
      }

      string fmtGPS("%F,%g"),fmtCAL("%Y,%m,%d,%H,%M,%S");
      try {
         (i==0 ? cfg.begTime:cfg.endTime).scanf(msg,(n==2 ? fmtGPS:fmtCAL));
      }
      catch(Exception& e) {
         oss << "Error - invalid time in --" << (i==0 ? "start" : "stop")
            << " " << (i==0 ? startStr : stopStr) << endl;
      }
   }

   if(cfg.noCA1) cfg.useCA1 = false;
   if(cfg.noCA2) cfg.useCA2 = false;

   // append errors
   cmdlineErrors += oss.str();
   stripTrailing(cmdlineErrors,'\n');

   // --------------------------------------------------------------------
   // dump a summary of the command line configuration
   oss.str("");         // clear it
   oss << "------ Summary of " << PrgmName
      << " command line configuration --------" << endl;
   opts.DumpConfiguration(oss);
      // perhaps dump the 'extra parsing' things
   oss << "------ End configuration summary --------" << endl;
   cfg.cmdlineSum = oss.str();

   // --------------------------------------------------------------------
   // return
   if(opts.hasHelp() || cfg.DChelp) {
      stripTrailing(cmdlineUsage,'\n');
      LOG(INFO) << cmdlineUsage;
      if(cfg.DChelp) cfg.GDConfig.DisplayParameterUsage(LOGstrm,cfg.verbose);
      return 1;
   }
   if(opts.hasErrors()) {
      LOG(ERROR) << cmdlineErrors << endl;
      return -1;
   }
   if(!cmdlineErrors.empty()) {     // unrecognized or extra parsing produced an error
      LOG(ERROR) << cmdlineErrors;
      return -2;
   }
   return 0;

} // end try
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void DumpConfiguration(void)
{
try {
   int i,j;
      // print config to log, first DF
   LOG(INFO) << "\nHere is the " << PrgmName << " configuration:";
   LOG(INFO) << " Input RINEX obs files are:";
   for(i=0; i<cfg.obsfiles.size(); i++) {
      LOG(INFO) << "   " << cfg.obsfiles[i];
   }
   LOG(INFO) << " Input path for obs files is " << cfg.inputPath;
   if(cfg.decimate > 0.0)
      LOG(INFO) << " Decimate to time interval " << cfg.decimate;
   if(cfg.begTime > Epoch(CommonTime::BEGINNING_OF_TIME))
   LOG(INFO) << " Begin time is "
      << printTime(cfg.begTime,"%04Y/%02m/%02d %02H:%02M:%.3f")
      << " = " << printTime(cfg.begTime,"%04F/%10.3g");
   if(cfg.endTime < Epoch(CommonTime::END_OF_TIME))
      LOG(INFO) << " End time is "
         << printTime(cfg.endTime,"%04Y/%02m/%02d %02H:%02M:%.3f")
         << " = " << printTime(cfg.endTime,"%04F/%10.3g");
   if(cfg.useCA1) 
      {
         LOG(INFO) << " Use the L1 C/A pseudorange if P-code is not found";
      }
   else 
      {
         LOG(INFO) << " Do not use L1 C/A code range (C1)";
      }
   if(cfg.useCA2) 
      {
         LOG(INFO) << " Use the L2 C/A pseudorange if P-code is not found";
      }
   else 
      {
         LOG(INFO) << " Do not use L2 C/A code range (C2)";
      }
   if(cfg.forceCA1) LOG(INFO) <<" Use the L1 C/A pseudorange even if P-code is found";
   if(cfg.forceCA2) LOG(INFO) <<" Use the L2 C/A pseudorange even if P-code is found";
   if(cfg.dt0 > 0) LOG(INFO) << " dt is input as " << cfg.dt0 << " seconds.";
   LOG(INFO) << " Max gap is " << cfg.MaxGap << " seconds";
   if(cfg.exSat.size()) {
      LOGstrm << " Exclude satellites";
      for(i=0; i<cfg.exSat.size(); i++) {
         if(cfg.exSat[i].id == -1)
            LOGstrm << " (all " << cfg.exSat[i].systemString() << ")";
         else
            LOGstrm << " " << cfg.exSat[i];
      }
      LOGstrm << endl;
   }
   if(cfg.SVonly.id > 0)
      LOG(INFO) << " Process only satellite : " << cfg.SVonly;
   LOG(INFO) << (cfg.doGLO ? " P":" Do not p") << "rocess GLONASS satellites";
   if(cfg.GLOfreqChannel.size() > 0) {
      j = 0;
      LOGstrm << " GLO frequency channels:";
      map<RinexSatID,int>::const_iterator it(cfg.GLOfreqChannel.begin());
      while(it != cfg.GLOfreqChannel.end()) {
         LOGstrm << (j==0 ? " ":",") << it->first << ":" << it->second;
         ++j; ++it;
         if((j % 9)==0) { j=0; LOGstrm << endl << "                        "; }
      }
      LOGstrm << endl;
   }
   LOG(INFO) << " Log file is " << cfg.LogFile;
   LOG(INFO) << " Out file is " << cfg.OutFile;
   LOG(INFO) << " Output times in this format '" << cfg.format << "', rounding to "
      << cfg.round << " digits.";
   if(!cfg.OutRinexObs.empty())
      LOG(INFO) << " Output RINEX file name is " << cfg.OutRinexObs;
   if(!cfg.HDRunby.empty())
      LOG(INFO) << " Output RINEX 'RUN BY' is " << cfg.HDRunby;
   if(!cfg.HDObs.empty())
      LOG(INFO) << " Output RINEX 'OBSERVER' is " << cfg.HDObs;
   if(!cfg.HDAgency.empty())
      LOG(INFO) << " Output RINEX 'AGENCY' is " << cfg.HDAgency;
   if(!cfg.HDMarker.empty())
      LOG(INFO) << " Output RINEX 'MARKER' is " << cfg.HDMarker;
   if(!cfg.HDNumber.empty())
      LOG(INFO) << " Output RINEX 'NUMBER' is " << cfg.HDNumber;
   if(cfg.smoothPR) LOG(INFO) << " 'Smoothed range' option is on\n";
   if(cfg.smoothPH) LOG(INFO) << " 'Smoothed phase' option is on\n";
   if(!cfg.smooth) LOG(INFO) << " No smoothing.\n";

} // end try
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
