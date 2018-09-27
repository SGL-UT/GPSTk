/// @file Tropdump.cpp Define a TropModel and dump table of values at all elevations

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
#include "singleton.hpp"
#include "Epoch.hpp"
#include "TimeString.hpp"           // printTime
#include "Exception.hpp"
#include "CommandLine.hpp"
#include "Position.hpp"
#include "TropModel.hpp"            // includes Zero
#include "GGHeightTropModel.hpp"
#include "GGTropModel.hpp"
#include "NBTropModel.hpp"
#include "SaasTropModel.hpp"
#include "SimpleTropModel.hpp"
#include "NeillTropModel.hpp"
#include "GlobalTropModel.hpp"
#include "GCATTropModel.hpp"
#include "MOPSTropModel.hpp"

// gpstk-geomatics
#include "logstream.hpp"

// geomatics

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// TODO

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// NB Version below class GlobalData

//------------------------------------------------------------------------------------
// prototypes in this module
/// @return 0 success
int Process(void) throw(Exception);

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
/// Class GlobalData (a singleton) encapsulates global static data as well as
/// command line definition and processing
class GlobalData : public Singleton<GlobalData> {
public:

   /// Default and only constructor, sets defaults.
   GlobalData() throw() { SetDefaults(); }

   /// Create, parse and process command line options and user input
   /// @param argc argv std command line arguments
   /// @return 0 ok, or error code
   int ProcessUserInput(int argc, char **argv) throw(Exception);

   /// Design the command line
   /// @return program description strng
   string BuildCommandLine(void) throw(Exception);

   /// Parsing of the command line args beyond that of CommandLine; start and stop
   /// strings, trop model, sys codes, ObsIDs.
   /// @return 0 ok, 4 invalid input
   int ExtraProcessing(string& errors, string& extras) throw(Exception);

   /// Open log file and assign log level
   /// @return 5 if the output file could not be opened, 0 for success
   int OpenLogFile(void) throw(Exception);

   /// destructor, clean up
   ~GlobalData() {
      if(pTrop) delete pTrop;
      if(oflog.is_open()) oflog.close();
   }

private:
   /// Define defaults for all command line input and global data
   void SetDefaults(void) throw();

public:
   // member data
   CommandLine opts;             ///< command line options and syntax page
   static const string PrgmName; ///< program name
   static const string Version;  ///< version string
   string Title;                 ///< ID line printed to screen/log: name, ver, run

   // command line input ----------------------------------------------------------
   bool inputIsValid;            ///< true finds no problem

   // output file
   string logfile;               ///< name of output log file
   string logpath;               ///< path of output log file

   // input paths
   // input files
   string dummy;                 ///< dummy string used for --file option

   // input strings
   string refPosstr;             ///< string for cmdline ref pos
   string tropName;              ///< string for cmdline trop model
   string tropstr;               ///< string for cmdline trop model weather (T,P,RH)
   double Temp,Press,Humid;      ///< weather for trop model
   int doy;                      ///< only time input to TropModel is DOY
   //string timestr;               ///< time input
   //CommonTime troptime;          ///< time at which to evaluate models

   double elevmin;               ///< lower limit on elevation (degrees)
   double elevmax;               ///< upper limit on elevation (degrees)
   double delev;                 ///< step in elevation (degrees)

   bool trophelp;                ///< if true, print syntax and list of trop models
   // end command line input ------------------------------------------------------

   bool verbose;                 ///< flag handled by CommandLine
   int debug;                    ///< int handled by CommandLine

   ofstream oflog;               ///< output log file stream

   Position refPos;              ///< reference position
   TropModel *pTrop;             ///< tropospheric delay model (cf. *TropModel)

}; // end class GlobalData

//------------------------------------------------------------------------------------
// define static constants
const string GlobalData::PrgmName = string("Tropdump");
const string GlobalData::Version(string("1.0 4/13/17"));

