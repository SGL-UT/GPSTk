/**
 * @file TECMaps.cpp
 * Program TECMaps reads a set of Rinex files containing observation types
 *    EL, AZ, and VR or SR and fits the ionospheric vertical TEC data to a model
 *    of the ionosphere. There are input options for the type of grid, the type of
 *    model, and the type of data (VTEC, MUF or F0F2) to be used. TD ...
 */

#pragma ident "$Id$"


//------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
#include "StringUtils.hpp"
#include "DayTime.hpp"
#include "CommandOption.hpp"
#include "CommandOptionWithTimeArg.hpp"
#include "CommandOptionParser.hpp"

#include "BCEphemerisStore.hpp"
#include "SP3EphemerisStore.hpp"
#include "WGS84Geoid.hpp"
#include "Position.hpp"

#include "VTECMap.hpp"
#include "RinexUtilities.hpp"

#include <time.h>
#include <iostream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------------
using namespace gpstk;
using namespace std;

//------------------------------------------------------------------------------------
// input data
bool verbose,debug;
   // log file
string LogFile;
ofstream oflog;
string Title;
   // input path
string InputPath;
string Title1,Title2,BaseName,BiasFile;
double DecorrelError,ElevThresh,MinAcqTime;
double BeginLat,DeltaLat,BeginLon,DeltaLon;
int NumLat,NumLon;
VTECMap::FitType typefit;
VTECMap::GridType typegrid;
bool doVTECmap,doMUFmap,doF0F2map;
Station refSite;
string KnownPos;         // string holding position x,y,z or l,l,h
bool KnownLLH;           // if true, KnownPos is l,l,h
bool GridOut;            // if true, write grid to file 'basename.LL'
bool GnuplotFormat;      // if true, write grid in format for gnuplot
   // excluded satellites
vector<RinexPrn> ExSV;
   // ephemeris
string NavDir;
vector<string> NavFiles;
SP3EphemerisStore SP3EphList;
BCEphemerisStore BCEphList;
EphemerisStore *pEph;
   // obs types needed
RinexObsHeader::RinexObsType ELot,AZot,VRot,SRot,TPot;
RinexObsHeader::RinexObsType LAot,LOot;      // TEMP
   // geoid
WGS84Geoid WGS84;
   // Start and stop times
DayTime BegTime(DayTime::BEGINNING_OF_TIME),EndTime(DayTime::END_OF_TIME);
   // processing
double IonoHt;
DayTime EarliestTime;
VTECMap vtecmap;
MUFMap mufmap;
F0F2Map f0f2map;
   // map of input sat+rx biases
map<string,map<RinexPrn,double> > BiasMap;
   // Data structures for all receivers
vector<Station> Stations;
RinexObsStream *instream; // array of streams, parallell to Stations

//------------------------------------------------------------------------------------
// prototypes
void ConfigureAndDefaults(void);
int GetCommandLine(int argc, char **argv);
void PreProcessArgs(const char *arg, vector<string>& Args);
int Initialize(void);
int ProcessStations(void);
void ProcessObsAndComputeMap(void);
void OutputGridToFile(VTECMap& vmap, string filename);
void OutputMapToFile(VTECMap& vtmap, string filename, DayTime t, int n);
void AddStation(string& filename);
int ProcessHeader(Station& S);
int ReadNextObs(Station& S);
int ProcessObs(Station& S, vector<ObsData>& obsvect);

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   int iret;
   clock_t totaltime=clock(); // timer
   DayTime CurrEpoch;

      // Title description and run time
   CurrEpoch.setLocalTime();
   Title = "TECMaps, built on the GPSTK ToolKit, Ver 1.0 8/12/04, Run ";
   Title += CurrEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S\n");
   cout << Title;

      // define extended types
   iret = RegisterARLUTExtendedTypes();
   if(iret) goto quit;
   iret = RegisterExtendedRinexObsType("TP","Acquisition time","seconds", 0);
   if(iret) goto quit;

      // set configuration and default values
   ConfigureAndDefaults();

      // get command line arguments
   iret = GetCommandLine(argc, argv);
   if(iret) goto quit;

      // initialize
   iret = Initialize();
   if(iret) goto quit;

      // make the grid
   if(doVTECmap) {
      vtecmap.MakeGrid(refSite);
      if(GridOut) OutputGridToFile(vtecmap, BaseName+string(".LL"));
   }
   if(doMUFmap) {
      mufmap.MakeGrid(refSite);
      if(GridOut) OutputGridToFile(mufmap, BaseName+string(".MUF.LL"));
   }
   if(doF0F2map) {
      f0f2map.MakeGrid(refSite);
      if(GridOut) OutputGridToFile(f0f2map, BaseName+string(".F0F2.LL"));
   }

      // process the headers, filling the Stations array
   iret = ProcessStations();
   if(iret) goto quit;

      // process the all the observation data
   ProcessObsAndComputeMap();

quit:
      // compute run time
   totaltime = clock()-totaltime;
   cout << "TECMaps timing: " << fixed << setprecision(3)
      << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.\n";
   oflog << "TECMaps timing: " << fixed << setprecision(3)
      << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.\n";
   oflog.close();

   return iret;
}
catch(gpstk::Exception& e) {
   cerr << e;
   return 1;
}
catch (...) {
   cerr << "Unknown error.  Abort." << endl;
   return 1;
}
   return 0;
}   // end main()

//------------------------------------------------------------------------------------
// set defaults
void ConfigureAndDefaults(void)
{
   Title1=string("TECMaps main title");
   Title2=string("TECMaps sub title");
   BaseName=string("tecmap_out");
   DecorrelError = 3.0;
   BiasFile = string("");
   ElevThresh = 10.0;
   MinAcqTime = 0.0;
   BeginLat = 21.0;
   DeltaLat = 0.25;
   BeginLon = 230;
   DeltaLon = 1.0;
   NumLat = 40;
   NumLon = 40;
   typefit = VTECMap::Constant;
   typegrid = VTECMap::UniformLatLon;
   doVTECmap = true;
   doMUFmap = false;
   doF0F2map = false;
   LogFile = string("vtm.log");
   IonoHt = 350.0;         // km
   verbose = false;
   debug = false;
   KnownPos = string("");
   GridOut = false;
   GnuplotFormat = false;
}

