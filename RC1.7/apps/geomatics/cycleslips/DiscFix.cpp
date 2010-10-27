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

//---------------------------------------------------------------------------------
// DiscFix.cpp:
// Read a RINEX observation file containing dual frequency pseudorange and phase,
// separate the data into satellite passes, then find and estimate discontinuities
// in the phase (using the GPSTk Discontinuity Corrector (GDC) of DiscCorr.hpp).
// The corrected data can be written out to another RINEX file, plus there is the
// option to smooth the pseudorange and/or debias the phase (SatPass::smooth()).
//
// This program is useful as a way to process RINEX data by satellite pass.
// It reads an entire RINEX obs file, breaks it into satellite passes (SatPass)
// and processes it (ProcessSatPass()), then writes it out from SatPass data.
// It was designed so that all the input data gets into one SatPass and is altered
// only by the routine(s) called in ProcessSatPass(). Thus, by modifying just that
// one routine, this program could be used to do something else to the satellite
// passes. Note that there is a choice of when to write out the data:
// either as soon as possible, or only at the end (cf. bool WriteASAP).
//---------------------------------------------------------------------------------

/**
 * @file DiscFix.cpp
 * Correct phase discontinuities (cycle slips) in dual-frequency data from a RINEX
 * observation file; optionally smooth the pseudoranges and/or debias the phases.
 */

#include "MathBase.hpp"
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "DayTime.hpp"
#include "StringUtils.hpp"

#include "SatPass.hpp"
#include "DiscCorr.hpp"

#include <ctime>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
// prgm data

string PrgmVers("5.0 8/20/07");
string PrgmName("DiscFix");

typedef struct configuration {
      // input
   string Directory;
   vector<string> InputObsName;
      // data flow
   double ith;
   DayTime begTime, endTime;
   double MaxGap;
   //int MinPts;
      // processing
   double dt;
   bool UseCA;
   vector<GSatID> ExSV;
   GSatID SVonly;
      // output files
   string LogFile,OutFile;
   ofstream oflog,ofout;
   string format;
      // output
   string OutRinexObs;
   string HDPrgm;         // header of output RINEX file
   string HDRunby;
   string HDObs;
   string HDAgency;
   string HDMarker;
   string HDNumber;
   int NrecOut;
   DayTime FirstEpoch,LastEpoch;
   bool smoothPR,smoothPH,smooth;
   bool WriteASAP;  // If true, write to RINEX only after ALL data has been processed.
   //bool CAOut;
   //bool DopOut;
   bool verbose;
      // estimate dt from data
   double estdt[9];
   int ndt[9];
} DFConfig;

//------------------------------------------------------------------------------------
// data input from command line

DFConfig config;                 // for DiscFix
GDCconfiguration GDConfig;       // the discontinuity corrector configuration

// data used in program

clock_t totaltime;
string Title;
DayTime CurrEpoch, PrgmEpoch;

RinexObsStream irfstr, orfstr;      // input and output RINEX files
RinexObsHeader rhead;
int inC1,inP1,inP2,inL1,inL2;      // indexes in rhead of C1, C1/P1, P2, L1 and L2
bool UsingCA;

// Data for an entire pass is stored in SatPass object:
// This contains all the SatPasses defined so far.
// The parallel vector holds an iterator for use in writing out the data.
vector<SatPass> SPList;
// convenience
static const string L1="L1",L2="L2",P1="P1",P2="P2",C1="C1";
// list of observation types to be included in each SatPass
vector<string> obstypes;
// this is a map relating a satellite to the index in SVPList of the current pass
vector<unsigned int> SPIndexList;
map<GSatID,int> SatToCurrentIndexMap;

//------------------------------------------------------------------------------------
// prototypes
int ReadFile(int nfile) throw(Exception);
int ProcessOneEntireEpoch(RinexObsData& ro) throw(Exception);
int ProcessOneSatOneEpoch(GSatID, DayTime, unsigned short&,
      vector<double>&, vector<unsigned short>&, vector<unsigned short>&)
   throw(Exception);

void ProcessSatPass(int index) throw(Exception);
int AfterReadingFiles(void) throw(Exception);
void WriteToRINEXfile(void) throw(Exception);
void WriteRINEXheader(void) throw(Exception);
void WriteRINEXdata(DayTime& WriteEpoch, const DayTime targetTime) throw(Exception);

void PrintSPList(ostream&, string, vector<SatPass>&, bool printTime);
int GetCommandLine(int argc, char **argv) throw(Exception);
void PreProcessArgs(const char *arg, vector<string>& Args) throw(Exception);

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
   try {
      totaltime = clock();
      int iret;

         // Title and description
      //cout << "Name " << string(argv[0]) << endl;
      Title = PrgmName + ", part of the GPS ToolKit, Ver " + PrgmVers + ", Run ";
      PrgmEpoch.setLocalTime();
      Title += PrgmEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S");
      cout << Title;

         // set fill char in GSatID
      config.SVonly.setfill('0');
      config.FirstEpoch = DayTime::BEGINNING_OF_TIME;
      config.LastEpoch = DayTime::BEGINNING_OF_TIME;
      CurrEpoch = DayTime::BEGINNING_OF_TIME;

         // get command line
      iret = GetCommandLine(argc, argv);
      if(iret) return iret;

         // configure SatPass
      {
         obstypes.push_back(L1);    // DiscFix requires these 4 observables only
         obstypes.push_back(L2);
         obstypes.push_back(UsingCA ? C1 : P1);
         obstypes.push_back(P2);

         SatPass dummy(config.SVonly,config.dt);
         dummy.setMaxGap(config.MaxGap);
         dummy.setOutputFormat(config.format);
      }

         // open output files
         // output for editing commands - write to this in ProcessSatPass()
      config.ofout.open(config.OutFile.c_str());
      if(!config.oflog.is_open()) {
         config.oflog << "Error: " << PrgmName << " failed to open output file "
            << config.OutFile << endl;
      }
      else
         cout << PrgmName << " is writing to output file " << config.OutFile << endl;

         // RINEX output
      orfstr.open(config.OutRinexObs.c_str(), ios::out);
      if(!config.OutRinexObs.empty()) {
         if(!orfstr.is_open()) {
            config.oflog << "Failed to open output file " << config.OutRinexObs
                         << ". Abort." << endl;
            cout << "Failed to open output file " << config.OutRinexObs
                 << ". Abort." << endl;
            irfstr.close();
            return 1;
         }
         else cout << PrgmName << " is writing to RINEX file "
            << config.OutRinexObs << endl;
         orfstr.exceptions(ios::failbit);
      }

         // loop over input files
      for(int nfile=0; nfile<config.InputObsName.size(); nfile++) {
         iret = ReadFile(nfile);
         if(iret < 0) break;
      }   // end loop over input files

      iret = AfterReadingFiles();

         // clean up
      orfstr.close();
      config.ofout.close();
      SatToCurrentIndexMap.clear();
      SPList.clear();
      SPIndexList.clear();

      totaltime = clock()-totaltime;
      config.oflog << PrgmName << " timing: " << fixed << setprecision(3)
                   << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds." << endl;
      cout << PrgmName << " timing: " << fixed << setprecision(3)
           << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds." << endl;

      config.oflog.close();

      return iret;
   }
   catch(Exception& e) {
      config.oflog << e;
   }
   catch (...) {
      config.oflog << PrgmName << ": Unknown error.  Abort." << endl;
      cout << PrgmName << ": Unknown error.  Abort." << endl;
   }

   return -1;

}   // end main()

