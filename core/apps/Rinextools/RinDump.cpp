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

/// @file RinDump.cpp
/// Read Rinex observation files (version 2 or 3) and dump data observations, linear
/// combinations or other computed quantities in tabular form.

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
#include "logstream.hpp"
#include "CommandLine.hpp"

#include "CommonTime.hpp"
#include "Epoch.hpp"
#include "TimeString.hpp"
#include "GPSWeekSecond.hpp"

#include "RinexSatID.hpp"

#include "RinexObsID.hpp"
#include "Rinex3ObsStream.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"

#include "Rinex3NavBase.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3NavStream.hpp"

#include "SP3Header.hpp"
#include "SP3Data.hpp"
#include "SP3Stream.hpp"

#include "SP3EphemerisStore.hpp"
#include "Rinex3EphemerisStore.hpp"
//#include "GPSEphemerisStore.hpp"
//#include "GloEphemerisStore.hpp"

#include "TropModel.hpp"
#include "EphemerisRange.hpp"
#include "Position.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
string Version(string("2.3 8/26/15"));
// TD
// VI LAT LON not implemented
// Code selection is not implemented - where to replace C1* with C1W ?
// option to use pos from PRSolve as ref
// GPS nav and GLO nav
// make R2 compatible - pos, ...
// debiasing the output....
//    combo only, phase SI VI IF GF WL NL + RP IR...explicit?
//    always remove initial value for above combos, unless told not to --nozero
//    incl option to reset bias when change exceeds limit --debias <lim>
//   --need to rework this...find a good design
//   still can't set bias on *:R and *:P separately
// In the case of RINEX v.2, set some defaults, e.g. --freq 12 --code GPS:PC
// END TD

//------------------------------------------------------------------------------------
// object to hold linear combination information
class LinCom {
public:
   string label;                    // straight from InputCombo
   string f1,f2;                    // frequencies: 1-char strings: 1,2,5
   double value;                    // sum (consts * ObsIDs)
   double limit;                    // debias jumps limit - skip debiasing if 0.0
   bool limit0;                     // initial debias
   map<RinexSatID,double> biases;   // current bias per sat
   map<RinexSatID,double> prev;     // previous value per sat
   // must be system dependent - <system(1-char),vector>
   map<string, vector<double> > sysConsts;    // vector of constants
   map<string, vector<string> > sysObsids;    // parallel vector of RinexObsIDs

   /// Constructor
   LinCom() throw() : value(0), limit0(false), label(string("Undef")) { }

   /// parse input string
   bool ParseAndSave(const string& str, bool save=true) throw();

   /// compute the linear combination, given the satellite and RINEX data
   double Compute(const RinexSatID sat, Rinex3ObsHeader& Rhead,
                  const vector<RinexDatum>& vrdata) throw(Exception);

   /// remove a bias if jump larger than limit occurs
   bool removeBias(const RinexSatID& sat) throw();

}; // end class LinCom

/// dump the object to an output stream
ostream& operator<<(ostream& os, LinCom& lc) throw();

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
   bool help, verbose, typehelp, combohelp, noHeader, doTECU;
   int debug;
   string cfgfile;

   vector<string> InputObsFiles; // RINEX obs file names
   vector<string> InputSP3Files; // SP3 ephemeris+clock file names
   vector<string> InputNavFiles; // RINEX nav file names

   string Obspath,SP3path,Navpath;      // paths

   vector<RinexSatID> InputSats; // input RinexSatID to dump
   vector<string> InputTags;     // input all tags
   vector<string> InputCombos;   // input linear combination tags
   vector<string> InputSyss;     // input systems to compute: GPS,GLO,etc
   vector<string> InputCodes;    // input sys:codes (RINEX track) to select, in order
   vector<string> InputFreqs;    // input frequency (e.g. 1,2,5,12,25,12+15)
   vector<string> GLOfreqStrs;   // input sat:n where n is GLO frequency channel

   // times derived from --start and --stop
   string defaultstartStr,startStr;
   string defaultstopStr,stopStr;
   CommonTime beginTime,endTime,decTime;

   vector<string> typeLimit;  // type,limit pairs for debiasing input
   vector<string> typeLimit0; // types for initial debiasing input

   string refPosStr;          // string used for input of --ref
   Position knownPos;         // receiver position

   double decimate;           // decimate input data
   string LogFile;            // output log file (required)
   string userfmt;            // user's time format for output
   string TropStr;            // temp used to parse --trop
   double IonoHt;             // ionospheric height
   double elevlimit;          // limit on elevation angle (degrees) requires ELE input

   // end of command line input

   // list of all non-RinexObsID tags
   vector<string> NonObsTags; // satellite-dependent
   vector<string> AuxTags;    // POS,RCL
   // list of all (2-char) linear combination tags
   vector<string> LinComTags;

   // stores
   XvtStore<SatID> *pEph;
   SP3EphemerisStore SP3EphStore;
   Rinex3EphemerisStore RinEphStore;
   //GPSEphemerisStore GPSNavStore;
   //GloEphemerisStore GLONavStore;      // not used - yet
   map<RinexSatID,int> GLOfreqChan;    // either user input or Nav files

   // trop models
   TropModel *pTrop;          // to pass to PRS
   string TropType;           // key ~ Black, NewB, etc; use to identify model
   bool TropPos,TropTime;     // true when trop model has been init with Pos,time
                              // default weather
   double defaultTemp,defaultPress,defaultHumid;

   string msg;
   static const string calfmt,gpsfmt,longfmt;

   // stuff for computing
   bool haveEph,haveRef;                        // flags - what has been input
   bool haveRCL,havePOS,haveObs,haveNonObs,haveCombo;
   // InputCodes -> map<sys,codes> in order eg. GLO:PC
   map<string,string> mapSysCodes;
   map<string,string> map1to3Sys, map3to1Sys;   // mappings "G" <==> "GPS"
   vector<string> vecAllSys;                    // list of all systems "GPS""GLO"...

   // save CER for each sat, clear each epoch
   map<RinexSatID, CorrectedEphemerisRange> mapSatCER;
   // parsed linear combos, used for computing
   vector<LinCom> Combos;

   // limits for debiasing
   map<string,double> debLimit;
   map<string,bool> debLimit0;

}; // end class Configuration

//------------------------------------------------------------------------------------
// const members of Configuration
const string Configuration::PrgmName = string("RinDump");
const string Configuration::calfmt = string("%04Y/%02m/%02d %02H:%02M:%02S");
const string Configuration::gpsfmt = string("%4F %10.3g");
const string Configuration::longfmt = calfmt + " = " + gpsfmt;

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// prototypes
int Initialize(string& errors) throw(Exception);
int ProcessFiles(void) throw(Exception);
double getObsData(string tag, RinexSatID sat, Rinex3ObsHeader& Rhead,
                  const vector<RinexDatum>& vrdata) throw(Exception);