//------------------------------------------------------------------------------------
// Define, parse and evaluate command line
int GetCommandLine(int argc, char **argv)
{
try {
   bool help=false;
   int i,j;
   RinexPrn prn;
   prn.setfill('0');

      // required options

   RequiredOption dashin(CommandOption::hasArgument, CommandOption::stdType,
      0,"input"," --input <file>       Input Rinex obs file name(s)"
                "                    \n(Reference site position also required)"
      );
   //dashin.setMaxCount(1);

      // optional options

   // this only so it will show up in help page...
   CommandOption dashf(CommandOption::hasArgument, CommandOption::stdType,
      'f',""," -f<file>             file containing more options");

   // reference position(s)
   CommandOption dashllh(CommandOption::hasArgument, CommandOption::stdType,0,"RxLLH",
     "Reference station position (one required):\n"
     " --RxLLH <l,l,h>      Reference site position in geodetic"
     " lat, lon (E), ht (deg,deg,m)");
   dashllh.setMaxCount(1);

   CommandOption dashxyz(CommandOption::hasArgument, CommandOption::stdType,0,"RxXYZ",
      " --RxXYZ <x,y,z>      Reference site position in ECEF coordinates (m)");
   dashxyz.setMaxCount(1);

      // require one only
   CommandOptionMutex refmutex(true);
   refmutex.addOption(&dashllh);
   refmutex.addOption(&dashxyz);

   CommandOption dashp(CommandOption::hasArgument, CommandOption::stdType,
      0,"inputdir"," --inputdir <path>    Path for input file(s)");
   dashp.setMaxCount(1);

   // ephemeris
   CommandOption dashnd(CommandOption::hasArgument, CommandOption::stdType, 0,
      "navdir","Ephemeris input:\n --navdir <dir>       Path of navigation file(s)");
   dashnd.setMaxCount(1);

   CommandOption dashn(CommandOption::hasArgument, CommandOption::stdType,
      0,"nav"," --nav <file>         Navigation (Rinex Nav OR SP3) file(s)");

   CommandOption dashl(CommandOption::hasArgument, CommandOption::stdType,
      0,"log","Output:\n --log <file>         Output log file name");
   dashl.setMaxCount(1);
   
   // time
   CommandOptionWithTimeArg dasheb(0,"BeginTime","%Y,%m,%d,%H,%M,%f",
      "Time limits:\n --BeginTime <arg>    Start time, arg is of the form "
      "YYYY,MM,DD,HH,Min,Sec");
   CommandOptionWithTimeArg dashgb(0,"BeginGPSTime","%F,%g",
      " --BeginGPSTime <arg> Start time, arg is of the form GPSweek,GPSsow");

   CommandOptionWithTimeArg dashee(0,"EndTime","%Y,%m,%d,%H,%M,%f",
      " --EndTime <arg>      End time, arg is of the form YYYY,MM,DD,HH,Min,Sec");
   CommandOptionWithTimeArg dashge(0,"EndGPSTime","%F,%g",
      " --EndGPSTime <arg>   End time, arg is of the form GPSweek,GPSsow");

   // allow ONLY one start time (use startmutex(true) if one is required)
   CommandOptionMutex startmutex(false);
   startmutex.addOption(&dasheb);
   startmutex.addOption(&dashgb);
   CommandOptionMutex stopmutex(false);
   stopmutex.addOption(&dashee);
   stopmutex.addOption(&dashge);

   CommandOptionNoArg dashVmap(0, "MUFmap","Processing:\n"
      " --noVTECmap          Do NOT create the VTEC map.");

   CommandOptionNoArg dashMUF(0, "MUFmap",
      " --MUFmap             Create MUF map as well as VTEC map.");

   CommandOptionNoArg dashF0F2(0, "F0F2map",
      " --F0F2map            Create F0F2 map as well as VTEC map.");

   CommandOption dashTitle1(CommandOption::hasArgument, CommandOption::stdType,
      0,"Title1"," --Title1 <title>     Title information");
   dashTitle1.setMaxCount(1);

   CommandOption dashTitle2(CommandOption::hasArgument, CommandOption::stdType,
      0,"Title2", " --Title2 <title>     Second title information");
   dashTitle2.setMaxCount(1);

   CommandOption dashBaseName(CommandOption::hasArgument, CommandOption::stdType,
      0,"BaseName", " --BaseName <name>    Base name for output files (a)");
   dashBaseName.setMaxCount(1);

   CommandOption dashDecor(CommandOption::hasArgument, CommandOption::stdType,
      0,"DecorrError",
      " --DecorrError <de>   Decorrelation error rate in TECU/1000km (3)");
   dashDecor.setMaxCount(1);

   CommandOption dashBiases(CommandOption::hasArgument, CommandOption::stdType,
      0,"Biases", " --Biases <file>      "
      "File containing estimated sat+rx biases (Prgm IonoBias)");
   dashBiases.setMaxCount(1);

   CommandOption dashElevThresh(CommandOption::hasArgument, CommandOption::stdType,
      0,"ElevThresh", " --ElevThresh <ele>   Minimum elevation (6 deg)");
   dashElevThresh.setMaxCount(1);

   CommandOption dashMinAcqTime(CommandOption::hasArgument, CommandOption::stdType,
      0,"MinAcqTime", " --MinAcqTime <t>     Minimum acquisition time (0 sec)");
   dashMinAcqTime.setMaxCount(1);

   CommandOptionNoArg dashFlatFit(
      0, "FlatFit", " --FlatFit            Flat fit type (default)");

   CommandOptionNoArg dashLinearFit(
      0, "LinearFit", " --LinearFit          Linear fit type");

   CommandOption dashIonoHt(CommandOption::hasArgument, CommandOption::stdType,
      0,"IonoHeight", " --IonoHeight <n>     Ionosphere height (km)");
   dashIonoHt.setMaxCount(1);

   CommandOptionNoArg dashUniSpace(0, "UniformSpacing",
      "Grid:\n --UniformSpacing     Grid uniform in space (XYZ) (default)");

   CommandOptionNoArg dashUniGrid(0, "UniformGrid",
      " --UniformGrid        Grid uniform in Lat and Lon");

   CommandOptionNoArg dashOutGrid(
      0, "OutputGrid", " --OutputGrid         Output the grid to file <basename.LL>");

   CommandOptionNoArg dashGnuOut(
      0, "GnuplotOutput", " --GnuplotOutput      Write the grid file for gnuplot"
      " (default: for Matlab)");

   CommandOption dashNumLat(CommandOption::hasArgument, CommandOption::stdType,
      0,"NumLat", " --NumLat <n>         Number of latitude grid points (40)");
   dashNumLat.setMaxCount(1);

   CommandOption dashNumLon(CommandOption::hasArgument, CommandOption::stdType,
      0,"NumLon", " --NumLon <n>         Number of longitude grid points (40)");
   dashNumLon.setMaxCount(1);

   CommandOption dashBeginLat(CommandOption::hasArgument, CommandOption::stdType,
      0,"BeginLat", " --BeginLat <lat>     Beginning latitude (21 deg)");
   dashBeginLat.setMaxCount(1);

   CommandOption dashBeginLon(CommandOption::hasArgument, CommandOption::stdType,
      0,"BeginLon", " --BeginLon <lon>     Beginning longitude (230 deg E)");
   dashBeginLon.setMaxCount(1);

   CommandOption dashDeltaLat(CommandOption::hasArgument, CommandOption::stdType,
      0,"DeltaLat", " --DeltaLat <del>     Grid spacing in latitude (0.25 deg)");
   dashDeltaLat.setMaxCount(1);

   CommandOption dashDeltaLon(CommandOption::hasArgument, CommandOption::stdType,
      0,"DeltaLon", " --DeltaLon <del>     Grid spacing in longitude (1.0 deg)");
   dashDeltaLon.setMaxCount(1);

   CommandOption dashXprn(CommandOption::hasArgument, CommandOption::stdType,
      '0', "XSat", "Other options:\n --XSat <sat>         Exclude this satellite "
      "(<sat> may be <system> only)");
   
   // ... other options
   CommandOptionNoArg dashv('v', "verbose",
         "Help:\n [-v|--verbose]       print extended output info.");

   CommandOptionNoArg dashd('d', "debug",
         " [-d|--debug]         print extended output info.");

   CommandOptionNoArg dashh('h', "help",
         " [-h|--help]          print syntax and summary of input, then quit.");

   // ... rest of the command line
   CommandOptionRest Rest("");

   CommandOptionParser Par(
      " Prgm TECMaps will open and read several preprocessed Rinex obs files\n"
      " (containing obs types EL,AZ,VR|SR) and use the data to ...\n"
      " Input is on the command line, or of the same format in a file (-f<file>).\n");

   // allow user to put all options in a file
   // could also scan for debug here
   vector<string> Args;
   for(j=1; j<argc; j++) PreProcessArgs(argv[j],Args);
   argc = Args.size();
   if(argc==0) Args.push_back(string("-h"));

      // pass the rest
   argc = Args.size()+1;
   char **CArgs=new char*[argc];
   if(!CArgs) { cerr << "Failed to allocate CArgs\n"; return -1; }
   CArgs[0] = argv[0];
   for(j=1; j<argc; j++) {
      CArgs[j] = new char[Args[j-1].size()+1];
      if(!CArgs[j]) { cerr << "Failed to allocate CArgs[j]\n"; return -1; }
      strcpy(CArgs[j],Args[j-1].c_str());
   }

   //if(debug) {
      //cout << "List passed to parse\n";
      //for(i=0; i<argc; i++) cout << i << " " << CArgs[i] << endl;
   //}
   Par.parseOptions(argc, CArgs);

   // help first
   if(dashh.getCount() > 0) {
      help = true;
   }

   // get values found on command line
   vector<string> values;

   // log file next
   if(dashl.getCount()) {
      values = dashl.getValue();
      if(help) cout << "Input name of output log file: " << values[0] << endl;
      LogFile = values[0];
   }
   oflog.open(LogFile.c_str(),ios_base::out);
   if(!oflog) {
      cerr << "Failed to open log file " << LogFile << endl;
      return -1;
   }
   cout << "TECMaps output directed to log file " << LogFile << endl;
   oflog << Title;

   // print syntax
   if(help) {
      Par.displayUsage(oflog,false); oflog << endl;
      Par.displayUsage(cout,false); cout << endl;
   }

   // errors on command line
   if(Par.hasErrors())
   {
      cerr << "\nErrors found in command line input:\n";
      Par.dumpErrors(cerr);
      cerr << "...end of Errors\n\n";
      oflog << "\nErrors found in command line input:\n";
      Par.dumpErrors(oflog);
      oflog << "...end of Errors\n\n";
      help = true;
   }
   
      // f never appears because we intercept it above
   //if(dashf.getCount()) { cout << "Option f "; dashf.dumpValue(cout); }
 
      // input path; do path before input file names
   if(dashp.getCount()) {
      values = dashp.getValue();
      if(help) cout << "Input path name: " << values[0] << endl;
      InputPath = values[0];
   }
   else InputPath = string("");

      // input file names -- create vector of Station here
   if(dashin.getCount()) {
      values = dashin.getValue();
      if(help) cout << "Input Rinex obs file names are:\n";
      string::size_type pos;
      string fname;
      for(i=0; i<values.size(); i++) {
         fname = values[i];
         // expand filenames of the form @name or name@
         // into the *contents* (one name per line) of file 'name'.
         pos = fname.find('@');
         if(pos == string::npos || (pos != 0 && pos != fname.length()-1)) {
               // value is a Rinex file name
            if(InputPath.size() > 0) { fname = InputPath + "/" + fname; }
            AddStation(fname);
            if(help) cout << "   " << fname << endl;
         }
         else {                 // value is a file containing Rinex file names
            fname.erase(pos,1);
            if(InputPath.size() > 0) { fname = InputPath + "/" + fname; }
            if(help) cout << "   " << "(Open and read file names from: "
               << fname << ")" << endl;
            ifstream infile(fname.c_str());
            if(!infile) {
               oflog << "Error: could not open file " << fname << endl;
            }
            else {
               while(infile >> fname) {
                  if(fname[0] == '#') {         // skip to end of line
                     char c;
                     while(infile.get(c)) { if(c=='\n') break; }
                  }
                  else {
                     AddStation(fname);
                     if(debug) oflog << "   " << fname << endl;
                  }
               }  // end loop over lines in the file
               infile.close();
            }  // end opened file
         }  // end if value is a file containing file names
      }  // end loop over values on command line
   }  // end dashin

   // reference position
   if(dashllh.getCount()) {
      values = dashllh.getValue();
      KnownPos = values[0];
      KnownLLH = true;
      if(help) cout << "Get reference position from explicit input (LLH):\n "
         << KnownPos << endl;
   }
   if(dashxyz.getCount()) {
      values = dashxyz.getValue();
      KnownPos = values[0];
      KnownLLH = false;
      if(help) cout << "Get reference position from explicit input (XYZ):\n "
         << KnownPos << endl;
   }

   if(KnownPos != string("")) {
      ECEF e;
      string::size_type pos;
      values.clear();
      while(KnownPos.size() > 0) {
         pos = KnownPos.find(",");
         if(pos==string::npos) pos=KnownPos.size();
         if(pos==0) values.push_back(" ");
         else values.push_back(KnownPos.substr(0,pos));
         if(pos >= KnownPos.size()) break;
         KnownPos.erase(0,pos+1);
      };
      refSite.filename = string("reference");
      if(values.size() > 3) refSite.filename=values[3];
      if(KnownLLH) {
         refSite.llr.setGeodetic(StringUtils::asDouble(values[0]),
                                 StringUtils::asDouble(values[1]),
                                 StringUtils::asDouble(values[2])); //WGS84 is default
         refSite.xyz = refSite.llr;
         try {
            refSite.llr.transformTo(Position::Geocentric);
            refSite.xyz.transformTo(Position::Cartesian);
         }
         catch(Exception& e) {
            cerr << "ERROR: Reference site input (geodetic LLH) is invalid\n";
            oflog << "ERROR: Reference site input (geodetic LLH) is invalid\n";
            return -2;
         }
      }
      else {
         refSite.xyz.setECEF(StringUtils::asDouble(values[0]),
                             StringUtils::asDouble(values[1]),
                             StringUtils::asDouble(values[2]));
         refSite.llr = refSite.xyz;
         refSite.llr.transformTo(Position::Geocentric);
      }
   }

   // ephemeris input
   if(dashnd.getCount()) {
      values = dashnd.getValue();
      NavDir = values[0];
      if(help) cout << "Input Nav Directory: " << NavDir  << endl;
   }
   if(dashn.getCount()) {
      values = dashn.getValue();
      NavFiles = values;
      if(help) {
         cout << "Input Nav files :";
         for(i=0; i<NavFiles.size(); i++) cout << " " << NavFiles[i];
         cout << endl;
      }
   }

   if(dasheb.getCount()) {
      values = dasheb.getValue();
      BegTime.setToString(values[0], "%Y,%m,%d,%H,%M,%S");
      if(help) cout << "Input BeginTime " << BegTime << endl;
   }
   if(dashee.getCount()) {
      values = dashee.getValue();
      EndTime.setToString(values[0], "%Y,%m,%d,%H,%M,%S");
      if(help) cout << "Input EndTime " << EndTime << endl;
   }
   if(dashgb.getCount()) {
      values = dashgb.getValue();
      BegTime.setToString(values[0], "%F,%g");
      if(help) cout << "Input BeginGPSTime " << BegTime << endl;
   }
   if(dashge.getCount()) {
      values = dashge.getValue();
      EndTime.setToString(values[0], "%F,%g");
      if(help) cout << "Input EndGPSTime " << EndTime << endl;
   }
      // processing
   if(dashVmap.getCount()) {
      doVTECmap = false;
      if(help) cout << "Do NOT create VTEC map" << endl;
   }
   if(dashMUF.getCount()) {
      doMUFmap = true;
      if(help) cout << "Create MUF map" << endl;
   }
   if(dashF0F2.getCount()) {
      doF0F2map = true;
      if(help) cout << "Create F0F2 map" << endl;
   }
   if(dashTitle1.getCount()) {
      values = dashTitle1.getValue();
      Title1 = values[0];
      if(help) cout << "Primary Title is " << Title1 << endl;
   }
   if(dashTitle2.getCount()) {
      values = dashTitle2.getValue();
      Title2 = values[0];
      if(help) cout << "Secondary Title is " << Title2 << endl;
   }
   if(dashBaseName.getCount()) {
      values = dashBaseName.getValue();
      BaseName = values[0];
      if(help) cout << "Base name for output files is " << BaseName << endl;
   }
   if(dashDecor.getCount()) {
      values = dashDecor.getValue();
      DecorrelError = StringUtils::asDouble(values[0]);
      if(help) cout << "Decorrelation error rate (TECU/1000km) is "
         << DecorrelError << endl;
   }
   if(dashNumLat.getCount()) {
      values = dashNumLat.getValue();
      NumLat = StringUtils::asInt(values[0]);
      if(help) cout << "Number of latitude grid points is " << NumLat << endl;
   }
   if(dashNumLon.getCount()) {
      values = dashNumLon.getValue();
      NumLon = StringUtils::asInt(values[0]);
      if(help) cout << "Number of longitude grid points is " << NumLon << endl;
   }
   if(dashBiases.getCount()) {
      values = dashBiases.getValue();
      BiasFile = values[0];
      if(help) cout << "Input sat+rx biases from file " << BiasFile << endl;
   }
   if(dashElevThresh.getCount()) {
      values = dashElevThresh.getValue();
      ElevThresh = StringUtils::asDouble(values[0]);
      if(help) cout << "Minimum elevation (deg) is " << ElevThresh << endl;
   }
   if(dashMinAcqTime.getCount()) {
      values = dashMinAcqTime.getValue();
      MinAcqTime = StringUtils::asDouble(values[0]);
      if(help) cout << "Minimum acquisition time (sec) is " << MinAcqTime << endl;
   }
   if(dashBeginLat.getCount()) {
      values = dashBeginLat.getValue();
      BeginLat = StringUtils::asDouble(values[0]);
      if(help) cout << "Beginning latitude (deg) is " << BeginLat << endl;
   }
   if(dashBeginLon.getCount()) {
      values = dashBeginLon.getValue();
      BeginLon = StringUtils::asDouble(values[0]);
      if(help) cout << "Beginning longitude (deg E) is " << BeginLon << endl;
   }
   if(dashDeltaLat.getCount()) {
      values = dashDeltaLat.getValue();
      DeltaLat = StringUtils::asDouble(values[0]);
      if(help) cout << "Grid step in latitude (deg) is " << DeltaLat << endl;
   }
   if(dashDeltaLon.getCount()) {
      values = dashDeltaLon.getValue();
      DeltaLon = StringUtils::asDouble(values[0]);
      if(help) cout << "Grid step in longitude (deg) is " << DeltaLon << endl;
   }
   if(dashUniSpace.getCount()) {
      typegrid = VTECMap::UniformSpace;
      if(help) cout << "Grid type is set to 'uniform spacing': " << typegrid << endl;
   }
   if(dashUniGrid.getCount()) {
      typegrid = VTECMap::UniformLatLon;
      if(help) cout << "Grid type is set to uniform: " << typegrid << endl;
   }
   if(dashOutGrid.getCount()) {
      GridOut = true;
      if(help) cout << "Output grid to file " << BaseName << ".LL" << endl;
   }
   if(dashGnuOut.getCount()) {
      GnuplotFormat = true;
      if(help) cout << "Output grid in gnuplot format" << endl;
   }
   if(dashFlatFit.getCount()) {
      typefit = VTECMap::Constant;
      if(help) cout << "Set fit type to FLAT" << endl;
   }
   if(dashLinearFit.getCount()) {
      typefit = VTECMap::Linear;
      if(help) cout << "Set fit type to LINEAR" << endl;
   }
   if(dashIonoHt.getCount()) {
      values = dashIonoHt.getValue();
      IonoHt = StringUtils::asDouble(values[0]);
      if(help) cout << "Ionosphere height = " << IonoHt << " km" << endl;
   }
   if(dashXprn.getCount()) {
      values = dashXprn.getValue();
      for(i=0; i<values.size(); i++) {
         prn = StringUtils::asData<RinexPrn>(values[i]);
         if(help) cout << "Input: exclude satellite " << prn << endl;
         ExSV.push_back(prn);
      }
   }

   if(dashh.getCount())
      oflog << "Option h appears " << dashh.getCount() << " times\n";
   if(dashv.getCount()) {
      verbose = true;
      if(help) cout << "Option v appears " << dashv.getCount() << " times\n";
   }
   if(dashd.getCount()) {
      debug = true;
      if(help) cout << "Option d appears " << dashd.getCount() << " times\n";
   }

   if(Rest.getCount() && help) {
      cout << "Remaining options:" << endl;
      values = Rest.getValue();
      for (i=0; i<values.size(); i++) cout << values[i] << endl;
   }
   if(verbose && help) {
      cout << "\nTokens on command line (" << Args.size() << ") are:" << endl;
      for(j=0; j<Args.size(); j++) cout << Args[j] << endl;
      cout << endl;
   }
   if(help) cout << "Configuration summarized in log file\n";

      // print configuration
   if(verbose || help || debug) {
      oflog << "\nInput configuration for TECMaps:\n";
      if(!InputPath.empty()) oflog << " Path for input files is "
         << InputPath << endl;
      oflog << " Input Rinex obs file names are:\n";
      for(i=0; i<Stations.size(); i++)
         oflog << "   " << Stations[i].filename << endl;
      if(!NavDir.empty()) oflog << " Path for input nav files is "
         << NavDir  << endl;
      if(NavFiles.size() > 0) {
         oflog << " Input Rinex nav file names are:\n";
         for(i=0; i<NavFiles.size(); i++)
            oflog << "   " << NavFiles[i] << endl;
      }
      if(BegTime > DayTime::BEGINNING_OF_TIME) oflog << " Begin time is "
         << BegTime.printf("%Y/%m/%d_%H:%M:%6.3f=%F/%10.3g") << endl;
      if(EndTime < DayTime::END_OF_TIME) oflog << " End   time is "
         << EndTime.printf("%Y/%m/%d_%H:%M:%6.3f=%F/%10.3g") << endl;
      oflog << " Processing:\n";
      oflog << "  Primary Title is " << Title1 << endl;
      oflog << "  Secondary Title is " << Title2 << endl;
      oflog << "  Reference " << refSite << endl;
      oflog << "  Do " << (doVTECmap ? "":"not ") << "create VTEC map\n";
      oflog << "  Do " << (doMUFmap ? "":"not ") << "create MUF map\n";
      oflog << "  Do " << (doF0F2map ? "":"not ") << "create F0F2 map\n";
      oflog << "  Grid type is "
         << (typegrid == VTECMap::UniformLatLon ? "Uniform " : "Uniform Space ")
         << typegrid << endl;
      oflog << "  Beginning latitude (deg) is " << BeginLat << endl;
      oflog << "  Beginning longitude (deg E) is " << BeginLon << endl;
      oflog << "  Number of latitude grid points is " << NumLat << endl;
      oflog << "  Number of latitude grid points is " << NumLat << endl;
      oflog << "  Grid step in latitude (deg) is " << DeltaLat << endl;
      oflog << "  Grid step in longitude (deg) is " << DeltaLon << endl;
      oflog << "  Minimum elevation (deg) is " << ElevThresh << endl;
      oflog << "  Minimum acquisition time (sec) is " << MinAcqTime << endl;
      if(BiasFile.length() > 0)
         oflog << "  Receiver biases input from file " << BiasFile << endl;
      else
         oflog << "  Do not input sat+rx biases" << endl;
      oflog << "  Decorrelation error rate (TECU/1000km) is " << DecorrelError
         << endl;
      oflog << "  Ionosphere height = " << IonoHt << " km" << endl;
      oflog << "  Base name for output files is " << BaseName << endl;
      cout << (GridOut ? "O":"Do NOT o") << "utput grid in file named " <<
         BaseName << ".LL" << endl;
      if(GridOut) cout << "Output grid in " << (GnuplotFormat ? "gnuplot" : "Matlab")
         << " format" << endl;
      if(ExSV.size() > 0) {
         oflog << "  Exclude these satellites:";
         for(i=0; i<ExSV.size(); i++) oflog << " " << ExSV[i];
         oflog << endl;
      }
      if(help) oflog << "  Help (-h) is set\n";
      if(debug) oflog << "  Debug (-d) is set\n";
      oflog << "End of input configuration for TECMaps" << endl << endl;
   }

   if(help) return 1;

   return 0;
}
catch(gpstk::Exception& e) {
      cerr << "TECMaps:GetCommandLine caught an exception\n" << e;
      GPSTK_RETHROW(e);
}
catch (...) {
      cerr << "TECMaps:GetCommandLine caught an unknown exception\n";
}
   return -1;
}