//------------------------------------------------------------------------------------
// open the file, read header and check for data; then loop over the epochs
// Return 0 ok, <0 fatal error, >0 non-fatal error (ie skip this file)
// 0 ok, 1 couldn't open file, 2 file doesn't have required data
int ReadFile(int nfile) throw(Exception)
{
   try {
      string name;
         // open input file
      name = config.InputObsName[nfile];
      if(!config.Directory.empty() && config.Directory != string("."))
         name = config.Directory + string("/") + name;

      irfstr.open(name.c_str(),ios::in);
      if(! irfstr.is_open()) {
         config.oflog << "Failed to open input file " << name << ". Abort." << endl;
         cout << "Failed to open input file " << name << ". Abort." << endl;
         return 1;
      }
      else if(config.verbose)
         config.oflog << "Opened input file " << name << endl;
      irfstr.exceptions(ios::failbit);

         // read the header
      irfstr >> rhead;
      if(config.verbose) {
         config.oflog << "Here is the input header for file " << name << endl;
         rhead.dump(config.oflog);
         config.oflog << endl;
      }

         // check that file contains C1/P1,P2,L1,L2
      inC1 = inP1 = inP2 = inL1 = inL2 = -1;
      for(int j=0; j<rhead.obsTypeList.size(); j++) {
         if(rhead.obsTypeList[j] == RinexObsHeader::convertObsType("C1")) inC1=j;
         if(rhead.obsTypeList[j] == RinexObsHeader::convertObsType("L1")) inL1=j;
         if(rhead.obsTypeList[j] == RinexObsHeader::convertObsType("L2")) inL2=j;
         if(rhead.obsTypeList[j] == RinexObsHeader::convertObsType("P1")) inP1=j;
         if(rhead.obsTypeList[j] == RinexObsHeader::convertObsType("P2")) inP2=j;
      }
      config.oflog << "Indexes are:"
         << " C1=" << inC1
         << " L1=" << inL1
         << " L2=" << inL2
         << " P1=" << inP1
         << " P2=" << inP2
         << endl;

      if((inC1 == -1 && config.UseCA) ||             // no C1, but user wants C1
         (inP1 == -1 && inC1 == -1) ||               // no C1 and no P1
          inP2 == -1 || inL1 == -1 || inL2 == -1)
      {
         config.oflog << "Error: file " << name << " does not contain";
         if(inC1 == -1) config.oflog
            << " C1 (--forceCA was" << (config.UseCA ? "" : " not") << " found)";
         if(inL1 == -1) config.oflog << " L1";
         if(inL2 == -1) config.oflog << " L2";
         if(inP1 == -1) config.oflog << " P1";
         if(inP2 == -1) config.oflog << " P2";
         config.oflog << " .. abort." << endl;
         irfstr.clear();
         irfstr.close();
         return 2;
      }
      else if(inP1==-1) {
         inP1 = inC1;
         config.UseCA = true;
      }

      if(inP1 == inC1) UsingCA = true; else UsingCA = false;

         // loop over epochs in the file
      bool first=true;
      int iret;
      RinexObsData rodata;
      while(1) {
         irfstr >> rodata;
         if(irfstr.eof()) break;
         if(irfstr.bad()) {
            config.oflog << "input RINEX stream is bad" << endl;
            break;
         }
         iret = ProcessOneEntireEpoch(rodata);
         if(iret < -1) break;
         if(iret == -1) { iret=0; break; }  // end of file
      }

      irfstr.clear();
      irfstr.close();

      return iret;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e)
      { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Return : (return < -1 means fatal error)
//       -2 time tags were out of order - fatal
//       -1 end of file (or past end time limit),
//        0 ok,
//        1 skip this epoch : before begin time
//        2 skip this epoch : comment block,
//        3 skip this epoch : decimated
int ProcessOneEntireEpoch(RinexObsData& roe) throw(Exception)
{
   try {
      bool ok;
      int i,j,k,iret;
      double dt;
      string datastr;
      GSatID sat;
      unsigned short flag;
      vector<double> data;
      vector<unsigned short> lli,ssi;
      RinexObsData::RinexObsTypeMap otmap;
      RinexObsData::RinexSatMap::iterator it;
      RinexObsData::RinexObsTypeMap::const_iterator jt;

         // stay within time limits
      if(roe.time < config.begTime) return 1;
      if(roe.time > config.endTime) return -1;

         // ignore comment blocks ...
      if(roe.epochFlag != 0 && roe.epochFlag != 1) return 2;

         // decimate data
         // if begTime is still undefined, set it to begin of week
      if(config.ith > 0.0) {
         if(fabs(config.begTime-DayTime::BEGINNING_OF_TIME) < 1.e-8)
            config.begTime =
               config.begTime.setGPSfullweek(roe.time.GPSfullweek(),0.0);
         double dt=fabs(roe.time - config.begTime);
         dt -= config.ith*long(0.5+dt/config.ith);
         if(fabs(dt) > 0.25) return 3;            // TD set tolerance? clock bias?
      }

         // save current time
      CurrEpoch = roe.time;

         // loop over sat=it->first, ObsTypeMap=it->second
      for(it=roe.obs.begin(); it != roe.obs.end(); ++it) {

            // Is this satellite excluded ?
         sat = it->first;
         if(sat.system != SatID::systemGPS) continue; // ignore non-GPS satellites
         for(k=-1,i=0; i<config.ExSV.size(); i++)     // ignore input sat (--exSat)
            if(config.ExSV[i] == sat) { k = i; break; }
         if(k > -1) continue;

            // if only one satellite is included, skip all the rest
         if(config.SVonly.id != -1 && !(sat == config.SVonly)) continue;

            // pull out the data and the SSI and LLI (indicators)
         data = vector<double>(4,0.0);
         lli = vector<unsigned short>(4,0);
         ssi = vector<unsigned short>(4,0);
         otmap = it->second;
         if( (jt = otmap.find(rhead.obsTypeList[inL1])) != otmap.end()) {
            data[0] = jt->second.data;
            lli[0] = jt->second.lli;
            ssi[0] = jt->second.ssi;
         }
         if( (jt = otmap.find(rhead.obsTypeList[inL2])) != otmap.end()) {
            data[1] = jt->second.data;
            lli[1] = jt->second.lli;
            ssi[1] = jt->second.ssi;
         }
         if( (jt = otmap.find(rhead.obsTypeList[inP1])) != otmap.end()) {
            data[2] = jt->second.data;
            lli[2] = jt->second.lli;
            ssi[2] = jt->second.ssi;
         }
         if( (jt = otmap.find(rhead.obsTypeList[inP2])) != otmap.end()) {
            data[3] = jt->second.data;
            lli[3] = jt->second.lli;
            ssi[3] = jt->second.ssi;
         }

            // is it good?
         ok = true;
         //don't do this! if(spd.P1 < 1000.0 || spd.P2 < 1000.0) ok = false;
         if(fabs(data[0]) <= 0.001 || fabs(data[1]) <= 0.001 ||
            fabs(data[2]) <= 0.001 || fabs(data[3]) <= 0.001) ok = false;
         flag = (ok ? SatPass::OK : SatPass::BAD);

            // process this sat
         iret = ProcessOneSatOneEpoch(sat, CurrEpoch, flag, data, lli, ssi);
         if(iret == -2) {
            config.oflog << "Error: time tags are out of order. Abort." << endl;
            return -2;
         }

      }  // end loop over sats

         // update LastEpoch and estimate of config.dt
      if(config.LastEpoch > DayTime::BEGINNING_OF_TIME) {
         double dt = CurrEpoch-config.LastEpoch;
         for(i=0; i<9; i++) {
            if(config.ndt[i] <=0 ) { config.estdt[i]=dt; config.ndt[i]=1; break; }
            if(fabs(dt-config.estdt[i]) < 0.0001) { config.ndt[i]++; break; }
            if(i == 8) {
               k = 0;
               int nl=config.ndt[k];
               for(j=1; j<9; j++) if(config.ndt[j] <= nl) {
                  k = j;
                  nl = config.ndt[j];
               }
               config.ndt[k] = 1;
               config.estdt[k] = dt;
            }
         }
      }
      config.LastEpoch = CurrEpoch;

         // check times looking for passes that ought to be processed
      for(i=0; i<SPList.size(); i++) {
         if(SPList[i].status() > 1)
            continue;                          // already processed
         if(SPList[i].includesTime(CurrEpoch))
            continue;                          // don't process yet

         ProcessSatPass(i);                    // ok, process this pass
         if(!orfstr) SPList[i].status() = 99;    // status == 99 means 'written out'
      }

      // try writing more data to output RINEX file
      if(config.WriteASAP) {
         WriteToRINEXfile();
         // gut passes that have 99
         //for(i=0; i<SPList.size(); i++) {
         //   if(SPList[i].status() != 99) continue;
         //   SPList[i].resize(0);
         //}
      }

      return 0;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e)
      { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// return -2 if time tags are out of order,
//         0 normal = data was added
int ProcessOneSatOneEpoch(GSatID sat, DayTime tt, unsigned short& flag,
      vector<double>& data, vector<unsigned short>& lli, vector<unsigned short>& ssi)
   throw(Exception)
{
   try {
      int index,iret;
      map<GSatID,int>::const_iterator kt;

         // find the current SatPass for this sat
      kt = SatToCurrentIndexMap.find(sat);

         // if there is not one, create one
      if(kt == SatToCurrentIndexMap.end()) {
         SatPass newSP(sat,config.dt,obstypes);
         SPList.push_back(newSP);
         SPIndexList.push_back(99999);                  // keep parallel
         SatToCurrentIndexMap[sat] = SPList.size()-1;
         kt = SatToCurrentIndexMap.find(sat);
      }

         // update the first epoch
      if(config.FirstEpoch == DayTime::BEGINNING_OF_TIME)
         config.FirstEpoch = CurrEpoch;

         // get the index of this SatPass in the SPList vector
         // and add the data to that SatPass
      index = kt->second;
      SPList[index].status() = 1;                // status == 1 means 'fill'
      iret = SPList[index].addData(tt, obstypes, data, lli, ssi, flag);
      if(iret == -2) return -2;                 // time tags are out of order
      if(iret >= 0) return 0;                   // data was added successfully

         // --- need to create a new pass ---

         // first process the old one
      ProcessSatPass(index);
      if(!orfstr)                         // not writing to RINEX
         SPList[index].status() = 99;       // status == 99 means 'written out'
      else if(config.WriteASAP)
         WriteToRINEXfile();              // try writing out

         // create a new SatPass for this sat
      SatPass newSP(sat,config.dt,obstypes);
         // add it to the list
      SPList.push_back(newSP);
      SPIndexList.push_back(99999);                  // keep parallel
         // get the new index
      index = SPList.size()-1;
         // and add it to the map
      SatToCurrentIndexMap[sat] = index;
         // add the data
      SPList[index].status() = 1;              // status == 1 means 'fill'
      SPList[index].addData(tt, obstypes, data, lli, ssi, flag);

      return 0;

   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e)
      { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// Process the pass (call DC); if there is an output file, try writing to it.
void ProcessSatPass(int in) throw(Exception)
{
   try {
      config.oflog << "Proc " << SPList[in]
         << " at " << CurrEpoch.printf(config.format) << endl;
      //SPList[in].dump(config.oflog,"RAW");      // temp

      // remove this SatPass from the SatToCurrentIndexMap map
      SatToCurrentIndexMap.erase(SPList[in].getSat());

      // --------- call DC on this pass -------------------
      string msg;
      vector<string> EditCmds;
      int iret = DiscontinuityCorrector(SPList[in], GDConfig, EditCmds, msg);
      if(iret != 0) {
         SPList[in].status() = 100;         // status == 100 means 'failed'
         config.oflog << "GDC failed for SatPass " << in << " : "
            << (iret == -1 ? "Polynomial fit to GF data was singular" :
               (iret == -2 ? "Premature end" :     // never used
               (iret == -3 ? "Time interval DT not set" :
               (iret == -4 ? "No data found" :
               (iret == -5 ? "Required obs types (L1,L2,P1/C1,P2) not found" :
                             "Unknown"))))) << endl;
         return;
      }
      SPList[in].status() = 2;              // status == 2 means 'processed'.

      // --------- output editing commands ----------------
      for(int i=0; i<EditCmds.size(); i++)
         config.ofout << EditCmds[i] << endl;

      // --------- smooth pseudorange and debias phase ----
      if(config.smooth) {
         SPList[in].smooth(config.smoothPR,config.smoothPH,msg);
         config.oflog << msg << endl;
         SPList[in].status() = 3;           // status == 3 means 'smoothed'.
      }

      // status ==   0 means 'new'
      // status ==   1 means 'still being filled', so status MUST be set to >1 here
      // status ==   2 means 'processed'
      // status ==   3 means 'smoothed'
      // status ==  98 means 'writing out'
      // status ==  99 means 'written out'
      // status == 100 means 'failed'
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e)
      { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
int AfterReadingFiles(void) throw(Exception)
{
   try {
      config.oflog << "After reading files" << endl;

      // compute the estimated data interval and write it out
      for(int i=1; i<9; i++) if(config.ndt[i] > config.ndt[0]) {
         int j = config.ndt[i];            double est = config.estdt[i];
         config.ndt[i] = config.ndt[0];    config.estdt[i] = config.estdt[0];
         config.ndt[0] = j;                config.estdt[0] = est;
      }
      if(config.verbose)
         config.oflog << "Data interval estimated from the data is "
            << config.estdt[0] << " seconds." << endl;

      // process all the passes that have not been processed yet
      for(int i=0; i<SPList.size(); i++) {
         if(SPList[i].status() <= 1) {
            ProcessSatPass(i);
            if(!orfstr)                         // not writing out to RINEX
               SPList[i].status() = 99;         // status == 99 means 'written out'
         }
      }

      // write out all the (processed) data that has not already been written
      WriteToRINEXfile();

      // print a summary
      PrintSPList(config.oflog,"Fine",SPList,false);

      return 0;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e)
      { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
// this will only write out passes for which ProcessSatPass() has been called. It
// could be called anytime, particularly after each call to ProcessSatPass.
void WriteToRINEXfile(void) throw(Exception)
{
   if(!orfstr) return;
   try {
      int in,n;
      DayTime targetTime=DayTime::END_OF_TIME;
      static DayTime WriteEpoch(DayTime::BEGINNING_OF_TIME);

      // find all passes that have been newly processed (status > 1 but < 98)
      // mark these passes 'being written out' and initialize the iterator
      for(in=0; in<SPList.size(); in++) {
         if(SPList[in].status() > 1 && SPList[in].status() < 98) {
            SPList[in].status() = 98;       // status == 98 means 'being written out'
            SPIndexList[in] = 0;          // initialize iteration over the data array
         }
      }

      // find the earliest FirstTime of 'non-processed' (status==1) passes
      for(in=0; in<SPList.size(); in++) {
         if(SPList[in].status() == 1 && SPList[in].getFirstTime() < targetTime)
            targetTime = SPList[in].getFirstTime();
      }
      // targetTime will == END_OF_TIME, when all passes have been processed

      if(targetTime < DayTime::END_OF_TIME
         && WriteEpoch == DayTime::BEGINNING_OF_TIME) {
         WriteRINEXheader();
         WriteEpoch = config.FirstEpoch;
      }

      // nothing to do
      if(targetTime <= WriteEpoch)
         return;

      WriteRINEXdata(WriteEpoch,targetTime);
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e)
      { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void WriteRINEXheader(void) throw(Exception)
{
   try {
      RinexObsHeader rheadout;   

      config.oflog << "Write the output header at "
         << CurrEpoch.printf(config.format) << endl;

         // copy input
      rheadout = rhead;

         // change the obs type list to include only P1(C1) P2 L1 L2
      rheadout.obsTypeList.clear();

      rheadout.obsTypeList.push_back(RinexObsHeader::L1);
      rheadout.obsTypeList.push_back(RinexObsHeader::L2);
      if(UsingCA)
         rheadout.obsTypeList.push_back(RinexObsHeader::C1);
      else
         rheadout.obsTypeList.push_back(RinexObsHeader::P1);
      rheadout.obsTypeList.push_back(RinexObsHeader::P2);

         // fill records in output header
      rheadout.date = PrgmEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S");
      rheadout.fileProgram = PrgmName + string(" v.") + PrgmVers.substr(0,4)
         + string(",") + GDConfig.Version().substr(0,4);
      if(!config.HDRunby.empty()) rheadout.fileAgency = config.HDRunby;
      if(!config.HDObs.empty()) rheadout.observer = config.HDObs;
      if(!config.HDAgency.empty()) rheadout.agency = config.HDAgency;
      if(!config.HDMarker.empty()) rheadout.markerName = config.HDMarker;
      if(!config.HDNumber.empty()) rheadout.markerNumber = config.HDNumber;
      rheadout.version = 2.1; rheadout.valid |= RinexObsHeader::versionValid;
      rheadout.firstObs = config.FirstEpoch; rheadout.valid
         |= RinexObsHeader::firstTimeValid;
      rheadout.interval = config.dt; rheadout.valid |= RinexObsHeader::intervalValid;
      if(!config.WriteASAP) {
         rheadout.interval = config.estdt[0];
         rheadout.valid |= RinexObsHeader::intervalValid;
         rheadout.lastObs = config.LastEpoch;
         rheadout.valid |= RinexObsHeader::lastTimeValid;
      }
      if(config.smoothPR)
         rheadout.commentList.push_back(string("Ranges smoothed by ") +
            PrgmName + string(" v.") + PrgmVers.substr(0,4) + string(" ") +
            rheadout.date);
      if(config.smoothPH)
         rheadout.commentList.push_back(string("Phases debiased by ") +
            PrgmName + string(" v.") + PrgmVers.substr(0,4) + string(" ") +
            rheadout.date);
      if(config.smoothPR || config.smoothPH)
         rheadout.valid |= RinexObsHeader::commentValid;
         // invalidate the table
      if(rheadout.valid & RinexObsHeader::numSatsValid)
         rheadout.valid ^= RinexObsHeader::numSatsValid;
      if(rheadout.valid & RinexObsHeader::prnObsValid)
         rheadout.valid ^= RinexObsHeader::prnObsValid;

      orfstr << rheadout;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e)
      { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void WriteRINEXdata(DayTime& WriteEpoch, const DayTime targetTime) throw(Exception)
{
   try {
      bool first;
      int in,n;
      unsigned short flag;
      //string str;
      GSatID sat;
      RinexObsData roe;

      // loop over epochs, up to just before targetTime
      do {
            // find the next WriteEpoch = earliest iterator time among the status==98
         first = true;
         for(in=0; in<SPList.size(); in++) {
            if(SPList[in].status() != 98)   // status == 98 means 'being written out'
               continue;

            n = SPIndexList[in];   // current iterator index
            if(first || SPList[in].time(n) < WriteEpoch) {
               WriteEpoch = SPList[in].time(n);
               first = false;
            }
         }
         if(first) break;

            // quit if reached the target
         if(WriteEpoch >= targetTime) break;
            // prepare the RINEX obs data
         roe.epochFlag = 0;
         roe.time = WriteEpoch;
         roe.clockOffset = 0.0;  // TD save from input?
         roe.numSvs = 0;         // will be incremented below
         roe.obs.clear();

            // output all data at this WriteEpoch
         for(in=0; in<SPList.size(); in++) {
            if(SPList[in].status() != 98) continue;

            sat = SPList[in].getSat();
            n = SPIndexList[in];   // current iterator index

            if(fabs(SPList[in].time(n) - WriteEpoch) < 0.00001) {
                  // get the data for this epoch
					flag = SPList[in].getFlag(SPIndexList[in]);
					if(flag != SatPass::BAD) {                // data is good
                     // add sat to RinexObs
                  RinexObsData::RinexObsTypeMap rotm;
                  roe.obs[sat] = rotm;
                  roe.numSvs++;

               	// build the RINEX data object
               	RinexObsData::RinexDatum rd;

               	rd.lli = SPList[in].LLI(SPIndexList[in],P1);
               	rd.ssi = SPList[in].SSI(SPIndexList[in],P1);
               	rd.data = SPList[in].data(SPIndexList[in],P1);
               	if(UsingCA)
                  	roe.obs[sat][RinexObsHeader::C1] = rd;
               	else
                  	roe.obs[sat][RinexObsHeader::P1] = rd;

               	rd.lli = SPList[in].LLI(SPIndexList[in],P2);
               	rd.ssi = SPList[in].SSI(SPIndexList[in],P2);
               	rd.data = SPList[in].data(SPIndexList[in],P2);
               	roe.obs[sat][RinexObsHeader::P2] = rd;

               	//rd.lli = asInt(asString<char>(str[4]));
                  // TD ought to set the low bit
						rd.lli = (flag & SatPass::LL1) != 0 ? 1 : 0;
               	rd.ssi = SPList[in].SSI(SPIndexList[in],L1);
               	rd.data = SPList[in].data(SPIndexList[in],L1);
               	roe.obs[sat][RinexObsHeader::L1] = rd;

               	//rd.lli = asInt(asString<char>(str[6]));
						rd.lli = (flag & SatPass::LL2) != 0 ? 1 : 0;
               	rd.ssi = SPList[in].SSI(SPIndexList[in],L2);
               	rd.data = SPList[in].data(SPIndexList[in],L2);
               	roe.obs[sat][RinexObsHeader::L2] = rd;

               	config.oflog << "Out "
               	   << WriteEpoch.printf(config.format)
               	   << " " << roe.time.printf(config.format)
               	   << " " << sat
               	   << " " << flag
               	   << " " << setw(3) << SPList[in].getCount(SPIndexList[in])
               	   << fixed << setprecision(3)
               	   << " " << setw(13) << SPList[in].data(SPIndexList[in],P1)
               	   << " " << setw(13) << SPList[in].data(SPIndexList[in],P2)
               	   << " " << setw(13) << SPList[in].data(SPIndexList[in],L1)
               	   << " " << setw(13) << SPList[in].data(SPIndexList[in],L2)
               	   << endl;
					}

                  // go to next point
               SPIndexList[in]++;

                  // end of data?
               if(SPIndexList[in] >= SPList[in].size())
                  SPList[in].status() = 99;        // status == 99 means 'written out'
            }
         }

         // actually write to RINEX
         if(roe.numSvs > 0) {
            config.SVonly.setfill(' ');         // just for the hell of it
            orfstr << roe;
            config.SVonly.setfill('0');
         }

      } while(1);  // end while loop over all epochs up to targetTime

   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e)
      { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void PrintSPList(ostream& os, string msg, vector<SatPass>& v, bool printTime)
{
   int i,j,gap;
   GSatID sat;
   map<GSatID,int> lastSP;
   map<GSatID,int>::const_iterator kt;

   os << "#" << leftJustify(msg,4)
             << "  N gap  tot sat   ok  s      start time        end time   dt"
      << " observation types" << endl;

   for(i=0; i<v.size(); i++) {
      os << msg;
      sat = v[i].getSat();
      kt = lastSP.find(sat);
      if(kt == lastSP.end())
         gap = 0;
      else {
         j = kt->second;
         gap = int((v[i].getFirstTime() - v[j].getLastTime()) / v[i].getDT() + 0.5);
         lastSP.erase(sat);
      }
      lastSP[sat] = i;
         // n,gap,sat,length,ngood,firstTime,lastTime
      os << " " << setw(2) << i+1 << " " << setw(4) << gap << " " << v[i];
      if(printTime)
         os << " at " << CurrEpoch.printf(config.format);
//"%04Y/%02m/%02d %02H:%02M:%6.3f"
      os << endl;
   }
}

//------------------------------------------------------------------------------------
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"
int GetCommandLine(int argc, char **argv) throw(Exception)
{
   try {
   bool help=false,DChelp=false,DChelpall=false;
   int i,j;
      // defaults
   config.WriteASAP = true;   // this is not in the input...
   config.verbose = false;
   config.ith = 0.0;
   config.begTime = DayTime::BEGINNING_OF_TIME;
   config.endTime = DayTime::END_OF_TIME;
   config.MaxGap = 600.0;
   //config.MinPts = 10;

   config.LogFile = string("df.log");
   config.OutFile = string("df.out");
   config.format = string("%4F %10.3g");

   config.UseCA = false;                  // meaning use P1 unless its absent, then C1
                                          // true would mean use C1 only
   config.dt = -1.0;
   
   config.HDPrgm = PrgmName + string(" v.") + PrgmVers.substr(0,4);
   config.HDRunby = string("ARL:UT/SGL/GPSTk");

   config.smoothPR = false;
   config.smoothPH = false;
   config.smooth = false;
   //config.CAOut = false;
   //config.DopOut = false;

   for(i=0; i<9; i++) config.ndt[i]=-1;

   config.Directory = string(".");

      // -------------------------------------------------
      // required options
   RequiredOption dashi(CommandOption::hasArgument, CommandOption::stdType,
      0,"inputfile",
      " --inputfile <file>  Input (RINEX obs) file - more than one may be given");

   RequiredOption dashDT(CommandOption::hasArgument, CommandOption::stdType,
      0,"dt"," --dt <dt>           Time spacing (sec) of the data.  "
      "[NB this defines DT\n                       in the GDC, hence "
      + PrgmName + " ignores --DCDT=<dt> ]");
   dashDT.setMaxCount(1);
   
      // optional options
      // this only so it will show up in help page...
   CommandOption dashf(CommandOption::hasArgument, CommandOption::stdType,
      'f',""," [-f|--file] <file>  file containing more options");

   CommandOption dashd(CommandOption::hasArgument, CommandOption::stdType,
      0,"inputdir"," --inputdir <dir>    Directory of input file(s)");
   dashd.setMaxCount(1);

   // time
   CommandOption dashbt(CommandOption::hasArgument, CommandOption::stdType,
      0,"beginTime", "# Time limits:"
      " args are of the form GPSweek,sow OR YYYY,MM,DD,HH,Min,Sec\n"
      " --beginTime <arg>   Start time of processing (BOF)");
   dashbt.setMaxCount(1);

   CommandOption dashet(CommandOption::hasArgument, CommandOption::stdType,
      0,"endTime", " --endTime <arg>     End time of processing (EOF)");
   dashet.setMaxCount(1);

   CommandOption dashith(CommandOption::hasArgument, CommandOption::stdType,
      0,"decimate","# Data configuration\n"
      " --decimate <dt>     Decimate data to time interval (sec) dt");
   dashith.setMaxCount(1);

   CommandOptionNoArg dashCA(0,"forceCA", " --forceCA           "
      "Use C/A code range, NOT P code (default: only if P absent)");
   dashCA.setMaxCount(1);
   
   CommandOption dashGap(CommandOption::hasArgument, CommandOption::stdType,
      0,"gap"," --gap <t>           Minimum data gap (sec) separating "
      "satellite passes (" + asString(int(config.MaxGap)) + ")");
   dashGap.setMaxCount(1);
   
   //CommandOption dashPts(CommandOption::hasArgument, CommandOption::stdType,
      //0,"Points"," --Points <n>        Minimum number of points needed to "
      //"process a pass");
   //dashPts.setMaxCount(1);
   
   CommandOption dashSV(CommandOption::hasArgument, CommandOption::stdType,
      0,"onlySat"," --onlySat <sat>     Process only satellite <sat> "
      "(a GPS SatID, e.g. G21)");
   dashSV.setMaxCount(1);
   
   CommandOption dashXsat(CommandOption::hasArgument, CommandOption::stdType,
      0,"exSat"," --exSat <sat>       Exclude satellite(s) [e.g. --exSat G22]");
   
   CommandOptionNoArg dashSmoothPR(0,"smoothPR",
   "# Smoothing: [NB smoothed " "pseudorange and debiased phase are not identical.]\n"
   " --smoothPR          Smooth pseudorange and output in place of raw pseudorange");
   dashSmoothPR.setMaxCount(1);
   
   CommandOptionNoArg dashSmoothPH(0,"smoothPH",
      " --smoothPH          Debias phase and output in place of raw phase");
   dashSmoothPH.setMaxCount(1);

   // last smooth option - tack on a 'vapor-option'
   CommandOptionNoArg dashSmooth(0,"smooth",
   " --smooth            Same as (--smoothPR AND --smoothPH)\n"
   "# Discontinuity Corrector (DC) configuration:\n"
   " --DClabel=value     Set Discontinuity Corrector parameter 'label' to 'value'\n"
   "                       [e.g. --DCWLSigma=1.5 or --DCDebug:7 " "or --DCMinPts,6]\n"
   " --DChelp            Print a list of GDC parameters and their defaults, then quit"
   );
   dashSmooth.setMaxCount(1);
   
   CommandOption dashLog(CommandOption::hasArgument, CommandOption::stdType,
      0,"logOut","# Output:\n --logOut <file>     Output log file name ("
      + config.LogFile + ")");
   //dashLog.setMaxCount(1);
   
   CommandOption dashOut(CommandOption::hasArgument, CommandOption::stdType,
      0,"cmdOut"," --cmdOut <file>     Output file name (for editing commands) ("
      + config.OutFile + ")");
   dashOut.setMaxCount(1);
   
   CommandOption dashFormat(CommandOption::hasArgument, CommandOption::stdType,
      0,"format"," --format \"<format>\" Output time format (cf. gpstk::"
      "DayTime) (" + config.format + ")");
   dashFormat.setMaxCount(1);

   CommandOption dashRfile(CommandOption::hasArgument, CommandOption::stdType,
      0,"RinexFile","# RINEX output:\n"
      " --RinexFile <file>  RINEX (obs) file name for output of corrected data");
   //dashRfile.setMaxCount(1);
   
   CommandOption dashRrun(CommandOption::hasArgument, CommandOption::stdType,
      0,"RunBy"," --RunBy <string>    RINEX header 'RUN BY' string for output");
   dashRrun.setMaxCount(1);
   
   CommandOption dashRobs(CommandOption::hasArgument, CommandOption::stdType,
      0,"Observer"," --Observer <string> RINEX header 'OBSERVER' string for output");
   dashRobs.setMaxCount(1);
   
   CommandOption dashRag(CommandOption::hasArgument, CommandOption::stdType,
      0,"Agency"," --Agency <string>   RINEX header 'AGENCY' string for output");
   dashRag.setMaxCount(1);
   
   CommandOption dashRmark(CommandOption::hasArgument, CommandOption::stdType,
      0,"Marker"," --Marker <string>   RINEX header 'MARKER' string for output");
   dashRmark.setMaxCount(1);
   
   CommandOption dashRnumb(CommandOption::hasArgument, CommandOption::stdType,
      0,"Number"," --Number <string>   RINEX header 'NUMBER' string for output");
   dashRnumb.setMaxCount(1);
   
   // TD? pass-through 'other' data found in input RINEX file - requires buffering?
   //CommandOptionNoArg dashCAOut(0,"CAOut",
   //   " --CAOut             Output C/A code in RINEX");
   //dashCAOut.setMaxCount(1);
   
   //CommandOptionNoArg dashDOut(0,"DOut",
   //   " --DOut              Output Doppler in RINEX");
   //dashDOut.setMaxCount(1);
   
   CommandOptionNoArg dashh('h', "help",
      "# Help:\n"
      " [-h|--help]         print this syntax page and quit."
      "\n --DChelp            Print a list of GDC parameters and "
      "their defaults, and quit");
   dashh.setMaxCount(1);

   CommandOptionNoArg dashVerb(0,"verbose",
      " --verbose           print extended output to the log file");
   dashVerb.setMaxCount(1);

   // ... other options
   CommandOptionRest Rest("");

   CommandOptionParser Par("Prgm " + PrgmName +
   " reads a RINEX observation data file containing GPS dual-frequency\n"
   "   pseudorange and carrier phase measurements, divides the data into 'satellite\n"
   "   passes', and finds and fixes discontinuities in the phases for each pass.\n"
   "   Output is a list of editing commands for use with program RinexEdit.\n"
   "   " + PrgmName
   + " will (optionally) write the corrected pseudorange and phase data\n"
   "   to a new RINEX observation file. Other options will also smooth the\n"
   "   pseudorange and/or debias the corrected phase.\n"
   "   "+PrgmName+" calls the GPSTk Discontinuity Corrector (GDC vers "
   + GDConfig.Version() + ").\n");

      // -------------------------------------------------
      // allow user to put all options in a file
      // could also scan for debug here
   vector<string> Args;
   for(j=1; j<argc; j++) PreProcessArgs(argv[j],Args);

   if(Args.size()==0) Args.push_back(string("-h"));

      // strip out the DCcmds
   vector<string> DCcmds;
   vector<string>::iterator it=Args.begin();
   while(it != Args.end()) {
      if(it->substr(0,4) == string("--DC")) {
         if(*it == "--DChelp") DChelp=true;
         else if(*it == "--DChelpall" || *it == "--DCall") DChelp=DChelpall=true;
         else DCcmds.push_back(*it);
         it = Args.erase(it);
      }
      else it++;
   }

      // pass the rest
   argc = Args.size()+1;
   char **CArgs=new char*[argc];
   if(!CArgs) { cout << "Failed to allocate CArgs" << endl; return -1; }
   CArgs[0] = argv[0];
   for(j=1; j<argc; j++) {
      CArgs[j] = new char[Args[j-1].size()+1];
      if(!CArgs[j]) { cout << "Failed to allocate CArgs[j]" << endl; return -1; }
      strcpy(CArgs[j],Args[j-1].c_str());
   }
   Par.parseOptions(argc, CArgs);
   delete[] CArgs;

   // -------------------------------------------------

   if(dashh.getCount() > 0) help = true;
   if(Par.hasErrors()) {
      if(!help && !DChelp) {
         cout << "Errors found in command line input:" << endl;
         Par.dumpErrors(cout);
         cout << "...end of Errors.  For help run with option --help"
              << endl << endl;
      }
      help = true;
   }
   
   // -------------------------------------------------
   // get values found on command line

   string msg;
   vector<string> field;
   vector<string> values;

      // f never appears because we intercept it above
   //if(dashf.getCount()) { cout << "Option f "; dashf.dumpValue(cout); }

      // do help first
   if(dashh.getCount()) help=true;

      // now get the rest of the options
   if(dashVerb.getCount()) config.verbose=true;
   if(dashi.getCount()) {
      values = dashi.getValue();
      if(help) cout << "Input RINEX obs files are:" << endl;
      for(i=0; i<values.size(); i++) {
         config.InputObsName.push_back(values[i]);
         if(help) cout << "   " << values[i] << endl;
      }
   }
   if(dashd.getCount()) {
      values = dashd.getValue();
      config.Directory = values[0];
      if(help) cout << "Input Directory is " << config.Directory << endl;
   }
   if(dashith.getCount()) {
      values = dashith.getValue();
      config.ith = asDouble(values[0]);
      if(help) cout << "Decimate value is " << config.ith << endl;
   }

   // TD put try {} around setToString and catch invalid formats...
   if(dashbt.getCount()) {
      values = dashbt.getValue();
      msg = values[0];
      field.clear();
      while(msg.size() > 0)
         field.push_back(stripFirstWord(msg,','));
      if(field.size() == 2)
         config.begTime.setToString(field[0]+","+field[1], "%F,%g");
      else if(field.size() == 6)
         config.begTime.setToString(field[0]+","+field[1]+","+field[2]+","
            +field[3]+","+field[4]+","+field[5], "%Y,%m,%d,%H,%M,%S");
      else {
         cout << "Error: invalid --beginTime input: " << values[0] << endl;
      }
      if(help) cout << " Input: begin time " << values[0] << " = "
         << config.begTime.printf("%Y/%02m/%02d %2H:%02M:%06.3f = %F/%10.3g") << endl;
   }
   if(dashet.getCount()) {
      values = dashet.getValue();
      msg = values[0];
      field.clear();
      while(msg.size() > 0)
         field.push_back(stripFirstWord(msg,','));
      if(field.size() == 2)
         config.endTime.setToString(field[0]+","+field[1], "%F,%g");
      else if(field.size() == 6)
         config.endTime.setToString(field[0]+","+field[1]+","+field[2]
            +","+field[3]+","+field[4]+","+field[5], "%Y,%m,%d,%H,%M,%S");
      else {
         cout << "Error: invalid --endTime input: " << values[0] << endl;
      }
      if(help) cout << " Input: end time " << values[0] << " = "
         << config.endTime.printf("%Y/%02m/%02d %2H:%02M:%06.3f = %F/%10.3g") << endl;
   }

   if(dashCA.getCount()) {
      config.UseCA = true;
      if(help) cout << "Input: Set the 'Use C/A code range' flag" << endl;
   }
   if(dashDT.getCount()) {
      values = dashDT.getValue();
      config.dt = asDouble(values[0]);
      if(help) cout << "dt is set to " << config.dt << " seconds." << endl;
   }
   if(dashGap.getCount()) {
      values = dashGap.getValue();
      config.MaxGap = asDouble(values[0]);
      if(help) cout << "Max gap is " << config.MaxGap << " seconds which is "
         << int(config.MaxGap/config.dt) << " points." << endl;
   }
   //if(dashPts.getCount()) {
      //values = dashPts.getValue();
      //config.MinPts = asInt(values[0]);
      //if(help) cout << "Minimum points is " << config.MinPts << endl;
   //}
   if(dashXsat.getCount()) {
      values = dashXsat.getValue();
      for(i=0; i<values.size(); i++) {
         GSatID p(values[i]);
         if(help) cout << "Exclude satellite " << p << endl;
         config.ExSV.push_back(p);
      }
   }
   if(dashSV.getCount()) {
      values = dashSV.getValue();
      GSatID p(values[0]);
      config.SVonly = p;
      if(help) cout << "Process only satellite : " << p << endl;
   }
   if(dashFormat.getCount()) {
      values = dashFormat.getValue();
      config.format = values[0];
      if(help) cout << "Output times with format: " << config.format << endl;
   }
   if(dashOut.getCount()) {
      values = dashOut.getValue();
      config.OutFile = values[0];
      if(help) cout << "Command output file is " << config.OutFile << endl;
   }
   if(dashRfile.getCount()) {
      values = dashRfile.getValue();
      // pick the last one entered
      config.OutRinexObs = values[values.size()-1];
      if(help) cout << "Output RINEX file name is "
         << config.OutRinexObs << endl;
   }
   if(dashRrun.getCount()) {
      values = dashRrun.getValue();
      config.HDRunby = values[0];
      if(help) cout << "Output RINEX 'RUN BY' is " << config.HDRunby << endl;
   }
   if(dashRobs.getCount()) {
      values = dashRobs.getValue();
      config.HDObs = values[0];
      if(help) cout << "Output RINEX 'OBSERVER' is " << config.HDObs << endl;
   }
   if(dashRag.getCount()) {
      values = dashRag.getValue();
      config.HDAgency = values[0];
      if(help) cout << "Output RINEX 'AGENCY' is " << config.HDAgency << endl;
   }
   if(dashRmark.getCount()) {
      values = dashRmark.getValue();
      config.HDMarker = values[0];
      if(help) cout << "Output RINEX 'MARKER' is " << config.HDMarker << endl;
   }
   if(dashRnumb.getCount()) {
      values = dashRnumb.getValue();
      config.HDNumber = values[0];
      if(help) cout << "Output RINEX 'NUMBER' is " << config.HDNumber << endl;
   }
   if(dashSmooth.getCount()) {
      config.smoothPH = config.smoothPR = true;
      if(help) cout << "'smooth both' option is on" << endl;
   }
   if(dashSmoothPR.getCount()) {
      config.smoothPR = true;
      if(help) cout << "smooth the pseudorange" << endl;
   }
   if(dashSmoothPH.getCount()) {
      config.smoothPH = true;
      if(help) cout << "debias the phase" << endl;
   }
   //if(dashCAOut.getCount()) {
   //   config.CAOut = true;
   //   if(help) cout << "Output the C/A code to RINEX" << endl;
   //}
   //if(dashDOut.getCount()) {
   //   config.DopOut = true;
   //   if(help) cout << "Output the doppler to RINEX" << endl;
   //}

   if(Rest.getCount() && help) {
      cout << "Remaining options:" << endl;
      values = Rest.getValue();
      for (i=0; i<values.size(); i++) cout << values[i] << endl;
   }
   //if(config.verbose && help) {
   //   cout << "Tokens on command line (" << Args.size() << ") are:"
   //      << endl;
   //   for(j=0; j<Args.size(); j++) cout << Args[j] << endl;
   //}

   //if(config.verbose) { // if GDCorrector::Debug is not set higher, set to 2
      //GDCorrector.SetParameter(string("Debug=2"));
   //}

      // if help, print usage and quit
   if(help || DChelp) {
      if(help) Par.displayUsage(cout,false);
      if(DChelp) {
         GDConfig.DisplayParameterUsage(cout,DChelpall);
         cout << "For " << PrgmName
              << ", GDC commands are of the form --DC<GDCcmd>,"
              << " e.g. --DCWLSigma=1.5" << endl;
      }
      return 1;
   }

      // get the log file name
   if(dashLog.getCount()) {
      values = dashLog.getValue();
      // pick the last one entered
      config.LogFile = values[values.size()-1];
      //if(help) cout << "Log file is " << config.LogFile << endl;
   }
      // open the log file
   config.oflog.open(config.LogFile.c_str(),ios::out);
   if(!config.oflog.is_open()) {
      cout << PrgmName << " failed to open log file "
           << config.LogFile << ". Abort." << endl;
      return -1;
   }
   else {
      cout << PrgmName << " is writing to log file " << config.LogFile << endl;
         // output first stuff to log file
      config.oflog << Title;
         // allow GDC to output to log file
      GDConfig.setDebugStream(config.oflog);
   }

   if(config.dt <= 0.0) {
      config.oflog << PrgmName << ": dt must be positive" << endl;
      return -1;
   }

   if(!config.smooth) config.smooth = (config.smoothPR || config.smoothPH);

      // set the commands now (setParameter may write to log file)
   for(i=0; i<DCcmds.size(); i++)
      GDConfig.setParameter(DCcmds[i]);
      // also, use the dt in SatPass to define the dt in GDC
      // NB this means --DCDT on the DiscFix command line is ignored!
   GDConfig.setParameter("DT",config.dt);

      // print config to log, first DF
   config.oflog << "Here is the " << PrgmName << " configuration:" << endl;
   config.oflog << " Input RINEX obs files are:" << endl;
   for(i=0; i<config.InputObsName.size(); i++) {
      config.oflog << "   " << config.InputObsName[i] << endl;
   }
   config.oflog << " Input Directory is " << config.Directory << endl;
   config.oflog << " Ithing time interval is " << config.ith << endl;
   if(config.begTime > DayTime::BEGINNING_OF_TIME)
   config.oflog << " Begin time is "
      << config.begTime.printf("%04Y/%02m/%02d %02H:%02M:%.3f")
      << " = " << config.begTime.printf("%04F/%10.3g") << endl;
   if(config.endTime < DayTime::END_OF_TIME)
      config.oflog << " End time is "
         << config.endTime.printf("%04Y/%02m/%02d %02H:%02M:%.3f")
         << " = " << config.endTime.printf("%04F/%10.3g") << endl;
   if(config.UseCA) config.oflog << " 'Use the C/A pseudorange' flag is set" << endl;
   else config.oflog << " Do not use C/A code range (C1) unless P1 is absent" << endl;
   config.oflog << " dt is set to " << config.dt << " seconds." << endl;
   config.oflog << " Max gap is " << config.MaxGap << " seconds which is "
      << int(config.MaxGap/config.dt) << " points." << endl;
   //config.oflog << " Minimum points is " << config.MinPts << endl;
   if(config.ExSV.size()) {
      config.oflog << " Exclude satellites";
      for(i=0; i<config.ExSV.size(); i++) {
         if(config.ExSV[i].id == -1) config.oflog << " (all "
            << config.ExSV[i].systemString() << ")" << endl;
         else config.oflog << " " << config.ExSV[i];
      }
      config.oflog << endl;
   }
   if(config.SVonly.id > 0)
      config.oflog << " Process only satellite : " << config.SVonly << endl;
   config.oflog << " Log file is " << config.LogFile << endl;
   config.oflog << " Out file is " << config.OutFile << endl;
   config.oflog << " Output times in this format " << config.format << endl;
   if(!config.OutRinexObs.empty())
      config.oflog << " Output RINEX file name is " << config.OutRinexObs << endl;
   if(!config.HDRunby.empty())
      config.oflog << " Output RINEX 'RUN BY' is " << config.HDRunby << endl;
   if(!config.HDObs.empty())
      config.oflog << " Output RINEX 'OBSERVER' is " << config.HDObs << endl;
   if(!config.HDAgency.empty())
      config.oflog << " Output RINEX 'AGENCY' is " << config.HDAgency << endl;
   if(!config.HDMarker.empty())
      config.oflog << " Output RINEX 'MARKER' is " << config.HDMarker << endl;
   if(!config.HDNumber.empty())
      config.oflog << " Output RINEX 'NUMBER' is " << config.HDNumber << endl;
   if(config.smoothPR) config.oflog << " 'Smoothed range' option is on" << endl;
   if(config.smoothPH) config.oflog << " 'Smoothed phase' option is on" << endl;
   if(!config.smooth) config.oflog << " No smoothing." << endl;
   //if(config.CAOut) config.oflog << " 'C/A output' option is on" << endl;
   //if(config.DopOut) config.oflog << " 'Doppler output' option is on" << endl;

      // print config to log, second GDC
   config.oflog << "Here is the GPSTk DC configuration:" << endl;
   GDConfig.DisplayParameterUsage(config.oflog,DChelpall);
   config.oflog << endl;

   return 0;

   } // end try
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e)
      { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
void PreProcessArgs(const char *arg, vector<string>& Args) throw(Exception)
{
   try {
      static bool found_cfg_file=false;

      if(string(arg) == string()) return;

      if(found_cfg_file || (arg[0]=='-' && arg[1]=='f')) {
         string filename(arg);
         if(!found_cfg_file) filename.erase(0,2); else found_cfg_file = false;
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
               else if(word[0] == '#') {         // skip this line
                  buffer.clear();
               }
               else if(word == "--file" || word == "-f")
                  again_cfg_file = true;
               else if(word[0] == '"') {
                  word = stripFirstWord(buffer,'"');
                  buffer = "dummy " + buffer;
                  PreProcessArgs(word.c_str(),Args);
               }
               else
                  PreProcessArgs(word.c_str(),Args);

               word = stripFirstWord(buffer);   // this simply removes it from buffer
            }
            // break on EOF here b/c there can be a line w/o LF at EOF
            if(infile.eof() || !infile.good()) break;
         }
      }
      else if(string(arg) == "--file" || string(arg) == "-f")
         found_cfg_file = true;
      // -v or --verbose
      else if((arg[0]=='-' && arg[1]=='v') || string(arg)==string("--verbose")) {
         config.verbose = true;
      }
      // old versions of args -- deprecated
      else if(string(arg)==string("--directory")) { Args.push_back("--inputdir"); }
      else if(string(arg)==string("--EpochBeg")) { Args.push_back("--beginTime"); }
      else if(string(arg)==string("--EpochEnd")) { Args.push_back("--endTime"); }
      else if(string(arg)==string("--GPSBeg")) { Args.push_back("--beginTime"); }
      else if(string(arg)==string("--GPSEnd")) { Args.push_back("--endTime"); }
      else if(string(arg)==string("--CA")) { Args.push_back("--forceCA"); }
      else if(string(arg)==string("--useCA")) { Args.push_back("--forceCA"); }
      else if(string(arg)==string("--DT")) { Args.push_back("--dt"); }
      else if(string(arg)==string("--Gap")) { Args.push_back("--gap"); }
      else if(string(arg)==string("--Smooth")) { Args.push_back("--smooth"); }
      else if(string(arg)==string("--SmoothPR")) { Args.push_back("--smoothPR"); }
      else if(string(arg)==string("--SmoothPH")) { Args.push_back("--smoothPH"); }
      else if(string(arg)==string("--XPRN")) { Args.push_back("--exSat"); }
      else if(string(arg)==string("--SVonly")) { Args.push_back("--onlySat"); }
      else if(string(arg)==string("--Log")) { Args.push_back("--logOut"); }
      else if(string(arg)==string("--Out")) { Args.push_back("--cmdOut"); }
      // else its a regular command
      else Args.push_back(arg);
      //if(debug) cout << "arg " << string(arg) << endl;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(exception& e)
      { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
   catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