//------------------------------------------------------------------------------------
// Define defaults for all command line input and global data
void GlobalData::SetDefaults(void) throw()
{
   logfile = string();

   // trop model and weather
   tropstr = string("Global,20,1013,50");
   Temp = 20.0;
   Press = 1013.;
   Humid = 50.;
   elevmin = 3.0;
   elevmax = 90.0;
   delev = 1.5;
   // position
   refPosstr = string("-740376.5046,-5457019.3545,3207315.7299"); // ARL:UT
   //timestr = string("2017,103,0.0");
   doy = 103;

   trophelp = false;
   inputIsValid = true;

}  // end SetDefaults()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
   string prgmname;        // for catch only
try {
   // begin counting time - for CPU run time
   clock_t totaltime = clock();
   // begin counting time - for wall clock run time
   Epoch wallclkbeg;
   wallclkbeg.setLocalTime();

   // get (create) the global data object (a singleton);
   // since this is the first instance, this will also set default values
   GlobalData& GD=GlobalData::Instance();
   prgmname = GD.PrgmName;

   // Build title = first line of output
   GD.Title = GD.PrgmName + ", Ver. "
      + GD.Version + printTime(wallclkbeg,", Run %04Y/%02m/%02d at %02H:%02M:%02S");
 
   // TEMP, for debugging CommandLine; OpenLogFile will set Level from GD.debug
   //LOGlevel = ConfigureLOG::Level("DEBUG");

   // process : loop once -----------------------------------------------------
   int iret;
   for(bool go=true; go; go=false)  {

      // process the command line ------------------------------------
      iret = GD.ProcessUserInput(argc,argv);
      if(iret) break; // if(iret) same as if(iret!=0)

      // do it -------------------------------------------------------
      iret = Process();
      if(iret) break;

   }  // end loop once

   // error condition ---------------------------------------------------------
   // return codes: 0 ok
   //              -3 cmd line definition invalid (CommandLine)
   //               1 help
   //               etc see immediately below
   if(iret != 0) {
      if(iret != 1) {                        // print code -- TEMP
         string msg;
         msg = GD.PrgmName + string(" is terminating with code ")
                           + StringUtils::asString(iret);
         LOG(ERROR) << msg;
      }

      if(iret == 1) { ; }                    // help
      else if(iret == -3) { LOG(INFO) << "The cmdline definition is invalid"; }
      else if(iret == 2) { ; }
      else if(iret == 3) { LOG(INFO) << "The user requested input validation."; }
      else if(iret == 4) { LOG(INFO) << "The input is invalid."; }
      else if(iret == 5) { LOG(INFO) << "The log file could not be opened."; }
      else                 // fix this
         LOG(INFO) << "temp - Some other return code... " << iret;
   }

   // compute and print run time ----------------------------------------------
   if(iret != 1) {
      Epoch wallclkend;
      wallclkend.setLocalTime();
      totaltime = clock()-totaltime;
      ostringstream oss;
      oss << GD.PrgmName << " CPU timing: " << fixed << setprecision(3)
         << double(totaltime)/double(CLOCKS_PER_SEC)
         << " seconds. (Wall Clock Timing: " << (wallclkend - wallclkbeg) << " sec)";
      if(pLOGstrm != &cout) LOG(INFO) << oss.str();
      cout << oss.str() << endl;
   }

   if(iret == 0) return 0; else return -1;
}
catch(Exception& e) {
   cerr << prgmname << " caught Exception:\n" << e.what() << endl;
   // don't use LOG here - causes hangup - don't know why
}
catch (...) {
   cerr << "Unknown error in " << prgmname << ".  Abort." << endl;
}
   return -1;
}   // end main()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int GlobalData::ProcessUserInput(int argc, char **argv) throw(Exception)
{
try {
   string PrgmDesc, cmdlineUsage, cmdlineErrors, cmdlineExtras;
   vector<string> cmdlineUnrec;
   ostringstream oss;

   // build the command line
   opts.DefineUsageString(PrgmName + " [options]");
   PrgmDesc = BuildCommandLine();
   LOG(INFO) << Title;

   // let CommandLine parse options; write all errors, etc to the passed strings
   int iret = opts.ProcessCommandLine(argc, argv, PrgmDesc,
                        cmdlineUsage, cmdlineErrors, cmdlineUnrec);

   // handle return values
   if(iret == -2) return iret;      // bad alloc
   if(iret == -3) return iret;      // invalid command line

   // help: print syntax page and quit
   if(opts.hasHelp() || trophelp) {
      LOG(INFO) << cmdlineUsage;
      if(trophelp)
         LOG(INFO) << "\n Valid trop model names (cf. gpstk/core/lib/GNSSCore):\n"
				<< "    GCAT - GPS Code Analysis Tool" << endl
				<< "    GG - Goad and Goodman (1974)" << endl
				<< "    GGHeight - Goad and Goodman with height" << endl
				<< "    Global - the Global (GMF) model" << endl
				<< "    MOPS - RTCA Minimum Operational Performance Standards" << endl
				<< "    NB - New Brunswick" << endl
				<< "    Neill - Neill (Neill 1996)" << endl
				<< "    Saas - Saastamoinen" << endl
				<< "    Simple - a simple Black model" << endl
            << "    Zero - all zeros";

      return 1;
   }

   // extra parsing (perhaps add to cmdlineErrors, cmdlineExtras)
   iret = ExtraProcessing(cmdlineErrors, cmdlineExtras);
   if(iret) return iret;

   // output warning / error messages
   if(cmdlineUnrec.size() > 0) {
      oss << "Warning - unrecognized arguments:";
      for(size_t i=0; i<cmdlineUnrec.size(); i++)
         oss << " >" << cmdlineUnrec[i] << "<";
      LOG(WARNING) << oss.str();
   }

   // fatal errors
   if(!cmdlineErrors.empty()) {
      StringUtils::stripTrailing(cmdlineErrors,'\n');
      StringUtils::replaceAll(cmdlineErrors,"\n","\n ");
      LOG(ERROR) << "Errors found on command line:\n " << cmdlineErrors
         << "\nEnd of command line errors.";
      return -3;
   }

   // success
   // open log file
   iret = OpenLogFile();
   if(iret < 0) return iret;

   // dump configuration summary
   if(verbose) {
      oss.str("");
      oss << "------ Summary of " << PrgmName << " command line configuration ------";
      opts.DumpConfiguration(oss);
      if(!cmdlineExtras.empty()) oss << "\n# Extra Processing\n" << cmdlineExtras;
      oss << "------ End configuration summary ------";
      LOG(VERBOSE) << oss.str() << endl;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end GlobalData::ProcessUserInput()

//------------------------------------------------------------------------------------
string GlobalData::BuildCommandLine(void) throw(Exception)
{
try {
   // build the options list == syntax page
   string prgmdesc;
   prgmdesc =
      string(" Program ") + PrgmName + string(" defines a trop model and prints "
      "a table of values for all elevations.\n"
      "\n Input is on the command line, or of the same format in a file "
      "(see --file below);\n lines in that file which begin with '#' are ignored. "
      "Accepted options are \n shown below, followed by a description, with default "
      "value, if any, in ().");

   // opts.Add(char, opt, arg, repeat?, required?, &target, pre-descript, descript.);
   // required options
   opts.noArgsRequired();

   // NB dummy must belong to GD, not local - don't know why
   /// Q: Where is the file name used to read the config file?
   opts.Add('f', "file", "name", true, false, &dummy, "\n# File I/O:",
            "Name of file containing more options");
   opts.Add(0, "log", "name", false, false, &logfile, "",
            "Name of output log file");
   opts.Add(0, "logpath", "path", false, false, &logpath, "",
            "Path for output log file");

   opts.Add('t', "trop", "m[,T,P,H]", false, false, &tropstr, "\n# Configuration",
            "Tropospheric model and optional weather T(C),P(mb),RH(%)");
   opts.Add(0, "refPos", "X,Y,Z", false, false, &refPosstr, "",
            "Receiver position (ECEF XYZ)");
   opts.Add(0, "doy", "d", false, false, &doy, "",
            "Day of year = time of model");
   //opts.Add(0, "time", "t", false, false, &timestr, "",
   //         "Time of model <t> = MJD|w,sow|y,doy,sod|y,m,d,h,m,s");
   opts.Add(0, "elevmin", "deg", false, false, &elevmin, "\n# Output",
            "Minimum elevation angle (degrees)");
   opts.Add(0, "elevmax", "deg", false, false, &elevmax, "",
            "Maximum elevation angle (degrees)");
   opts.Add(0, "delev", "deg", false, false, &delev, "",
            "Step in degrees of elevation for the output table");
   // help
   opts.Add(0, "trophelp", "", false, false, &trophelp, "\n# Help",
            "Print a list of available trop model names and quit");
   // CommandLine will provide help verbose debug

   return prgmdesc;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end GlobalData::BuildCommandLine()

//------------------------------------------------------------------------------------
int GlobalData::ExtraProcessing(string& errors, string& extras) throw(Exception)
{
try {
   // do extra parsing, define cmdlineExtras, and append errors to cmdlineErrors
   string str;
   vector<string> fields;
   ostringstream oss, ossx;

   // trop
   fields = StringUtils::split(tropstr,',');
   if(fields.size() != 1 && fields.size() != 4) {
      oss << "Error - invalid field in --Trop input: " << tropstr << endl;
      inputIsValid = false;
   }
   else {
      tropName = fields[0];
      if(fields.size() == 4) {
         Temp = StringUtils::asDouble(fields[1]);
         Press = StringUtils::asDouble(fields[2]);
         Humid = StringUtils::asDouble(fields[3]);
      }

      // initialize trop (initially it was Simple)
      // define the final trop model, from the user's input
      if(tropName == string("Simple"))        pTrop = new SimpleTropModel();
      else if(tropName == string("Zero"))     pTrop = new ZeroTropModel();
      else if(tropName == string("Saas"))     pTrop = new SaasTropModel();
      else if(tropName == string("NB"))       pTrop = new NBTropModel();
      else if(tropName == string("GG"))       pTrop = new GGTropModel();
      else if(tropName == string("GGHeight")) pTrop = new GGHeightTropModel();
      else if(tropName == string("Neill"))    pTrop = new NeillTropModel();
      else if(tropName == string("Global"))   pTrop = new GlobalTropModel();
      else if(tropName == string("GCAT"))     pTrop = new GCATTropModel();
      else if(tropName == string("MOPS"))     pTrop = new MOPSTropModel();
      else {
         ossx << " Warning - unknown trop model: " << tropName
               << "; use Global" << endl;
         tropName = string("Global");
         pTrop = new GlobalTropModel();
      }
      // else error

      // set weather here, to the default, in case there is NO met
      // NB this setWeather() belongs to TropModel
      pTrop->setWeather(Temp,Press,Humid);
      ossx << "   Trop model " << pTrop->name() << " weather: "
         << Temp << "degC, " << Press << "mbar, " << Humid << "%RH" << endl;

   }

   // reference position
   if(!refPosstr.empty()) {
      fields = StringUtils::split(refPosstr,',');
      if(fields.size() != 3) {
         oss << " Error - invalid field in --refPos input: " << refPosstr << endl;
      }
      else {
         try {
            refPos.setECEF(StringUtils::asDouble(fields[0]),
                           StringUtils::asDouble(fields[1]),
                           StringUtils::asDouble(fields[2]));

            // set position for trop
            // Q: why is it checking the coordinate system of refPos? I thought that
            // refPos could provide the position values in multiple coordinate systems?
            // if it does need to be set to some primary coordinate system somewhere,
            // where is that done?  Set to ECEF by refPos.setECEF above? if so, why
            // do this check?
            if(refPos.getCoordinateSystem() != Position::Unknown) {
               pTrop->setReceiverHeight(refPos.getHeight());
               pTrop->setReceiverLatitude(refPos.getGeodeticLatitude());
               pTrop->setReceiverLongitude(refPos.getLongitude());
               ossx << "   Trop model Rx at lat " << refPos.getGeodeticLatitude()
               << ", long " << refPos.getLongitude()
               << ", ht " << refPos.getHeight();

               ossx << endl;
            }
            else {
               oss << " Error - definition of reference position failed." << endl;
               inputIsValid = false;
            }

         }
         catch(Exception& ) {
            oss << " Error - invalid position in --refPos : " << refPosstr << endl;
            inputIsValid = false;
         }
      }
   }
   else {
      ossx << " Error - no reference position given" << endl;
      inputIsValid = false;
   }

   // time
   pTrop->setDayOfYear(doy);
   ossx << "   Trop model on day of year " << doy << endl;

   //i = StringUtils::numWords(timestr,',');
   //if(i != 1 && i != 2 && i != 3 && i != 6) {
   //   oss << " Error - invalid time (# fields) in --time : " << timestr << endl;
   //   inputIsValid = false;
   //}
   //else {
   //   try {
   //      scanTime(troptime, timestr,
   //         ( i==1 ? "%Q" :
   //         ( i==2 ? "%F,%g" :
   //         ( i==3 ? "%Y,%j,%s" : "%Y,%m,%d,%H,%M,%f"))));

   //      int doy = static_cast<YDSTime>(troptime).doy;
   //      pTrop->setDayOfYear(doy);
   //      ossx << "   Trop model on day of year " << doy << endl;
   //   }
   //   catch(Exception& e) {
   //      oss << " Error - invalid time in --time : " << timestr << endl;
   //      inputIsValid = false;
   //   }
   //}

   // append to errors, define extras
   errors += oss.str();
   extras = ossx.str();

   return (inputIsValid ? 0 : 4);
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
int GlobalData::OpenLogFile(void) throw(Exception)
{
try {
   // open log file, if it exists
   if(!logfile.empty()) {
      oflog.open(logfile.c_str(),ios_base::out);
      if(!oflog) {
         cerr << "Failed to open log file " << logfile << endl;
         return 5;
      }
      LOG(INFO) << "Output directed to log file " << logfile;
      pLOGstrm = &oflog; // ConfigureLOG::Stream() = &oflog;
   }

   // configure log stream
   ConfigureLOG::ReportLevels() = false;
   ConfigureLOG::ReportTimeTags() = false;

   // help, debug and verbose handled automatically by CommandLine
   verbose = (LOGlevel >= VERBOSE);
   debug = (LOGlevel - DEBUG);

   if(pLOGstrm != &cout) LOG(INFO) << Title;

   // dump configuration
   if(debug > -1) {
      LOG(INFO) << "Found debug switch at level " << debug;
      // NB debug turns on verbose
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int Process(void) throw(Exception)
{
try {
   GlobalData& GD=GlobalData::Instance();

   LOG(INFO) << setw(8) << GD.pTrop->name() << " Trop model"
         << fixed << setprecision(4)
         << " has zenith delays: dry = " << setw(6) << GD.pTrop->dry_zenith_delay()
         << " wet = " << setw(6) << GD.pTrop->wet_zenith_delay() << " meters";

   LOG(INFO) << "drytotal = drymap*(dry zenith delay), wettotal = wetmap*(wet zenith delay)";

   LOG(INFO) << setw(8) << "Name"
         << " " << setw(5) << "elev"
         << " " << setw(11) << "drymap"
         //<< " " << setw(8) << "drydelay"
         << " " << setw(8) << "drytotal"
         << " " << setw(8) << "wetmap"
         //<< " " << setw(8) << "wetdelay"
         << " " << setw(8) << "wettotal"
         << " " << setw(8) << "total(m)";

   double elev(GD.elevmin);
   while(elev <= GD.elevmax) {
      LOG(INFO) << setw(8) << GD.pTrop->name() << fixed << setprecision(2)
         << " " << setw(5) << elev << setprecision(4)
         << " " << setw(11) << GD.pTrop->dry_mapping_function(elev)
         //<< " " << setw(8) << GD.pTrop->dry_zenith_delay()
         << " " << setw(8)
            << GD.pTrop->dry_zenith_delay()*GD.pTrop->dry_mapping_function(elev)
         << " " << setw(8) << GD.pTrop->wet_mapping_function(elev)
         //<< " " << setw(8) << GD.pTrop->wet_zenith_delay()
         << " " << setw(8)
            << GD.pTrop->wet_zenith_delay()*GD.pTrop->wet_mapping_function(elev)
         << " " << setw(8) << GD.pTrop->correction(elev);

      elev += GD.delev;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}  // end Process()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
