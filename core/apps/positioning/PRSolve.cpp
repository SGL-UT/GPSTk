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

/// @file PRSolve.cpp
/// Read Rinex observation files (version 2 or 3) and ephemeris store, and compute a
/// a pseudorange-only position solution.

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
#include "GNSSconstants.hpp"

#include "singleton.hpp"
#include "expandtilde.hpp"
#include "logstream.hpp"
#include "CommandLine.hpp"

#include "CommonTime.hpp"
#include "Epoch.hpp"
#include "TimeString.hpp"
#include "GPSWeekSecond.hpp"
#include "ObsID.hpp"

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
#include "HelmertTransform.hpp"

#include "PRSolution.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

//------------------------------------------------------------------------------------
string Version(string("5.2 10/13/15"));

// forward declarations
class SolutionObject;

//------------------------------------------------------------------------------------
// Object for command line input and global data
class Configuration : public Singleton<Configuration> {

public:

   // Default and only constructor
   Configuration() throw() { SetDefaults(); }

   // Create, parse and process command line options and user input
   int ProcessUserInput(int argc, char **argv) throw();

   // Create and output help message for --sol
   void SolDescHelp(void);

   // Design the command line
   string BuildCommandLine(void) throw();

   // Open the output file, and parse the strings used on the command line
   // return -4 if log file could not be opened
   //int ExtraProcessing(void) throw();
   //TD on clau, this leads to the SPS algorithm failing to converge on some problems.
   int ExtraProcessing(string& errors, string& extras) throw();

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

   string Obspath,SP3path,Clkpath,Navpath,Metpath,DCBpath;              // paths

   // times derived from --start and --stop
   string defaultstartStr,startStr;
   string defaultstopStr,stopStr;
   CommonTime beginTime,endTime,gpsBeginTime,decTime;

   double decimate;           // decimate input data
   double elevLimit;          // limit sats to elevation mask
   bool forceElev;            // use elevLimit even without --ref
   bool searchUser;           // use SearchUser() for BCE, else SearchNear()
   vector<RinexSatID> exclSat;// exclude satellites

   bool PisY;                 // Interpret RINEX 2 P code as if the receiver was keyed
   bool SPSout,ORDout;        // output autonomous solutions? ORDs?
   bool outver2;              // output RINEX version 2 (OutputObsFile)
   string LogFile;            // output log file (required)
   string OutputORDFile;      // output ORD file
   string OutputObsFile;      // output RINEX obs file
   string userfmt;            // user's time format for output
   string refPosStr;          // temp used to parse --ref input

   vector<string> inSolDesc;  // input: strings sys,freq,code e.g. GPS+GLO,1+2,PC
   bool SOLhelp;              // print more help info

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
   list<RinexMetData> MetStore;
   map<RinexSatID,double> P1C1bias;
   map<RinexSatID,int> GLOfreqChannel;
   int PZ90ITRFold, PZ90WGS84old;   // Helmert transforms before 20 Sept 07
   int PZ90ITRF, PZ90WGS84;         // Helmert transforms after 20 Sept 07

   // trop models
   TropModel *pTrop;          // to pass to PRS
   string TropType;           // key ~ Black, NewB, etc; use to identify model
   bool TropPos,TropTime;     // true when trop model has been init with Pos,time
                              // default weather
   double defaultTemp,defaultPress,defaultHumid;

   // solutions to build
   vector<SolutionObject> SolObjs;     // solution objects to process

   // reference position and rotation matrix
   Position knownPos;         // position derived from --ref
   Matrix<double> Rot;        // Rotation matrix (R*XYZ=NEU) :

   // useful stuff
   string msg;                      // temp used everywhere
   // vector of 1-char strings containing systems needed in all solutions: G,R,E,C,S,J
   vector<string> allSystemChars;

   string PrgmDesc, cmdlineUsage, cmdlineErrors, cmdlineExtras;
   vector<string> cmdlineUnrecognized;

}; // end class Configuration

//------------------------------------------------------------------------------------
// const members of Configuration
const string Configuration::PrgmName = string("PRSolve");
const string Configuration::calfmt = string("%04Y/%02m/%02d %02H:%02M:%02S");
const string Configuration::gpsfmt = string("%4F %10.3g");
const string Configuration::longfmt = calfmt + " = %4F %w %10.3g %P";

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// Encapsulate one observation datum that will be input to PRSolution, including a
// string that comes from the solution descriptor, constants in the linear combination
// and RinexObsID observation types matching those available in RINEX header.
// e.g. GPS:1:PC => "G1PC" => 1.0 * GC1P or GC1C
//      GPS:12:PC => "G12PC" => a+1/a * GC1P/C , -1/a * GC2P/C   a=alpha(G,12)
// SolutionObject contains a vector of these:
// e.g. GPS:12:PC+GLO:12:PC => "G12PC" + "R12PC" =>
//        (a+1/a * GC1P/C, -1/a * GC2P/C) + (a+1/a * RC1P/C, -1/a * RC2P/C)
// Used within SolutionObject only; assumes valid input everywhere!
class SolutionData {
public:
   // short string version of descriptor, e.g. GPS:12:PC  =>  G12PC
   string sfcodes;
   // vector of constants in linear combination, parallel to obsids
   // probably (a+1)/a and -1/a where a is alpha(freq1,freq2)
   vector<double> consts;
   // vector of RinexObsIDs in linear combination, parallel to consts
   vector<vector<string> > obsids;
   // vector of indexes into the RinexObsData map for each obsids
   vector<vector<int> > indexes;

