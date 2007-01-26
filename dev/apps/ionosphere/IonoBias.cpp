#pragma ident "$Id$"

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

/**
 * @file IonoBias.cpp
 * Program IonoBias will estimate satellite and receiver biases and compute
 *   a simple ionospheric model using least squares and slant TEC values
 *   from multiple stations.
 */

//------------------------------------------------------------------------------------
#include "StringUtils.hpp"
#include "DayTime.hpp"
#include "RinexSatID.hpp"
#include "CommandOption.hpp"
#include "CommandOptionWithTimeArg.hpp"
#include "CommandOptionParser.hpp"

#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"

#include "Vector.hpp"
#include "Matrix.hpp"

#include "Position.hpp"
#include "WGS84Geoid.hpp"
#include "icd_200_constants.hpp"     // for TWO_PI
#include "geometry.hpp"              // for DEG_TO_RAD and RAD_TO_DEG

#include "RinexUtilities.hpp"

#include <iostream>
#include <time.h>
#include <string>
#include <vector>
#include <utility>      // for pair

using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
// Max PRN 
const int MAXPRN=32;
// data input from command line
bool verbose,debug;
   // log file
string LogFile;
ofstream oflog;
string Title;
   // output file
string ATFileName,BiasFileName;
ofstream fout;
ios::pos_type current_header_pos;
   // input path
string InputPath;
vector<string> Filenames;
   // excluded satellites
vector<RinexSatID> ExSV;
   // ephemeris
string NavDir;
vector<string> NavFiles;
EphemerisStore *pEph;
   // obs types needed
RinexObsHeader::RinexObsType ELot,LAot,LOot,SRot,SSot;
   // geoid
WGS84Geoid WGS84;
   // Start and stop times
DayTime BegTime,EndTime;

   // processing
int MinPoints;
double MinTimeSpan;  // TD not implemented
double MinElevation;
double MinLatitude,MaxLatitude;
double MinLongitude,MaxLongitude;
string TimeSector;
double TermOffset;
double IonoHt;
   // 
double sunrise,sunset;     // times in hours of day
double begintime,endtime;  // "
   // normalizations
double MJDNorm,LonNorm;
   // data that goes into output file headers
long NgoodStations;
vector<vector<bool> > EstimationFlag;
vector<bool> BoolVec;
   // data per station that goes into AT output file
int nfile;     // current file number (0..Filenames.size()-1)
long NgoodPoints;
double TotalSpan;       // time in days covered by the file
string StationName;
Position StationPosition;    // station position in geographic lat,lon,radius
   // least squares
bool ComputeSatBiases,DoEstimation,SkipPreproc;
string Model("linear");
int NIonoParam,NBiasParam,NTotalParam;
Vector<double> Sol,InfData;
Matrix<double> Cov;
int ndata;
double MaxLat,MinLat,MaxCRLon,MinCRLon,PM[10];
vector<pair<string,int> > ComponentIDs;

//------------------------------------------------------------------------------------
// prototypes
void ConfigureAndDefaults(void) throw(Exception);
int GetCommandLine(int argc, char **argv) throw(Exception);
void PreProcessArgs(const char *arg, vector<string>& Args) throw(Exception);
int Initialize(void) throw(Exception);
int Process(void) throw(Exception);
int ProcessHeader(RinexObsStream& ins, string& filename, RinexObsHeader& head)
   throw(Exception);
void TimeLimits(Position llr, int doy, string& sector, double& begin, double& end)
   throw(Exception);
void SolarPosition(int doy, double hod, double& lat, double& lon) throw(Exception);
void Sunrise(double lat, double lon, double ht, int doy, double& rise, double& set)
   throw(Exception);
int ProcessObs(RinexObsStream& ins, string& filename, RinexObsHeader& head)
   throw(Exception);
void WriteATHeader(void) throw(Exception);
void WriteStationHeader(int npts, string sta_name, Position llr) throw(Exception);
void ParseLine(string& str, vector<string>& wds) throw(Exception);
int ReadATandCompute(void) throw(Exception);
double obliquity(double elevation) throw(Exception);
//void PartialsMatrix(Matrix<double>& P,int index,double lat,double lon,double obq);

//------------------------------------------------------------------------------------
// utility routines
//------------------------------------------------------------------------------------
// find the index of first occurance of item t (of type T) in vector<T> v;
// i.e. v[index]=t  Return -1 if t is not found.
template<class T> int index(const std::vector<T> v, const T& t) 
{
   for(int i=v.size()-1; i>=0; i--) {
      if(v[i] == t) return i;
   }
   return -1;
}

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   int iret;
   clock_t totaltime=clock(); // timer
   DayTime CurrEpoch;

   BegTime = DayTime::BEGINNING_OF_TIME;
   EndTime = DayTime::END_OF_TIME;

      // Title description and run time
   CurrEpoch.setLocalTime();
   Title = "IonoBias, built on the GPSTK ToolKit, Ver 1.0 6/25/04, Run ";
   Title += CurrEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S\n");
   cout << Title;

      // set configuration and default values
   ConfigureAndDefaults();

      // define extended types
   iret = RegisterARLUTExtendedTypes();
   if(iret) goto quit;
   iret = RegisterExtendedRinexObsType("SS","Slant TEC (Phase smoothed)","TECU",0x1E);
   if(iret) goto quit;

      // get command line arguments
   iret = GetCommandLine(argc, argv);
   if(iret) goto quit;

   if(!SkipPreproc) {
         // initialize
      iret = Initialize();
      if(iret) goto quit;

         // process the data
      iret = Process();

         // write the revised header
      WriteATHeader();
      fout.close();
   }

   if(DoEstimation) {
      // read the AT file and compute biases and model
      iret = ReadATandCompute();
   }

