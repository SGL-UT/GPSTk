#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/converters/novaRinex.cpp#3 $"

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






// novaRinex.cpp
// TD test on Solaris : temp files, intelToHost

#include <iostream>
#include <iomanip>
#include <time.h>
#include <string>
#include <map>

// GPSTk
#include "CommandOption.hpp"
#include "CommandOptionWithTimeArg.hpp"
#include "CommandOptionParser.hpp"
#include "StringUtils.hpp"
#include "DayTime.hpp"
#include "NovatelStream.hpp"
#include "NovatelData.hpp"
#include "RinexObsStream.hpp"
#include "RinexNavStream.hpp"
#include "RinexObsHeader.hpp"
#include "Triple.hpp"

using namespace std;
using namespace gpstk;

// -----------------------------------------------------------------------------------
string Prgm("novaRinex");                 // name of this program
string Vers("v1.1 2/06");                 // version - keep to 10 char
// 1.0 8/05
// 1.1 2/06 process obs only when datasize > 4 - empty records were setting FirstEpoch

// -----------------------------------------------------------------------------------
// global data, mostly to save information to go in the final Rinex header
// for computing the data time interval
int ndt[9];
double bestdt[9];
// epochs
DayTime CurrEpoch,PrevEpoch,FirstEpoch;
// table of PRN/#obs
map<RinexPrn,vector<int> > table;
vector<int> totals;
// Command line input
bool help,Debug;
DayTime BegTime,EndTime;
string NovatelFile, RinexObsFile, RinexNavFile;
string InputDirectory;
// header fields
bool FillOptionalHeader;
Triple HDAntPos,HDAntOffset;      // TD
vector<string> HDcomments;
vector<RinexObsHeader::RinexObsType> OutputTypes;
long gpsWeek;
bool debias;

//------------------------------------------------------------------------------------
// other global data
string TempFileName;       // initial output is here, before header is filled
NovatelStream instr;
RinexObsStream rostr;
RinexNavStream rnstr;
RinexObsHeader roh;        // used in CommandLine
// indexes for the std obs types in the header
int inC1,inP1,inL1,inD1,inS1,inP2,inL2,inD2,inS2;

// -----------------------------------------------------------------------------------
// command line input
int GetCommandInput(int argc, char **argv);
void PreProcessArgs(const char *arg, vector<string>& Args);
void DumpCommandLine(ostream& ofs = cout);

// open input and output files
int OpenFiles(void);

// fill header initially
void InitializeHeaders(RinexObsHeader& roh, RinexNavHeader& rnh);

// update saved information for revised header
void UpdateInformation(RinexObsData& rod);

// final header update, and write out
int UpdateHeader(string& TempFile, string& OutputFile, RinexObsHeader& rh);

// final obs output - modify header and write to the real output file name
string GetTempFileName(void);
int FillHeaderAndReplaceFile(string& TempFile,string& OutputFile,RinexObsHeader& rh);

