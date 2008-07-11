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
 * @file ResCor.cpp
 * 'Residuals and Corrections'
 * Open and read a single Rinex observation file, apply editing commands
 * using the RinexEditor package, compute any of several residuals and corrections
 * and register extended Rinex observation types for them, and then write
 * the edited data, along with the new extended observation types,
 * to an output Rinex observation file. Input is all on the command line.
 * ResCor is implemented by deriving a special class from class RinexEditor and
 * using its virtual functions to implement all the changes necessary to define
 * and compute the residuals and corrections.
 */

//------------------------------------------------------------------------------------
// ToDo
// catch exceptions -- elsewhere and on reading header and obs
// allow user to specify trop model, both for RAIM and for TR output
//

#include "StringUtils.hpp"
#include "DayTime.hpp"
#include "RinexSatID.hpp"
#include "CommandOptionParser.hpp"
#include "CommandOption.hpp"
#include "CommandOptionWithTimeArg.hpp"
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"
#include "SP3Stream.hpp"
#include "SP3EphemerisStore.hpp"
#include "GPSEphemerisStore.hpp"
#include "EphemerisRange.hpp"
#include "TropModel.hpp"
#include "PRSolution.hpp"
#include "WGS84Geoid.hpp"           // for obliquity
#include "Stats.hpp"
#include "geometry.hpp"             // DEG_TO_RAD
#include "icd_200_constants.hpp"    // PI,C_GPS_M,OSC_FREQ,L1_MULT,L2_MULT

#include "RinexEditor.hpp"
#include "RinexUtilities.hpp"
#include "Position.hpp"

#include <ctime>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
   // prgm data
string PrgmName("ResCor");
string PrgmVers("3.8 7/11/08");

// data used in program
const double CFF=C_GPS_M/OSC_FREQ;
const double F1=L1_MULT;   // 154.0;
const double F2=L2_MULT;   // 120.0;
const double f12=F1*F1;
const double f22=F2*F2;
const double wl1=CFF/F1;
const double wl2=CFF/F2;
const double wl1r=F1/(F1+F2);
const double wl2r=F2/(F1+F2);
const double wl1p=wl1*F1/(F1-F2);
const double wl2p=-wl2*F2/(F1-F2);
const double if1r=f12/(f12-f22);
const double if2r=-f22/(f12-f22);
const double if1p=wl1*f12/(f12-f22);
const double if2p=-wl2*f22/(f12-f22);
const double gf1r=-1;
const double gf2r=1;
const double gf1p=wl1;
const double gf2p=-wl2;
const double alpha=f12/f22 - 1.0;
const double FL1=F1*10.23e6;                 // Hz
const double TECUperM=FL1*FL1*1.e-16/40.28;  // 6.1617 TECU/m (0.16229 m/TECU)

clock_t totaltime;
string Title;
   // input flags and data
bool Debug,Verbose,Callow,Cforce,doTgd;
double IonoHt;
RinexSatID SVonly;
string LogFile;
ofstream logof,oferr;         // don't call it oflog - RinexEditor has that
   // Rinex headers, input and output, saved
RinexObsHeader rhead, rheadout;
   // ephemeris
string NavDir;
vector<string> NavFiles;
SP3EphemerisStore SP3EphList;
GPSEphemerisStore BCEphList;
SimpleTropModel ggtm;
   // for use with current position and in RefPosMap (RAIM and/or RefPosFile)
typedef struct ReferencePositionFileData {
   Position RxPos;              // XYZT
   bool valid;
   int NPRN;
   double clk,PDOP,GDOP,RMS;
} RefPosData;
RefPosData CurrRef;        // current reference position
   // reference and RAIM solution
string RefPosFile,KnownPos;
bool doRAIM,editRAIM,outRef,headRAIM,HaveRAIM;
bool RefPosInput,KnownPosInput,KnownLLH,RefPosFlat;
double minElev;
vector<SatID> Sats;
vector<double> PRange;
//RAIMSolution RAIMSol;
PRSolution prsol;
Stats<double> ARSX,ARSY,ARSZ;      // average solution, for header output
   // computation
int inC1,inP1,inP2,inL1,inL2;      // indexes in rhead of C1, C1/P1, P2, L1 and L2
int inEP,inPS;                     // flags for input of ephemeris, Rx position
int inD1,inD2,inS1,inS2;
DayTime CurrentTime, PrgmEpoch;
// these 3 vectors parallel
vector<string> OTstrings;          // list of OTs (strings) to be computed
vector<RinexObsHeader::RinexObsType> OTList;
vector<int> OTindex;
int otC1,otP1,otP2,otL1,otL2;      // indexes in rheadout of C1, C1/P1, P2, L1 and L2
int otD1,otD2,otS1,otS2;
bool DoSVX;
WGS84Geoid WGS84;
// compute non-dispersive range, ionospheric delay, multipath (L1 and L2)
bool DoXR;
double XRM0[4],XRM1[4],XRM2[4],XRM3[4];
double *XRM[4]={XRM0,XRM1,XRM2,XRM3};
double XRdat[4],XRsol[4];
   // structure for holding raw range and phase data during computation
typedef struct range_and_phase_data {
   double L1,L2,P1,P2;
   int LL1,LL2;
} RCData;
   // map of <sat,RCData>
RCData DataStore;
map<RinexSatID,RCData> DataStoreMap;
   // debiasing output data
map<RinexObsHeader::RinexObsType,map<RinexSatID,double> > AllBiases; // (OT,SV)
   // reference position as function of time (from input)
map<DayTime,RefPosData> RefPosMap;
double RefPosMapDT;

string RxhelpString=
"\n --RxFlat <fn> : fn is a file with reference receiver positions and times:\n"
"  The first line in the file (other than comments, marked by # in column 1)\n"
"  is the format for each line of the file, using the specifications in\n"
"  DayTime::setToString() and Position::setToString().\n"
"  The second line is a pattern made up of characters T, P and X indicating the\n"
"  content of both the lines in the file and the format: (white-space-delimited)\n"
"  words on each line are either part of the time(T) or position(P) specification,\n"
"  or are to be ignored(X). For example, the file begins with these six lines:\n"
"  # format:\n"
"  t= %F %g p= %x %y %z\n"
"  # pattern:\n"
"  XTTXPPP\n"
"  # data:\n"
"  t= 1281 259200    p=   -2701232.4        6123085.7        1419837.5";

//------------------------------------------------------------------------------------
// inherit RinexEditor so that callback routines can be defined by Prgm ResCor
class RCRinexEditor : public RinexEditor
{
   public:
         /// Constructor.
      RCRinexEditor() throw() {};

         /// destructor
      virtual ~RCRinexEditor() {}
   
         /// after reading input header and before calling
         /// RinexEditor::EditHeader (pass input header)
      virtual int BeforeEditHeader(const RinexObsHeader& rhin) throw(Exception);

         /// after calling RinexEditor::EditHeader (pass output header)
      virtual int AfterEditHeader(const RinexObsHeader& rhout) throw(Exception);

         /// after reading input obs and before calling
         /// RinexEditor::EditObs (pass input obs)
      virtual int BeforeEditObs(const RinexObsData& roin) throw(Exception);

         /// before writing out header (pass output header)
      virtual int BeforeWritingHeader(RinexObsHeader& rhout) throw(Exception);

         /// before writing out filled header
      virtual int BeforeWritingFilledHeader(RinexObsHeader& rhout) throw(Exception);

         /// just before writing output obs (pass output obs)
      virtual int BeforeWritingObs(RinexObsData& roout) throw(Exception);

}; // end class RCRinexEditor

//------------------------------------------------------------------------------------
// prototypes
int GetCommandLine(int argc, char **argv, RCRinexEditor& rc) throw(Exception);
int PrepareInput(void) throw(Exception);
int LoopOverObs(void) throw(Exception);
void SaveData(const RinexObsData& rod, const RinexObsHeader& rh,
   int xL1, int xL2, int xP1, int xP2) throw(Exception);
int UpdateRxPosition(void) throw(Exception);
void ComputeNewOTs(RinexObsData& rod) throw(Exception);
void CloseOutputFile(void) throw(Exception);
void PreProcessArgs(const char *arg, vector<string>& Args) throw(Exception);
int setBiasLimit(RinexObsHeader::RinexObsType& ot, double lim) throw(Exception);
double removeBias(const RinexObsHeader::RinexObsType& ot, const RinexSatID& sat,
   bool& reset, DayTime& tt, double delta) throw(Exception);

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   totaltime = clock();
   int iret;
   // NB. Do not instantiate editor outside main(), b/c DayTime::END_OF_TIME is a
   // static const that can produce static intialization order problems under some OS.
   RCRinexEditor REC;
   CurrentTime = DayTime::BEGINNING_OF_TIME; // for same reason, init here...

      // Title and description
   Title = PrgmName + ", part of the GPS ToolKit, Ver. " + PrgmVers
      + " (editor " + REC.getRinexEditVersion() + string("), Run ");
   time_t timer;
   struct tm *tblock;
   timer = time(NULL);
   tblock = localtime(&timer);
   PrgmEpoch.setYMDHMS(1900+tblock->tm_year,1+tblock->tm_mon,
               tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
   Title += PrgmEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S");
   Title += "\n";
   cout << Title;

      // define extended types
   iret = RegisterARLUTExtendedTypes();
   if(iret) goto quit;

   // Set defaults, define command line and parse it.
   // Send REdit cmds to REC. Check validity of input.
   iret = GetCommandLine(argc, argv, REC);
   if(iret) goto quit;

   // Initialize, read ephemerides, set flags and prepare for processing
   iret = PrepareInput();
   if(iret) goto quit;

   // Edit the file, including callbacks
   iret = REC.EditFile();
   if(Debug) logof << "EditFile returned " << iret << endl;
   if(iret) goto quit;

   quit:
   // compute run time
   totaltime = clock()-totaltime;
   logof << "ResCor timing: " << setprecision(3)
      << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.\n";

   logof.close();
   cout << "End ResCor" << endl;
   return iret;
}
catch(gpstk::FFStreamError& e) { cerr << e; }
catch(gpstk::Exception& e) { cerr << e; }
catch(exception& e) { cerr << e.what(); }
catch (...) { cerr << "Unknown error.  Abort." << endl; }
   return 1;
}   // end main()