//------------------------------------------------------------------------------------
void PreProcessArgs(const char *arg, vector<string>& Args)
{
   if(arg[0]=='-' && arg[1]=='f') {
      string filename(arg);
      filename.erase(0,2);
      //cout << "Found a file of options: " << filename << endl;
      ifstream infile(filename.c_str());
      if(!infile) {
         cerr << "Error: could not open options file "
            << filename << endl;
      }
      else {
         char c;
         string buffer;
         while(infile >> buffer) {
            if(buffer[0] == '#') {         // skip to end of line
               while(infile.get(c)) { if(c=='\n') break; }
            }
            else if(buffer[0] == '"') {    // read to next "
               buffer.erase(0,1);
               buffer += ' ';
               while(infile.get(c)) {
                  if(c=='"') {
                     PreProcessArgs(buffer.c_str(),Args);
                     break;
                  }
                  else {
                     buffer += c;
                  }
               }
            }
            else PreProcessArgs(buffer.c_str(),Args);
         }
      }
   }
   else if((arg[0]=='-' && arg[1]=='d') || string(arg)==string("--debug")) {
      debug = true;
      //cout << "Found the debug switch" << endl;
   }
   else if((arg[0]=='-' && arg[1]=='v') || string(arg)==string("--verbose")) {
      verbose = true;
      //cout << "Found the verbose switch" << endl;
   }
   else Args.push_back(arg);
}