// -----------------------------------------------------------------------------------
int main(int argc, char **argv)
{
   try {

      int i,j,k;
      double dt;

      // get the current system time
      time_t timer;
      struct tm *tblock;
      timer = time(NULL);
      tblock = localtime(&timer);
      //CurrEpoch.setYMDHMS(1900+tblock->tm_year,1+tblock->tm_mon,
      //         tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
      CurrEpoch.setLocalTime();

      i = GetCommandInput(argc, argv);
      if(i) return 0;
      if(Debug) DumpCommandLine();

      i = OpenFiles();
      if(i) return i;

      // declare data objects used for I/O
      long bytesread=0;  // at the end, bytesread should equal the Novatel file size.
      NovatelData novad;
      novad.setWeek(gpsWeek);

      RinexNavHeader rnh;
      RinexNavData rnd;
      RinexObsData rod;

      // initialize the headers (indexes inC1,etc defined here)
      InitializeHeaders(roh, rnh);

      // write headers
      rostr << roh;
      rnstr << rnh;

      // prep for the I/O loop
      FirstEpoch = DayTime::BEGINNING_OF_TIME;
      for(i=0; i<9; i++) ndt[i] = -1;

      // loop over data in the Novatel file
      while(instr >> novad) {
         if(Debug) cout << "Read " << NovatelData::RecNames[novad.rectype]
            << " size " << novad.headersize << " + " << novad.datasize
            << " number " << novad.recnum;
         if(novad.isOEM2()) {
            if(roh.recVers == string("OEM2/4")) roh.recVers = "OEM2";
            if(Debug) cout << " OEM2";
         }
         if(novad.isOEM4()) {
            if(Debug) cout << " OEM4";
            if(roh.recVers == string("OEM2/4")) roh.recVers = "OEM4";
         }
         if(Debug) {
            if(novad.isObs()) cout << " obs";
            if(novad.isNav()) cout << " nav";
            if(novad.isAux()) cout << " aux";
            cout << endl;
         }

         bytesread += novad.datasize + novad.headersize;
         if(novad.isOEM2()) bytesread += 1;      // CRC byte
         if(novad.isOEM4()) bytesread += 4;      // CRC bytes

         if(novad.isObs() && novad.datasize > 4) {   // obs only, with data
            rod = RinexObsData(novad);    // convert
            if(rod.time < BegTime) continue;
            if(rod.time > EndTime) break;
            if(Debug) rod.dump(cout);     // dump
            rostr << rod;                 // write out

            UpdateInformation(rod);
         }
         else if(novad.isNav()) {                                 // nav only
            rnd = RinexNavData(novad);    // convert
            if(Debug) rnd.dump(cout);     // dump
            rnstr << rnd;                 // write out
         }

      }  // end while loop over data

      if(Debug) cout << "Total bytes read = " << bytesread << endl;

      //instr.clear();
      //instr.close();
      //rostr.clear();
      rostr.close();
      //rnstr.clear();
      //rnstr.close();

      // now update the header and (re)write it to the file
      return UpdateHeader(TempFileName, RinexObsFile, roh);

   }
   catch(Exception& e) {
      cerr << "Caught exception\n" << e << endl;
   }
   return -1;
}

//------------------------------------------------------------------------------------
int OpenFiles(void)
{
   string filename;
   filename = InputDirectory + string("/") + NovatelFile;
   instr.open(filename.c_str(),ios::in | ios::binary);
   if(!instr) {
      cerr << "Failed to open input file " << NovatelFile << endl;
      return -1;
   }
   if(Debug) cout << "Opened input file " << NovatelFile << endl;
   //instr.exceptions(fstream::failbit);

   TempFileName = GetTempFileName();
   rostr.open(TempFileName.c_str(),ios::out);
   if(!rostr) {
      cerr << "Failed to open temporary output file " << TempFileName << endl;
      return -2;
   }
   rostr.exceptions(fstream::failbit);

   rnstr.open(RinexNavFile.c_str(),ios::out);
   if(!rnstr) {
      cerr << "Failed to open output nav file " << RinexNavFile << endl;
      return -3;
   }
   if(Debug) cout << "Opened output nav file " << RinexNavFile << endl;
   rnstr.exceptions(fstream::failbit);

   return 0;
}

//------------------------------------------------------------------------------------
void InitializeHeaders(RinexObsHeader& roh, RinexNavHeader& rnh)
{
   // observation header
   roh.version = 2.1;
   roh.fileType = "Observation";
   roh.system = systemGPS;
   roh.date = CurrEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S");
   roh.antennaPosition = Triple(0.0,0.0,0.0);
   roh.antennaOffset = Triple(0.0,0.0,0.0);
   roh.wavelengthFactor[0] = 1;
   roh.wavelengthFactor[1] = 1;

   // must keep track of indexes - for use in table
   if(Debug) cout << "Output obs types and indexes:";
   inC1 = inP1 = inL1 = inD1 = inS1 = inP2 = inL2 = inD2 = inS2 = -1;
   for(int i=0; i<OutputTypes.size(); i++) {
      if(OutputTypes[i] == RinexObsHeader::C1) inC1=i;
      if(OutputTypes[i] == RinexObsHeader::P1) inP1=i;
      if(OutputTypes[i] == RinexObsHeader::L1) inL1=i;
      if(OutputTypes[i] == RinexObsHeader::D1) inD1=i;
      if(OutputTypes[i] == RinexObsHeader::S1) inS1=i;
      if(OutputTypes[i] == RinexObsHeader::P2) inP2=i;
      if(OutputTypes[i] == RinexObsHeader::L2) inL2=i;
      if(OutputTypes[i] == RinexObsHeader::D2) inD2=i;
      if(OutputTypes[i] == RinexObsHeader::S2) inS2=i;
      if(Debug)
         cout << " " << RinexObsHeader::convertObsType(OutputTypes[i]) << ":" << i;
   }
   if(Debug) cout << endl;
   roh.obsTypeList = OutputTypes;

   roh.interval = 10.; // defined later by data
   roh.firstObs = CurrEpoch; // defined later by data
   roh.firstSystem = systemGPS;
   roh.lastObs = CurrEpoch; // defined later by data
   //roh.commentList.push_back("Created by GPSTK program " + Prgm + " " + Vers
      //+ CurrEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S"));

   roh.valid = RinexObsHeader::allValid21;
   roh.valid |= RinexObsHeader::commentValid;

   // navigation header
   rnh.version = 2.1;
   rnh.fileType = "Observation";
   rnh.fileProgram = roh.fileProgram;
   rnh.date = CurrEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S");
   //rnh.commentList.push_back("Created by GPSTK program " + Prgm + " " + Vers
      //+ CurrEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S"));

   rnh.valid = RinexNavHeader::allValid21;
   rnh.valid |= RinexNavHeader::commentValid;
}

