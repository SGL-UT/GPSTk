#pragma ident "$Id$"

/// @file PRSolve.cpp
/// Read Rinex observation files (version 2 or 3) and ephemeris store, and compute a
/// a pseudorange-only position solution.

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

// system
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>

// GPSTK
#include "Exception.hpp"
#include "MathBase.hpp"
#include "GNSSconstants.hpp"
#include "geometry.hpp"

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

#include "RinexMetHeader.hpp"
#include "RinexMetData.hpp"
#include "RinexMetStream.hpp"

#include "SP3Header.hpp"
#include "SP3Data.hpp"
#include "SP3Stream.hpp"

#include "SP3EphemerisStore.hpp"
#include "Rinex3EphemerisStore.hpp"

#include "Position.hpp"
#include "TropModel.hpp"
#include "EphemerisRange.hpp"
#include "RinexUtilities.hpp"

#include "Stats.hpp"
#include "Namelist.hpp"

#include "PRSolution.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
string Version(string("4.3 8/3/12"));

// forward declaration
class SolutionObject;

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

   // Build solution descriptors from the input - separate routine b/c its complicated
   void BuildSolDescriptors(ostringstream& oss) throw();

   // update weather in the trop model using the Met store
   void setWeather(const CommonTime& ttag) throw(Exception);

private:

   // Define default values
   void SetDefaults(void) throw();

public:

// member data
   CommandLine opts;             // command line options and syntax page
   static const string PrgmName; // program name
   string Title;                 // id line printed to screen and log

   // start command line input
   bool help, verbose;
   int debug;
   string filedummy;

   vector<string> InputObsFiles; // RINEX obs file names
   vector<string> InputSP3Files; // SP3 ephemeris+clock file names
   vector<string> InputClkFiles; // RINEX clock file names
   vector<string> InputNavFiles; // RINEX nav file names
   vector<string> InputMetFiles; // RINEX met file names
   vector<string> InputDCBFiles; // differential code bias C1-P1 file names

   string Obspath,SP3path,Clkpath,Navpath,Metpath,DCBpath,ANTpath;      // paths

   // times derived from --start and --stop
   string defaultstartStr,startStr;
   string defaultstopStr,stopStr;
   CommonTime beginTime,endTime,gpsBeginTime,decTime;

   double decimate;           // decimate input data
   double elevLimit;          // limit sats to elevation mask
   bool forceElev;            // use elevLimit even without --ref
   vector<RinexSatID> exclSat;// exclude satellites

   bool SPSout,ORDout;        // output autonomous solutions? ORDs?
   bool outver2;              // output RINEX version 2 (OutputObsFile)
   string LogFile;            // output log file (required)
   string OutputORDFile;      // output ORD file
   string OutputObsFile;      // output RINEX obs file
   string userfmt;            // user's time format for output
   string refPosStr;          // temp used to parse --ref input

   vector<string> inSolDesc;  // input: strings sys,freq,code e.g. GPS+GLO,1+2,PC
   string inSolSys;           // input: systems to compute: GPS,GLO,GPS+GLO,etc
   vector<string> inSolCode;  // input: codes (RINEX track char) to select, in order
   vector<string> inSolFreq;  // input: frequency (e.g. 1,2,5,12,25,12+15) to process

   // config for PRSolution
   bool weight;               // build a measurement covariance if true
   double RMSLimit;           // Upper limit on RMS post-fit residual (m)
   double SlopeLimit;         // Upper limit on RAIM 'slope'
   int maxReject;             // Max number of sats to reject [-1 for no limit]
   int nIter;                 // Maximum iteration count in linearized LS
   double convLimit;          // Minimum convergence criterion in estimation (meters)

   string TropStr;            // temp used to parse --trop

   // end of command line input

   // output file streams
   ofstream logstrm, ordstrm; // for LogFile, OutputORDFile

   // time formats
   static const string calfmt, gpsfmt, longfmt;

   // stores
   XvtStore<SatID> *pEph;
   SP3EphemerisStore SP3EphStore;
   Rinex3EphemerisStore RinEphStore;
   //GPSEphemerisStore GPSEphStore;
   //GloEphemerisStore GLOEphStore;
   list<RinexMetData> MetStore;
   map<RinexSatID,double> P1C1bias;
   map<RinexSatID,int> GLOfreqChannel;

   // trop models
   TropModel *pTrop;          // to pass to PRS
   string TropType;           // key ~ Black, NewB, etc; use to identify model
   bool TropPos,TropTime;     // true when trop model has been init with Pos,time
                              // default weather
   double defaultTemp,defaultPress,defaultHumid;

   // solutions to build
   vector<string> vecSys;           // list of systems allowed
   map<string,string> mapSysCodes;  // map of system:allowed codes e.g. GLO:PC
   map<string,string> defMapSysCodes;  // map of system:default codes
   vector<string> SolDesc;          // strings sys,freq,code e.g. GPS+GLO,1+2,PWXC+PC
   vector<SolutionObject> SolObjs;  // solution objects to process

   // reference position and rotation matrix
   Position knownPos;         // position derived from --ref
   Matrix<double> Rot;        // Rotation matrix (R*XYZ=NEU) :

   // useful stuff
   string msg;                      // temp used everywhere
   map<string,string> map1to3Sys;   // [G]=GPS [R]=GLO etc
   map<string,string> map3to1Sys;   // [GPS]=G [GLO]=R etc
   // vector of 1-char strings containing systems needed in all solutions: G,R,E,C,S
   vector<string> allsyss;

   // constants
   static const double beta12GPS, beta12GLO, beta15GPS, beta25GPS;
   static const double alpha12GPS, alpha12GLO, alpha15GPS, alpha25GPS;

}; // end class Configuration

//------------------------------------------------------------------------------------
// const members of Configuration
const string Configuration::PrgmName = string("PRSolve");
const string Configuration::calfmt = string("%04Y/%02m/%02d %02H:%02M:%02S");
const string Configuration::gpsfmt = string("%4F %10.3g");
const string Configuration::longfmt = calfmt + " = %4F %w %10.3g %P";
const double Configuration::beta12GPS = L1_MULT_GPS/L2_MULT_GPS;
const double Configuration::beta12GLO = 9./7.;
const double Configuration::beta15GPS = L1_MULT_GPS/L5_MULT_GPS;
const double Configuration::beta25GPS = L2_MULT_GPS/L5_MULT_GPS;
const double Configuration::alpha12GPS = beta12GPS*beta12GPS-1.0;
const double Configuration::alpha12GLO = beta12GLO*beta12GLO-1.0;
const double Configuration::alpha15GPS = beta15GPS*beta15GPS-1.0;
const double Configuration::alpha25GPS = beta25GPS*beta25GPS-1.0;

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// Object to encapsulate everything for one solution (system:freq:code)
class SolutionObject {
public:
// member functions
   // Default and only constructor
   SolutionObject(const string& desc) throw() { Initialize(desc); }

   // Destructor
   ~SolutionObject() throw() { }

   // check validity of input descriptor, set default values
   void Initialize(const string& desc) throw()
   {
      nepochs = 0;
      Descriptor = desc;
      ParseDescriptor();

      // TD test for validity?
      isValid = true;

      Configuration& C(Configuration::Instance());
      b12["G"] = -1.0/C.alpha12GPS;
      a12["G"] = 1.0 - b12["G"];
      b15["G"] = -1.0/C.alpha15GPS;
      a15["G"] = 1.0 - b15["G"];
      b25["G"] = -1.0/C.alpha25GPS;
      a25["G"] = 1.0 - b25["G"];

      b12["R"] = -1.0/C.alpha12GLO;
      a12["R"] = 1.0 - b12["R"];
      a15["R"] = b15["R"] = a25["R"] = b25["R"] = 0.0;

      // others TBD
      a12["E"] = b12["E"] = a15["E"] = b15["E"] = a25["E"] = b25["E"] = 0.0;
      a12["S"] = b12["S"] = a15["S"] = b15["S"] = a25["S"] = b25["S"] = 0.0;
      a12["C"] = b12["C"] = a15["C"] = b15["C"] = a25["C"] = b25["C"] = 0.0;

      //LOG(INFO) << fixed << setprecision(6)
      //   << " beta12GPS = " << C.beta12GPS << "\n"
      //   << " beta12GLO = " << C.beta12GLO << "\n"
      //   << " beta15GPS = " << C.beta15GPS << "\n"
      //   << " beta25GPS = " << C.beta25GPS << "\n"
      //   << " alpha12GPS = " << C.alpha12GPS << "\n"
      //   << " alpha12GLO = " << C.alpha12GLO << "\n"
      //   << " alpha15GPS = " << C.alpha15GPS << "\n"
      //   << " alpha25GPS = " << C.alpha25GPS << "\n"
      //   << " GPS a12 b12 " << a12["G"] << " " << b12["G"] << "\n"
      //   << " GLO a12 b12 " << a12["R"] << " " << b12["R"] << "\n"
      //   << " GPS a15 b15 " << a15["G"] << " " << b15["G"] << "\n"
      //   << " GPS a25 b25 " << a25["G"] << " " << b25["G"];
   }

   // parse descriptor into member data 'freqs', 'syss', and 'syscodes'
   void ParseDescriptor(void) throw();

   // set defaults, mostly from configuration
   void SetDefaults(void) throw();

   // Given a RINEX header, verify that the necessary ObsIDs are present, and
   // define an ordered set of ObsIDs for each slot required.
   // Return true if enough ObsIDs are found to compute the solution.
   bool ChooseObsIDs(map<string,vector<RinexObsID> >& mapObsTypes) throw();

   // dump. level 0: descriptor and all available obs types
   //       level 1: descriptor and obs types actually used
   //       level 2: level 1 plus pseudorange data
   // return string containing dump
   string dump(int level, string msg="SOLN", string msg2="") throw();

   // reset the object before each epoch
   void EpochReset(void) throw();

   // Given a RINEX data object, pull out the data to be used, and set the flag
   // indicating whether there is sufficient good data.
   void CollectData(const RinexSatID& s,
                    const double& elev, const double& ER,
                    const vector<RinexDatum>& v) throw();

   // Compute a solution for the given epoch; call after CollectData()
   // same return value as RAIMCompute()
   int ComputeSolution(const CommonTime& t) throw(Exception);

   // Write out ORDs - call after ComputeSolution
   int WriteORDs(const CommonTime& t) throw(Exception);

   // Output final results
   void FinalOutput(void) throw(Exception);

// member data

   // linear combination constants
   map<string,double> a12, b12, a15, b15, a25, b25;

   // true unless descriptor is not valid, or required ObsIDs are not available
   bool isValid;

   // solution descriptor: sys[+sys]:freq[+freq]:codes[+codes]
   // sys+sys implies codes+codes; codes is string of 'attribute' characters (ObsID)
   // giving the preferred ObsID (sys/C/freq/attr) to use as the data
   string Descriptor;

   // frequency combinations needed in this solution, e.g. "12" "15" or "12,15"
   vector<string> freqs;
   // string contining all the frequencies without repetition
   string ufreqs;

   // vector of 1-char strings containing systems needed in this solution: G,R,E,C,S
   vector<string> syss;
   // vector of satellite systems parallel to syss
   vector<SatID::SatelliteSystem> Syss;

   // map of 1-char system strings to strings containing attribute characters,
   // in order, of ObsIDs needed in this solution, e.g. [G]=CWXP
   map<string,string> syscodes;