//------------------------------------------------------------------------------------
int Initialize(void)
{
try {
      // open nav files and read EphemerisStore
   if(!NavDir.empty())
      for(int i=0; i<NavFiles.size(); i++)
         NavFiles[i] = NavDir + "/" + NavFiles[i];
   FillEphemerisStore(NavFiles, SP3EphList, BCEphList);
   if(SP3EphList.size()) {
      if(verbose) SP3EphList.dump(0,oflog);
   }
   else if(verbose) oflog << "SP3 Ephemeris list is empty\n";

   if(BCEphList.size()) {
      BCEphList.SearchNear();
      if(verbose) BCEphList.dump(0,oflog);
   }
   else if(verbose) oflog << "BC Ephemeris list is empty\n";

   if(SP3EphList.size()) pEph = &SP3EphList;
   else if(BCEphList.size()) pEph = &BCEphList;
   else {
      cerr << "TECMaps abort -- no ephemeris\n";
      oflog << "TECMaps abort -- no ephemeris\n";
      return -1;
   }

      // read the sat+rx biases file and build the map
   if(BiasFile.length() > 0) {
      ifstream inf(BiasFile.c_str());
      if(!inf) {
         cerr << "Error: could not open biases file " << BiasFile << endl;
         oflog << "Error: could not open biases file " << BiasFile << endl;
         return -1;
      }
      else {
         int nbiases,n=0;
         double bias;
         RinexPrn sat;
         string line,station;
         vector<string> words;
         while(!inf.eof() && inf.good()) {
            {        // parse line into words
               istringstream iss(line);
               string wd;
               words.clear();
               while(iss >> wd) { words.push_back(wd); }
            }
            while(1) {
               if(words.size() != 5) break;
               if(debug) {
                  oflog << "Biases file:";
                  for(int i=0; i<words.size(); i++) oflog << " " << words[i];
                  oflog << endl;
               }
               if(words[0] == string("IonoBias,")) break;
               if(words[1] == string("Number")) {
                  nbiases = StringUtils::asInt(words[0]);
                  break;
               }
               station = words[1];
               sat = StringUtils::asData<RinexPrn>(words[2]);
               bias = StringUtils::asDouble(words[3]);
               BiasMap[station][sat] = bias;
               n++;
               break;
            }
            getline(inf,line);
         }
         inf.close();
         if(verbose) {
            oflog << "Read sat+rx biases file " << BiasFile << ":" << endl;
            oflog << " Expected " << nbiases << " biases, read " << n << "." << endl;
            oflog << " Here is the biases map:" << endl;
            map<string,map<RinexPrn,double> >::const_iterator it;
            map<RinexPrn,double>::const_iterator jt;
            for(it=BiasMap.begin(); it!=BiasMap.end(); it++) {
               for(jt=it->second.begin(); jt!=it->second.end(); jt++) {
                  oflog << "  " << it->first
                     << " " << jt->first
                     << " " << setw(12) << setprecision(6) << jt->second << endl;
               }
            }
         }
      }
   }

      // create the obs types for later use
   ELot = RinexObsHeader::convertObsType("EL");
   AZot = RinexObsHeader::convertObsType("AZ");
   LAot = RinexObsHeader::convertObsType("LA"); // TEMP
   LOot = RinexObsHeader::convertObsType("LO"); // TEMP
   SRot = RinexObsHeader::convertObsType("SR");
   VRot = RinexObsHeader::convertObsType("VR");
   TPot = RinexObsHeader::convertObsType("TP");

      // initialize the map(s)
      // set parameters
   vtecmap.Decorrelation = DecorrelError;
   vtecmap.MinElevation = ElevThresh;
   vtecmap.IonoHeight = IonoHt*1000;
   vtecmap.gridtype = typegrid;
   vtecmap.fittype = typefit;
   vtecmap.BeginLat = BeginLat;
   vtecmap.DeltaLat = DeltaLat;
   vtecmap.NumLat = NumLat;
   vtecmap.BeginLon = BeginLon;
   vtecmap.DeltaLon = DeltaLon;
   vtecmap.NumLon = NumLon;
   if(doMUFmap) mufmap.CopyInputData(vtecmap);
   if(doF0F2map) f0f2map.CopyInputData(vtecmap);

   return 0;
}
catch(gpstk::Exception& e) {
   cerr << "TECMaps:Initialize caught an exception\n";
   GPSTK_RETHROW(e);
}
}