double getNonObsData(string tag, RinexSatID sat, const CommonTime& time)
   throw(Exception);

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
      // get information from the command line
      // iret -2 -3 -4
      if((iret = C.ProcessUserInput(argc, argv)) != 0) break;
      if(!C.noHeader) cout << C.Title << endl;

      // read stores, check input etc
      string errs;
      if((iret = Initialize(errs)) != 0) {
         LOG(ERROR) << "#------- Input is not valid: ----------\n" << errs
                    << "\n#------- end errors -----------";
         break;
      }
      if(!errs.empty()) LOG(INFO) << errs;         // Warnings are here too

      iret = ProcessFiles();                       // iret == number of files

      break;      // mandatory
   }

   if(iret < 0) {
      if(iret == -2) { LOG(INFO) << "Error - Memory error."; }
      else if(iret == -3) { LOG(INFO) << "Error - invalid command line"; }
      else if(iret == -4) { LOG(INFO) << "Error - log file could not be opened"; }
      else if(iret == -5) ; //{ LOG(INFO) << "Error - invalid input"; }
      else { LOG(INFO) << "Error - some other error code"; }
      return iret;
   }
   else if(iret >= 0 && !C.noHeader) {
      // print elapsed time
      totaltime = clock()-totaltime;
      Epoch wallclkend;
      wallclkend.setLocalTime();
      ostringstream oss;
      oss << "# " << C.PrgmName << " timing: processing " << fixed << setprecision(3)
         << double(totaltime)/double(CLOCKS_PER_SEC) << " sec, wallclock: "
         << setprecision(0) << (wallclkend-wallclkbeg) << " sec.";
      LOG(INFO) << oss.str();
   }

   return 0;
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
   include_path(C.Obspath, C.InputObsFiles);
   include_path(C.SP3path, C.InputSP3Files);
   include_path(C.Navpath, C.InputNavFiles);

   expand_filename(C.InputSP3Files);
   expand_filename(C.InputNavFiles);

   size_t i, nfile, nread, nrec;
   ostringstream ossE;

   // -------- SP3 files --------------------------
   // read ephemeris files and fill store
   // first sort them on start time; this for ultra-rapid files, which overlap
   {
      ostringstream os;
      multimap<CommonTime,string> startNameMap;
      for(i=0; i<C.InputSP3Files.size(); i++) {
         SP3Header header;
         try {
            SP3Stream strm(C.InputSP3Files[i].c_str());
            if(!strm.is_open()) {
               os << "Failed to open file " << C.InputSP3Files[i] << endl;
               isValid = false;
               continue;
            }

            strm.exceptions(ios_base::failbit);
            strm >> header;
         }
         catch(Exception& e) {
            os << "Exception: " << e.what() << endl; isValid = false; continue; }
         catch(std::exception& e) {
            os << "exception: " << e.what(); isValid = false; continue; }

         startNameMap.insert(
            multimap<CommonTime, string>::value_type(header.time,C.InputSP3Files[i]));
      }

      ossE << os.str();
      C.InputSP3Files.clear();
      for(multimap<CommonTime,string>::const_iterator it = startNameMap.begin();
                                                      it != startNameMap.end(); ++it)
         C.InputSP3Files.push_back(it->second);
   }

   // read sorted ephemeris files and fill store
   nread = 0;        // use for both SP3 and RINEXnav
   try {
      if(isValid) for(nfile=0; nfile<C.InputSP3Files.size(); nfile++) {
         LOG(DEBUG) << "Load SP3 file " << C.InputSP3Files[nfile];
         C.SP3EphStore.loadSP3File(C.InputSP3Files[nfile]);
         nread++; C.haveEph=true;
      }
   }
   catch(Exception& e) {
      ossE << "Error : failed to read ephemeris files: " << e.getText(0) << endl;
      isValid = false;
   }

   // ------------- configure and dump SP3 and clock stores -----------------
   if(isValid && nread > 0) {
      LOG(VERBOSE) << "Read " << nread << " SP3 ephemeris files into store.";
      LOG(VERBOSE) << "Ephemeris store contains " << C.SP3EphStore.ndata() << " data";

      // set to linear interpolation - TD input?
      C.SP3EphStore.setClockLinearInterp();

      // set gap checking - don't b/c TimeStep may vary GPS/GLO
      //C.SP3EphStore.setClockGapInterval(C.SP3EphStore.getClockTimeStep()+1.);
      //C.SP3EphStore.setClockMaxInterval(2*C.SP3EphStore.getClockTimeStep()+1.);

      // ignore predictions for now // TD make user input?
      C.SP3EphStore.rejectPredPositions(true);
      C.SP3EphStore.rejectPredClocks(true);

      // set gap checking  TD be sure InterpolationOrder is set first
      C.SP3EphStore.setPositionInterpOrder(10);
      //C.SP3EphStore.setPosGapInterval(C.SP3EphStore.getPositionTimeStep()+1.);
      //C.SP3EphStore.setPosMaxInterval(
      //   (C.SP3EphStore.getInterpolationOrder()-1)
      //      * C.SP3EphStore.getPositionTimeStep() + 1.);

      // dump the SP3 ephemeris store; while looping, check the GLO freq channel
      LOG(DEBUG) << "\nDump clock and position stores, including file stores";
      // NB clock dumps are huge!
      if(C.debug > -1) C.SP3EphStore.dump(LOGstrm, (C.debug > 6 ? 2 : 1));
      LOG(DEBUG) << "End of clock store and ephemeris store dumps.";

      // dump a list of satellites, with counts, times and GLO channel
      C.msg = "";
      LOG(VERBOSE) << "\nDump ephemeris sat list with count, times and GLO chan.";
      vector<SatID> sats(C.SP3EphStore.getSatList());
      for(i=0; i<sats.size(); i++) {                           // loop over sats
         // check for some GLO channel - can't compute b/c we don't have data yet
         if(sats[i].system == SatID::systemGlonass) {
            map<RinexSatID,int>::const_iterator it(C.GLOfreqChan.find(sats[i]));
            if(it==C.GLOfreqChan.end()
                                 && sats[i].system==RinexSatID::systemGlonass) {
               //LOG(WARNING) << "Warning - no input GLONASS frequency channel "
               //   << "for satellite " << RinexSatID(sats[i]);

               // set it to zero
               C.GLOfreqChan.insert(map<RinexSatID,int>::value_type(sats[i],0));
               it = C.GLOfreqChan.find(sats[i]);
            }
            C.msg = string(" freq.chan. ") + rightJustify(asString(it->second),2);
         }

         LOG(VERBOSE) << " Sat: " << RinexSatID(sats[i])
         << " Neph: " << setw(3) << C.SP3EphStore.ndata(sats[i])
         << " Beg: " << printTime(C.SP3EphStore.getInitialTime(sats[i]),C.longfmt)
         << " End: " << printTime(C.SP3EphStore.getFinalTime(sats[i]),C.longfmt)
         << C.msg;

      }  // end loop over sats

      RinexSatID sat(sats[0]);
      LOG(VERBOSE) << "Eph Store time intervals for " << sat
         << " are " << C.SP3EphStore.getPositionTimeStep(sat)
         << " (pos), and " << C.SP3EphStore.getClockTimeStep(sat) << " (clk)";
      sat = RinexSatID(sats[sats.size()-1]);
      LOG(VERBOSE) << "Eph Store time intervals for " << sat
         << " are " << C.SP3EphStore.getPositionTimeStep(sat)
         << " (pos), and " << C.SP3EphStore.getClockTimeStep(sat) << " (clk)";

   }

   // assign pEph // TD add GLONav later
   if(C.SP3EphStore.size()) {
      C.pEph = &C.SP3EphStore;
   }

   // currently only have one type of ephemeris store - eph or nav
   if(C.SP3EphStore.size() && C.InputNavFiles.size() > 0) {
      LOG(WARNING) << "Warning - Only one type of satellite ephemeris input accepted;"
            << " ignore RINEX navigation (--nav) input.";
   }

   // -------- Nav files --------------------------
   // NB Nav files may set GLOfreqChan
   if(C.SP3EphStore.size() == 0 && C.InputNavFiles.size() > 0) {
      try {
         // configure - input?
         C.RinEphStore.setOnlyHealthyFlag(true);   // keep only healthy ephemerides

         for(nrec=0,nread=0,nfile=0; nfile < C.InputNavFiles.size(); nfile++) {
            string filename(C.InputNavFiles[nfile]);
            int n(C.RinEphStore.loadFile(filename,(C.debug > -1),LOGstrm));
            if(n == -1) {        // failed to open
               LOG(WARNING) << C.RinEphStore.what;
               continue;
            }
            else if(n == -2) {   // failed to read header
               LOG(WARNING) << "Warning - Failed to read header: "
                  << C.RinEphStore.what << "\nHeader dump follows.";
               C.RinEphStore.Rhead.dump(LOGstrm);
               continue;
            }
            else if(n == -3) {   // failed to read data
               LOG(WARNING) << "Warning - Failed to read data: "
                  << C.RinEphStore.what << "\nData dump follows.";
               C.RinEphStore.Rdata.dump(LOGstrm);
               continue;
            }

            nrec += n;           // number of records read
            nread += 1;

            //if(C.verbose) {
            //   LOG(VERBOSE) << "Read " << n << " ephemeris data from file "
            //      << filename << "; header follows.";
            //   C.RinEphStore.Rhead.dump(LOGstrm);
            //}
         }  // end loop over InputNavFiles

         // expand the network of time system corrections
         C.RinEphStore.expandTimeCorrMap();

         // set search method
         C.RinEphStore.SearchUser(); //C.RinEphStore.SearchNear();
      }
      catch(Exception& e) {
         ossE << "Error : while reading RINEX nav files: " << e.what() << endl;
         isValid = false;
      }

      if(nread == 0) {
         ossE << "Error : Unable to read any RINEX nav files." << endl;
         isValid = false;
      }

      if(isValid) {
         //LOG(VERBOSE) << "Read " << nread << " RINEX navigation files, containing "
         //   << nrec << " records, into store.";
         //LOG(VERBOSE) << "GPS ephemeris store contains "
         //   << C.RinEphStore.size(SatID::systemGPS) << " ephemerides.";
         //LOG(VERBOSE) << "GAL ephemeris store contains "
         //   << C.RinEphStore.size(SatID::systemGalileo) << " ephemerides.";
         //LOG(VERBOSE) << "BDS ephemeris store contains "
         //   << C.RinEphStore.size(SatID::systemBeiDou) << " ephemerides.";
         //LOG(VERBOSE) << "QZS ephemeris store contains "
         //   << C.RinEphStore.size(SatID::systemQZSS) << " ephemerides.";
         //LOG(VERBOSE) << "GLO ephemeris store contains "
         //   << C.RinEphStore.size(SatID::systemGlonass) << " satellites.";
         // dump the entire store
         //int level=0;
         //if(C.verbose) level=2; else if(C.debug > -1) level=3;
         //C.RinEphStore.dump(LOGstrm,level);
         C.haveEph=true;
         C.pEph = &C.RinEphStore;
      }
   }

   // ------ compute and save a reference time for decimation
   if(C.decimate > 0.0) {
      // TD what if beginTime == BEGINNING_OF_TIME ?
      C.decTime = C.beginTime;
      double s,sow(static_cast<GPSWeekSecond>(C.decTime).sow);
      s = int(C.decimate * int(sow/C.decimate));
      if(::fabs(s-sow) > 1.0) LOG(WARNING) << "Warning : decimation reference time "
         << "(--start) is not an even GPS-seconds-of-week mark.";
      C.decTime = static_cast<CommonTime>(
         GPSWeekSecond(static_cast<GPSWeekSecond>(C.decTime).week,0.0));
   }

   // ------- initialize trop model
   // NB only Saas,NewB and Neill require this input, but calls to others are harmless
   if(C.knownPos.getCoordinateSystem() != Position::Unknown) {
      C.pTrop->setReceiverLatitude(C.knownPos.getGeodeticLatitude());
      C.pTrop->setReceiverHeight(C.knownPos.getHeight());
      C.TropPos = true;
   }
   else {
      C.pTrop->setReceiverLatitude(0.0);
      C.pTrop->setReceiverHeight(0.0);
   }

   if(C.beginTime != CommonTime::BEGINNING_OF_TIME) {
      C.pTrop->setDayOfYear(static_cast<YDSTime>(C.beginTime).doy);
      C.TropTime = true;
   }
   else if(C.endTime != CommonTime::END_OF_TIME) {
      C.pTrop->setDayOfYear(static_cast<YDSTime>(C.endTime).doy);
      C.TropTime = true;
   }
   else
      C.pTrop->setDayOfYear(100);

   // -------- check that required input is present
   if(!C.haveEph) {
      for(i=0; i<C.InputTags.size(); i++) {
         string tag(C.InputTags[i]);
         if(find(C.NonObsTags.begin(),C.NonObsTags.end(),tag) != C.NonObsTags.end()) {
            ossE << "Error : Sat-dependent data " << tag << " requires --eph input\n";
            isValid = false;
         }
      }
      for(i=0; i<C.InputCombos.size(); i++) {
         if(C.InputCombos[i].substr(0,2) == "VI") {
            ossE << "Error : Combination data VI requires --eph input\n";
            isValid = false;
         }
      }
   }
   if(!C.haveRef) {
      bool needPos(false);
      for(i=0; i<C.InputTags.size(); i++) {
         string tag(C.InputTags[i]);
         if(tag == "RNG" || tag == "ELE" || tag == "AZI"
               || tag == "TRP" || tag == "LAT" || tag == "LON") {
            ossE << "Error : Rx-dependent data " << tag << " requires --ref input\n";
            isValid = false;
         }
         if(tag == "REL" || tag == "SVX" || tag == "SVY" || tag == "SVZ" ||
            tag == "SCL" || tag == "SVA" || tag == "SVO" || tag == "SVH")
            needPos = true;
      }

      // if needPos, need a position to call CER but it can be anything
      if(needPos) C.knownPos = Position(1.,2.,3.,Position::Cartesian);

      for(i=0; i<C.InputCombos.size(); i++) {
         if(C.InputCombos[i].substr(0,2) == "VI") {
            ossE << "Error : Combination data VI requires --ref input\n";
            isValid = false;
         }
      }
   }

   for(i=0; i<C.InputTags.size(); i++) {
      string tag(C.InputTags[i]);
      if(tag == "LAT" || tag == "LON") {
         ossE << "Error : " << tag << " not implemented\n";
         isValid = false;
      }
   }
   for(i=0; i<C.InputCombos.size(); i++) {
      if(C.InputCombos[i].substr(0,2) == "VI") {
         ossE << "Error : Combination data VI not implemented\n";
         isValid = false;
      }
   }

   // -------- parse combos and save valid ones in C.Combos
   for(int j=int(C.InputCombos.size()-1); j>=0; j--) {
      LinCom lc;
      if(! lc.ParseAndSave(C.InputCombos[j])) {
         ossE << "Warning : Invalid linear combination " << C.InputCombos[j] << "\n";
         C.InputCombos.erase(C.InputCombos.begin()+j);
      }
   }

   // -------- set flags for output convenience
   for(i=0; i<C.InputTags.size(); i++) {
      string tag(C.InputTags[i]);
      if(tag == string("RCL"))
         C.haveRCL = true;                // TD remove from InputTags here
      else if(tag == string("POS"))
         C.havePOS = true;                // TD remove from InputTags here
      else if(isValidRinexObsID(tag))
         C.haveObs = true;
      else if(find(C.NonObsTags.begin(),C.NonObsTags.end(),tag) != C.NonObsTags.end())
         C.haveNonObs = true;
   }
   if(C.Combos.size() > 0) C.haveCombo = true;

   LOG(DEBUG) << (C.haveObs ? "" : "not ") << "haveObs "
              << (C.haveNonObs ? "" : "not ") << "haveNonObs "
              << (C.haveRCL ? "" : "not ") << "haveRCL "
              << (C.havePOS ? "" : "not ") << "havePOS "
              << (C.haveCombo ? "" : "not ") << "haveCombo ";

   // -------- dump all output info to headers
   if(!C.noHeader) {
      //LOG(INFO) << "# " << C.PrgmName << " output spec.s:";
      ostringstream oss;
      oss << "# Allow systems";
      for(i=0; i<C.InputSyss.size(); i++) oss << " " << C.InputSyss[i];
      LOG(INFO) << oss.str();  oss.str("");

      if(C.InputFreqs.size() > 0) {
         oss << "# Output for freq.s";
         for(i=0; i<C.InputFreqs.size(); i++) oss << " " << C.InputFreqs[i];
         LOG(INFO) << oss.str();  oss.str("");
      }

      oss << "# Input file(s)";
      for(i=0; i<C.InputObsFiles.size(); i++) oss << " " << C.InputObsFiles[i];
      LOG(INFO) << oss.str();  oss.str("");

      if(C.haveObs || C.haveNonObs || C.haveCombo) {        // sat-dependent
         oss << "# Satellites";
         if(C.InputSats.size() > 0)
            for(i=0; i<C.InputSats.size(); i++)
               oss << " " << C.InputSats[i];
         else
            oss << " All";
         LOG(INFO) << oss.str();  oss.str("");
      }

      if(C.InputTags.size() > 0) {
         oss << "# Data";
         for(i=0; i<C.InputTags.size(); i++) oss << " " << C.InputTags[i];
         LOG(INFO) << oss.str();  oss.str("");
      }

      if(C.knownPos.getCoordinateSystem() != Position::Unknown) {
         LOG(INFO) << "# Refpos " << C.knownPos.printf(
                              "XYZ(m): %.3x %.3y %.3z = LLH(ddm): %.9AN %.9LE %.3h");
      }

      if(C.Combos.size() > 0) {
         oss << "# Linear combinations";
         for(i=0; i<C.Combos.size(); i++) {
            oss << " " << C.Combos[i].label; // Don't list systems << " (";
            //map<string,vector<double> >::const_iterator jt;
            //for(jt=C.Combos[i].sysConsts.begin();
            //    jt!=C.Combos[i].sysConsts.end(); ++jt)
            //       oss << (jt==C.Combos[i].sysConsts.begin() ? "":" ")
            //          << C.map1to3Sys[jt->first];
            //oss << ")";
         }
         LOG(INFO) << oss.str();  oss.str("");
         // TEMP
         //for(i=0; i<C.Combos.size(); i++) LOG(INFO) << C.Combos[i];
      }
   }

   //if(!C.haveObs && !C.haveNonObs && !C.haveRCL && !C.haveCombo && !C.havePOS) {
   //   ossE << "Error : No data has been specified for output.";
   //   isValid = false;
   //}

   if(C.InputObsFiles.size() == 0) {
      ossE << "Error : No valid input files have been specified.";
      isValid = false;
   }

   // -------- save errors and output
   errors = ossE.str();
   stripTrailing(errors,'\n');
   if(!C.noHeader && !errors.empty()) {
      errors.insert(0,"# ");
      replaceAll(errors,"\n","\n# ");
   }

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
   decimate = 0.0;

   TropType = string("NewB");
   TropPos = TropTime = false;
   defaultTemp = 20.0;
   defaultPress = 1013.0;
   defaultHumid = 50.0;
   TropStr = TropType + string(",") + asString(defaultTemp,1) + string(",")
      + asString(defaultPress,1) + string(",") + asString(defaultHumid,1);
   IonoHt = 400.0;
   elevlimit = 0.0;

   userfmt = gpsfmt;
   help = verbose = noHeader = doTECU = false;
   debug = -1;

   NonObsTags.push_back("RNG");
   NonObsTags.push_back("TRP");
   NonObsTags.push_back("REL");
   NonObsTags.push_back("SCL");
   NonObsTags.push_back("ELE");
   NonObsTags.push_back("AZI");
   NonObsTags.push_back("LAT");
   NonObsTags.push_back("LON");
   NonObsTags.push_back("SVX");
   NonObsTags.push_back("SVY");
   NonObsTags.push_back("SVZ");
   NonObsTags.push_back("SVA");
   NonObsTags.push_back("SVO");
   NonObsTags.push_back("SVH");

   AuxTags.push_back("POS");
   AuxTags.push_back("RCL");        // ... output with epoch, not aux

   LinComTags.push_back("SI");  debLimit["SI"] = 10.;   debLimit0["SI"] = false;
   LinComTags.push_back("VI");  debLimit["VI"] = 10.;   debLimit0["VI"] = false;
   LinComTags.push_back("RP");  debLimit["RP"] = 100.;  debLimit0["RP"] = true;
   LinComTags.push_back("IF");  debLimit["IF"] = 0.0;   debLimit0["IF"] = false;
   LinComTags.push_back("IR");  debLimit["IR"] = 100.;  debLimit0["IR"] = true;
   LinComTags.push_back("GF");  debLimit["GF"] = 10.;   debLimit0["GF"] = true;
   LinComTags.push_back("WL");  debLimit["WL"] = 0.0;   debLimit0["WL"] = false;
   LinComTags.push_back("NL");  debLimit["NL"] = 0.0;   debLimit0["NL"] = false;
   LinComTags.push_back("WLC");  debLimit["WLC"] = 10.;   debLimit0["WLC"] = true;
   LinComTags.push_back("NLC");  debLimit["NLC"] = 10.;   debLimit0["NLC"] = true;

   haveEph = haveRef = false;
   haveCombo = haveRCL = havePOS = haveObs = haveNonObs = false;

   // NB. if vector is given a default, CommandLine will _add to_, not replace, this.
   InputSyss.push_back("GPS");
   InputSyss.push_back("GLO");

   //map<string,string> mapSysCodes;   // map of system, default codes e.g. GLO,PC
   // don't use ObsID::validRinexTrackingCodes b/c order is important
   mapSysCodes.insert(make_pair(string("GPS"),string("PYWLMIQSXCN")));
   mapSysCodes.insert(make_pair(string("GLO"),string("PC")));
   mapSysCodes.insert(make_pair(string("GAL"),string("ABCIQXZ")));
   mapSysCodes.insert(make_pair(string("GEO"),string("CIQX")));
   mapSysCodes.insert(make_pair(string("BDS"),string("IQX")));
   mapSysCodes.insert(make_pair(string("QZS"),string("CSLXZ")));

   map1to3Sys["G"] = "GPS";   map3to1Sys["GPS"] = "G";
   map1to3Sys["R"] = "GLO";   map3to1Sys["GLO"] = "R";
   map1to3Sys["E"] = "GAL";   map3to1Sys["GAL"] = "E";
   map1to3Sys["S"] = "GEO";   map3to1Sys["GEO"] = "S";
   map1to3Sys["C"] = "BDS";   map3to1Sys["BDS"] = "C";
   map1to3Sys["J"] = "QZS";   map3to1Sys["QZS"] = "J";

   string validSys(ObsID::validRinexSystems);
   for(size_t i=0; i<validSys.size(); i++) {
      if(map1to3Sys.count(string(1,validSys[i])) == 0) {
         LOG(WARNING) << "Warning - system \"" << validSys[i]
            << "\" does not have 3-char entry in map1to3Sys";
      }
      else {
         vecAllSys.push_back(map1to3Sys[string(1,validSys[i])]);
      }
   }

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
   if(opts.hasHelp() || typehelp || combohelp) {
      LOG(INFO) << Title;
      LOG(INFO) << cmdlineUsage;
      //return 1; // return below
   }

   if(combohelp) {
      LOG(INFO) << fixed << setprecision(1) <<
"\n These additional <data> tags are supported by " << PrgmName << ":\n"
"# Linear combinations, specified by a tag:type:frequency(ies), as follows:\n"
"    > Type t must be either pseudorange (t=C or R) or phase (t=L or P)\n"
"    > Freq i or ij is optional e.g. 1, 2 or 12, defaults to --freq input(s)\n"
"    > Codes <X>, unless explicit, are determined by option --code\n"
"    > System(s) may be fixed by --sys, or specified as first of 4-char ObsID oi\n"
"    > Below, beta = fi/fj (fi and fj are frequencies); alpha = beta^2 - 1\n"
"    > Phases are multiplied by wavelength, leaving everything in units meters\n"
"  SI:t:ij   Slant ionospheric delay (in meters, unless --TECU)\n"
"              e.g. SI:C:12 = (C1X - C2X)/alpha\n"
"  VI:t:ij   Vertical ionospheric delay [requires --eph --ref --ionoht]\n"
"              VI = SI * obliquity factor\n"
"  RP:i      Pseudorange-minus-phase combinations (Note no type, only one freq)\n"
"  RP:oi:oi  RP using explicit RINEX observation IDs [also see IR:ij below]\n"
"              e.g. RP:1 = C1X-L1X or  RP:GC1C:GL1W (same, but GPS only)\n"
"  IF:t:ij   Ionosphere-free combinations\n"
"              e.g. IF:C:12 = [(alpha+1)*C1X - C2X]/alpha\n"
"  IR:ij     Ionosphere-free pseudorange-minus-phase\n"
"              this is identical to (IF:C:ij - IF:L:ij)\n"
"  GF:t:ij   Geometry-free combinations\n"
"              e.g. GF:L:12 = L1X - L2X  or  GF:R:12 = C2X - C1X\n"
"  WL:t:ij   Wide-lane combinations\n"
"              e.g. WL:C:12 = [beta*C1X - C2X]/(beta-1)\n"
"  NL:t:ij   Narrow-lane combinations\n"
"              e.g. NL:C:12 = [beta*C1X + C2X]/(beta+1)\n"
"  WLC:ij     Melbourne-Wubbena combination (note no type), == (WL:P - NL:R)\n"
"              e.g. WLC:12 = [WL:L:12] - [NL:C:12]\n"
"  NLC:ij     'Narrow lane' combination (note no type), == (NL:P - WL:R)\n"
"# An explicit linear combination given with option --combo co[co[co...]]\n"
"     Here c is a number, with sign, and\n"
"          o is a 3- or 4-char RINEX observation ID (system character optional).\n"
"     By definition, the data = sum(c*o) for all the co's.\n"
"              e.g. --combo +1C1W-1L1W  is the same as RP:C1W:L1W\n"
"              e.g. --combo -0.243875ED8X+1.53908SC5Q (strange, but valid!)"
      ;
      //return 1;
   }  // end combohelp

   // print all valid RinexObsIDs
   if(typehelp) {
      vector<string> goodtags;
      string syss(ObsID::validRinexSystems);
      // build a table
      map<string, map<string, map<string, map<char,string> > > > table;
      for(size_t s=0; s<syss.size(); s++)
         for(int j=ObsID::cbAny; j<ObsID::cbUndefined; ++j)
            for(int k=ObsID::tcAny; k<ObsID::tcUndefined; ++k)
               for(int i=ObsID::otAny; i<ObsID::otUndefined; ++i)
                  try {
                     string tag(string(1,syss[s]) +
                                string(1,ObsID::ot2char[ObsID::ObservationType(i)]) +
                                string(1,ObsID::cb2char[ObsID::CarrierBand(j)]) +
                                string(1,ObsID::tc2char[ObsID::TrackingCode(k)]));
                     ObsID obs(tag);
                     string name(asString(obs));
                     if(name.find("Unknown") != string::npos ||
                        name.find("undefined") != string::npos ||
                        name.find("Any") != string::npos ||
                        !isValidRinexObsID(tag)) continue;

                     if(find(goodtags.begin(),goodtags.end(),tag) == goodtags.end()) {
                        goodtags.push_back(tag);
                        string sys(RinexSatID(string(1,tag[0])).systemString3());
                        char type(ObsID::ot2char[ObsID::ObservationType(i)]);
                        string id(tag); // TD keep sys char ? id(tag.substr(1));
                        string desc(asString(ObsID(tag)));
                        vector<string> fld(split(desc,' '));
                        string codedesc(fld[1].substr(syss[s]=='S'?4:3));
                        string band(fld[0]);
                        table[sys][band][codedesc][type] = id;
                     }
                  }
                  catch(InvalidParameter& ir) { continue; }

      map<string, map<string, map<string, map<char,string> > > >::iterator it;
      map<string, map<string, map<char,string> > >::iterator jt;
      map<string, map<char,string> >::iterator kt;
      // find field lengths
      size_t len2(4),len3(5),len4(6);  // 3-char len4(7);        // 4-char
      for(it=table.begin(); it!=table.end(); ++it)
         for(jt=it->second.begin(); jt!=it->second.end(); ++jt)
            for(kt=jt->second.begin(); kt!=jt->second.end(); ++kt) {
               if(jt->first.length() > len2) len2 = jt->first.length();
               if(kt->first.length() > len3) len3 = kt->first.length();
            }
      LOG(INFO) << "\n# All valid RINEX observation codes";
                                    // (as sys+code = 1+3 char):";
      LOG(INFO) << " Sys " << leftJustify("Freq",len2)
                << " " << center("Track",len3)
                << " Pseudo- Carrier Doppler  Signal";
      LOG(INFO) << "     " << leftJustify("    ",len2)
                << " " << center("     ",len3)
                << "  range   phase          Strength";
      for(size_t i=0; i<syss.size(); ++i) {
         it = table.find(RinexSatID(string(1,syss[i])).systemString3());
         if(it == table.end()) continue;
         if(i > 0) LOG(INFO) << "";
         for(jt=it->second.begin(); jt!=it->second.end(); ++jt)
            for(kt=jt->second.begin(); kt!=jt->second.end(); ++kt) {
               LOG(INFO) << " " << it->first // GPS
                         << " " << leftJustify(jt->first,len2) // L1
                         << " " << center(kt->first,len3) // C/A
                << " " << center((kt->second['C']==""?"----":kt->second['C']),len4)
                << " " << center((kt->second['L']==""?"----":kt->second['L']),len4)
                << " " << center((kt->second['D']==""?"----":kt->second['D']),len4)
                << " " << center((kt->second['S']==""?"----":kt->second['S']),len4);
            }
      }
      //return 1;
   }  // end if typehelp

   // print above
   if(opts.hasHelp() || typehelp || combohelp) return 1;

   // extra parsing (perhaps add to cmdlineErrors, cmdlineExtras)
   iret = ExtraProcessing(cmdlineErrors, cmdlineExtras);
   if(iret == -4) return iret;      // log file could not be opened

   // pull out file name, sats and data tags
   if(cmdlineUnrecognized.size() > 0) {
      for(int i=cmdlineUnrecognized.size()-1; i >= 0; i--) {
         RinexSatID sat;
         string tag(cmdlineUnrecognized[i]);
         upperCase(tag);
         LOG(DEBUG) << "# unrecognized cmdline arg: " << tag;

         // Rinex obs type?
         if(isValidRinexObsID(tag)) {
            LOG(DEBUG) << "# Deduce obs >" << cmdlineUnrecognized[i] << "<";
            InputTags.push_back(tag);
            cmdlineUnrecognized.erase(cmdlineUnrecognized.begin()+i);
            continue;
         }

         // satellite?
         if((!isdigit(tag[0]) && isDigitString(tag.substr(1))) || isDigitString(tag)){
            try {
               sat.fromString(tag);
               LOG(DEBUG) << "# Deduce sat >" << cmdlineUnrecognized[i] << "<";
               InputSats.push_back(RinexSatID(sat));
               cmdlineUnrecognized.erase(cmdlineUnrecognized.begin()+i);
               continue;
            }
            catch(Exception& e) { ; }
         }

         // other data tag?
         if(find(NonObsTags.begin(),NonObsTags.end(),tag) != NonObsTags.end()) {
            LOG(DEBUG) << "# Deduce non-obs >" << cmdlineUnrecognized[i] << "<";
            InputTags.push_back(tag);
            cmdlineUnrecognized.erase(cmdlineUnrecognized.begin()+i);
            continue;
         }

         if(find(AuxTags.begin(),AuxTags.end(),tag) != AuxTags.end()) {
            LOG(DEBUG) << "# Deduce aux >" << cmdlineUnrecognized[i] << "<";
            InputTags.push_back(tag);
            cmdlineUnrecognized.erase(cmdlineUnrecognized.begin()+i);
            continue;
         }

         // linear combo tag?
         string tag2(tag.substr(0,2));
         if(find(LinComTags.begin(),LinComTags.end(),tag2) != LinComTags.end()) {
            LOG(DEBUG) << "# Deduce input combo >" << cmdlineUnrecognized[i] << "<";
            InputCombos.push_back(tag);
            cmdlineUnrecognized.erase(cmdlineUnrecognized.begin()+i);
            continue;
         }

         // last chance - probably a filename
         {
            try {
               string filename(cmdlineUnrecognized[i]);
               ifstream ifstrm(filename.c_str());
               if(ifstrm.is_open()) {
                  LOG(DEBUG) << "# Deduce filename >" << filename << "<";
                  InputObsFiles.push_back(cmdlineUnrecognized[i]);
                  cmdlineUnrecognized.erase(cmdlineUnrecognized.begin()+i);
                  ifstrm.close();
                  continue;
               }
               else {
                  include_path(Obspath, filename);
                  ifstrm.open(filename.c_str());
                  if(ifstrm.is_open()) {
                     LOG(DEBUG) << "# Deduce filename >" << filename << "<";
                     InputObsFiles.push_back(cmdlineUnrecognized[i]);
                     cmdlineUnrecognized.erase(cmdlineUnrecognized.begin()+i);
                     ifstrm.close();
                     continue;
                  }
               }
            } catch(Exception& e) { ; }
         }

         // TEMP?
         LOG(DEBUG) << "Unrecognized >" << cmdlineUnrecognized[i] << "<";
      }
   }

   // output warning / error messages
   if(cmdlineUnrecognized.size() > 0) {
      ostringstream oss;
      oss << "Warning - unrecognized arguments:";
      for(size_t i=0; i<cmdlineUnrecognized.size(); i++)
         oss << " >" << cmdlineUnrecognized[i] << "<";
      LOG(WARNING) << oss.str();
   }

   // fatal errors
   if(!cmdlineErrors.empty()) {
      stripTrailing(cmdlineErrors,'\n');
      replaceAll(cmdlineErrors,"\n","\n ");
      LOG(ERROR) << "Errors found on command line:\n " << cmdlineErrors
         << "\nEnd of command line errors.";
      return -3;
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
" reads one or more RINEX (v.2+) observation files and dump the given\n"
" observation IDs, linear combinations, satellite-dependent information or other\n"
" things, to the screen, as a table, with one time and one satellite per line.\n"
"\n"
" Usage: " + PrgmName + " [options] <file> [<sat>] <data>\n"
"      E.g. " + PrgmName + " test2820.11o G17 C1C L1C R09 ELE AZI\n"
"   <file> is the input RINEX observation file\n"
"   <sat>  is the satellite(s) to output (e.g. G17 or R9); optional, default all\n"
"   <data> is the quantity to be output, either raw data, satellite-dependent data\n"
"          or linear combinations, as given by one of the following tags:\n"
"# Raw data:\n"
"   <oi>  Any RINEX observation ID (3-char), optionally with system (4-char)\n"
"           e.g. C1C GC1C L2* EL5X (see --typehelp below)\n"
"# Satellite-dependent things [and their required input]:\n"
"   RNG   Satellite range in m [--eph --ref]\n"
"   TRP   Tropospheric correction  in m [--eph --ref --trop]\n"
"   REL   Satellite relativity correction  in m [--eph]\n"
"   SCL   Satellite clock  in m [--eph]\n"
"   ELE   Elevation angle in deg [--eph --ref]\n"
"   AZI   Azimuth angle in deg [--eph --ref]\n"
"   LAT   Latitude of ionospheric intercept in deg [--eph --ref --ionoht]\n"
"   LON   Longitude of ionospheric intercept in deg [--eph --ref --ionoht]\n"
"   SVX   Satellite ECEF X coordinate in m [--eph]\n"
"   SVY   Satellite ECEF Y coordinate in m [--eph]\n"
"   SVZ   Satellite ECEF Z coordinate in m [--eph]\n"
"   SVA   Satellite ECEF latitude in deg [--eph]\n"
"   SVO   Satellite ECEF longitude in deg [--eph]\n"
"   SVH   Satellite ECEF height in m [--eph]\n"
"# Linear combinations of the data: run with --combohelp\n"
"# Other things:\n"
"   POS   Receiver position solutions found in auxiliary comments (see PRSolve)\n"
"   RCL   RINEX receiver clock offset in m\n"
"\n Options:";
   ;

   // options to appear on the syntax page, and to be accepted on command line
   //opts.Add(char, opt, arg, repeat?, required?, &target, pre-desc, desc);
   // NB cfgfile is a dummy, but it must exist when cmdline is processed.
   opts.Add('f', "file", "fn", true, false, &cfgfile,
            "# Input via configuration file:",
            "Name of file with more options [#->EOL = comment]");

   opts.Add(0, "obs", "file", true, false, &InputObsFiles,
            "# Required input [--obs,--dat,--sat are optional, but "
            "remove ambiguity]", "Input RINEX observation file name");
   opts.Add(0, "sat", "sat", true, false, &InputSats,"",
            "sat is a RINEX satellite id (see above)");
   opts.Add(0, "dat", "data", true, false, &InputTags,"",
            "data (raw,combination, or other) to dump (see above)");
   opts.Add(0, "combo", "spec", true, false, &InputCombos,"",
            "custom linear combination; spec is co[co[co]]; see --combohelp");

   opts.Add(0, "sys", "s", true, false, &InputSyss,
            "# Define or restrict values used in --dat and --combo",
            "System(s) (GNSSs) <s>=S[,S], where S=RINEX system");
   opts.Add(0, "code", "s:c", true, false, &InputCodes, "                     "
            "RINEX systems are GPS,GLO,GAL,GEO|SBAS,BDS,QZS",
            "System <s> allowed tracking codes <c>, in order [see --typehelp]");
   opts.Add(0, "freq", "f", true, false, &InputFreqs, "                     "
      "Defaults: GPS:PYMNIQSLXWCN, GLO:PC, GAL:ABCIQXZ, GEO:CIQX, BDS:IQX, QZS:CSLXZ",
            "Frequencies to use in solution [e.g. 1, 12, 5, 15]");

   opts.Add(0, "eph", "fn", true, false, &InputSP3Files,
            "# Other file input. NB currently accept only one type, default eph",
            "Input Ephemeris+clock (SP3 format) file name(s)");
   opts.Add(0, "nav", "fn", true, false, &InputNavFiles, "",
            "Input RINEX nav file name(s) [GLO Nav includes freq channel]");

   opts.Add(0, "obspath", "p", false, false, &Obspath,
            "# Paths of input files (optional):",
            "Path of input RINEX observation file(s)");
   opts.Add(0, "ephpath", "p", false, false, &SP3path, "",
            "Path of input ephemeris+clock file(s)");
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
   opts.Add(0, "decimate", "dt", false, false, &decimate, "",
            "Decimate data to time interval dt (0: no decimation)");
   opts.Add(0, "debias", "type:lim", true, false, &typeLimit, "",
            "Debias jumps in data larger than limit (0: no debias)");
   opts.Add(0, "debias0", "type", true, false, &typeLimit0, "",
            "Toggle initial debias of data <type> ()");
   opts.Add(0, "elevlim", "lim", false, false, &elevlimit, "",
            "Limit output to data with elevation angle > lim degrees [ELE req'd]");

   opts.Add(0, "ref", "p[:f]", false, false, &refPosStr, "# Other input",
            "Known position, default fmt f '%x,%y,%z', for resids, elev and ORDs");
   opts.Add(0,"GLOfreq", "sat:n", true, false, &GLOfreqStrs, "",
            "GLO satellite and frequency channel number, e.g. R09:-7");
   opts.Add(0, "Trop", "m,T,P,H", false, false, &TropStr, "",
            "Trop model <m> [one of Zero,Black,Saas,NewB,Neill,GG,GGHt\n             "
            "         with optional weather T(C),P(mb),RH(%)]");
   opts.Add(0,"ionoht", "ht", false, false, &IonoHt, "",
            "Ionospheric height in kilometers [for VI, LAT, LON]");

   opts.Add(0, "timefmt", "fmt", false, false, &userfmt, "# Output:",
            "Format for time tags (see GPSTK::Epoch::printf) in output");
   opts.Add(0, "headless", "", false, false, &noHeader, "",
            "Turn off printing of headers and no-eph-warnings in output");
   opts.Add(0, "TECU", "", false, false, &doTECU, "",
            "Compute iono delay (SI,VI) in TEC units (else meters)");
   opts.Add(0, "verbose", "", false, false, &verbose, "",
            "Print extra output information");
   opts.Add(0, "debug", "", false, false, &debug, "",
            "Print debug output at level 0 [debug<n> for level n=1-7]");
   opts.Add(0, "help", "", false, false, &help, "",
            "Print this syntax page, and quit");
   opts.Add(0, "typehelp", "", false, false, &typehelp, "",
            "Print all valid RINEX obs IDs, and quit");
   opts.Add(0, "combohelp", "", false, false, &combohelp, "",
            "Print syntax for linear combination data tags, and quit");

   // deprecated (old,new)
   opts.Add_deprecated("--SP3","--eph");
   opts.Add_deprecated("--refPos","--ref");

   return PrgmDesc;

}  // end Configuration::BuildCommandLine()