   // list of ObsIDs needed for this solution and available from RINEX header
   // for each system and frequency
   map<string, map<string, vector<string> > > mapSysFreqObsIDs;

   // map of ObsIDs to indexes in RINEX header mapObsTypes, also RINEX data vector
   map<string, int> mapObsIndex;

   // data for PR solution algorithm
   bool hasData;                             // true if enough data for solution
   vector<SatID> Satellites;                 // sats with data
   vector<double> PRanges;                   // data, parallel to Satellites
   vector<double> Elevations;                // elevations, parallel to Satellites
   vector<double> ERanges;                   // corr eph range, parallel to Satellites
   vector<double> RIono;                     // range iono, parallel to Satellites
   vector<double> R1,R2;                     // raw ranges, parallel to Satellites
   multimap<RinexSatID,string> UsedObsIDs;   // valid or not; may be comma-sep. list

   // the PRS itself
   PRSolution prs;

   // statistics on the solution residuals
   int nepochs;
   WtdAveStats statsXYZresid;                // RPF (XYZ) minus reference position
   WtdAveStats statsNEUresid;                // RNE above rotated into local NEU
   //WtdAveStats statsSPSXYZresid;             // SPF (XYZ) minus reference position
   //WtdAveStats statsSPSNEUresid;             // SNE above rotated into local NEU

}; // end class SolutionObject

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
   C.Title = C.PrgmName + ", part of the GPS Toolkit, Ver " + Version
      + ", Run " + printTime(wallclkbeg,C.calfmt);
   cout << C.Title << endl;

   for(;;) {
      // get information from the command line
      // iret -2 -3 -4
      if((iret = C.ProcessUserInput(argc, argv)) != 0) break;

      // read stores, check input etc
      string errs;
      if((iret = Initialize(errs)) != 0) {
         LOG(ERROR) << "------- Input is not valid: ----------\n" << errs
                    << "------- end errors -----------";
         break;
      }

      // open files, read, compute solutions and output
      int nfiles = ProcessFiles();
      if(nfiles < 0) break;
      LOG(VERBOSE) << "Successfully read " << nfiles
         << " RINEX observation file" << (nfiles > 1 ? "s.":".");

      // output final results
      for(int i=0; i<C.SolObjs.size(); ++i) {
         LOG(INFO) << "\n ----- Final output " << C.SolObjs[i].Descriptor << " -----";
         C.SolObjs[i].FinalOutput();
      }

      break;      // mandatory
   }

   if(iret == 0) {
      // print elapsed time
      totaltime = clock()-totaltime;
      Epoch wallclkend;
      wallclkend.setLocalTime();
      ostringstream oss;
      oss << C.PrgmName << " timing: processing " << fixed << setprecision(3)
         << double(totaltime)/double(CLOCKS_PER_SEC) << " sec, wallclock: "
         << setprecision(0) << (wallclkend-wallclkbeg) << " sec.";
      LOG(INFO) << oss.str();
      cout << oss.str() << endl;
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
   Configuration& C(Configuration::Instance());
   bool isValid(true);
   int i,j,nfile,nread,nrec;
   ostringstream ossE;
   CommonTime typtime;        // typical time for the dataset

   errors = string("");

   // add path to filenames, and expand tilde (~)
   include_path(C.Obspath, C.InputObsFiles);
   include_path(C.SP3path, C.InputSP3Files);
   include_path(C.Clkpath, C.InputClkFiles);
   include_path(C.Navpath, C.InputNavFiles);
   include_path(C.Metpath, C.InputMetFiles);
   include_path(C.DCBpath, C.InputDCBFiles);

   //expand_filename(C.InputObsFiles);
   expand_filename(C.InputSP3Files);
   expand_filename(C.InputClkFiles);
   expand_filename(C.InputNavFiles);
   expand_filename(C.InputMetFiles);
   expand_filename(C.InputDCBFiles);

   // -------- quick check that obs files exist and are RINEX -------
   if(C.InputObsFiles.size() > 0) {
      try {
         for(nread=0,nfile=0; nfile<C.InputObsFiles.size(); nfile++) {
            Rinex3ObsStream rostrm(C.InputObsFiles[nfile].c_str(), ios_base::in);
            if(!rostrm.is_open()) {
               ossE << "Error : failed to open RINEX obs file: "
                  << C.InputObsFiles[nfile] << endl;
               isValid = false;
               continue;
            }
            Rinex3ObsHeader rhead;
            rostrm >> rhead;

            typtime = rhead.firstObs.convertToCommonTime();
            typtime.setTimeSystem(TimeSystem::Any);

            rostrm.close();

            if(!isRinex3ObsFile(C.InputObsFiles[nfile])) {
               ossE << "Error : File: " << C.InputObsFiles[nfile]
                  << " is not a valid RINEX file." << endl;
               isValid = false;
            }
            nread++;
            LOG(VERBOSE) << "Found RINEX obs file " << C.InputObsFiles[nfile];
         }
      }
      catch(Exception& e) {
         ossE << "Error : failed to read RINEX obs files: " << e.getText(0) << endl;
         isValid = false;
      }

   }
   else {
      ossE << "Error : no RINEX observation files specified.\n";
      isValid = false;
   }

   // -------- RINEX clock files --------------------------
   // Read RINEX clock files for clock (only) store.
   // This will set the clock store to use RINEX clock rather than SP3.
   // Read clock files before SP3, in case there are none and SP3 clock to be used.
   if(C.InputClkFiles.size() > 0) {
      try {
         for(nread=0,nfile=0; nfile<C.InputClkFiles.size(); nfile++) {
            LOG(VERBOSE) << "Load Clock file " << C.InputClkFiles[nfile];
            C.SP3EphStore.loadRinexClockFile(C.InputClkFiles[nfile]);
            nread++;
         }
      }
      catch(Exception& e) {
         ossE << "Error : failed to read RINEX clock files: " << e.getText(0) << endl;
         isValid = false;
      }

      LOG(VERBOSE) << "Read " << nread << " RINEX clock files into store.\n"
         << "RINEX clock file store contains " << C.SP3EphStore.ndataClock()
         << " data.";
   }
   else LOG(VERBOSE) << "No RINEX clock files";

   // -------- SP3 files --------------------------
   // read ephemeris files and fill store
   // first sort them on start time; this for ultra-rapid files, which overlap in time
   if(C.InputSP3Files.size() > 0) {
      ostringstream os;
      multimap<CommonTime,string> startNameMap;
      for(nfile=0; nfile<C.InputSP3Files.size(); nfile++) {
         SP3Header header;
         try {
            SP3Stream strm(C.InputSP3Files[nfile].c_str());
            if(!strm.is_open()) {
               os << "Failed to open file " << C.InputSP3Files[nfile] << endl;
               isValid = false;
               continue;
            }

            strm.exceptions(ios_base::failbit);
            strm >> header;
         }
         catch(Exception& e) {
            os << "Exception: " << e.what() << endl; isValid = false; continue; }
         catch(exception& e) {
            os << "exception: " << e.what(); isValid = false; continue; }
         startNameMap.insert(multimap<CommonTime,string>::value_type(
               header.time,C.InputSP3Files[nfile]));
      }

      ossE << os.str();
      C.InputSP3Files.clear();
      for(multimap<CommonTime,string>::const_iterator it = startNameMap.begin();
                                                      it != startNameMap.end(); ++it)
         C.InputSP3Files.push_back(it->second);

      // read sorted ephemeris files and fill store
      if(isValid) {
         try {
            if(isValid) for(nread=0,nfile=0; nfile<C.InputSP3Files.size(); nfile++) {
               LOG(VERBOSE) << "Load SP3 file " << C.InputSP3Files[nfile];
               C.SP3EphStore.loadSP3File(C.InputSP3Files[nfile]);
               nread++;
            }
         }
         catch(Exception& e) {
            ossE << "Error : failed to read ephemeris files: "
               << e.getText(0) << endl;
            isValid = false;
         }

      }
   }

   // ------------- configure and dump SP3 and clock stores -----------------
   if(isValid && C.SP3EphStore.ndata() > 0) {
      LOG(VERBOSE) << "Read " << nread << " SP3 ephemeris files into store.";
      LOG(VERBOSE) << "SP3 Ephemeris store contains "
         << C.SP3EphStore.ndata() << " data";

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
      LOG(VERBOSE) << "\nDump clock and position stores, including file stores";
      // NB clock dumps are huge!
      if(C.verbose) C.SP3EphStore.dump(LOGstrm, (C.debug > 6 ? 2 : 1));
      LOG(VERBOSE) << "End of clock store and ephemeris store dumps.";

      // dump a list of satellites, with counts, times and GLO channel
      C.msg = "";
      LOG(INFO) << "\nDump ephemeris sat list with count, times and GLO channel.";
      vector<SatID> sats(C.SP3EphStore.getSatList());
      for(i=0; i<sats.size(); i++) {                           // loop over sats
         // check for some GLO channel - can't compute b/c we don't have data yet
         if(sats[i].system == SatID::systemGlonass) {
            map<RinexSatID,int>::const_iterator it(C.GLOfreqChannel.find(sats[i]));
            if(it == C.GLOfreqChannel.end()
                           && sats[i].system == RinexSatID::systemGlonass) {
               //LOG(WARNING) << "Warning - no input GLONASS frequency channel "
               //   << "for satellite " << RinexSatID(sats[i]);
               // set it to zero
               C.GLOfreqChannel.insert(map<RinexSatID, int>::value_type(sats[i], 0));
               it = C.GLOfreqChannel.find(sats[i]);
            }
            C.msg = string(" frch ") + rightJustify(asString(it->second),2);
         }

         LOG(INFO) << " Sat: " << RinexSatID(sats[i])
            << " Neph: " << setw(3) << C.SP3EphStore.ndata(sats[i])
            << " Beg: " << printTime(C.SP3EphStore.getInitialTime(sats[i]),C.longfmt)
            << " End: " << printTime(C.SP3EphStore.getFinalTime(sats[i]),C.longfmt)
            << C.msg;
      }  // end loop over sats

      RinexSatID sat(sats[0]);
      LOG(VERBOSE) << "\nEphemeris Store time intervals for " << sat
         << " are " << C.SP3EphStore.getPositionTimeStep(sat)
         << " (pos), and " << C.SP3EphStore.getClockTimeStep(sat) << " (clk)";
      sat = RinexSatID(sats[sats.size()-1]);
      LOG(VERBOSE) << "Ephemeris Store time intervals for " << sat
         << " are " << C.SP3EphStore.getPositionTimeStep(sat)
         << " (pos), and " << C.SP3EphStore.getClockTimeStep(sat) << " (clk)";
   }

   // -------- Nav files --------------------------
   // NB Nav files may set GLOfreqChannel
   if(C.InputNavFiles.size() > 0) {
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

            if(C.verbose) {
               LOG(VERBOSE) << "Read " << n << " ephemeris data from file "
                  << filename << "; header follows.";
               C.RinEphStore.Rhead.dump(LOGstrm);
            }
         }  // end loop over InputNavFiles

         // expand the network of time system corrections
         C.RinEphStore.expandTimeCorrMap();
      }
      catch(Exception& e) {
         ossE << "Error : while reading RINEX nav files: " << e.what() << endl;
         isValid = false;
      }

      if(isValid) {
         LOG(VERBOSE) << "Read " << nread << " RINEX navigation files, containing "
            << nrec << " records, into store.";
         LOG(VERBOSE) << "GPS ephemeris store contains "
            << C.RinEphStore.size(SatID::systemGPS) << " ephemerides.";
         LOG(VERBOSE) << "GLO ephemeris store contains "
            << C.RinEphStore.size(SatID::systemGlonass) << " satellites.";
         // TD temp? debug?
         C.RinEphStore.dump(LOGstrm);
      }
   }

   // assign pEph
   if(isValid) {
      if(C.SP3EphStore.ndata() > 0)
         C.pEph = &C.SP3EphStore;
      else if(C.RinEphStore.size() > 0)
         C.pEph = &C.RinEphStore;
   }

   // -------- Met files --------------------------
   // get met files and build MetStore
   if(C.InputMetFiles.size() > 0) {
      try {
         for(nfile=0; nfile < C.InputMetFiles.size(); nfile++) {
            RinexMetStream mstrm(C.InputMetFiles[nfile].c_str());
            if(!mstrm.is_open()) {
               ossE << "Error : failed to open RINEX meteorological file "
                  << C.InputMetFiles[nfile] << endl;
               isValid = false;
               continue;
            }
            mstrm.exceptions(ios_base::failbit);

            RinexMetHeader mhead;
            RinexMetData mdata;

            mstrm >> mhead;
            while(mstrm >> mdata)
               C.MetStore.push_back(mdata);

            mstrm.close();
         }  // end loop over met file names

         // sort on time
         C.MetStore.sort();

         // dump
         if(isValid && C.verbose) {
            list<RinexMetData>::const_iterator it = C.MetStore.begin();
            LOG(VERBOSE) << "Meteorological store contains "
               << C.MetStore.size() << " records:";
            if(C.MetStore.size() > 0) {
               it = C.MetStore.begin();
               if(C.MetStore.size() == 1)
                  {LOG(VERBOSE) << "  Met store is at single time "
                     << printTime(it->time,C.longfmt);}
               else {
                  LOG(VERBOSE) << "  Met store starts at time "
                     << printTime(it->time,C.longfmt);
                  it = C.MetStore.end();
                  it--;
                  LOG(VERBOSE) << "  Met store   ends at time "
                     << printTime(it->time,C.longfmt);
               }
            }

            if(C.debug > -1) {
               LOG(DEBUG) << "Dump of meteorological data store ("
                  << C.MetStore.size() << "):";
               for(it = C.MetStore.begin(); it != C.MetStore.end(); it++) {
                  ostringstream os;
                  os << printTime(it->time,C.longfmt) << fixed << setprecision(1);
                  RinexMetData::RinexMetMap::const_iterator jt=it->data.begin();
                  for( ; jt != it->data.end(); jt++) {
                     os << "  " << RinexMetHeader::convertObsType(jt->first)
                        << " = " << setw(6) << jt->second;
                  }
                  LOG(DEBUG) << os.str();
               }
               LOG(DEBUG) << "End dump of meteorological data store.";
            }

            if(C.MetStore.size() == 0) {
               C.InputMetFiles.clear();
               LOG(WARNING) << "Warning : Met data store is empty - clear file names";
            }
         }
      }
      catch(Exception& e) {
         ossE << "Error : failed to read meteorological files: " << e.what() << endl;
         isValid = false;
         C.MetStore.clear();
      }
   }  // end if there are met file names

   // -------- DCB files --------------------------
   // load the P1C1 files
   if(C.InputDCBFiles.size() > 0) {
      for(nfile=0; nfile<C.InputDCBFiles.size(); nfile++) {
         string line,word,filename(C.InputDCBFiles[nfile]);
         RinexSatID sat;
         ifstream ifs(filename.c_str());
         if(!ifs.is_open()) {
            ossE << "Error : Failed to open P1-C1 bias file name " << filename <<endl;
            isValid = false;
            continue;
         }
         LOG(VERBOSE) << "Opened P1C1 file " << filename;

         // loop over lines in the file
         while(1) {
            getline(ifs,line);
            if(ifs.eof() || !ifs.good()) break;

            stripTrailing(line,"\r\n");   // remove trailing CRLF
            stripLeading(line," \t");     // remove leading whitespace
            if(line.empty()) continue;    // skip empty lines

            word = stripFirstWord(line);  // get sat
            if(word.empty()) continue;

            try { sat.fromString(word); }
            catch(Exception& e) { continue; }
            if(sat.system == SatID::systemUnknown || sat.id == -1) continue;

            word = stripFirstWord(line);  // get bias
            if(word.empty()) continue;
            if(!isScientificString(word)) continue;
            double bias(asDouble(word)*C_MPS*1.e-9);  // ns to m

            if(C.P1C1bias.find(sat) != C.P1C1bias.end())
               {LOG(WARNING) << "Warning : satellite " << sat
                  << " is duplicated in P1-C1 bias file(s)";}
            else {
               C.P1C1bias.insert(map<RinexSatID,double>::value_type(sat,bias));
               LOG(DEBUG) << " Found P1-C1 bias for sat " << sat
                  << " = " << setw(6) << word << " ns = "
                  << fixed << setprecision(3) << setw(6) << bias
                  << " m (from " << filename << ")";
            }
         }
      }
   }  // end InputP1C1Name processing

   // ------ compute and save a reference time for decimation
   if(C.decimate > 0.0) {
      C.decTime = C.beginTime;
      double s,sow(static_cast<GPSWeekSecond>(C.decTime).sow);
      s = int(C.decimate * int(sow/C.decimate));
      if(::fabs(s-sow) > 1.0) { LOG(WARNING) << "Warning : decimation reference time "
         << "(--start) is not an even GPS-seconds-of-week mark."; }
   }

   // ------ compute rotation matrix for knownPos
   if(C.knownPos.getCoordinateSystem() != Position::Unknown) {
      double lat = C.knownPos.geodeticLatitude() * DEG_TO_RAD;
      double lon = C.knownPos.longitude() * DEG_TO_RAD;
      double ca = ::cos(lat);
      double sa = ::sin(lat);
      double co = ::cos(lon);
      double so = ::sin(lon);
      // Rotation matrix (R*XYZ=NEU) :
      C.Rot = Matrix<double>(3,3);
      // NEU
      C.Rot(2,0) =  ca*co; C.Rot(2,1) =  ca*so; C.Rot(2,2) =  sa;
      C.Rot(1,0) =    -so; C.Rot(1,1) =     co; C.Rot(1,2) = 0.0;
      C.Rot(0,0) = -sa*co; C.Rot(0,1) = -sa*so; C.Rot(0,2) =  ca;
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

   if(C.beginTime != C.gpsBeginTime) {
      C.pTrop->setDayOfYear(static_cast<YDSTime>(C.beginTime).doy);
      C.TropTime = true;
   }
   else if(C.endTime != CommonTime::END_OF_TIME) {
      C.pTrop->setDayOfYear(static_cast<YDSTime>(C.endTime).doy);
      C.TropTime = true;
   }
   else
      C.pTrop->setDayOfYear(100);

   // ------------ build SolutionObjects from solution descriptors SolDesc -----
   // these may be invalid, or there may not be data for them -> invalid
   for(j=0,i=0; i<C.SolDesc.size(); i++) {
      LOG(DEBUG) << "Build solution object from descriptor " << C.SolDesc[i];
      SolutionObject so(C.SolDesc[i]);
      if(!so.isValid) {
         LOG(WARNING) << "Warning : solution descriptor " << C.SolDesc[i]
            << " is invalid - ignore";
         continue;
      }

      so.SetDefaults();    // NB. requires C.knownPos and so.syss

      C.SolObjs.push_back(so);
      LOG(DEBUG) << "Initial solution #" << ++j << " " << C.SolDesc[i];
   }

   // keep a list of all systems used, for convenience
   C.allsyss.clear();
   for(i=0; i<C.SolObjs.size(); i++) {
      for(j=0; j<C.SolObjs[i].syss.size(); j++) {
         if(find(C.allsyss.begin(), C.allsyss.end(), C.SolObjs[i].syss[j])
                                                               == C.allsyss.end())
            C.allsyss.push_back(C.SolObjs[i].syss[j]);
      }
   }
   if(C.debug > -1) {
      ostringstream oss;
      oss << "List of all systems needed for solutions";
      for(i=0; i<C.allsyss.size(); i++) oss << " " << C.allsyss[i];
      LOG(DEBUG) << oss.str();
   }

   // save errors and output
   errors = ossE.str();

   if(!isValid) return -5;
   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end Initialize()

//------------------------------------------------------------------------------------
// Return 0 ok, >0 number of files successfully read, <0 fatal error
int ProcessFiles(void) throw(Exception)
{
try {
   Configuration& C(Configuration::Instance());
   bool firstepoch(true);
   int i,j,k,iret,nfile,nfiles;
   Position PrevPos(C.knownPos);
   Rinex3ObsStream ostrm;

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
         LOG(VERBOSE) << "Opened input file " << filename;
      istrm.exceptions(ios::failbit);

      // read the header ----------------------------------------------
      try { istrm >> Rhead; }
      catch(Exception& e) {
         LOG(WARNING) << "Warning : Failed to read header; dump follows.";
         Rhead.dump(LOGstrm);
         istrm.close();
         iret = 2;
         continue;
      }
      if(C.verbose) {
         LOG(VERBOSE) << "Input header for RINEX file " << filename;
         Rhead.dump(LOGstrm);
      }

      // does header include C1C (for DCB correction)?
      bool DCBcorr(false);
      map<string,int> mapDCBindex;
      for(;;) {
         map<string,vector<RinexObsID> >::const_iterator sit;
         sit = Rhead.mapObsTypes.begin();
         for( ; sit != Rhead.mapObsTypes.end(); ++sit) {
            for(i=0; i<sit->second.size(); i++) {
               if(asString(sit->second[i]) == string("C1C")) {
                  DCBcorr = true;
                  mapDCBindex.insert(map<string,int>::value_type(sit->first,i));
                  LOG(DEBUG) << "Correct for DCB: found " << asString(sit->second[i])
                     << " for system " << sit->first << " at index " << i;
                  break;
               }
            }
         }
         break;
      }

      // do on first epoch only
      if(firstepoch) {
         // if writing to output RINEX, open and write header ---------
         if(!C.OutputObsFile.empty()) {
            ostrm.open(C.OutputObsFile.c_str(),ios::out);
            if(!ostrm.is_open()) {
               LOG(WARNING) << "Warning : could not open output file "
                  << C.OutputObsFile;
               C.OutputObsFile = string();
            }
            else {
               LOG(VERBOSE) << "Opened output RINEX file " << C.OutputObsFile;
               ostrm.exceptions(ios::failbit);

               // copy header and modify it?
               Rheadout = Rhead;
               Rheadout.fileProgram = C.PrgmName;

               // output version 2
               if(C.outver2)
                  Rheadout.PrepareVer2Write();

               ostrm << Rheadout;
            }
         }

         // if writing out ORDs, open the file
         if(!C.OutputORDFile.empty()) {
            C.ordstrm.open(C.OutputORDFile.c_str(),ios::out);
            if(!C.ordstrm.is_open()) {
               LOG(WARNING) << "Warning : failed to open output ORDs file "
                  << C.OutputORDFile << " - abort ORD output.";
               C.ORDout = false;
            }
            else {
               C.ORDout = true;
               // write header
               C.ordstrm << "ORD sat week  sec-of-wk   elev   iono     ORD1"
                  << "     ORD2      ORD    Clock  Solution_descriptor\n";
            }
         }

         firstepoch = false;
      }

      // figure out where the desired pseudoranges are ----------------
      LOG(INFO) << "Solutions to be computed for this file:";
      for(i=0; i<C.SolObjs.size(); ++i) {
         bool ok(C.SolObjs[i].ChooseObsIDs(Rhead.mapObsTypes));
         //LOG(INFO) << (ok ? "    ":" NO ") << i+1 << " " << C.SolObjs[i].dump(0);
         LOG(INFO) << C.SolObjs[i].dump(0);
      }

      //GPSWeekSecond g(1577,345600.);
      //CommonTime c(g); //,d(g.convertToCommonTime());
      // e=static_cast<CommonTime>(g);
      //LOG(INFO) << "GPSWS->CT " << printTime(g,"g %F %10.3g = g %Y,%m,%d,%H,%M,%S")
      //         << " = " << printTime(c,"c %F %10.3g = c %Y,%m,%d,%H,%M,%S")
      //         //<< " = " << printTime(d,"d %F %10.3g = d %Y,%m,%d,%H,%M,%S")
      //         ;

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
         catch(exception& e) {
            Exception ge(string("Std excep: ") + e.what());
            GPSTK_THROW(ge);
         }
         catch(...) {
            Exception ue("Unknown exception while reading RINEX data.");
            GPSTK_THROW(ue);
         }

         // normal EOF
         if(!istrm.good() || istrm.eof()) { iret = 0; break; }

         // if aux header data, or no data, skip it
         if(Rdata.epochFlag > 1 || Rdata.obs.empty()) {
            LOG(DEBUG) << " RINEX Data is aux header or empty.";
            continue;
         }

         LOG(DEBUG) << "\n Read RINEX data: flag " << Rdata.epochFlag
            << ", timetag " << printTime(Rdata.time,C.longfmt);

         // stay within time limits
         if(Rdata.time < C.beginTime) {
            LOG(DEBUG) << " RINEX data timetag " << printTime(C.beginTime,C.longfmt)
               << " is before begin time.";
            continue;
         }
         if(Rdata.time > C.endTime) {
            LOG(DEBUG) << " RINEX data timetag " << printTime(C.endTime,C.longfmt)
               << " is after end time.";
            break;
         }

         // decimate
         if(C.decimate > 0.0) {
            double dt(::fabs(Rdata.time - C.decTime));
            dt -= C.decimate * long(0.5 + dt/C.decimate);
            if(::fabs(dt) > 0.25) {
               LOG(DEBUG) << " Decimation rejects RINEX data timetag "
                  << printTime(Rdata.time,C.longfmt);
               continue;
            }
         }

         // reset solution objects for this epoch
         for(i=0; i<C.SolObjs.size(); ++i)
            C.SolObjs[i].EpochReset();

         // loop over satellites -----------------------------
         RinexSatID sat;
         Rinex3ObsData::DataMap::iterator it;
         for(it=Rdata.obs.begin(); it!=Rdata.obs.end(); ++it) {
            sat = it->first;
            vector<RinexDatum>& vrdata(it->second);
            string sys(asString(sat.systemChar()));

            // is this system excluded?
            if(find(C.allsyss.begin(),C.allsyss.end(),sys) == C.allsyss.end()) {
               LOG(DEBUG) << " Sat " << sat << " : system " << sys << " is excluded.";
               continue;
            }

            // has user excluded this satellite?
            if(find(C.exclSat.begin(),C.exclSat.end(),sat) != C.exclSat.end()) {
               LOG(DEBUG) << " Sat " << sat << " is excluded.";
               continue;
            }

            // correct for DCB
            if(DCBcorr && mapDCBindex.find(sys) != mapDCBindex.end()) {
               i = mapDCBindex[sys];
               if(C.P1C1bias.find(sat) != C.P1C1bias.end()) {
                  LOG(DEBUG) << "Correct data " << asString(Rhead.mapObsTypes[sys][i])
                     << " = " << fixed << setprecision(2) << vrdata[i].data
                     << " for DCB with " << C.P1C1bias[sat];
                  vrdata[i].data += C.P1C1bias[sat];
               }
            }

            // elevation mask, azimuth and ephemeris range corrected with trop
            // - pass elev to CollectData for m-cov matrix and ORDs
            double elev(0),azim(0),ER(0),tcorr;
            if((C.elevLimit > 0 || C.weight || C.ORDout)
                              && PrevPos.getCoordinateSystem() != Position::Unknown) {
               CorrectedEphemerisRange CER;
               try {
                  CER.ComputeAtReceiveTime(Rdata.time, PrevPos, sat, *C.pEph);
                  elev = CER.elevation;
                  azim = CER.azimuth;
                  if(C.ORDout) {
                     tcorr = C.pTrop->correction(PrevPos,CER.svPosVel.x,Rdata.time);
                     ER = CER.rawrange - CER.svclkbias - CER.relativity + tcorr;
                  }
                  if(elev < C.elevLimit) {         // TD add elev mask [azim]
                     LOG(VERBOSE) << " Reject sat " << sat << " for elevation "
                        << fixed << setprecision(2) << elev << " at time "
                        << printTime(Rdata.time,C.longfmt);
                     continue;
                  }
               }
               catch(Exception& e) {
                  LOG(WARNING) << "WARNING : Failed to get elevation for sat "
                     << sat << " at time " << printTime(Rdata.time,C.longfmt);
                  continue;
               }
            }

            // pick out data for each solution object
            for(i=0; i<C.SolObjs.size(); ++i)
               C.SolObjs[i].CollectData(sat,elev,ER,vrdata);

         }  // end loop over satellites

         // debug: dump the RINEX data object
         if(C.debug > -1) Rdata.dump(LOGstrm,Rhead);

         // update the trop model's weather ------------------
         if(C.MetStore.size() > 0) C.setWeather(Rdata.time);

         // put a blank line here for readability
         LOG(INFO) << "";

         // compute the solution(s) --------------------------
         if(C.verbose) C.msg = printTime(Rdata.time,"DAT "+C.gpsfmt);   // tag for DAT

         // compute and print the solution(s) ----------------
         for(i=0; i<C.SolObjs.size(); ++i) {
            // skip invalid descriptors
            if(!C.SolObjs[i].isValid) continue;

            // dump the "DAT" record
            if(C.verbose) {LOG(VERBOSE)
               << C.SolObjs[i].dump((C.debug > -1 ? 2:1), "RPF", C.msg);}

            // compute the solution
            j = C.SolObjs[i].ComputeSolution(Rdata.time);

            // write ORDs, if solution is good
            if(C.ORDout && j==0) C.SolObjs[i].WriteORDs(Rdata.time);
         }

         // write to output RINEX ----------------------------
         if(!C.OutputObsFile.empty()) {
            Rinex3ObsData auxData;
            auxData.time = Rdata.time;
            auxData.clockOffset = Rdata.clockOffset;
            auxData.epochFlag = 4;
            ostringstream oss;
            // loop over valid descriptors
            for(k=0,i=0; i<C.SolObjs.size(); ++i) if(C.SolObjs[i].isValid) {
               oss.str("");
               oss << "XYZ" << fixed << setprecision(3)
                  << " " << setw(12) << C.SolObjs[i].prs.Solution(0)
                  << " " << setw(12) << C.SolObjs[i].prs.Solution(1)
                  << " " << setw(12) << C.SolObjs[i].prs.Solution(2);
               oss << " " << C.SolObjs[i].Descriptor;     // may get truncated
               auxData.auxHeader.commentList.push_back(oss.str());
               k++;
               oss.str("");
               oss << "CLK" << fixed << setprecision(3);

               for(j=0; j<C.SolObjs[i].prs.SystemIDs.size(); j++) {
                  RinexSatID sat(1,C.SolObjs[i].prs.SystemIDs[j]);
                  oss << " " << sat.systemString3()
                     << " " << setw(11) << C.SolObjs[i].prs.Solution(3+j);
               }
               oss << " " << C.SolObjs[i].Descriptor;     // may get truncated
               auxData.auxHeader.commentList.push_back(oss.str());
               k++;
               oss.str("");
               oss << "DIA" << setw(2) << C.SolObjs[i].prs.Nsvs
                  << fixed << setprecision(2)
                  << " " << setw(4) << C.SolObjs[i].prs.PDOP
                  << " " << setw(4) << C.SolObjs[i].prs.GDOP
                  << " " << setw(8) << C.SolObjs[i].prs.RMSResidual
                  << " " << C.SolObjs[i].Descriptor;     // may get truncated
               auxData.auxHeader.commentList.push_back(oss.str());
               k++;
            }
            auxData.numSVs = k;            // number of lines to write
            auxData.auxHeader.valid |= Rinex3ObsHeader::validComment;
            ostrm << auxData;

            ostrm << Rdata;
         }

      }  // end while loop over epochs

      istrm.close();

      // failure due to critical error
      if(iret < 0) break;

      if(iret == 0) nfiles++;

   }  // end loop over files

   if(!C.OutputObsFile.empty()) ostrm.close();

   if(iret < 0) return iret;

   return nfiles;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end ProcessFiles()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int routine(void) throw(Exception)
{
try {
   Configuration& C(Configuration::Instance());

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end routine()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
void Configuration::SetDefaults(void) throw()
{
   SPSout = ORDout = false;
   LogFile = string("prs.log");

   decimate = elevLimit = 0.0;
   forceElev = false;
   defaultstartStr = string("[Beginning of dataset]");
   defaultstopStr = string("[End of dataset]");
   beginTime = gpsBeginTime = GPSWeekSecond(0,0.,TimeSystem::Any);
   endTime = CommonTime::END_OF_TIME;

   inSolSys = string("GPS,GLO,GPS+GLO");
   inSolFreq.push_back(string("12"));

   TropType = string("NewB");
   TropPos = TropTime = false;
   defaultTemp = 20.0;
   defaultPress = 1013.0;
   defaultHumid = 50.0;
   TropStr = TropType + string(",") + asString(defaultTemp,1) + string(",")
      + asString(defaultPress,1) + string(",") + asString(defaultHumid,1);

   // get defaults from PRSolution
   {
      PRSolution dummy;
      RMSLimit = dummy.RMSLimit;
      SlopeLimit = dummy.SlopeLimit;
      maxReject = dummy.NSatsReject;
      nIter = dummy.MaxNIterations;
      convLimit = dummy.ConvergenceLimit;
   }

   userfmt = gpsfmt;
   help = verbose = false;
   debug = -1;

   // not for command line, but for processing of command line
   vecSys.push_back(string("GPS"));
   vecSys.push_back(string("GLO"));
   vecSys.push_back(string("GAL"));
   vecSys.push_back(string("GEO"));
   vecSys.push_back(string("COM"));

   //map<string,string> defMapSysCodes;   // map of system, default codes e.g. GLO/PC
   // TD shouldn't these come from ObsID?
   defMapSysCodes.insert(map<string,string>::value_type(
      string("GPS"),string("PYWLMIQSXCN")));
   defMapSysCodes.insert(map<string,string>::value_type(
      string("GLO"),string("PC")));
   defMapSysCodes.insert(map<string,string>::value_type(
      string("GAL"),string("ABCIQXZ")));
   defMapSysCodes.insert(map<string,string>::value_type(
      string("GEO"),string("IQXC")));
   defMapSysCodes.insert(map<string,string>::value_type(
      string("COM"),string("IQX")));

   map1to3Sys["G"] = "GPS";   map3to1Sys["GPS"] = "G";
   map1to3Sys["R"] = "GLO";   map3to1Sys["GLO"] = "R";
   map1to3Sys["E"] = "GAL";   map3to1Sys["GAL"] = "E";
   map1to3Sys["S"] = "GEO";   map3to1Sys["GEO"] = "S";
   map1to3Sys["C"] = "COM";   map3to1Sys["COM"] = "C";

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

   // output warning / error messages
   if(cmdlineUnrecognized.size() > 0) {
      LOG(INFO) << "Warning - unrecognized arguments:";
      for(int i=0; i<cmdlineUnrecognized.size(); i++)
         LOG(INFO) << "  " << cmdlineUnrecognized[i];
      LOG(INFO) << "End of unrecognized arguments";
   }

   // fatal errors
   if(!cmdlineErrors.empty()) {
      stripTrailing(cmdlineErrors,'\n');
      replaceAll(cmdlineErrors,"\n","\n ");
      LOG(INFO) << "Errors found on command line:\n " << cmdlineErrors
         << "\nEnd of command line errors.";
      return 1;
   }

   // success: dump configuration summary
   ostringstream oss;
   oss << "------ Summary of " << PrgmName << " command line configuration ------\n";
   opts.DumpConfiguration(oss);
   if(!cmdlineExtras.empty()) oss << "# Extra Processing:\n" << cmdlineExtras;
   oss << "------ End configuration summary ------";
   LOG(INFO) << oss.str();

   return 0;

}  // end Configuration::CommandLine()

//------------------------------------------------------------------------------------
string Configuration::BuildCommandLine(void) throw()
{
   // Program description will appear at the top of the syntax page
   string PrgmDesc = " Program " + PrgmName +
" reads one or more RINEX (v.2+) observation files, plus one or more\n"
" ephemeris (RINEX nav or SP3) files, and computes a pseudorange position-and-clock\n"
" solution, using a RAIM algorithm to eliminate outliers. Either single- or\n"
" mixed-system (GNSSs) processing may be selected; input data is determined\n"
" by, and solutions are labelled with, the 'solution descriptor' (see below).\n"
" Output is to a log file, and also optionally to a RINEX observation file with\n"
" the position solutions in comments in auxiliary header blocks. A final solution,\n"
" covariance and statistics are given at the bottom of the log file.\n"
"\n"
" In the log file, results at each time tag appear in lines with the format:\n"
"     \"TAG descriptor LABEL week sec.of.week CONTENT (code) [N]V\"\n"
" where TAG denotes the type of solution or solution residuals:\n"
"   RPF  RAIM ECEF XYZ solution\n"
"   RPR  RAIM ECEF XYZ solution residuals [only if --ref given]\n"
"   RNE  RAIM North-East-Up solution residuals [only if --ref given]\n"
"   SPS  Simple ECEF XYZ solution [only if --SPSout given]\n"
"   SPR  Simple ECEF XYZ solution residuals [only if both SPS & ref given]\n"
"   SNE  Simple North-East-Up solution residuals [only if SPS & ref given]\n"
" and LABEL followed by CONTENT is:\n"
"   NAV  X Y Z SYS clock_bias [SYS clock_bias ...]\n"
"   POS  X Y Z\n"
"   CLK  SYS clock_bias [SYS clock_bias ...]\n"
"   RMS  Nrej Ngood RMS TDOP PDOP GDOP Slope niter conv SAT [SAT ...]\n"
"   DAT  Ngood Nsats <SAT>:<freq><code> ... (list of sats with freq+code found)\n"
" and where\n"
"   X Y Z = position solution, or solution residuals, depending on TAG;\n"
"           RNE and SNE yield North-East-Up residuals, at --ref position\n"
"   SYS = system or GNSS, e.g. GPS GLO GAL ... (identifies system of clock bias)\n"
"   Nsats = number of satellites in the RINEX file at this time\n"
"   Ngood = number of satellites used in the solution algorithm\n"
"   Nrej = number of satellites rejected by the RAIM algorithm\n"
"   RMS = RMS residual of fit (meters)\n"
"   Slope = RAIM 'slope' value\n"
"   xDOP = Dilution of precision (T=time, P=position, G=geometric=T+P)\n"
"   niter = number of iterations performed by the solution algorithm\n"
"   conv = final convergence value (delta RMS position) of the solution algorithm\n"
"   SAT = satellite identifier (e.g. G10, R07); minus sign means rejected\n"
"   CODE = return value from solution algorithm (with words if --verbose)\n"
"   [N]V = V for valid solution, NV for not valid (don't use!)\n"
"\n"
" Default values appear in () after options below.\n"
   ;

   // options to appear on the syntax page, and to be accepted on command line
   //opts.Add(char, opt, arg, repeat?, required?, &target, pre-desc, desc);
   // NB filedummy is a dummy, but it must exist when cmdline is processed.
   opts.Add('f', "file", "fn", true, false, &filedummy,
            "# Input via configuration file:",
            "Name of file with more options [#->EOL = comment]");

   opts.Add(0, "obs", "fn", true, true, &InputObsFiles,
            "# Required input data and ephemeris files:",
            "RINEX observation file name(s)");
   opts.Add(0, "eph", "fn", true, false, &InputSP3Files,"",
            "Input Ephemeris+clock (SP3 format) file name(s)");
   opts.Add(0, "nav", "fn", true, false, &InputNavFiles, "",
            "Input RINEX nav file name(s)");

   opts.Add(0, "clk", "fn", true, false, &InputClkFiles,
            "# Other (optional) input files",
            "Input clock (RINEX format) file name(s)");
   opts.Add(0, "met", "fn", true, false, &InputMetFiles, "",
            "Input RINEX meteorological file name(s)");
   opts.Add(0, "dcb", "fn", true, false, &InputDCBFiles, "",
            "Input differential code bias (P1-C1) file name(s)");

   opts.Add(0, "obspath", "p", false, false, &Obspath,
            "# Paths of input files:", "Path of input RINEX observation file(s)");
   opts.Add(0, "ephpath", "p", false, false, &SP3path, "",
            "Path of input ephemeris+clock file(s)");
   opts.Add(0, "navpath", "p", false, false, &Navpath, "",
            "Path of input RINEX navigation file(s)");
   opts.Add(0, "clkpath", "p", false, false, &Clkpath, "",
            "Path of input RINEX clock file(s)");
   opts.Add(0, "metpath", "p", false, false, &Metpath, "",
            "Path of input RINEX meteorological file(s)");
   opts.Add(0, "dcbpath", "p", false, false, &DCBpath, "",
            "Path of input DCB (P1-C1) bias file(s)");

   startStr = defaultstartStr;
   stopStr = defaultstopStr;
   opts.Add(0, "start", "t[:f]", false, false, &startStr,
            "# Editing [t(time),f(format) = strings; "
               "default wk,sec.of.wk OR YYYY,mon,d,h,min,s]",
            "Start processing data at this epoch");
   opts.Add(0, "stop", "t[:f]", false, false, &stopStr, "",
            "Stop processing data at this epoch");
   opts.Add(0, "decimate", "dt", false, false, &decimate, "",
            "Decimate data to time interval dt (0: no decimation)");
   opts.Add(0, "elev", "deg", false, false, &elevLimit, "",
            "Minimum elevation angle (deg) [--ref or --forceElev req'd]");
   opts.Add(0, "forceElev", "", false, false, &forceElev, "",
            "Apply elev mask (--elev, w/o --ref) using sol. at prev. time tag");
   opts.Add(0, "exSat", "sat", true, false, &exclSat, "",
            "Exclude this satellite [eg. G24 | R | R23,G31]");

   opts.Add(0, "sol", "s:f:c", true, false, &inSolDesc, "# Solution Descriptors"
            "  [NB. --sol causes --sys, --code and --freq to be ignored]",
            "Explicit descriptor <sys:freq:code> e.g. GPS+GLO:12:PWC+PC");
   opts.Add(0, "sys", "s", true, false, &inSolSys,"",
            "Compute solutions for system(s) (GNSSs) <s>=S[,S,S+S], etc.");
   opts.Add(0, "code", "s:c", true, false, &inSolCode, "                    "
            "Allowed systems s: GPS,GLO,GAL,GEO(SBAS),COM",
            "System <s> preferred tracking codes <c>, in order [cf RINEX]");
   // make up a string of default codes
   string defcode = "Defaults: GPS:"+defMapSysCodes["GPS"]
                           +", GLO:"+defMapSysCodes["GLO"]
                           +", GAL:"+defMapSysCodes["GAL"]
                           +", GEO:"+defMapSysCodes["GEO"]
                           +", COM:"+defMapSysCodes["COM"];
   opts.Add(0, "freq", "f", true, false, &inSolFreq,
            "                    " + defcode,
            //"Defaults: GPS:PYMIQSLXWCN, GLO:PC, GAL:ABCIQXZ, GEO:CIQX, COM:IQX",
            "Frequencies (L<f>) to use in solution [e.g. 1 12 12+15]");

   opts.Add(0, "wt", "", false, false, &weight,
            "# Solution Algorithm:",
            "Weight the measurements using elevation [--ref req'd]");
   opts.Add(0, "rms", "lim", false, false, &RMSLimit, "",
            "Upper limit on RMS post-fit residual (m)");
   opts.Add(0, "slope", "lim", false, false, &SlopeLimit, "",
            "Upper limit on maximum RAIM 'slope'");
   opts.Add(0, "nrej", "n", false, false, &maxReject, "",
            "Maximum number of satellites to reject [-1 for no limit]");
   opts.Add(0, "niter", "lim", false, false, &nIter, "",
            "Maximum iteration count in linearized LS");
   opts.Add(0, "conv", "lim", false, false, &convLimit, "",
            "Maximum convergence criterion in estimation in meters");
   opts.Add(0, "Trop", "m,T,P,H", false, false, &TropStr, "",
            "Trop model <m> [one of Zero,Black,Saas,NewB,Neill,GG,GGHt\n             "
            "         with optional weather T(C),P(mb),RH(%)]");

   opts.Add(0, "log", "fn", false, false, &LogFile, "# Output [for formats see "
            "GPSTK::Position (--ref) and GPSTK::Epoch (--timefmt)] :",
            "Output log file name");
   opts.Add(0, "out", "fn", false, false, &OutputObsFile, "",
            "Output RINEX observations (with position solution in comments)");
   opts.Add(0, "ver2", "", false, false, &outver2, "",
            "In output RINEX (--out), write RINEX version 2.11 [otherwise 3.01]");
   opts.Add(0, "ref", "p[:f]", false, false, &refPosStr, "",
            "Known position p in fmt f (def. '%x,%y,%z'), for resids, elev and ORDs");
   opts.Add(0, "SPSout", "", false, false, &SPSout, "",
            "Output autonomous pseudorange solution [tag SPS, no RAIM]");
   opts.Add(0, "ORDs", "fn", false, false, &OutputORDFile, "",
            "Write ORDs (Observed Range Deviations) to file <fn> [--ref req'd]");
   opts.Add(0, "timefmt", "f", false, false, &userfmt, "",
            "Format for time tags in output");

   opts.Add(0, "verbose", "", false, false, &verbose, "# Diagnostic output:",
            "Print extended output information");
   opts.Add(0, "debug", "", false, false, &debug, "",
            "Print debug output at level 0 [debug<n> for level n=1-7]");
   opts.Add(0, "help", "", false, false, &help, "",
            "Print this and quit");

   // deprecated (old,new)
   opts.Add_deprecated("--SP3","--eph");

   return PrgmDesc;

}  // end Configuration::BuildCommandLine()

//------------------------------------------------------------------------------------
int Configuration::ExtraProcessing(string& errors, string& extras) throw()
{
   int i,n;
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
               << "Zero,Black,Saas,NewB,GG,GGht,Neill (cf. gpstk::TropModel)" << endl;
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

   // build descriptors (sys,freq,code) of output solution ------------------------
   BuildSolDescriptors(oss);

   // open the log file (so warnings, configuration summary, etc can go there) -----
   logstrm.open(LogFile.c_str(), ios::out);
   if(!logstrm.is_open()) {
      LOG(ERROR) << "Error : Failed to open log file " << LogFile;
      return -4;
   }
   LOG(INFO) << "Output redirected to log file " << LogFile;
   pLOGstrm = &logstrm;
   LOG(INFO) << Title;

   // check consistency
   if(elevLimit>0. && knownPos.getCoordinateSystem()==Position::Unknown && !forceElev)
      oss << "Error : --elev with no --ref input requires --forceElev\n";

   if(forceElev && elevLimit <= 0.0)
      ossx << "   Warning : --forceElev with no --elev <= 0 appears.";
   else if(forceElev && knownPos.getCoordinateSystem() == Position::Unknown)
      ossx << "   Warning : with --ref input, --forceElev is not required.";

   if(!OutputORDFile.empty() && knownPos.getCoordinateSystem() == Position::Unknown)
      oss << "Error : --ORDs requires --ref\n";

   // add new errors to the list
   msg = oss.str();
   if(!msg.empty()) errors += msg;
   msg = ossx.str();
   if(!msg.empty()) extras += msg;

   return 0;

} // end Configuration::ExtraProcessing(string& errors) throw()

//------------------------------------------------------------------------------------
void Configuration::BuildSolDescriptors(ostringstream& oss) throw()
{
   bool ok;
   int i,j,k;
   vector<string> fld,subfld,codfld;

   // check and save explicit input solution descriptors
   if(inSolDesc.size() > 0) {       // user has input explicit solution descriptors
      // ignore other input
      inSolSys = string();
      inSolFreq.clear();
      inSolCode.clear();

      // check them and save the good ones
      for(i=0; i<inSolDesc.size(); ++i) {
         fld = split(inSolDesc[i],':');
         if(fld.size() != 3)                       // wrong number of fields
            oss << "Error : invalid arg in --sol : " << inSolDesc[i] << endl;
         else {
            ok = true;

            // check the freq(s) first
            subfld = split(fld[1],'+');            // fld[1] ~ 12 1+2 15 12+15
            for(j=0; j<subfld.size(); j++) {       // subfld ~ 12 1 2 15
               if(subfld[j].size() > 2) {
                  oss << "Error : only single or dual frequency allowed in --sol : "
                     << subfld[j] << endl;
                  ok = false;
               }
               for(k=0; k<subfld[j].size(); k++) {
                  if(subfld[j][k] != '1' &&
                     subfld[j][k] != '2' && subfld[j][k] != '5') {
                        oss << "Error : invalid frequency in --sol " << subfld[j]
                           << endl;
                     ok = false;
                  }
               }
            }

            // check the system(s) and code(s)
            subfld = split(fld[0],'+');
            codfld = split(fld[2],'+');

            // same number of systems/codes?
            if(subfld.size() != codfld.size()) {
               oss << "Error : in --sol, each system must have codes : "
                  << inSolDesc[i] << " e.g. GPS+GLO,12,PWC+PC\n";
               ok = false;
            }

            // check the code(s)
            else for(j=0; j<subfld.size(); j++) {
               msg = defMapSysCodes[subfld[j]];    // all allowed for this sys
               for(k=0; k<codfld[j].size(); k++) {
                  if(msg.find(codfld[j][k]) == string::npos) {
                     oss << "Error : code " << codfld[j][k]
                        << " is not allowed for system " << subfld[j] << endl;
                     ok = false;
                  }
               }
            }
            if(ok) SolDesc.push_back(inSolDesc[i]);
         }
      }

      return;           // all other --sys --freq --code input is ignored
   }

   // process and check --sys
   vector<string> inSystems;
   if(!inSolSys.empty()) {
      fld = split(inSolSys,',');

      for(i=0; i<fld.size(); i++) {
         ok = true;
         subfld = split(fld[i],'+');
         for(j=0; j<subfld.size(); j++) {
            if(find(vecSys.begin(),vecSys.end(),subfld[j]) == vecSys.end()) {
               oss << "Error : invalid system in --sys : " << subfld[j] << endl;
               ok = false;
            }
         }
         if(ok) inSystems.push_back(fld[i]);
      }
   }

   // process and check --code
   if(inSolCode.size() == 0)                 // take all defaults
      mapSysCodes = defMapSysCodes;

   else for(i=0; i<inSolCode.size(); i++) {  // check and process input
      fld = split(inSolCode[i],':');
      ok = true;

      // wrong number of fields
      if(fld.size() != 2) {
         oss << "Error : invalid arg in '--code S:C' : " << inSolCode[i]
                  << " (NB. use ':' not ',' e.g. GPS:PYWXC)" << endl;
         ok = false;
      }

      upperCase(fld[0]);
      upperCase(fld[1]);
      if(find(vecSys.begin(),vecSys.end(),fld[0]) == vecSys.end()) {
         // system is not found
         oss << "Error : invalid system in --code : " << fld[0] << endl;
         ok = false;
      }

      // check the codes
      msg = defMapSysCodes[fld[0]];       // all allowed for this sys
      for(j=0; j<fld[1].size(); j++) {
         string::size_type pos(0);
         if(msg.find(fld[1][j],pos) == string::npos) {
            oss << "Error : code " << fld[1][j] << " is not allowed for system "
               << fld[0] << endl;
            ok = false;
         }
      }

      // ok - use either input or default
      if(ok) mapSysCodes[fld[0]] = fld[1];
      else   mapSysCodes[fld[0]] = defMapSysCodes[fld[0]];
   }

   // process and check --freq
   vector<string> inFreqs;
   for(i=0; i<inSolFreq.size(); i++) {
      fld = split(inSolFreq[i],'+');

      ok = true;
      for(j=0; j<fld.size(); j++) {
         if(fld[j].size() > 2) {
            oss << "Error : only single or dual frequency allowed in --freq : "
               << inSolFreq[i] << endl;
            ok = false;
         }
         else for(k=0; k<fld[j].size(); k++) {
            if(fld[j][k] != '1' && fld[j][k] != '2' && fld[j][k] != '5') {
               oss << "Error : invalid frequency in --freq : " << fld[j][k] << endl;
               ok = false;
            }
            else LOG(DEBUG) << "  Accept frequency " << fld[j][k];
         }
      }
      if(ok) {
         inFreqs.push_back(inSolFreq[i]);
         LOG(DEBUG) << " Accept frequency combo " << inSolFreq[i];
      }
   }

   // do we have input?  always yes, since there are defaults...
   if(inSystems.size()==0 || inFreqs.size() == 0) {
      oss << "Error : without --sol, both --sys and --freq must be given"
         << endl;
      return;
   }

   // build descriptors
   for(i=0; i<inSystems.size(); ++i) {
      fld = split(inSystems[i],'+');

      for(j=0; j<inFreqs.size(); ++j) {   // for each frequency
         // desc = sys:freq:code e.g. GPS+GLO:1+2:PC+PC
         string ds(inSystems[i] + ":" + inFreqs[j] + ":" + mapSysCodes[fld[0]]);
         // append codes for other systems
         for(k=1; k<fld.size(); k++)  ds += "+" + mapSysCodes[fld[k]];
         // save it
         SolDesc.push_back(ds);
      }
   }

}  // end void Configuration::BuildSolDescriptors(ostringstream& oss) throw()

//------------------------------------------------------------------------------------
// update weather in the trop model using the Met store
void Configuration::setWeather(const CommonTime& ttag) throw(Exception)
{
   try {
      Configuration& C(Configuration::Instance());
      static list<RinexMetData>::iterator it(MetStore.begin());
      list<RinexMetData>::iterator nextit;
      static CommonTime currentTime(C.gpsBeginTime);
      double dt;

      while(it != MetStore.end()) {
         (nextit = it)++;  // point to next entry after it

         //                // if ttag is before next but after current,
         if( (nextit != MetStore.end() && ttag < nextit->time && ttag >= it->time)
            ||             // OR there is no next, but ttag is w/in 15 min of current
             (nextit == MetStore.end() && (dt=ttag-it->time) >= 0.0 && dt < 900.0))
         {
            // skip if its already done
            if(it->time == currentTime) break;
            currentTime = it->time;

            if(it->data.count(RinexMetHeader::TD) > 0)
               defaultTemp = it->data[RinexMetHeader::TD];
            if(it->data.count(RinexMetHeader::PR) > 0)
               defaultPress = it->data[RinexMetHeader::PR];
            if(it->data.count(RinexMetHeader::HR) > 0)
               defaultHumid = it->data[RinexMetHeader::HR];

            LOG(DEBUG) << "Reset weather at "
               << printTime(ttag,longfmt) << " to " << printTime(currentTime,longfmt)
               << " " << defaultTemp
               << " " << defaultPress
               << " " << defaultHumid;

            pTrop->setWeather(defaultTemp,defaultPress,defaultHumid);

            break;
         }

         // time is beyond next epoch
         else if(nextit != MetStore.end() && ttag >= nextit->time)
            ++it;

         // do nothing, because ttag is before the next epoch
         else break;
      }
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
void SolutionObject::ParseDescriptor(void) throw()
{
   int i,j,k;
   Configuration& C(Configuration::Instance());

   vector<string> flds(split(Descriptor,':'));

   freqs = split(flds[1],'+');               // flds[1] ~ '12' '15' or '12+15'
   // TD require single or dual frequency?

   // build a list (string) of unique frequencies
   ufreqs = string("");
   for(i=0; i<freqs.size(); i++)
      for(j=0; j<freqs[i].size(); j++)
         if(ufreqs.find(freqs[i][j]) == string::npos)
            ufreqs += string(1,freqs[i][j]);

   map<string,string>::const_iterator it(C.map1to3Sys.begin());
   for( ; it != C.map1to3Sys.end(); ++it)
      replaceAll(flds[0],it->second,it->first);
   syss = split(flds[0],'+');                // syss ~ vec<char> ~ G,R,E,C,S

   for(i=0; i<syss.size(); i++) {            // Syss ~ vec<SatID::sys> parallel syss
      RinexSatID sat(syss[i]);
      Syss.push_back(sat.system);
   }

   vector<string> code(split(flds[2],'+'));  // code ~ vec<str> parallel to syss
   syscodes.clear();
   for(i=0; i<syss.size(); i++)              // build a little map [G]=PWXC
      syscodes.insert(map<string,string>::value_type(syss[i],code[i]));

   // build empty mapSysFreqObsIDs   e.g. map[G][1] = vector<ObsIDs>
   mapSysFreqObsIDs.clear();
   for(i=0; i<syss.size(); i++) {            // loop over systems
      map<string, vector<string> > sysmap;
      mapSysFreqObsIDs.insert(
         map<string,map<string,vector<string> > >::value_type(syss[i],sysmap));
      for(j=0; j<ufreqs.size(); j++) {       // loop over unique frequencies
         vector<string> vec;
         mapSysFreqObsIDs[syss[i]].insert(
            map<string,vector<string> >::value_type(ufreqs.substr(j,1),vec));
         LOG(DEBUG) << "Build mapSysFreqObsIDs[" << syss[i]
                     << "][" << ufreqs.substr(j,1) << "]";
      }
   }
}

//------------------------------------------------------------------------------------
void SolutionObject::SetDefaults(void) throw()
{
   Configuration& C(Configuration::Instance());

   prs.RMSLimit = C.RMSLimit;
   prs.SlopeLimit = C.SlopeLimit;
   prs.NSatsReject = C.maxReject;
   prs.MaxNIterations = C.nIter;
   prs.ConvergenceLimit = C.convLimit;

   // specify systems
   // syss ~ vec<1-char string> ~ G,R,E,C,S; must have at least one member
   RinexSatID sat;
   for(int i=0; i<syss.size(); i++) {
      sat.fromString(syss[i]);               // 1-char string
      prs.SystemIDs.push_back(sat.system);
      LOG(DEBUG) << " Add system " << syss[i] << " = " << sat << " to SystemIDs";
   }

   // initialize apriori solution
   if(C.knownPos.getCoordinateSystem() != Position::Unknown)
      prs.memory.fixAPSolution(C.knownPos.X(),C.knownPos.Y(),C.knownPos.Z());
}

//------------------------------------------------------------------------------------
bool SolutionObject::ChooseObsIDs(map<string,vector<RinexObsID> >& mapObsTypes)
   throw()
{
   int i,j,k,m,n;
   Configuration& C(Configuration::Instance());
   vector<string> obstypes;

   isValid = true;
   mapObsIndex.clear();    // build map<string, int> , e.g. [C1C]=17

   // loop over systems, then obs types
   map<std::string,vector<RinexObsID> >::const_iterator sit;
   for(sit=mapObsTypes.begin(); sit != mapObsTypes.end(); ++sit) {
      string sys(sit->first);                                  // sys ~ G,R,E,C,S
      // skip if system not found
      if(find(syss.begin(),syss.end(),sys) == syss.end())
         continue;

      // loop over obs types
      const vector<RinexObsID>& vec(mapObsTypes[sys]);
      for(j=0; j<vec.size(); j++) {
         string ot(vec[j].asString());                            // e.g. C1P
         // reject this obs type?
         if(ot[0] != 'C' ||                                       // not a pseudorange
            ufreqs.find(ot[1],0) == string::npos ||               // freq not found
            syscodes[sit->first].find(ot[2],0) == string::npos)   // code not found
               continue;

         obstypes.push_back(sys+ot);                  // keep this one
         mapObsIndex[sys+ot] = j;                     // save index for RINEX data
      }
   }

   // alphabetize
   sort(obstypes.begin(),obstypes.end());

   // further sort on last character, but according to syscodes[sys]
   // poor man's sort(iter,iter,op()) b/c I don't know how to call it....
   string currf3,f3,sys,fre;                 // current first 3 characters
   vector<string> tempot;
   for(j=0; j<=obstypes.size()+1; j++) {     // note one more than obstypes.size()
      if(j>=obstypes.size())
         f3 = string("END");                 // this will -> finish up
      else
         f3 = obstypes[j].substr(0,3);       // first three char. - already sorted

      if(currf3.empty()) {                   // new first three
         currf3 = f3;
         sys = currf3.substr(0,1);
         fre = currf3.substr(2,1);
         k = j;
      }
      else if(f3 == currf3)                  // same first three - wait
         ;
      else if(j-1 == k) {                    // different f3, but only one saved
         tempot.push_back(obstypes[k]);
         mapSysFreqObsIDs[sys][fre].push_back(obstypes[k]);
         if(j == obstypes.size()) break;
         currf3 = obstypes[j].substr(0,3);
         sys = currf3.substr(0,1);
         fre = currf3.substr(2,1);
         k = j;
      }
      else {                                 // different f3, and must sort k..j-1
         string codes(syscodes[currf3.substr(0,1)]);
         for(int n=0; n<codes.size(); n++)   // loop over the codes one at a time
            for(int m=k; m<j; m++)              // loop over obstypes k..j-1
               if(codes[n] == obstypes[m][3]) {    // is there a match?
                  tempot.push_back(obstypes[m]);      // save it
                  mapSysFreqObsIDs[sys][fre].push_back(obstypes[m]);
               }

         if(j == obstypes.size()) break;
         currf3 = obstypes[j].substr(0,3);
         sys = currf3.substr(0,1);
         fre = currf3.substr(2,1);
         k = j;
      }
   }

   obstypes = tempot;

   // check that there are obs types for each sys/freq
   ostringstream oss;
   if(C.debug > -1) oss << " Dump mapSysFreqObsIDs:";
   for(i=0; i<syss.size(); i++) {
      string s(syss[i]);
      for(j=0; j<ufreqs.size(); j++) {
         string f(ufreqs.substr(j,1));
         if(C.debug > -1) oss << " " << C.map1to3Sys[s] << ":L" << f;
         if(mapSysFreqObsIDs[s][f].size() == 0) {
            //LOG(WARNING) << "Warning : No 'C' (PR) ObsIDs available for "
            //   << C.map1to3Sys[s] << ",L" << f << " in solution " << Descriptor;
            isValid = false;
            if(C.debug > -1) oss << ":NA";
         }
         else if(C.debug > -1) {
            for(k=0; k<mapSysFreqObsIDs[s][f].size(); k++)
               oss << (k==0 ? ":":",") << mapSysFreqObsIDs[s][f][k];
         }
      }
   }
   LOG(DEBUG) << oss.str();

   return isValid;
}

//------------------------------------------------------------------------------------
string SolutionObject::dump(int level, string msg1, string msg2) throw()
{
   int i,j;
   ostringstream oss;
   Configuration& C(Configuration::Instance());

   oss << msg1 << " " << Descriptor << (msg2.empty() ? "" : " "+msg2);

   if(level == 0) {
      // add the RinexObsIDs
      for(i=0; i<syss.size(); i++) {
         string s(syss[i]);
         for(j=0; j<ufreqs.size(); j++) {
            string f(1,ufreqs[j]);
            oss << " " << C.map1to3Sys[s] << ":L" << f << ":";
            if(mapSysFreqObsIDs[s][f].size() == 0)
               oss << "NA";
            else for(int k=0; k<mapSysFreqObsIDs[s][f].size(); k++)
               oss << (k==0?"":",") << mapSysFreqObsIDs[s][f][k].substr(1,3);
                  //<< "(" << mapObsIndex[mapSysFreqObsIDs[s][f][k]] << ")";
         }
      }
   }

   else if(level >= 1) {
      // Descriptor ndata [-]sat:ot,ot[:PR] ...  (-: no data, PR: data in level 2)
      oss << " " << setw(2) << Satellites.size()
          << " " << setw(2) << UsedObsIDs.size();

      // loop over all potential data: sats+code(s); j counts good values
      multimap<RinexSatID,string>::const_iterator it(UsedObsIDs.begin());
      for(j=0; it != UsedObsIDs.end(); ++it) {
         // is the sat (it->first) found in Satellites (i.e. does it have data)?
         vector<SatID>::const_iterator jt;
         jt = find(Satellites.begin(),Satellites.end(),it->first);
         // and all code(s) found ?
         bool good(jt != Satellites.end() && it->second.find('-') == string::npos);

         // dump it, putting a - in front of sat if its not good
         oss << " " << (good ? "":"-") << it->first << ":" << it->second;

         // add data if level 2 and its available
         if(level > 1 && good)
            oss << ":" << fixed << setprecision(3) << PRanges[j++];
      }
   }

   // valid?
   if(!isValid) oss << " Invalid";

   return oss.str();
}

//------------------------------------------------------------------------------------
void SolutionObject::EpochReset(void) throw()
{
   Satellites.clear();
   PRanges.clear();
   Elevations.clear();
   ERanges.clear();
   RIono.clear();
   R1.clear();
   R2.clear();
   UsedObsIDs.clear();
}

//------------------------------------------------------------------------------------
void SolutionObject::CollectData(const RinexSatID& sat,
                                 const double& elev, const double& ER,
                                 const vector<RinexDatum>& vrd)
   throw()
{
   if(!isValid) return;

   int i,j,k,n;

   string sys(1,sat.systemChar());                          // satellite's system
   if(mapSysFreqObsIDs.find(sys) == mapSysFreqObsIDs.end()) // this sys not needed
      return;

   UsedObsIDs.erase(sat);        // just in case
   map<string,double> RawPRs;    // e.g. ["1"] = PR(L1)
   map<string,string> used;      // e.g. ["1"] = "1W";

   // pull out the raw data for each frequency
   for(i=0; i<ufreqs.size(); i++) {
      string f(1,ufreqs[i]);            // freq as a string e.g. "1"

      // loop over RinexObsIDs for this sys,f
      for(k=-1,j=0; j<mapSysFreqObsIDs[sys][f].size(); j++) {
         // n = index into vrd
         n = mapObsIndex[mapSysFreqObsIDs[sys][f][j]];
         // is it good?
         if(vrd[n].data == 0) continue;
         // save n and the index (j) into mapSysFreqObsIDs[sys][f]
         k = j;
         break;
      }

      // save this PR and the freq/code that it came from
      RawPRs[f] = (k == -1 ? 0.0 : vrd[n].data);
      used[f] = (k == -1 ? f+"-" : mapSysFreqObsIDs[sys][f][k].substr(2,2));
   }

   // build the PR (possibly a linear combination, possibly more than one)
   // usually, PR = iono-free pseudorange, and RI = ionospheric delay, built from
   // the two pseudoranges at different frequencies.
   for(i=0; i<freqs.size(); i++) {
      bool ok;
      double PR,RI(0),Rone(0),Rtwo(0);
      if(freqs[i].size() == 1) {
         PR = RawPRs[freqs[i]];
         UsedObsIDs.insert(
            multimap<RinexSatID,string>::value_type(sat,used[freqs[i]]));
         ok = (used[freqs[i]] != freqs[i]+"-");
      }
      else if(freqs[i] == "12" || freqs[i] == "21") {
         PR = a12[sys]*RawPRs["1"] + b12[sys]*RawPRs["2"];
         RI = (RawPRs["1"]-RawPRs["2"])*b12[sys];
         Rone = RawPRs["1"];
         Rtwo = RawPRs["2"];
         UsedObsIDs.insert(
            multimap<RinexSatID,string>::value_type(sat,used["1"]+used["2"]));
         ok = (used["1"] != "1-" && used["2"] != "2-");
      }
      else if(freqs[i] == "15" || freqs[i] == "51") {
         PR = a15[sys]*RawPRs["1"] + b15[sys]*RawPRs["5"];
         RI = (RawPRs["1"]-RawPRs["5"])*b15[sys];
         Rone = RawPRs["1"];
         Rtwo = RawPRs["5"];
         UsedObsIDs.insert(
            multimap<RinexSatID,string>::value_type(sat,used["1"]+used["5"]));
         ok = (used["1"] != "1-" && used["5"] != "5-");
      }
      else if(freqs[i] == "25" || freqs[i] == "52") {
         PR = a25[sys]*RawPRs["2"] + b25[sys]*RawPRs["5"];
         RI = (RawPRs["2"]-RawPRs["5"])*b25[sys];
         Rone = RawPRs["2"];
         Rtwo = RawPRs["5"];
         UsedObsIDs.insert(
            multimap<RinexSatID,string>::value_type(sat,used["2"]+used["5"]));
         ok = (used["2"] != "2-" && used["5"] != "5-");
      }
      // TD else three-freq ?
      else ok=false;   // throw?

      if(!ok) continue;

      // add to data for this solution
      Satellites.push_back(sat);
      PRanges.push_back(PR);
      Elevations.push_back(elev);
      ERanges.push_back(ER);
      RIono.push_back(RI);
      R1.push_back(Rone);
      R2.push_back(Rtwo);
   }
}

//------------------------------------------------------------------------------------
// return 0 good, negative failure - same as RAIMCompute
int SolutionObject::ComputeSolution(const CommonTime& ttag) throw(Exception)
{
   try {
      int i,n,iret;
      Configuration& C(Configuration::Instance());

      LOG(DEBUG) << "ComputeSolution for " << Descriptor
            << " at time " << printTime(ttag,C.longfmt);

      // compute the inverse measurement covariance
      Matrix<double> invMCov;       // default is empty
      if(C.weight) {
         n = Elevations.size();
         invMCov = Matrix<double>(n,n);
         ident(invMCov);            // start with identity
         static const double elev0(30.0);
         static const double sin0(::sin(elev0 * DEG_TO_RAD));
         for(i=0; i<n; i++) if(Elevations[i] < elev0) {       // mod only el<30
            double invsig(::sin(Elevations[i] * DEG_TO_RAD) / sin0);
            invMCov(i,i) = invsig*invsig;
         }
         LOG(DEBUG) << "invMeasCov for " << Descriptor
            << " at time " << printTime(ttag,C.longfmt) << "\n"
            << fixed << setprecision(4) << invMCov;
      }

      // get the straight solution --------------------------------------
      if(C.SPSout) {
         Matrix<double> SVP;
         iret = prs.PreparePRSolution(ttag, Satellites, Syss, PRanges, C.pEph, SVP);

         if(iret > -3) {
            Vector<double> APSol(5,0.0),Resid,Slopes;
            if(prs.hasMemory) APSol = prs.memory.APSolution;
            iret = prs.SimplePRSolution(ttag, Satellites, SVP, invMCov, C.pTrop,
               prs.MaxNIterations, prs.ConvergenceLimit, Syss, APSol, Resid, Slopes);
         }

         if(iret < 0) {LOG(VERBOSE) << "SimplePRS failed "
            << (iret==-4 ? "to find ANY ephemeris" :
               (iret==-3 ? "to find enough satellites with data" :
               (iret==-2 ? "because the problem is singular" :
              /*iret==-1*/ "because the algorithm failed to converge")))
            << " for " << Descriptor
            << " at time " << printTime(ttag,C.longfmt);}

         else {
            // at this point we have a good solution

            // output XYZ solution
            LOG(INFO) << prs.outputString(string("SPS ")+Descriptor,iret);

            if(prs.RMSFlag || prs.SlopeFlag || prs.TropFlag)
               {LOG(WARNING) << "Warning for " << Descriptor
                  << " - possible degraded SPS solution at "
                  << printTime(ttag,C.longfmt) << " due to"
                  << (prs.RMSFlag ? " large RMS":"")           // NB strings are used
                  << (prs.SlopeFlag ? " large slope":"")       // in PRSplot.pl
                  << (prs.TropFlag ? " missed trop. corr.":"");}

            // compute residuals using known position, output XYZ resids, NEU resids
            if(C.knownPos.getCoordinateSystem() != Position::Unknown && iret >= 0) {
               Matrix<double> Cov;
               Vector<double> V(3);

               // compute residuals in XYZ
               Position pos(prs.Solution(0), prs.Solution(1), prs.Solution(2));
               Position res=pos-C.knownPos;
               // their covariance
               Cov = Matrix<double>(prs.Covariance,0,0,3,3);
               // output these as SPR record
               V(0) = res.X(); V(1) = res.Y(); V(2) = res.Z();
               LOG(INFO) << prs.outputPOSString(string("SPR ")+Descriptor,iret,V);
               // and accumulate statistics on XYZ residuals
               //statsSPSXYZresid.add(V,Cov);

               // convert to NEU
               V = C.Rot * V;
               Cov = C.Rot * Cov * transpose(C.Rot);
               // output them as RNE record
               LOG(INFO) << prs.outputPOSString(string("SNE ")+Descriptor,iret,V);
               // and accumulate statistics on NEU residuals
               //statsSPSNEUresid.add(V,Cov);
            }
         }
      }

      // get the RAIM solution ------------------------------------------
      iret = prs.RAIMCompute(ttag, Satellites, Syss, PRanges, invMCov, C.pEph,
                              C.pTrop);

      if(iret < 0) {
         LOG(VERBOSE) << "RAIMCompute failed "
            << (iret==-4 ? "to find ANY ephemeris" :
               (iret==-3 ? "to find enough satellites with data" :
               (iret==-2 ? "because the problem is singular" :
              /*iret==-1*/ "because the algorithm failed to converge")))
            << " for " << Descriptor
            << " at time " << printTime(ttag,C.longfmt);
         return iret;
      }

      // at this point we have a good RAIM solution

      // output XYZ solution
      LOG(INFO) << prs.outputString(string("RPF ")+Descriptor,iret);

      if(prs.RMSFlag || prs.SlopeFlag || prs.TropFlag)
         {LOG(WARNING) << "Warning for " << Descriptor
            << " - possible degraded RPF solution at "
            << printTime(ttag,C.longfmt) << " due to"
            << (prs.RMSFlag ? " large RMS":"")           // NB these strings are used
            << (prs.SlopeFlag ? " large slope":"")       // in PRSplot.pl
            << (prs.TropFlag ? " missed trop. corr.":"");}

      // dump pre-fit residuals
      if(prs.hasMemory && ++nepochs > 1)
         {LOG(VERBOSE) << "RPF " << Descriptor << " PFR"
            << " " << printTime(ttag,C.gpsfmt)              // time
            << fixed << setprecision(3)
            << " " << ::sqrt(prs.memory.getAPV())           // sig(APV)
            << " " << setw(2) << prs.PreFitResidual.size()  // n resids
            << " " << prs.PreFitResidual;}                  // pre-fit residuals

      // compute residuals using known position, and output XYZ resids, NEU resids
      if(C.knownPos.getCoordinateSystem() != Position::Unknown && iret >= 0) {
         Matrix<double> Cov;
         Vector<double> V(3);

         // compute residuals in XYZ
         Position pos(prs.Solution(0), prs.Solution(1), prs.Solution(2));
         Position res=pos-C.knownPos;
         // their covariance
         Cov = Matrix<double>(prs.Covariance,0,0,3,3);
         // output these as RPR record
         V(0) = res.X(); V(1) = res.Y(); V(2) = res.Z();
         LOG(INFO) << prs.outputPOSString(string("RPR ")+Descriptor,iret,V);
         // and accumulate statistics on XYZ residuals
         statsXYZresid.add(V,Cov);

         // convert to NEU
         V = C.Rot * V;
         Cov = C.Rot * Cov * transpose(C.Rot);
         // output them as RNE record
         LOG(INFO) << prs.outputPOSString(string("RNE ")+Descriptor,iret,V);
         // and accumulate statistics on NEU residuals
         //if(iret == 0)        //   TD ? but not if RMS/Slope/TropFlag?
         statsNEUresid.add(V,Cov);
      }

      // prepare for next epoch

      // if trop model has not been initialized, do so
      if(!C.TropPos) {
         Position pos(prs.Solution(0), prs.Solution(1), prs.Solution(2));
         C.pTrop->setReceiverLatitude(pos.getGeodeticLatitude());
         C.pTrop->setReceiverHeight(pos.getHeight());
         C.TropPos = true;
      }
      if(!C.TropTime) {
         C.pTrop->setDayOfYear(static_cast<YDSTime>(ttag).doy);
         C.TropTime = true;
      }

      // update apriori solution
      if(prs.hasMemory) prs.memory.updateAPSolution(prs.Solution);

      return iret;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int SolutionObject::WriteORDs(const CommonTime& time) throw(Exception)
{
   try {
      Configuration& C(Configuration::Instance());

      int i,j;
      double clk;
      for(i=0; i<Satellites.size(); i++) {
         if(Satellites[i].id < 0) continue;

         // get the system, then clock solution for this system
         vector<SatID::SatelliteSystem>::const_iterator jt;
         jt = find(prs.SystemIDs.begin(),prs.SystemIDs.end(),Satellites[i].system);
         if(jt == prs.SystemIDs.end()) continue;      // should never happen
         j = jt - prs.SystemIDs.begin();              // index
         clk = prs.Solution(3+j);

         C.ordstrm << "ORD " << RinexSatID(Satellites[i]).toString()
            << " " << printTime(time,C.userfmt) << fixed << setprecision(3)
            << " " << setw(6) << Elevations[i]
            << " " << setw(6) << RIono[i]
            << " " << setw(8) << R1[i] - ERanges[i] - clk
            << " " << setw(8) << R2[i] - ERanges[i] - clk
            << " " << setw(8) << PRanges[i] - ERanges[i] - clk
            << " " << setw(13) << clk
            //<< " " << setw(12) << PRanges[i]
            //<< " " << setw(12) << ERanges[i]
            << " " << Descriptor
            << endl;
      }

      return 0;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
void SolutionObject::FinalOutput(void) throw(Exception)
{
   try {
      Configuration& C(Configuration::Instance());

      prs.memory.dump(LOGstrm,Descriptor+" RAIM solution");
      LOG(INFO) << "\n";

      if(C.knownPos.getCoordinateSystem() != Position::Unknown) {
         // output stats on XYZ residuals
         statsXYZresid.setMessage(Descriptor + " RAIM XYZ position residuals (m)");
         LOG(INFO) << statsXYZresid << endl;

         // output stats on NEU residuals
         statsNEUresid.setMessage(Descriptor + " RAIM NEU position residuals (m)");
         statsNEUresid.setLabels("North","East ","Up   ");
         LOG(INFO) << statsNEUresid;

         // output the covariance for NEU
         double apv(::sqrt(prs.memory.getAPV()));        // APV from XYZ stats
         if(apv > 0.0) {
            Matrix<double> Cov(statsNEUresid.getCov());  // cov from NEU stats

            // scale the covariance
            for(int i=0; i<Cov.rows(); i++) for(int j=i; j<Cov.cols(); j++)
               Cov(i,j) = Cov(j,i) = Cov(i,j)*apv;

            // print this covariance as labelled matrix
            Namelist NL;
            NL += "North"; NL += "East "; NL += "Up   ";
            LabelledMatrix LM(NL,Cov);
            LM.scientific().setprecision(3).setw(14);
            LOG(INFO) << "Covariance of " << statsNEUresid.getMessage() << endl << LM;
         }
      }

   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