//------------------------------------------------------------------------------------
void UpdateInformation(RinexObsData& rod)
{
   int i,j,k;
   double dt;
   RinexPrn sat;

   if(fabs(FirstEpoch - DayTime::BEGINNING_OF_TIME) < 1)  {
      PrevEpoch = FirstEpoch = rod.time;
      if(Debug) cout << "Set First Epoch to "
         << rod.time.printf("%Y/%m/%d %H:%02M:%6.3f = %F/%10.3g") << endl;
   }
   else
      PrevEpoch = CurrEpoch;
   CurrEpoch = rod.time;

      // compute the most likely value of dt, the time spacing of the data
   dt=CurrEpoch-PrevEpoch;
   if(dt > 0.0) {
      for(i=0; i<9; i++) {
         if(ndt[i] <= 0) { bestdt[i]=dt; ndt[i]=1; break; }
         if(fabs(dt-bestdt[i]) < 0.0001) { ndt[i]++; break; }
         if(i == 8) {
            k = 0;
            int nleast=ndt[k];
            for(j=1; j<9; j++) if(ndt[j] <= nleast) {
               k=j; nleast=ndt[j];
            }
            ndt[k]=1; bestdt[k]=dt;
         }
      }
   }

   RinexObsData::RinexPrnMap::iterator jt;
   map<RinexPrn,vector<int> >::iterator it;
   for(jt=rod.obs.begin(); jt != rod.obs.end(); jt++) {
      // find this satellite in the table
      sat = jt->first;
      if((it=table.find(sat)) == table.end()) {
         table[sat] = vector<int>(OutputTypes.size(),0);
         it = table.find(sat);
      }

      // increment counter for each obstype found
      if(inC1 >= 0 &&
         rod.obs[sat][RinexObsHeader::C1].data != 0.0) {
         table[sat][inC1]++;
         totals[inC1]++;
      }
      if(inP1 >= 0 &&
         rod.obs[sat][RinexObsHeader::P1].data != 0.0) {
         table[sat][inP1]++;
         totals[inP1]++;
      }
      if(inL1 >= 0 &&
         rod.obs[sat][RinexObsHeader::L1].data != 0.0) {
         table[sat][inL1]++;
         totals[inL1]++;
      }
      if(inD1 >= 0 &&
         rod.obs[sat][RinexObsHeader::D1].data != 0.0) {
         table[sat][inD1]++;
         totals[inD1]++;
      }
      if(inS1 >= 0 &&
         rod.obs[sat][RinexObsHeader::S1].data != 0.0) {
         table[sat][inS1]++;
         totals[inS1]++;
      }
      if(inP2 >= 0 &&
         rod.obs[sat][RinexObsHeader::P2].data != 0.0) {
         table[sat][inP2]++;
         totals[inP2]++;
      }
      if(inL2 >= 0 &&
         rod.obs[sat][RinexObsHeader::L2].data != 0.0) {
         table[sat][inL2]++;
         totals[inL2]++;
      }
      if(inD2 >= 0 &&
         rod.obs[sat][RinexObsHeader::D2].data != 0.0) {
         table[sat][inD2]++;
         totals[inD2]++;
      }
      if(inS2 >= 0 &&
         rod.obs[sat][RinexObsHeader::S2].data != 0.0) {
         table[sat][inS2]++;
         totals[inS2]++;
      }
   }
}