   // For use in ComputeData()
   // ObsIDs actually used - parallel to consts - passed to DAT output
   vector<string> usedobsids;
   // raw pseudoranges
   vector<double> RawPR;
   // computed pseudorange and iono delay
   double PR, RI;

   // default and only constructor; input must NOT have + but may have dual freq
   SolutionData(const string& desc)
   {
      vector<string> fields = split(desc,':');
      sfcodes = ObsID::map3to1sys[fields[0]];   // first char of sfcodes
      // const char csys(sfcodes[0]);
      sfcodes += fields[1];                     // 1 or 2 freq chars
      sfcodes += fields[2];
   }

   // Destructor
   ~SolutionData() {}

   // get the system as 1-char string
   string getSys(void) { return string(1,sfcodes[0]); }

   // get the freqs as string
   string getFreq(void)
   {
      if(isDigitString(sfcodes.substr(1,2)))
         return sfcodes.substr(1,2);
      return sfcodes.substr(1,1);
   }

   // get codes
   string getCodes(void)
   {
      if(isDigitString(sfcodes.substr(1,2)))
         return sfcodes.substr(3);
      return sfcodes.substr(2);
   }

   // dump
   string asString(void)
   {
      size_t i,j;
      ostringstream oss;
      oss << "(" << sfcodes << ")";
      oss << " " << ObsID::map1to3sys[sfcodes.substr(0,1)];
      for(i=0; i<consts.size(); i++) {
         oss << " [c=" << fixed << setprecision(3) << consts[i];
         for(j=0; j<obsids[i].size(); j++)
            oss << (j==0 ? " o=":",") << obsids[i][j];
         oss << "]";
      }
      return oss.str();
   }

   // define the consts and obsids vectors, given the obstype map from RINEX header
   bool ChooseObsIDs(map<string,vector<RinexObsID> >& mapObsTypes)
   {
      size_t i,j,k;
      string sys1=getSys();
      string frs=getFreq();
      string codes=getCodes();

      for(i=0; i<frs.size(); i++) // loop over frequencies
      {
         // add place holders now
         consts.push_back(1.0);
         vector<string> vs;
         obsids.push_back(vs);
         vector<int> vi;
         indexes.push_back(vi);

         for(j=0; j<codes.size(); j++)
         {  // loop over codes
            // the desired ObsID
            string obsid = string("C") + string(1,frs[i]) + string(1,codes[j]);

            // now loop over available RinexObsTypes : map<1-char sys, string RObsID>
            map<string,vector<RinexObsID> >::const_iterator it;
            for(it=mapObsTypes.begin(); it != mapObsTypes.end(); ++it)
            {
               // wrong GNSS system
               if(it->first != sys1)
                  continue;
               
               // loop over obs types
               const vector<RinexObsID>& vecROID(it->second);
               for(k=0; k<vecROID.size(); k++)
               {
                  if(vecROID[k].asString() == obsid)
                  {
                     obsids[i].push_back(obsid);
                     indexes[i].push_back(k);
                  }
               }
            }  // end loop over RINEX obs types in header
         }  // end loop over codes
      }  // end loop over freqs

      // no obs ids found, for either frequency
      if(obsids[0].size() == 0 || (obsids.size() > 1 && obsids[1].size() == 0))
         return false;

      // compute constants
      if(obsids.size() > 1) {
         int na(asInt(frs.substr(0,1)));
         int nb(asInt(frs.substr(1,1)));
         RinexSatID sat(sys1);
         sat.fromString(sys1);
         double alpha = getAlpha(sat,na,nb);
         if(alpha == 0.0) return false;
         consts[1] = -1.0/alpha;
         consts[0] = 1.0 - consts[1];
      }

      return true;
   }  // end ChooseObsIDs()

   // compute the actual datum, for the given satellite, given the RinexObsData vector
   // remember which ObsIDs were actually used, in usedobsids
   // return true if the data could be computed
   bool ComputeData(const RinexSatID& sat, const vector<RinexDatum>& vrd)
   {
      usedobsids.clear();
      RawPR.clear();
      PR = RI = 0.0;

      string sys1=getSys();
      if(sys1[0] != sat.systemChar())              // wrong system
         return false;

      string frs=getFreq();
      for(size_t i=0; i<frs.size(); i++) {         // loop over frequencies
         RawPR.push_back(0.0);                     // placeholder = 0 == missing
         usedobsids.push_back(string("---"));      // placeholder == none
         for(size_t j=0; j<indexes[i].size(); j++) {// loop over codes (RINEX indexes)
            int k = indexes[i][j];
            if(vrd[k].data == 0.0)                 // data is no good
               continue;
            usedobsids[i] = obsids[i][j];          // use this ObsID
            RawPR[i] = vrd[k].data;                // use this data
            PR += RawPR[i] * consts[i];            // compute (dual-freq) PR
            break;
         }
      }

      // missing data?
      if(RawPR[0]==0.0 || (frs.size()>1 && RawPR[1]==0.0)) return false;

      // iono delay
      if(consts.size() > 1) RI = consts[1]*(RawPR[0] - RawPR[1]);

      return true;
   }  // end ComputeData()

