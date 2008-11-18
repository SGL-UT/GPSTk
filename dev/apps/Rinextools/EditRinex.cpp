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
 * @file EditRinex.cpp
 * Edit a Rinex observation file using the RinexEditor in gpstk.
 */

#include <cstring>

#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "DayTime.hpp"
#include "CommandOptionParser.hpp"
#include "CommandOption.hpp"
#include "RinexUtilities.hpp"
#include "StringUtils.hpp"

#include "RinexEditor.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <time.h>

using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
// NB Version for this prgm is just the RinexEditor version.

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
int GetCommandLine(int argc, char **argv, RinexEditor& re) throw(Exception);
void PreProcessArgs(const char *arg, vector<string>& Args) throw(Exception);

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   totaltime = clock();

   int iret;
   DayTime last;
   // NB. Do not instantiate editor outside main(), b/c DayTime::END_OF_TIME is a
   // static const that can produce static intialization order problems under some OS.
   RinexEditor REC;

      // Title and description
   Title = string("EditRinex, part of the GPS ToolKit, Ver ")
            + REC.getRinexEditVersion() + string(", Run ");
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
   iret=GetCommandLine(argc, argv, REC);
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
catch(gpstk::FFStreamError& e) { cerr << e; }
catch(gpstk::Exception& e) { cerr << e; }
catch(exception& e) { cerr << e.what(); }
catch (...) { cerr << "Unknown error.  Abort." << endl; }
   return 1;
}   // end main()

//------------------------------------------------------------------------------------
int GetCommandLine(int argc, char **argv, RinexEditor& REC) throw(Exception)
{
   bool help=false;
   int i,j,iret=0;
   vector<string> values; // to get values found on command line

try {
      // required options

      // optional options
      // this only so it will show up in help page...
   CommandOption dashf(CommandOption::hasArgument, CommandOption::stdType,
      'f',""," [-f|--file] <file>   file containing more options");

   CommandOption dashl(CommandOption::hasArgument, CommandOption::stdType,
      0,"log"," [-l|--log] <file>    Output log file name");
   dashl.setMaxCount(1);
   
   CommandOptionNoArg dashh('h', "help",
      " [-h|--help]          print syntax and quit.");

   CommandOptionNoArg dashd('d', "debug",
      " [-d|--debug]         print extended output info.");

   CommandOptionNoArg dashv('v', "verbose",
      " [-v|--verbose]       print extended output info."
      "\n [<REC>]              Rinex editing commands - cf. following");

   // ... other options
   CommandOptionRest Rest("");

   CommandOptionParser Par(
      " Prgm EditRinex will open and read one RINEX file, apply editing commands,\n"
      " and write the modified RINEX data to another RINEX file(s).\n"
      " Input is on the command line, or of the same format in a file (-f<file>).\n");

   // allow user to put all options in a file
   // could also scan for debug here
   vector<string> Args;
   for(j=1; j<argc; j++) PreProcessArgs(argv[j],Args);

   if(Args.size()==0 || dashh.getCount())
      help = true;

      // open the log file first
   oflog.open(LogFile.c_str(),ios_base::out);
   if(!oflog) {
      cerr << "Failed to open log file " << LogFile << endl;
      return -1;
   }
   cout << "EditRinex output directed to log file " << LogFile << endl;
   REC.oflog = &oflog;
   oflog << Title;

   //if(Debug) {
      //cout << "List passed to REditCommandLine:\n";
      //for(i=0; i<Args.size(); i++) cout << i << " " << Args[i] << endl;
      // strip out the REditCmds
   //}

   // set up editor and pull out (delete) editing commands
   REC.REVerbose = Verbose;
   REC.REDebug = Debug;
   REC.AddCommandLine(Args);

   //if(Debug) {
      //deque<REditCmd>::iterator jt=REC.Cmds.begin();
      //cout << "\nHere is the list of RE cmds\n";
      //while(jt != REC.Cmds.end()) { jt->Dump(cout,string("")); ++jt; }
      //cout << "End of list of RE cmds" << endl;
   //}

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
   delete[] CArgs;

   if(iret != 0 || dashh.getCount() > 0) {      // iret from ParseCommands
      if(help) {
         Par.displayUsage(cout,false);
         cout << endl;
         DisplayRinexEditUsage(cout);
      }
      else {
         Par.displayUsage(oflog,false);
         oflog << endl;
         DisplayRinexEditUsage(oflog);
      }
      help = true;   //return 1;
   }

   if (Par.hasErrors())
   {
      cerr << "\nErrors found in command line input:\n";
      oflog << "\nErrors found in command line input:\n";
      Par.dumpErrors(cerr);
      Par.dumpErrors(oflog);
      cerr << "...end of Errors\n\n";
      oflog << "...end of Errors\n\n";
      help = true;
   }
   
      // f never appears because we intercept it in PreProcessArgs
   //if(dashf.getCount()) { cout << "Option f "; dashf.dumpValue(cout); }
      // get log file name - pull out in PreProcessArgs
   //if(dashl.getCount()) {
   //   values = dashl.getValue();
   //   LogFile = values[0];
   //   if(help) cout << "Output log file is: " << LogFile << endl;
   //}

   //if(dashh.getCount() && help)
   //   oflog << "Option h appears " << dashh.getCount() << " times\n";
   if(dashv.getCount() && help) {
      Verbose = true;
      //if(help) oflog << "Option v appears " << dashv.getCount() << " times\n";
   }
   if(dashd.getCount() && help) {
      Debug = true;
      //if(help) oflog << "Option d appears " << dashd.getCount() << " times\n";
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
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
   return -1;
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
      if(Debug) cout << "Found a file of options: " << filename << endl;
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
   else if((arg[0]=='-' && arg[1]=='d') || string(arg)==string("--debug"))
      Debug = true;
   else if((arg[0]=='-' && arg[1]=='v') || string(arg)==string("--verbose"))
      Verbose = true;
   else if(string(arg) == "--file" || string(arg) == "-f")
      found_cfg_file = true;
   else if(string(arg) == "--log")
      found_log_file = true;
   else Args.push_back(arg);
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