quit:
      // compute run time
   totaltime = clock()-totaltime;
   cout << "IonoBias timing: " << fixed << setprecision(3)
      << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.\n";
   oflog << "\nIonoBias timing: " << fixed << setprecision(3)
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
void ConfigureAndDefaults(void) throw(Exception)
{
try {
   verbose = false;
   debug = false;
   LogFile = string("IonoBias.log");
   BiasFileName = string("");          // no output

   MinPoints = 0;
   MinTimeSpan = 0.0;      // minutes
   MinElevation = 0.0;
   MinLatitude = 0.0;
   MaxLatitude = 90.0;
   MinLongitude = 0.0;
   MaxLongitude = 360.0;
   TimeSector = string("night");
   TermOffset = 0.0;       // min
   IonoHt = 350.0;         // km

   DoEstimation=true;      // if false, quit after writing the AT file
   SkipPreproc =false;     // if true, assume AT file exists and don't generate it
   ComputeSatBiases=true;  // if true, compute Sat+Rx biases, else Rx biases only
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Define, parse and evaluate command line
int GetCommandLine(int argc, char **argv) throw(Exception)
{
try {
   bool help=false;
   int i,j;
   RinexSatID sat;
   sat.setfill('0');

      // required options

   RequiredOption dashin(CommandOption::hasArgument, CommandOption::stdType,
      0,"input"," --input <file>       Input Rinex obs file name(s)");
   //dashin.setMaxCount(1);

      // optional options

   // this only so it will show up in help page...
   CommandOption dashf(CommandOption::hasArgument, CommandOption::stdType,
      'f',""," -f<file>             file containing more options");

   CommandOption dashp(CommandOption::hasArgument, CommandOption::stdType,
      0,"inputdir"," --inputdir <path>    Path for input file(s)");
   dashp.setMaxCount(1);

   // ephemeris
   CommandOption dashnd(CommandOption::hasArgument, CommandOption::stdType, 0,
      "navdir"," Ephemeris input:\n --navdir <dir>       Path of navigation file(s)");
   dashnd.setMaxCount(1);

   CommandOption dashn(CommandOption::hasArgument, CommandOption::stdType,
      0,"nav"," --nav <file>         Navigation (Rinex Nav OR SP3) file(s)");

   CommandOption dashat(CommandOption::hasArgument, CommandOption::stdType,
      0,"datafile",
      " Output:\n --datafile <file>    Data (AT) file name, for output and/or input");
   dashat.setMaxCount(1);
   
   CommandOption dashl(CommandOption::hasArgument, CommandOption::stdType,
      0,"log"," --log <file>         Output log file name");
   dashl.setMaxCount(1);

   CommandOption dashout(CommandOption::hasArgument, CommandOption::stdType,
      0,"biasout"," --biasout <file>     Output satellite+receiver biases file name");
   dashout.setMaxCount(1);

   // time
   CommandOptionWithTimeArg dasheb(0,"BeginTime","%Y,%m,%d,%H,%M,%f",
      " Time limits:\n --BeginTime <arg>    Start time, arg is of the form "
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

   CommandOptionNoArg dashde(0, "NoEstimation"," Processing:\n"
      " --NoEstimation       Do NOT perform the estimation (default=false).");

   CommandOptionNoArg dashwo(0, "NoPreprocess",
      " --NoPreprocess       Skip preprocessing; read (existing) AT file "
      "(false).");

   CommandOptionNoArg dashsb(0, "NoSatBiases",
      " --NoSatBiases        Compute Receiver biases ONLY (not Rx+Sat biases) "
      "(false).");

   CommandOption dashmod(CommandOption::hasArgument, CommandOption::stdType,
      0,"Model"," --Model <type>       Ionospheric model: type is linear, "
      "quadratic or cubic");

   CommandOption dashMinPoints(CommandOption::hasArgument, CommandOption::stdType,
      0,"MinPoints",
     " --MinPoints <n>      Minimum points per satellite required");
   dashMinPoints.setMaxCount(1);

   CommandOption dashMinTimeSpan(CommandOption::hasArgument, CommandOption::stdType,
      0,"MinTimeSpan",
     " --MinTimeSpan <n>    Minimum timespan per satellite required (minutes)");
   dashMinTimeSpan.setMaxCount(1);

   CommandOption dashMinElevation(CommandOption::hasArgument, CommandOption::stdType,
      0,"MinElevation", " --MinElevation <n>   Minimum elevation angle (degrees)");
   dashMinElevation.setMaxCount(1);

   CommandOption dashMinLatitude(CommandOption::hasArgument, CommandOption::stdType,
      0,"MinLatitude", " --MinLatitude <n>    Minimum latitude (degrees)");
   dashMinLatitude.setMaxCount(1);

   CommandOption dashMaxLatitude(CommandOption::hasArgument, CommandOption::stdType,
      0,"MaxLatitude", " --MaxLatitude <n>    Maximum latitude (degrees)");
   dashMaxLatitude.setMaxCount(1);

   CommandOption dashMinLongitude(CommandOption::hasArgument, CommandOption::stdType,
      0,"MinLongitude", " --MinLongitude <n>   Minimum longitude (degrees)");
   dashMinLongitude.setMaxCount(1);

   CommandOption dashMaxLongitude(CommandOption::hasArgument, CommandOption::stdType,
      0,"MaxLongitude", " --MaxLongitude <n>   Maximum longitude (degrees)");
   dashMaxLongitude.setMaxCount(1);

   CommandOption dashTimeSector(CommandOption::hasArgument, CommandOption::stdType,
      0,"TimeSector", " --TimeSector <n>     Time sector (day | night | both)");
   dashTimeSector.setMaxCount(1);

   CommandOption dashTermOffset(CommandOption::hasArgument, CommandOption::stdType,
      0,"TerminOffset", " --TerminOffset <n>   Terminator offset (minutes)");
   dashTermOffset.setMaxCount(1);

   CommandOption dashIonoHt(CommandOption::hasArgument, CommandOption::stdType,
      0,"IonoHeight", " --IonoHeight <n>     Ionosphere height (km)");
   dashIonoHt.setMaxCount(1);

   CommandOption dashXsat(CommandOption::hasArgument, CommandOption::stdType,
      '0', "XSat", " Other options:\n --XSat <sat>         Exclude this satellite "
      "(<sat> may be <system> only)");
   
   // ... other options
   CommandOptionNoArg dashv('v', "verbose",
         " Help:\n [-v|--verbose]       print extended output info.");

   CommandOptionNoArg dashd('d', "debug",
         " [-d|--debug]         print extended output info.");

   CommandOptionNoArg dashh('h', "help",
         " [-h|--help]          print syntax and quit.");

   // ... rest of the command line
   CommandOptionRest Rest("");

   CommandOptionParser Par(
      " Prgm IonoBias will open and read several preprocessed Rinex obs files\n"
      " (containing obs types EL,LA,LO,SR or SS) and use the data to estimate\n"
      " satellite and receiver biases and to compute a simple ionospheric model\n"
      " using least squares and the slant TEC values.\n"
      " Input is on the command line, or of the same format in a file (-f<file>).\n");

   // allow user to put all options in a file
   // could also scan for debug here
   vector<string> Args;
   for(j=1; j<argc; j++) PreProcessArgs(argv[j],Args);

   if(Args.size()==0) Args.push_back(string("-h"));

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

   if(debug) {
      cout << "List passed to parse\n";
      for(i=0; i<argc; i++) cout << i << " " << CArgs[i] << endl;
   }
   Par.parseOptions(argc, CArgs);
   delete[] CArgs;

   if(dashh.getCount() > 0) { help = true; }

   if(Par.hasErrors()) {
      cerr << "\nErrors found in command line input:\n";
      Par.dumpErrors(cerr);
      cerr << "...end of Errors\n\n";
      help = true;
   }

   if(help) {
      Par.displayUsage(cout,false);
      cout << endl;
   }
   
      // get values found on command line
   vector<string> values;

      // f never appears because we intercept it above
   //if(dashf.getCount()) { cout << "Option f "; dashf.dumpValue(cout); }
 
      // open the log file first
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
   oflog << Title;
   cout << "IonoBias output directed to log file " << LogFile << endl;
   if(help) {
      Par.displayUsage(oflog,false);
      oflog << endl;
      if (Par.hasErrors()) {
         oflog << "\nErrors found in command line input:\n";
         Par.dumpErrors(oflog);
         oflog << "...end of Errors\n\n";
         return -1;
      }
   }

      // input path; do path before input file names
   if(dashp.getCount()) {
      values = dashp.getValue();
      if(help) cout << "Input path name: " << values[0] << endl;
      InputPath = values[0];
   }
   else InputPath = string("");

      // input file names
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
            Filenames.push_back(fname);
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
                     Filenames.push_back(fname);
                     if(debug) oflog << "   " << fname << endl;
                  }
               }  // end loop over lines in the file
               infile.close();
            }  // end opened file
         }  // end if value is a file containing file names
      }  // end loop over values on command line
   }  // end dashin

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

   if(dashat.getCount()) {
      values = dashat.getValue();
      if(help) cout << "Input name of AT file: " << values[0] << endl;
      ATFileName = values[0];
   }
   if(dashout.getCount()) {
      values = dashout.getValue();
      if(help) cout << "Output biases file name: " << values[0] << endl;
      BiasFileName = values[0];
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
   if(dashde.getCount()) {
      DoEstimation = false;
      if(help) cout << "Turn OFF estimation of biases and model" << endl;
   }
   if(dashwo.getCount()) {
      SkipPreproc = true;
      if(help) cout << "Read the existing AT file (skip preprocessing)" << endl;
   }
   if(dashsb.getCount()) {
      ComputeSatBiases = false;
      if(help) cout << "Compute Rx biases only, not Rx+Sat biases" << endl;
   }
   if(dashmod.getCount()) {
      values = dashmod.getValue();
      Model = values[0];
      if(help) cout << "Model input is " << Model << endl;
      if(Model != string("linear") &&
         Model != string("quadratic") &&
         Model != string("cubic")) {
         cout << "Warning: --Model is invalid -- linear will be used\n";
      }
   }
   if(dashMinPoints.getCount()) {
      values = dashMinPoints.getValue();
      MinPoints = asInt(values[0]);
      if(help) cout << "Minimum points per satellite = " << MinPoints << endl;
   }
   if(dashMinTimeSpan.getCount()) {
      values = dashMinTimeSpan.getValue();
      MinTimeSpan = asDouble(values[0]);
      if(help) cout << "Minimum time span = " << MinTimeSpan << " minutes" << endl;
   }
   if(dashMinElevation.getCount()) {
      values = dashMinElevation.getValue();
      MinElevation = asDouble(values[0]);
      //if(MinElevation <= 0.0 || MinElevation >= 90.0) {
      //}
      if(help) cout << "Minimum elevation = " << MinElevation << "degrees " << endl;
   }
   if(dashMinLatitude.getCount()) {
      values = dashMinLatitude.getValue();
      MinLatitude = asDouble(values[0]);
      //if(MinLatitude <= -90.0 || MinLatitude >= 90.0) {
      //}
      if(help) cout << "Minimum latitude = " << MinLatitude << " degrees" << endl;
   }
   if(dashMaxLatitude.getCount()) {
      values = dashMaxLatitude.getValue();
      MaxLatitude = asDouble(values[0]);
      //if(MaxLatitude <= -90.0 || MaxLatitude >= 90.0) {
      //}
      if(help) cout << "Maximum latitude = " << MaxLatitude << " degrees" << endl;
   }
   if(dashMinLongitude.getCount()) {
      values = dashMinLongitude.getValue();
      MinLongitude = asDouble(values[0]);
      while(MinLongitude < 0.0) MinLongitude+=360.0;
      if(help) cout << "Minimum longitude = " << MinLongitude << " degrees" << endl;
   }
   if(dashMaxLongitude.getCount()) {
      values = dashMaxLongitude.getValue();
      MaxLongitude = asDouble(values[0]);
      while(MaxLongitude < 0.0) MaxLongitude+=360.0;
      if(help) cout << "Maximum longitude = " << MaxLongitude << " degrees" << endl;
   }
   if(dashTimeSector.getCount()) {
      values = dashTimeSector.getValue();
      TimeSector = lowerCase(values[0]);
      if(help) cout << "Time sector = " << TimeSector << endl;
      // TD check that it is valid
   }
   if(dashTermOffset.getCount()) {
      values = dashTermOffset.getValue();
      TermOffset = asDouble(values[0]);
      if(help) cout << "Terminal offset = " << TermOffset << " minutes" << endl;
   }
   if(dashIonoHt.getCount()) {
      values = dashIonoHt.getValue();
      IonoHt = asDouble(values[0]);
      if(help) cout << "Ionosphere height = " << IonoHt << " km" << endl;
   }

   if(dashXsat.getCount()) {
      values = dashXsat.getValue();
      for(i=0; i<values.size(); i++) {
         sat.fromString(values[i]);
         if(help) cout << "Input: exclude satellite " << sat << endl;
         ExSV.push_back(sat);
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
      oflog << "Input configuration for IonoBias:\n";
      if(!InputPath.empty()) oflog << " Path for input files is "
         << InputPath << endl;
      oflog << " Input Rinex obs file names are:\n";
      for(i=0; i<Filenames.size(); i++)
         oflog << "   " << Filenames[i] << endl;
      if(!NavDir.empty()) oflog << " Path for input nav files is "
         << NavDir  << endl;
      if(NavFiles.size() > 0) {
         oflog << " Input Rinex nav file names are:\n";
         for(i=0; i<NavFiles.size(); i++)
            oflog << "   " << NavFiles[i] << endl;
      }
      //oflog << " Input data interval is: " << fixed << setprecision(2)
         //<< DataInterval << endl;
      if(!ATFileName.empty()) oflog << " AT file name is "
         << ATFileName << endl;
      if(BegTime > DayTime::BEGINNING_OF_TIME) oflog << " Begin time is "
         << BegTime.printf("%Y/%m/%d_%H:%M:%6.3f=%F/%10.3g") << endl;
      if(EndTime < DayTime::END_OF_TIME) oflog << " End   time is "
         << EndTime.printf("%Y/%m/%d_%H:%M:%6.3f=%F/%10.3g") << endl;
      oflog << " Processing:\n";
      oflog << "   Use a " << Model << " ionospheric model" << endl;
      oflog << "   Minimum points per satellite = " << MinPoints << endl;
      oflog << "   Minimum time span = " << MinTimeSpan << " minutes" << endl;
      oflog << "   Minimum elevation = " << MinElevation << " degrees" << endl;
      oflog << "   Minimum latitude = " << MinLatitude << " degrees" << endl;
      oflog << "   Maximum latitude = " << MaxLatitude << " degrees" << endl;
      oflog << "   Minimum longitude = " << MinLongitude << " degrees" << endl;
      oflog << "   Maximum longitude = " << MaxLongitude << " degrees" << endl;
      oflog << "   Time sector = " << TimeSector << endl;
      oflog << "   Terminal offset = " << TermOffset << " minutes" << endl;
      oflog << "   Ionosphere height = " << IonoHt << " km" << endl;
      if(ExSV.size() > 0) {
         oflog << "  Exclude these satellites:";
         for(i=0; i<ExSV.size(); i++) oflog << " " << ExSV[i];
         oflog << endl;
      }
      oflog << " Compute " << (ComputeSatBiases ? "Sat+":"") << "Rx biases" << endl;
      if(BiasFileName.length() > 0)
         oflog << " Output biases to file " << BiasFileName << endl;
      else
         oflog << " Do not output biases to a file" << endl;
      oflog << " Do" << (DoEstimation?"":" not do")
         << " the estimation problem" << endl;
      oflog << " " << (SkipPreproc?"Skip":"Do not skip") << " the preprocessing"
         << endl;
      if(help) oflog << " Help (-h) is set\n";
      if(debug) oflog << " Debug (-d) is set\n";
      oflog << "End of input configuration for IonoBias" << endl;
   }

   if(!DoEstimation && SkipPreproc) {
      cout << "ERROR: Estimation is turned off and so is preprocessing!\n";
      cout << "ERROR: Abort: nothing to do.\n";
      return -1;
   }

   if(help) return 1;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Pull out --debug --verbose -f<f> and --file <f> and -l<f> --log <f> options.
void PreProcessArgs(const char *arg, vector<string>& Args) throw(Exception)
{
try {
   static bool found_cfg_file=false;
   static bool found_log_file=false;

   if(found_cfg_file || (arg[0]=='-' && arg[1]=='f')) {
      string filename(arg);
      if(!found_cfg_file) filename.erase(0,2); else found_cfg_file = false;
      if(debug) cout << "Found a file of options: " << filename << endl;
      ifstream infile(filename.c_str());
      if(!infile) {
         cout << "Error: could not open options file " << filename << endl;
         return;
      }

      bool again_cfg_file=false;
      bool again_log_file=false;
      char c;
      string buffer,word;
      while(1) {
         getline(infile,buffer);
         stripTrailing(buffer,'\r');

         // process the buffer before checking eof or bad b/c there can be
         // a line at EOF that has no CRLF...
         while(!buffer.empty()) {
            word = firstWord(buffer);
            if(again_cfg_file) {
               word = "-f" + word;
               again_cfg_file = false;
               PreProcessArgs(word.c_str(),Args);
            }
            else if(again_log_file) {
               word = "-l" + word;
               again_log_file = false;
               PreProcessArgs(word.c_str(),Args);
            }
            else if(word[0] == '#') { // skip to end of line
               buffer.clear();
            }
            else if(word == "--file" || word == "-f")
               again_cfg_file = true;
            else if(word == "--log" || word == "-l")
               again_log_file = true;
            else if(word[0] == '"') {
               word = stripFirstWord(buffer,'"');
               buffer = "dummy " + buffer;            // to be stripped later
               PreProcessArgs(word.c_str(),Args);
            }
            else
               PreProcessArgs(word.c_str(),Args);

            word = stripFirstWord(buffer);      // now remove it from buffer
         }
         if(infile.eof() || !infile.good()) break;
      }
   }
   else if(found_log_file || (arg[0]=='-' && arg[1]=='l')) {
      LogFile = string(arg);
      if(!found_log_file) LogFile.erase(0,2); else found_log_file = false;
   }
   else if(string(arg) == "--log")
      found_log_file = true;
   else if((arg[0]=='-' && arg[1]=='d') || string(arg)==string("--debug"))
      debug = true;
   else if((arg[0]=='-' && arg[1]=='v') || string(arg)==string("--verbose"))
      verbose = true;
   else if(string(arg) == "--file" || string(arg) == "-f")
      found_cfg_file = true;
   else Args.push_back(arg);
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}


//------------------------------------------------------------------------------------
int Initialize(void) throw(Exception)
{
try {
   static SP3EphemerisStore SP3EphList;
   static BCEphemerisStore BCEphList;

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
      cerr << "IonoBias abort -- no ephemeris\n";
      oflog << "IonoBias abort -- no ephemeris\n";
      return -1;
   }

      // create the obs types for later use
   ELot = RinexObsHeader::convertObsType("EL");
   LAot = RinexObsHeader::convertObsType("LA");
   LOot = RinexObsHeader::convertObsType("LO");
   SRot = RinexObsHeader::convertObsType("SR");
   SSot = RinexObsHeader::convertObsType("SS");

      // initialize AT header data
   int i;
   NgoodStations = 0;
   for(i=0; i<=MAXPRN; i++) BoolVec.push_back(false);
   for(i=0; i<Filenames.size(); i++) EstimationFlag.push_back(BoolVec);

      // open output file and write zero-filled header
   fout.open(ATFileName.c_str(),ios_base::out);
   if(!fout) {
      cerr << "IonoBias abort -- failed to open AT file " << ATFileName
         << " for output." << endl;
      oflog << "IonoBias abort -- failed to open AT file " << ATFileName
         << " for output." << endl;
      return -2;
   }
   WriteATHeader();

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Return 0 ok,
//       -2 could not open a file,
//       -3 FFStream exception,
//       -4 gpstk exception,
//       -5 no sunrise
int Process(void) throw(Exception)
{
try {
   int i,iret;
   string fname;
   RinexObsStream instream;
   RinexObsHeader header;

      // loop over input file names
   if(verbose) oflog << "\nProcess " << Filenames.size() << " input files:\n";
   for(ndata=0,nfile=0; nfile<Filenames.size(); nfile++) {
      if(verbose) oflog << endl;
      fname = Filenames[nfile];
      instream.open(fname.c_str(),ios_base::in);
      if(!instream) {
         oflog << " Rinex file " << fname << " could not be opened -- abort.\n";
         return -2;
      }
      instream.exceptions(ios::failbit);
      if(verbose) oflog << "Opened input file #" << nfile+1 << ": " << fname << endl;
      
      iret = ProcessHeader(instream,fname,header);
      if(iret != 0) return iret;

      if(nfile==0) {
         MJDNorm = header.firstObs.MJD();
         LonNorm = StationPosition[1]; //.getLongitude();
      }

      iret = ProcessObs(instream,fname,header);
      if(iret != 0) return iret;

      instream.close();
      instream.clear();

   }  // end loop over file names

   if(verbose) {
      oflog << endl << "Processed " << Filenames.size()
      << " files; " << NgoodStations << " of them had good data.\n";
      oflog << "Total number of data points = " << ndata << endl;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Return 0 ok, -3 FFStream exception, -4 gpstk exception, -5 no sunrise
int ProcessHeader(RinexObsStream& ins, string& filename, RinexObsHeader& head)
   throw(Exception)
{
try {
      // input header
   try {
      ins >> head;
   }
   catch(gpstk::FFStreamError& e) {
      cerr << "Caught an FFStreamError while reading header for file "
         << filename << ":\n" << e.getText(0) << endl;
      oflog << "Caught an FFStreamError while reading header for file "
         << filename << ":\n" << e.getText(0) << endl;
      return -3;
   }
   catch(gpstk::Exception& e) {
      cerr << "Caught a gpstk exception while reading header for file "
         << filename << ":\n" << e.getText(0) << endl;
      oflog << "Caught a gpstk exception while reading header for file "
         << filename << ":\n" << e.getText(0) << endl;
      return -4;
   }

      // convert to LLH
   Position xyz;
   xyz.setECEF(head.antennaPosition);
   StationPosition = xyz;
   StationPosition.transformTo(Position::Geocentric);

      // compute begin and end times
   TimeLimits(StationPosition, head.firstObs.DOY(), TimeSector, begintime, endtime);
   if(begintime == -999 || endtime == -999) return -5;

      // save station info
   StationName = head.markerName;
   TotalSpan = head.lastObs.MJD()-head.firstObs.MJD();

      // dump header information
   if(verbose) {
      int i;
      oflog << "File name: " << filename << endl;
      oflog << "Marker name: " << head.markerName << "\n";
      oflog << "Position (XYZ,m) : " << fixed
         << setprecision(3) << head.antennaPosition << "\n";
      oflog << "Position (LLH  ) : ("
         << setprecision(8) << StationPosition[0] << ", "
         << setprecision(8) << StationPosition[1] << ", "
         << setprecision(4)
         << StationPosition[2] - StationPosition.radiusEarth()
         << ")\n";
      oflog << "Observation types (" << head.obsTypeList.size() << ") :";
      for(i=0; i<head.obsTypeList.size(); i++)
         oflog << " " << RinexObsHeader::convertObsType(head.obsTypeList[i]);
      oflog << endl;
      oflog << "Time of first obs "
         << head.firstObs.printf("%04Y/%02m/%02d %02H:%02M:%010.7f")
         << " " << (head.firstSystem.system==RinexSatID::systemGlonass?"GLO":
                   (head.firstSystem.system==RinexSatID::systemGalileo?"GAL":"GPS"))
         << endl;
      oflog << "Time of  last obs "
         << head.lastObs.printf("%04Y/%02m/%02d %02H:%02M:%010.7f")
         << " " << (head.lastSystem.system==RinexSatID::systemGlonass?"GLO":
                   (head.lastSystem.system==RinexSatID::systemGalileo?"GAL":"GPS"))
         << endl;
      oflog << "DOY = " << head.firstObs.DOY() << endl;
      oflog << "Sunrise = " << setprecision(2) << sunrise;
      oflog << "  Sunset  = " << setprecision(2) << sunset << endl;
      oflog << "Begin time = " << setprecision(2) << begintime;
      oflog << "  End time = " << setprecision(2) << endtime << endl;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Given a position (LLH), day of year, and sector flag (TimeSector),
// compute the begin and end times (hours of the day) of our data window,
// which will = sunrise + TermOffset and sunset - TermOffset.
void TimeLimits(Position llr, int doy, string& sector, double& begin, double& end)
   throw(Exception)
{
try {
   begin = 0;
   end = 24.;

   Sunrise(llr[0], llr[1], IonoHt*1000.0, doy, sunrise, sunset);
   if(sector == string("both")) return;
   if(sector == string("day")) {
      begin = sunrise + TermOffset/60.0;
      end   = sunset  - TermOffset/60.0;
   }
   if(sector == string("night")) {
      begin = sunset  + TermOffset/60.0;
      end   = sunrise - TermOffset/60.0;
   }

   while(begin <  0) begin += 24;
   while(begin >= 24) begin -= 24;
   while(end <  0) end += 24;
   while(end >= 24) end -= 24;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Compute the position (latitude and longitude, in degrees) of the sun
// given the day of year and the hour of the day.
// Adapted from sunpos by D. Coco 12/15/94
//#include "icd_200_constants.hpp     // for TWO_PI
//#include "geometry.hpp"             // for DEG_TO_RAD and RAD_TO_DEG
void SolarPosition(int doy, double hod, double& lat, double& lon) throw(Exception)
{
try {
   lat = sin(23.5*DEG_TO_RAD)*sin(TWO_PI*double(doy-83)/365.25);
   lat = lat / ::sqrt(1.0-lat*lat);
   lat = RAD_TO_DEG*atan(lat);
   lon = 180.0 - hod*15.0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Compute the time of day of sunrise and sunset (set to -999 if they do not exist),
// given a geographic position and day of year.
// Adapted from D. Coco 2/7/96 from equation in Supplement to the Astromonical Almanac
void Sunrise(double lat, double lon, double ht, int doy, double& rise, double& set)
   throw(Exception)
{
try {
   const double DEG_TO_HRS=(24.0/360.0); // should this be sidereal day?
   double sunlat,sunlon,hod;

   // find the position of the sun
   hod = 0;
   SolarPosition(doy, hod, sunlat, sunlon);

   double x = -1 * tan(sunlat*DEG_TO_RAD) * tan(lat*DEG_TO_RAD);
	if(x <= -1.0 || x >= 1.0) {
      rise = set = -999;
      return;
   }
   x = acos(x) * RAD_TO_DEG;

   rise = DEG_TO_HRS * (sunlon - lon - x);
   set  = DEG_TO_HRS * (sunlon - lon + x);

   // adjust for height above sea level
   double dht=0,radius=Position::radiusEarth(lat,WGS84.a(),WGS84.eccSquared());
   dht = 24.0*acos(radius/(radius+ht))/TWO_PI;
   rise -= dht;
   set += dht;

   while(rise <  0) rise += 24;
   while(rise >= 24) rise -= 24;
   while(set <  0) set += 24;
   while(set >= 24) set -= 24;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Return 0 ok, -3 FFStream exception, -4 gpstk exception, -6 stream not good
int ProcessObs(RinexObsStream& ins, string& filename, RinexObsHeader& head)
   throw(Exception)
{
try {
   int i,j,k,npts[MAXPRN+1];
   double EL,LA,LO,SR,hours,cr,ob;
   Position LLI;
   DayTime begin[MAXPRN+1],end[MAXPRN+1];
   RinexObsData robs;
   RinexSatID sat;
   //RinexObsData::RinexObsTypeMap otmap;
   RinexObsData::RinexSatMap::const_iterator it;
   RinexObsData::RinexObsTypeMap::const_iterator jt;

   if(!ins.good()) return -6;

      // initialize for this station
   fout.seekp(0,ios_base::end);           // go to EOF
   current_header_pos = fout.tellp();     // save for later
   NgoodPoints = 0;
   WriteStationHeader(NgoodPoints,StationName,StationPosition);   // dummy

      // loop over epochs
   for(i=1; i<=MAXPRN; i++) npts[i]=0;
   do {
      try {
         ins >> robs;
      }
      catch(gpstk::FFStreamError& e) {
         cerr << "Caught an FFStreamError while reading obs for file "
            << filename << ":\n" << e.getText(0) << endl;
         oflog << "Caught an FFStreamError while reading obs for file "
            << filename << ":\n" << e.getText(0) << endl;
         return -3;
      }
      catch(gpstk::Exception& e) {
         cerr << "Caught a gpstk exception while reading obs for file "
            << filename << ":\n" << e.getText(0) << endl;
         oflog << "Caught a gpstk exception while reading obs for file "
            << filename << ":\n" << e.getText(0) << endl;
         return -4;
      }
      if(ins.eof()) break;
      if(!ins.good()) { return -6; }

      //if(verbose) oflog << " Read file " << filename
      //   << " epoch " << robs.time.printf("%Y/%m/%d_%H:%M:%6.3f=%F/%10.3g") << endl;

      hours = robs.time.secOfDay()/3600.0;          // hours of the day
         // compute co-rotating longitude CL = LO + cr
      cr = (robs.time.MJD()-MJDNorm) * 360.0;
      cr -= LonNorm + TotalSpan * 180.0;

         // loop over sat=it->first, ObsTypeMap=it->second
      for(it=robs.obs.begin(); it != robs.obs.end(); ++it) {
         sat = it->first;
         if(sat.system != SatID::systemGPS) continue; // ignore non-GPS satellites
         if(sat.id <= 0 || sat.id > MAXPRN) continue; // just in case...
         for(i=0,k=-1; i<ExSV.size(); i++) {   // Is this satellite excluded ?
            if( ExSV[i] == sat ||                                 // sat is excluded
               (ExSV[i].id==-1 && ExSV[i].system==sat.system) ) {// system excluded
               k=i;
               break;
            }
         }
         if(k != -1) continue;
   
            // process this sat
         if( (jt=it->second.find(ELot)) != it->second.end()) EL = jt->second.data;
         if(EL < MinElevation) continue;

         if( (jt=it->second.find(LAot)) != it->second.end()) LA = jt->second.data;
         if(LA < MinLatitude || LA > MaxLatitude) continue;

         if( (jt=it->second.find(LOot)) != it->second.end()) LO = jt->second.data;
         while(LO < 0.0) LO+=360.0;
         if(LO < MinLongitude || LO > MaxLongitude) continue;

         if( (jt=it->second.find(SRot)) != it->second.end()) {
            SR = jt->second.data;
            //if(jt->second.ssi == 1) continue;    // reject if ssi==1 ?? TD
         }
         else if( (jt=it->second.find(SSot)) != it->second.end()) {
            SR = jt->second.data;
         }

         LLI = Position(LA,LO,IonoHt*1000.0);     // 3rd entry is actually not used.
         TimeLimits(LLI, robs.time.DOY(), TimeSector, begintime, endtime);
         if(endtime >= begintime) {
            if(hours < begintime || hours > endtime) continue;
         }
         else {
            if(hours < begintime && hours > endtime) continue;
         }

            // compute the obliquity
         ob = obliquity(EL);

            // write out
         fout <<        setw(4)                    << robs.time.GPSfullweek();
         fout << " " << setw(8) << setprecision(1) << robs.time.GPSsow();
         fout << " " << setw(9) << setprecision(5) << LA; // latitude
         fout << " " << setw(10) << setprecision(5) << LO+cr; // co-rotating longitude
         fout << " " << setw(4) << setprecision(2) << ob; // 1/obliquity
         fout << " " << setw(8) << setprecision(3) << SR; // slant TEC
         fout << " " << setw(6) << setprecision(2) << 1;  // sigma ?? TD
         fout << " " << setw(2) << sat.id; // PRN
         fout << " " << setw(3) << nfile+1; // file number
         fout << endl;

         EstimationFlag[nfile][sat.id] = true;
         NgoodPoints++;
         npts[sat.id]++; // Npts for this sat
         if(npts[sat.id]==1) begin[sat.id] = robs.time;
         end[sat.id] = robs.time;

      }  // end for loop over sats


   } while(1);

      // revised header
   WriteStationHeader(NgoodPoints,StationName,StationPosition);

      // revise estimation flags
   if(verbose) oflog << "PRN  Points  Timespan   Begin       End  (hrs)\n";
   for(i=1; i<=MAXPRN; i++) {
      if(npts[i] > 0) {
         if(verbose) oflog << "G" << setfill('0') << setw(2) << i << setfill(' ')
            << setw(6) << npts[i]
            << setw(10) << setprecision(2) << (end[i]-begin[i])/3600.0
            << setw(10) << setprecision(2) << begin[i].secOfDay()/3600.0
            << setw(10) << setprecision(2) << end[i].secOfDay()/3600.0;
         if((end[i]-begin[i] < MinTimeSpan*60.0 || npts[i] < MinPoints)){
            if(verbose) {
               oflog << " reject(";
               if(end[i]-begin[i] < MinTimeSpan*60.0) oflog << " time ";
               if(npts[i] < MinPoints) oflog << " pts ";
               oflog << ")";
            }
            EstimationFlag[nfile][i] = false;
            NgoodPoints -= npts[i];
         }
         if(verbose) oflog << endl;
      }
   }

   if(NgoodPoints > 0) {
      NgoodStations++;
      ndata += NgoodPoints;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void WriteATHeader(void) throw(Exception)
{
try {
   int i,j;
   fout.seekp(0);
   fout << setw(5) << Filenames.size() << " " << setw(5) << NgoodStations
      << " Number (max, good) stations in this file \n";
   for(i=0; i<Filenames.size(); i++) {
      for(j=0; j<MAXPRN+1; j++) fout << (EstimationFlag[i][j] ? '1' : '0');
      fout << "\n";
   }
   fout << fixed;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void WriteStationHeader(int npts, string sta_name, Position llr) throw(Exception)
{
try {
   fout.seekp(current_header_pos);
   fout << "Npt " << setw(5) << npts;
   //fout << in_file_s;
   //fout << sta_id;
   fout << " Sta " << sta_name;
   fout << " LLH " << setw(10) << setprecision(4) << llr[0]; //gllh.getLatitude();
   fout << " " << setw(10) << setprecision(4) << llr[1]; //gllh.getLongitude();
   fout << " " << setw(10) << setprecision(4)
      << llr[2]-Position::radiusEarth(llr[0],WGS84.a(),WGS84.eccSquared());
                //gllh.getAltitude();
   fout << endl;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void ParseLine(string& str, vector<string>& wds) throw(Exception)
{
try {
   istringstream iss(str);
   string wd;
   wds.clear();
   while(iss >> wd) {
      wds.push_back(wd);
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int ReadATandCompute(void) throw(Exception)
{
try {
   ifstream ifs;
   ifs.open(ATFileName.c_str());       // output file is now the input
   if(!ifs) {
      cerr << "Failed to open AT file " << ATFileName << " for input" << endl;
      return -1;
   }
   else if(verbose) oflog << "\nOpened AT file " << ATFileName << " for input\n";

      // read the AT header
   int i,j,k,ii,jj;
   long N,n;
   string line;

   EstimationFlag.clear();
   ifs >> N >> n;
   getline(ifs,line);      // read to eol
   for(i=0; i<N; i++) {
      getline(ifs,line);
      for(j=0; j<line.size(); j++) BoolVec[j] = (line[j] == '1');
      EstimationFlag.push_back(BoolVec);
   }
   
   //if(verbose) {
   //   oflog << "Read AT (" <<N<< "," <<n<< "," << EstimationFlag.size() << ")\n";
   //   for(i=0; i<EstimationFlag.size(); i++) {
   //      for(j=0; j<MAXPRN+1; j++) oflog << (EstimationFlag[i][j] ? '1' : '0');
   //      oflog << "\n";
   //   }
   //}

   if(N != EstimationFlag.size()) { //oops
   }

      // dimension and initialize the LS problem
   if(Model == "cubic") {
      oflog << "Model is cubic\n";
      NIonoParam = 10;
   }
   else if(Model == "quadratic") {
      oflog << "Model is quadratic\n";
      NIonoParam = 6;
   }
   else {   // linear and default
      oflog << "Model is linear\n";
      NIonoParam = 3;
   }

   for(NBiasParam=0,i=0; i<N; i++) {
      if(!ComputeSatBiases) NBiasParam++;
      else for(j=0; j<MAXPRN+1; j++) if(EstimationFlag[i][j]) NBiasParam++;
   }
   NTotalParam = NIonoParam + NBiasParam;

   Sol.resize(NTotalParam,0.0);
   Cov.resize(NTotalParam,NTotalParam,0.0);
   InfData.resize(NTotalParam,0.0);

      // Read the rest of the AT file
   int wn,prn,nfile,in;
   double sow,lat,lon,obq,sr,sig,d2=0;
   string stationID;
   vector<string> words;
   pair<string,int> Comp;

      // loop over stations
   oflog << setw(2) << N << "  Number of stations (N data and filename follow).\n";
   for(ndata=0,i=0; i<N; i++) {
         // read station header
      getline(ifs,line);
      ParseLine(line,words);
      if(words[0] != string("Npt")) { //oops
      }
      n = asInt(words[1]);
      stationID = words[3];

      if(n > 0 && verbose) {
         oflog << setw(3) << i+1 << "  " << stationID << " " << setw(4) << n << " ";
         //for(j=0; j<=MAXPRN; j++) oflog << (EstimationFlag[i][j] ? '1' : '0');
         oflog << Filenames[i];
         oflog << endl;
      }

         // read data
      for(j=0; j<n; j++) {
         getline(ifs,line);
         ParseLine(line,words);
         wn = asInt(words[0]);
         sow = asDouble(words[1]);
         lat = asDouble(words[2]);
         lon = asDouble(words[3]);
         obq = asDouble(words[4]);
         sr = asDouble(words[5]);
         sig = asDouble(words[6]);
         prn = asInt(words[7]);
         nfile = asInt(words[8]);

         // do not include rejected data
         if(!(EstimationFlag[i][prn])) continue;

         // if NOT computing satellite biases, lump all data together into "PRN 0"
         if(!ComputeSatBiases) { prn = 0; }

         // find min and max lat and lon
         if(ndata == 0) {
            MaxLat = MinLat = lat;
            MaxCRLon = MinCRLon = lon;
         }
         else {
            if(fabs(lat) > MaxLat) MaxLat=lat;
            if(fabs(lat) < MinLat) MinLat=lat;
            if(fabs(lon) > MaxCRLon) MaxCRLon=lon;
            if(fabs(lon) < MinCRLon) MinCRLon=lon;
         }
         ndata++;

         // add this data to the LS
         //d2 += sr*sr;
         // find the index in partials matrix for this station-satellite pair
         Comp = make_pair(stationID,prn);
         in = index(ComponentIDs,Comp);
         if(in == -1) {
            in = ComponentIDs.size();
            ComponentIDs.push_back(Comp);
         }

         //PartialsMatrix(Par,in,lat,lon,obq);
         // note that obq is 1/obliquity
         // row of partials matrix has [in] = 1 and if nb=NBiasParam
            PM[0] =       obq; // [nb+0]               (all models)
            PM[1] = lat * obq; // [nb+1]               (all models)
            PM[2] = lon * obq; // [nnb2]               (all models)
         if(NIonoParam > 3) {
            PM[3] = lat * lat * obq; // [nb+3]         (quadratic and cubic)
            PM[4] = lon * lon * obq; // [nb+4]         (quadratic and cubic)
            PM[5] = lat * lon * obq; // [nb+5]         (quadratic and cubic)
         }
         if(NIonoParam > 6) {
            PM[6] = lat * lat * lat * obq; // [nb+6]   (cubic only)
            PM[7] = lon * lon * lon * obq; // [nb+7]   (cubic only)
            PM[8] = lat * lat * lon * obq; // [nb+8]   (cubic only)
            PM[9] = lat * lon * lon * obq; // [nb+9]   (cubic only)
         }

         //LS.Add(Par,Dat,Wgt); do the sequential LS by hand for efficiency
         //
         // Inf += transpose(partials) * partials (weight = 1)
         // InfData += transpose(partials) * data
         //
         Cov(in,in) += 1.0;
         InfData(in) += sr;
         for(ii=0; ii<NIonoParam; ii++) {
            k = NBiasParam + ii;
            InfData(k) += sr * PM[ii];
            Cov(k,in) += PM[ii];
            Cov(in,k) += PM[ii];
            for(jj=0; jj<NIonoParam; jj++) {
               Cov(k,NBiasParam+jj) += PM[ii]*PM[jj];
            }
         }

      }  // end loop over points for this station

   }  // end loop over stations

   ifs.close();

   oflog << setw(9) << setprecision(2) << MinLat << "  Minimum Latitude\n";
   oflog << setw(9) << setprecision(2) << MaxLat << "  Maximum Latitude\n";
   oflog << setw(9) << setprecision(2) << MinCRLon << "  Minimum Co-rot lon\n";
   oflog << setw(9) << setprecision(2) << MaxCRLon << "  Maximum Co-rot lon\n";
   oflog << setw(5) << ndata << " data points used." << endl << endl;

   // solve the LS problem
   // Cov = inverse(information)
   // X = Cov * InfData
   try { Cov = inverse(Cov); }
   catch(Exception& e) {
      oflog << "Least squares failed because the problem is singular\n";
      return -2;
   }

   // Invert Cov via SVD - also expensive - maybe make option, see SVs and conditionN
   //SVD<double> svd;
   //svd(Cov);
   //oflog << "Singular Values range " << svd.S(0)
   //   << " to " << svd.S(NTotalParam-1) << endl;
   //for(i=1; i<NIonoParam; i++) {
   //   if(svd.S(i) < 1.e-14 * svd.S(0)) {
   //      oflog << "Edit SingularValue(" << i << ") = " << svd.S(i) << endl;
   //      svd.S(i) = 0;
   //   }
   //}
   //Vector<double> T(NIonoParam);
   //for(j=0; j<NIonoParam; j++) { // loop over columns
   //   T = 0.0;
   //   T(j) = 1.0;
   //   svd.backSub(T);
   //   for(i=0; i<NTotalParam; i++) Cov(i,j)=T(i);
   //}

   // compute solution
   Sol = Cov * InfData;
   //if(verbose) oflog << "Least squares solved successfully.\n";

   // print solution and sigma - remember lat and lon may be scaled by 1/1000
   bool biasout=false;
   if(BiasFileName.length() > 0) {
      fout.open(BiasFileName.c_str(),ios_base::out);
      if(!fout)
         cerr << "Failed to open output biases file " << BiasFileName << endl;
      else {
         biasout = true;
         fout << Title;
      }
   }
   oflog << setw(2) << NBiasParam << "  Number of SPR biases\n";
   if(biasout) fout << setw(2) << NBiasParam << "  Number of SPR biases\n";
   for(i=0; i<NBiasParam; i++) {
      ostringstream oss;
      oss << setw(3) << i+1                                             // number
         << "  " << ComponentIDs[i].first                               // station id
         << " G" << setw(2) << setfill('0') << ComponentIDs[i].second   // sat G<prn>
         << setfill(' ') << fixed
         << " " << setw(12) << setprecision(6) << Sol(i)                // bias
         << scientific
         << " " << setw(10) << setprecision(3) << ::sqrt(Cov(i,i))        // sigma
         << endl;
      oflog << oss.str();
      if(biasout) fout << oss.str();
   }
   oflog << setw(2) << NTotalParam-NBiasParam << "  Number of ion model parameters\n";
   for(i=NBiasParam; i<NTotalParam; i++) {
      ostringstream oss;
      oss << setw(3) << i+1-NBiasParam << fixed                         // number
         << " " << setw(12) << setprecision(6) << Sol(i)                // solution
         << scientific
         << " " << setw(10) << setprecision(3) << ::sqrt(Cov(i,i))        // sigma
         << endl;
      oflog << oss.str();
      if(biasout) fout << oss.str();
   }

   // compute standard error estimates
   // TBD


   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// elevation in degrees. Return the inverse of the obliquity factor.
double obliquity(double elevation) throw(Exception)
{
try {
   double ob;
   //const double coef[4]={1.02056,0.466332,3.50523,-1.84119};
   //double x2=(1-elevation/90.)*(1-elevation/90.);
   //ob = coef[3];
   //for(int i=2; i>=0; i--) ob = ob*x2 + coef[i];

   ob = WGS84.a()*cos(elevation*DEG_TO_RAD)/(WGS84.a()+IonoHt*1000);
   ob = ::sqrt(1.0-ob*ob);

   return ob;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