   // compute and return a string of the form fc[fc] (freq code freq code) giving
   // the frequency and code of the data actually used by ComputeData. If no data
   // was available, then use '-' for the code.
   string usedString(void)
   {
      string msg;
      string frs = getFreq();
      for(size_t i=0; i<frs.size(); i++) {
         msg += frs[i];
         msg += (usedobsids.size() > i ? usedobsids[i].substr(2,1) : "?");
      }
      return msg;
   }

}; // end class SolutionData

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

   // static function to determine consistency of input descriptor
   // msg returns explanation
   static bool ValidateDescriptor(string desc, string& msg);

   // check validity of input descriptor, set default values
   void Initialize(const string& desc) throw()
   {
      if(!ValidateDescriptor(desc, Descriptor)) {
         isValid = false;
         return;
      }
      isValid = true;

      // parse desc into systems, freqs, codes, etc
      Descriptor = desc;
      ParseDescriptor();

      nepochs = 0;

      // for initialization of constants and PRSolution
      Configuration& C(Configuration::Instance());

      // set defaults in PRSolution
      prs.RMSLimit = C.RMSLimit;
      prs.SlopeLimit = C.SlopeLimit;
      prs.NSatsReject = C.maxReject;
      prs.MaxNIterations = C.nIter;
      prs.ConvergenceLimit = C.convLimit;

      // specify systems in PRSolution
      // sysChars ~ vec<1-char string> ~ G,R,E,C,S; must have at least one member
      RinexSatID sat;
      for(size_t i=0; i<sysChars.size(); i++) {
         sat.fromString(sysChars[i]);               // 1-char string
         prs.SystemIDs.push_back(sat.system);
         LOG(DEBUG) << " Add sys " << sysChars[i] << " = " << sat
            << " to PRS::SystemIDs";
      }

      // initialize apriori solution
      if(C.knownPos.getCoordinateSystem() != Position::Unknown)
         prs.memory.fixAPSolution(C.knownPos.X(),C.knownPos.Y(),C.knownPos.Z());

      return;
   }

   // parse descriptor into member data and 'sysChars'
   void ParseDescriptor(void) throw();

   // Given a RINEX header, verify that the necessary ObsIDs are present, and
   // define an ordered set of ObsIDs for each component and SolutionData required.
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
   // pass it iret from ComputeSolution
   int WriteORDs(const CommonTime& t, const int iret) throw(Exception);

   // Output final results
   void FinalOutput(void) throw(Exception);