//------------------------------------------------------------------------------------
// Return 0 ok,
//       -1 could not allocate array of RinexObsStream
//       -2 could not open a file,
//       -3 FFStream exception,
//       -4 gpstk exception,
int ProcessStations(void)
{
try {
   int iret,nfile;

   if(verbose) oflog << "\nProcess " << Stations.size()
      << " input files / Stations:\n";

      // create vector of RinexObsStream for use by the files
   instream = new RinexObsStream[Stations.size()];
   if(!instream) {
      oflog << " ERROR: Array of " << Stations.size()
         << " RinexObsStream's could not be allocated\n";
      return -1;
   }

      // loop over input file names
      // open and read header, define entry in Stations array
   for(nfile=0; nfile<Stations.size(); nfile++) {
      instream[nfile].open(Stations[nfile].filename.c_str(),ios_base::in);
      if(!instream[nfile]) {
         oflog << " Rinex file " << Stations[nfile].filename
            << " could not be opened -- abort.\n";
         Stations[nfile].nfile = -1;
         Stations[nfile].nread = -1;
         Stations[nfile].getNext = false;
         return -2;
      }
      oflog << "\nInput file #" << nfile+1 << ": "
         << Stations[nfile].filename << endl;

      instream[nfile].exceptions(ios::failbit);
      Stations[nfile].nfile = nfile;
      Stations[nfile].getNext = true;

      iret = ProcessHeader(Stations[nfile]);
      if(iret != 0) return iret;

   }  // end loop over file names -> stations
   oflog << "\nEnd of loop over stations to read headers.\n\n";

   return 0;
}
catch(gpstk::Exception& e) {
   cerr << "TECMaps:ProcessStations caught an exception\n";
   GPSTK_RETHROW(e);
}
catch(...) {
   cerr << "TECMaps:ProcessStations caught an unknown exception\n";
   return -1;
}
}