//------------------------------------------------------------------------------------
string GetTempFileName(void)
{
#ifdef _MSC_VER
   char newname[L_tmpnam];
   if(!tmpnam(newname)) {
#else
   char newname[]="TempnovaRinex.XXXXXX";
   if(mkstemp(newname)==-1) {
#endif
      return string("");
   }
   return string(newname);
}

//------------------------------------------------------------------------------------
int UpdateHeader(string& TempFile, string& OutputFile, RinexObsHeader& rh)
{
   int i,j;

   // update header
   if(FillOptionalHeader) {
      for(i=1,j=0; i<9; i++) if(ndt[i]>ndt[j]) j=i;
      rh.interval = bestdt[j];
      rh.valid |= RinexObsHeader::intervalValid;
      rh.firstObs = FirstEpoch;
      rh.lastObs = CurrEpoch;
      rh.valid |= RinexObsHeader::lastTimeValid;
   }

   // edit out obs types that have no data
   vector<RinexObsHeader::RinexObsType>::iterator it;
   vector<int> indexes;    // indexes is a list of 'good' indexes into table
   for(i=0, it=rh.obsTypeList.begin(); it != rh.obsTypeList.end(); i++) {
      if(totals[i] <= 0) {
         // no data for this obs type
         if(Debug) cout << " Obs type " << RinexObsHeader::convertObsType(*it)
            << " had no data - delete" << endl;
         // delete from header
         rh.obsTypeList.erase(it);
      }
      else {
         indexes.push_back(i);      // this is an index with data
         it++;
      }
   }
   // now edit the table
   map<RinexPrn,vector<int> >::iterator jt;
   for(jt=table.begin(); jt != table.end(); jt++) {      // for each sat..
      for(j=0,i=0; i<indexes.size(); i++,j++)
         if(j != indexes[i]) jt->second[j] = jt->second[indexes[i]];
      jt->second.resize(indexes.size());
   }
   

   // add the PRN/obs table
   if(FillOptionalHeader && table.size() > 0) {
      rh.numSVs = table.size();
      rh.valid |= RinexObsHeader::numSatsValid;
      rh.numObsForPrn.clear();
      rh.numObsForPrn = table;
      rh.valid |= RinexObsHeader::prnObsValid;
   }

      // re-open the file and replace the header
   RinexObsHeader rhjunk;
   RinexObsStream InAgain(TempFile.c_str());
   RinexObsStream ROutStr(OutputFile.c_str(), ios::out);
   InAgain.exceptions(fstream::failbit);
   ROutStr.exceptions(fstream::failbit);

   if(Debug) cout << "Opened " << OutputFile << " for output." << endl;
   InAgain >> rhjunk;
   ROutStr << rh;

   RinexObsData robs;
   while(InAgain >> robs)
      ROutStr << robs;

   //InAgain.clear();
   InAgain.close();
   //ROutStr.clear();
   ROutStr.close();

      // delete the temporary
   if(remove(TempFile.c_str()) != 0) {
      cerr << "Error: Could not remove existing temp file: " << TempFile << endl;
      return -1;
   }
   else if(Debug) cout << "Deleted temporary file " << TempFile << endl;

   return 0;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int GetCommandInput(int argc, char **argv)
{
try {
   int i,j;
   vector<string> values;

   // --------------------------------------------------------------------------------
   // set all the defaults
   Debug = help = false;
   BegTime = DayTime::BEGINNING_OF_TIME;
   EndTime = DayTime::END_OF_TIME;
   //NovatelFile,
   RinexObsFile = string("RnovaRinex.obs");
   RinexNavFile = string("RnovaRinex.nav");
   InputDirectory = string(".");
   // header fields
   FillOptionalHeader = true;
   roh.fileProgram = Prgm+" "+Vers;
   roh.fileAgency = string("ARL:UT/GPSTk");
   roh.observer = string(" ");
   roh.agency = string("ARL:UT/GPSTk");
   roh.markerName = string(" ");
   roh.markerNumber = string(" ");
   roh.recNo = " ";
   roh.recType = "Novatel";
   roh.recVers = "OEM2/4"; // defined later by data
   roh.antNo = " ";
   roh.antType = " ";
   //string HDRxNo,HDRxType,HDRxVer,HDAntNo,HDAntType;     // TD
   //vector<string> HDcomments;        // none
   //vector<RinexObsHeader::RinexObsType> OutputTypes;  // define later
   gpsWeek = -1;
   debias = false;

   // --------------------------------------------------------------------------------
   // Define the options

   // required options:
   RequiredOption dashinput(CommandOption::hasArgument, CommandOption::stdType,
      0,"input"," --input <file>    Novatel binary input file");
   dashinput.setMaxCount(1);

   // optional arguments:
   // this is here only so it will show up in the help msg...
   CommandOption dashf(CommandOption::hasArgument, CommandOption::stdType,
      'f',""," -f<file>          Name of file containing more options"
      " ('#' to EOL : comment)");

   CommandOption dashdir(CommandOption::hasArgument, CommandOption::stdType,0,"dir",
      " --dir <dir>       Directory in which to find input file (defaults to ./)");
   dashdir.setMaxCount(1);

   CommandOption dashobs(CommandOption::hasArgument, CommandOption::stdType,0,"obs",
      " --obs <file>      Rinex observation output file (RnovaRinex.obs)");
   dashobs.setMaxCount(1);

   CommandOption dashnav(CommandOption::hasArgument, CommandOption::stdType,0,"nav",
      " --nav <file>      Rinex navigation output file (RnovaRinex.nav)");
   dashnav.setMaxCount(1);

   CommandOption dashNHF(CommandOption::hasArgument, CommandOption::stdType,0,
      "noHDopt", "\nOutput Rinex header fields:\n --noHDopt         If present, "
      "do not fill optional records in the output Rinex header");
   dashNHF.setMaxCount(1);

   CommandOption dashHDp(CommandOption::hasArgument, CommandOption::stdType,0,"HDp",
      " --HDp <program>   Set output Rinex header 'program' field ('"
      + roh.fileProgram + "')");
   dashHDp.setMaxCount(1);

   CommandOption dashHDr(CommandOption::hasArgument, CommandOption::stdType,0,"HDr",
      " --HDr <run_by>    Set output Rinex header 'run by' field ('"
      + roh.fileAgency + "')");
   dashHDr.setMaxCount(1);

   CommandOption dashHDo(CommandOption::hasArgument, CommandOption::stdType,0,"HDo",
      " --HDo <obser>     Set output Rinex header 'observer' field ('"
      + roh.observer + "')");
   dashHDo.setMaxCount(1);

   CommandOption dashHDa(CommandOption::hasArgument, CommandOption::stdType,0,"HDa",
      " --HDa <agency>    Set output Rinex header 'agency' field ('"
      + roh.agency + "')");
   dashHDa.setMaxCount(1);

   CommandOption dashHDm(CommandOption::hasArgument, CommandOption::stdType,0,"HDm",
      " --HDm <marker>    Set output Rinex header 'marker' field ('"
      + roh.markerName + "')");
   dashHDm.setMaxCount(1);

   CommandOption dashHDn(CommandOption::hasArgument, CommandOption::stdType,0,"HDn",
      " --HDn <number>    Set output Rinex header 'number' field ('"
      + roh.markerNumber + "')");
   dashHDn.setMaxCount(1);

   CommandOption dashHDrn(CommandOption::hasArgument, CommandOption::stdType,0,"HDrn",
      " --HDrn <number>   Set output Rinex header 'Rx number' field ('"
      + roh.recNo + "')");
   dashHDrn.setMaxCount(1);

   CommandOption dashHDrt(CommandOption::hasArgument, CommandOption::stdType,0,"HDrt",
      " --HDrt <type>     Set output Rinex header 'Rx type' field ('"
      + roh.recType + "')");
   dashHDrt.setMaxCount(1);

   CommandOption dashHDrv(CommandOption::hasArgument, CommandOption::stdType,0,"HDrv",
      " --HDrv <vers>     Set output Rinex header 'Rx version' field ('"
      + roh.recVers + "')");
   dashHDrv.setMaxCount(1);

   CommandOption dashHDan(CommandOption::hasArgument, CommandOption::stdType,0,"HDan",
      " --HDan <number>   Set output Rinex header 'antenna number' field ('"
      + roh.antNo + "')");
   dashHDan.setMaxCount(1);

   CommandOption dashHDat(CommandOption::hasArgument, CommandOption::stdType,0,"HDat",
      " --HDat <type>     Set output Rinex header 'antenna type' field ('"
      + roh.antType + "')");
   dashHDat.setMaxCount(1);

   CommandOption dashHDc(CommandOption::hasArgument, CommandOption::stdType,0,"HDc",
      " --HDc <comment>   Add comment to output Rinex header (>1 allowed).");
   //dashHDc.setMaxCount(1);

   CommandOption dashobstype(CommandOption::hasArgument, CommandOption::stdType,
   0,"obstype","\nOutput Rinex observation data:\n"
   " --obstype <OT>    Output this Rinex (standard) obs type (i.e. <OT> is one of\n"
   "                     L1,L2,C1,P1,P2,D1,D2,S1,or S2); repeat for each type.\n"
   "                     NB default is ALL std. types that have data.");
   //dashobstype.setMaxCount(1);


   // times
   CommandOptionWithTimeArg dasheb(0,"begin","%Y,%m,%d,%H,%M,%f",
      "\nOutput configuration:\n --begin <arg>     Start time, arg is of the form "
      "YYYY,MM,DD,HH,Min,Sec");
   CommandOptionWithTimeArg dashgb(0,"beginGPS","%F,%g",
      " --beginGPS <arg>  Start time, arg is of the form GPSweek,GPSsow");

   CommandOptionWithTimeArg dashee(0,"end","%Y,%m,%d,%H,%M,%f",
      " --end <arg>       End time, arg is of the form YYYY,MM,DD,HH,Min,Sec");
   CommandOptionWithTimeArg dashge(0,"endGPS","%F,%g",
      " --endGPS <arg>    End time, arg is of the form GPSweek,GPSsow");

   // allow ONLY one start time (use startmutex(true) if one is required)
   CommandOptionMutex startmutex(false);
   startmutex.addOption(&dasheb);
   startmutex.addOption(&dashgb);
   CommandOptionMutex stopmutex(false);
   stopmutex.addOption(&dashee);
   stopmutex.addOption(&dashge);

   CommandOption dashweek(CommandOption::hasArgument, CommandOption::stdType,0,"week",
   " --week <week>     GPS Week number of this data, NB: this is for OEM2;\n"
   "                     this command serves two functions, resolving the ambiguity\n"
   "                     in the 10-bit week (default uses --begin, --end, or the\n"
   "                     current system time) and ensuring that ephemeris records\n"
   "                     that precede any obs records are not lost.");
   dashweek.setMaxCount(1);

   CommandOption dashdebias(CommandOption::hasArgument, CommandOption::stdType,
      0,"debias", " --debias          Remove an initial bias from the phase");
   dashdebias.setMaxCount(1);

   CommandOption dashhelp(CommandOption::hasArgument, CommandOption::stdType,
      'h',"help", " [-h|--help]       print this message and quit");
   dashhelp.setMaxCount(1);

   CommandOption dashDebug(CommandOption::hasArgument, CommandOption::stdType,
      'd',"debug", " [-d|--debug]      print extended output info");
   dashDebug.setMaxCount(1);


   // ... other options
   CommandOptionRest Rest("");

   // --------------------------------------------------------------------------------
   // Define the parser here, after the options -- this is the 'prgm description'
   CommandOptionParser Par(
" Prgm " + Prgm + " (" + Vers + ") will open and read a binary Novatel file\n"
"  (OEM2 and OEM4 receivers are supported), and convert the data to Rinex format\n"
"  observation and navigation files. The Rinex header is filled using user input\n"
"  (see below), and optional records are filled. Input is on the command line,\n"
"  or of the same format in a file (-f<file>).\n");

   // parse the command line
   // allow user to put all options in a file
   // PreProcessArgs pulls out help and Debug
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

   // if help, print usage
   if(help) {
      Par.displayUsage(cout,false);
      cout << endl;
   }

   // check for errors on the command line
   if (Par.hasErrors())
   {
      cerr << "\nErrors found in command line input:\n";
      Par.dumpErrors(cerr);
      cerr << "...end of Errors\n\n";
      help = true;
   }

   if(help && argc > 1)
      cout << endl << "--------- parsed input:" << endl;

   // --------------------------------------------------------------------------------
   // pull out the parsed input

   // these already parsed by PreProcessArgs
   //if(dashhelp.getCount()) help
   //if(dashDebug.getCount()) Debug

   if(dashinput.getCount()) {
      values = dashinput.getValue();
      if(help) cout << " Input Novatel file name " << values[0] << endl;
      NovatelFile = values[0];
   }
   if(dashdir.getCount()) {
      values = dashdir.getValue();
      if(help) cout << " Input Novatel file directory " << values[0] << endl;
      InputDirectory = values[0];
   }
   if(dashobs.getCount()) {
      values = dashobs.getValue();
      if(help) cout << " Input Rinex obs file name " << values[0] << endl;
      RinexObsFile = values[0];
   }
   if(dashnav.getCount()) {
      values = dashnav.getValue();
      if(help) cout << " Input Rinex nav file name " << values[0] << endl;
      RinexNavFile = values[0];
   }
   if(dashNHF.getCount()) {
      values = dashNHF.getValue();
      if(help) cout << " Turn off filling of optional header" << endl;
      FillOptionalHeader = false;
   }
   if(dashHDp.getCount()) {
      values = dashHDp.getValue();
      if(help) cout << " Input header program name " << values[0] << endl;
      roh.fileProgram = values[0];
   }
   if(dashHDr.getCount()) {
      values = dashHDr.getValue();
      if(help) cout << " Input header 'run by' field " << values[0] << endl;
      roh.fileAgency = values[0];
   }
   if(dashHDo.getCount()) {
      values = dashHDo.getValue();
      if(help) cout << " Input header observer field " << values[0] << endl;
      roh.observer = values[0];
   }
   if(dashHDa.getCount()) {
      values = dashHDa.getValue();
      if(help) cout << " Input header agency field " << values[0] << endl;
      roh.agency = values[0];
   }
   if(dashHDm.getCount()) {
      values = dashHDm.getValue();
      if(help) cout << " Input header marker field " << values[0] << endl;
      roh.markerName = values[0];
   }
   if(dashHDn.getCount()) {
      values = dashHDn.getValue();
      if(help) cout << " Input header marker name " << values[0] << endl;
      roh.markerNumber = values[0];
   }
   if(dashHDrn.getCount()) {
      values = dashHDrn.getValue();
      if(help) cout << " Input header receiver number " << values[0] << endl;
      roh.recNo = values[0];
   }
   if(dashHDrt.getCount()) {
      values = dashHDrt.getValue();
      if(help) cout << " Input header receiver type " << values[0] << endl;
      roh.recType = values[0];
   }
   if(dashHDrv.getCount()) {
      values = dashHDrv.getValue();
      if(help) cout << " Input header receiver version " << values[0] << endl;
      roh.recVers = values[0];
   }
   if(dashHDan.getCount()) {
      values = dashHDan.getValue();
      if(help) cout << " Input header antenna number " << values[0] << endl;
      roh.antNo = values[0];
   }
   if(dashHDat.getCount()) {
      values = dashHDat.getValue();
      if(help) cout << " Input header antenna type " << values[0] << endl;
      roh.antType = values[0];
   }
   if(dashHDc.getCount()) {
      values = dashHDc.getValue();
      if(help) cout << " Input comment for header " << values[0] << endl;
      NovatelFile = values[0];
   }
   if(dashobstype.getCount()) {
      values = dashobstype.getValue();
      for(i=0; i<values.size(); i++) {
         RinexObsHeader::RinexObsType rot;
         rot = RinexObsHeader::convertObsType(values[i]);
         OutputTypes.push_back(rot);
         if(help) cout << " Input output Rinex obs type " << values[i] << endl;
      }
   }
   if(dasheb.getCount()) {
      values = dasheb.getValue();
      BegTime.setToString(values[0], "%Y,%m,%d,%H,%M,%S");
      if(help) cout << " Input begin time " << values[0] << " = " << BegTime << endl;
      if(gpsWeek == -1) gpsWeek = BegTime.GPSfullweek();
   }
   if(dashgb.getCount()) {
      values = dashgb.getValue();
      BegTime.setToString(values[0], "%F,%g");
      if(help) cout << " Input begin time " << values[0] << " = " << BegTime << endl;
      if(gpsWeek == -1) gpsWeek = BegTime.GPSfullweek();
   }
   if(dashee.getCount()) {
      values = dashee.getValue();
      EndTime.setToString(values[0], "%Y,%m,%d,%H,%M,%S");
      if(help) cout << " Input end time " << values[0] << " = " << EndTime << endl;
      if(gpsWeek == -1) gpsWeek = EndTime.GPSfullweek();
   }
   if(dashge.getCount()) {
      values = dashge.getValue();
      EndTime.setToString(values[0], "%F,%g");
      if(help) cout << " Input end time " << values[0] << " = " << EndTime << endl;
      if(gpsWeek == -1) gpsWeek = EndTime.GPSfullweek();
   }
   if(dashweek.getCount()) {
      values = dashweek.getValue();
      gpsWeek = StringUtils::asInt(values[0]);
   }
   if(dashdebias.getCount()) {
      values = dashdebias.getValue();
      if(help) cout << " Turn on debiasing of the phase " << values[0] << endl;
      debias = true;
   }
   //if(dashhelp.getCount()) {
   //   help = true;
   //}
   //if(dashDebug.getCount()) {
   //   Debug = true;
   //}

   // process input
   if(gpsWeek == -1) gpsWeek = CurrEpoch.GPSfullweek();
   if(OutputTypes.size() == 0) {                     // fill with the standard types
      OutputTypes.push_back(RinexObsHeader::C1);
      OutputTypes.push_back(RinexObsHeader::P1);
      OutputTypes.push_back(RinexObsHeader::L1);
      OutputTypes.push_back(RinexObsHeader::D1);
      OutputTypes.push_back(RinexObsHeader::S1);
      OutputTypes.push_back(RinexObsHeader::P2);
      OutputTypes.push_back(RinexObsHeader::L2);
      OutputTypes.push_back(RinexObsHeader::D2);
      OutputTypes.push_back(RinexObsHeader::S2);
   }
   // table will be initialized inside the loop
   totals = vector<int>(OutputTypes.size(),0);

   if(help) return 1;
   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
void PreProcessArgs(const char *arg, vector<string>& Args)
{
try {
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
         string buffer;
         while( infile >> buffer) {
            if(buffer[0] == '#') {         // skip to end of line
               while(infile.get(c)) { if(c=='\n') break; }
            }
            else PreProcessArgs(buffer.c_str(),Args);
         }
      }
   }
   else if((arg[0]=='-' && arg[1]=='h') || string(arg)==string("--help")) {
      help = true;
      if(Debug) cout << "Found the help switch" << endl;
   }
   else if((arg[0]=='-' && arg[1]=='d') || string(arg)==string("--debug")) {
      Debug = true;
      cout << "Found the debug switch" << endl;
   }
   else Args.push_back(arg);
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
void DumpCommandLine(ostream& ofs)
{
try {
   int i;

   ofs << "Summary of command line input:" << endl;
   ofs << " Debug is " << (Debug ? "on":"off") << endl;
   if(!InputDirectory.empty()) ofs << " Path for input Novatel file is "
      << InputDirectory << endl;
   ofs << " Input Novatel file is: " << NovatelFile << endl;
   ofs << " Output Rinex obs file is: " << RinexObsFile << endl;
   ofs << " Output Rinex nav file is: " << RinexNavFile << endl;
   ofs << " --------- Header information:\n";
   if(!FillOptionalHeader) ofs << " Do not";
   ofs << " Fill optional records in header" << endl;
   ofs << " Header program: " << roh.fileProgram << endl;
   ofs << " Header run by: " << roh.fileAgency << endl;
   ofs << " Header observer: " << roh.observer << endl;
   ofs << " Header agency: " << roh.agency << endl;
   ofs << " Header marker name: " << roh.markerName << endl;
   ofs << " Header marker number: " << roh.markerNumber << endl;
   if(HDcomments.size() > 0) {
      ofs << " Header comments:\n";
      for(i=0; i<HDcomments.size(); i++) ofs << HDcomments[i] << endl;
   }
   ofs << " Output Rinex observation types (if found in the data):\n";
   for(i=0; i<OutputTypes.size(); i++)
      ofs << " " << RinexObsHeader::convertObsType(OutputTypes[i]);
   ofs << endl;
   if(BegTime > DayTime::BEGINNING_OF_TIME) ofs << " Begin time is "
      << BegTime.printf("%Y/%m/%d %H:%02M:%6.3f = %F/%10.3g") << endl;
   if(EndTime < DayTime::END_OF_TIME) ofs << " End   time is "
      << EndTime.printf("%Y/%m/%d %H:%02M:%6.3f = %F/%10.3g") << endl;
   ofs << " Debiasing of phase is turned " << (debias ? "on" : "off") << endl;

   ofs << "End of command line input summary." << endl;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
}

//------------------------------------------------------------------------------------
