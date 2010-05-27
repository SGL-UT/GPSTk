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
 * @file novaRinex.cpp
 * gpstk::novaRinex - convert Novatel binary data files to RINEX format
 */

#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <string>
#include <map>

// GPSTk
#include "StringUtils.hpp"
#include "DayTime.hpp"
#include "CommandOption.hpp"
#include "CommandOptionWithTimeArg.hpp"
#include "CommandOptionParser.hpp"
#include "NovatelStream.hpp"
#include "NovatelData.hpp"
#include "RinexObsStream.hpp"
#include "RinexNavStream.hpp"
#include "RinexObsHeader.hpp"
#include "Triple.hpp"
//#include "RinexBinex.hpp"

using namespace std;
using namespace gpstk;
using namespace StringUtils;

// -----------------------------------------------------------------------------------
string Prgm("novaRinex");                 // name of this program
string Vers("v2.1 9/07");                 // version - keep to 10 char
// 1.0 8/05
// 1.1 2/06 process obs only when datasize > 4 - empty records were setting FirstEpoch
// 1.2 6/06 catch exceptions, and allow blanks on cmd line and in input file
// 1.3 7/06 correctly open files in other than current directory, don't print input
//          errors when help is the only option
// 1.4 7/06 handle exceptions
// 1.5 7/06 correct handling of header inputs
// 2.0 4/07 added Binex output -- commented out
// 2.1 9/07 declare failure when message length is < 0 in NovatelData.cpp

// -----------------------------------------------------------------------------------
// global data, mostly to save information to go in the final RINEX header
// for computing the data time interval
int ndt[9];
double bestdt[9];
// epochs
DayTime CurrEpoch,PrevEpoch,FirstEpoch;
// table of PRN/#obs
map<SatID,vector<int> > table;
vector<int> totals;
// Command line input
bool help,Debug,verbose;
DayTime BegTime,EndTime;
string NovatelFile, RinexObsFile, RinexNavFile; //, BinexFile;
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
int GetCommandInput(int argc, char **argv) throw(Exception);
void PreProcessArgs(const char *arg, vector<string>& Args) throw(Exception);
void DumpCommandLine(ostream& ofs = cout) throw(Exception);

// open input and output files
int OpenFiles(void) throw(Exception);

// fill header initially
void InitializeHeaders(RinexObsHeader& roh, RinexNavHeader& rnh) throw(Exception);

// update saved information for revised header
void UpdateInformation(RinexObsData& rod) throw(Exception);

// final header update, and write out
int UpdateHeader(string& TempFile, string& OutputFile, RinexObsHeader& rh)
   throw(Exception);

// final obs output - modify header and write to the real output file name
string GetTempFileName(void) throw(Exception);
int FillHeaderAndReplaceFile(string& TempFile,string& OutputFile,RinexObsHeader& rh)
   throw(Exception);