//------------------------------------------------------------------------------------
void ProcessObsAndComputeMap(void)
{
try {
   int iret,nfile,ngood,nepochs=0;
   vector<ObsData> AllObs; // ObsData vector, passed into ProcessObs and Compute

      // loop over all epochs in all files
   do {
         // read the data for the next (earliest in future) observation epoch
      EarliestTime = DayTime(DayTime::END_OF_TIME);
      for(nfile=0; nfile<Stations.size(); nfile++) {
         iret = ReadNextObs(Stations[nfile]);
         if(iret < 0) {            // set file 'inactive'
            Stations[nfile].nread = -1;
         }
         else {                    // iret is 0 or 1 - file is active
            if(Stations[nfile].robs.time < EarliestTime)
               EarliestTime = Stations[nfile].robs.time;
         }
      }

         // if no more data available, EarliestTime will never get set
      if(EarliestTime == DayTime(DayTime::END_OF_TIME)) break;

         // time limits
      if(EarliestTime < BegTime) {
         oflog << "Before begin time : "
            << EarliestTime.printf("%Y/%m/%d %H:%M:%6.3f=%F/%10.3g\n");
         continue;
      }
      if(EarliestTime > EndTime) {
         oflog << "After end time (quit) : "
            << EarliestTime.printf("%Y/%m/%d %H:%M:%6.3f=%F/%10.3g\n");
         break;
      }

         // process at EarliestTime
      oflog << "Process at time = "
         << EarliestTime.printf("%Y/%m/%d %H:%M:%6.3f=%F/%10.3g\n");
      AllObs.clear();
      for(ngood=0,nfile=0; nfile<Stations.size(); nfile++) {
            // if this data time == EarliestTime, process and set flag to read again
         if(ABS(Stations[nfile].robs.time - EarliestTime) < 1.) {
            iret = ProcessObs(Stations[nfile],AllObs);
            if(iret > 0) ngood += iret;
            Stations[nfile].getNext = true;
         }
         else Stations[nfile].getNext = false;
      }

         // compute map(s) and output
      nepochs++;
      if(ngood > 0) {
         oflog << ngood << " data at epoch "
            << EarliestTime.printf("%Y/%m/%d %H:%M:%6.3f=%F/%10.3g")
            << ", file #" << nepochs << "." << endl;
            // compute the map(s)
         if(doVTECmap) {
            vtecmap.ComputeMap(EarliestTime,AllObs);
            OutputMapToFile(vtecmap,BaseName,EarliestTime,nepochs);
         }
         if(doMUFmap) {
            mufmap.ComputeMap(EarliestTime,AllObs);
            OutputMapToFile(mufmap,BaseName+string(".MUF"),EarliestTime,nepochs);
         }
         if(doF0F2map) {
            f0f2map.ComputeMap(EarliestTime,AllObs);
            OutputMapToFile(f0f2map,BaseName+string(".F0F2"),EarliestTime,nepochs);
         }
      }
      else oflog << "0 data at epoch "
            << EarliestTime.printf("%Y/%m/%d %H:%M:%6.3f=%F/%10.3g")
            << ", file #" << nepochs << "." << endl;

   } while(1);

      // finished...close all files
   for(nfile=0; nfile<Stations.size(); nfile++) instream[nfile].close();

   if(verbose)
      oflog << endl << "Processed " << Stations.size() << " stations\n";
}
catch(gpstk::Exception& e) {
   cerr << "TECMaps:ProcessObsAndComputeMap caught an exception\n";
   GPSTK_RETHROW(e);
}
catch(...) {
   cerr << "TECMaps:ProcessObsAndComputeMap caught an unknown exception\n";
}
}

