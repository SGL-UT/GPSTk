#pragma ident "$Id: $"

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

/**
 * @file CommandInput.cpp
 * Implement command line input, including defaults and validation for program DDBase.
 */

//------------------------------------------------------------------------------------
// TD CommandInput.cpp need baseline identifier to Tight and Loose cmds
// TD CommandInput.cpp test reasonableness of input station positions

//------------------------------------------------------------------------------------
// includes
// system

// GPSTk
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"

// DDBase
#include "DDBase.hpp"
// DDBase.hpp includes CommandInput.hpp

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
// local data

//------------------------------------------------------------------------------------
void CommandInput::SetDefaults()
{
try {
   Debug = false;
   Verbose = false;
   Screen = true;            // TD user input
   Validate = false;
      // log file
   LogFile = string("ddbase.log");
      // input data files
   InputPath = string("");
   NavPath = string("");
   NavFileNames.clear();
   EOPPath = string("");
   EOPFileNames.clear();
      // time limits
   BegTime = DayTime::BEGINNING_OF_TIME;
   EndTime = DayTime::END_OF_TIME;
      // process configuration
   Frequency = 1;
      // for pseudorange solution
   PRSrmsLimit = 6.5;                     // this is the PRSolution() default
   PRSalgebra = false;
   PRSnIter = 10;
   PRSconverge = 1.e-9;
   PRSMinElevation = 10.0;
      // for modeling residual zenith delay
   NRZDintervals = 0;
   RZDtimeconst = 2.0;                    // hours
   RZDsigma = 0.5;                        // meters
      //
   DataInterval = -1.0;
      // editing
   MinElevation = 10.0;
   RotatedAntennaElevation = 0.0;
   RotatedAntennaAzimuth = 0.0;
   MaxGap = 10;
   MinDDSeg = 50;
   PhaseBiasReset = 10;
   ExSV.clear();
      // timetable
   RefSat = GSatID(-1,SatID::systemGPS);
      // estimation
   noEstimate = false;                    // for Estimation()
   nIter = 5;                             // for Estimation()
   convergence = 5.0e-8;                  // TD convergence criterion input
   noRAIM = false;                        // turn off pseudorange solution (! -> clk?)
   FixBiases = false;
   // Don't implement default constraints - this needs more study
   TightConstraint = 1.e-4; // 1.e-5;
   LooseConstraint = 1.e-1; // 1.e-1;
   DefaultTemp = 20.0;                    // deg C
   DefaultPress = 1010.0;                 // mbars at sea level
   DefaultRHumid = 50.0;                  // %
      // output
   OutputClkFile = string("");
   OutputDDDFile = string("");
   OutputTDDFile = string("");
   OutputRawFile = string("");
   OutputRawDDFile = string("");
   OutputPRSFile = string("");
   OutputDDRFile = string("");
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int CommandInput::GetCmdInput(int argc, char **argv)
{
try {
   help = false;
   int i,j;
   string msg;
   vector<string> values,field;

      // set all to default
   SetDefaults();

   // --------------------------------------------------------------------------------
   // Define the options

   // required options:

   // optional options:

   // this is here only so it will show up in the help msg...
   CommandOption dashf(CommandOption::hasArgument, CommandOption::stdType,
      'f',"","\n -f<file>              Name of file containing more options"
      " ('#' to EOL : comment)");

   // log file
   CommandOption dashl(CommandOption::hasArgument, CommandOption::stdType,
      0,"Log"," --Log <file>          Name of output log file (" + LogFile + ")");
   dashl.setMaxCount(1);
   
   // files
   // observation
   CommandOption dashop(CommandOption::hasArgument, CommandOption::stdType,
      0,"ObsPath",
      "\n# Observations:\n --ObsPath <path>      Path for input obs file(s) (.)");
   dashop.setMaxCount(1);

   CommandOption dashof(CommandOption::hasArgument, CommandOption::stdType,
      0,"ObsFile"," --ObsFile <name,id>   Rinex observation file name(s),"
      " followed by a station label.");

   // ephemeris
   CommandOption dashnp(CommandOption::hasArgument, CommandOption::stdType, 0,
      "NavPath","# Ephemeris and Earth orientation:\n"
      " --NavPath <dir>       Path of navigation file(s) (.)");
   dashnp.setMaxCount(1);

   CommandOption dashnf(CommandOption::hasArgument, CommandOption::stdType,
      0,"NavFile"," --NavFile <file>      Navigation (Rinex Nav OR SP3) file(s)");

   // earth orientation
   CommandOption dashep(CommandOption::hasArgument, CommandOption::stdType, 0,
      "EOPPath"," --EOPPath <dir>       "
      "Path of earth orientation file(s)");
   dashep.setMaxCount(1);

   CommandOption dashef(CommandOption::hasArgument, CommandOption::stdType,
      0,"EOPFile"," --EOPFile <file>      "
      "Earth orientation parameter (EOPP or IERS format) file(s).\n"
      "                        If no EOP file is given, DDBase will search "
      "for the IERS\n                        format file 'finals.daily' in the "
      "current directory.");

   // station configuration
   CommandOption dashXYZ(CommandOption::hasArgument, CommandOption::stdType,
      0,"PosXYZ",
      "\n# Station configuration [--Pos.. (1 only) MUST be given for each site]:\n"
      " --PosXYZ <X,Y,Z,id>   Station position in ECEF coordinates (m),\n"
      "                         followed by a label identifying the station.");

   CommandOption dashLLH(CommandOption::hasArgument, CommandOption::stdType,
      0,"PosLLH"," --PosLLH <La,Lo,H,id> Station position in geodetic coordinates:\n"
      "                         Latitude(deg),Longitude(E,deg),Height(m),label");

   CommandOption dashPRS(CommandOption::hasArgument, CommandOption::stdType,
      0,"PosPRS"," --PosPRS <id>         Let position of station labelled <id> be set"
      " to the computed\n                         average pseudorange solution"
      " for that site.");

   CommandOption dashtrop(CommandOption::hasArgument, CommandOption::stdType,
      0,"TropModel"," --TropModel <trop,id> Use trop model <trop> for station <id>, "
      "choices are: 'Zero',\n                        'Black','NewB','ModHop',"
      "'ModHopH','Saas' (Saas) [cf. GPSTk]");

   msg = string(
      " --Weather <T,P,H,id>  Weather parameters: Temperature(degC),Pressure(mbar),\n"
      "                         Humidity(%), followed by a label identifying the\n"
      "                         station. ("
            + StringUtils::asString(CI.DefaultTemp,1) + string(",")
            + StringUtils::asString(CI.DefaultPress,2) + string(",")
            + StringUtils::asString(CI.DefaultRHumid,1) + string(")")
      );
   CommandOption dashWx(CommandOption::hasArgument, CommandOption::stdType,
      0,"Weather",msg);

   CommandOption dashfix(CommandOption::hasArgument, CommandOption::stdType,
      0,"Fix"," --Fix <id>            Hold the station <id> fixed "
      "in estimation (don't)");

   //CommandOption dashant(CommandOption::hasArgument, CommandOption::stdType,
   //   0,"AntAz"," --AntAz <angle,id>    "
   //   "Antenna relative azimuth angle (deg) for station <id> (0.0)");

   // configuration
   CommandOptionNoArg dashnoest(0, "noEstimate", "\n# Configuration:\n"
      " --noEstimate          Quit before performing the estimation.");
   dashnoest.setMaxCount(1);

   CommandOption dashfreq(CommandOption::hasArgument, CommandOption::stdType,
      0,"Freq"," --Freq <L1|L2|L3>     Process L1, L2 or L3(L1+L2) frequency data"
      " (L3 not validated)");
   dashfreq.setMaxCount(1);

   CommandOption dashnit(CommandOption::hasArgument, CommandOption::stdType,
      0,"nIter"," --nIter <n>           Maximum number of estimation iterations ("
      + StringUtils::asString(nIter) + ")");
   dashnit.setMaxCount(1);

   {
      ostringstream oss;
      oss << scientific << setprecision(2) << convergence;
      msg = oss.str();
   }
   CommandOption dashconv(CommandOption::hasArgument, CommandOption::stdType,
      0,"Converge"," --Converge <cl>       Convergence limit on RSS change in state ("
      + msg + " m)");
   dashconv.setMaxCount(1);

   CommandOptionNoArg dashfixbias(0, "FixBiases", " --FixBiases           "
      "Perform an extra, last iteration that fixes the phase biases");
   dashfixbias.setMaxCount(1);

   // state model
   CommandOption dashntrop(CommandOption::hasArgument, CommandOption::stdType,
      0,"RZDnIntervals","\n# State model, a priori constraints:\n"
      " --RZDnIntervals <n>   Number of (equal time) residual zenith delay "
      "intervals (" + StringUtils::asString(NRZDintervals) + ")\n"
      "                         (enter 0 to turn off estimation of RZD)");
   dashntrop.setMaxCount(1);
   
   CommandOption dashttrop(CommandOption::hasArgument, CommandOption::stdType,
      0,"RZDtimeconst",
      " --RZDtimeconst <tau>  Time constant (hours) for multiple RZD intervals ("
      + StringUtils::asString(RZDtimeconst,2) + ")");
   dashttrop.setMaxCount(1);
   
   CommandOption dashstrop(CommandOption::hasArgument, CommandOption::stdType,
      0,"RZDsigma",
      " --RZDsigma <sig>      A priori sigma (m) for residual zenith delay ("
      + StringUtils::asString(RZDsigma,2) + ")");
   dashstrop.setMaxCount(1);
   
   // TD need baseline identifier: --Tight <id,id,ppm>. also Loose
   CommandOption dashtight(CommandOption::hasArgument, CommandOption::stdType,
      0,"Tight"," --Tight <ppm>         Tight a priori constraint, a fraction "
      "of baseline (" + StringUtils::asString(TightConstraint,4) + ")");
   dashtight.setMaxCount(1);

   CommandOption dashloose(CommandOption::hasArgument, CommandOption::stdType,
      0,"Loose"," --Loose <ppm>         Loose a priori constraint, a fraction "
      "of baseline ("+StringUtils::asString(LooseConstraint,1)+")");
   dashloose.setMaxCount(1);

   // times - don't use CommandOptionWithTimeArg
   CommandOption dashbt(CommandOption::hasArgument, CommandOption::stdType,
      0,"BeginTime",
      "\n# Time limits:\n --BeginTime <arg>     Start time: arg is "
      "'GPSweek,sow' OR 'YYYY,MM,DD,HH,Min,Sec'");
   dashbt.setMaxCount(1);

   CommandOption dashet(CommandOption::hasArgument, CommandOption::stdType,
      0,"EndTime",
      " --EndTime <arg>       End time: arg is 'GPSweek,sow' OR "
      "'YYYY,MM,DD,HH,Min,Sec'");
   dashet.setMaxCount(1);

   // time table(s)
   CommandOption dashttab(CommandOption::hasArgument, CommandOption::stdType,
      0,"TimeTable","\n# Satellite time table:\n"
      " --TimeTable <file>    Time table file name (if this option does not appear"
      "\n                          a time table will be computed and output to log"
      " file)");
   dashttab.setMaxCount(1);

   CommandOption dashRef(CommandOption::hasArgument, CommandOption::stdType, 0,
      "Ref", " --Ref <sat>           Use <sat> as 'reference' "
      "in DDs; don't use a timetable");
   dashRef.setMaxCount(1);
   
   // data editing
   CommandOption dashelev(CommandOption::hasArgument, CommandOption::stdType,
      0,"MinElev","\n# Data editing:\n --MinElev <elev>      Ignore data "
      "below elevation <elev> degrees, DDs only ("
      + StringUtils::asString(MinElevation,2) + ")");
   dashelev.setMaxCount(1);

   CommandOption dashrotelev(CommandOption::hasArgument, CommandOption::stdType,
      0,"AntRotElev"," --AntRotElev <elev>   Apply MinElev to antenna rotated "
      "in elevation by <elev> deg.");
   dashrotelev.setMaxCount(1);

   CommandOption dashrotaz(CommandOption::hasArgument, CommandOption::stdType,
      0,"AntRotAz"," --AntRotAz <az>       Apply MinElev to antenna rotated "
      "in azimuth by <az> deg.");
   dashrotaz.setMaxCount(1);

   CommandOption dashgap(CommandOption::hasArgument, CommandOption::stdType, 0,
      "MaxGap"," --MaxGap              Maximum acceptable gap in data "
      "[number of --DT intervals] (" + StringUtils::asString(MaxGap) + ")\n"
      "                          [Used in raw data editing and synchronization]");
   dashgap.setMaxCount(1);

   CommandOption dashmindd(CommandOption::hasArgument, CommandOption::stdType, 0,
      "MinDDSeg"," --MinDDSeg            Minimum acceptable length of DD data segment"
      " (" + StringUtils::asString(MinDDSeg) + ")");
   dashmindd.setMaxCount(1);

   CommandOption dashphbias(CommandOption::hasArgument, CommandOption::stdType, 0,
      "PhaseBiasReset"," --PhaseBiasReset      Limit on pt-to-pt change in pha"
      "se without reset, in cycles (" + StringUtils::asString(PhaseBiasReset) + ")");
   dashphbias.setMaxCount(1);

   CommandOption dashXsat(CommandOption::hasArgument, CommandOption::stdType, 0,
      "XSat", " --XSat <sat>          Exclude this satellite ()");

   CommandOption dashDT(CommandOption::hasArgument, CommandOption::stdType, 0,
      "DT"," --DT <t>              Data time interval in sec [will also decimate"
      " input data]");
   dashDT.setMaxCount(1);

   // pseudorange solution
   CommandOption dashprsnit(CommandOption::hasArgument, CommandOption::stdType,
      0,"PRSniter","\n# Pseudorange solution (PRS) configuration:\n"
      " --PRSniter <n>        PRS: Limit on number of iterations ("
      + StringUtils::asString(PRSnIter) + ")");
   dashprsnit.setMaxCount(1);

   {
      ostringstream oss;
      oss << scientific << setprecision(2) << PRSconverge;
      msg = oss.str();
   }
   CommandOption dashprscon(CommandOption::hasArgument, CommandOption::stdType,
      0,"PRSconverge",
      " --PRSconverge <cl>    PRS: Convergence limit (m) (" + msg + ")");
   dashprscon.setMaxCount(1);

   CommandOption dashprsrms(CommandOption::hasArgument, CommandOption::stdType,
      0,"PRSrmsLimit",
      " --PRSrmsLimit <rms>   PRS: RMS residual limit (m) ("
      + StringUtils::asString(PRSrmsLimit,2) + ")");
   dashprsrms.setMaxCount(1);

   CommandOption dashprsalg(CommandOption::hasArgument, CommandOption::stdType,
      0,"PRSalgebra",
      " --PRSalgebra          PRS: Use algebraic algorithm (don't)");
   dashprsalg.setMaxCount(1);

   CommandOption dashprselev(CommandOption::hasArgument, CommandOption::stdType,
      0,"PRSMinElev"," --PRSMinElev <elev>   PRS: Reject data below elevation "
      "<elev> degrees ("
      + StringUtils::asString(PRSMinElevation,2) + ")");
   dashprselev.setMaxCount(1);

   //dont CommandOptionNoArg dashnoprs(0, "noPRS",
      //dont " --noPRS               Skip the pseudorange solution (!)");
   //dont dashnoprs.setMaxCount(1);

   // output flags
   CommandOption dashrawout(CommandOption::hasArgument, CommandOption::stdType,
      0,"RAWFileOut","\n# Output files:\n --RAWFileOut <file>   "
      "Filename for output of raw data ()");
   dashrawout.setMaxCount(1);

   CommandOption dashprsout(CommandOption::hasArgument, CommandOption::stdType,
      0,"PRSFileOut"," --PRSFileOut <file>   "
      "Filename for output of pseudorange solution ()");
   dashprsout.setMaxCount(1);

   CommandOption dashclkout(CommandOption::hasArgument, CommandOption::stdType,
      0,"CLKFileOut"," --CLKFileOut <file>   "
      "Filename for output of Rx clock bias and model ()");
   dashclkout.setMaxCount(1);

   CommandOption dashrddout(CommandOption::hasArgument, CommandOption::stdType,
      0,"RDDFileOut",
      " --RDDFileOut <file>   Filename for output of raw DD data ()");
   dashrddout.setMaxCount(1);

   CommandOption dashdddout(CommandOption::hasArgument, CommandOption::stdType,
      0,"DDDFileOut"," --DDDFileOut <file>   Filename for output of (edited) DD data"
      " ()");
   dashdddout.setMaxCount(1);

   CommandOption dashtddout(CommandOption::hasArgument, CommandOption::stdType,
      0,"TDDFileOut"," --TDDFileOut <file>   Filename for output of triple "
      "difference data ()");
   dashtddout.setMaxCount(1);

   CommandOption dashddrout(CommandOption::hasArgument, CommandOption::stdType,
      0,"DDRFileOut"," --DDRFileOut <file>   Filename for output of DD post-fit "
      "residuals ()");
   dashddrout.setMaxCount(1);

   CommandOption dashbaseout(CommandOption::hasArgument, CommandOption::stdType,
      0,"BaseOut","\n# Output misc:\n --BaseOut <id-id,x,y,z> Baseline to output;"
      " <id>s are station labels, '-' is\n                          "
      "required, <x,y,z> are optional baseline coordinates.");

   CommandOptionNoArg dashvalid('0', "validate",
         " --validate            Read input and validate it, then quit.");
   dashvalid.setMaxCount(1);

   CommandOptionNoArg dashv('v', "verbose",
         " --verbose             (also -v) print extended output info.");

   CommandOptionNoArg dashd('d', "debug",
         " --debug               (also -d) print very extended output info "
         "(for developers).");

   CommandOptionNoArg dashh('h', "help",
         " --help                (also -h) print this help message and quit.");

   // ... other options
   CommandOptionRest Rest("");

   // --------------------------------------------------------------------------------
   // Define the parser here: before the options -- this is the "prgm description" ...
   CommandOptionParser Par(PrgmDesc);

   // parse the command line
   // allow user to put all options in a file
   // PreProcessArgs pulls out help, Debug, Verbose
   vector<string> Args;
   for(j=1; j<argc; j++) PreProcessArgs(argv[j],Args);
   argc = Args.size();
   if(argc==0)
      help = true;

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

   if(Debug) {
      cout << "Argument list passed to parser:\n";
      for(j=0; j<argc; j++) cout << j << " " << CArgs[j] << endl;
   }

   Par.parseOptions(argc, CArgs);
   for(j=1; j<argc; j++) delete[] CArgs[j];
   delete[] CArgs;

   // check for errors on the command line
   // hasErrors() returns invalid commands
   // Rest contains things not recognized
   if (Par.hasErrors() || Rest.getCount()) {
      cerr << "\nErrors found in command line input:\n";
      if(Par.hasErrors()) Par.dumpErrors(cerr);
      if(Rest.getCount()) {
         cerr << "The following command line fields were not recognized:\n";
         values = Rest.getValue();
         for(i=0; i<values.size(); i++) cerr << " " << values[i] << endl;
      }
      cerr << "...end of Errors\nAbort.\n";
      help = true;
   }

   // if help, print usage
   if(help) {
      Par.displayUsage(cout,false);
      cout << endl;
   }

   if(help && argc > 1) cout << endl << "--------- parsed input:" << endl;

   // --------------------------------------------------------------------------------
   // pull out the parsed input

   // these already parsed by PreProcessArgs
   //if(dashh.getCount()) help
   //if(dashv.getCount()) verbose
   //if(dashd.getCount()) debug

   if(help && Verbose) cout << " Input: turn on verbose flag" << endl;
   if(help && Debug) cout << " Input: turn on debug flag" << endl;

      // open the log file first
   if(dashl.getCount()) {
      values = dashl.getValue();
      LogFile = values[0];
      if(help) cout << " Input: log file name " << LogFile << endl;
   }
   if(Debug) cout << " Log file name is " << LogFile << endl;
   oflog.open(LogFile.c_str(),ios_base::out);
   if(!oflog) {
      cerr << "Failed to open log file " << LogFile << endl;
      return -1;
   }

   if(Debug) cout << " Opened log file " << LogFile << endl;
   oflog << Title << endl;
   if(Par.hasErrors()) {
      oflog << "\nErrors found in command line input:\n";
      Par.dumpErrors(oflog);
      oflog << "...end of Errors\n\n";
   }

   // paths
   if(dashop.getCount()) {
      values = dashop.getValue();
      if(help) cout << " Input: obs path name: " << values[0] << endl;
      InputPath = values[0];
   }
   else InputPath = string("");
   if(dashnp.getCount()) {
      values = dashnp.getValue();
      if(help) cout << " Input: nav path name: " << values[0] << endl;
      NavPath = values[0];
   }
   else NavPath = string("");
   if(dashep.getCount()) {
      values = dashep.getValue();
      if(help) cout << " Input: EOP path name: " << values[0] << endl;
      EOPPath = values[0];
   }
   else EOPPath = string("");

   // files
   // obs
   if(dashof.getCount()) {
      values = dashof.getValue();
      for(i=0; i<values.size(); i++) {
         field.clear();
         while(values[i].size() > 0)
            field.push_back(StringUtils::stripFirstWord(values[i],','));
         if(field.size() < 2) {
            oflog << "Error: no label in --obsfile input: " << values[i] << endl;
            cerr << "Error: no label in --obsfile input: " << values[i] << endl;
            continue;
         }
         Station& st=findStationInList(Stations,field[1]);
         // create new ObsFile and add to list
         ObsFile of;
         of.name = field[0];
         of.label = field[1];
         of.nread = -1;
         of.valid = false;
         ObsFileList.push_back(of);

         if(help) cout << " Input: Obs file (" << field[1] << ") : "
            << field[0] << endl;
      }
   }
   // nav
   if(dashnf.getCount()) {
      values = dashnf.getValue();
      NavFileNames = values;
      if(help) {
         cout << " Input: Nav files :";
         for(j=0; j<NavFileNames.size(); j++) cout << " " << NavFileNames[j];
         cout << endl;
      }
   }
   // eop
   if(dashef.getCount()) {
      values = dashef.getValue();
      EOPFileNames = values;
      if(help) {
         cout << " Input: Earth orientation files :";
         for(j=0; j<EOPFileNames.size(); j++) cout << " " << EOPFileNames[j];
         cout << endl;
      }
   }

   // positions
   if(dashXYZ.getCount()) {
      values = dashXYZ.getValue();
      for(i=0; i<values.size(); i++) {
         field.clear();
         while(values[i].size() > 0)
            field.push_back(StringUtils::stripFirstWord(values[i],','));
         if(field.size() < 4) {
            oflog << "Error: less than four fields in --PosXYZ input: "
               << values[i] << endl;
            cerr << "Error: less than four fields in --PosXYZ input: "
               << values[i] << endl;
            continue;
         }
         Station& st=findStationInList(Stations,field[3]);
         Position p(StringUtils::asDouble(field[0]),
                    StringUtils::asDouble(field[1]),
                    StringUtils::asDouble(field[2]));
         st.pos = p;
         if(help) cout << " Input: XYZ for station " << field[3] << ":"
            << " " << field[0] << " " << field[1] << " " << field[2] << endl;
      }
   }
   if(dashLLH.getCount()) {
      values = dashLLH.getValue();
      for(i=0; i<values.size(); i++) {
         field.clear();
         while(values[i].size() > 0)
            field.push_back(StringUtils::stripFirstWord(values[i],','));
         if(field.size() < 4) {
            oflog << "Error: less than four fields in --PosLLH input: "
               << values[i] << endl;
            cerr << "Error: less than four fields in --PosLLH input: "
               << values[i] << endl;
            continue;
         }
         Station& st=findStationInList(Stations,field[3]);
         Position p;
         p.setGeodetic(StringUtils::asDouble(field[0]),
                       StringUtils::asDouble(field[1]),
                       StringUtils::asDouble(field[2]));
         st.pos = p;
         if(help) cout << " Input: LLH for station " << field[3] << ":"
            << " " << field[0] << " " << field[1] << " " << field[2] << endl;
      }
   }
   if(dashPRS.getCount()) {
      values = dashPRS.getValue();
      for(i=0; i<values.size(); i++) {
         field.clear();
         field.push_back(StringUtils::stripFirstWord(values[i],','));
         Station& st=findStationInList(Stations,field[0]);
         st.usePRS = true;
         if(help) cout << " Input: pos for station " << field[0]
            << ": use average PR solution" << endl;
      }
   }
   if(dashtrop.getCount()) {
      values = dashtrop.getValue();
      for(i=0; i<values.size(); i++) {
         field.clear();
         while(values[i].size() > 0)
            field.push_back(StringUtils::stripFirstWord(values[i],','));
         if(field.size() < 2) {
            oflog << "Error: less than two fields in --TropModel input: "
               << values[i] << endl;
            cerr << "Error: less than two fields in --TropModel input: "
               << values[i] << endl;
            continue;
         }
         Station& st=findStationInList(Stations,field[1]);
         st.TropType = field[0];
         if(help) cout << " Input: Trop model " << field[0]
            << " at Station " << field[1] << endl;
      }
   }
   if(dashWx.getCount()) {
      values = dashWx.getValue();
      for(i=0; i<values.size(); i++) {
         field.clear();
         while(values[i].size() > 0)
            field.push_back(StringUtils::stripFirstWord(values[i],','));
         if(field.size() < 4) {
            oflog << "Error: less than four fields in --Weather input: "
               << values[i] << endl;
            cerr << "Error: less than four fields in --Weather input: "
               << values[i] << endl;
            continue;
         }
         Station& st=findStationInList(Stations,field[3]);
         st.temp = StringUtils::asDouble(field[0]);
         st.press = StringUtils::asDouble(field[1]);
         st.rhumid = StringUtils::asDouble(field[2]);

         if(help) cout << " Weather input for station " << field[3] << ":"
            << " " << field[0] << " " << field[1] << " " << field[2] << endl;
      }
   }
   if(dashfix.getCount()) {
      values = dashfix.getValue();
      for(i=0; i<values.size(); i++) {
         Station& st=findStationInList(Stations,values[i]);
         st.fixed = true;
         if(help) cout << " Input: Hold Station " << values[i] << " fixed." << endl;
      }
   }
   //if(dashant.getCount()) {
   //   values = dashant.getValue();
   //   for(i=0; i<values.size(); i++) {
   //      field.clear();
   //      while(values[i].size() > 0)
   //         field.push_back(StringUtils::stripFirstWord(values[i],','));
   //      if(field.size() < 2) {
   //         oflog << "Error: invalid AntAz input: " << values[i] << endl;
   //         cerr << "Error: invalid AntAz input: " << values[i] << endl;
   //         continue;
   //      }
   //      Station& st=findStationInList(Stations,field[1]);
   //      st.ant_azimuth = StringUtils::asDouble(field[0]);

   //      if(help) cout << " Input: antenna azimuth for station " << field[1] << ": "
   //         << fixed << setprecision(2) << st.ant_azimuth << " degrees" << endl;
   //   }
   //}

   // configuration
   if(dashfreq.getCount()) {
      values = dashfreq.getValue();
      if(values[0] == string("L1") || values[0] == string("1"))
         Frequency = 1;
      else if(values[0] == string("L2") || values[0] == string("2"))
         Frequency = 2;
      else if(values[0] == string("L3") || values[0] == string("3"))
         Frequency = 3;
      else {
         cout << "Error: invalid input (" << values[0]
            << ") --Frequency must be followed by 'L1','L2' or 'L3'" << endl;
         return -1;
      }
      if(help) cout << " Input: process frequency L" << Frequency << endl;
   }
   if(dashRef.getCount()) {
      values = dashRef.getValue();
      RefSat.fromString(values[0]);
      if(help) cout << " Input: set satellite " << RefSat << " as reference" << endl;
   }
   if(dashnoest.getCount()) {
      noEstimate = true;
      if(help) cout << " *** Turn OFF the estimation ***" << endl;
   }
   if(dashprsnit.getCount()) {
      values = dashprsnit.getValue();
      PRSnIter = StringUtils::asInt(values[0]);
      if(help) cout << " Input: set PRS iteration limit to  " << PRSnIter << endl;
   }
   if(dashprsrms.getCount()) {
      values = dashprsrms.getValue();
      PRSrmsLimit = StringUtils::asDouble(values[0]);
      if(help) cout << " Input: set PRS RMS residual limit to  "
         << scientific << setprecision(2) << PRSrmsLimit << endl;
   }
   if(dashprscon.getCount()) {
      values = dashprscon.getValue();
      PRSconverge = StringUtils::asDouble(values[0]);
      if(help) cout << " Input: set PRS convergence limit to  "
         << scientific << setprecision(2) << PRSconverge << endl;
   }
   if(dashprsalg.getCount()) {
      PRSalgebra = true;
      if(help) cout << " Input: use algebraic algorithm in pseudorange solution"
         << endl;
   }
   if(dashprselev.getCount()) {
      values = dashprselev.getValue();
      PRSMinElevation = StringUtils::asDouble(values[0]);
      if(help) cout << " Input: set PRS elevation limit to  "
         << fixed << setprecision(2) << PRSMinElevation << endl;
   }
   //dont if(dashnoprs.getCount()) {
      //dont noRAIM = true;
      //dont if(help) cout << " *** Turn OFF the pseudorange solution ***" << endl;
   //dont }
   if(dashnit.getCount()) {
      values = dashnit.getValue();
      nIter = StringUtils::asInt(values[0]);
      if(help)
         cout << " Input: number of iterations in Estimation : " << nIter << endl;
   }
   if(dashconv.getCount()) {
      values = dashconv.getValue();
      convergence = fabs(StringUtils::asDouble(values[0]));
      if(help)
         cout << " Input: convergence limit in Estimation : "
            << scientific << setprecision(3) << convergence << endl;
   }
   if(dashfixbias.getCount()) {
      FixBiases = true;
      if(help) cout << " Input: Turn ON fixing of biases in last iteration" << endl;
   }
   if(dashntrop.getCount()) {
      values = dashntrop.getValue();
      NRZDintervals = StringUtils::asInt(values[0]);
      if(help) cout << " Input: " << NRZDintervals
         << " residual zenith delay intervals" << endl;
   }
   if(dashttrop.getCount()) {
      values = dashttrop.getValue();
      RZDtimeconst = StringUtils::asDouble(values[0]);
      if(help) cout << " Input: RZD time constant " << fixed << setprecision(2)
         << RZDtimeconst << " hours" << endl;
   }
   if(dashstrop.getCount()) {
      values = dashstrop.getValue();
      RZDsigma = StringUtils::asDouble(values[0]);
      if(help) cout << " Input: RZD sigma " << fixed << setprecision(2)
         << RZDsigma << " meters" << endl;
   }
   if(dashtight.getCount()) {
      values = dashtight.getValue();
      TightConstraint = StringUtils::asDouble(values[0]);
      if(help) cout << " Input: tight constraint "
         << scientific << setprecision(2) << TightConstraint
         << " (fraction of baseline)" << endl;
   }
   if(dashloose.getCount()) {
      values = dashloose.getValue();
      LooseConstraint = StringUtils::asDouble(values[0]);
      if(help) cout << " Input: loose constraint "
         << scientific << setprecision(2) << LooseConstraint
         << " (fraction of baseline)" << endl;
   }

   // times
   // TD put try  {} around setToString and catch invalid formats...
   if(dashbt.getCount()) {
      values = dashbt.getValue();
      msg = values[0];
      field.clear();
      while(msg.size() > 0)
         field.push_back(StringUtils::stripFirstWord(msg,','));
      if(field.size() == 2)
         BegTime.setToString(field[0]+","+field[1], "%F,%g");
      else if(field.size() == 6)
         BegTime.setToString(field[0]+","+field[1]+","+field[2]+","+field[3]+","
            +field[4]+","+field[5], "%Y,%m,%d,%H,%M,%S");
      else {
         cerr << "Error: invalid --BeginTime input: " << values[0] << endl;
         oflog << "Error: invalid --BeginTime input: " << values[0] << endl;
      }
      if(help) cout << " Input: begin time " << values[0] << " = "
         << BegTime.printf("%Y/%02m/%02d %2H:%02M:%06.3f = %F/%10.3g") << endl;
   }
   if(dashet.getCount()) {
      values = dashet.getValue();
      msg = values[0];
      field.clear();
      while(msg.size() > 0)
         field.push_back(StringUtils::stripFirstWord(msg,','));
      if(field.size() == 2)
         EndTime.setToString(field[0]+","+field[1], "%F,%g");
      else if(field.size() == 6)
         EndTime.setToString(field[0]+","+field[1]+","+field[2]+","+field[3]+","
            +field[4]+","+field[5], "%Y,%m,%d,%H,%M,%S");
      else {
         cerr << "Error: invalid --EndTime input: " << values[0] << endl;
         oflog << "Error: invalid --EndTime input: " << values[0] << endl;
      }
      if(help) cout << " Input: end time " << values[0] << " = "
         << EndTime.printf("%Y/%02m/%02d %2H:%02M:%06.3f = %F/%10.3g") << endl;
   }

   // time table
   if(dashttab.getCount()) {
      values = dashttab.getValue();
      TimeTableFile = values[0];
      if(help) cout << " Input: time table file name " << TimeTableFile << endl;
   }

   // data editing
   if(dashelev.getCount()) {
      values = dashelev.getValue();
      MinElevation = StringUtils::asDouble(values[0]);
      if(help) cout << " Input: minimum elevation for phases : "
         << values[0] << " degrees" << endl;
   }
   if(dashrotelev.getCount()) {
      values = dashrotelev.getValue();
      RotatedAntennaElevation = StringUtils::asDouble(values[0]);
      if(help) cout << " Input: rotate antenna in elevation by : "
         << values[0] << " degrees" << endl;
   }
   if(dashrotaz.getCount()) {
      values = dashrotaz.getValue();
      RotatedAntennaAzimuth = StringUtils::asDouble(values[0]);
      if(help) cout << " Input: rotate antenna in azimuth by : "
         << values[0] << " degrees" << endl;
   }
   if(dashgap.getCount()) {
      values = dashgap.getValue();
      MaxGap = StringUtils::asInt(values[0]);
      if(help) cout << " Input: maximum acceptable gap (units DT): "
         << values[0] << endl;
   }
   if(dashmindd.getCount()) {
      values = dashmindd.getValue();
      MinDDSeg = StringUtils::asInt(values[0]);
      if(help) cout << " Input: minimum acceptable double difference segment: "
         << values[0] << endl;
   }
   if(dashphbias.getCount()) {
      values = dashphbias.getValue();
      PhaseBiasReset = StringUtils::asInt(values[0]);
      if(help) cout << " Input: phase bias reset limit (cycles) "
         << values[0] << endl;
   }
   if(dashXsat.getCount()) {
      values = dashXsat.getValue();
      for(i=0; i<values.size(); i++) {
         GSatID p(values[i]);
         if(help) cout << " Exclude satellite " << p << endl;
         ExSV.push_back(p);
      }
   }
   if(dashDT.getCount()) {
      values = dashDT.getValue();
      DataInterval = StringUtils::asDouble(values[0]);
      if(DataInterval < 0.0) DataInterval=fabs(DataInterval);
      if(help) cout << " Input: data interval " << fixed << setprecision(2)
         << DataInterval << " seconds" << endl;
   }

   // output
   if(dashrawout.getCount()) {
      values = dashrawout.getValue();
      OutputRawFile = values[0];
      if(help) cout << " Input: file name for raw data output ............... "
         << values[0] << endl;
   }
   if(dashprsout.getCount()) {
      values = dashprsout.getValue();
      OutputPRSFile = values[0];
      if(help) cout << " Input: file name for PRS output .................... "
         << values[0] << endl;
   }
   if(dashclkout.getCount()) {
      values = dashclkout.getValue();
      OutputClkFile = values[0];
      if(help) cout << " Input: file name for clock output .................. "
         << values[0] << endl;
   }
   if(dashrddout.getCount()) {
      values = dashrddout.getValue();
      OutputRawDDFile = values[0];
      if(help) cout << " Input: file name for raw DD data output ............ "
         << values[0] << endl;
   }
   if(dashdddout.getCount()) {
      values = dashdddout.getValue();
      OutputDDDFile = values[0];
      if(help) cout << " Input: file name for edited DD data output ......... "
         << values[0] << endl;
   }
   if(dashtddout.getCount()) {
      values = dashtddout.getValue();
      OutputTDDFile = values[0];
      if(help) cout << " Input: file name for triple differece data output .. "
         << values[0] << endl;
   }
   if(dashddrout.getCount()) {
      values = dashddrout.getValue();
      OutputDDRFile = values[0];
      if(help) cout << " Input: file name for DD post-fit residuals output .. "
            << values[0] << endl;
   }
   if(dashbaseout.getCount()) {
      values = dashbaseout.getValue();
      for(i=0; i<values.size(); i++) {
         field.clear();
         field.push_back(StringUtils::stripFirstWord(values[i],'-'));
         while(values[i].size() > 0)
            field.push_back(StringUtils::stripFirstWord(values[i],','));
         if(field.size() != 2 && field.size() != 5) {
            oflog << "Error: invalid --BaseOut input: " << values[i] << endl;
            cerr << "Error: invalid --BaseOut input: " << values[i] << endl;
            continue;
         }

         CI.OutputBaselines.push_back(field[0] + string("-") + field[1]);
         if(help) cout << " Input: baseline for output "
            << field[0] << "-" << field[1];

         Triple trip(0.0,0.0,0.0);
         if(field.size() == 5) {
            double x=StringUtils::asDouble(field[2]);
            double y=StringUtils::asDouble(field[3]);
            double z=StringUtils::asDouble(field[4]);
            trip = Triple(x,y,z);
            if(help) cout << " with offset " << field[2]
                           << "," << field[3] << "," << field[4];
         }
         else {
            if(help) cout << " ... no offset";
         }
         CI.OutputBaselineOffsets.push_back(trip);

         if(help) cout << endl;
      }
   }

   if(dashvalid.getCount()) {
      Validate = true;
      if(help) cout << " Input: validate -- read, test input and quit" << endl;
   }

   if(help) {
      if(argc > 1) cout << "--------- end of parsed input, Quit." << endl << endl;
      return -1;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void CommandInput::PreProcessArgs(const char *arg, vector<string>& Args)
{
try {
   if(string(arg) == string()) return;
   if(arg[0]=='-' && arg[1]=='f') {
      string filename(arg);
      filename.erase(0,2);
      if(Debug) cout << "Found a file of options: " << filename << endl;
      ifstream infile(filename.c_str());
      if(!infile) {
         cerr << "Error: could not open options file "
            << filename << endl;
      }
      else {
         char c;
         string buffer,word;
         while(1) {
            getline(infile,buffer);
            StringUtils::stripTrailing(buffer,'\r');

            // process the buffer before checking eof or bad b/c there can be
            // a line at EOF that has no CRLF...
            while(!buffer.empty()) {
               word = StringUtils::firstWord(buffer);
               if(word[0] == '#') {        // skip to end of line
                  break;
               }
               else if(word[0] == '"') {
                  word = StringUtils::stripFirstWord(buffer,'"');
               }
               else {
                  word = StringUtils::stripFirstWord(buffer);
               }
               PreProcessArgs(word.c_str(),Args);
               if(buffer.empty()) break;
            }

            if(infile.eof() || !infile.good()) break;
         }
      }
   }
   else if(string(arg)==string("-h") || string(arg)==string("--help")) {
      help = true;
      if(Debug) cout << "Found the help switch" << endl;
   }
   else if(string(arg)==string("-d") || string(arg)==string("--debug")) {
      Debug = true;
      cout << "Found the debug switch" << endl;
   }
   else if(string(arg)==string("-v") || string(arg)==string("--verbose")) {
      Verbose = true;
      if(Debug) cout << "Found the verbose switch" << endl;
   }
   // undocumented shortcut
   else if(string(arg).substr(0,7)==string("-AllOut")) {
      string stem=string(arg).substr(7);
      Args.push_back("--RAWFileOut"); Args.push_back("RAW" + stem);
      Args.push_back("--PRSFileOut"); Args.push_back("PRS" + stem);
      Args.push_back("--CLKFileOut"); Args.push_back("CLK" + stem);
      Args.push_back("--RDDFileOut"); Args.push_back("RDD" + stem);
      Args.push_back("--DDDFileOut"); Args.push_back("DDD" + stem);
      Args.push_back("--TDDFileOut"); Args.push_back("TDD" + stem);
      Args.push_back("--DDRFileOut"); Args.push_back("DDR" + stem);
   }
   else Args.push_back(arg);
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int CommandInput::ValidateCmdInput(void)
{
try {
   bool ok=true,flag;
   int i,n;
   string site1,site2,msg;
   vector<string> fixed,notfixed;
   map<string,Station>::iterator it;

      // data interval must be given
   if(DataInterval == -1.0) {
      cerr << "Input ERROR: data interval must be specified: --DT <t>. Abort.\n";
      oflog << "Input ERROR: data interval must be specified: --DT <t>. Abort.\n";
      ok = false;
   }

   if(BegTime > DayTime::BEGINNING_OF_TIME &&
      EndTime < DayTime::END_OF_TIME &&
      BegTime >= EndTime)
   {
      cerr << "Input ERROR: end time is at or before begin time. Abort.\n";
      oflog << "Input ERROR: end time is at or before begin time. Abort.\n";
      ok = false;
   }

   if(Frequency == 3 && FixBiases) {
      msg = string("Input ERROR: Frequency L3 (--Freq L3) and bias fixing "
         "(--FixBias) are inconsistent. Abort.\n");
      cerr << msg;
      oflog << msg;
      ok = false;
   }

   if(MinElevation < 0.0 || MinElevation > 90.0) {
      msg = "Input ERROR: Elevation limit (--MinElevation) is out of bounds: "
         + StringUtils::asString(MinElevation,2) + " Abort.\n";
      cerr << msg;
      oflog << msg;
      ok = false;
   }

   if(PRSMinElevation < 0.0 || PRSMinElevation > 90.0) {
      msg = "Input ERROR: Elevation limit (--PRSMinElevation) is out of bounds: "
         + StringUtils::asString(PRSMinElevation,2) + " Abort.\n";
      cerr << msg;
      oflog << msg;
      ok = false;
   }

      // loop over stations
      // make sure there is at least one fixed station, and one non-fixed.
      // check weather, create trop model, etc
   for(it=Stations.begin(); it != Stations.end(); it++) {
      if(it->second.fixed) {
         fixed.push_back(it->first);
      }
      else {
         notfixed.push_back(it->first);
      }

         // check that ids do not contain '-' or '_'
      if(it->first.find_first_of(string("-_")) != string::npos) {
         cerr << "Input ERROR: station label " << it->first
            << " invalid (must not contain '-' or '_'). Abort.\n";
         oflog << "Input ERROR: station label " << it->first
            << " invalid (must not contain '-' or '_'). Abort.\n";
         ok = false;
      }

         // check that there is 1+ data files
      for(n=0,i=0; i<ObsFileList.size(); i++)
         if(ObsFileList[i].label == it->first) n++;
      if(n==0) {
         cerr << "Input ERROR: station " << it->first
            << " has no observation data files. Abort.\n";
         oflog << "Input ERROR: station " << it->first
            << " has no observation data files. Abort.\n";
         ok = false;
      }

         // check weather
      if(it->second.temp == 0.0) it->second.temp = CI.DefaultTemp;
      if(it->second.press == 0.0) it->second.press = CI.DefaultPress;
      if(it->second.rhumid == 0.0) it->second.rhumid = CI.DefaultRHumid;
      if(it->second.temp <= 0 || it->second.temp > 40 ||
         it->second.press < 900. || it->second.press > 1050. ||
         it->second.rhumid < 0 || it->second.rhumid > 100) {
         cerr << "Input ERROR: station " << it->first
            << " has invalid weather parameters. Abort.\n";
         oflog << "Input ERROR: station " << it->first
            << " has invalid weather parameters. Abort.\n";
         ok = false;
      }
         // define the trop model for the estimation
         // see dashtrop above "choices are 'Black','NewB','ModHop','ModHopH','Saas'"
         // configure in Configure(2)
      flag = true;
      if(it->second.TropType == string("Saas")) {
         it->second.pTropModel = new SaasTropModel;
      }
      else if(it->second.TropType == string("ModHop")) {
         it->second.pTropModel = new GGTropModel;
      }
      else if(it->second.TropType == string("ModHopH")) {
         it->second.pTropModel = new GGHeightTropModel;
      }
      else if(it->second.TropType == string("NewB")) {
         it->second.pTropModel = new NBTropModel;
      }
      else if(it->second.TropType == string("Black")) {
         it->second.pTropModel = new SimpleTropModel;
      }
      else if(it->second.TropType == string("Zero")) {
         it->second.pTropModel = new ZeroTropModel;
      }
      else {
         cerr << "Input ERROR: unknown trop model for station " << it->first
            << " : " << it->second.TropType
            << "\n   (choices are 'Black','NewB','ModHop','ModHopH','Saas')"
            << ". Abort.\n";
         oflog << "Input ERROR: unknown trop model for station " << it->first
            << " : " << it->second.TropType
            << "\n   (choices are 'Black','NewB','ModHop','ModHopH','Saas')"
            << ". Abort.\n";
         ok = flag = false;
      }
      if(flag && ! it->second.pTropModel) {
         cerr << "Input ERROR: failed to allocate Trop model. Abort.\n";
         oflog << "Input ERROR: failed to allocate Trop model. Abort.\n";
         ok = false;
      }

      // test validity of stations -- is everything there?

      // is position valid? TD check geodetic height - warn if not small
      if(!it->second.usePRS && it->second.pos.getRadius() < 1.) {
         oflog << "Input ERROR: station " << it->first
            << " has undefined position. Abort.\n";
         cerr << "Input ERROR: station " << it->first
            << " has undefined position. Abort.\n";
         ok = false;
      }

   }  // end loop over stations

   if(fixed.size() == 0) {
      cerr << "Input ERROR: at least one station must be fixed. Abort.\n";
      oflog << "Input ERROR: at least one station must be fixed. Abort.\n";
      ok = false;
   }
   // not true
   //if(notfixed.size() == 0) {
   //   cerr << "Input ERROR: at least one station must be not fixed. Abort.\n";
   //   oflog << "Input ERROR: at least one station must be not fixed. Abort.\n";
   //   ok = false;
   //}

      // make up the list of baselines for computation
      // use first fixed site - all others ... can this matter?
   for(i=1; i<fixed.size(); i++) {
      Baselines.push_back(fixed[0] + string("-") + fixed[i]);
      if(CI.Verbose)
         oflog << " Compute baseline : " << Baselines[i] << endl;
      if(CI.Screen)
         cout << " Compute baseline : " << Baselines[i] << endl;
   }
   for(i=0; i<notfixed.size(); i++) {
      Baselines.push_back(fixed[0] + string("-") + notfixed[i]);
      if(CI.Verbose) 
         oflog << " Compute baseline : " << Baselines[i] << endl;
      if(CI.Screen)
         cout << " Compute baseline : " << Baselines[i] << endl;
   }

      // check that baselines for output are all valid
   for(i=0; i<CI.OutputBaselines.size(); i++) {
      site1 = StringUtils::word(CI.OutputBaselines[i],0,'-');
      site2 = StringUtils::word(CI.OutputBaselines[i],1,'-');
      if(  Stations.find(site1) == Stations.end()
        || Stations.find(site2) == Stations.end()) {
         
         cerr << "Input ERROR: Invalid output baseline : '" << CI.OutputBaselines[i]
               << "', station not found. Abort.\n";
         oflog << "Input ERROR: Invalid output baseline : '" << CI.OutputBaselines[i]
               << "', station not found. Abort.\n";
         ok = false;
      }
   }

   oflog << " ---- Input is " << (ok ? "" : "NOT ") << "valid ----\n";
   if(CI.Screen) cout << " ---- Input is " << (ok ? "" : "NOT ") << "valid ----\n";
   return (ok ? 0 : 1);
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void CommandInput::Dump(std::ostream& ofs) const
{
try {
   int i;
   ofs << "Summary of command line input:" << endl;
   if(Validate) ofs << " ------------ validate: this run will quit after "
      << "validating the input ---------" << endl;
   ofs << " Debug is " << (Debug ? "on":"off") << endl;
   ofs << " Verbose is " << (Verbose ? "on":"off") << endl;
   ofs << " Log file name is " << LogFile << endl;
   if(!InputPath.empty()) ofs << " Path for input obs files is "
      << InputPath << endl;
   ofs << " Input Observation files are:\n";
   for(i=0; i<ObsFileList.size(); i++)
      ofs << "  " << ObsFileList[i].name
         << " (station " << ObsFileList[i].label << ")" << endl;
   if(!NavPath.empty()) ofs << " Path for input nav files is "
      << NavPath << endl;
   ofs << " Input Navigation files are:\n";
   for(i=0; i<NavFileNames.size(); i++) ofs << "  " << NavFileNames[i]; ofs << endl;
   if(!EOPPath.empty()) ofs << " Path for input earth orientation files is "
      << EOPPath << endl;
   if(EOPFileNames.size() > 0) {
      ofs << " Input Earth orientation parameter files are:\n";
      for(i=0; i<EOPFileNames.size(); i++)
         ofs << "  " << EOPFileNames[i]; ofs << endl;
   }
   else
      ofs << " No EOP files - DDBase will search for 'finals.daily'" << endl;
   ofs << " Station list is:\n";
   map<string,Station>::const_iterator it;
   for(it=Stations.begin(); it != Stations.end(); it++) {
      ofs << "  Station " << it->first
         << " (" << (it->second.fixed ? "":"not ") << "fixed)" << endl;
      ofs << "   Position:   " << it->second.pos.printf("%.4x %.4y %.4z m\n");
      ofs << "   Position:   " << it->second.pos.printf("%A deg N %L deg E %h m\n");
      if(it->second.usePRS)
         ofs << "      (NB: use average PR solution to define this position.)\n";
      ofs << "   Trop model: " << it->second.TropType << endl;
      ofs << "   Weather:    " << fixed
         << setprecision(1) << it->second.temp << " degC, "
         << setprecision(2) << it->second.press << " mbar, "
         << setprecision(1) << it->second.rhumid << " %" << endl;
      //if(it->second.ant_azimuth != 0) ofs << "   Antenna orientation: "
      //   << setprecision(2) << it->second.ant_azimuth << " deg" << endl;
      ofs << "   Obs files: " << endl;
      for(i=0; i<ObsFileList.size(); i++)
         if(it->first == ObsFileList[i].label)
            ofs << "      " << ObsFileList[i].name << endl;
   }
   ofs << " End of Station list.\n";
   if(TimeTableFile.size() > 0)
      ofs << " Input time table file name " << TimeTableFile << endl;
   ofs << " Process L" << Frequency << " data." << endl;
   if(BegTime > DayTime::BEGINNING_OF_TIME) ofs << " Begin time is "
      << BegTime.printf("%Y/%m/%d %H:%02M:%6.3f = %F/%10.3g") << endl;
   if(EndTime < DayTime::END_OF_TIME) ofs << " End   time is "
      << EndTime.printf("%Y/%m/%d %H:%02M:%6.3f = %F/%10.3g") << endl;
   ofs << " Set satellite " << RefSat << " as the reference in DDs" << endl;
   if(noEstimate) ofs << " ** Estimation is turned OFF **" << endl;
   if(noRAIM) ofs << " ** Pseudorange solution is turned OFF **" << endl;
   ofs << " Set the number of iterations to " << nIter << endl;
   ofs << " Set the convergence limit to "
      << scientific << setprecision(3) << convergence << endl;
   ofs << " On last iteration," << (FixBiases ? "" : " do not")
      << " fix biases" << endl;
   if(NRZDintervals > 0) {
      ofs << " Estimate " << NRZDintervals
         << " residual zenith delay intervals" << endl;
      ofs << " RZD time constant is " << RZDtimeconst << " hours" << endl;
      ofs << " RZD sigma is " << RZDsigma << " meters" << endl;
   }
   else ofs << " Do not estimate any residual zenith delay" << endl;
   ofs << " A priori constraints, in ppm of the baseline:" << endl;
   ofs << "  Loose : " << scientific << setprecision(2) << LooseConstraint << endl;
   ofs << "  Tight : " << scientific << setprecision(2) << TightConstraint << endl;
   ofs << " Pseudorange solution: iterations " << PRSnIter
      << ", convergence " << scientific << setprecision(2) << PRSconverge
      << ", " << (PRSalgebra ? "" : "do not ") << "use algebra," << endl
      << "  RMS residual limit " << fixed << PRSrmsLimit
      << ", elevation mask " << fixed << PRSMinElevation
      << endl;
   if(DataInterval != -1)
      ofs << " Data interval is DT = "
      << fixed << setprecision(2) << DataInterval << " seconds." << endl;
   else
      ofs << " ERROR -- data interval must be specified: --DT <t>" << endl;
   ofs << " Maximum gap in data = " << MaxGap << " * DT" << endl;
   ofs << " Minimum DD dataset length = " << MinDDSeg << endl;
   ofs << " Phase bias reset limit is " << PhaseBiasReset << " cycles" << endl;
   ofs << " Minimum elevation for DD data is "
      << fixed << setw(6) << setprecision(2) << MinElevation << " degrees." << endl;
   if(RotatedAntennaElevation > 0 || RotatedAntennaAzimuth > 0)
      ofs << " Minimum elevation cutoff for antenna rotated through (el,az) = ("
         << fixed << setprecision(2) << RotatedAntennaElevation
         << "," << RotatedAntennaAzimuth << ") degrees." << endl;
   if(ExSV.size()) {
      ofs << " Exclude satellites :";
      for(i=0; i<ExSV.size(); i++) ofs << " " << ExSV[i];
      ofs << endl;
   }
   ofs << " Computed baselines :" << endl;
   for(i=0; i<Baselines.size(); i++) ofs << "  " << Baselines[i] << endl;
   if(CI.OutputBaselines.size()) {
      ofs << " Output baselines :" << endl;
      for(i=0; i<CI.OutputBaselines.size(); i++) {
         ofs << "  " << CI.OutputBaselines[i] << setprecision(5);
         Triple trip = CI.OutputBaselineOffsets[i];
         if(trip.mag() < 0.01) ofs << " ...without offset";
         else ofs << " with offset " << trip[0] << "," << trip[1] << "," << trip[2];
         ofs << endl;
      }
   }
   if(!OutputRawFile.empty())
      ofs << " Output file name: " << OutputRawFile << " for raw data." << endl;
   if(!OutputPRSFile.empty())
      ofs << " Output file name: " << OutputPRSFile << " for PRS." << endl;
   if(!OutputClkFile.empty())
      ofs << " Output file name: " << OutputClkFile << " for clock bias." << endl;
   if(!OutputRawDDFile.empty())
      ofs << " Output file name: " << OutputRawDDFile << " for raw DD data." << endl;
   if(!OutputDDDFile.empty())
      ofs << " Output file name: " << OutputDDDFile << " for edited DD data." << endl;
   if(!OutputDDRFile.empty())
      ofs << " Output file name: " << OutputDDRFile<< " for DD post-fit residuals." 
         << endl;
   if(!OutputTDDFile.empty())
      ofs << " Output file name: " << OutputTDDFile << " for triple diff data."
         << endl;

   ofs << "End of command line input summary." << endl;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
