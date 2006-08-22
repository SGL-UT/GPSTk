#pragma ident "$Id$"


/**
 * @file EditRinex.cpp
 * Edit a Rinex observation file using the RinexEditor in gpstk.
 */

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
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "DayTime.hpp"
#include "CommandOptionParser.hpp"
#include "CommandOption.hpp"
#include "RinexUtilities.hpp"

#include "RinexEditor.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <time.h>

using namespace gpstk;
using namespace std;

//------------------------------------------------------------------------------------
// Instantiate an editor
RinexEditor REC;

//------------------------------------------------------------------------------------
// data input from command line
string LogFile("EditRinex.log");
bool Verbose=false,Debug=false;
string Title;
// timer
clock_t totaltime;
// log file
ofstream oflog;

//------------------------------------------------------------------------------------
// prototypes
int GetCommandLine(int argc, char **argv);
void PreProcessArgs(const char *arg, vector<string>& Args, bool& Verbose, bool& Debug);

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   totaltime = clock();

   int iret;
   DayTime last;

      // Title and description
   Title = "EditRinex, part of the GPSTK ToolKit, Ver 3.0 9/4/03, Run ";
   time_t timer;
   struct tm *tblock;

   timer = time(NULL);
   tblock = localtime(&timer);
   last.setYMDHMS(1900+tblock->tm_year,1+tblock->tm_mon,
               tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
   Title += last.printf("%04Y/%02m/%02d %02H:%02M:%02S\n");
   cout << Title;

      // define extended types
   iret = RegisterARLUTExtendedTypes();
   if(iret) goto quit;

      // get command line
   iret=GetCommandLine(argc, argv);
   if(iret) goto quit;

   iret=REC.EditFile();
   if(iret) goto quit;

   quit:
   // compute run time
   totaltime = clock()-totaltime;
   oflog << "EditRinex timing: " << setprecision(3)
      << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.\n";

   return iret;
}
catch(gpstk::FFStreamError& e) {
   cerr << e;
   return 1;
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
int GetCommandLine(int argc, char **argv)
{
   bool help=false;
   int i,j,iret=0;
try {
      // required options

      // optional options
      // this only so it will show up in help page...
   CommandOption dashf(CommandOption::hasArgument, CommandOption::stdType,
      'f',""," -f<file>             file containing more options");

   CommandOption dashl(CommandOption::hasArgument, CommandOption::stdType,
      'l',""," -l<file>             Output log file name");
   dashl.setMaxCount(1);
   
   CommandOptionNoArg dashh('h', "help",
      " [-h|--help]          print syntax and quit.");

   CommandOptionNoArg dashd('d', "debug",
      " [-d|--debug]       print extended output info.");

   CommandOptionNoArg dashv('v', "verbose",
      " [-v|--verbose]       print extended output info."
      "\n [-<REC...>]          Rinex editing commands - see the following");

   // ... other options
   CommandOptionRest Rest("");

   CommandOptionParser Par(
      " Prgm EditRinex will open and read one RINEX file, apply editing commands,\n"
      " and write the modified RINEX data to another RINEX file(s).\n"
      " Input is on the command line, or of the same format in a file (-f<file>).\n");

   // allow user to put all options in a file
   // could also scan for debug here
   vector<string> Args;
   for(j=1; j<argc; j++) PreProcessArgs(argv[j],Args,Verbose,Debug);
   argc = Args.size();
   if(argc==0) Args.push_back(string("-h"));

   //if(Debug) {
      //cout << "List passed to REditCommandLine:\n";
      //for(i=0; i<argc; i++) cout << i << " " << Args[i] << endl;
      // strip out the REditCmds
   //}

   REC.REVerbose = Verbose;
   REC.REDebug = Debug;
   REC.AddCommandLine(Args);

   //if(Debug) {
      //deque<REditCmd>::iterator it=REC.Cmds.begin();
      //cout << "\nHere is the list of RE cmds\n";
      //while(it != REC.Cmds.end()) { it->Dump(cout,string("")); ++it; }
      //cout << "End of list of RE cmds" << endl;
   //}

      // open the log file first
   oflog.open(LogFile.c_str(),ios_base::out);
   if(!oflog) {
      cerr << "Failed to open log file " << LogFile << endl;
      return -1;
   }
   cout << "EditRinex output directed to log file " << LogFile << endl;
   REC.oflog = &oflog;
   oflog << Title;

      // preprocess the commands
   iret = REC.ParseCommands();
   if(iret) {
      cerr << "EditRinex Error: no " << (iret==-1 ? "input" : "output")
         << " file specified\n";
      oflog << "EditRinex Error: no " << (iret==-1 ? "input" : "output")
         << " file specified\n";
   }
   //if(Debug) {
      //cout << "\nHere is the parsed list of RE cmds\n";
      //it=REC.Cmds.begin();
      //while(it != REC.Cmds.end()) { it->Dump(cout,string("")); ++it; }
      //cout << "End of sorted list of RE cmds" << endl;
   //}

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

   //if(Debug) {
      //cout << "List passed to parse\n";
      //for(i=0; i<argc; i++) cout << i << " " << CArgs[i] << endl;
   //}
   Par.parseOptions(argc, CArgs);

   if(iret != 0 || dashh.getCount() > 0) {      // iret from ParseCommands
      Par.displayUsage(cout,false);
      DisplayRinexEditUsage(cout);
      help = true;   //return 1;
   }

   if (Par.hasErrors())
   {
      cerr << "\nErrors found in command line input:\n";
      oflog << "\nErrors found in command line input:\n";
      Par.dumpErrors(oflog);
      oflog << "...end of Errors\n\n";
      help = true;
   }
   
      // get values found on command line
   vector<string> values;

      // f never appears because we intercept it above
   //if(dashf.getCount()) { cout << "Option f "; dashf.dumpValue(cout); }
 
   //if(dashl.getCount()) {
   //   values = dashl.getValue();
   //   if(help) cout << "Output log file is: " << values[0] << endl;
   //   LogFile = values[0];
   //}

   if(dashh.getCount() && help)
      oflog << "Option h appears " << dashh.getCount() << " times\n";
   if(dashv.getCount() && help) {
      Verbose = true;
      if(help) oflog << "Option v appears " << dashv.getCount() << " times\n";
   }
   if(dashd.getCount() && help) {
      Debug = true;
      if(help) oflog << "Option d appears " << dashd.getCount() << " times\n";
   }

   if(Rest.getCount() && help) {
      oflog << "Remaining options:" << endl;
      values = Rest.getValue();
      for (i=0; i<values.size(); i++) oflog << values[i] << endl;
   }
   if(Verbose && help) {
      oflog << "\nTokens on command line (" << Args.size() << ") are:" << endl;
      for(j=0; j<Args.size(); j++) oflog << Args[j] << endl;
   }
   if(help) return 1;

   return 0;
}
catch(gpstk::Exception& e) {
      cerr << "EditRinex:GetCommandLine caught an exception\n" << e;
      GPSTK_RETHROW(e);
}
catch (...) {
      cerr << "EditRinex:GetCommandLine caught an unknown exception\n";
}
   return -1;
}

//------------------------------------------------------------------------------------
void PreProcessArgs(const char *arg, vector<string>& Args, bool& ver, bool& deb)
{
   if(arg[0]=='-' && arg[1]=='f') {
      string filename(arg);
      filename.erase(0,2);
      ifstream infile(filename.c_str());
      if(!infile) {
         cerr << "Error: could not open options file "
            << filename << endl;
      }
      else {
         char c;
         string buffer;
         while( infile >> buffer) {
            if(buffer[0] == '#') {         // skip to end of line
               while(infile.get(c)) { if(c=='\n') break; }
            }
            else PreProcessArgs(buffer.c_str(),Args,ver,deb);
         }
      }
   }
   else if((arg[0]=='-' && arg[1]=='d') || string(arg)==string("--debug")) {
      deb = true;
   }
   else if((arg[0]=='-' && arg[1]=='v') || string(arg)==string("--verbose")) {
      ver = true;
   }
   else if((arg[0]=='-' && arg[1]=='l')) {
      LogFile = string(&arg[2]);
   }
   else Args.push_back(arg);
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