//------------------------------------------------------------------------------------
// output the grid to a file
void OutputGridToFile(VTECMap& vmap, string filename)
{
   ofstream ofs(filename.c_str());
   if(!ofs) {
      cerr << "Failed to open grid output file " << filename << endl;
      oflog << "Failed to open grid output file " << filename << endl;
   }
   else {
      vmap.OutputGrid(ofs);
      ofs.close();
   }
}

//------------------------------------------------------------------------------------
// output map
void OutputMapToFile(VTECMap& vtmap, string filename, DayTime t, int n)
{
      // make this a function, pass it the name MUF etc, map and time
   ostringstream oss;
   oss << filename << "." << setw(4) << setfill('0') << n;
   string fn = oss.str();
   ofstream ofs(fn.c_str());
   if(!ofs) {
      cerr << "Failed to open map output file " << fn << " at epoch "
         << t.printf("%Y/%m/%d %H:%M:%6.3f=%F/%10.3g") << endl;
      oflog << "Failed to open map output file " << fn << " at epoch "
         << t.printf("%Y/%m/%d %H:%M:%6.3f=%F/%10.3g") << endl;
   }
   else {
      oflog << "Output map at epoch "
         << t.printf("%Y/%m/%d %H:%M:%6.3f=%F/%10.3g")
         << " to file " << fn << endl;
      vtmap.OutputMap(ofs,GnuplotFormat);
      ofs.close();
   }
}

//------------------------------------------------------------------------------------
void AddStation(string& name)
{
try {
   Station s;
   s.filename = name;
   for(int i=1; i<33; i++) {
      RinexPrn p(i,systemGPS);
      s.InitTime[p] = DayTime::BEGINNING_OF_TIME;
   }
   Stations.push_back(s);
}
catch(gpstk::Exception& e) {
   cerr << "TECMaps:AddStation caught an exception\n";
   GPSTK_RETHROW(e);
}
catch(...) {
   cerr << "TECMaps:AddStation caught an unknown exception\n";
}
}

//------------------------------------------------------------------------------------
// Return 0 ok,
//       -3 FFStream exception,
//       -4 gpstk exception,
int ProcessHeader(Station& S)
{
try {
      // input header
   try {
      instream[S.nfile] >> S.header;
   }
   catch(gpstk::FFStreamError& e) {
      cerr << "Caught an FFStreamError while reading header for file "
         << S.filename << ":\n" << e.getText(0) << endl;
      oflog << "Caught an FFStreamError while reading header for file "
         << S.filename << ":\n" << e.getText(0) << endl;
      return -3;
   }
   catch(gpstk::Exception& e) {
      cerr << "Caught a gpstk exception while reading header for file "
         << S.filename << ":\n" << e.getText(0) << endl;
      oflog << "Caught a gpstk exception while reading header for file "
         << S.filename << ":\n" << e.getText(0) << endl;
      return -4;
   }
   S.nread = 0;

      // convert to geocentric LLH
   S.xyz.setECEF(S.header.antennaPosition);
   S.llr = S.xyz;
   S.llr.transformTo(Position::Geocentric);

      // save station info
   //StationName = head.markerName;
   //TotalSpan = head.lastObs.MJD()-head.firstObs.MJD();

      // dump header information
   if(verbose) {
      int i;
      oflog << "File name: " << S.filename << "  ";
      oflog << "Marker name: " << S.header.markerName << "\n";
      oflog << "Antenna position:    " << setprecision(3) << S.header.antennaPosition
         << endl;
      oflog << "ECEF Position:       " << S.xyz << endl;
      oflog << "Geocentric Position: " << S.llr << endl;
      oflog << "Observation types (" << S.header.obsTypeList.size() << ") :";
      for(i=0; i<S.header.obsTypeList.size(); i++)
         oflog << " " << RinexObsHeader::convertObsType(S.header.obsTypeList[i]);
      oflog << endl;
      oflog << "Time of first obs "
         << S.header.firstObs.printf("%04Y/%02m/%02d %02H:%02M:%010.7f")
         << " " << (S.header.firstSystem==systemGlonass?"GLO":"GPS") << endl;
      oflog << "Time of  last obs "
         << S.header.lastObs.printf("%04Y/%02m/%02d %02H:%02M:%010.7f")
         << " " << (S.header.lastSystem==systemGlonass?"GLO":"GPS") << endl;
   }

   return 0;
}
catch(gpstk::Exception& e) {
   cerr << "TECMaps:ProcessHeader caught an exception\n";
   GPSTK_RETHROW(e);
}
catch(...) {
   cerr << "TECMaps:ProcessHeader caught an unknown exception\n";
   return -1;
}
}

