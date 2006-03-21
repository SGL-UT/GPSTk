// RinSum.cpp  Read and summarize Rinex obs files
//  RinSum is part of the GPS Tool Kit (GPSTK) developed in the
//  Satellite Geophysics Group at Applied Research Laboratories,
//  The University of Texas at Austin (ARL:UT), and was written by Dr. Brian Tolman.
//------------------------------------------------------------------------------------
#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/Rinextools/RinSum.cpp#1 $"

/**
 * @file RinSum.cpp
 * Read and summarize Rinex observation files.
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
#include "MathBase.hpp"
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "RinexNavBase.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "DayTime.hpp"
#include "CommandOptionParser.hpp"
#include "CommandOption.hpp"
#include "CommandOptionWithTimeArg.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <time.h>

using namespace gpstk;
using namespace std;

//------------------------------------------------------------------------------------
// data input from command line
vector<string> InputFiles;
string InputDirectory;
string OutputFile;
ostream* pout;
DayTime BegTime(DayTime::BEGINNING_OF_TIME);
DayTime EndTime(DayTime::END_OF_TIME);
bool ReplaceHeader=false;
bool TimeSortTable=false;
bool GPSTimeOutput=false;
bool Debug=false;

//------------------------------------------------------------------------------------
// data used for computation
const int ndtmax=15;
double dt,bestdt[ndtmax];
int ndt[ndtmax]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int nepochs,ncommentblocks;

//------------------------------------------------------------------------------------
// class used to store PRN/Obs table
class TableData {
public:
   RinexPrn prn;
   vector<int> nobs;
   DayTime begin,end;
   TableData(const RinexPrn& p, const int& n)
      { prn=p; nobs=vector<int>(n); };
      // needed for find()
   inline bool operator==(const TableData& d) {return d.prn == prn;}
};
   // for sort()
class TablePRNLessThan  {      
public:
   bool operator()(const TableData& d1, const TableData& d2)
      { return d1.prn < d2.prn; }
};
class TableBegLessThan  {
public:
   bool operator()(const TableData& d1, const TableData& d2)
      { return d1.begin < d2.begin; }
};

//------------------------------------------------------------------------------------
// prototypes
int GetCommandLine(int argc, char **argv);
int RegisterARLUTExtendedTypes(void);
bool isRinexObsFile(const string& file);
bool isRinexNavFile(const string& file);

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   int iret,i,j,k,n;
   DayTime last,prev(DayTime::BEGINNING_OF_TIME),ftime(DayTime::BEGINNING_OF_TIME);

      // Title and description
   string Title;
   Title = "RINSUM, part of the GPS ToolKit, Ver 2.0 9/2/03, Run ";
   time_t timer;
   struct tm *tblock;
   timer = time(NULL);
   tblock = localtime(&timer);
   last.setYMDHMS(1900+tblock->tm_year,1+tblock->tm_mon,
               tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
   Title += last.printf("%04Y/%02m/%02d %02H:%02M:%02S\n");
   cout << Title;

   iret=GetCommandLine(argc, argv);
   if(iret) return iret;

   iret = RegisterARLUTExtendedTypes();
   if(iret) return iret;

      // open the output file and write to it
   if(!OutputFile.empty()) {
      pout = new ofstream(OutputFile.c_str(),ios::out);
      if(pout->fail()) {
         cerr << "Could not open output file " << OutputFile << endl;
         pout = &cout;
      }
      else {
         pout->exceptions(ios::failbit);
         *pout << Title;
      }
   }
   else pout = &cout;

      // now open the input files, read the headers and data
   RinexObsHeader rheader;
   RinexObsData robs;
   for(i=0; i<InputFiles.size(); i++) {
      string filename;
      if(!InputDirectory.empty()) filename = InputDirectory + "/";
      filename += InputFiles[i];
      RinexObsStream InStream(filename.c_str());
      if(!InStream) {
         *pout << "File " << filename << " could not be opened.\n";
         continue;
      }
      InStream.exceptions(ios::failbit);
      if(!isRinexObsFile(filename)) {
         *pout << "File " << filename << " is not a Rinex observation file\n";
         if(isRinexNavFile(filename)) {
            *pout << "File " << filename << " is a Rinex navigation file\n";
         }
         continue;
      }

      // obs file
      *pout << "+++++++++++++ RinSum summary of Rinex obs file "
         << filename << " +++++++++++++\n";
      
         // input header
      try {
         InStream >> rheader;
      }
      catch(gpstk::FFStreamError& e) {
         cerr << "Caught an FFStreamError while reading header: "
            << e.getText(0) << endl;
      }
      catch(gpstk::Exception& e) {
         cerr << "Caught a gpstk exception while reading header: "
            << e.getText(0) << endl;
      }

      *pout << "Rinex header:\n";
      rheader.dump(*pout);

      if(!rheader.isValid()) {
         *pout << "Abort: header is invalid\n";
         *pout << "\n+++++++++++++ End of RinSum summary of "
            << filename << " +++++++++++++\n";
         continue;
      }

      //RinexObsStream out(argv[2], ios::out);
      //out << rheader;

      nepochs = ncommentblocks = 0;
      n = rheader.obsTypeList.size();
      vector<TableData> table;
      vector<int> totals(n);

         // input obs
      while(InStream >> robs)
      {
         if(Debug) *pout << "Epoch: " << robs.time
            << ", Flag " << robs.epochFlag
            << ", Nprn " << robs.obs.size()
            << ", clk " << robs.clockOffset << endl;
         if(robs.epochFlag > 1) {
            ncommentblocks++;
            //*pout << "inline header info:\n";
            //robs.auxHeader.dump(*pout);
            continue;
         }
         last = robs.time;
         if(last < BegTime) continue;
         if(last > EndTime) break;
         if(ftime == DayTime::BEGINNING_OF_TIME) ftime=last;
         nepochs++;
         RinexObsData::RinexPrnMap::const_iterator it;
         RinexObsData::RinexObsTypeMap::const_iterator jt;
         for(it=robs.obs.begin(); it != robs.obs.end(); ++it) {
            vector<TableData>::iterator ptab;
            ptab = find(table.begin(),table.end(),TableData(it->first,n));
            if(ptab == table.end()) {
               table.push_back(TableData(it->first,n));
               ptab = find(table.begin(),table.end(),TableData(it->first,n));
               ptab->begin = last;
            }
            ptab->end = last;
            if(Debug) *pout << "Prn " << setw(2) << it->first;
            for(jt=it->second.begin(); jt!=it->second.end(); jt++) {
               for(k=0; k<n; k++) if(rheader.obsTypeList[k] == jt->first) break;
               if(jt->second.data != 0) {
                  ptab->nobs[k]++;      // per obs
                  totals[k]++;
               }
               if(Debug) *pout << " " << RinexObsHeader::convertObsType(jt->first)
                  << " " << setw(13) << setprecision(3) << jt->second.data << " "
                  << jt->second.lli << " " << jt->second.ssi;
            }
            if(Debug) *pout << endl;
         }

         //out << robs;

         if(prev.year() != 1) {
            dt = last-prev;
            for(i=0; i<ndtmax; i++) {
               if(ndt[i] <= 0) { bestdt[i]=dt; ndt[i]=1; break; }
               if(ABS(dt-bestdt[i]) < 0.0001) { ndt[i]++; break; }
               if(i == ndtmax-1) {
                  k = 0;
                  int nleast=ndt[k];
                  for(j=1; j<ndtmax; j++) if(ndt[j] <= nleast) {
                     k=j; nleast=ndt[j];
                  }
                  ndt[k]=1; bestdt[k]=dt;
               }
            }
         }
         prev = last;
      }
      InStream.close();

         // compute interval
      for(i=1,j=0; i<ndtmax; i++) if(ndt[i]>ndt[j]) j=i;
      dt = bestdt[j];

         // warnings
      if((rheader.valid & RinexObsHeader::intervalValid)
            && ABS(dt-rheader.interval) > 1.e-3)
         *pout << "\n WARNING: Computed interval is " << setprecision(2)
            << dt << " sec, while input header has " << setprecision(2)
            << rheader.interval << " sec.\n";
      if(ABS(ftime-rheader.firstObs) > 1.e-8)
         *pout << "\n WARNING: Computed first time does not agree with header\n";
      if((rheader.valid & RinexObsHeader::lastTimeValid)
            && ABS(last-rheader.lastObs) > 1.e-8)
         *pout << "\n WARNING: Computed last time does not agree with header\n";

         // summary info
      *pout << "\n Computed interval is "
         << fixed << setw(5) << setprecision(2) << dt << endl;
      if(GPSTimeOutput) {
         *pout << " Computed first epoch is " << ftime.printf("%4F %14.7g") << endl;
         *pout << " Computed last epoch is " << last.printf("%4F %14.7g") << endl;
      }
      else {
         *pout << " Computed first epoch is "
            << ftime.printf("%04Y/%02m/%02d %02H:%02M:%010.7f") << endl;
         *pout << " Computed last epoch is "
            << last.printf("%04Y/%02m/%02d %02H:%02M:%010.7f") << endl;
      }
      i = 1+int(0.5+(last-ftime)/dt);
      *pout << " There were " << nepochs << " epochs ("
         << setprecision(2) << double(nepochs*100)/i
         << "% of " << i << " possible epochs in this timespan) and "
         << ncommentblocks << " inline header blocks.\n";

         // sort table
      sort(table.begin(),table.end(),TablePRNLessThan());
      if(TimeSortTable) sort(table.begin(),table.end(),TableBegLessThan());

         // output table
         // header
      *pout << "\n          Summary of data available in this file: "
         << "(Totals are based on times and interval)\n";
      *pout << "PRN/OT: ";
      for(k=0; k<n; k++)
         *pout << setw(6) << RinexObsHeader::convertObsType(rheader.obsTypeList[k]);
      *pout << " Total  Begin - End time\n";
      table.begin()->prn.setfill('0');
         // loop
      vector<TableData>::iterator tit;
      for(tit=table.begin(); tit!=table.end(); ++tit) {
         *pout << "PRN " << tit->prn << " ";
         for(k=0; k<n; k++) *pout << setw(6) << tit->nobs[k];
         // compute total based on times
         *pout << setw(6) << 1+int(0.5+(tit->end-tit->begin)/dt);
         if(GPSTimeOutput) {
            *pout << "  " << tit->begin.printf("%4F %10.3g")
               << " - " << tit->end.printf("%4F %10.3g") << endl;
         }
         else {
            *pout << "  " << tit->begin.printf("%04Y/%02m/%02d %02H:%02M:%06.3f")
               << " - " << tit->end.printf("%04Y/%02m/%02d %02H:%02M:%06.3f") << endl;
         }
      }
      *pout << "TOTAL   "; for(k=0; k<n; k++) *pout << setw(6) << totals[k];
      *pout << endl;

         // look for 'empty' obs types
      for(k=0; k<n; k++) {
         if(totals[k] <= 0) *pout << "\n WARNING: ObsType "
            << rheader.obsTypeList[k].type
            << " should be deleted from header.\n";
      }

      if(ReplaceHeader) {
            // modify the header
         rheader.version = 2.1; rheader.valid |= RinexObsHeader::versionValid;
         rheader.interval = dt; rheader.valid |= RinexObsHeader::intervalValid;
         rheader.lastObs = last; rheader.valid |= RinexObsHeader::lastTimeValid;
            // now the table
         rheader.numSVs = table.size(); rheader.valid |= RinexObsHeader::numSatsValid;
         rheader.numObsForPrn.clear();
         for(tit=table.begin(); tit!=table.end(); ++tit) {      // tit defined above
            rheader.numObsForPrn.insert(
               map<RinexPrn, vector<int> >::value_type(tit->prn,tit->nobs) );
         }
         rheader.valid |= RinexObsHeader::prnObsValid;
         //*pout << "\nNew header\n";
         //rheader.dump(*pout);

            // now re-open the file and replace the header
#ifdef _MSC_VER
         char newname[L_tmpnam];
         if(!tmpnam(newname)) {
            cerr << "Could not create temporary file name - abort\n";
            return -1;
         }
#else
         char newname[]="RinSumTemp.XXXXXX";
         if(mkstemp(newname)==-1) {
            cerr << "Could not create temporary file name - abort\n";
            return -1;
         }
#endif

         RinexObsHeader rhjunk;
         RinexObsStream ROutStr(newname, ios::out);
         RinexObsStream InAgain(filename.c_str());
         InAgain.exceptions(ios::failbit);

         InAgain >> rhjunk;
         ROutStr << rheader;
         while(InAgain >> robs) {
            last = robs.time;
            if(last < BegTime) continue;
            if(last > EndTime) break;
            ROutStr << robs;
         }
         InAgain.close();
         ROutStr.close();
            // delete original file and rename the temporary
         iret = remove(filename.c_str());
         if(iret) *pout << "RinSum: Error: Could not remove existing file: "
            << filename << endl;
         else {
            iret = rename(newname,filename.c_str());
            if(iret) *pout << "RinSum: ERROR!! Could not rename new file " << newname
               << " using old name " << filename << endl;
            else *pout << "\nRinSum: Replaced original header with complete one,"
               << " using temporary file name "
               << newname << endl;
         }
      }

      *pout << "\n+++++++++++++ End of RinSum summary of " << filename
         << " +++++++++++++\n";
   }

   if(pout != &cout) delete pout;

   return 0;
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
   int j;
try {
      // required options
   //RequiredOption dashi(CommandOption::hasArgument, CommandOption::stdType,
   CommandOption dashi(CommandOption::hasArgument, CommandOption::stdType,
      'i',"input"," [-i|--input] <file>  Input file name(s)");
   //dashi.setMaxCount(1);

      // optional options
      // this only so it will show up in help page...
   CommandOption dashf(CommandOption::hasArgument, CommandOption::stdType,
      'f',""," -f<file>             file containing more options");

   CommandOption dasho(CommandOption::hasArgument, CommandOption::stdType,
      'o',"output"," [-o|--output] <file> Output file name");
   dasho.setMaxCount(1);
   
   CommandOption dashp(CommandOption::hasArgument, CommandOption::stdType,
      'p',"path"," [-p|--path] <path>   Path for input file(s)");
   dashp.setMaxCount(1);

   CommandOptionNoArg dashr('R', "Replace",
      " [-R|--Replace]       Replace header with full one.");
      //"\n   *** NB the preceding option DELETES the original file.");
   //dashr.setMaxCount(1);

   CommandOptionNoArg dashs('s', "sort",
      " [-s|--sort]          Sort the PRN/Obs table on begin time.");

   CommandOptionNoArg dashg('g', "gps",
      " [-g|--gps]           Print times in the PRN/Obs table as GPS times.");

   // time
   CommandOptionWithTimeArg dasheb(0,"EpochBeg","%Y,%m,%d,%H,%M,%f",
      " --EpochBeg <arg>     Start time, arg is of the form YYYY,MM,DD,HH,Min,Sec");
   CommandOptionWithTimeArg dashgb(0,"GPSBeg","%F,%g",
      " --GPSBeg <arg>       Start time, arg is of the form GPSweek,GPSsow");

   CommandOptionWithTimeArg dashee(0,"EpochEnd","%Y,%m,%d,%H,%M,%f",
      " --EpochEnd <arg>     End time, arg is of the form YYYY,MM,DD,HH,Min,Sec");
   CommandOptionWithTimeArg dashge(0,"GPSEnd","%F,%g",
      " --GPSEnd <arg>       End time, arg is of the form GPSweek,GPSsow\n");

   // allow ONLY one start time (use startmutex(true) if one is required)
   CommandOptionMutex startmutex(false);
   startmutex.addOption(&dasheb);
   startmutex.addOption(&dashgb);
   CommandOptionMutex stopmutex(false);
   stopmutex.addOption(&dashee);
   stopmutex.addOption(&dashge);

   // help and debug
   CommandOptionNoArg dashh('h', "help",
      " [-h|--help]          print syntax and quit.");
   CommandOptionNoArg dashd('d', "debug",
      " [-d|--debug]         print debugging info.");

   // ... other options
   CommandOptionRest Rest("filename(s)");

   CommandOptionParser Par(
      "    Prgm RINSUM reads a Rinex file and summarizes it content.\n");

   // allow user to put all options in a file
   // could also scan for debug here
   char **CArgs=argv;
   vector<string> Args;
   for(j=1; j<argc; j++) {
      if(argv[j][0]=='-' && argv[j][1]=='f') {
         string filename(argv[j]);
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
               else Args.push_back(buffer);
            }
         }
      }
      else Args.push_back(argv[j]);
   }

   if(Args.size()==0) Args.push_back(string("-h"));

   argc = Args.size()+1;
   CArgs = new char * [argc];
   if(!CArgs) { cerr << "Failed to allocate CArgs\n"; return -1; }
   CArgs[0] = argv[0];
   for(j=1; j<argc; j++) {
      CArgs[j] = new char[Args[j-1].size()+1];
      if(!CArgs[j]) { cerr << "Failed to allocate CArgs[j]\n"; return -1; }
      strcpy(CArgs[j],Args[j-1].c_str());
   }

   Par.parseOptions(argc, CArgs);

      // get help option first
   if(dashh.getCount() > 0) {
      Par.displayUsage(cout,false);
      help = true;   //return 1;
   }

   if (Par.hasErrors())
   {
      cerr << "\nErrors found in command line input:\n";
      Par.dumpErrors(cerr);
      cerr << "...end of Errors\n\n";
      Par.displayUsage(cout,false);
      help = true; // return -1;
   }
   
      // get values found on command line
   vector<string> values;
      // f never appears because we intercept it above
   //if(dashf.getCount()) { cout << "Option f "; dashf.dumpValue(cout); }

   if(dashi.getCount()) {
      InputFiles = dashi.getValue();
      if(help) {
         cout << "Input files are:\n";
         for(int i=0; i<InputFiles.size(); i++)
            cout << "   " << InputFiles[i] << endl;
      }
   }
   if(dasho.getCount()) {
      values = dasho.getValue();
      OutputFile = values[0];
      if(help) cout << "Output file is " << OutputFile << endl;
   }
   if(dashp.getCount()) {
      values = dashp.getValue();
      InputDirectory = values[0];
      if(help) {
         cout << "Path options are:\n";
         for(int i=0; i<values.size(); i++) cout << "   " << values[i] << endl;
      }
   }

   if(dashr.getCount()) {
      ReplaceHeader=true;
      if(help) cout << "Option R appears " << dashr.getCount() << " times\n";
   }
   if(dashs.getCount()) {
      TimeSortTable=true;
      if(help) cout << "Option s appears " << dashs.getCount() << " times\n";
   }
   if(dashg.getCount()) {
      GPSTimeOutput=true;
      if(help) cout << "Print times in PRN/Obs table as GPS times\n";
   }
   if(dasheb.getCount()) {
      values = dasheb.getValue();
      BegTime.setToString(values[0], "%Y,%m,%d,%H,%M,%S");
      if(help) {
         cout << "EpochBeg options are:\n";
         for(int i=0; i<values.size(); i++) cout << values[i] << endl;
         cout << "BegTime is " << BegTime << endl;
      }
   }
   if(dashee.getCount()) {
      values = dashee.getValue();
      EndTime.setToString(values[0], "%Y,%m,%d,%H,%M,%S");
      if(help) {
         cout << "EpochEnd options are:\n";
         for(int i=0; i<values.size(); i++) cout << values[i] << endl;
         cout << "EndTime is " << EndTime << endl;
      }
   }
   if(dashgb.getCount()) {
      values = dashgb.getValue();
      BegTime.setToString(values[0], "%F,%g");
      if(help) {
         cout << "GPSBeg options are:\n";
         for(int i=0; i<values.size(); i++) cout << values[i] << endl;
         cout << "BegTime is " << BegTime << endl;
      }
   }
   if(dashge.getCount()) {
      values = dashge.getValue();
      EndTime.setToString(values[0], "%F,%g");
      if(help) {
         cout << "GPSEnd options are:\n";
         for(int i=0; i<values.size(); i++) cout << values[i] << endl;
         cout << "EndTime is " << EndTime << endl;
      }
   }
   if(dashh.getCount() && help)
      cout << "Option h appears " << dashh.getCount() << " times\n";
   if(dashd.getCount()) {
      Debug = true;
      if(help) cout << "Option d appears " << dashd.getCount() << " times\n";
   }

   if(Rest.getCount())
   {
      values = Rest.getValue();
      if(help) cout << "Input files (w/o -i) are:\n";
      for (int i=0; i<values.size(); i++) {
         if(help) cout << "  " << values[i] << endl;
         InputFiles.push_back(values[i]);
      }
   }

   if(Debug && help) {
      cout << "\nTokens on command line (" << Args.size() << ") are:" << endl;
      for(j=0; j<Args.size(); j++) cout << Args[j] << endl;
   }
   if(help) return 1;

   return 0;
}
catch(gpstk::Exception& e) {
      cerr << "RinSum:GetCommandLine caught an exception\n" << e;
      GPSTK_RETHROW(e);
}
catch (...) {
      cerr << "RinSum:GetCommandLine caught an unknown exception\n";
}
   return -1;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