// -----------------------------------------------------------------------------------
int main(int argc, char **argv)
{
   try {

      int i,n,nobs,nnav;
      double dt;

      // get the current system time
      time_t timer;
      struct tm *tblock;
      timer = time(NULL);
      tblock = localtime(&timer);
      CurrEpoch.setLocalTime();

      i = GetCommandInput(argc, argv);
      if(i) return 0;
      if(verbose) {
         cout << Prgm << " version " << Vers << " run " << CurrEpoch << endl;
         DumpCommandLine();
      }

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

      // show a counter
      nobs = nnav = n = 0;
      // loop over data in the Novatel file
      try{
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
	       try{
                   rod = RinexObsData(novad);    // convert
	       }catch(Exception e){cout << "Malformed Novatel obs record" << endl;}
               if(rod.time < BegTime) continue;
               if(rod.time > EndTime) break;
               if(Debug) rod.dump(cout);     // dump

               rostr << rod;                 // write out
               nobs++;

               UpdateInformation(rod);
            }
            else if(novad.isNav()) {                                 // nav only
	       try{
                   rnd = RinexNavData(novad);    // convert
	       }catch(Exception e){cout << "Malformed Novatel nav record" << endl;}
               if(Debug) rnd.dump(cout);     // dump
               rnstr << rnd;                 // write out
               nnav++;
            }

            n++;
            if(verbose && !Debug) {
               if(n == 100) cout << "Reading Novatel records: (100 per .)\n";
               if(!(n % 100)) { cout << "."; cout.flush(); }
               if(!(n % 8000)) cout << endl;
            }

         }  // end while loop over data
      }
      catch(Exception& e) { GPSTK_RETHROW(e); }

      if(verbose && !Debug) cout << "\n";

      //instr.clear();
      instr.close();
      //rostr.clear();
      rostr.close();
      //rnstr.clear();
      rnstr.close();

      // now update the header and (re)write it to the file
      i = UpdateHeader(TempFileName, RinexObsFile, roh);

      if(verbose) {
         cout << "novaRinex read " << n
            << " records, and wrote " << nobs
            << " observations and " << nnav << " ephemerides\n";
         cout << "Total bytes read = " << bytesread << endl;
      }

      return i;
   }
   catch(Exception& e) { cerr << "Caught exception\n" << e << endl; }
   catch(...) { cerr << "Unknown error." << endl; }

   return -1;
}