//------------------------------------------------------------------------------------
int Configuration::ExtraProcessing(string& errors, string& extras) throw()
{
   int n;
   size_t i;
   vector<string> fld;
   ostringstream oss,ossx;       // oss for Errors, ossx for Warnings and info

   // reference position
   if(!refPosStr.empty()) {
      bool hasfmt(refPosStr.find('%') != string::npos);
      if(hasfmt) {
         fld = split(refPosStr,':');
         if(fld.size() != 2)
            oss << "Error : invalid arg pos:fmt for --ref: " << refPosStr << endl;
         else try {
            knownPos.setToString(fld[0],fld[1]);
            haveRef = true;
            ossx << "   Reference position --ref is "
                 << knownPos.printf("XYZ(m): %.3x %.3y %.3z = LLH: %.9A %.9L %.3h\n");
         }
         catch(Exception& e) {
            oss << "Error: invalid pos or format for --ref: " << refPosStr << endl;
         }
      }
      else {
         fld = split(refPosStr,',');
         if(fld.size() != 3)
            oss << "Error : invalid format or number of fields in --ref arg: "
               << refPosStr << endl;
         else {
            try {
               knownPos.setECEF(asDouble(fld[0]),asDouble(fld[1]),asDouble(fld[2]));
               haveRef = true;
               ossx << "   Reference position --ref is "
                 << knownPos.printf("XYZ(m): %.3x %.3y %.3z = LLH: %.9A %.9L %.3h\n");
            }
            catch(Exception& e) {
               oss << "Error : invalid position in --ref arg: " << refPosStr
                  << endl;
            }
         }
      }
   }

   // start and stop times
   for(i=0; i<2; i++) {
      static const string fmtGPS("%F,%g"),fmtCAL("%Y,%m,%d,%H,%M,%S");
      msg = (i==0 ? startStr : stopStr);
      if(msg == (i==0 ? defaultstartStr : defaultstopStr)) continue;
      CommonTime ct;

      bool ok(true);
      bool hasfmt(msg.find('%') != string::npos);
      n = numWords(msg,',');
      if(hasfmt) {
         fld = split(msg,':');
         if(fld.size() != 2) { ok = false; }
         else try {
            stripLeading(fld[0]," \t");
            stripLeading(fld[1]," \t");
            scanTime(ct,fld[0],fld[1]);
            (i==0 ? beginTime : endTime) = ct;
         }
         catch(Exception& e) { ok = false; LOG(INFO) << "excep " << e.what(); }
      }
      else if(n == 2 || n == 6) {        // try the defaults
         try {
            scanTime(ct,msg,(n==2 ? fmtGPS : fmtCAL));
            (i==0 ? beginTime : endTime) = ct;
         }
         catch(Exception& e) { ok = false; LOG(INFO) << "excep " << e.what(); }
      }
      (i==0 ? beginTime : endTime).setTimeSystem(TimeSystem::Any);

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

   // GLO frequency channel numbers
   for(i=0; i<GLOfreqStrs.size(); i++) {
      fld = split(GLOfreqStrs[i],':');
      if(fld.size() != 2)
         oss << "Error - invalid GLO sat:channel pair in --GLOfreq: "
            << GLOfreqStrs[i] << endl;
      else {
         RinexSatID sat(fld[0]);
         GLOfreqChan.insert(map<RinexSatID, int>::value_type(sat, asInt(fld[1])));
      }
   }

   // trop model and default weather
   if(!TropStr.empty()) {
      fld = split(TropStr,',');
      if(fld.size() != 1 && fld.size() != 4) {
         oss << "Error : invalid format or number of fields in --Trop arg: "
            << TropStr << endl;
      }
      else {
         msg = fld[0];
         upperCase(msg);
         if     (msg=="ZERO")  { pTrop = new ZeroTropModel(); TropType = "Zero"; }
         else if(msg=="BLACK") { pTrop = new SimpleTropModel(); TropType = "Black"; }
         else if(msg=="SAAS")  { pTrop = new SaasTropModel(); TropType = "Saas"; }
         else if(msg=="NEWB")  { pTrop = new NBTropModel(); TropType = "NewB"; }
         else if(msg=="GG")    { pTrop = new GGTropModel(); TropType = "GG"; }
         else if(msg=="GGHT")  { pTrop = new GGHeightTropModel(); TropType = "GGht"; }
         else if(msg=="NEILL") { pTrop = new NeillTropModel(); TropType = "Neill"; }
         else {
            msg = string();
            oss << "Error : invalid trop model (" << fld[0] << "); choose one of "
               << "Zero,Black,Saas,NewB,GG,GGht,Neill (see gpstk::TropModel)" << endl;
         }

         if(!msg.empty() && !pTrop) {
            oss << "Error : failed to create trop model " << TropType << endl;
         }

         if(fld.size() == 4) {
            defaultTemp = asDouble(fld[1]);
            defaultPress = asDouble(fld[2]);
            defaultHumid = asDouble(fld[3]);
         }

         pTrop->setWeather(defaultTemp,defaultPress,defaultHumid);
      }
   }

   // input systems
   //// remove duplicates
   //sort(InputSyss.begin(), InputSyss.end());
   //InputSyss.erase(unique(InputSyss.begin(), InputSyss.end()), InputSyss.end());
   // remove GPS,GLO if user added to list
   if(InputSyss.size() > 2) {
      InputSyss.erase(InputSyss.begin());
      InputSyss.erase(InputSyss.begin());
   }
   // are they valid?
   for(i=0; i<InputSyss.size(); i++) {
      if(InputSyss[i] == string("SBAS")) InputSyss[i] = string("GEO");
      if(find(vecAllSys.begin(),vecAllSys.end(),InputSyss[i]) == vecAllSys.end())
         oss << "Error : invalid system in --sys: " << InputSyss[i] << endl;
   }

   // input sys:codes
   if(InputCodes.size() > 0) {               // otherwise the defaults are in place
      for(i=0; i<InputCodes.size(); i++) {
         fld = split(InputCodes[i],':');
         if(fld.size() != 2) {
            oss << "Error : invalid arg for --code: " << InputCodes[i] << endl;
            continue;
         }
         upperCase(fld[0]);
         upperCase(fld[1]);
         // is it a valid system?
         if(find(vecAllSys.begin(),vecAllSys.end(),fld[0]) == vecAllSys.end()) {
            oss << "Error : invalid system in --code arg: " << fld[0] << endl;
            continue;
         }
         // are the codes allowed?
         msg = mapSysCodes[fld[0]];
         bool ok(true);
         for(size_t j=0; j<fld[1].size(); j++)
            if(msg.find(fld[1][j],0) == string::npos) { ok = false; break; }
         if(ok) mapSysCodes[fld[0]] = fld[1];
      }
   }

   // debiasing limits
   for(i=0; i<typeLimit.size(); i++) {
      fld = split(typeLimit[i],':');
      if(fld.size() != 2) {
         LOG(WARNING) << "Error - argument to --debias is invalid; use type:limit";
         continue;
      }
      debLimit[fld[0]] = asDouble(fld[1]);
   }
   for(i=0; i<typeLimit0.size(); i++) {
      debLimit0[typeLimit0[i]] = !debLimit0[typeLimit0[i]];
   }

   // open the log file (so warnings, configuration summary, etc can go there) -----
   //logstrm.open(LogFile.c_str(), ios::out);
   //if(!logstrm.is_open()) {
      //LOG(ERROR) << "Error : Failed to open log file " << LogFile;
      //return -4;
   //}
   //LOG(INFO) << "Output redirected to log file " << LogFile;
   //pLOGstrm = &logstrm;
   //LOG(INFO) << Title;

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
   static const int width=13;
   int iret,nfiles;
   size_t i,j,k,nfile;
   string tag;
   RinexSatID sat;
   Rinex3ObsStream ostrm;
   ostringstream oss;

   for(nfiles=0,nfile=0; nfile<C.InputObsFiles.size(); nfile++) {
      Rinex3ObsStream istrm;
      Rinex3ObsHeader Rhead, Rheadout;
      Rinex3ObsData Rdata;
      string filename(C.InputObsFiles[nfile]);

      // iret is set to 0 ok, or could not: 1 open file, 2 read header, 3 read data
      iret = 0;

      // open the file ------------------------------------------------
      istrm.open(filename.c_str(),ios::in);
      if(!istrm.is_open()) {
         LOG(WARNING) << "Warning : could not open file " << filename;
         iret = 1;
         continue;
      }
      else
         LOG(DEBUG) << "Opened input file " << filename;
      istrm.exceptions(ios::failbit);

      // read the header ----------------------------------------------
      try { istrm >> Rhead; }
      catch(Exception& e) {
         LOG(WARNING) << "Warning : Failed to read header: " << e.what()
            << "\n Header dump follows.";
         Rhead.dump(LOGstrm);
         istrm.close();
         iret = 2;
         continue;
      }
      if(C.debug > -1) {
         LOG(DEBUG) << "Input header for RINEX file " << filename;
         Rhead.dump(LOGstrm);
      }

      if(!C.noHeader) {
         LOG(INFO) << "# " << C.PrgmName << " output for file " << filename;

         // dump the obs types
         map<string,vector<RinexObsID> >::const_iterator kt;
         for(kt = Rhead.mapObsTypes.begin(); kt != Rhead.mapObsTypes.end(); kt++) {
            sat.fromString(kt->first);
            // is this system found in the list of satellites?
            bool ok=false;
            for(i=0; i<C.InputSats.size(); i++)
               if(C.InputSats[i].system == sat.system) { ok=true; break; }
            if(!ok) continue;

            oss.str("");
            oss << "# Header ObsIDs " << sat.systemString3() //<< " " << kt->first
               << " (" << kt->second.size() << "):";
            for(i=0; i<kt->second.size(); i++) oss << " " << kt->second[i].asString();
            LOG(INFO) << oss.str();
         }

         // write file name and header line(s)
         if(C.havePOS)
            LOG(INFO) << "# wk secs-of-wk POS" 
               << " Sol-Desc        X            Y           Z"
               << "     SYS Clk[...] Nsats PDOP GDOP RMS";
               
         if(C.haveRCL)
            LOG(INFO) << "# wk secs-of-wk RCL clock_bias(m)";

         if(C.haveObs || C.haveNonObs || C.haveCombo) {
            oss.str("");
            oss << "# wk secs-of-wk sat";
            for(i=0; i<C.InputTags.size(); i++) {
               tag = C.InputTags[i];
               if(find(C.AuxTags.begin(), C.AuxTags.end(), tag) != C.AuxTags.end())
                  continue;
               //if(isValidRinexObsID(tag) &&tag.find_first_of("*") != string::npos) {
               //   // replace '*' with first available tracking code
               //}
               oss << " " << center(tag,width);
            }
            for(i=0; i<C.Combos.size(); i++) {
               tag = C.Combos[i].label;
               oss << " " << center(tag,width);
            }
            LOG(INFO) << oss.str();
         }
      }

      // check for no data here
      if(!C.haveObs && !C.haveNonObs && !C.haveRCL && !C.haveCombo && !C.havePOS) {
         LOG(INFO) << "Warning - No data specified for output...skip this file.";
         continue;
      }

      // loop over epochs ---------------------------------------------
      while(1) {
         try { istrm >> Rdata; }
         catch(Exception& e) {
            LOG(WARNING) << " Warning : Failed to read obs data (Exception "
               << e.getText(0) << "); dump follows.";
            Rdata.dump(LOGstrm,Rhead);
            istrm.close();
            iret = 3;
            break;
         }
         catch(std::exception& e) {
            Exception ge(string("Std excep: ") + e.what());
            GPSTK_THROW(ge);
         }
         catch(...) {
            Exception ue("Unknown exception while reading RINEX data.");
            GPSTK_THROW(ue);
         }

         // normal EOF
         if(!istrm.good() || istrm.eof()) { iret = 0; break; }

         //LOG(INFO) << "";
         LOG(DEBUG) << " Read RINEX data: flag " << Rdata.epochFlag
            << ", timetag " << printTime(Rdata.time,C.longfmt);

         // stay within time limits
         if(Rdata.time < C.beginTime) {
            LOG(DEBUG) << " RINEX data timetag " << printTime(Rdata.time,C.longfmt)
               << " is before begin time " << printTime(C.beginTime,C.longfmt);
            continue;
         }
         if(Rdata.time > C.endTime) {
            LOG(DEBUG) << " RINEX data timetag " << printTime(Rdata.time,C.longfmt)
               << " is after end time " << printTime(C.endTime,C.longfmt);
            break;
         }

         // decimate
         if(C.decimate > 0.0) {
            if(C.decTime == CommonTime::BEGINNING_OF_TIME) C.decTime = Rdata.time;
            double dt(::fabs(Rdata.time - C.decTime));
            dt -= C.decimate * long(0.5 + dt/C.decimate);
            if(::fabs(dt) > 0.25) {
               LOG(DEBUG) << " Decimation rejects RINEX data timetag "
                  << printTime(Rdata.time,C.longfmt);
               continue;
            }
         }

         // prepare start of output line
         string line(printTime(Rdata.time,C.userfmt));

         // if aux header data, either output or skip
         if(Rdata.epochFlag > 1) {
            // skip unless POS has been selected
            if(!C.havePOS) continue;

            oss.str("");
            for(j=0; j<Rdata.auxHeader.commentList.size(); j++) {
               string com(Rdata.auxHeader.commentList[j]);
               vector<string> fld(split(com,' '));
               // NB keep R2 first in if-tree: DIAG looks like DIA
               if(fld[0] == string("XYZT")) {                     // R2
                  if(C.knownPos.getCoordinateSystem() != Position::Unknown) {
                     oss << "    NA    " << fixed << setprecision(3)
                         << " " << setw(8) << (asDouble(fld[1])-C.knownPos.X())
                         << " " << setw(8) << (asDouble(fld[2])-C.knownPos.Y())
                         << " " << setw(8) << (asDouble(fld[3])-C.knownPos.Z())
                         << " GPS " << fld[4];
                  }
                  else {
                     oss << "    NA    "
                         << " " << fld[1] << " " << fld[2] << " " << fld[3]
                         << " GPS " << fld[4];
                  }
               }
               else if(fld[0] == string("DIAG")) {                // R2
                  for(k=1; k<fld.size()-1; k++)
                     oss << " " << fld[k];
                  LOG(INFO) << line << " POS" << oss.str();
                  oss.str("");
               }
               else if(fld[0] == string("XYZ")) {                 // R3
                  if(C.knownPos.getCoordinateSystem() != Position::Unknown) {
                     oss << " " << fld[4] << fixed << setprecision(3)
                         << " " << setw(8) << (asDouble(fld[1])-C.knownPos.X())
                         << " " << setw(8) << (asDouble(fld[2])-C.knownPos.Y())
                         << " " << setw(8) << (asDouble(fld[3])-C.knownPos.Z())
                         << " GPS " << fld[4];
                  }
                  else {
                     oss << " " << fld[4]
                         << " " << fld[1] << " " << fld[2] << " " << fld[3];
                  }
               }
               else if(fld[0] == string("CLK")) {                 // R3
                  for(k=1; k<fld.size()-1; k++)
                     oss << " " << fld[k];
               }
               else if(fld[0].substr(0,3) == string("DIA")) {     // R3
                  oss << " " << fld[0].substr(3);
                  for(k=1; k<fld.size()-1; k++)
                     oss << " " << fld[k];
                  LOG(INFO) << line << " POS" << oss.str();
                  oss.str("");
               }
            }
         }

         // epochFlag is for regular data
         else {
            if(C.haveObs || C.haveNonObs || C.haveCombo) {
               // dump receiver clock offset - its own line
               if(C.haveRCL) LOG(INFO) << line << " RCL " << fixed << setprecision(3)
                  << setw(width) << Rdata.clockOffset * C_MPS;

               // clear CER store
               if(C.haveNonObs) C.mapSatCER.clear();

               // loop over satellites -----------------------------
               Rinex3ObsData::DataMap::const_iterator it;
               for(it=Rdata.obs.begin(); it!=Rdata.obs.end(); ++it) {
                  sat = it->first;
                  // output this sat?
                  if(C.InputSats.size() > 0 &&
                        find(C.InputSats.begin(), C.InputSats.end(), sat)
                           == C.InputSats.end())
                  {
                     // check for all sats of this system
                     RinexSatID tsat(-1, sat.system);
                     if(find(C.InputSats.begin(), C.InputSats.end(), tsat)
                           == C.InputSats.end())
                     {
                        //LOG(INFO) << "Reject sat " << sat;
                        continue;
                     }
                  }

                  // is system allowed?
                  bool ok(false);
                  for(i=0; i<C.InputSyss.size(); i++) {
                     if(sat.systemChar() == C.map3to1Sys[C.InputSyss[i]][0]) {
                        ok = true;
                        break;
                     }
                  }
                  if(!ok) {
                     //LOG(WARNING) << "Warning - system " << sat << " not allowed.";
                     continue;
                  }

                  // access the data
                  const vector<RinexDatum>& vrdata(it->second);

                  // don't output all zero's, or elev > elevlimit
                  ok = false;                // reuse ok; if one datum is good, output
                  bool badele(false);

                  // output the sat ID
                  oss.str("");
                  oss << " " << sat << fixed << setprecision(3);

                  // output the data, in order (zero-filled)
                  for(i=0; i<C.InputTags.size(); i++) {
                     double data(0);
                     tag = C.InputTags[i];

                     // skip AuxTags
                     if(find(C.AuxTags.begin(),C.AuxTags.end(),tag)!=C.AuxTags.end())
                        continue;

                     else if(isValidRinexObsID(tag))     // tag = RINEX Obs ID
                        data = getObsData(tag, sat, Rhead, vrdata);

                     else if(find(C.NonObsTags.begin(),  // tag = Sat-dep non-obs type
                                 C.NonObsTags.end(), tag) != C.NonObsTags.end())
                        data = getNonObsData(tag, sat, Rdata.time);

                     oss << " " << setw(width) << data;
                     if(data != 0.0) ok=true;
                     if(tag==string("ELE") && C.elevlimit > 0.0 && data < C.elevlimit)
                        badele = true;
                  }
                  if(badele) continue;    // don't compute lincombos due to removeBias

                  // output linear combinations
                  vector<string> resets;     // check for reset of bias on any lc
                  for(i=0; i<C.Combos.size(); i++) {
                     C.Combos[i].Compute(sat, Rhead, vrdata);  // member value
                     if(C.Combos[i].value && C.Combos[i].removeBias(sat))
                        resets.push_back(C.Combos[i].label);
                     oss << " " << setw(width) << C.Combos[i].value;
                     if(C.Combos[i].value != 0.0) ok=true;
                  }

                  // output resets
                  if(resets.size() > 0) oss << "  BR";
                  for(i=0; i<resets.size(); i++)
                     oss << " " << resets[i];

                  // output the complete line
                  if(ok) LOG(INFO) << line << oss.str();

               }  // end loop over satellites
            }  // end if haveObs

         }  // end epochFlag for data

         // debug: dump the RINEX data object
         if(C.debug > -1) Rdata.dump(LOGstrm,Rhead);
         //else if(C.verbose) LOG(VERBOSE) << " RINEX " << Rdata.epochFlag
         //   << " " << printTime(Rdata.time,C.userfmt) << oss.str();

      }  // end while loop over epochs

      istrm.close();

      // failure due to critical error
      if(iret < 0) break;

      if(iret == 0) nfiles++;

   }  // end loop over files

   if(iret < 0) return iret;

   return nfiles;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end ProcessFiles()

//------------------------------------------------------------------------------------
double getObsData(string tag, RinexSatID sat, Rinex3ObsHeader& Rhead,
                   const vector<RinexDatum>& vrdata) throw(Exception)
{
   try {
      double data(0);
      string sys(1,sat.systemChar());              // system of this sat

      if(tag.size() == 4 && tag[0] != sys[0])
         return 0;                                 // system does not match
      if(tag.size() == 3) {
         tag = sys + tag;                          // add system char to tag
         if(!isValidRinexObsID(tag))
            return 0;                              // system+tag is not valid
      }

      RinexObsID obsid(tag);                       // ObsID for this tag

      // find it in the header
      vector<RinexObsID>::const_iterator jt(
         find(Rhead.mapObsTypes[sys].begin(),Rhead.mapObsTypes[sys].end(),obsid));
      if(jt != Rhead.mapObsTypes[sys].end()) {     // its in the header
         int j = jt - Rhead.mapObsTypes[sys].begin();
         data = vrdata[j].data;
      }

      return data;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end getObsData()

//------------------------------------------------------------------------------------
double getNonObsData(string tag, RinexSatID sat, const CommonTime& time)
   throw(Exception)
{
   try {
      double data(0);
      Configuration& C(Configuration::Instance());
      // need the CER for this sat?
      if(C.mapSatCER.find(sat) == C.mapSatCER.end()) {
         CorrectedEphemerisRange CER;
         try {
            CER.ComputeAtReceiveTime(time, C.knownPos, sat, *C.pEph);
            C.mapSatCER[sat] = CER;
         }
         catch(Exception& e) {
            if(!C.noHeader) LOG(VERBOSE) << "# Warning - no ephemeris for ("
                  << tag << ") sat " << sat
                  << " at time " << printTime(time,C.longfmt);
            return data;
         }
      }

      // compute the thing
      if(tag == string("RNG"))
         data = C.mapSatCER[sat].rawrange;
      else if(tag == string("TRP")) {
         Position SV(C.mapSatCER[sat].svPosVel.x, Position::Cartesian);
         data = C.pTrop->correction(C.knownPos,SV,time);
      }
      else if(tag == string("REL"))
         data = C.mapSatCER[sat].relativity;
      else if(tag == string("SCL"))
         data = C.mapSatCER[sat].svclkbias;
      else if(tag == string("ELE"))
         data = C.mapSatCER[sat].elevationGeodetic;
      else if(tag == string("AZI"))
         data = C.mapSatCER[sat].azimuthGeodetic;
      else if(tag == string("LAT")) {
         // TD
      }
      else if(tag == string("LON")) {
         // TD
      }
      else if(tag == string("SVX"))
         data = C.mapSatCER[sat].svPosVel.x[0];
      else if(tag == string("SVY"))
         data = C.mapSatCER[sat].svPosVel.x[1];
      else if(tag == string("SVZ"))
         data = C.mapSatCER[sat].svPosVel.x[2];
      else if(tag == string("SVA")) {
         Position pos(C.mapSatCER[sat].svPosVel.x, Position::Cartesian);
         data = pos.geodeticLatitude();
      }
      else if(tag == string("SVO")) {
         Position pos(C.mapSatCER[sat].svPosVel.x, Position::Cartesian);
         data = pos.longitude();
      }
      else if(tag == string("SVH")) {
         Position pos(C.mapSatCER[sat].svPosVel.x, Position::Cartesian);
         data = pos.height();
      }

      return data;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
// TD codes ....
// Parse combo given by lab, and if valid save in C.Combos
bool LinCom::ParseAndSave(const string& lab, bool save) throw()
{
   size_t i,j;
   string sys,obsid;
   RinexSatID sat;
   Configuration& C(Configuration::Instance());

   if(lab.size() == 0)                       // nothing to parse
      return false;

   LOG(DEBUG2) << "Parse label >" << lab << "<";
   label = lab;                              // save the label

   vector<string> fld(split(lab,':'));       // split into fields on :

   string tag(fld[0]);                       // LinCom tag ~ SI,VI,IF,GF,WLC,NLC,etc
   //if(find(C.LinComTags.begin(), C.LinComTags.end(), tag) == C.LinComTags.end()) {
   //   LOG(DEBUG2) << "tag is not in LinComTags list";
   //   return false;                          // tag is not in the list
   //}

   // set limit and limit0
   limit = C.debLimit[tag];
   limit0 = C.debLimit0[tag];

   sysConsts.clear();
   sysObsids.clear();

   LOG(DEBUG2) << "Parse tag is >" << tag << "<";
   if(tag == string("RP")) {                 // RP
      if(fld.size() == 1) {                  // RP alone : i from --freq
         for(i=0; i<C.InputFreqs.size(); i++) {
            if(C.InputFreqs[i].size() == 1)  // only take single freqs
               if(!this->ParseAndSave("RP:"+C.InputFreqs[i]))
                  return false;
         }
         return true;
      }

      else if(fld.size() == 2) {             // RP:i
         f1 = fld[1]; f2 = string();
         for(i=0; i<C.InputSyss.size(); i++) {
            sys = C.map3to1Sys[C.InputSyss[i]];
            sysConsts[sys].push_back(1.0);
            sysObsids[sys].push_back(sys+"C"+fld[1]+"*");
            sysConsts[sys].push_back(-1.0);
            sysObsids[sys].push_back(sys+"L"+fld[1]+"*");   // Cf* - Lf*
         }
      }

      else if(fld.size() == 3) {             // RP:oi:oi
         if(!isValidRinexObsID(fld[1]) || !isValidRinexObsID(fld[2]))
            return false;

         // must make systems consistent
         vector<string> syss;
         if(fld[1].size() == 4 && fld[2].size() == 4) {
            if(fld[1][0] != fld[2][0]) return false;           // systems don't match
            syss.push_back(C.map1to3Sys[string(1,fld[1][0])]);
         }
         else if(fld[1].size() == 4) {                         // first oid has sys ch
            syss.push_back(C.map1to3Sys[string(1,fld[1][0])]);
            fld[2] = string(1,fld[1][0]) + fld[2];
         }
         else if(fld[2].size() == 4) {                         // second oid has sys
            syss.push_back(C.map1to3Sys[string(1,fld[2][0])]);
            fld[1] = string(1,fld[2][0]) + fld[1];
         }
         else syss = C.InputSyss;                              // do for all sys

         for(i=0; i<syss.size(); i++) {
            sys = C.map3to1Sys[syss[i]];
            sysConsts[sys].push_back(1.0);
            sysObsids[sys].push_back(fld[1]);
            sysConsts[sys].push_back(-1.0);
            sysObsids[sys].push_back(fld[2]);
         }
      }

      else return false;
   }

   else if(tag == string("IR")) {                              // IF RP
      LOG(DEBUG2) << "Parse freqs are >" << fld[1] << "<";
      if(fld[1].size() != 2) return false;
      f1 = string(1,fld[1][0]);
      f2 = string(1,fld[1][1]);
      int n1 = asInt(f1);
      int n2 = asInt(f2);
      for(i=0; i<C.InputSyss.size(); i++) {
         sys = C.map3to1Sys[C.InputSyss[i]];
         sat = RinexSatID(C.InputSyss[i]);
         double alpha(getAlpha(sat,n1,n2));

         obsid = sys+"C"+f1+"*";                         // Ci*
         LOG(DEBUG2) << "Parse obsid >" << obsid << "<";
         if(!isValidRinexObsID(obsid))
            continue;                                    // skip this sys
         sysObsids[sys].push_back(obsid);
         sysConsts[sys].push_back((alpha+1.0)/alpha);

         obsid = sys+"L"+f1+"*";                         // Li*
         LOG(DEBUG2) << "Parse obsid >" << obsid << "<";
         if(!isValidRinexObsID(obsid))
            continue;                                    // skip this sys
         sysObsids[sys].push_back(obsid);
         sysConsts[sys].push_back(-(alpha+1.0)/alpha);

         obsid = sys+"C"+f2+"*";                         // Cj*
         LOG(DEBUG2) << "Parse obsid >" << obsid << "<";
         if(!isValidRinexObsID(obsid))
            continue;                                    // skip this sys
         sysObsids[sys].push_back(obsid);
         sysConsts[sys].push_back(-1.0/alpha);

         obsid = sys+"L"+f2+"*";                         // Lj*
         LOG(DEBUG2) << "Parse obsid >" << obsid << "<";
         if(!isValidRinexObsID(obsid))
            continue;                                    // skip this sys
         sysObsids[sys].push_back(obsid);
         sysConsts[sys].push_back(1.0/alpha);

         LOG(DEBUG2) << "Parse ok";
      }
   }

   else if(tag == string("SI") || tag == string("VI") ||
           tag == string("IF") || tag == string("GF") ||
           tag == string("WL") || tag == string("NL"))
   {                                         // tag:t:ij
      string type;
           if(fld[1] == "C" || fld[1] == "R") type = string("C");
      else if(fld[1] == "L" || fld[1] == "P") type = string("L");
      else return false;
      LOG(DEBUG2) << "Parse type is >" << type << "<";

      if(fld.size() == 2) {                  // tag:t
         for(i=0; i<C.InputFreqs.size(); i++) {
            if(C.InputFreqs[i].size() == 2)  // only take dual freqs
               if(! this->ParseAndSave(tag+":"+type+":"+C.InputFreqs[i]))
                  return false;
         }
         return true;
      }
      else if(fld.size() != 3) return false;

      LOG(DEBUG2) << "Parse freqs are >" << fld[2] << "<";
      if(fld[2].size() != 2) return false;
      f1 = string(1,fld[2][0]);
      f2 = string(1,fld[2][1]);
      int n1 = asInt(f1);
      int n2 = asInt(f2);

      for(i=0; i<C.InputSyss.size(); i++) {              // loop over systems
         sys = C.map3to1Sys[C.InputSyss[i]];
         sat = RinexSatID(sys);

         obsid = sys+type+f1+"*";
         LOG(DEBUG2) << "Parse obsid >" << obsid << "<";
         if(!isValidRinexObsID(obsid))
            continue;                                    // skip this sys
         sysObsids[sys].push_back(obsid);

         obsid = sys+type+f2+"*";
         LOG(DEBUG2) << "Parse obsid >" << obsid << "<";
         if(!isValidRinexObsID(obsid))
            { sysObsids[sys].clear(); continue; }        // skip this sys
         sysObsids[sys].push_back(obsid);

         LOG(DEBUG2) << "Parse ok";

         // TD VI LAT LON not implemented
         if(tag == string("SI") || tag == string("VI")) {         // iono delay
            double alpha(getAlpha(sat,n1,n2));
            // convert to TECU
            double TECUperM(1.0);
            if(C.doTECU) {
               if(sat.system == SatID::systemGPS) {
                  static const double GPSL1(L1_FREQ_GPS*1.e-8);
                  TECUperM = GPSL1*GPSL1/40.28;
               }
               else if(sat.system == SatID::systemGlonass) {
                  static const double GLOL1((L1_FREQ_GLO
                                 + C.GLOfreqChan.count(sat)*L1_FREQ_STEP_GLO)*1.e-8);
                  TECUperM = GLOL1*GLOL1/40.28;
               }
            }
 
            //LOG(DEBUG2) << "Parse alpha is " << fixed << setprecision(4) << alpha
            //   << " for sat " << sat << " and TECUperM " << scientific << TECUperM;
            sysConsts[sys].push_back(TECUperM/alpha);
            sysConsts[sys].push_back(-TECUperM/alpha);
         }
         else if(tag == string("IF")) {                           // iono-free
            double alpha(getAlpha(sat,n1,n2));
            //LOG(DEBUG2) << "Parse alpha is " << fixed << setprecision(4) << alpha;
            sysConsts[sys].push_back((alpha+1.0)/alpha);
            sysConsts[sys].push_back(-1.0/alpha);
         }
         else if(tag == string("GF")) {                           // geo-free
            sysConsts[sys].push_back(1.0);
            sysConsts[sys].push_back(-1.0);
         }
         else if(tag == string("WL")) {                           // widelane
            double beta(getBeta(sat,n1,n2));
            //LOG(DEBUG2) << "Parse beta is " << fixed << setprecision(4) << beta;
            sysConsts[sys].push_back(beta/(beta-1.0));
            sysConsts[sys].push_back(-1.0/(beta-1.0));
         }
         else if(tag == string("NL")) {                           // narrowlane
            double beta(getBeta(sat,n1,n2));
            //LOG(DEBUG2) << "Parse beta is " << fixed << setprecision(4) << beta;
            sysConsts[sys].push_back(beta/(beta+1.0));
            sysConsts[sys].push_back(1.0/(beta+1.0));
         }
      }  // end loop over systems

      if(sysConsts.size() == 0 || sysObsids.size() == 0) return false;
   }

   else if(tag == string("WLC")) {            // MW:ij
      if(fld.size() == 1) {                  // MW alone : ij from --freq
         for(i=0; i<C.InputFreqs.size(); i++) {
            sysConsts.clear(); sysObsids.clear();
            if(C.InputFreqs[i].size() == 2)  // only take dual freqs
               if(! this->ParseAndSave("WLC:"+C.InputFreqs[i]))
                  return false;
         }
         return true;
      }
      else if(fld.size() != 2)
         return false;                       // not WLC and not WLC:ij

      LOG(DEBUG2) << "Parse construct " << ("WL:L:"+fld[1]);
      LinCom tempLC;
      if(!tempLC.ParseAndSave("WL:L:"+fld[1],false))   // WL phase - don't save
         return false;
      // copy out results
      for(i=0; i<C.InputSyss.size(); i++) {            // loop over systems
         sys = C.map3to1Sys[C.InputSyss[i]];
         for(j=0; j<tempLC.sysConsts[sys].size(); j++) {
            sysConsts[sys].push_back(tempLC.sysConsts[sys][j]);
            sysObsids[sys].push_back(tempLC.sysObsids[sys][j]);
         }
      }
      LOG(DEBUG2) << "Parse construct " << ("NL:C:"+fld[1]);
      if(!tempLC.ParseAndSave("NL:C:"+fld[1],false))   // NL range - don't save
         return false;
      for(i=0; i<C.InputSyss.size(); i++) {            // loop over systems
         sys = C.map3to1Sys[C.InputSyss[i]];
         for(j=0; j<tempLC.sysConsts[sys].size(); j++) {
            sysConsts[sys].push_back(-tempLC.sysConsts[sys][j]);   // note minus
            sysObsids[sys].push_back(tempLC.sysObsids[sys][j]);
         }
      }
      LOG(DEBUG2) << "Parse finish construct " << tag;
   }
   else if(tag == string("NLC")) {            // NLC:ij
      if(fld.size() == 1) {                  // NLC alone : ij from --freq
         for(i=0; i<C.InputFreqs.size(); i++) {
            sysConsts.clear(); sysObsids.clear();
            if(C.InputFreqs[i].size() == 2)  // only take dual freqs
               if(! this->ParseAndSave("NLC:"+C.InputFreqs[i]))
                  return false;
         }
         return true;
      }
      else if(fld.size() != 2)
         return false;                       // not NLC and not NLC:ij

      LOG(DEBUG2) << "Parse construct " << ("NL:L:"+fld[1]);
      LinCom tempLC;
      if(!tempLC.ParseAndSave("NL:L:"+fld[1],false))   // NL phase - don't save
         return false;
      // copy out results
      for(i=0; i<C.InputSyss.size(); i++) {            // loop over systems
         sys = C.map3to1Sys[C.InputSyss[i]];
         for(j=0; j<tempLC.sysConsts[sys].size(); j++) {
            sysConsts[sys].push_back(tempLC.sysConsts[sys][j]);
            sysObsids[sys].push_back(tempLC.sysObsids[sys][j]);
         }
      }
      LOG(DEBUG2) << "Parse construct " << ("WL:C:"+fld[1]);
      if(!tempLC.ParseAndSave("WL:C:"+fld[1],false))   // WL range - don't save
         return false;
      for(i=0; i<C.InputSyss.size(); i++) {            // loop over systems
         sys = C.map3to1Sys[C.InputSyss[i]];
         for(j=0; j<tempLC.sysConsts[sys].size(); j++) {
            sysConsts[sys].push_back(-tempLC.sysConsts[sys][j]);   // note minus
            sysObsids[sys].push_back(tempLC.sysObsids[sys][j]);
         }
      }
      LOG(DEBUG2) << "Parse finish construct " << tag;
   }

   else {                                    // must be cococo.. (--combo)
      vector<string> syss,obsids;
      vector<double> consts;
      string::size_type pos;
      while(tag.size()) {
         pos = tag.find_first_of("GRESCLD"); // system or obsid
         if(pos == string::npos) break;
         if(tag.substr(0,pos).empty() || tag.substr(0,pos) == "+")
            consts.push_back(1.0);
         else if(tag.substr(0,pos) == "-")
            consts.push_back(-1.0);
         else
            consts.push_back(asDouble(tag.substr(0,pos)));
         tag = tag.substr(pos);

         pos = tag.find_first_of("+-");
         obsid = (pos == string::npos ? tag : tag.substr(0,pos));
         if(obsid.size() == 4) {
            sys = C.map1to3Sys[string(1,obsid[0])];
            if(find(syss.begin(),syss.end(),sys) == syss.end())   // syss is set of
               syss.push_back(sys);                               // unique systems
         }
         if(!isValidRinexObsID(obsid)) return false;
         obsids.push_back(obsid);
         if(pos == string::npos) break;
         tag = tag.substr(pos);
      }
      if(syss.empty()) syss = C.InputSyss;

      // copy out for each system - assume the user knows which systems are valid
      for(i=0; i<syss.size(); i++) {
         sys = C.map3to1Sys[syss[i]];
         sysConsts[sys] = consts;
         sysObsids[sys] = obsids;
      }
   }

   // check that its valid
   bool ok(true);
   for(map<string,vector<double> >::const_iterator it=sysConsts.begin();
                                                   it!=sysConsts.end(); ++it)
   {
      sys = it->first;
      LOG(DEBUG2) << "Final sys " << sys << " consts " << sysConsts[sys].size()
                                       << " obsids " << sysObsids[sys].size();
      if(sysConsts[sys].size() != sysObsids[sys].size()) { ok=false; break; }
   }

   if(!ok) return false;
   if(save) {
      LOG(DEBUG2) << "Parse saves combo >" << label << "<";
      C.Combos.push_back(*this);
   }
   else LOG(DEBUG2) << "Parse dont save >" << label << "<";

   return true;
}

//------------------------------------------------------------------------------------
double LinCom::Compute(const RinexSatID sat, Rinex3ObsHeader& Rhead,
                  const vector<RinexDatum>& vrdata) throw(Exception)
{
   Configuration& C(Configuration::Instance());

   string sys1(string(1,sat.systemChar()));
   string sys3(sat.systemString3());
   string msg("Compute " + label + " sat " + asString(sat));

   // is the system valid for this object?
   if(sysConsts.count(sys1) == 0)
      { LOG(DEBUG2) << msg << " System not found"; return 0.0; }

   // sum up the terms
   value = 0.0;      // member
   for(size_t i=0; i<sysConsts[sys1].size(); i++) {
      // convert the string to a RinexObsID
      string obsid(sysObsids[sys1][i]);
      if(obsid.size() == 4 && obsid[0] != sys1[0]) {     // system does not match
         LOG(DEBUG2) << msg << " Sys " << sys1 << " does not match obsid " << obsid;
         return 0.0;
      }

      if(obsid.size() == 3) obsid = sys1 + obsid;        // add system char to obsid
      if(!isValidRinexObsID(obsid)) {                    // TD do this earlier?
         LOG(DEBUG2) << msg << " obsid " << obsid << " not valid";
         return 0.0;                                     // obsid is not valid
      }
      RinexObsID Obsid(obsid);                           // RinexObsID for this term

      // find which code to use
      vector<RinexObsID> allObsIDs;
      if(obsid[3] == '*') {                              // try every possibility
         for(size_t j=0; j<C.mapSysCodes[sys3].size(); j++) {
            string oi(obsid.substr(0,3)+string(1,C.mapSysCodes[sys3][j]));
            if(isValidRinexObsID(oi))
               allObsIDs.push_back(RinexObsID(oi));
         }
      }
      else
         allObsIDs.push_back(Obsid);                     // only one

      // find the index of the ObsID in the header
      double data(0.0);
      vector<RinexObsID>::const_iterator jt;
      for(size_t k=0; k<allObsIDs.size(); k++) {
         string oi(sys1 + allObsIDs[k].asString());
         jt = find(Rhead.mapObsTypes[sys1].begin(),
                   Rhead.mapObsTypes[sys1].end(), allObsIDs[k]);
         if(jt == Rhead.mapObsTypes[sys1].end()) {
            LOG(DEBUG2) << msg << " obs >" << oi << "< obsid not in header";
            continue;                                    // not in header
         }

         int j = jt - Rhead.mapObsTypes[sys1].begin();
         data = vrdata[j].data;
         if(data == 0.0) {
            LOG(DEBUG2) << msg << " obs >" << oi << "< data is zero";
            value = 0.0; return value;
            //continue;                                    // data is missing
         }

         obsid = oi;
         break;                                          // found one, not zero
      }

      string msg2(" obs >" + obsid + "<");
      if(data == 0.0) {
         LOG(DEBUG2) << msg << msg2 << " no data";
         return 0.0;
      }

      // if this is phase data, multiply by the wavelength
      if(obsid[1] == 'L') {
         int N(0);
         if(sys1[0] == 'R') {
            if(C.GLOfreqChan.count(sat))
               N = C.GLOfreqChan[sat];
            else {
               LOG(WARNING) << "No frequency channel for GLO sat " << sat;
               return 0.0;
            }
         }
         data *= getWavelength(sat, asInt(string(1,obsid[2])), N);
      }

      LOG(DEBUG2) << msg << msg2 << " ok, sum: " << fixed << setprecision(4)
         << sysConsts[sys1][i] << " * " << data;
      value += sysConsts[sys1][i] * data;
   }

   return value;     // also member data
}

//------------------------------------------------------------------------------------
// Reset bias when jump in value exceeds limit.
// Set initial bias to 0 if initial value is < limit, otherwise to value.
// Save previous value and debias value.
bool LinCom::removeBias(const RinexSatID& sat) throw()
{
   bool reset(false);
   if(!limit0 && limit == 0.0) return reset;

   if(biases.find(sat) == biases.end()) {    // sat not found - initial point
      if(limit0 || (limit > 0.0 && ::fabs(value) > limit)) {
         biases[sat] = value;
         reset = true;
      }
      else
         biases[sat] = 0.0;
   }

   // this is the test
   if(limit > 0.0 && ::fabs(value-prev[sat]) > limit) {
      biases[sat] = value;                   // this makes value-bias = 0
      reset = true;
   }

   prev[sat] = value;
   value -= biases[sat];
   return reset;
}

//------------------------------------------------------------------------------------
ostream& operator<<(ostream& os, LinCom& lc) throw()
{
   ostringstream oss;
   oss << "Dump LC " << lc.label << " freq " << lc.f1 << "," << lc.f2
      << " limit " << fixed << setprecision(3) << lc.limit
      << " limit0 " << (lc.limit0 ? "T":"F");
   map<string, vector<double> >::const_iterator it;
   it = lc.sysConsts.begin();
   while(it != lc.sysConsts.end()) {
      string sys = it->first;
      oss << "  Sys " << sys << ":";
      for(size_t i=0; i<it->second.size(); ++i)
         oss << (i==0 ? " " : " + ") << lc.sysConsts[sys][i]
            << " * " << lc.sysObsids[sys][i];
      ++it;
   }
   os << oss.str();
   return os;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