//------------------------------------------------------------------------------------
// return
//        0 ok
//        1 ok but no data read
//       -1 EOF or non-existant
//       -3 FFStream exception,
//       -4 gpstk exception,
//       -6 read error
int ReadNextObs(Station& S)
{
try {
   if(S.nread == -1) return -1;
   if(!S.getNext) return 1;

   do {
      try {
         instream[S.nfile] >> S.robs;
      }
      catch(gpstk::FFStreamError& e) {
         cerr << "Caught an FFStreamError while reading obs for file "
            << S.filename << ":\n" << e.getText(0) << endl;
         oflog << "Caught an FFStreamError while reading obs for file "
            << S.filename << ":\n" << e.getText(0) << endl;
         return -3;
      }
      catch(gpstk::Exception& e) {
         cerr << "Caught a gpstk exception while reading obs for file "
            << S.filename << ":\n" << e.getText(0) << endl;
         oflog << "Caught a gpstk exception while reading obs for file "
            << S.filename << ":\n" << e.getText(0) << endl;
         return -4;
      }
      if(instream[S.nfile].eof()) {
         oflog << "End of file: " << S.filename << endl;
         return -1;
      }
      if(!instream[S.nfile].good()) {
         oflog << "Read error on file: " << S.filename << endl;
         return -6;
      }

   } while(S.robs.epochFlag != 0 && S.robs.epochFlag != 1);

   S.nread++;
   return 0;
}
catch(gpstk::Exception& e) {
   cerr << "TECMaps:ReadNextObs caught an exception\n";
   GPSTK_RETHROW(e);
}
catch(...) {
   cerr << "TECMaps:ReadNextObs caught an unknown exception\n";
   return -1;
}
}

//------------------------------------------------------------------------------------
// return 0 if data is good and was accepted
int ProcessObs(Station& S, vector<ObsData>& obsvec)
{
try {
   int i,k,n;
   double EL,AZ,LA,LO,SR,VR,TP,bias,obliq;
   double la,lo;     // TEMP
   RinexPrn sat;
   RinexObsData::RinexPrnMap::const_iterator it;
   RinexObsData::RinexObsTypeMap::const_iterator jt;
   Position IPP;

   //S.robs.dump(oflog);
      // loop over sat=it->first, ObsTypeMap=it->second
   for(n=0,it=S.robs.obs.begin(); it!=S.robs.obs.end(); ++it) {
      ObsData od;
      sat = it->first;
      if(sat.system != systemGPS) continue; // ignore non-GPS satellites
      k = -1;
      for(i=0; i<ExSV.size(); i++) {   // Is this satellite excluded ?
         if(ExSV[i] == sat ||                                 // sat is excluded
           (ExSV[i].prn==-1 && ExSV[i].system==sat.system) ) {// system excluded
            k=i;
            break;
         }
      }
      if(k != -1) continue;
   
         // save first time
      if(S.InitTime[sat] == DayTime::BEGINNING_OF_TIME) {
         S.InitTime[sat] = S.robs.time;
      }
      
         // process this sat
      if((jt=it->second.find(ELot)) != it->second.end())
         EL = jt->second.data;
      else continue;
      if(EL < vtecmap.MinElevation) continue;   // here or inside class?
  
      if((jt=it->second.find(AZot)) != it->second.end())
         AZ = jt->second.data;
      else continue;
  
      //TEMP
      if((jt=it->second.find(LAot)) != it->second.end())
         la = jt->second.data;
      else lo = -999.0;
 
      //TEMP
      if((jt=it->second.find(LOot)) != it->second.end())
         lo = jt->second.data;
      else lo = -999.0;

      if((jt=it->second.find(SRot)) != it->second.end())
         SR = jt->second.data;
      else SR = -1.0;

      if((jt=it->second.find(VRot)) != it->second.end())
         VR = jt->second.data;
      else VR = -1.0;

      if(SR == -1.0 && VR == -1.0) continue;

      if((jt=it->second.find(TPot)) != it->second.end())
         TP = jt->second.data;
      else TP = -1.0;

         // compute the pierce point
      if(la == -999.0 || lo == -999.0) {
         IPP = S.llr.getIonosphericPiercePoint(EL,AZ,IonoHt*1000);
         LA = IPP.geocentricLatitude();
         LO = IPP.longitude();
      }
      else { LA=la; LO=lo; }

      od.elevation = EL;
      od.azimuth = AZ;
      od.latitude = LA;
      od.longitude = LO;

      if(TP != -1.0) od.AcqTime = TP;
      else od.AcqTime = S.robs.time - S.InitTime[sat];

      if(od.AcqTime < MinAcqTime) continue;

         // get the bias
      map<string,map<RinexPrn,double> >::const_iterator jt;
      jt = BiasMap.find(S.header.markerName);
         // skip sat+rx for which there are no biases
      if(jt == BiasMap.end()) continue;
      map<RinexPrn,double>::const_iterator kt;
      kt = jt->second.find(sat);
      if(kt == jt->second.end()) continue;
      bias = kt->second;
      if(debug) oflog << "Apply bias for station " << S.header.markerName
         << " and sat " << sat << " = " << fixed << setw(12) << setprecision(6)
         << bias << endl;

         // compute the obliquity
      obliq = WGS84.a()*cos(EL*DEG_TO_RAD)/(WGS84.a()+IonoHt*1000);
      obliq = SQRT(1.0-obliq*obliq);

      if(VR != -1.0) {
         od.VTEC = VR - bias*obliq;
      }
      else {
         od.VTEC = (SR - bias)*obliq;
      }

         // compute the error and save results
      od.VTECerror = vtecmap.VTECError(od.AcqTime, EL, od.VTEC);
      n++;
      obsvec.push_back(od);

         // write out
      /* */
      oflog <<        setw(4) << S.robs.time.GPSfullweek();
      oflog << " " << setw(8) << setprecision(1) << S.robs.time.GPSsow();
      oflog << " " << setw(2) << n;
      oflog << " " << setw(9) << setprecision(5) << LA; // latitude
      oflog << " " << setw(10) << setprecision(5)<< LO; // longitude
      oflog << " " << setw(4) << setprecision(2) << obliq; // obliquity
      oflog << " " << setw(8) << setprecision(3) << od.VTEC; // vertical TEC
      oflog << " " << setw(8) << setprecision(3) << od.AcqTime; // acquisition time
      oflog << " " << setw(2) << sat.prn; // PRN
      oflog << " " << setw(3) << S.nfile+1; // file number
      oflog << endl;
      /* */

   }  // end for loop over sats

   if(n>0) return n;
   else return -1;
}
catch(gpstk::Exception& e) {
   cerr << "TECMaps:ProcessObs caught an exception\n";
   GPSTK_RETHROW(e);
}
catch(...) {
   cerr << "TECMaps:ProcessObs caught an unknown exception\n";
   return -1;
}
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