// member data

   // true unless descriptor is not valid, or required ObsIDs are not available
   bool isValid;

   // solution descriptor: sys[+sys]:freq[+freq]:codes[+codes]
   // sys+sys implies codes+codes; codes is string of 'attribute' characters (ObsID)
   // giving the preferred ObsID (sys/C/freq/attr) to use as the data
   string Descriptor;

   // vector of SolutionData, one for each data component required in solution (1+).
   vector<SolutionData> vecSolData;

   // vector of 1-char strings containing systems needed in this solution: G,R,E,C,S,J
   vector<string> sysChars;

   // vector of SatID::SatelliteSystem satellite systems parallel to sysChars
   vector<SatID::SatelliteSystem> satSyss;

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
   //cout << C.Title << endl;
   
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
      for(size_t i=0; i<C.SolObjs.size(); ++i) {
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
   int nread,nrec;
   size_t i,j,nfile;
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

   // -------- SP3 files --------------------------
   // if Rinex clock files are to be loaded, tell the SP3 reader so
   bool useSP3clocks(C.InputClkFiles.size() == 0);

   // read ephemeris files and fill store
   // first sort them on start time; this for ultra-rapid files, which overlap in time
   if(C.InputSP3Files.size() > 0) {
      if(!useSP3clocks) {
         // if RINEX clocks are to be loaded, ignore the clock in the SP3 files
         C.SP3EphStore.rejectBadClocks(false);
         // this causes the store to ignore the SP3 clock; read RINEX clock later
         C.SP3EphStore.useRinexClockData();
      }

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
         catch(std::exception& e) {
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

   // ------------- configure and dump SP3 and clock stores -----------------
   if(isValid && C.SP3EphStore.ndata() > 0) {
      LOG(VERBOSE) << "Read " << nread << " SP3 ephemeris files into store.";
      LOG(VERBOSE) << "SP3 Ephemeris store contains "
         << C.SP3EphStore.ndata() << " data";

      // set to linear interpolation, as this is best estimate for clocks - TD input?
      C.SP3EphStore.setClockLinearInterp();     // changes 'interp order' to 1

      vector<SatID> sats(C.SP3EphStore.getSatList());
      RinexSatID sat(sats[sats.size()-1]);
      double dtp = C.SP3EphStore.getPositionTimeStep(sat);
      double dtc = C.SP3EphStore.getClockTimeStep(sat);
      LOG(VERBOSE) << "\nSP3 Ephemeris Store time intervals for " << sat
         << " are " << dtp << " (pos), and " << dtc << " (clk)";
      LOG(VERBOSE) << "SP3 Ephemeris store time system "
         << C.SP3EphStore.getTimeSystem().asString();

      // set gap checking - don't b/c TimeStep may vary GPS/GLO
      // TD this is a problem
      //C.SP3EphStore.setClockGapInterval(C.SP3EphStore.getClockTimeStep()+1.);
      //C.SP3EphStore.setClockMaxInterval(2*C.SP3EphStore.getClockTimeStep()+1.);

      // ignore predictions for now // TD make user input?
      C.SP3EphStore.rejectPredPositions(true);
      C.SP3EphStore.rejectPredClocks(true);

      // set gap checking  NB be sure InterpolationOrder is set first
      C.SP3EphStore.setPositionInterpOrder(10);
      C.SP3EphStore.setPosGapInterval(dtp+1.);
      C.SP3EphStore.setPosMaxInterval(
         (C.SP3EphStore.getInterpolationOrder()-1) * dtp + 1.);

      // dump the SP3 ephemeris store; while looping, check the GLO freq channel
      LOG(VERBOSE) << "\nDump clock and position stores, including file stores";
      // NB clock dumps are huge!
      if(C.verbose) C.SP3EphStore.dump(LOGstrm, (C.debug > 6 ? 2 : 1));
      LOG(VERBOSE) << "End of clock store and ephemeris store dumps.";

      // dump a list of satellites, with counts, times and GLO channel
      C.msg = "";
      LOG(INFO) << "\nDump ephemeris sat list with count, times and GLO channel.";
      for(i=0; i<sats.size(); i++) {                           // loop over sats
         // check for some GLO channel - can't compute b/c we don't have data yet
         if(sats[i].system == SatID::systemGlonass) {
            map<RinexSatID,int>::const_iterator it(C.GLOfreqChannel.find(sats[i]));
            if(it == C.GLOfreqChannel.end()
                           && sats[i].system == RinexSatID::systemGlonass) {
               //LOG(WARNING) << "Warning - no input GLONASS frequency channel "
               //   << "for satellite " << RinexSatID(sats[i]);
               // set it to zero
               C.GLOfreqChannel.insert(map<RinexSatID,int>::value_type(sats[i],0));
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

         // set search method
         if(C.searchUser) C.RinEphStore.SearchUser();
         else             C.RinEphStore.SearchNear();
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
         LOG(VERBOSE) << "Read " << nread << " RINEX navigation files, containing "
            << nrec << " records, into store.";
         LOG(VERBOSE) << "GPS ephemeris store contains "
            << C.RinEphStore.size(SatID::systemGPS) << " ephemerides.";
         LOG(VERBOSE) << "GAL ephemeris store contains "
            << C.RinEphStore.size(SatID::systemGalileo) << " ephemerides.";
         LOG(VERBOSE) << "BDS ephemeris store contains "
            << C.RinEphStore.size(SatID::systemBeiDou) << " ephemerides.";
         LOG(VERBOSE) << "QZS ephemeris store contains "
            << C.RinEphStore.size(SatID::systemQZSS) << " ephemerides.";
         LOG(VERBOSE) << "GLO ephemeris store contains "
            << C.RinEphStore.size(SatID::systemGlonass) << " satellites.";
         // dump the entire store
         C.RinEphStore.dump(LOGstrm,(C.debug > -1 ? 2:0));
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
               if(C.MetStore.size() == 1) {
                  LOG(VERBOSE) << "  Met store is at single time "
                     << printTime(it->time,C.longfmt);
               }
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

            if(C.P1C1bias.find(sat) != C.P1C1bias.end()) {
               LOG(WARNING) << "Warning : satellite " << sat
                  << " is duplicated in P1-C1 bias file(s)";
            }
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
      if(::fabs(s-sow) > 1.0) LOG(WARNING) << "Warning : decimation reference time "
         << "(--start) is not an even GPS-seconds-of-week mark.";
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

   // Choose transforms to be used; dump the available Helmert Tranformations
   LOG(INFO) << "\nAvailable Helmert Tranformations:";
   for(i=0; i<(size_t)HelmertTransform::stdCount; i++) {
      const HelmertTransform& ht(HelmertTransform::stdTransforms[i]);
      // pick the ones to use
      C.msg = "";
      if(ht.getFromFrame() == ReferenceFrame::PZ90) {
         if(ht.getToFrame() == ReferenceFrame::ITRF) {
            if(ht.getEpoch() >= HelmertTransform::PZ90Epoch)
               { C.PZ90ITRF = i; C.msg = "\n  [use this for PZ90-ITRF]"; }
            else
               { C.PZ90ITRFold = i; C.msg = "\n  [use this for PZ90-ITRF old]"; }
         }
         else if(ht.getToFrame() == ReferenceFrame::WGS84) {
            if(ht.getEpoch() >= HelmertTransform::PZ90Epoch)
               { C.PZ90WGS84 = i; C.msg = "\n  [use this for PZ90-WGS84]"; }
            else
               { C.PZ90WGS84old = i; C.msg = "\n  [use this for PZ90-WGS84 old]"; }
         }
      }

      LOG(INFO) << i << " " << ht.asString() << C.msg;
   }
   LOG(INFO) << "End of Available Helmert Tranformations.\n";

   // ----- build SolutionObjects from solution descriptors inSolDesc -----
   // these may be invalid, or there may not be data for them -> invalid
   for(j=0,i=0; i<C.inSolDesc.size(); i++) {
      string msg;
      LOG(DEBUG) << "Build solution object from descriptor " << C.inSolDesc[i];
      if(!SolutionObject::ValidateDescriptor(C.inSolDesc[i],msg)) {
         LOG(WARNING) << "Warning : --sol " << msg;
         continue;
      }

      // create a solution object
      SolutionObject SO(C.inSolDesc[i]);
      if(!SO.isValid) {
         LOG(WARNING) << "Warning : solution descriptor " << C.inSolDesc[i]
            << " could not be created - ignore";
         continue;
      }

      // is there ephemeris for each system?
      bool ok=true;
      for(size_t k=0; k<SO.sysChars.size(); k++) {
         RinexSatID sat;
         sat.fromString(SO.sysChars[k]);
         LOG(INFO) << " Found system " << SO.sysChars[k]
            << " with " << C.RinEphStore.size(sat.system) << " ephemerides.";
         if((C.pEph == &C.RinEphStore && C.RinEphStore.size(sat.system) == 0) ||
            (C.pEph == &C.SP3EphStore && C.SP3EphStore.ndata(sat.system) == 0)) {
            LOG(WARNING) << "Warning - no ephemeris found for system "
               << ObsID::map1to3sys[SO.sysChars[k]] << ", in solution descriptor "
               << C.inSolDesc[i] << " => invalidate.";
            ok = false;
         }
      }
      if(!ok) continue;

      // save the SolutionObject
      C.SolObjs.push_back(SO);
      LOG(DEBUG) << "Initial solution #" << ++j << " " << C.inSolDesc[i];
   }  // end loop over input solution descriptors

   if(C.SolObjs.size() == 0) {
      LOG(ERROR) << "Error: No valid solution descriptors";
      isValid = false;
   }

   // keep a list of all system characters used, for convenience
   C.allSystemChars.clear();
   for(i=0; i<C.SolObjs.size(); i++) {
      for(j=0; j<C.SolObjs[i].sysChars.size(); j++) {
         if(find(C.allSystemChars.begin(), C.allSystemChars.end(),
               C.SolObjs[i].sysChars[j]) == C.allSystemChars.end())
            C.allSystemChars.push_back(C.SolObjs[i].sysChars[j]);
      }
   }
   if(C.debug > -1) {
      ostringstream oss;
      oss << "List of all systems needed for solutions";
      for(i=0; i<C.allSystemChars.size(); i++) oss << " " << C.allSystemChars[i];
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
   int k,iret,nfiles;
   size_t i,j,nfile;
   Position PrevPos(C.knownPos);
   Rinex3ObsStream ostrm;

   for(nfiles=0,nfile=0; nfile<C.InputObsFiles.size(); nfile++) {
      Rinex3ObsStream istrm;
      Rinex3ObsHeader Rhead, Rheadout;
      Rinex3ObsData Rdata;
      string filename(C.InputObsFiles[nfile]);
      
      if (C.PisY)
      {
         LOG(DEBUG) << "Converting P/W code data to Y code";
         Rhead.PisY = C.PisY;
      }

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
         LOG(VERBOSE) << "Time system for RINEX file " << filename
            << " is " << istrm.timesystem.asString();
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
                  Rheadout.prepareVer2Write();

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

      // Dump the solution descriptors and needed conversions ---------
      LOG(INFO) << "\nSolutions to be computed for this file:";
      for(i=0; i<C.SolObjs.size(); ++i) {
         bool ok(C.SolObjs[i].ChooseObsIDs(Rhead.mapObsTypes));

         LOG(INFO) << (ok ? " OK ":" NO ") << i+1 << " " << C.SolObjs[i].dump(0);
         LOG(INFO) << C.SolObjs[i].dump(0);
         if(C.verbose) for(j=0; j<C.SolObjs[i].sysChars.size(); j++) {
            TimeSystem ts;
            if(C.SolObjs[i].sysChars[j] == "G") ts = TimeSystem::GPS;
            if(C.SolObjs[i].sysChars[j] == "R") ts = TimeSystem::GLO;
            if(C.SolObjs[i].sysChars[j] == "E") ts = TimeSystem::GAL;
            if(C.SolObjs[i].sysChars[j] == "C") ts = TimeSystem::BDT;
            if(C.SolObjs[i].sysChars[j] == "S") ts = TimeSystem::GPS;
            if(C.SolObjs[i].sysChars[j] == "J") ts = TimeSystem::QZS;
            LOG(INFO) << C.RinEphStore.dumpTimeSystemCorrection(istrm.timesystem,ts);
         }
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
            if(find(C.allSystemChars.begin(),C.allSystemChars.end(),sys)
                  == C.allSystemChars.end())
            {
               LOG(DEBUG) << " Sat " << sat << " : system " << sys
                  << " is not needed.";
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
            double elev(0), ER(0), tcorr;
            if((C.elevLimit > 0 || C.weight || C.ORDout)
                              && PrevPos.getCoordinateSystem() != Position::Unknown) {
               CorrectedEphemerisRange CER;
               try {
                  CER.ComputeAtReceiveTime(Rdata.time, PrevPos, sat, *C.pEph);
                  elev = CER.elevation;
                  // const double azim = CER.azimuth;
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
         // tag for DAT - required for PRSplot
         C.msg = printTime(Rdata.time,"DAT "+C.gpsfmt);

         // compute and print the solution(s) ----------------
         for(i=0; i<C.SolObjs.size(); ++i) {
            // skip invalid descriptors
            if(!C.SolObjs[i].isValid) continue;

            // dump the "DAT" record
            LOG(INFO) << C.SolObjs[i].dump((C.debug > -1 ? 2:1), "RPF", C.msg);

            // compute the solution
            j = C.SolObjs[i].ComputeSolution(Rdata.time);

            // write ORDs, even if solution is not good
            if(C.ORDout) C.SolObjs[i].WriteORDs(Rdata.time,j);
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
   searchUser = false;
   defaultstartStr = string("[Beginning of dataset]");
   defaultstopStr = string("[End of dataset]");
   beginTime = gpsBeginTime = GPSWeekSecond(0,0.,TimeSystem::Any);
   endTime = CommonTime::END_OF_TIME;

   PisY = false;
   SOLhelp = false;

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

}  // end Configuration::SetDefaults()

//------------------------------------------------------------------------------------
void Configuration::SolDescHelp(void)
{
   // build the table
   string systs(ObsID::validRinexSystems);
   string freqs(ObsID::validRinexFrequencies);
   string codes;
   string space("   ");
   size_t i,j, k;
   
   // first find the length of the longest codes entry for each system
   map<char,int> syslen;
   for(i=0; i<systs.size(); i++) {
      for(k=0, j=0; j<freqs.size(); j++) {
         codes = ObsID::validRinexTrackingCodes[systs[i]][freqs[j]];
         strip(codes,' '); strip(codes,'*');
         // GPS C1N and C2N are not allowed
         if(systs[i] == 'G' && (freqs[j] == '1' || freqs[j] == '2')) strip(codes,'N');
         if(codes.length() > k) k=codes.length();
      }
      syslen[systs[i]] = k;
   }
   string table;
   table = space + string("Valid PR tracking codes for systems and frequencies:\n");
   string head;
   for(i=0; i<systs.size(); i++) {
      head += (i==0 ? space+string("freq| ") : string(" | "));
      codes = ObsID::map1to3sys[string(1,systs[i])];
      head += center(codes,syslen[systs[i]]);
   }
   //head += string("\n") + space + string(head.size()-space.size()+1,'-');
   table += head + string("\n");
   for(i=0; i<freqs.size(); i++) {
      table += space + string("  ") + string(1,freqs[i]);
      for(j=0; j<systs.size(); j++) {
         codes = ObsID::validRinexTrackingCodes[systs[j]][freqs[i]];
         strip(codes,' '); strip(codes,'*');
         // GPS C1N and C2N are not allowed
         if(systs[i] == 'G' && (freqs[j] == '1' || freqs[j] == '2')) strip(codes,'N');
         if(codes.empty()) codes = string("---");
         table += string(" | ") + center(codes,syslen[systs[j]]);
      }
      if(i < freqs.size()-1) table += string("\n");
   }

   ostringstream os;
   os << "=== Help for Solution Descriptors, option --sol <S:F:C> ===\n"
      << " The --sol option is repeatable, so all --sol solutions, if valid,\n"
      << "   will be computed and output in one run of the program.\n\n"
      << " Solution descriptors are of the form S:F:C where\n"
      << "   S is a system, one of:";
   for(i=0; i<systs.size(); i++) os <<" "<< ObsID::map1to3sys[string(1,systs[i])];
   //os << " or";
   //for(i=0; i<systs.size(); i++) os <<" "<< systs[i];
   os << endl;
   os << "   F is a frequency, one of:";
   for(i=0; i<freqs.size(); i++) os << " " << freqs[i];
   os << endl;
   os << "   C is an ordered set of one or more tracking codes, for example WPC\n"
      << "   These must be consistent - not all F and C apply to all systems.\n\n";
   os << " The S:F:C are the RINEX codes used to identify pseudorange "
      << "observations." << endl;
   os << table << endl << endl;
   os << " Example solution descriptors are GPS:1:P  GLO:3:I  BDS:7:Q\n"
      << "   These are single-frequency solutions, that is the GPS:1:P solution\n"
      << "   will use GPS L1 P-code pseudorange data to find a solution.\n"
      << " Dual frequency solutions are allowed; they combine data of different\n"
      << "   frequencies to eliminate the ionospheric delay, for example\n"
      << "   GPS:12:PC is the usual L1/L2-ionosphere-corrected GPS solution.\n"
      << " Triple frequency solutions are not supported.\n\n"
      << " More that one tracking code may be provided, for example GPS:12:PC\n"
      << "  This tells PRSolve to prefer P, but if it is not available, use C.\n\n"
      << " Finally, combined solutions may be specified, in which different\n"
      << "  data types, even from different systems, are used together.\n"
      << "  The component descriptors are combined using a '+'. For example\n"
      << "    GPS:12:PC+GLO:12:PC\n"
      << "  describes a dual frequency solution that uses both GPS and GLO\n"
      << "  L1/L2 P-code (or C/A) data in a single solution algorithm.\n";

   LOG(INFO) << Title;
   LOG(INFO) << os.str();
}

//------------------------------------------------------------------------------------
int Configuration::ProcessUserInput(int argc, char **argv) throw()
{
   // build the command line
   opts.DefineUsageString(PrgmName + " [options]");
   PrgmDesc = BuildCommandLine();

   // let CommandLine parse options; write all errors, etc to the passed strings
   int iret = opts.ProcessCommandLine(argc, argv, PrgmDesc,
                        cmdlineUsage, cmdlineErrors, cmdlineUnrecognized);

   // handle return values
   if(iret == -2) return iret;      // bad alloc
   if(iret == -3) return iret;      // invalid command line

   // SOLhelp: print explanation of Solution Descriptors
   if(SOLhelp) {
      SolDescHelp();
      return 1;
   }

   // help: print syntax page and quit
   if(opts.hasHelp()) {
      LOG(INFO) << Title;
      LOG(INFO) << cmdlineUsage;
      //// temp TEMP
      //{
      //   LOG(INFO) << "Valid RINEX observation IDs:";
      //   RinexObsID::dumpCheck(LOGstrm);
      //}
      return 1;
   }

   // extra parsing (perhaps add to cmdlineErrors, cmdlineExtras)
   //TD clau failure: cmdlineErrors, cmdlineExtras);
   //iret = ExtraProcessing();
   iret = ExtraProcessing(cmdlineErrors, cmdlineExtras);
   if(iret == -4) return iret;      // log file could not be opened

   // output warning / error messages
   if(cmdlineUnrecognized.size() > 0) {
      LOG(INFO) << "Warning - unrecognized arguments:";
      for(size_t i=0; i<cmdlineUnrecognized.size(); i++)
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
"   RMS  Nsats RMS TDOP PDOP GDOP Slope niter conv SAT [SAT ...]\n"
"   DAT  Ngood Ndata <SAT>:<freq><code> ... (list of sats with freq+code found)\n"
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
            "Input RINEX nav file name(s) (also cf. --BCEpast)");

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
   opts.Add(0, "BCEpast", "", false, false, &searchUser, "",
            "Use 'User' find-ephemeris-algorithm (else nearest) (--nav only)");
   opts.Add(0, "PisY", "", false, false, &PisY, "",
            "P code data is actually Y code data");
   opts.Add(0, "sol", "S:F:C", true, false, &inSolDesc,
            "# Solution Descriptors <S:F:C> define data used in solution algorithm",
            "Specify data System:Freqs:Codes to be used to generate solution(s)");
   opts.Add(0, "SOLhelp", "", false, false, &SOLhelp, "",
            "Show more information on --sol <Solution Descriptor>");

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
   //opts.Add(0, "memory", "", false, false, &doMemory, "",
   //         "Keep information between epochs, output APV etc.");
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
               double X(asDouble(fld[0])),Y(asDouble(fld[1])),Z(asDouble(fld[2]));
               knownPos.setECEF(X,Y,Z);
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
      static const string fmtGPS("%F,%g"),fmtCAL("%Y,%m,%d,%H,%M,%S %P");
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
            (i==0 ? beginTime : endTime).setTimeSystem(TimeSystem::Any);
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
         ossx << (i==0 ? "   Begin time --start" : "   End time --stop") << " is "
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
   //if(!msg.empty()) cmdlineErrors += msg;
   if(!msg.empty()) errors += msg;
   msg = ossx.str();
   //if(!msg.empty()) cmdlineExtras += msg;
   if(!msg.empty()) extras += msg;

   return 0;

} // end Configuration::ExtraProcessing() throw()

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
// handles mixed system descriptors (desc+desc) by split and call itself
// return true if system/freq/codes are consistent
bool SolutionObject::ValidateDescriptor(const string desc, string& msg)
{
   stripLeading(desc);
   stripTrailing(desc);
   if(desc.empty()) return false;

   size_t i,j;
   vector<string> fields = split(desc,'+');
   if(fields.size() > 1) {
      for(i=0; i<fields.size(); i++)
         if(! ValidateDescriptor(fields[i],msg))
            return false;
      return true;
   }

   // Now we can assumes descriptor is single system and does NOT contain +
   fields = split(desc,':');

   // test system
   string sys(fields[0]);
   string sys1(ObsID::map3to1sys[sys]);
   if(!sys1.size() || ObsID::validRinexSystems.find_first_of(sys1[0])==string::npos) {
      msg = desc + " : invalid system /" + sys + "/";
      return false;
   }
   char csys(sys1[0]);

   // test freq(s) and code(s)
   if(fields[1].size() > 2) {
      msg = desc + " : only 1 or 2 frequencies allowed";
      return false;
   }

   for(i=0; i<fields[1].size(); i++) {
      if(ObsID::validRinexTrackingCodes[csys].find(fields[1][i]) ==
         ObsID::validRinexTrackingCodes[csys].end())
      {
         msg = desc + string(" : invalid frequency /") + fields[1][i] + string("/");
         return false;
      }
      string codes = ObsID::validRinexTrackingCodes[csys][fields[1][i]];
      // GPS C1N and C2N are not allowed
      if(csys == 'G' && (fields[1][i] == '1'||fields[1][i] == '2')) strip(codes,'N');
      for(j=0; j<fields[2].size(); j++) {
         if(codes.find_first_of(fields[2][j]) == string::npos) {
            msg = desc + string(" : invalid code /") + fields[2][j] + string("/");
            return false;
         }
      }
   }

   return true;
}

//------------------------------------------------------------------------------------
// parse descriptor into member data and 'sysChars'
// called by Initialize() which is called by c'tor
// assumes descriptor has been validated.
void SolutionObject::ParseDescriptor(void) throw()
{
   size_t i;
   vector<string> fields;

   sysChars.clear();          // sysChars ~ vec<string> ~ G,R,E,C,S
   satSyss.clear();           // satSyss ~ vec<SatID::sys> parallel sysChars

   // split into components on '+'
   vector<string> descs(split(Descriptor,"+"));
   for(i=0; i<descs.size(); i++) {           // loop over component descriptors

      // create a SolutionData object for each component, of form SYS:F:Codes
      // vector<SolutionData> vecSolData;
      SolutionData sd(descs[i]);
      LOG(INFO) << "Parser(" << i << "): " << sd.asString();

      string sys1(sd.getSys()), sys3(ObsID::map3to1sys[sys1]), frs(sd.getFreq());

      // system
      if(find(sysChars.begin(),sysChars.end(),sys1) == sysChars.end()) {
         sysChars.push_back(sys1);           // add to sysChars
         RinexSatID sat(sys1);
         satSyss.push_back(sat.system);      // and add to satSyss
      }

      vecSolData.push_back(sd);

   }  // end loop over component descriptors
}

//------------------------------------------------------------------------------------
// Given a RINEX header, verify that the necessary ObsIDs are present, and
// define an ordered set of ObsIDs for each required SolutionData.
// Return true if enough ObsIDs are found (in header) to compute the solution.
bool SolutionObject::ChooseObsIDs(map<string,vector<RinexObsID> >& mapObsTypes)
   throw()
{
   size_t i;

   Configuration& C(Configuration::Instance());

   isValid = true;
   
   for (i=0; i<vecSolData.size(); i++)
   {
      SolutionData& sd(vecSolData[i]);
      bool coi = sd.ChooseObsIDs(mapObsTypes);
      if (!coi)
      {
         isValid = false;
         return false;
      }
      LOG(DEBUG) << " Chooser: " << vecSolData[i].asString();
   }

   return isValid;
}

//------------------------------------------------------------------------------------
string SolutionObject::dump(int level, string msg1, string msg2) throw()
{
   int j;
   size_t i;
   ostringstream oss;

   Configuration& C(Configuration::Instance());

   oss << msg1 << " " << Descriptor << (msg2.empty() ? "" : " "+msg2);

   if(level == 0) {
      for(i=0; i<vecSolData.size(); i++)
         oss << " [" << i << "]" << vecSolData[i].asString();
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

   for(size_t i=0; i<vecSolData.size(); i++) {
      if(vecSolData[i].ComputeData(sat,vrd)) {
         // add to data for this solution
         Satellites.push_back(sat);
         PRanges.push_back(vecSolData[i].PR);
         Elevations.push_back(elev);
         ERanges.push_back(ER);
         RIono.push_back(vecSolData[i].RI);
         R1.push_back(vecSolData[i].RawPR[0]);
         if(vecSolData[i].RawPR.size() > 1) R2.push_back(vecSolData[i].RawPR[1]);
         else R2.push_back(0.0);
         UsedObsIDs.insert(
            multimap<RinexSatID,string>::value_type(sat,vecSolData[i].usedString()));
      }
   }
}

//------------------------------------------------------------------------------------
// return 0 good, negative failure - same as RAIMCompute
int SolutionObject::ComputeSolution(const CommonTime& ttag) throw(Exception)
{
   try {
      int i,n,iret;
      Configuration& C(Configuration::Instance());

      // is there data?
      if(Satellites.size() < 4) {
         LOG(VERBOSE) << "Solution algorithm failed, not enough data"
            << " for " << Descriptor
            << " at time " << printTime(ttag,C.longfmt);
         return -3;
      }

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
         iret=prs.PreparePRSolution(ttag, Satellites, satSyss, PRanges, C.pEph, SVP);

         if(iret > -3) {
            //Vector<double> APSol(5,0.0),Resid,Slopes;
            Vector<double> Resid,Slopes;
            //if(prs.hasMemory) APSol = prs.memory.getAprioriSolution(satSyss);
            iret = prs.SimplePRSolution(ttag, Satellites, SVP,
                                        invMCov, C.pTrop,
                                        prs.MaxNIterations, prs.ConvergenceLimit,
                                        satSyss, Resid, Slopes);
         }

         if(iret < 0) { LOG(VERBOSE) << "SimplePRS failed "
            << (iret==-4 ? "to find ANY ephemeris" :
               (iret==-3 ? "to find enough satellites with data" :
               (iret==-2 ? "because the problem is singular" :
              /*iret==-1*/ "because the algorithm failed to converge")))
            << " for " << Descriptor
            << " at time " << printTime(ttag,C.longfmt) << " iret " << iret;
         }
         else {
            // at this point we have a good solution

            // output XYZ solution
            LOG(INFO) << prs.outputString(string("SPS ")+Descriptor,iret);

            if(prs.RMSFlag || prs.SlopeFlag || prs.TropFlag)
               LOG(WARNING) << "Warning for " << Descriptor
                  << " - possible degraded SPS solution at "
                  << printTime(ttag,C.longfmt) << " due to"
                  << (prs.RMSFlag ? " large RMS":"")           // NB strings are used
                  << (prs.SlopeFlag ? " large slope":"")       // in PRSplot.pl
                  << (prs.TropFlag ? " missed trop. corr.":"");

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
      iret = prs.RAIMCompute(ttag, Satellites, satSyss, PRanges, invMCov, C.pEph,
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
         LOG(WARNING) << "Warning for " << Descriptor
            << " - possible degraded RPF solution at "
            << printTime(ttag,C.longfmt) << " due to"
            << (prs.RMSFlag ? " large RMS":"")           // NB these strings are used
            << (prs.SlopeFlag ? " large slope":"")       // in PRSplot.pl
            << (prs.TropFlag ? " missed trop. corr.":"");

      // dump pre-fit residuals
      if(prs.hasMemory && ++nepochs > 1)
         LOG(VERBOSE) << "RPF " << Descriptor << " PFR"
            << " " << printTime(ttag,C.gpsfmt)              // time
            << fixed << setprecision(3)
            << " " << ::sqrt(prs.memory.getAPV())           // sig(APV)
            << " " << setw(2) << prs.PreFitResidual.size()  // n resids
            << " " << prs.PreFitResidual;                   // pre-fit residuals

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
int SolutionObject::WriteORDs(const CommonTime& time, const int iret) throw(Exception)
{
   try {
      Configuration& C(Configuration::Instance());

      int j;
      size_t i;
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
            << " " << iret
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
   
      if(prs.memory.getN() <= 0) {
         LOG(INFO) << " No data!";
         return;
      }

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
            for(size_t i=0; i<Cov.rows(); i++) for(size_t j=i; j<Cov.cols(); j++)
               Cov(i,j) = Cov(j,i) = Cov(i,j)*apv;

            // print this covariance as labelled matrix
            Namelist NL;
            NL += "North"; NL += "East "; NL += "Up   ";
            LabeledMatrix LM(NL,Cov);
            LM.scientific().setprecision(3).setw(14);
            LOG(INFO) << "Covariance of " << statsNEUresid.getMessage() << endl << LM;
         }
      }

   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