//------------------------------------------------------------------------------------
// Set defaults, define command line and parse it. Send REdit cmds to REC.
// Check validity of input
int GetCommandLine(int argc, char **argv, RCRinexEditor& REC) throw(Exception)
{
   bool help=false,Rxhelp=false,REChelp=false,ROThelp=false;
   int i,j,iret;
try {
      // defaults
   Debug = Verbose = false;

   doRAIM = false;
   KnownPosInput = RefPosInput = false;
   outRef = true;
   editRAIM = true;
   headRAIM = false;
   minElev = 0.0;
   
   IonoHt = 400.0;      // km

   Callow = true;
   Cforce = false;
   doTgd = false;

   LogFile = string("ResCor.log");

      // -------------------------------------------------
      // required options

      // optional options

      // this only so it will show up in help page...
   CommandOption dashf(CommandOption::hasArgument, CommandOption::stdType,
      'f',"","\nConfiguration input:\n --file <file>   File containing more options");

   // ephemeris
   CommandOption dashn(CommandOption::hasArgument, CommandOption::stdType,
      0,"nav"," --nav <file>    Navigation (Rinex Nav OR SP3) file(s)");

   CommandOption dashnd(CommandOption::hasArgument, CommandOption::stdType,
      0,"navdir"," --navdir <dir>  Directory of navigation file(s)");
   dashnd.setMaxCount(1);

   // reference position(s)
   CommandOption dashRx1(CommandOption::hasArgument,CommandOption::stdType,0,"RxLLH",
      "Reference position input: (there are six ways to input the reference "
      "position(s):\n --RxLLH <l,l,h> 1.Receiver position (static) in geodetic "
      "lat, lon(E), ht (deg,deg,m)");
   dashRx1.setMaxCount(1);

   CommandOption dashRx2(CommandOption::hasArgument, CommandOption::stdType,0,"RxXYZ",
      " --RxXYZ <x,y,z> 2.Receiver position (static) in ECEF coordinates (m)");
   dashRx2.setMaxCount(1);

   CommandOptionNoArg dashRx3(0,"Rxhere",
      " --Rxhere        3.Reference site positions(time) from this file"
      " (i.e. -IF<RinexFile>)");
   dashRx3.setMaxCount(1);

   CommandOption dashRx4(CommandOption::hasArgument, CommandOption::stdType,0,
      "RxRinex"," --RxRinex <fn>  4.Reference site positions(time) from another "
      "Rinex file named <fn>");
   dashRx4.setMaxCount(1);

   CommandOption dashRx5(CommandOption::hasArgument,CommandOption::stdType,0,"RxFlat",
      " --RxFlat <fn>   5.Reference site positions and times given in a flat file"
      " named <fn>");
   dashRx5.setMaxCount(1);

   CommandOptionNoArg dashRxhelp(0,"Rxhelp"," --Rxhelp        "
      "(Enter --Rxhelp for a description of the -RxFlat file format)");
   dashRxhelp.setMaxCount(1);

   CommandOptionNoArg dashRx6(0,"RAIM",
      " --RAIM          6.Reference site positions computed via RAIM"
      " (requires P1,P2,EP)");
   dashRx6.setMaxCount(1);

   CommandOptionNoArg dashred(0,"noRAIMedit",
      "  (NB the following four options apply only if --RAIM is found)\n"
      " --noRAIMedit    Do not edit data based on RAIM solution");
   dashred.setMaxCount(1);

   CommandOptionNoArg dashrh(0,"RAIMhead",
      " --RAIMhead      Output average RAIM solution to Rinex header "
      "(if -HDf also appears)");
   dashrh.setMaxCount(1);

   CommandOptionNoArg dashro(0,"noRefout",
      " --noRefout      Do not output reference solution to Rinex");
   dashro.setMaxCount(1);

   CommandOption dashelev(CommandOption::hasArgument,CommandOption::stdType,
      0,"MinElev",
      " --MinElev <el>  Minimum satellite elevation (deg) for output");
   dashelev.setMaxCount(1);

   // residual and correction computation, processing options
   CommandOption dashdb(CommandOption::hasArgument, CommandOption::stdType,0,"debias",
      "Residual/Correction computation:\n"
      " --debias <OT,l> Debias new output type <OT>; "
      "trigger a bias reset with limit <l>");

   CommandOptionNoArg dashca(0,"Callow",
      " --Callow        Allow C1 to replace P1 when P1 is not available");
   dashca.setMaxCount(1);

   CommandOptionNoArg dashcf(0,"Cforce",
      " --Cforce        Force C/A code pseudorange C1 to replace P1");
   dashcf.setMaxCount(1);

   CommandOption dashih(CommandOption::hasArgument, CommandOption::stdType,0,"IonoHt",
      " --IonoHt <ht>   Height of ionosphere in km (default 400) "
      "(needed for LA,LO,VR,VP)");
   dashih.setMaxCount(1);

   CommandOptionNoArg dashtgd(0,"Tgd",
      " --Tgd           Apply the Tgd from BC ephemeris to SR,SP,VR,VP");
   dashtgd.setMaxCount(1);

   CommandOption dashSV(CommandOption::hasArgument, CommandOption::stdType, 0,"SVonly",
      " --SVonly <sat>  Process this satellite ONLY (use editing command -DS<Sat> to delete Sat)");
   dashSV.setMaxCount(1);

   // output files
   CommandOption dashLog(CommandOption::hasArgument, CommandOption::stdType,
      0,"Log","Output files:\n --Log <file>    Output log file name (rc.log)");
   dashLog.setMaxCount(1);

   //CommandOption dashErr(CommandOption::hasArgument, CommandOption::stdType,
   //   0,"Err"," --Err <file>    Output error file name (rc.err)");
   //dashErr.setMaxCount(1);

   // help
   CommandOptionNoArg dashVerb(0,"verbose",
      "Help:\n --verbose       Print extended output to log file.");
   dashVerb.setMaxCount(1);

   CommandOptionNoArg dashDebug(0,"debug",
      " --debug         Print debugging information to log file.");
   dashDebug.setMaxCount(1);

   CommandOptionNoArg dashh('h', "help"," --help [or -h]  Print syntax and quit.");
   CommandOptionNoArg dashrech(0, "REChelp",
      " --REChelp       Print syntax of RinexEditor commands and quit.");
   CommandOptionNoArg dashexth(0, "ROThelp",
      " --ROThelp       Print list of extended Rinex observation types and quit.");

   // ... other options
   CommandOptionRest Rest("");

   CommandOptionParser Par(
   "Prgm ResCor will open and read a single Rinex observation file, "
   "apply editing commands\n"
   "   using the RinexEditor package, compute any of several residuals "
   "and corrections and\n"
   "   register extended Rinex observation types for them, and then write "
   "the edited data,\n"
   "   along with the new extended observation types, to an output Rinex "
   "observation file.\n"
   "\nRequired arguments:\n"
   " -IF and -OF (RinexEditor commands: cf. --REChelp) are required arguments.\n");

      // -------------------------------------------------
      // allow user to put all options in a file
      // could also scan for debug here
   vector<string> Args;
   for(j=1; j<argc; j++) PreProcessArgs(argv[j],Args);
   argc = Args.size();
   if(argc==0) Args.push_back(string("--help"));

   //if(Debug) {
      //cout << "List after PreProcessArgs\n";
      //for(i=0; i<argc; i++) cout << i << " " << Args[i] << endl;
   //}

      // add PRGM and RUNBY strings to the header
   REC.REVerbose = Verbose;
   REC.REDebug = Debug;
   Args.push_back(string("-HDp") + PrgmName + string(" v.") + PrgmVers.substr(0,4));
   Args.push_back(string("-HDrARL:UT/SGL/GPSTK"));

   if(Debug) {
      cout << "List passed to REditCommandLine:\n";
      for(i=0; i<argc; i++) cout << i << " " << Args[i] << endl;
   }

      // Add RE cmds; this will strip out the REditCmds from Args
   REC.AddCommandLine(Args);
   if(Debug) {
      cout << "List after REC.AddCommandLine(Args)\n";
      argc = Args.size();
      for(i=0; i<argc; i++) cout << i << " " << Args[i] << endl;
   }

      // get the list of commands and create OTstrings
   vector<string> cmds=REC.CommandList();
   if(Debug) cout << "Here is the list of RE commands:\n";
   for(i=0; i<cmds.size(); i++) {
      string cmd = cmds[i];
      if(Debug) cout << "  " << cmd << endl;

      vector<string> field;
      while(cmd.size() > 0)
         field.push_back(stripFirstWord(cmd,','));
      if(field.size() < 5) continue;
      if(field[0] == "AO")
         OTstrings.push_back(field[4]);
   }
   if(Debug) cout << "End list of RE commands." << endl;

      // preprocess the commands
      // Return 0 ok, -1 no input file name, -2 no output file name
   iret = REC.ParseCommands();
   //if(Debug) {
      //cout << "\nHere is the parsed list of RE cmds\n";
      //it=REC.Cmds.begin();
      //while(it != REC.Cmds.end()) { it->Dump(cout,string("")); ++it; }
      //cout << "End of sorted list of RE cmds" << endl << endl;

      // pass the rest to the regular command line processor
   //}

      // -------------------------------------------------------------------
   argc = Args.size()+1;
   char **CArgs=new char*[argc];
   if(!CArgs) { cerr << "Failed to allocate CArgs\n"; return -1; }
   CArgs[0] = argv[0];
   for(j=1; j<argc; j++) {
      CArgs[j] = new char[Args[j-1].size()+1];
      if(!CArgs[j]) { cerr << "Failed to allocate CArgs[j]\n"; return -1; }
      strcpy(CArgs[j],Args[j-1].c_str());
   }

   //if(Debug) {
      //cout << "List passed to parser\n";
      //for(i=0; i<argc; i++) cout << i << " " << CArgs[i] << endl;
   //}

   Par.parseOptions(argc, CArgs);
   delete[] CArgs;

      // -------------------------------------------------
      // was help requested?
   if(dashh.getCount() > 0) help=true;
   if(dashRxhelp.getCount() > 0) Rxhelp=true;
   if(dashrech.getCount() > 0) REChelp=true;
   if(dashexth.getCount() > 0) ROThelp=true;
      // if errors on the command line, dump them and turn on help
   if(!(help || Rxhelp || REChelp || ROThelp) && (iret<0 || Par.hasErrors())) {
      cout << "Errors found in command line input:\n";
      if(iret==-1 || iret==-3) cout << "Input file name required: use -IF<name>\n";
      if(iret==-2 || iret==-3) cout << "Output file name required: use -OF<name>\n";
      Par.dumpErrors(cout);
      cout << "...end of Errors\n\n";
      help = true;
   }
      // display syntax page(s)
   if(help || Rxhelp || REChelp || ROThelp) {
      if(help)
         Par.displayUsage(cout,false);
      if(Rxhelp)
         cout << RxhelpString << endl;
      if(REChelp) {
         if(help || Rxhelp) cout << endl;
         cout << "ResCor is an implementation of the RinexEditor, therefore the"
               << " following commands are accepted.\n";
         DisplayRinexEditUsage(cout);
      }
      if(ROThelp) {
         if(help || Rxhelp || REChelp) cout << endl;
         DisplayStandardRinexObsTypes(cout);
         cout << "End of list of standard observation types\n";
         DisplayExtendedRinexObsTypes(cout);
         cout << "End of list of extended observation types\n";
      }
      if(iret < 0) return iret;
   }

      // -------------------------------------------------
      // get values found on command line
   vector<string> values;

   //dashf intercepted above by PreProcessArgs
   //dashh Handled above (first)
   //if(dashDebug.getCount()) Debug=true; done by PreProcessArgs
   //if(dashVerb.getCount()) Verbose=true; done by PreProcessArgs

      // now do the rest
   // ephemeris input
   if(dashnd.getCount()) {
      values = dashnd.getValue();
      NavDir = values[0];
      if(help) cout << "Nav Directory is " << NavDir  << endl;
   }
   if(dashn.getCount()) {
      values = dashn.getValue();
      NavFiles = values;
      if(help) {
         cout << "Nav files are:";
         for(i=0; i<NavFiles.size(); i++) cout << " " << NavFiles[i];
         cout << endl;
      }
   }

   // reference position
   if(dashRx1.getCount()) {
      values = dashRx1.getValue();
      KnownPos = values[0];
      KnownLLH = true;
      KnownPosInput = true;
      if(help) cout << "Get reference position from explicit input (LLH) "
         << KnownPos << endl;
   }
   if(dashRx2.getCount()) {
      values = dashRx2.getValue();
      KnownPos = values[0];
      KnownLLH = false;
      KnownPosInput = true;
      if(help) cout << "Get reference position from explicit input (XYZ) "
         << KnownPos << endl;
   }
   if(dashRx3.getCount()) {       // get ref from this input file
      RefPosInput = true;
      if(help) cout << "Get reference position from this input file" << endl;
   }
   if(dashRx4.getCount()) {
      values = dashRx4.getValue();
      RefPosFile = values[0];
      RefPosFlat = false;
      if(help) cout << "Get reference position from Rinex file " << RefPosFile<<endl;
   }
   if(dashRx5.getCount()) {
      values = dashRx5.getValue();
      RefPosFile = values[0];
      RefPosFlat = true;
      if(help) cout << "Get reference position from flat file " << RefPosFile << endl;
   }
   if(dashRx6.getCount()) {
      doRAIM = true;
      if(help) cout << "Compute a RAIM solution" << endl;
   }

   // RAIM options
   if(dashred.getCount()) {
      if(doRAIM) {
         editRAIM = false;
         if(help) cout << "Do not edit data based on RAIM solution" << endl;
      }
      else if(help) cout << "Ignore --noRAIMedit: --RAIM was not set" << endl;
   }
   if(dashro.getCount()) {
      outRef = false;
      if(help) cout << "Do not output Reference solution to Rinex" << endl;
   }
   if(dashelev.getCount()) {
      values = dashelev.getValue();
      minElev = asDouble(values[0]);
      if(help) cout << "Set minimum elevation angle "
         << fixed << setprecision(2) << minElev << endl;
   }
   if(dashrh.getCount()) {
      if(doRAIM) {
         headRAIM = true;
         if(help) cout << "Output average RAIM solution to header" << endl;
      }
      else if(help) cout << "Ignore --RAIMhead: --RAIM was not set" << endl;
   }

   if(dashdb.getCount()) {
      values = dashdb.getValue();
      vector<string> subfield;
      string::size_type pos;
      for(i=0; i<values.size(); i++ ) {
         string argbias=values[i];
         subfield.clear();
         while(argbias.size() > 0) {
            pos = argbias.find(",");
            if(pos==string::npos) pos=argbias.size();
            if(pos==0) subfield.push_back(" ");
            else subfield.push_back(argbias.substr(0,pos));
            if(pos >= argbias.size()) break;
            argbias.erase(0,pos+1);
         }
         RinexObsHeader::RinexObsType OT;
         OT = RinexObsHeader::convertObsType(subfield[0]);
         double limit=asDouble(subfield[1]);
         int iret=setBiasLimit(OT,limit);
         if(iret) {
            cout << "Error: '--debias <OT,lim>' input is invalid: "
               << values[i] << endl;
            cerr << "Error: '--debias <OT,lim>' input is invalid: "
               << values[i] << endl;
         }
         else if(Debug)
            cout << "Set bias limit for " << RinexObsHeader::convertObsType(OT)
            << " to " << fixed << setprecision(3) << limit
            << " (" << values[i] << ")" << endl;
      }
   }
   if(dashca.getCount()) {
      Callow = true;
      if(help) cout << "Allow C1 to be P1 when P1 not available\n";
   }
   if(dashcf.getCount()) {
      Cforce = true;
      if(help) cout << "Force C1 to replace P1 when C1 available\n";
   }
   if(dashih.getCount()) {
      values = dashih.getValue();
      IonoHt = asDouble(values[0]);
      if(help) cout << "Set ionosphere height to " << values[0] << " km" << endl;
   }
   if(dashtgd.getCount()) {
      doTgd = true;
      if(help) cout << "Apply Tgd correction when BC ephemeris is provided" << endl;
   }
   if(dashSV.getCount()) {
      values = dashSV.getValue();
      SVonly.fromString(values[0]);
      if(help) cout << "Process only satellite : " << SVonly << endl;
   }
   if(dashLog.getCount()) {
      values = dashLog.getValue();
      LogFile = values[0];
      if(help) cout << "Log file is " << LogFile << endl;
   }

   if(Rest.getCount() && help) {
      cout << "Remaining options:" << endl;
      values = Rest.getValue();
      for (i=0; i<values.size(); i++) cout << values[i] << endl;
   }

   //if(Verbose && help) {
   //   cout << "\nTokens on command line (" << Args.size() << ") are:" << endl;
   //   for(j=0; j<Args.size(); j++) cout << Args[j] << endl;
   //}

      // -------------------------------------------------
      // now process some of the input
   try {
      logof.clear();
      logof.exceptions(ios_base::badbit | ios_base::failbit);
      logof.open(LogFile.c_str(),ios::out);
      if(logof.fail()) {
         cout << "Failed to open log file " << LogFile << endl;
         return -1;
      }
      else {
         cout << "Opened log file (for all output, including debug) "
            << LogFile << endl;
         logof << Title;
      }
      REC.oflog = &logof;
   }
   catch(ios_base::failure& e) {
      cout << "Exception " << e.what() << endl;
      return -1;
   }

   // check for multiple inputs
   if(KnownPosInput || !RefPosFile.empty() || doRAIM || RefPosInput) {
      i = 0;
      if(KnownPosInput) i++;
      if(!RefPosFile.empty()) i++;
      if(doRAIM) i++;
      if(RefPosInput) i++;
      if(i > 1) {
         ostringstream stst;
         stst << "ERROR: multiple inputs inconsistent:";
         if(KnownPosInput) stst << (KnownLLH ? " --RxLLH" : " --RxXYZ");
         if(!RefPosFile.empty()) stst << (RefPosFlat ? " --RxFlat" : " --RxRinex");
         if(doRAIM) stst << " --RAIM";
         if(RefPosInput) stst << " --RxHere";
         stst << endl;
         logof << stst.str();
         cerr << stst.str();
         return -1;           // fail? or take default
      }
      else if(help) logof << "Position input ok\n";
   }
      // print config to log
   if(Verbose) {
      logof << "-------- Here is the program configuration:\n";
      logof << "Input Rinex observation file name is: "
         << REC.InputFileName() << endl;
      logof << "Input Directory is " << REC.InputDirectory() << endl;
      logof << "Output Rinex obs file name is: " << REC.OutputFileName() << endl;
      logof << "Output Directory is " << REC.OutputDirectory() << endl;
      if(REC.BeginTimeLimit() > DayTime::BEGINNING_OF_TIME)
         logof << "Begin time limit is " << REC.BeginTimeLimit() << endl;
      if(REC.EndTimeLimit() < DayTime::END_OF_TIME)
         logof << "End time limit is " << REC.EndTimeLimit() << endl;
      if(REC.Decimation() != 0) logof << "Decmimation time interval is "
         << setprecision(2) << REC.Decimation() << " seconds." << endl;
      logof << "Tolerance in time-comparisions is " << setprecision(8)
         << REC.Tolerance() << " seconds." << endl;
      logof << "Log file name is " << LogFile << " (this file)" << endl;
      if(SVonly.id > 0) logof << "Process only satellite : " << SVonly << endl;

      if(!NavDir.empty()) logof << "Nav Directory is " << NavDir  << endl;
      if(NavFiles.size()) {
         logof << "Nav files:";
         for(i=0; i<NavFiles.size(); i++) logof << " " << NavFiles[i];
         logof << endl;
      }
      if(KnownPosInput) logof << "Get reference position from explicit input ("
         << (KnownLLH ? "LLH" : "XYZ") << ") : " << KnownPos << endl;
      if(doRAIM) logof << "Compute a RAIM solution" << endl;
      if(minElev > 0.0) logof << "Minimum elevation angle limit "
         << fixed << setprecision(2) << minElev << " degrees." << endl;
      if(RefPosInput) logof << "Get reference position from in-line headers in "
         << "the input Rinex file" << endl;
      if(!RefPosFile.empty())
         logof << "Get reference position from a " << (RefPosFlat ? "flat" : "Rinex")
            << " file: " << RefPosFile << endl;
      if(!editRAIM) logof << "Do not ";
      logof << "Edit data based on RAIM solution" << endl;
      if(!outRef) logof << "Do not ";
      logof << "Output Reference solution to Rinex" << endl;
      if(!headRAIM) logof << "Do not ";
      logof << "Output average RAIM solution to header" << endl;
      if(Callow) logof << "Allow C1 to be P1 when P1 not available\n";
      if(Cforce) logof << "Force C1 to replace P1 when C1 available\n";
      logof << "Ionosphere height is " << IonoHt << " km" << endl;
      if(doTgd) logof << "Apply Tgd correction when BC ephemeris is provided" << endl;
      if(AllBiases.size()) {
         logof << "The list of de-biasing limits is:\n";
         map<RinexObsHeader::RinexObsType,map<RinexSatID,double> >::iterator it;
         for(it=AllBiases.begin(); it!=AllBiases.end(); it++) {
            map<RinexSatID,double>::iterator jt;
            for(jt=it->second.begin(); jt!=it->second.end(); jt++) {
               logof << "  Bias limit(" << RinexObsHeader::convertObsType(it->first)
                  << ") = " << fixed << setprecision(3) << jt->second << endl;
            }
         }
      }
      logof << "-------- End of the program configuration.\n";
      logof << endl;
   }

   if(help) return 1;
   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
   return -1;
}

//------------------------------------------------------------------------------------
// Initialize, read ephemerides, set flags and prepare for processing
int PrepareInput(void) throw(Exception)
{
try {
   int iret,i;

      // set all input/output indexes to 'undefined'
   inC1 = inP1 = inP2 = inL1 = inL2 = inEP = inPS = inD1 = inD2 = inS1 = inS2 = -1;
   otC1 = otP1 = otP2 = otL1 = otL2 = otD1 = otD2 = otS1 = otS2 = -1;

      // --------------------------------------------------------------------
      // ephemeris
      // add Nav directory to nav file names
   if(!NavDir.empty() && NavFiles.size()>0) {
      for(i=0; i<NavFiles.size(); i++)
         NavFiles[i] = NavDir + string("/") + NavFiles[i];
   }

      // open nav files and read EphemerisStore -- set inEP and inPS
   iret = FillEphemerisStore(NavFiles, SP3EphList, BCEphList);
   if(SP3EphList.size()) {
      if(Verbose) SP3EphList.dump(logof,1);
      inEP = 1;
   }
   else if(Verbose) logof << "SP3 Ephemeris list is empty\n";

   if(BCEphList.size()) {
      BCEphList.SearchNear();
      if(Verbose) BCEphList.dump(logof,0);
      inEP = 1;
   }
   else if(Verbose) logof << "BC Ephemeris list is empty\n";

      // --------------------------------------------------------------------
      // position:
      //    if KnownPosInput, position is input
      //    if !RefPosFile.empty(), open file
      //    if RefPosInput, use the aux headers in input file
      //    if(doRAIM) set up RAIMsolution - including input of RMS, etc?
   if(KnownPosInput) {            // parse the string to get position
      vector<string> subfield;
      string::size_type pos;
      while(KnownPos.size() > 0) {
         pos = KnownPos.find(",");
         if(pos==string::npos) pos=KnownPos.size();
         if(pos==0) subfield.push_back(" ");
         else subfield.push_back(KnownPos.substr(0,pos));
         if(pos >= KnownPos.size()) break;
         KnownPos.erase(0,pos+1);
      };

      CurrRef.valid = true;
      CurrRef.clk = 0;
      CurrRef.NPRN = 0;
      CurrRef.PDOP = 0;
      CurrRef.GDOP = 0;
      CurrRef.RMS = 0;
      if(KnownLLH) {
         CurrRef.RxPos.setGeodetic(asDouble(subfield[0]), asDouble(subfield[1]),
            asDouble(subfield[2]));
         CurrRef.RxPos.transformTo(Position::Cartesian);
      }
      else {
         CurrRef.RxPos.setECEF(asDouble(subfield[0]), asDouble(subfield[1]),
            asDouble(subfield[2]));
      }

      // output
      logof << "Reference position comes from explicit input of "
         << "position components:\n";
      logof << " " << subfield[0] << " " << subfield[1] << " " << subfield[2] << endl;
      logof << " =" << fixed
            << " " << setw(13) << setprecision(3) << CurrRef.RxPos.X()
            << " " << setw(13) << setprecision(3) << CurrRef.RxPos.Y()
            << " " << setw(13) << setprecision(3) << CurrRef.RxPos.Z()
            << endl;
      logof << " = " << fixed
            << setw(12) << setprecision(8) << CurrRef.RxPos.geodeticLatitude() << "N "
            << setw(12) << setprecision(8) << CurrRef.RxPos.longitude() << "E "
            << setw(9) << setprecision(3) << CurrRef.RxPos.height() << "m" << endl;
      inPS = 1;
   }
   else if(!RefPosFile.empty()) {
      DayTime timetag;
      //logof << "Reference position from a file (" << RefPosFile << ")\n";
      // make sure it exists first
      ifstream inf(RefPosFile.c_str());
      if(!inf) {
         logof << "Error: could not open positions file " << RefPosFile << endl;
         oferr << "Error: could not open positions file " << RefPosFile << endl;
         return -1;
      }
      // fill the map<DayTime,RefPosData> RefPosMap;
      RefPosMap.clear();
      if(isRinexObsFile(RefPosFile)) {
         if(Verbose) {
            logof << "Reference position will come from input Rinex obs file "
               << RefPosFile << endl;
            if(RefPosFlat)
               logof << " WARNING -- Reference position file is Rinex, not flat!\n";
         }

         inf.close();
         RinexObsHeader header;
         RinexObsData robs;
         RinexObsStream rostream(RefPosFile.c_str());
         rostream.exceptions(fstream::failbit);

         rostream >> header;
         //timetag = header.firstObs;
         while(rostream >> robs) {
            if(robs.epochFlag == 4) {
               // TD: check this; often the in-line header has a bad epoch
               // But if it has XYZT and DIAG, then GPSTk probably wrote it....
               timetag = robs.time;
               CurrRef.NPRN = 0;
               CurrRef.valid = true;
               CurrRef.clk = CurrRef.PDOP = CurrRef.GDOP = CurrRef.RMS = 0.0;
               for(i=0; i<robs.auxHeader.commentList.size(); i++) {
                  string s=robs.auxHeader.commentList[i];
                  string t=stripFirstWord(s);
                  if(t == string("XYZT")) {
                     double x=asDouble(stripFirstWord(s));
                     double y=asDouble(stripFirstWord(s));
                     double z=asDouble(stripFirstWord(s));
                     CurrRef.RxPos.setECEF(x,y,z);
                     CurrRef.clk = asDouble(stripFirstWord(s));
                  }
                  else if(t==string("DIAG")) {
                     CurrRef.NPRN = asInt(stripFirstWord(s));
                     CurrRef.PDOP = asDouble(stripFirstWord(s));
                     CurrRef.GDOP = asDouble(stripFirstWord(s));
                     CurrRef.RMS = asDouble(stripFirstWord(s));
                  }
               }
               RefPosMap[timetag] = CurrRef;
            }
         }
         rostream.close();
         inPS = 1;
      }
      else {            // flat file input
         if(Verbose) {
            logof << "Reference position will come from input flat file "
               << RefPosFile << endl;
            if(!RefPosFlat)
               logof << " WARNING -- Reference position file is flat, not Rinex!\n";
         }

         bool ok,have=false,havefmt=false,havepat=false;
         string line,format,pattern,lineT,lineP,word,fword,fmtT,fmtP;
         Position pos;
         CurrRef.NPRN = 0;
         CurrRef.clk = CurrRef.PDOP = CurrRef.GDOP = CurrRef.RMS = 0.0;
         while(!inf.eof() && inf.good()) {
            ok = true;
            while(line.size() > 0) {
               if(Debug) logof << "echo: " << line << endl;
               if(line[0] == '#') break;              // skip comments
               if(!have) {
                  if(!havefmt) {
                     format = line; 
                     havefmt = true;
                     if(Debug) logof << "Format is " << format << endl;
                  }
                  else if(!havepat) {
                     pattern = line; 
                     havepat = true;
                     if(Debug) logof << "Pattern is " << pattern << endl;
                  }
                  have = havefmt & havepat;
                  break;
               }
               fmtT = fmtP = lineT = lineP = string("");
               for(i=0; i<numWords(line); i++) {
                  word = words(line,i,1);
                  fword = words(format,i,1);
                  if(pattern[i] == 'X') continue;
                  else if(pattern[i] == 'T') {
                     lineT += string(" ") + word;
                     fmtT += string(" ") + fword;
                  }
                  else if(pattern[i] == 'P') {
                     lineP += string(" ") + word;
                     fmtP += string(" ") + fword;
                  }
               }
               try {
                  timetag.setToString(lineT,fmtT);
               }
               catch(Exception& dte) {
                  logof << "ERROR: reading the receiver position flat file threw"
                     << " a DayTime exception:\n"
                     << "  This is the time format: " << fmtT << endl;
                  ok = have = havefmt = false;
                  break;
               }
               try {
                  pos.setToString(lineP,fmtP);
                  pos.transformTo(Position::Cartesian);
                  CurrRef.RxPos = pos;
               }
               catch(Exception& ge) {
                  logof << "ERROR: reading the receiver position flat file threw"
                     << " a Position exception:\n"
                     << "  This is the position format: " << fmtP << endl;
                  ok = have = havefmt = havepat = false;
               }
               if(ok) {
                  if(Debug)logof << "Result: t= " << timetag << " p= " << pos << endl;
                  RefPosMap[timetag] = CurrRef;
                  CurrRef.valid = true;
               }
               break;
            }
            if(!ok) break;
            getline(inf,line);
            stripTrailing(line,'\r');
         }
         inf.close();
         if(!have) {
            logof << "ERROR in reading receiver position file: ";
            if(!havefmt) logof << "format ";
            if(!havepat) {
               if(!havefmt) logof << "and pattern ";
               else logof << "pattern ";
            }
            logof << ((havepat || havefmt) ? "was " : "were ")
               << "wrong or not found!\n";
            logof << RxhelpString << endl;
            logof << "  [The input format is " << format << "]" << endl;
            logof << "  [The input pattern is " << pattern << "]" << endl;
            return -2;
         }
         inPS = 1;
      }  // end flat file input

      // compute the nominal time spacing of the map
      {
         const int ndtmax=15;
         double dt,bestdt[ndtmax];
         int j,k,nleast,ndt[ndtmax]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
         DayTime prev(DayTime::BEGINNING_OF_TIME);
         map<DayTime,RefPosData>::const_iterator it;

         if(Debug) logof << "Here is the reference position map\n";
         for(it=RefPosMap.begin(); it != RefPosMap.end(); it++) {
            if(Debug) logof << "   " << it->first << " " << fixed
                  << " " << setw(13) << setprecision(3) << it->second.RxPos.X()
                  << " " << setw(13) << setprecision(3) << it->second.RxPos.Y()
                  << " " << setw(13) << setprecision(3) << it->second.RxPos.Z()
                  << endl;
            if(prev != DayTime::BEGINNING_OF_TIME) {
               dt = it->first - prev;
               for(i=0; i<ndtmax; i++) {
                  if(ndt[i] <= 0) { bestdt[i]=dt; ndt[i]=1; break; }
                  if(fabs(dt-bestdt[i]) < 0.0001) { ndt[i]++; break; }
                  if(i == ndtmax-1) {
                     k = 0; nleast = ndt[k];
                     for(j=1; j<ndtmax; j++) if(ndt[j] <= nleast) {
                        k=j; nleast=ndt[j];
                     }
                     ndt[k]=1; bestdt[k]=dt;
                  }
               }
            }
            prev = it->first;
         }
         for(i=1,j=0; i<ndtmax; i++) if(ndt[i] > ndt[j]) j=i;
         RefPosMapDT = bestdt[j];
      }

   }  // end non-empty RefPosFile name

   else if(doRAIM) {
      // if(Debug) prsol.Debug = true; // write to cout ...
      prsol.Algebraic = false;
      //prsol.MaxNIterations = PIC.NIter;    // TD add to command line?
      //prsol.Convergence = PIC.Conv;
      // set inPS below, when you know you can do RAIM
      logof << "Reference position will come from RAIM\n";
   }
   else if(RefPosInput) {
      logof << "Reference position will come from the input file\n";
      inPS = 1;
   }
 
      // reset average RAIM solution
   if(headRAIM) {
      ARSX.Reset();
      ARSY.Reset();
      ARSZ.Reset();
   }

      // --------------------------------------------------------------------
      // misc
      // IonoHt used in meters
   IonoHt *= 1000.0;

      // search for SX,Y,Z input and set DoSX flag, also XR,XI,X1,X2 and DoXR
   DoSVX = DoXR = false;
   for(i=0; i<OTstrings.size(); i++) {
      if(OTstrings[i]==string("SX")
            || OTstrings[i]==string("SY")
            || OTstrings[i]==string("SZ")) DoSVX = true;
      if(OTstrings[i]==string("XR") || OTstrings[i]==string("XI")
            || OTstrings[i]==string("X1") || OTstrings[i]==string("X2")) DoXR = true;
   }

   if(DoXR) {
      int j;
      // transformation matrix is constant
      XRM0[0] = alpha+1;      XRM0[1] = -1;      XRM0[2] = 0;     XRM0[3] = 0;
      XRM1[0] = 1;            XRM1[1] = -1;      XRM1[2] = 0;     XRM1[3] = 0;
      XRM2[0] = -alpha-2;     XRM2[1] = 2;       XRM2[2] = alpha; XRM2[3] = 0;
      XRM3[0] = -2*(alpha+1); XRM3[1] = alpha+2; XRM3[2] = 0;     XRM3[3] = alpha;
      for(i=0; i<4; i++) for(j=0; j<4; j++) XRM[i][j] /= alpha;
      if(Debug) {
         logof << "XRM matrix is:\n" << fixed;
         for(i=0; i<4; i++) {
            for(j=0; j<4; j++) {
               logof << " " << setw(20) << setprecision(4) << XRM[i][j];
            }
            logof << endl;
         }
      }
   }

   CurrRef.valid = false;
   if(Debug) logof << "Return from PrepareInput" << endl;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
   return -1;
}

//------------------------------------------------------------------------------------
// after reading input header and before calling REC.EditHeader (pass input header)
int RCRinexEditor::BeforeEditHeader(const RinexObsHeader& rhin) throw(Exception)
{
try {
   int i;

      // save the header for later use by SaveData and ComputeNewOTs
   rhead = rhin;

      // -----------------------------------------------------------------------
      // get indexes of input obs types, for dependence checking and fast access
   for(i=0; i<rhin.obsTypeList.size(); i++) {
      if(rhin.obsTypeList[i] == RinexObsHeader::convertObsType("C1")) inC1=i;
      if(rhin.obsTypeList[i] == RinexObsHeader::convertObsType("L1")) inL1=i;
      if(rhin.obsTypeList[i] == RinexObsHeader::convertObsType("L2")) inL2=i;
      if(rhin.obsTypeList[i] == RinexObsHeader::convertObsType("P1")) inP1=i;
      if(rhin.obsTypeList[i] == RinexObsHeader::convertObsType("P2")) inP2=i;
      if(rhin.obsTypeList[i] == RinexObsHeader::convertObsType("D1")) inD1=i;
      if(rhin.obsTypeList[i] == RinexObsHeader::convertObsType("D2")) inD2=i;
      if(rhin.obsTypeList[i] == RinexObsHeader::convertObsType("S1")) inS1=i;
      if(rhin.obsTypeList[i] == RinexObsHeader::convertObsType("S2")) inS2=i;
   }

      // redefine inP1 based on inC1, Callow and Cforce
   if(Callow && inC1 > -1 && inP1 == -1) inP1=inC1;
   if(Cforce && inC1 > -1)               inP1=inC1;

      // -----------------------------------------------------------------------
      // Check dependences of input and output data types
      // -----------------------------------------------------------------------
      // check that we can do RAIM
   if(doRAIM) {
      if(inP1>-1 && inP2>-1) inPS=1;
      else {
         ostringstream stst;
         stst << "Error: cannot compute RAIM solution: missing";
         if(inP1 == -1) stst << " P1";
         if(inP2 == -1) stst << " P2";
         if(inEP == -1) stst << " EP";
         stst << "; abort.\n";
         logof << stst.str();
         oferr << stst.str();
         return -2;
      }
   }

      // -----------------------------------------------------------------------
      // Define bit flags for input data types
   unsigned int InputData=0;
   if(Verbose) logof << "Input data:\n";
   if(inP1 > -1) {
      InputData |= 0x08;
      if(Verbose) logof << " P1(" << inP1 << ")";
   }
   if(inP2 > -1) {
      InputData |= 0x10;
      if(Verbose) logof << " P2(" << inP2 << ")";
   }
   if(inL1 > -1) {
      InputData |= 0x02;
      if(Verbose) logof << " L1(" << inL1 << ")";
   }
   if(inL2 > -1) {
      InputData |= 0x04;
      if(Verbose) logof << " L2(" << inL2 << ")";
   }
   if(inEP > -1) {
      InputData |= RinexObsHeader::RinexObsType::EPdepend;
      if(Verbose) logof << " EP";
   }
   if(inPS > -1) {
      InputData |= 0x40;
      if(Verbose) logof << " PS";
   }
   if(Verbose) logof << "(" << hex << InputData << ")" << dec << endl;

      // -----------------------------------------------------------------------
      // create list OTList of RinexObsTypes here, for use later
      // check dependencies of requested output OTs
   if(Verbose) logof << "Here is the list of added OTs:";
   for(i=0; i<OTstrings.size(); i++) {
      if(Verbose) logof << " " << OTstrings[i];
      OTList.push_back(RinexObsHeader::convertObsType(OTstrings[i]));
   }
   if(Verbose) logof << endl;
   bool ok=true;
   for(i=0; i<OTList.size(); i++) {
      if((InputData & OTList[i].depend) != OTList[i].depend) {
         ostringstream stst;
         ok = false;
         stst << "ResCor Error: Abort: Output OT " << OTstrings[i]
            << " requires missing input:";
         unsigned int test=(InputData & OTList[i].depend);
         test ^= OTList[i].depend;
         if(test & rhin.obsTypeList[inL1].depend) stst << " L1";
         if(test & rhin.obsTypeList[inL2].depend) stst << " L2";
         if(test & rhin.obsTypeList[inP1].depend) stst << " P1";
         if(test & rhin.obsTypeList[inP2].depend) stst << " P2";
         if(test & RinexObsHeader::RinexObsType::EPdepend) stst << " EP";
         if(test & RinexObsHeader::RinexObsType::PSdepend) stst << " PS";
         stst << endl;
         logof << stst.str();
         oferr << stst.str();
      }
   }
   if(!ok) return -3;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// after calling REC.EditHeader (pass output header)
int RCRinexEditor::AfterEditHeader(const RinexObsHeader& rhout) throw(Exception)
{
try {
   int i,j;

      // save header for later use by SaveData
   rheadout = rhout;

      // -----------------------------------------------------------------------
      // define indexes of raw data in output header
   for(i=0; i<rhout.obsTypeList.size(); i++) {
      if(rhout.obsTypeList[i] == RinexObsHeader::convertObsType("C1")) otC1=i;
      if(rhout.obsTypeList[i] == RinexObsHeader::convertObsType("L1")) otL1=i;
      if(rhout.obsTypeList[i] == RinexObsHeader::convertObsType("L2")) otL2=i;
      if(rhout.obsTypeList[i] == RinexObsHeader::convertObsType("P1")) otP1=i;
      if(rhout.obsTypeList[i] == RinexObsHeader::convertObsType("P2")) otP2=i;
      if(rhout.obsTypeList[i] == RinexObsHeader::convertObsType("D1")) otD1=i;
      if(rhout.obsTypeList[i] == RinexObsHeader::convertObsType("D2")) otD2=i;
      if(rhout.obsTypeList[i] == RinexObsHeader::convertObsType("S1")) otS1=i;
      if(rhout.obsTypeList[i] == RinexObsHeader::convertObsType("S2")) otS2=i;
   }

      // redefine otP1 based on otC1, inP1, inC1, Callow and Cforce
   if(Callow && otC1 > -1 && inC1 > -1 && inP1 == -1) otP1=otC1;
   if(Cforce && otC1 > -1)                            otP1=otC1;

      // -----------------------------------------------------------------------
      // create a list of indexes parallel to OTstrings and OTList
   for(j=0; j<OTList.size(); j++) {
      for(i=0; i<rhout.obsTypeList.size(); i++) {
         if(rhout.obsTypeList[i] == OTList[j]) OTindex.push_back(i);
      }
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// after reading input obs and before calling EditObs (pass input obs)
int RCRinexEditor::BeforeEditObs(const RinexObsData& roin) throw(Exception)
{
try {
   if(Debug) logof << "\n----------------------------- " << roin.time
      << " ------------------------" << endl;

   // -----------------------------------------------------------------------
   // in-line header info
   // note that often these have a bad (all zeros) epoch
   if(roin.epochFlag != 0 && roin.epochFlag != 1) {
      if(Debug) logof << "Found in-line header (dump comments only)" << endl;
      //roin.auxHeader.dump(logof);
      for(int i=0; i<roin.auxHeader.commentList.size(); i++) {
         string s=roin.auxHeader.commentList[i];
         if(Debug) logof << s << endl;
         if(RefPosInput) {
            string t=stripFirstWord(s);
            if(t == string("XYZT")) {
               double x=asDouble(stripFirstWord(s));
               double y=asDouble(stripFirstWord(s));
               double z=asDouble(stripFirstWord(s));
               CurrRef.RxPos.setECEF(x,y,z);
               CurrRef.clk = asDouble(stripFirstWord(s));
            }
            else if(t==string("DIAG")) {
               CurrRef.NPRN = asInt(stripFirstWord(s));
               CurrRef.PDOP = asDouble(stripFirstWord(s));
               CurrRef.GDOP = asDouble(stripFirstWord(s));
               CurrRef.RMS = asDouble(stripFirstWord(s));
               CurrRef.valid = true;
//logof << "Found position:\n" << CurrRef.RxPos.printf("%.4x %.4y %.4z\n");
            }
         }
      }
      return 0;
   }
   
   // --------------------------------------------------------------------
   // Save the time tag (wait to define until after in-line header info)
   CurrentTime = roin.time;

   // --------------------------------------------------------------------
   // save the raw data, if they're not in the output
   DataStoreMap.clear();
   if((inL1>-1 && otL1==-1) || (inL2>-1 && otL2==-1) ||
      (inP1>-1 && (otP1==-1 || (Cforce && otC1==-1))) || (inP2>-1 && otP2==-1)) {
         SaveData(roin, rhead, inL1, inL2, inP1, inP2);
   }
   
   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// before writing out header (pass output header)
int RCRinexEditor::BeforeWritingHeader(RinexObsHeader& rhout) throw(Exception)
{
try {
   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// before writing out filled header (pass output header)
int RCRinexEditor::BeforeWritingFilledHeader(RinexObsHeader& rhout) throw(Exception)
{
try {
   if(headRAIM) {
         // put average RAIM position in header
      rhout.antennaPosition[0] = ARSX.Average();
      rhout.antennaPosition[1] = ARSY.Average();
      rhout.antennaPosition[2] = ARSZ.Average();
      rhout.valid |= RinexObsHeader::antennaPositionValid;
      if(Verbose) logof << "Average RAIM solution (" << ARSX.N()
         << ") at time " << CurrentTime << " : "
         << " " << fixed << setw(16) << setprecision(6) << ARSX.Average()
         << " +/- " << scientific << setw(8) << setprecision(2) << ARSX.StdDev()
         << ", " << fixed << setw(16) << setprecision(6) << ARSY.Average()
         << " +/- " << scientific << setw(8) << setprecision(2) << ARSY.StdDev()
         << ", " << fixed << setw(16) << setprecision(6) << ARSZ.Average()
         << " +/- " << scientific << setw(8) << setprecision(2) << ARSZ.StdDev()
         << endl;
   }

   if(Verbose) logof << "\nHere is the output header after optional records filled\n";
   rhout.dump(logof);

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// just before writing output obs (pass output obs)
// return value of BeforeWritingObs determines what is written:
// if return <0 abort
//            0 write nothing
//            1 write the obs data structure (note that if epochFlag==4,
//               this will result in in-line header information only)
//            2 write both header data (in auxHeader) and obs data
int RCRinexEditor::BeforeWritingObs(RinexObsData& roout) throw(Exception)
{
try {
   int i;
      // what to do with other epochFlags (in-line header information, etc)
   if(roout.epochFlag != 0 && roout.epochFlag != 1) return 0;

      // save the data, if they're in the output
   if(otL1>-1 || otL2>-1 || otP1>-1 || otP2>-1)
      SaveData(roout, rheadout, otL1, otL2, otP1, otP2);

      // update the receiver position (via RAIM or file input)
   if(UpdateRxPosition()) {
      logof << "Failed to update Rx position at time " << CurrentTime << endl;
      cerr << "Failed to update Rx position at time " << CurrentTime << endl;
      return -1;
   }

      // compute new OTs, and add to obs
   ComputeNewOTs(roout);

      // write RAIM position solution to in-line header
   if(outRef && (HaveRAIM || !RefPosFile.empty())) {
      ostringstream stst1,stst2;
      roout.auxHeader.clear();
      stst1 << "XYZT";
      stst1 << fixed << " " << setw(13) << setprecision(3) << CurrRef.RxPos.X();
      stst1 << fixed << " " << setw(13) << setprecision(3) << CurrRef.RxPos.Y();
      stst1 << fixed << " " << setw(13) << setprecision(3) << CurrRef.RxPos.Z();
      stst1 << fixed << " " << setw(13) << setprecision(3) << CurrRef.clk;
      roout.auxHeader.commentList.push_back(stst1.str());
      if(Verbose)
         logof << "RAIM output: " << roout.time.printf("%02M:%04.1f ") << stst1.str();

      //for(Nsvs=0,i=0; i<Sats.size(); i++) if(Sats[i].sat > 0) Nsvs++;
      //PDOP = RSS(prsol.Covariance(0,0),
      //      prsol.Covariance(1,1),prsol.Covariance(2,2));
      //GDOP = RSS(PDOP, prsol.Covariance(3,3));
      //rms = prsol.RMSResidual;
      stst2 << "DIAG";
      stst2 << " " << setw(2) << CurrRef.NPRN
         << " " << fixed << setw(5) << setprecision(2) << CurrRef.PDOP
         << " " << fixed << setw(5) << setprecision(2) << CurrRef.GDOP
         << " " << fixed << setw(9) << setprecision(3) << CurrRef.RMS
         << " (N,P-,G-Dop,RMS)";
      roout.auxHeader.commentList.push_back(stst2.str());
      if(Verbose) logof << " " << stst2.str() << endl;
      roout.auxHeader.valid |= RinexObsHeader::commentValid;

      return 4;         // write both header (with epochFlag=4) and obs data
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
void SaveData(const RinexObsData& rod, const RinexObsHeader& rhd,
   int xL1, int xL2, int xP1, int xP2) throw(Exception)
{
try {
   RinexSatID sat;
   RinexObsData::RinexObsTypeMap otmap;
   RinexObsData::RinexSatMap::const_iterator it;
   RinexObsData::RinexObsTypeMap::const_iterator jt;
   map<RinexSatID,RCData>::const_iterator kt;

   for(it=rod.obs.begin(); it != rod.obs.end(); ++it) { // loop over satellites
      sat = RinexSatID(it->first.id,SatID::systemGPS);
      otmap = it->second;
      // find the saved input data for this sat, if any
      kt = DataStoreMap.find(sat);
      if(kt != DataStoreMap.end()) DataStore=kt->second;

      if(xL1>-1 && (jt=otmap.find(rhd.obsTypeList[xL1])) != otmap.end()) {
         DataStore.L1 = jt->second.data;
         DataStore.LL1 = int(jt->second.lli);
      }
      if(xL2>-1 && (jt=otmap.find(rhd.obsTypeList[xL2])) != otmap.end()) {
         DataStore.L2 = jt->second.data;
         DataStore.LL2 = int(jt->second.lli);
      }
      if(xP1>-1 && (jt=otmap.find(rhd.obsTypeList[xP1])) != otmap.end()) {
         DataStore.P1 = jt->second.data;
      }
      if(xP2>-1 && (jt=otmap.find(rhd.obsTypeList[xP2])) != otmap.end()) {
         DataStore.P2 = jt->second.data;
      }
      DataStoreMap[sat] = DataStore;
   }  // end loop over sats
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// fill global data CurrRef
int UpdateRxPosition(void) throw(Exception)
{
try {
   int iret,i;
   double rho;
   Xvt xvt;
   RinexSatID sat;
   CorrectedEphemerisRange CER;

      // compute a RAIM solution, add it to average
   HaveRAIM = false;
   map<RinexSatID,RCData>::const_iterator kt;
   if(doRAIM) {
      Sats.clear();
      PRange.clear();

         //map<RinexSatID,RCData> DataStoreMap;
      for(kt=DataStoreMap.begin(); kt != DataStoreMap.end(); kt++) {
         if(kt->second.P1 == 0 || kt->second.P2 == 0) continue;
         sat = kt->first;
         if(minElev > 0.0 && CurrRef.valid) {
            try {
               if(SP3EphList.size() > 0)
                  rho = CER.ComputeAtReceiveTime(CurrentTime, xvt, sat,
                        SP3EphList);
               else if(BCEphList.size() > 0)
                  rho = CER.ComputeAtReceiveTime(CurrentTime, xvt, sat,
                        BCEphList);
               else continue;
            }
            catch(InvalidRequest& e) {
               continue;
            }
         }
         Sats.push_back(sat);
         PRange.push_back(if1r*kt->second.P1+if2r*kt->second.P2);
      }

      if(SP3EphList.size() > 0)
         iret = prsol.RAIMCompute(CurrentTime, Sats, PRange, SP3EphList, &ggtm);
      else if(BCEphList.size() > 0)
         iret = prsol.RAIMCompute(CurrentTime, Sats, PRange, BCEphList, &ggtm);
      else iret = -4;
         //  2  failed to find a good solution (RMS residual or slope exceed limits)
         //  1  solution is suspect (slope is large)
         //  0  ok
         // -1  failed to converge
         // -2  singular problem
         // -3  not enough good data to form a RAIM solution
         //     (the 4 satellite solution might be returned - check isValid())
         // -4  ephemeris not found for one or more satellites
      HaveRAIM = (iret==0 || iret==1);
      if(HaveRAIM) {
         if(Verbose) {                          // output results and return value
            int Nsvs;
            for(Nsvs=0,i=0; i<Sats.size(); i++) if(Sats[i].id > 0) Nsvs++;
            logof << "RPF " << setw(2) << Sats.size()-Nsvs
               << " " << setw(4) << CurrentTime.GPSfullweek() << fixed
               << " " << setw(10) << setprecision(3) << CurrentTime.GPSsecond()
               << " " << setw(2) << Nsvs
               << " " << setw(16) << setprecision(6) << prsol.Solution(0)
               << " " << setw(16) << setprecision(6) << prsol.Solution(1)
               << " " << setw(16) << setprecision(6) << prsol.Solution(2)
               << " " << setw(16) << setprecision(6) << prsol.Solution(3)
               << " " << setw(16) << setprecision(6) << prsol.RMSResidual
               << " " << fixed << setw(7) << setprecision(1) << prsol.MaxSlope
               << " " << prsol.NIterations
               << " " << scientific
               << setw(8) << setprecision(2) << prsol.Convergence;
            for(i=0; i<Sats.size(); i++) logof << " " << setw(3) << Sats[i].id;
            logof << " (" << iret << ")" << (prsol.isValid() ? " V":" NV") << endl;
         }

         CurrRef.RxPos.setECEF(prsol.Solution(0),prsol.Solution(1),
            prsol.Solution(2));
         CurrRef.valid = true;
         CurrRef.clk = prsol.Solution(3);
         CurrRef.NPRN = prsol.Nsvs;
         CurrRef.PDOP = RSS(prsol.Covariance(0,0),prsol.Covariance(1,1),
            prsol.Covariance(2,2));
         CurrRef.GDOP = RSS(CurrRef.PDOP, prsol.Covariance(3,3));
         CurrRef.RMS = prsol.RMSResidual;
         if(headRAIM) {       // add to average
            ARSX.Add(CurrRef.RxPos.X());
            ARSY.Add(CurrRef.RxPos.Y());
            ARSZ.Add(CurrRef.RxPos.Z());
         }
         inPS = 1;
      }
      else {                     // RAIM failed
         if(Verbose) {
            logof << "RAIM failed at " << CurrentTime << " : returned '";
            if(iret == 2) logof << "failed to find a good solution "
               << "(RMS residual or slope exceed limits)";
            if(iret == -1) logof << "failed to converge";
            if(iret == -2) logof << "singular problem";
            if(iret == -3) logof << "not enough good data to form a RAIM solution";
            if(iret == -4) {
               logof << "ephemeris not found for satellite";
               for(i=0; i<Sats.size(); i++) {
                  if(Sats[i].id < 0) {
                     Sats[i].id *= -1;
                     logof << " " << Sats[i];
                  }
               }
            }
            logof << "'." << endl;
         }
         inPS=-1;
      }
   }
   else if(!RefPosFile.empty()) { // update RxPos from map
      map<DayTime,RefPosData>::iterator ite;
      ite = RefPosMap.lower_bound(CurrentTime);
      // ite points to first element with value >= CurrentTime
      if(ite == RefPosMap.end() || fabs(ite->first - CurrentTime) > 0.1*RefPosMapDT) {
         if(Verbose) logof << "No Rx position found at " << CurrentTime << endl;
         CurrRef.valid = false;
         inPS = -1;
      }
      else {
         CurrRef.RxPos = ite->second.RxPos;
         CurrRef.clk = ite->second.clk;
         CurrRef.NPRN = ite->second.NPRN;
         CurrRef.PDOP = ite->second.PDOP;
         CurrRef.GDOP = ite->second.GDOP;
         CurrRef.RMS = ite->second.RMS;
         CurrRef.valid = true;
         inPS = 1;
      }
   }

   if(Debug && inPS > -1) {
      logof << "RxPos " << CurrentTime
         << " " << CurrentTime.printf("%04F %10.3g") << fixed << setprecision(3)
         << " " << setw(13) << CurrRef.RxPos.X()
         << " " << setw(13) << CurrRef.RxPos.Y()
         << " " << setw(13) << CurrRef.RxPos.Z()
         << endl;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void ComputeNewOTs(RinexObsData& rod) throw(Exception)
{
try {
   bool HaveR,HaveP,HaveEphRange,ok,reset,HaveEphThisSat;
   double rho,IPPLat,IPPLon,Obliq,Trop,Tgd;
   RinexObsData::RinexSatMap::iterator it;         // for loop over sats
   map<RinexSatID,RCData>::const_iterator kt;        // for DataStoreMap
   vector<RinexSatID> SVDelete;
   RinexSatID sat;
   //RinexObsData::RinexObsTypeMap otmap;
   CorrectedEphemerisRange CER;

   if(Debug) {
      logof << "Obs data before mods\n";
      rod.dump(logof);
   }

      // loop over sats
   for(it=rod.obs.begin(); it != rod.obs.end(); ++it) {
      sat = RinexSatID(it->first.id,SatID::systemGPS);
      //otmap = it->second; 

         // delete this satellite if it is excluded, or if RAIM has marked it
      if( (SVonly.id > 0 && sat != SVonly) ||
           (editRAIM && HaveRAIM &&
            find(Sats.begin(),Sats.end(),RinexSatID(-sat.id,sat.system))!=Sats.end()))
      {
         SVDelete.push_back(sat);
         continue;
      }

         // --------------------------------------------------------
         // find the saved input data for this sat
      kt = DataStoreMap.find(sat);
      HaveR = HaveP = false;
      if(kt != DataStoreMap.end()) {       // have data
         HaveR = (kt->second.P1 != 0.0 && kt->second.P2 != 0.0);
         HaveP = (kt->second.L1 != 0.0 && kt->second.L2 != 0.0);
      }
      if(doRAIM && !HaveRAIM) inPS=-1;
      
         // --------------------------------------------------------
         // compute ephemeris range and ionospheric pierce point
      if(inEP > -1) HaveEphThisSat=true;
      HaveEphRange = (HaveEphThisSat && inPS > -1);
      rho = IPPLat = IPPLon = Obliq = Tgd = 0;
      if(HaveEphRange) {
         Xvt xvt;
         xvt.x[0] = CurrRef.RxPos.X();
         xvt.x[1] = CurrRef.RxPos.Y();
         xvt.x[2] = CurrRef.RxPos.Z();
         try {
            if(SP3EphList.size() > 0)
               rho = CER.ComputeAtReceiveTime(CurrentTime, xvt, sat, SP3EphList);
            else if(BCEphList.size() > 0)
               rho = CER.ComputeAtReceiveTime(CurrentTime, xvt, sat, BCEphList);
            else {
               InvalidRequest e("No ephemeris in store");
               GPSTK_THROW(e);
            }
         }
         catch(InvalidRequest& e) {
            if(Verbose)
               logof << "ComputeNewOTs failed to find ephemeris for satellite "
               << sat << " at time " << CurrentTime << endl;
            HaveEphThisSat = false;
            HaveEphRange = false;
         }

         if(HaveEphRange) {
            if(minElev > 0.0 && CER.elevation < minElev) {
               HaveEphRange = HaveEphThisSat = false;
               SVDelete.push_back(sat);
            }
            else {
               Position IPP=CurrRef.RxPos.getIonosphericPiercePoint(
                  CER.elevation,CER.azimuth, IonoHt);
               IPPLat = IPP.geodeticLatitude();
               IPPLon = IPP.longitude();
               //Obliq = WGS84.a()*cos(CER.elevation*DEG_TO_RAD)/(WGS84.a()+IonoHt);
               Obliq = cos(CER.elevation*DEG_TO_RAD)/(1.0+IonoHt/WGS84.a());
               Obliq = ::sqrt(1.0-Obliq*Obliq);
                  // NB other trop models may require a different call,
                  // and will throw(InvalidTropModel) here
               Trop = ggtm.correction(CER.elevation);
               if(doTgd && BCEphList.size() > 0) {
                  const EngEphemeris& eph = BCEphList.findEphemeris(sat,CurrentTime);
                  Tgd = C_GPS_M * eph.getTgd();
               }
            }
         }
      }

         // --------------------------------------------------------
         // compute XR,XI,X1,X2
      if(DoXR && HaveR && HaveP) {
         XRdat[0] = wl1 * kt->second.L1;
         XRdat[1] = wl2 * kt->second.L2;
         XRdat[2] = kt->second.P1;
         XRdat[3] = kt->second.P2;
         for(int i=0; i<4; i++) {
            XRsol[i] = 0.0;
            for(int j=0; j<4; j++) {
               XRsol[i] += XRM[i][j] * XRdat[j];
            }
         }
      }

         // --------------------------------------------------------
         // get satellite position (if not found above)
      if(DoSVX && HaveEphThisSat && inPS == -1) {
         unsigned long ref;
         try {
            if(SP3EphList.size())
               CER.svPosVel = SP3EphList.getXvt(sat,CurrentTime);
            else
               CER.svPosVel = BCEphList.getXvt(sat,CurrentTime);
         }
         catch(InvalidRequest& e) {
            HaveEphThisSat = false;
         }
      }

         // --------------------------------------------------------
         // now loop over new output OTs, compute and debias them
      RinexObsData::RinexObsTypeMap::iterator jt;
      for(int i=0; i<OTList.size(); i++) {
         jt = it->second.find(OTList[i]);
         if(jt == it->second.end()) continue;   // error. throw?

         jt->second.data = 0.0;                 // default = marked bad
         ok = false;

         if(OTstrings[i] == string("ER")) {
            ok = HaveEphRange;
            if(ok) jt->second.data = rho;
         }
         else if(OTstrings[i] == string("RI")) {
            ok = HaveR;
            if(ok) jt->second.data = (kt->second.P2 - kt->second.P1)/alpha;
         }
         else if(OTstrings[i] == string("PI")) {
            ok = HaveP;
            if(ok) jt->second.data = (wl1*kt->second.L1 - wl2*kt->second.L2)/alpha;
         }
         else if(OTstrings[i] == string("TR")) {
            ok = HaveEphRange;
            if(ok) jt->second.data = Trop;
         }
         else if(OTstrings[i] == string("RL")) {
            ok = HaveEphThisSat;
            if(ok) jt->second.data = CER.relativity;
         }
         else if(OTstrings[i] == string("SC")) {
            ok = HaveEphThisSat;
            if(ok) jt->second.data = CER.svclkbias;
         }
         else if(OTstrings[i] == string("EL")) {
            ok = HaveEphRange;
            if(ok) jt->second.data = CER.elevation;
         }
         else if(OTstrings[i] == string("AZ")) {
            ok = HaveEphRange;
            if(ok) jt->second.data = CER.azimuth;
         }
         else if(OTstrings[i] == string("SR")) {
            ok = HaveR;
            if(ok) jt->second.data =
               (kt->second.P2 - kt->second.P1 - Tgd)*TECUperM/alpha;
         }
         else if(OTstrings[i] == string("SP")) {
            ok = HaveP;
            if(ok) {
               jt->second.data = removeBias(OTList[i], sat, reset, rod.time,
                  (wl1*kt->second.L1 - wl2*kt->second.L2 - Tgd)*TECUperM/alpha);
               if(reset) jt->second.lli |= 1;
            }
         }
         else if(OTstrings[i] == string("VR")) {
            ok = (HaveR && HaveEphRange);
            if(ok) jt->second.data =
               ((kt->second.P2 - kt->second.P1 - Tgd)*TECUperM/alpha)*Obliq;
         }
         else if(OTstrings[i] == string("VP")) {
            ok = (HaveP && HaveEphRange);
            if(ok) {
               jt->second.data = removeBias(OTList[i], sat, reset, rod.time,
                  ((wl1*kt->second.L1-wl2*kt->second.L2 - Tgd)*TECUperM/alpha)*Obliq);
               if(reset) jt->second.lli |= 1;
            }
         }
         else if(OTstrings[i] == string("LA")) {
            ok = HaveEphRange;
            if(ok) jt->second.data = IPPLat;
         }
         else if(OTstrings[i] == string("LO")) {
            ok = HaveEphRange;
            if(ok) jt->second.data = IPPLon;
         }
         else if(OTstrings[i] == string("P3")) {
            ok = HaveR;
            if(ok) jt->second.data = if1r*kt->second.P1 + if2r*kt->second.P2;
         }
         else if(OTstrings[i] == string("L3")) {
            ok = HaveP;
            if(ok) {
               jt->second.data = removeBias(OTList[i], sat, reset, rod.time,
                  if1p*kt->second.L1 + if2p*kt->second.L2);
               if(reset) jt->second.lli |= 1;
            }
         }
         else if(OTstrings[i] == string("PF")) {
            ok = HaveR;
            if(ok) jt->second.data = gf1r*kt->second.P1 + gf2r*kt->second.P2;
         }
         else if(OTstrings[i] == string("LF")) {
            ok = HaveP;
            if(ok) {
               jt->second.data = removeBias(OTList[i], sat, reset, rod.time,
                  gf1p*kt->second.L1 + gf2p*kt->second.L2);
               if(reset) jt->second.lli |= 1;
            }
         }
         else if(OTstrings[i] == string("PW")) {
            ok = HaveR;
            if(ok) jt->second.data = wl1r*kt->second.P1 + wl2r*kt->second.P2;
         }
         else if(OTstrings[i] == string("LW")) {
            ok = HaveP;
            if(ok) {
               jt->second.data = removeBias(OTList[i], sat, reset, rod.time,
                  wl1p*kt->second.L1 + wl2p*kt->second.L2);
               if(reset) jt->second.lli |= 1;
            }
         }
         else if(OTstrings[i] == string("MP") || OTstrings[i] == string("M3")) {
            ok = (HaveP && HaveR);
            if(ok) {
               jt->second.data = removeBias(OTList[i], sat, reset, rod.time,
                  if1r*kt->second.P1 + if2r*kt->second.P2
                  - (if1p*kt->second.L1 + if2p*kt->second.L2));
               if(reset) jt->second.lli |= 1;
            }
         }
         else if(OTstrings[i] == string("R1")) {
            ok = (kt->second.P1 != 0 && kt->second.L1 != 0);
            if(ok) jt->second.data = 0.5*(kt->second.P1 + kt->second.L1);
         }
         else if(OTstrings[i] == string("R2")) {
            ok = (kt->second.P2 != 0 && kt->second.L2 != 0);
            if(ok) jt->second.data = 0.5*(kt->second.P2 + kt->second.L2);
         }
         else if(OTstrings[i] == string("M1")) {
            ok = (kt->second.P1 != 0 && kt->second.L1 != 0);
            if(ok) {
               jt->second.data = removeBias(OTList[i], sat, reset, rod.time,
                  kt->second.P1 - wl1*kt->second.L1);
               if(reset) jt->second.lli |= 1;
            }
         }
         else if(OTstrings[i] == string("M2")) {
            ok = (kt->second.P2 != 0 && kt->second.L2 != 0);
            if(ok) {
               jt->second.data = removeBias(OTList[i], sat, reset, rod.time,
                  kt->second.P2 - wl2*kt->second.L2);
               if(reset) jt->second.lli |= 1;
            }
         }
         // M3 is MP
         else if(OTstrings[i] == string("M4")) {
            ok = (HaveP && HaveR);
            if(ok) {
               jt->second.data = removeBias(OTList[i], sat, reset, rod.time,
                  gf1r*kt->second.P1 + gf2r*kt->second.P2
                  - (gf1p*kt->second.L1 + gf2p*kt->second.L2));
               if(reset) jt->second.lli |= 1;
            }
         }
         else if(OTstrings[i] == string("M5")) {
            ok = (HaveP && HaveR);
            if(ok) {
               jt->second.data = removeBias(OTList[i], sat, reset, rod.time,
                  wl1r*kt->second.P1 + wl2r*kt->second.P2
                  - (wl1p*kt->second.L1 + wl2p*kt->second.L2));
               if(reset) jt->second.lli |= 1;
            }
         }
         else if(OTstrings[i] == string("XR")) {
            ok = HaveR && HaveP;
            if(ok) {
               jt->second.data = XRsol[0];
               if(reset) jt->second.lli |= 1;
            }
         }
         else if(OTstrings[i] == string("XI")) {
            ok = HaveR && HaveP;
            if(ok) {
               jt->second.data = removeBias(OTList[i], sat, reset, rod.time,XRsol[1]);
               if(reset) jt->second.lli |= 1;
            }
         }
         else if(OTstrings[i] == string("X1")) {
            ok = HaveR && HaveP;
            if(ok) {
               jt->second.data = removeBias(OTList[i], sat, reset, rod.time,XRsol[2]);
               if(reset) jt->second.lli |= 1;
            }
         }
         else if(OTstrings[i] == string("X2")) {
            ok = HaveR && HaveP;
            if(ok) {
               jt->second.data = removeBias(OTList[i], sat, reset, rod.time,XRsol[3]);
               if(reset) jt->second.lli |= 1;
            }
         }
         else if(OTstrings[i] == string("SX")) {
            ok = HaveP && HaveEphThisSat;
            if(ok) jt->second.data = CER.svPosVel.x[0];
         }
         else if(OTstrings[i] == string("SY")) {
            ok = HaveP && HaveEphThisSat;
            if(ok) jt->second.data = CER.svPosVel.x[1];
         }
         else if(OTstrings[i] == string("SZ")) {
            ok = HaveP && HaveEphThisSat;
            if(ok) jt->second.data = CER.svPosVel.x[2];
         }
         else ok = false;

         if(!ok) continue;

         // --------------------------------------------------------
         // set LLI flag, if it depends on phase, and if phase LLI is set
         unsigned int test=0;
         if(inL1 > -1) test=rhead.obsTypeList[inL1].depend;
         else if(otL1 > -1) test=rhead.obsTypeList[otL1].depend;
         if((OTList[i].depend & test) && (kt->second.LL1 & 0x01))
            jt->second.lli |= 1;
         test = 0;
         if(inL2 > -1) test=rhead.obsTypeList[inL2].depend;
         else if(otL2 > -1) test=rhead.obsTypeList[otL2].depend;
         if((OTList[i].depend & test) && (kt->second.LL2 & 0x01))
            jt->second.lli |= 1;

         //if(ok && Verbose) ;  // TD output here

      }  // end loop over new output OTs

      // --------------------------------------------------------
      // delete this satellite if there is no good data in it
      for(jt=it->second.begin(); jt != it->second.end(); jt++) {
         if(jt->second.data != 0.0) break;
      }
      if(jt == it->second.end()) SVDelete.push_back(sat);

   }  // end loop over sats

      // delete satellites
   for(int i=0; i<SVDelete.size(); i++) {
      rod.obs.erase(RinexSatID(SVDelete[i].id,SatID::systemGPS));
      rod.numSvs--;
   }

   if(Debug) {
      logof << "Obs data after mods\n";
      rod.dump(logof);
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}
//------------------------------------------------------------------------------------//------------------------------------------------------------------------------------
// NB reentrant, but ugly, function
// NB PreProcessArgs pulls out --debug --verbose and the -f<f> and --file <f> options.
// NB It also allow --ROThelp and --REChelp to have any case
void PreProcessArgs(const char *arg, vector<string>& Args) throw(Exception)
{
try {
   static bool found_cfg_file=false;

   if(found_cfg_file || (arg[0]=='-' && arg[1]=='f')) {
      string filename(arg);
      if(!found_cfg_file) filename.erase(0,2); else found_cfg_file = false;
      if(Debug) cout << "Found a file of options: " << filename << endl;
      ifstream infile(filename.c_str());
      if(!infile) {
         cout << "Error: could not open options file " << filename << endl;
         return;
      }
      bool again_cfg_file=false;
      char c;
      string buffer,word;
      while(1) {
         getline(infile,buffer);
         stripTrailing(buffer,'\r');

         while(!buffer.empty()) {
            word = firstWord(buffer);
            if(again_cfg_file) {
               word = "-f" + word;
               again_cfg_file = false;
               PreProcessArgs(word.c_str(),Args);
            }
            else if(word[0] == '#') {         // skip to end of line
               buffer = "";
            }
            else if(word == "--file")
               again_cfg_file = true;
            else if(word[0] == '"') {
               word = stripFirstWord(buffer,'"');
               buffer = "dummy " + buffer;      // to be stripped below
               PreProcessArgs(word.c_str(),Args);
            }
            else
               PreProcessArgs(word.c_str(),Args);

            word = stripFirstWord(buffer);      // now remove it from buffer
         }
         // check this last b/c there can be a line at EOF without CRLF...
         if(infile.eof() || !infile.good()) break;
      }
   }
   else if(string(arg)==string("--verbose"))
      Verbose = true;
   else if(string(arg)==string("--debug"))
      Debug = true;
   else if(string(arg)==string("--file"))
      found_cfg_file = true;
   else if(lowerCase(string(arg)) == string("--rothelp"))
      Args.push_back(string("--ROThelp"));
   else if(lowerCase(string(arg)) == string("--rechelp"))
      Args.push_back(string("--REChelp"));
   else if(lowerCase(string(arg)) == string("--rxhelp"))
      Args.push_back(string("--Rxhelp"));
   else
      Args.push_back(arg);
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
// define the bias limit, assign it to the invalid (-1,GPS) satellite
int setBiasLimit(RinexObsHeader::RinexObsType& ot, double lim) throw(Exception)
{
try {
   if(RinexObsHeader::convertObsType(ot)==string("UN") || lim <= 0.0) return -1;
   RinexSatID p;          // invalid: -1,GPS ... let this hold the LIMIT in the map
   map<RinexObsHeader::RinexObsType,map<RinexSatID,double> >::iterator it;
   if( (it=AllBiases.find(ot)) == AllBiases.end()) {     // not found
      map<RinexSatID,double> bm;
      bm[p] = lim;
      AllBiases[ot] = bm;
      if(Verbose) logof << "Set bias for " << RinexObsHeader::convertObsType(ot)
         << "," << p << " to " << fixed << setprecision(3) << lim << endl;
   }
   else {                                                // found
      it->second[p] = lim;
      if(Verbose) logof << "Re-Set bias for " << RinexObsHeader::convertObsType(ot)
         << "," << p << " to " << fixed << setprecision(3) << lim << endl;
   }
   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// set bias, if necessary, and return raw-bias
double removeBias(const RinexObsHeader::RinexObsType& ot, const RinexSatID& sv,
   bool& rset, DayTime& tt, double raw) throw(Exception)
{
try {
   rset = false;
   // is the input valid?
   if(RinexObsHeader::convertObsType(ot)==string("UN") || sv.id==-1) return raw;

   // get the map<RinexSatID,double> for this OT
   map<RinexObsHeader::RinexObsType,map<RinexSatID,double> >::iterator it;
   if( (it=AllBiases.find(ot)) == AllBiases.end()) return raw; // did not find OT
   // it->second is the right map<RinexSatID,double>

   // get the limit
   RinexSatID p;
   map<RinexSatID,double>::iterator jt;
   jt = it->second.find(p);                  // p is (-1,GPS) here, so bias=limit
   if(jt == it->second.end()) return raw;    // should never happen - throw?
   double limit=jt->second;

   // now find the current bias for the input satellite
   double bias;
   if( (jt=it->second.find(sv)) == it->second.end()) {   // sat not found, define bias
      bias = it->second[sv] = raw-0.001;
      if(Verbose) logof << "Did not find a bias for "
         << RinexObsHeader::convertObsType(ot) << "," << sv
         << " at time " << tt.printf("%4F %10.3g = %4Y/%02m/%02d %02H:%02M:%02S")
         << ", set it to " << fixed << setprecision(3) << bias << endl;
      rset = true;
   }
   else {                                                      // found the sat
      bias = jt->second;
      // logof << "Found bias for " << RinexObsHeader::convertObsType(ot)
      // << "," << sv << " = " << fixed << setprecision(3) << bias << endl;
      if(fabs(raw-jt->second) > limit) {
         if(Verbose) logof << "Bias limit for " << RinexObsHeader::convertObsType(ot)
            << "," << sv << " was exceeded at time "
            << tt.printf("%4F %10.3g = %4Y/%02m/%02d %02H:%02M:%02S")
            << " (" << fixed << setprecision(3) << raw-jt->second
            << " > " << setprecision(3) << limit
            << "), set it to " << fixed << setprecision(3) << raw-0.001 << endl;
         bias = it->second[sv] = raw-0.001;
         rset = true;
      }
   }

   return raw-bias;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