//------------------------------------------------------------------------------------
int OpenFiles(void) throw(Exception)
{
try {
   string filename;
   if(InputDirectory.empty())
      filename = NovatelFile;
   else
      filename = InputDirectory + string("/") + NovatelFile;
   instr.open(filename.c_str(),ios::in | ios::binary);
   if(!instr.is_open()) {
      cerr << "Failed to open input file " << NovatelFile << endl;
      return -1;
   }
   if(verbose) cout << "Opened input file " << NovatelFile << endl;
   instr.exceptions(fstream::failbit);

   TempFileName = GetTempFileName();
   rostr.open(TempFileName.c_str(),ios::out);
   if(!rostr.is_open()) {
      cerr << "Failed to open temporary output file " << TempFileName << endl;
      return -2;
   }
   rostr.exceptions(fstream::failbit);

   rnstr.open(RinexNavFile.c_str(),ios::out);
   if(!rnstr.is_open()) {
      cerr << "Failed to open output nav file " << RinexNavFile << endl;
      return -3;
   }
   if(verbose) cout << "Opened output nav file " << RinexNavFile << endl;
   rnstr.exceptions(fstream::failbit);

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void InitializeHeaders(RinexObsHeader& roh, RinexNavHeader& rnh) throw(Exception)
{
try {
   int i;
   // observation header
   roh.version = 2.1;
   roh.fileType = "Observation";
   //Currently only supports GPS data.  
   roh.system = RinexSatID(-1, RinexSatID::systemGPS);
   // use same format as writer in RinexObsHeader.cpp uses
      // old "%04Y/%02m/%02d %02H:%02M:%02S");
   roh.date = CurrEpoch.printf("%02m/%02d/%04Y %02H:%02M:%02S");
   roh.antennaPosition = Triple(0.0,0.0,0.0);
   roh.antennaOffset = Triple(0.0,0.0,0.0);
   roh.wavelengthFactor[0] = 1;
   roh.wavelengthFactor[1] = 1;

   // must keep track of indexes - for use in table
   if(Debug) cout << "Output obs types and indexes:";
   inC1 = inP1 = inL1 = inD1 = inS1 = inP2 = inL2 = inD2 = inS2 = -1;
   for(i=0; i<OutputTypes.size(); i++) {
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
   roh.firstSystem = RinexSatID();
   roh.lastObs = CurrEpoch; // defined later by data
   roh.commentList.push_back("Created by GPSTK program " + Prgm + " " + Vers
      + CurrEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S"));
   for(i=0; i<HDcomments.size(); i++)
      roh.commentList.push_back(HDcomments[i]);

   roh.valid = RinexObsHeader::allValid21;
   roh.valid |= RinexObsHeader::commentValid;

   // navigation header
   rnh.version = 2.1;
   rnh.fileType = "Observation";
   rnh.fileProgram = roh.fileProgram;
   rnh.fileAgency = roh.fileAgency;
   rnh.date = CurrEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S");
   rnh.commentList.push_back("Created by GPSTK program " + Prgm + " " + Vers
      + CurrEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S"));
   for(i=0; i<HDcomments.size(); i++)
      rnh.commentList.push_back(HDcomments[i]);

   rnh.valid = RinexNavHeader::allValid21;
   rnh.valid |= RinexNavHeader::commentValid;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void UpdateInformation(RinexObsData& rod) throw(Exception)
{
try {
   int i,j,k;
   double dt;
   SatID sat;

   if(fabs(FirstEpoch - DayTime::BEGINNING_OF_TIME) < 1)  {
      PrevEpoch = FirstEpoch = rod.time;
      if(verbose) cout << "Set First Epoch to "
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
   else if(dt < 0.0)
      cout << "Warning! observation records out of time order (previous > current) : "
         << PrevEpoch.printf("%F %.3g") << " > " << CurrEpoch.printf("%F %.3g")
         << endl;

   RinexObsData::RinexSatMap::iterator jt;
   map<SatID,vector<int> >::iterator it;
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
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
string GetTempFileName(void) throw(Exception)
{
try {
#ifdef _MSC_VER
   char newfilename[L_tmpnam];
   if(!tmpnam(newfilename)) {
      return string("");
   }
   char *dtemp = getenv("TEMP");
   string pathname(dtemp);
   pathname += string("\\") + string(newfilename);
   remove(pathname.c_str());
   //cout << "Open temporary file " << pathname << endl;
   return pathname;
#else
   char newname[]="TempnovaRinex.XXXXXX";
   if(mkstemp(newname)==-1) {
      return string("");
   }
   remove(newname);
   return string(newname);
#endif
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int UpdateHeader(string& TempFile, string& OutputFile, RinexObsHeader& rh)
   throw(Exception)
{
try {
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
   map<SatID,vector<int> >::iterator jt;
   for(jt=table.begin(); jt != table.end(); jt++) {      // for each sat..
      for(j=0,i=0; i<indexes.size(); i++,j++)
         if(j != indexes[i]) jt->second[j] = jt->second[indexes[i]];
      jt->second.resize(indexes.size());
   }
   

   // add the PRN/obs table
   if(FillOptionalHeader && table.size() > 0) {
      rh.numSVs = table.size();
      rh.valid |= RinexObsHeader::numSatsValid;
      rh.numObsForSat.clear();
      rh.numObsForSat = table;
      rh.valid |= RinexObsHeader::prnObsValid;
   }

      // re-open the obs file for reading, and replace the header
   RinexObsStream InAgain(TempFile.c_str());
   if(!InAgain) {
      cerr << "Failed to re-open temp output Rinex obs file " << TempFile << endl;
      return -3;
   }
   InAgain.exceptions(fstream::failbit);

      // open the true output obs file for writing
   RinexObsStream ROutStr(OutputFile.c_str(), ios::out);
   if(!ROutStr) {
      cerr << "Failed to open output Rinex obs file " << OutputFile << endl;
      return -3;
   }
   if(verbose) cout << "Opened file " << OutputFile << " for RINEX output." << endl;
   ROutStr.exceptions(fstream::failbit);

   //   // open a BINEX stream
   //BinexStream BinexOut;
   //if(!BinexFile.empty()) {
   //   BinexOut.open(BinexFile.c_str(), std::ios::out | std::ios::binary);
   //   if(!BinexOut.is_open()) {
   //      cerr << "Failed to open output BINEX file " << BinexFile << endl;
   //      return -3;
   //   }
   //   BinexOut.exceptions(ios_base::failbit | ios_base::badbit);
   //   if(verbose) cout << "Opened file " << BinexFile << " for BINEX output." << endl;
   //}

      // read preliminary header, ...
   RinexObsHeader rhjunk;
   InAgain >> rhjunk;
      // ...write out the full one
   ROutStr << rh;
      // write header to BINEX, also all the nav information
   //if(!BinexFile.empty()) {
   //   writeBinex(BinexOut, rh, char(4));     // 4 means 'from native receiver format'
   //      // open nav file RinexNavFile, read it all and write it all to Binex
   //   RinexNavStream rns(RinexNavFile.c_str(),ios::in);
   //   if(!rns) {
   //      cerr << "Failed to re-open output nav file " << RinexNavFile << endl;
   //      return -3;
   //   }
   //   rns.exceptions(fstream::failbit);
   //      // ignore the header
   //   RinexNavHeader rnh;
   //   rns >> rnh;
   //   RinexNavData rnd;
   //   while(rns >> rnd) writeBinex(BinexOut, rnd);
   //   rns.close();
   //}

   RinexObsData robs;
   while(InAgain >> robs) {
      ROutStr << robs;
      //if(!BinexFile.empty()) writeBinex(BinexOut, robs);
   }

   InAgain.close();
   ROutStr.close();
   //if(!BinexFile.empty()) BinexOut.close();

      // delete the temporary
   if(remove(TempFile.c_str()) != 0) {
      cerr << "Error: Could not remove existing temp file: " << TempFile << endl;
      return -1;
   }
   else if(Debug) cout << "Deleted temporary file " << TempFile << endl;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
int GetCommandInput(int argc, char **argv) throw(Exception)
{
try {
   int i,j;
   vector<string> values;

   // --------------------------------------------------------------------------------
   // set all the defaults
   Debug = help = verbose = false;
   BegTime = DayTime::BEGINNING_OF_TIME;
   EndTime = DayTime::END_OF_TIME;
   //NovatelFile,
   RinexObsFile = string("RnovaRinex.obs");
   RinexNavFile = string("RnovaRinex.nav");
   //BinexFile = string();
   InputDirectory = string("");
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
      'f',""," [-f|--file] <fn>  Name of file containing more options"
      " (ignores '#' to EOL)");

   CommandOption dashdir(CommandOption::hasArgument, CommandOption::stdType,0,"dir",
      " --dir <dir>       Directory in which to find input file (defaults to ./)");
   dashdir.setMaxCount(1);

   CommandOption dashobs(CommandOption::hasArgument, CommandOption::stdType,0,"obs",
      " --obs <file>      RINEX observation output file (RnovaRinex.obs)");
   dashobs.setMaxCount(1);

   CommandOption dashnav(CommandOption::hasArgument, CommandOption::stdType,0,"nav",
      " --nav <file>      RINEX navigation output file (RnovaRinex.nav)");
   dashnav.setMaxCount(1);

   //CommandOption dashbin(CommandOption::hasArgument, CommandOption::stdType,0,"bin",
   //   " --bin <file>      BINEX (binary) output file (RnovaRinex.bnx)");
   //dashbin.setMaxCount(1);

   CommandOption dashNHF(CommandOption::hasArgument, CommandOption::stdType,0,
      "noHDopt", "\nOutput RINEX header fields:\n --noHDopt         If present, "
      "do not fill optional records in the output RINEX header");
   dashNHF.setMaxCount(1);

   CommandOption dashHDp(CommandOption::hasArgument, CommandOption::stdType,0,"HDp",
      " --HDp <program>   Set output RINEX header 'program' field ('"
      + roh.fileProgram + "')");
   dashHDp.setMaxCount(1);

   CommandOption dashHDr(CommandOption::hasArgument, CommandOption::stdType,0,"HDr",
      " --HDr <run_by>    Set output RINEX header 'run by' field ('"
      + roh.fileAgency + "')");
   dashHDr.setMaxCount(1);

   CommandOption dashHDo(CommandOption::hasArgument, CommandOption::stdType,0,"HDo",
      " --HDo <obser>     Set output RINEX header 'observer' field ('"
      + roh.observer + "')");
   dashHDo.setMaxCount(1);

   CommandOption dashHDa(CommandOption::hasArgument, CommandOption::stdType,0,"HDa",
      " --HDa <agency>    Set output RINEX header 'agency' field ('"
      + roh.agency + "')");
   dashHDa.setMaxCount(1);

   CommandOption dashHDm(CommandOption::hasArgument, CommandOption::stdType,0,"HDm",
      " --HDm <marker>    Set output RINEX header 'marker' field ('"
      + roh.markerName + "')");
   dashHDm.setMaxCount(1);

   CommandOption dashHDn(CommandOption::hasArgument, CommandOption::stdType,0,"HDn",
      " --HDn <number>    Set output RINEX header 'number' field ('"
      + roh.markerNumber + "')");
   dashHDn.setMaxCount(1);

   CommandOption dashHDrn(CommandOption::hasArgument, CommandOption::stdType,0,"HDrn",
      " --HDrn <number>   Set output RINEX header 'Rx number' field ('"
      + roh.recNo + "')");
   dashHDrn.setMaxCount(1);

   CommandOption dashHDrt(CommandOption::hasArgument, CommandOption::stdType,0,"HDrt",
      " --HDrt <type>     Set output RINEX header 'Rx type' field ('"
      + roh.recType + "')");
   dashHDrt.setMaxCount(1);

   CommandOption dashHDrv(CommandOption::hasArgument, CommandOption::stdType,0,"HDrv",
      " --HDrv <vers>     Set output RINEX header 'Rx version' field ('"
      + roh.recVers + "')");
   dashHDrv.setMaxCount(1);

   CommandOption dashHDan(CommandOption::hasArgument, CommandOption::stdType,0,"HDan",
      " --HDan <number>   Set output RINEX header 'antenna number' field ('"
      + roh.antNo + "')");
   dashHDan.setMaxCount(1);

   CommandOption dashHDat(CommandOption::hasArgument, CommandOption::stdType,0,"HDat",
      " --HDat <type>     Set output RINEX header 'antenna type' field ('"
      + roh.antType + "')");
   dashHDat.setMaxCount(1);

   CommandOption dashHDc(CommandOption::hasArgument, CommandOption::stdType,0,"HDc",
      " --HDc <comment>   Add comment to output RINEX headers (>1 allowed).");
   //dashHDc.setMaxCount(1);

   CommandOption dashobstype(CommandOption::hasArgument, CommandOption::stdType,
   0,"obstype","\nOutput RINEX observation data:\n"
   " --obstype <OT>    Output this RINEX (standard) obs type (i.e. <OT> is one of\n"
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

   CommandOptionNoArg dashdebias(0,"debias",
      " --debias          Remove an initial bias from the phase");
   dashdebias.setMaxCount(1);

   CommandOptionNoArg dashhelp('h',"help",
      " [-h|--help]       print this message and quit");
   dashhelp.setMaxCount(1);

   CommandOptionNoArg dashVerbose('v', "verbose",
      " --verbose         print more information");
   dashVerbose.setMaxCount(1);

   CommandOptionNoArg dashDebug('d',"debug",
      " [-d|--debug]      print much more information");
   dashDebug.setMaxCount(1);

   // ... other options
   CommandOptionRest Rest("");

   // --------------------------------------------------------------------------------
   // Define the parser here, after the options -- this is the 'prgm description'
   CommandOptionParser Par(
" Prgm " + Prgm + " (" + Vers + ") will open and read a binary Novatel file\n"
"  (OEM2 and OEM4 receivers are supported), and convert the data to RINEX format\n"
"  observation and navigation files. The RINEX header is filled using user input\n"
"  (see below), and optional records are filled. Input is on the command line,\n"
"  or of the same format in a file (--file <file>).\n");

   // parse the command line
   // allow user to put all options in a file
   // PreProcessArgs pulls out help and Debug
   vector<string> Args;
   for(j=1; j<argc; j++) PreProcessArgs(argv[j],Args);

   if(Args.size()==0) help = true;

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
      if(argc <= 2) return 1;
   }

   // check for errors on the command line
   if (Par.hasErrors() || Rest.getCount()) {
      cerr << "\nErrors found in command line input:\n";
      if(Par.hasErrors()) Par.dumpErrors(cerr);
      if(Rest.getCount()) {
         cerr << "The following command line fields were not recognized:\n";
         values = Rest.getValue();
         for(i=0; i<values.size(); i++)
            cerr << "  " << values[i] << endl;
      }
      cerr << "...end of Errors. Abort.\n";
      help = true;
   }

   if(help && argc > 1)
      cout << endl << "--------- parsed input:" << endl;

   // --------------------------------------------------------------------------------
   // pull out the parsed input

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
      if(help) cout << " Input RINEX obs file name " << values[0] << endl;
      RinexObsFile = values[0];
   }
   if(dashnav.getCount()) {
      values = dashnav.getValue();
      if(help) cout << " Input RINEX nav file name " << values[0] << endl;
      RinexNavFile = values[0];
   }
   //if(dashbin.getCount()) {
   //   values = dashbin.getValue();
   //   if(help) cout << " Input BINEX file name " << values[0] << endl;
   //   BinexFile = values[0];
   //}
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
      for(i=0; i<values.size(); i++) {
         if(help) cout << " Input comment for headers " << values[i] << endl;
         HDcomments.push_back(values[i]);
      }
   }
   if(dashobstype.getCount()) {
      values = dashobstype.getValue();
      for(i=0; i<values.size(); i++) {
         RinexObsHeader::RinexObsType rot;
         rot = RinexObsHeader::convertObsType(values[i]);
         OutputTypes.push_back(rot);
         if(help) cout << " Input output RINEX obs type " << values[i] << endl;
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
      if(help) cout << " Turn on debiasing of the phase " << endl;
      debias = true;
   }
   // help and Debug are pulled out by PreProcessArgs
   //if(dashhelp.getCount()) {
   //   help = true;
   //}
   //if(dashDebug.getCount()) {
   //   Debug = true;
   //}
   if(dashVerbose.getCount() || Debug) verbose = true;


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
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Pull out --debug --help and --file
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

         // process the buffer before checking eof or bad b/c there can be
         // a line at EOF that has no CRLF...
         while(!buffer.empty()) {
            word = firstWord(buffer);
            if(again_cfg_file) {
               word = "-f" + word;
               again_cfg_file = false;
               PreProcessArgs(word.c_str(),Args);
            }
            else if(word[0] == '#') { // skip to end of line
               buffer.clear();
            }
            else if(word == "--file" || word == "-f")
               again_cfg_file = true;
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
   else if((arg[0]=='-' && arg[1]=='d') || string(arg)==string("--debug"))
      Debug = true;
   else if((arg[0]=='-' && arg[1]=='h') || string(arg)==string("--help"))
      help = true;
   else if(string(arg) == "--file" || string(arg) == "-f")
      found_cfg_file = true;
   else Args.push_back(arg);
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}


//------------------------------------------------------------------------------------
void DumpCommandLine(ostream& ofs) throw(Exception)
{
try {
   int i;

   ofs << "Summary of command line input:" << endl;
   ofs << " Debug is " << (Debug ? "on":"off") << endl;
   ofs << " Verbose is " << (verbose ? "on":"off") << endl;
   if(!InputDirectory.empty()) ofs << " Path for input Novatel file is "
      << InputDirectory << endl;
   ofs << " Input Novatel file is: " << NovatelFile << endl;
   ofs << " Output RINEX obs file is: " << RinexObsFile << endl;
   ofs << " Output RINEX nav file is: " << RinexNavFile << endl;
   //ofs << " Output BINEX (obs/nav) file is: " << BinexFile << endl;
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
   ofs << " Output RINEX observation types (if found in the data):\n";
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
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
