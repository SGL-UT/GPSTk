// DiscFix.cpp Read a Rinex observation file containing dual frequency
//    pseudorange and phase, and find and estimate discontinuities in the phase.
//
//    DiscFix is part of the GPS Tool Kit (GPSTK) developed in the
//    Satellite Geophysics Group at Applied Research Laboratories,
//    The University of Texas at Austin (ARL:UT), and was written by Dr. Brian Tolman.
//------------------------------------------------------------------------------------
#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/cycleslips/DiscFix.cpp#5 $"

/**
 * @file DiscFix.cpp
 * Program to apply discontinuity correction to a Rinex observation file
 */

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
// TD
// Does it correctly delete and/or not output segments that are too small,
// esp when Smooth=T ?
//
//------------------------------------------------------------------------------------
#include "MathBase.hpp"
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "DayTime.hpp"
#include "CommandOptionParser.hpp"
#include "CommandOption.hpp"
#include "CommandOptionWithTimeArg.hpp"
#include "Stats.hpp"
#include "icd_200_constants.hpp"

#include "DiscCorr.hpp"

#include <time.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace gpstk;
using namespace std;

//------------------------------------------------------------------------------------
// prgm data
string PrgmName("DiscFix");
string PrgmVers("3.1 5/1/04");

typedef struct DiscFixConfiguration {
      // input
   string Directory;
   vector<string> InputObsName;
      // output
   string OutRinexObs;
   string HDPrgm;         // header of output Rinex file
   string HDRunby;
   string HDObs;
   string HDAgency;
   string HDMarker;
   string HDNumber;
   int NrecOut;
   DayTime FirstEpoch,LastEpoch;
   double DT;
   bool SmoothedRange,SmoothedPhase,Smooth;
   bool CAOut;
   bool DopOut;
   bool Verbose;
      // data flow
   double ith;
   DayTime Tbeg, Tend;
   double MaxGap;
   int MinPts;
      // output files
   string LogFile,ErrFile,OutFile;
   ofstream oflog,oferr,ofout;
      // processing
   double DataInt;
   bool UseCA;
   vector<RinexPrn> ExSV;
   RinexPrn SVonly;
      // estimate DT from data
   double estdt[9];
   int ndt[9];
} DFConfig;

//------------------------------------------------------------------------------------
// data input from command line
DFConfig DFC;
// DC configuration
GDCConfig GDC;
// data used in program
const double CMPS=C_GPS_M;
const double CFF=CMPS/RSVCLK;
const double F1=L1_MULT;
const double F2=L2_MULT;
const double wl1=CFF/F1;
const double wl2=CFF/F2;
const double alpha=((F1*F1)/(F2*F2) - 1.0);
clock_t totaltime;
string Title;
int reading;
RinexObsStream ifstr, ofstr;      // input and output Rinex files
RinexObsHeader rhead, rheadout;   
int inC1,inP1,inP2,inL1,inL2;      // indexes in rhead of C1, C1/P1, P2, L1 and L2
DayTime CurrEpoch(DayTime::BEGINNING_OF_TIME), PrgmEpoch;
// Data for an entire pass is stored in SVPass object
// this contains all the SVPass's defined so far
vector<SVPass> SVPList;
// this is a map relating a satellite to the index in SVPList of the current pass
map<RinexPrn,int> CurrentSVP;

//------------------------------------------------------------------------------------
// prototypes
int ReadFile(int nfile, int reading);
int ProcessOneEntireEpoch(RinexObsData& ro, int reading);
int ProcessOneSatOneEpoch(RinexObsData::RinexPrnMap::iterator& it, int reading);
int FindSatInCurrentSVPList(const RinexPrn& sat, const DayTime& ttag,
  const int reading);
int CreateNewSVPass(RinexPrn& sat, int in);

void ProcessEntireSVPass(int read, int index);
int AfterReadingFiles(int reading);
void PrintSVPList(ostream& os, int in, string msg, bool PrintAt=true,
   bool PrintRMP=false, double b1=0, double a1=0, double s1=0, double b2=0,
   double a2=0, double s2=0);

int GetCommandLine(int argc, char **argv);
void PreProcessArgs(const char *arg, vector<string>& Args, bool& Verbose);

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   totaltime = clock();
   int iret;

      // Title and description
   Title = PrgmName + ", part of the GPS ToolKit, Ver " + PrgmVers + ", Run ";
   time_t timer;
   struct tm *tblock;
   timer = time(NULL);
   tblock = localtime(&timer);
   PrgmEpoch.setYMDHMS(1900+tblock->tm_year,1+tblock->tm_mon,
               tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
   Title += PrgmEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S\n");
   cout << Title;

      // set fill char in RinexPrn
   DFC.SVonly.setfill('0');
   DFC.LastEpoch = DayTime::BEGINNING_OF_TIME;

      // get command line
   iret=GetCommandLine(argc, argv);
   if(iret) return iret;

      // read files 2 or 3 times
   reading = 0;
   while(1) {
      reading++;
      // set list of pointers to passes for all satellites to null

         // loop over input files
      for(int nfile=0; nfile<DFC.InputObsName.size(); nfile++) {
         iret = ReadFile(nfile,reading);
         if(iret < 0) break;
      }   // end loop over input files
      if(iret < 0) break;

      iret = AfterReadingFiles(reading);
      if(iret) break;
   };

      // add commands to set all LLI for L1 and L2 to zero
      // -SL commands generated by the DC will be applied by Editor after this cmd
   DFC.ofout << "# set all LLI to 0 (this command will not interfere "
      << "with other -SL cmds)" << endl;
   {
      ostringstream stst;
      if(GDC.OutputGPSTime) stst << DFC.FirstEpoch.printf("-SL+G-1,L1,%F,%g,0");
      else stst << DFC.FirstEpoch.printf("-SL+G-1,L1,%Y,%m,%d,%H,%M,%f,0");
      DFC.ofout << stst.str() << endl;
   }
   {
      ostringstream stst;
      if(GDC.OutputGPSTime) stst << DFC.FirstEpoch.printf("-SL+G-1,L2,%F,%g,0");
      else stst << DFC.FirstEpoch.printf("-SL+G-1,L2,%Y,%m,%d,%H,%M,%f,0");
      DFC.ofout << stst.str() << endl;
   }

      // clean up
   CurrentSVP.clear();
   SVPList.clear();

   totaltime = clock()-totaltime;
   //TEMP? if(GDC.Debug>1)
   DFC.oflog << "DiscFix timing: " << setprecision(3)
      << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.\n";

   DFC.oferr.close();
   DFC.oflog.close();
   DFC.ofout.close();

   return iret;
}
catch(gpstk::FFStreamError& e) {
   DFC.oferr << e;
   DFC.oferr << e;
}
catch(gpstk::Exception& e) {
   DFC.oferr << e;
   DFC.oferr << e;
}
catch (...) {
   DFC.oferr << "Unknown error.  Abort." << endl;
   DFC.oferr << "Unknown error.  Abort." << endl;
}
   DFC.oferr.close();

   return 1;
}   // end main()

//------------------------------------------------------------------------------------
// open the file, read header and check for data; then loop over the epochs
// Return 0 ok, <0 fatal error, >0 non-fatal error (ie skip this file)
// 0 ok, 1 couldn't open file, 2 file doesn't have required data
int ReadFile(int nfile, int reading)
{
try {
   string name;
      // open input file
   name = DFC.Directory + string("/") + DFC.InputObsName[nfile];
   ifstr.open(name.c_str(),ios::in);
   if(ifstr.fail()) {
      DFC.oflog << "Failed to open input file " << name << ". Abort.\n";
      return 1;
   }
   else if(GDC.Debug>1)
      DFC.oflog << "Opened input file " << name << " on reading number "
         << reading << endl;
   ifstr.exceptions(ios::failbit);

      // open output file (open on reading 1 in case it fails)
   if(reading == 1 && !DFC.OutRinexObs.empty()) {
      ofstr.open(DFC.OutRinexObs.c_str(), ios::out);
      if(ofstr.fail()) {
         DFC.oflog << "Failed to open output file " << DFC.OutRinexObs
            << ". Abort.\n";
         ifstr.close();
         return 1;
      }
      else if(GDC.Debug>1) DFC.oflog << "Opened output file " << DFC.OutRinexObs
         << " on reading number " << reading << endl;
      ofstr.exceptions(ios::failbit);
   }

      // read the header
   ifstr >> rhead;
   if(GDC.Debug > 2) {
      DFC.oflog << "Here is the input header for file " << name << ", reading "
         << reading << endl;
      rhead.dump(DFC.oflog);
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
   if((inP1==-1 && (!DFC.UseCA || inC1==-1)) || inP2==-1 || inL1==-1 || inL2==-1) {
      DFC.oflog << "Error: file " << name << " does not contain";
      if(inC1==-1) DFC.oflog << " C1";
      if(inL1==-1) DFC.oflog << " L1";
      if(inL2==-1) DFC.oflog << " L2";
      if(inP1==-1) DFC.oflog << " P1 (--CA was" << (DFC.UseCA?"":" not") << " found)";
      if(inP2==-1) DFC.oflog << " P2";
      DFC.oflog << endl;
      ifstr.clear();
      ifstr.close();
      return 2;
   }
   else if(inP1==-1 && DFC.UseCA) inP1=inC1;

      // loop over epochs in the file
   bool first=true;
   int iret;
   RinexObsData rodata;
   while(1) {
      ifstr >> rodata;
      if(ifstr.eof()) break;
      if(ifstr.bad()) {
         DFC.oflog << "input Rinex stream is bad" << endl;
         break;
      }
      iret = ProcessOneEntireEpoch(rodata,reading);
      if(iret < -1) break;
      if(iret == -1) { iret=0; break; }           // end of file
      //if(iret==1) ignore this epoch
      if(iret > 1) {                              // output : 2=comment block, 3=good
         if(first) {
            ofstr << rheadout;
            first=false;
         }
         ofstr << rodata;
      }
   }

   ifstr.clear();
   ifstr.close();
   if(reading==3) ofstr.close();

   return iret;
}
catch(gpstk::Exception& e) {
      DFC.oferr << "DiscFix:ReadFile caught an exception\n" << e << endl;
      GPSTK_RETHROW(e);
}
catch (...) {
      DFC.oferr << "DiscFix:ReadFile caught an unknown exception\n";
}
   return -1;
}

//------------------------------------------------------------------------------------
// Return 0 ok,
//       -1 end file,
//      <-1 fatal error,
//        1 skip this epoch,
//        2 comment block,
//        3 output Rinex
int ProcessOneEntireEpoch(RinexObsData& roe, int reading)
{
try {
   int i,k;
      // stay within time limits
   if(roe.time < DFC.Tbeg) return 1;
   if(roe.time > DFC.Tend) return -1;
      // ignore comment blocks ...
   if(roe.epochFlag != 0 && roe.epochFlag != 1) if(reading==3) return 2;
   else return 1;
      // decimate data
      // if Tbeg is still undefined, set it to begin of week
   if(DFC.ith > 0.0) {
      if(fabs(DFC.Tbeg-DayTime(DayTime::BEGINNING_OF_TIME)) < 1.e-8)
         DFC.Tbeg = DFC.Tbeg.setGPSfullweek(roe.time.GPSfullweek(),0.0);
      double dt=fabs(roe.time - DFC.Tbeg);
      dt -= DFC.ith*long(0.5+dt/DFC.ith);
      if(fabs(dt) > 0.25) return 1;            // TD set tolerance? clock bias?
   }
      // save current time
   CurrEpoch = roe.time;
   if(fabs(DFC.FirstEpoch-DayTime(DayTime::BEGINNING_OF_TIME)) < 1.e-8)
      DFC.FirstEpoch=CurrEpoch;

      // loop over satellites
   RinexPrn sat;
   //RinexObsData::RinexObsTypeMap otmap;
   RinexObsData::RinexPrnMap::iterator it;
      // loop over sat=it->first, ObsTypeMap=it->second
   for(it=roe.obs.begin(); it != roe.obs.end(); ++it) {
         // Is this satellite excluded ?
      sat = it->first;
      for(i=0,k=-1; i<DFC.ExSV.size(); i++) {
         if(DFC.ExSV[i] == sat) { k=i; break; }
            // whole system is excluded
         if(DFC.ExSV[i].prn==-1 && DFC.ExSV[i].system==sat.system) { k=0; break; }
      }
      if(k != -1) continue;
         // if only one satellite is included, skip all the rest
      if(DFC.SVonly.prn != -1 && !(sat == DFC.SVonly)) continue;

         // process this sat
      //otmap = it->second;
      int iret = ProcessOneSatOneEpoch(it,reading); // (always returns 0)

   }  // end loop over sats

      // After loop over satellites
   if(reading==1) {
         // update LastEpoch and estimate of DFC.DT
      if(DFC.LastEpoch > DayTime(DayTime::BEGINNING_OF_TIME)) {
         double dt = CurrEpoch-DFC.LastEpoch;
         for(int i=0; i<9; i++) {
            if(DFC.ndt[i]<=0) { DFC.estdt[i]=dt; DFC.ndt[i]=1; break; }
            if(fabs(dt-DFC.estdt[i]) < 0.0001) { DFC.ndt[i]++; break; }
            if(i == 8) {
               int k=0,nl=DFC.ndt[k];
               for(int j=1; j<9; j++) if(DFC.ndt[j] <= nl) { k=j; nl=DFC.ndt[j]; }
               DFC.ndt[k]=1; DFC.estdt[k]=dt;
            }
         }
      }
      DFC.LastEpoch = CurrEpoch;
   }
   else if(reading==3) return 3;         // output to Rinex file

   return 0;
}
catch(gpstk::Exception& e) {
   DFC.oferr << "DiscFix:ProcessOneEntireEpoch caught an exception " << e << endl;
   GPSTK_RETHROW(e);
}
catch (...) {
   DFC.oferr << "DiscFix:ProcessOneEntireEpoch caught an unknown exception\n";
}
   return -1;
}

//------------------------------------------------------------------------------------
int ProcessOneSatOneEpoch(RinexObsData::RinexPrnMap::iterator& it, int reading)
{
try {
   bool good;
   int in,n,k;
   double P1,P2,L1,L2,dt;
   RinexPrn sat=it->first;
   RinexObsData::RinexObsTypeMap otmap=it->second;

      // ignore non-GPS satellites
   if(sat.system != systemGPS) return 0;

      // pull out the data
   RinexObsData::RinexObsTypeMap::const_iterator jt;
   if( (jt=otmap.find(rhead.obsTypeList[inP1])) != otmap.end()) P1=jt->second.data;
   if( (jt=otmap.find(rhead.obsTypeList[inP2])) != otmap.end()) P2=jt->second.data;
   if( (jt=otmap.find(rhead.obsTypeList[inL1])) != otmap.end()) L1=jt->second.data;
   if( (jt=otmap.find(rhead.obsTypeList[inL2])) != otmap.end()) L2=jt->second.data;

      // find this sat in the map of sats,int
   in = FindSatInCurrentSVPList(sat,CurrEpoch,reading);

   if(reading==1) {         // update or create a new pass ---------------------------
      dt = 0.0;
      good = true;
      if(L1==0 || L2==0 || P1==0 || P2==0) good=false; //only create pass on good data
      if(in > -1) {
         // Npts here is current gap size
         if(good) {
            SVPList[in].Npts = 0;
            dt = CurrEpoch - SVPList[in].EndTime;      // gap since last good point
         }
         else SVPList[in].Npts++;
      }
         // should a new pass be created?
      if(good) {
         if(in==-1 || dt>DFC.MaxGap || SVPList[in].Npts*DFC.DT>DFC.MaxGap) {
            if(in > -1) CurrentSVP.erase(sat);
            in = CreateNewSVPass(sat, in);
         }
         SVPList[in].EndTime = CurrEpoch;
      }
      return 0;
   }   // end of reading==1

   // reading > 1 after this
 
   if(in==-1) {
         // search the list for the right SVPass
      for(int j=0; j<SVPList.size(); j++) {
         if(SVPList[j].SV==sat && fabs(CurrEpoch-SVPList[j].BegTime)<1.e-6) {
            in = j;
            CurrentSVP.insert(map<RinexPrn,int>::value_type(sat,in));
            break;
         }
      }

         // found a pass - and this is its first epoch
      if(in != -1) {
         if(SVPList[in].Npts <= 0) in=-1;
         else {
               // Resize the arrays in this pass
            if(reading==2) SVPList[in].Resize(SVPList[in].Npts);
               // print
            if(GDC.Debug > 2) PrintSVPList(DFC.oflog, in,
                  string(reading==2?"Fill":"Read"),true,false);
         }
      }
   }

      // still did not find pass - data did not get into an SVPass
      // - must be bad data inside a gap
      // mark it and quit
   if(in==-1) {
      if(reading==3) {      // mark the data bad for output
         it->second[rhead.obsTypeList[inP1]].data = 0;
         it->second[rhead.obsTypeList[inP2]].data = 0;
         it->second[rhead.obsTypeList[inL1]].data = 0;
         it->second[rhead.obsTypeList[inL2]].data = 0;
      }
      return 0;
   }

      // empty pass - ignore
   if(in>-1 && SVPList[in].Length <= 0) {
      if(CurrEpoch-SVPList[in].EndTime > -1.0e-8) CurrentSVP.erase(sat);
      return 0;
   }

      // At this point we have a good pass with data and reading > 1 --------------

      // find the index for this epoch's data
   dt = CurrEpoch - SVPList[in].BegTime;
   n = int(0.5+dt/DFC.DT);

      // add this data to the SVPass (editing could be done here)
   if(reading==2) {
      SVPList[in].L1[n] = L1;
      SVPList[in].L2[n] = L2;
      SVPList[in].P1[n] = P1;
      SVPList[in].P2[n] = P2;
      if(DFC.Smooth) {         // save for later
         SVPList[in].E1[n] = P1;
         SVPList[in].E2[n] = P2;
      }
         // Npts now means number of *good* points; Length holds array size
      if(L1==0 || L2==0 || P1==0 || P2==0) SVPList[in].Flag[n]=SVPass::BAD;
      else {
         SVPList[in].Flag[n] = SVPass::OK;
         SVPList[in].Npts++;
      }
   }  // end if reading==2

      // if there is data, store it in RinexObsData for output to Rinex
   if(reading==3) {
      if(SVPList[in].Flag[n] >= SVPass::OK) {
            // discontinuity-corrected phases
         it->second[rhead.obsTypeList[inL1]].data = SVPList[in].L1[n];
         it->second[rhead.obsTypeList[inL2]].data = SVPList[in].L2[n];
            // replace range with phase smoothed
         if(DFC.Smooth) {
            if(SVPList[in].P1[n] != -1 && SVPList[in].P2[n] != -1) {
               double b1,b2,d1,d2;
                  // phase biases
               d1 = SVPList[in].P1[n];
               d2 = SVPList[in].P2[n];
                  // debiased phases
               b1 = wl1*SVPList[in].L1[n] - d1;
               b2 = wl2*SVPList[in].L2[n] - d2;
                  // smoothed ranges
               d1 = ((alpha+2)*b1-2*b2)/alpha;
               d2 = (2*(alpha+1)*b1-(alpha+2)*b2)/alpha;

               if(DFC.SmoothedRange) {
                  it->second[rhead.obsTypeList[inP1]].data = d1;
                  it->second[rhead.obsTypeList[inP2]].data = d2;
               }
               if(DFC.SmoothedPhase) {
                  it->second[rhead.obsTypeList[inL1]].data = b1/wl1;
                  it->second[rhead.obsTypeList[inL2]].data = b2/wl2;
               }
            }
         }
      }

            // now set flags
      unsigned int onec=1;
      if((SVPList[in].Flag[n] & SVPass::SLIP)
         && SVPList[in].Flag[n] >= SVPass::OK      // these are redundant ...
         && (!(SVPList[in].Flag[n] & SVPass::FIXWL) || !(SVPList[in].Flag[n]
               & SVPass::FIXGF))) {
            // set the LLI flag
         it->second[rhead.obsTypeList[inL1]].lli |= onec;
         it->second[rhead.obsTypeList[inL2]].lli |= onec;
      }
      else {
            // unset the flag, but only if already set
         if(it->second[rhead.obsTypeList[inL1]].lli & onec)
            it->second[rhead.obsTypeList[inL1]].lli ^= onec;
         if(it->second[rhead.obsTypeList[inL2]].lli & onec)
            it->second[rhead.obsTypeList[inL2]].lli ^= onec;
      }
   }      // end if reading==3

      // have reached end of pass - process it and then (perhaps) delete contents
   if(CurrEpoch-SVPList[in].EndTime > -1.0e-8) {
         // process the entire pass - this is where GDC and phase smoothing are ...
      if(reading==2) ProcessEntireSVPass(reading,in);

         // remove from the CurrentSVP map
      //PrintSVPList(DFC.oflog, in, string("Remove this pass"));
      CurrentSVP.erase(SVPList[in].SV);

         // gut the structure if: 1) no output Rinex, hence no third reading,
         // 2) this is third reading
      if(reading==3 || DFC.OutRinexObs.empty()) SVPList[in].Resize(0);
   }

   return 0;
}
catch(gpstk::Exception& e) {
   DFC.oferr << "DiscFix:ProcessOneSatOneEpoch caught an exception " << e << endl;
   GPSTK_RETHROW(e);
}
catch (...) {
   DFC.oferr << "DiscFix:ProcessOneSatOneEpoch caught an unknown exception\n";
}
   return -1;
}

//------------------------------------------------------------------------------------
int FindSatInCurrentSVPList(const RinexPrn& sat, const DayTime& ttag,
      const int reading)
{
try {
   map<RinexPrn,int>::const_iterator kt=CurrentSVP.find(sat);
   if(kt != CurrentSVP.end())
      return kt->second;
   else
      return -1;
}
catch(gpstk::Exception& e) {
   DFC.oferr << "DiscFix:FindSatInCurrentSVPList caught an exception " << e << endl;
   GPSTK_RETHROW(e);
}
catch (...) {
   DFC.oferr << "DiscFix:FindSatInCurrentSVPList caught an unknown exception\n";
}
   return -1;
}

//------------------------------------------------------------------------------------
int CreateNewSVPass(RinexPrn& sat, int in)
{
try {
   int inew;
   //DFC.oflog << "Create a new pass for " << sat << " at " << CurrEpoch << endl;
   SVPass SVP;
   SVP.SV = sat;
   SVP.BegTime = CurrEpoch;
   if(DFC.Smooth) SVP.Extra=true;
   SVPList.push_back(SVP);
   inew = SVPList.size()-1;
   CurrentSVP.insert(map<RinexPrn, int>::value_type(sat,inew));
   map<RinexPrn,int>::iterator kt= CurrentSVP.find(sat);
   if(kt == CurrentSVP.end()) {
      gpstk::Exception e("Failed to create new pass");
      GPSTK_THROW(e);
   }
   return inew;
}
catch(gpstk::Exception& e) {
   DFC.oferr << "DiscFix:CreateNewSVPass caught an exception " << e << endl;
   GPSTK_RETHROW(e);
}
catch (...) {
   DFC.oferr << "DiscFix:CreateNewSVPass caught an unknown exception\n";
}
   return -1;
}

//------------------------------------------------------------------------------------
void ProcessEntireSVPass(int reading, int in)
{
try {
   int i,j,k,n;
      // print
   if(GDC.Debug > 1) PrintSVPList(DFC.oflog, in, string("Proc"));

      // not enough data
   if(SVPList[in].Npts < DFC.MinPts) {
      SVPList[in].P1[2] = SVPList[in].P2[2] = -1.0;
      return;
   }

      // call the GDC
   vector<string> EditCmds;         // GDC will clear this
   k = GPSTKDiscontinuityCorrector(SVPList[in], GDC, EditCmds);
   if(k) return;

      // output the results
   for(i=0; i<EditCmds.size(); i++) DFC.ofout << EditCmds[i] << endl;
   EditCmds.clear();

      // if smoothing, compute stats on range - phase
      // compute initial bias and average on these 2 quantities:
   if(DFC.Smooth) {
      n = SVPList[in].Npts;
      j = 0;
      do {
         if(n < 3) break;
         bool first=true;
         int k1,k2;
         double d1,d2,b1,b2,t1,t2,gfp;
         Stats<double> S1,S2;
         DayTime bt(SVPList[in].BegTime),et(SVPList[in].EndTime),holdb,holde;
         for(k=j; k<SVPList[in].Length; k++) {
            SVPList[in].P1[k] = SVPList[in].P2[k] = -1;
            if(SVPList[in].Flag[k] >= SVPass::OK) {
               if(!first && (SVPList[in].Flag[k] & SVPass::SLIP)) break;
               d1 = SVPList[in].E1[k] - wl1*SVPList[in].L1[k];
               d2 = SVPList[in].E2[k] - wl2*SVPList[in].L2[k];
               gfp = wl1*SVPList[in].L1[k] - wl2*SVPList[in].L2[k];
               d1 -= 2*gfp/alpha;
               d2 -= 2*(alpha+1)*gfp/alpha;
               if(first) {
                  k1 = k;
                  b1 = d1;
                  b2 = d2;
                  first = false;
               }
               k2 = k;
               S1.Add(d1-b1);
               S2.Add(d2-b2);
            }
         }
         et += double(k-1)*DFC.DT;
            // prepare for next continuous segment
         j = k;
            // compute phase biases
         t1 = b1 + S1.Average();
         t2 = b2 + S2.Average();
         d1 = (-(alpha+2)*t1+2*t2)/alpha;
         d2 = (-2*(alpha+1)*t1+(alpha+2)*t2)/alpha;
         if(S1.N() < 2 || S2.N() < 2) d1=d2=-1;
            // store results in P1 and P2 arrays
         for(k=k1; k<=k2; k++) {
            SVPList[in].P1[k] = d1;
            SVPList[in].P2[k] = d2;
         }
            // print RMP stats
         holdb = SVPList[in].BegTime;
         holde = SVPList[in].EndTime;
         SVPList[in].BegTime = bt;
         SVPList[in].EndTime = et;
         if(GDC.Debug>2) PrintSVPList(DFC.oflog, in, string("Stat"), true, true,
               b1,S1.Average(),S1.StdDev(),b2,S2.Average(),S2.StdDev());
         SVPList[in].BegTime = holdb;
         SVPList[in].EndTime = holde;
            // go on to next continuous segment
         n = SVPList[in].Length - j;
      } while(1);

   }  // end if Smooth
}
catch(gpstk::Exception& e) {
   DFC.oferr << "DiscFix:ProcessEntireSVPass caught an exception " << e << endl;
   GPSTK_RETHROW(e);
}
catch (...) {
   DFC.oferr << "DiscFix:ProcessEntireSVPass caught an unknown exception\n";
}
}

//------------------------------------------------------------------------------------
int AfterReadingFiles(int reading)
{
try {
   int i,j,iret=0;
   double dt;

   CurrentSVP.clear();

   if(reading==1) {
         // if no data read ... fail
      if(SVPList.size() == 0) {
         DFC.oflog << "DiscFix Abort : no SV passes defined after first reading\n";
         return -2;
      }

         // compute data interval for this file
      for(j=0,i=1; i<9; i++) { if(DFC.ndt[i]>DFC.ndt[j]) j=i; }
      DFC.DT = DFC.estdt[j];
      GDC.DT = DFC.DT;
      if(GDC.Debug > 2) DFC.oflog << "Estimated data interval to be "
         << DFC.DT << " seconds.\n";
         // compute the number of points in each pass, incl gaps
      for(i=0; i<SVPList.size(); i++) {
         dt = SVPList[i].EndTime-SVPList[i].BegTime;
         SVPList[i].Npts = 1+int(0.5+dt/DFC.DT);
         if(SVPList[i].Npts < DFC.MinPts) SVPList[i].Npts=0;
      }
         // sort the list of sv passes
      sort(SVPList.begin(),SVPList.end(),SVPLessThan());
   }
   else if(reading==2 && !DFC.OutRinexObs.empty()) {   // edit the output Rinex header
      rheadout = rhead;
         // fill records in output header
      rheadout.date = PrgmEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S");
      rheadout.fileProgram = PrgmName;
      if(!DFC.HDRunby.empty()) rheadout.fileAgency = DFC.HDRunby;
      if(!DFC.HDObs.empty()) rheadout.observer = DFC.HDObs;
      if(!DFC.HDAgency.empty()) rheadout.agency = DFC.HDAgency;
      if(!DFC.HDMarker.empty()) rheadout.markerName = DFC.HDMarker;
      if(!DFC.HDNumber.empty()) rheadout.markerNumber = DFC.HDNumber;
      rheadout.version = 2.1; rheadout.valid |= RinexObsHeader::versionValid;
      rheadout.firstObs = DFC.FirstEpoch; rheadout.valid
         |= RinexObsHeader::firstTimeValid;
      rheadout.interval = DFC.DT; rheadout.valid |= RinexObsHeader::intervalValid;
      rheadout.lastObs = DFC.LastEpoch;
      rheadout.valid |= RinexObsHeader::lastTimeValid;
      if(DFC.SmoothedRange)
         rheadout.commentList.push_back(string("Ranges smoothed by ") +
         PrgmName + string(" v.") + PrgmVers.substr(0,4) + string(" ") +
         rheadout.date);
      if(DFC.SmoothedPhase)
         rheadout.commentList.push_back(string("Phases debiased by ") +
         PrgmName + string(" v.") + PrgmVers.substr(0,4) + string(" ") +
         rheadout.date);
         // invalidate the table
      if(rheadout.valid & RinexObsHeader::numSatsValid)
         rheadout.valid ^= RinexObsHeader::numSatsValid;
      if(rheadout.valid & RinexObsHeader::prnObsValid)
         rheadout.valid ^= RinexObsHeader::prnObsValid;
   }
   else iret=-1;

   // print a summary of the sv passes
   if(GDC.Debug > 2) {
      DFC.oflog << "\nSatellite passes " << (reading==1?"defined":"processed")
         << " (" << SVPList.size() << ")\n";
      for(i=0; i<SVPList.size(); i++)
         PrintSVPList(DFC.oflog, i,
               string(reading==1?"Defd":(reading==2?"Done":"Fine")),false);
      DFC.oflog << "---------------- End of "
         << (reading==1?"first":(reading==2?"second":"third"))
         << " read through -------------------\n";
   }

   return iret;
}
catch(gpstk::Exception& e) {
      DFC.oferr << "DiscFix:AfterReadingFiles caught an exception " << e << endl;
      GPSTK_RETHROW(e);
}
catch (...) {
      DFC.oferr << "DiscFix:AfterReadingFiles caught an unknown exception\n";
}
   return -1;
}

//------------------------------------------------------------------------------------
void PrintSVPList(ostream& os, int in, string msg, bool PrintAt,
      bool PrintRMP, double b1, double a1, double s1, double b2, double a2, double s2)
{
   os << msg << " " << SVPList[in].SV
      << " " << setw(4) << SVPList[in].Length << "(" << setw(4) << SVPList[in].Npts
      << ") (" << SVPList[in].BegTime.printf("%04Y/%02m/%02d %02H:%02M:%6.3f")
      << ")-(" << SVPList[in].EndTime.printf("%04Y/%02m/%02d %02H:%02M:%6.3f") << ")";
   if(PrintAt) os << " at " << CurrEpoch.printf("%04Y/%02m/%02d %02H:%02M:%6.3f");
   if(PrintRMP) os << " RMP1 " << fixed << setw(13) << setprecision(3) << b1
      << fixed << setw(8) << setprecision(3) << a1
      << fixed << setw(6) << setprecision(3) << s1
      << " RMP2 " << fixed << setw(13) << setprecision(3) << b2
      << fixed << setw(8) << setprecision(3) << a2
      << fixed << setw(6) << setprecision(3) << s2;
   os << endl;
}

//------------------------------------------------------------------------------------
int GetCommandLine(int argc, char **argv)
{
   bool help=false;
   int i,j;
try {
      // defaults
   DFC.Verbose = false;
   DFC.ith = 0.0;
   DFC.Tbeg = DFC.FirstEpoch = DayTime(DayTime::BEGINNING_OF_TIME);
   DFC.Tend = DayTime(DayTime::END_OF_TIME);
   DFC.MaxGap = 600.0;
   DFC.MinPts = 10;
   DFC.DT = 0;

   DFC.ErrFile = string("df.err");
   DFC.LogFile = string("df.log");
   DFC.OutFile = string("df.out");

   DFC.UseCA = false;
   DFC.DataInt = -1.0;
   
   DFC.HDPrgm = PrgmName + string(" v.") + PrgmVers.substr(0,4);
   DFC.HDRunby = string("ARL:UT/SGL/GPSTK");

   DFC.SmoothedRange = false;
   DFC.SmoothedPhase = false;
   DFC.Smooth = false;
   DFC.CAOut = false;
   DFC.DopOut = false;

   for(i=0; i<9; i++) DFC.ndt[i]=-1;

   //DFC.FileType = Rinex;
   DFC.Directory = string("");

      // -------------------------------------------------
      // required options
   RequiredOption dashi(CommandOption::hasArgument, CommandOption::stdType,
      'i',"inputfile"," [-i|--inputfile]<file> Input (Rinex obs) file(s)");

      // optional options
      // this only so it will show up in help page...
   CommandOption dashf(CommandOption::hasArgument, CommandOption::stdType,
      'f',""," -f<file>               file containing more options");

   CommandOption dashd(CommandOption::hasArgument, CommandOption::stdType,
      'd',"directory"," [-d|--directory] <dir> Directory of input file(s)");
   dashd.setMaxCount(1);

   CommandOption dashith(CommandOption::hasArgument, CommandOption::stdType,
        0,"decimate"," --decimate <dt>        Decimate data to time interval dt");
   dashith.setMaxCount(1);

   // time
   CommandOptionWithTimeArg dasheb(0,"EpochBeg","%Y,%m,%d,%H,%M,%f",
      " --EpochBeg <arg>       Start time, arg is of the form YYYY,MM,DD,HH,Min,Sec");
   CommandOptionWithTimeArg dashgb(0,"GPSBeg","%F,%g",
      " --GPSBeg <arg>         Start time, arg is of the form GPSweek,GPSsow");

   CommandOptionWithTimeArg dashee(0,"EpochEnd","%Y,%m,%d,%H,%M,%f",
      " --EpochEnd <arg>       End time, arg is of the form YYYY,MM,DD,HH,Min,Sec");
   CommandOptionWithTimeArg dashge(0,"GPSEnd","%F,%g",
      " --GPSEnd <arg>         End time, arg is of the form GPSweek,GPSsow");

   // allow ONLY one start time (use startmutex(true) if one is required)
   CommandOptionMutex startmutex(false);
   startmutex.addOption(&dasheb);
   startmutex.addOption(&dashgb);
   CommandOptionMutex stopmutex(false);
   stopmutex.addOption(&dashee);
   stopmutex.addOption(&dashge);

   CommandOptionNoArg dashCA(0,"CA",
      " --CA                   Use C/A code pseudorange if P1 is not available");
   dashCA.setMaxCount(1);
   
   CommandOption dashDT(CommandOption::hasArgument, CommandOption::stdType,
      0,"DT"," --DT <dt>              Time interval (s) of data points "
      "(needed for -Ps only)");
   dashDT.setMaxCount(1);
   
   CommandOption dashGap(CommandOption::hasArgument, CommandOption::stdType,
      0,"Gap"," --Gap <t>              Time (s) of largest allowed gap within pass");
   dashGap.setMaxCount(1);
   
   CommandOption dashPts(CommandOption::hasArgument, CommandOption::stdType,
      0,"Points"," --Points <n>           Minimum number of points needed to "
      "process a pass");
   dashPts.setMaxCount(1);
   
   CommandOption dashXprn(CommandOption::hasArgument, CommandOption::stdType,
      0,"XPRN"," --XPRN <prn>           Exclude this satellite "
      "(prn may be only <system>)");
   
   CommandOption dashSV(CommandOption::hasArgument, CommandOption::stdType,
      0,"SVonly"," --SVonly <prn>         Process this satellite ONLY");
   dashSV.setMaxCount(1);
   
   CommandOption dashLog(CommandOption::hasArgument, CommandOption::stdType,
      0,"Log"," --Log <file>           Output log file name (df.log)");
   dashLog.setMaxCount(1);
   
   CommandOption dashErr(CommandOption::hasArgument, CommandOption::stdType,
      0,"Err"," --Err <file>           Output error file name (df.err)");
   dashErr.setMaxCount(1);
   
   CommandOption dashOut(CommandOption::hasArgument, CommandOption::stdType,
      0,"Out"," --Out <file>           Output (editing commands) file name (df.out)");
   dashOut.setMaxCount(1);
   
   CommandOption dashRfile(CommandOption::hasArgument, CommandOption::stdType,
      0,"RinexFile"," --RinexFile <file>     Output Rinex obs file name");
   dashRfile.setMaxCount(1);
   
   CommandOption dashRrun(CommandOption::hasArgument, CommandOption::stdType,
      0,"RunBy"," --RunBy <string>       Output Rinex header 'RUN BY' string");
   dashRrun.setMaxCount(1);
   
   CommandOption dashRobs(CommandOption::hasArgument, CommandOption::stdType,
      0,"Observer"," --Observer <string>    Output Rinex header 'OBSERVER' string");
   dashRobs.setMaxCount(1);
   
   CommandOption dashRag(CommandOption::hasArgument, CommandOption::stdType,
      0,"Agency"," --Agency <string>      Output Rinex header 'AGENCY' string");
   dashRag.setMaxCount(1);
   
   CommandOption dashRmark(CommandOption::hasArgument, CommandOption::stdType,
      0,"Marker"," --Marker <string>      Output Rinex header 'MARKER' string");
   dashRmark.setMaxCount(1);
   
   CommandOption dashRnumb(CommandOption::hasArgument, CommandOption::stdType,
      0,"Number"," --Number <string>      Output Rinex header 'NUMBER' string");
   dashRnumb.setMaxCount(1);
   
   CommandOptionNoArg dashSmooth(0,"Smooth"," --Smooth               Smooth "
      "pseudorange and debias phase and output both in place of raw");
   dashSmooth.setMaxCount(1);
   
   CommandOptionNoArg dashSmoothPR(0,"SmoothPR"," --SmoothPR             Smooth "
      "pseudorange and debias phase but replace only raw pseudorange");
   dashSmoothPR.setMaxCount(1);
   
   CommandOptionNoArg dashSmoothPH(0,"SmoothPH"," --SmoothPH             Smooth "
      "pseudorange and debias phase but replace only raw phase");
   dashSmoothPH.setMaxCount(1);
   
   CommandOptionNoArg dashCAOut(0,"CAOut",
      " --CAOut                Output C/A code in Rinex");
   dashCAOut.setMaxCount(1);
   
   CommandOptionNoArg dashDOut(0,"DOut",
      " --DOut                 Output Doppler in Rinex");
   dashDOut.setMaxCount(1);
   
   CommandOptionNoArg dashVerb(0,"verbose",
      " --verbose              print extended output "
      "(NB --DCDebug,7 => all debugging output)");
   dashVerb.setMaxCount(1);

   CommandOptionNoArg dashh('h', "help",
      " [-h|--help]            print syntax and quit."
      "\n --DC...                Set parameter in Discontinuity Corrector : "
      "(use --DChelp -h for a listing) ");

   // ... other options
   CommandOptionRest Rest("");

   CommandOptionParser Par(
   "Prgm DiscFix reads a data file containing dual-frequency pseudorange and phase\n"
   "   measurements and finds and fixes discontinuities in the phase; output is\n"
   "   a list of editing commands (for use with PRGM RinexEdit); the program will\n"
   "   also (optional) write out the raw (uncorrected) data to a RINEX file.\n");

      // -------------------------------------------------
      // allow user to put all options in a file
      // could also scan for debug here
   vector<string> Args;
   for(j=1; j<argc; j++) PreProcessArgs(argv[j],Args,DFC.Verbose);
   argc = Args.size();
   if(argc==0) Args.push_back(string("-h"));

      // strip out the DCcmds
   vector<string> DCcmds;
   vector<string>::iterator it=Args.begin();
   while(it != Args.end()) {
      if(it->substr(0,4) == string("--DC")) {
         DCcmds.push_back(*it);
         it = Args.erase(it);
      }
      else it++;
   }
   for(i=0; i<DCcmds.size(); i++) {
      GDC.SetParameter(DCcmds[i]);
   }

      // pass the rest
   argc = Args.size()+1;
   char **CArgs=new char*[argc];
   if(!CArgs) { cout << "Failed to allocate CArgs\n"; return -1; }
   CArgs[0] = argv[0];
   for(j=1; j<argc; j++) {
      CArgs[j] = new char[Args[j-1].size()+1];
      if(!CArgs[j]) { cout << "Failed to allocate CArgs[j]\n"; return -1; }
      strcpy(CArgs[j],Args[j-1].c_str());
   }
   Par.parseOptions(argc, CArgs);

      // -------------------------------------------------
   if(dashh.getCount() > 0) {
      Par.displayUsage(cout,false);
      if(GDC.help) GDC.DisplayParameterUsage(cout);
      help = true;
   }

   if (Par.hasErrors())
   {
      cout << "\nErrors found in command line input:\n";
      Par.dumpErrors(cout);
      cout << "...end of Errors\n\n";
      help = true;
   }
   
      // -------------------------------------------------
      // get values found on command line
   vector<string> values;
      // f never appears because we intercept it above
   //if(dashf.getCount()) { cout << "Option f "; dashf.dumpValue(cout); }
      // do help first
   if(dashh.getCount()) help=true;
   if(dashLog.getCount()) {
      values = dashLog.getValue();
      DFC.LogFile = values[0];
      if(help) cout << "Log file is " << DFC.LogFile << endl;
   }
      // open the log file
   DFC.oflog.open(DFC.LogFile.c_str(),ios::out);
   if(DFC.oflog.fail()) {
      cout << "DiscFix failed to open log file " << DFC.LogFile << "! Abort.\n";
      return -1;
   }
   DFC.oflog << Title;
   GDC.oflog = &DFC.oflog;
   if(GDC.Debug > 0) cout << "DiscFix opened log file " << DFC.LogFile << endl;

      // now get the rest of the options
   if(dashVerb.getCount()) DFC.Verbose=true;
   if(dashi.getCount()) {
      values = dashi.getValue();
      if(help) DFC.oflog << "Input Rinex obs files are:\n";
      for(i=0; i<values.size(); i++) {
         DFC.InputObsName.push_back(values[i]);
         if(help) DFC.oflog << "   " << values[i] << endl;
      }
   }
   if(dashd.getCount()) {
      values = dashd.getValue();
      DFC.Directory = values[0];
      if(help) DFC.oflog << "Input Directory is " << DFC.Directory << endl;
   }
   if(dashith.getCount()) {
      values = dashith.getValue();
      DFC.ith = StringUtils::asDouble(values[0]);
      if(help) DFC.oflog << "Ithing values is " << DFC.ith << endl;
   }
   if(dasheb.getCount()) {
      values = dasheb.getValue();
      DFC.Tbeg.setToString(values[0], "%Y,%m,%d,%H,%M,%S");
      if(help) DFC.oflog << "Begin time is "
         << DFC.Tbeg.printf("%04Y/%02m/%02d %02H:%02M:%.3f") << endl;
   }
   if(dashgb.getCount()) {
      values = dashgb.getValue();
      DFC.Tbeg.setToString(values[0], "%F,%g");
      if(help) DFC.oflog << "Begin time is "
         << DFC.Tbeg.printf("%04F/%10.3g") << endl;
   }
   if(dashee.getCount()) {
      values = dashee.getValue();
      DFC.Tend.setToString(values[0], "%Y,%m,%d,%H,%M,%S");
      if(help) DFC.oflog << "End time is "
         << DFC.Tend.printf("%04Y/%02m/%02d %02H:%02M:%.3f") << endl;
   }
   if(dashge.getCount()) {
      values = dashge.getValue();
      DFC.Tend.setToString(values[0], "%F,%g");
      if(help) DFC.oflog << "End time is "
         << DFC.Tend.printf("%04F/%10.3g") << endl;
   }
   if(dashCA.getCount()) {
      DFC.UseCA = true;
      if(help) DFC.oflog << "'Use C/A' flag is set\n";
   }
   if(dashDT.getCount()) {
      values = dashDT.getValue();
      DFC.DT = StringUtils::asDouble(values[0]);
      if(help) DFC.oflog << "DT is set to " << DFC.DT << endl;
   }
   if(dashGap.getCount()) {
      values = dashGap.getValue();
      DFC.MaxGap = StringUtils::asDouble(values[0]);
      if(help) DFC.oflog << "Max gap (sec) is " << DFC.MaxGap << endl;
   }
   if(dashPts.getCount()) {
      values = dashPts.getValue();
      DFC.MinPts = StringUtils::asInt(values[0]);
      if(help) DFC.oflog << "Minimum points is " << DFC.MinPts << endl;
   }
   if(dashXprn.getCount()) {
      values = dashXprn.getValue();
      for(i=0; i<values.size(); i++) {
         RinexPrn p=StringUtils::asData<RinexPrn>(values[i]);
         if(help) DFC.oflog << "Exclude satellite " << p << endl;
         DFC.ExSV.push_back(p);
      }
   }
   if(dashSV.getCount()) {
      values = dashSV.getValue();
      RinexPrn p=StringUtils::asData<RinexPrn>(values[0]);
      DFC.SVonly = p;
      if(help) DFC.oflog << "Process only satellite : " << p << endl;
   }
   if(dashErr.getCount()) {
      values = dashErr.getValue();
      DFC.ErrFile = values[0];
      if(help) DFC.oflog << "Err file is " << DFC.ErrFile << endl;
   }
   if(dashOut.getCount()) {
      values = dashOut.getValue();
      DFC.OutFile = values[0];
      if(help) DFC.oflog << "Out file is " << DFC.OutFile << endl;
   }
   if(dashRfile.getCount()) {
      values = dashRfile.getValue();
      DFC.OutRinexObs = values[0];
      if(help) DFC.oflog << "Output Rinex file name is " << DFC.OutRinexObs << endl;
   }
   if(dashRrun.getCount()) {
      values = dashRrun.getValue();
      DFC.HDRunby = values[0];
      if(help) DFC.oflog << "Output Rinex 'RUN BY' is " << DFC.HDRunby << endl;
   }
   if(dashRobs.getCount()) {
      values = dashRobs.getValue();
      DFC.HDObs = values[0];
      if(help) DFC.oflog << "Output Rinex 'OBSERVER' is " << DFC.HDObs << endl;
   }
   if(dashRag.getCount()) {
      values = dashRag.getValue();
      DFC.HDAgency = values[0];
      if(help) DFC.oflog << "Output Rinex 'AGENCY' is " << DFC.HDAgency << endl;
   }
   if(dashRmark.getCount()) {
      values = dashRmark.getValue();
      DFC.HDMarker = values[0];
      if(help) DFC.oflog << "Output Rinex 'MARKER' is " << DFC.HDMarker << endl;
   }
   if(dashRnumb.getCount()) {
      values = dashRnumb.getValue();
      DFC.HDNumber = values[0];
      if(help) DFC.oflog << "Output Rinex 'NUMBER' is " << DFC.HDNumber << endl;
   }
   if(dashSmooth.getCount()) {
      DFC.SmoothedPhase = DFC.SmoothedRange = true;
      if(help) DFC.oflog << "'Smooth both' option is on\n";
   }
   if(dashSmoothPR.getCount()) {
      DFC.SmoothedRange = true;
      if(help) DFC.oflog << "'Smooth range' option is on\n";
   }
   if(dashSmoothPH.getCount()) {
      DFC.SmoothedPhase = true;
      if(help) DFC.oflog << "'Smooth phase' option is on\n";
   }
   if(dashCAOut.getCount()) {
      DFC.CAOut = true;
      if(help) DFC.oflog << "'C/A output' option is on\n";
   }
   if(dashDOut.getCount()) {
      DFC.DopOut = true;
      if(help) DFC.oflog << "'Doppler output' option is on\n";
   }

   if(Rest.getCount() && help) {
      DFC.oflog << "Remaining options:" << endl;
      values = Rest.getValue();
      for (i=0; i<values.size(); i++) DFC.oflog << values[i] << endl;
   }
   //if(DFC.Verbose && help) {
   //   DFC.oflog << "\nTokens on command line (" << Args.size() << ") are:" << endl;
   //   for(j=0; j<Args.size(); j++) DFC.oflog << Args[j] << endl;
   //}

   if(DFC.Verbose && GDC.Debug<2) { // if GDC::Debug is not set higher, set to 2
      GDC.SetParameter(string("Debug=2"));
   }

   if(help) return 1;

   DFC.oferr.open(DFC.ErrFile.c_str());
   if(DFC.oferr.good()) {
      if(GDC.Debug>1)DFC.oflog << "DiscFix opened error file " << DFC.ErrFile << endl;
      if(GDC.Debug>1) cout << "DiscFix opened error file " << DFC.ErrFile << endl;
      DFC.oferr << Title;
   }
   DFC.ofout.open(DFC.OutFile.c_str());
   if(DFC.ofout.good()) {
      if(GDC.Debug>1) DFC.oflog << "DiscFix opened output file "
         << DFC.OutFile << endl;
   }
   else {
      DFC.oferr << "Error: DiscFix failed to open output file "
         << DFC.OutFile << endl;
      DFC.oflog << "Error: DiscFix failed to open output file "
         << DFC.OutFile << endl;
   }

   if(GDC.DT <= 0.0) {
      DFC.oferr << "DiscFix:DT in the GDC must be positive - use --DCDT" << endl;
      DFC.oflog << "DiscFix:DT in the GDC must be positive - use --DCDT" << endl;
      return -1;
   }

   DFC.Smooth = (DFC.SmoothedRange || DFC.SmoothedPhase);

      // print config to log
   if(GDC.Debug > 0) {
      DFC.oflog << "Here is the DF configuration:\n";
      DFC.oflog << "Input Rinex obs files are:\n";
      for(i=0; i<DFC.InputObsName.size(); i++) {
         DFC.oflog << "  " << DFC.InputObsName[i] << endl;
      }
      DFC.oflog << "Input Directory is " << DFC.Directory << endl;
      DFC.oflog << "Ithing time interval is " << DFC.ith << endl;
      if(DFC.Tbeg > DayTime(DayTime::BEGINNING_OF_TIME))
      DFC.oflog << "Begin time is "
         << DFC.Tbeg.printf("%04Y/%02m/%02d %02H:%02M:%.3f")
         << " = " << DFC.Tbeg.printf("%04F/%10.3g") << endl;
      if(DFC.Tend < DayTime(DayTime::END_OF_TIME))
         DFC.oflog << "End time is "
            << DFC.Tend.printf("%04Y/%02m/%02d %02H:%02M:%.3f")
            << " = " << DFC.Tend.printf("%04F/%10.3g") << endl;
      if(DFC.UseCA) DFC.oflog << "'Use C/A' flag is set\n";
      DFC.oflog << "DT is set to " << DFC.DT << endl;
      DFC.oflog << "Max gap (sec) is " << DFC.MaxGap << endl;
      DFC.oflog << "Minimum points is " << DFC.MinPts << endl;
      if(DFC.ExSV.size()) {
         DFC.oflog << "Exclude satellites";
         for(i=0; i<DFC.ExSV.size(); i++) {
            if(DFC.ExSV[i].prn == -1) DFC.oflog << " (all "
               << (DFC.ExSV[i].system == systemGPS ? "GPS" :
                  (DFC.ExSV[i].system == systemGlonass ? "Glonass" :
                  (DFC.ExSV[i].system == systemTransit ? "Transit" :
                  (DFC.ExSV[i].system == systemGeosync ? "Geosync" : "Mixed"))))
               << ")";
            else DFC.oflog << " " << DFC.ExSV[i];
         }
         DFC.oflog << endl;
      }
      if(DFC.SVonly.prn > 0)
         DFC.oflog << "Process only satellite : " << DFC.SVonly << endl;
      DFC.oflog << "Log file is " << DFC.LogFile << endl;
      DFC.oflog << "Err file is " << DFC.ErrFile << endl;
      DFC.oflog << "Out file is " << DFC.OutFile << endl;
      if(!DFC.OutRinexObs.empty())
         DFC.oflog << "Output Rinex file name is " << DFC.OutRinexObs << endl;
      if(!DFC.HDRunby.empty())
         DFC.oflog << "Output Rinex 'RUN BY' is " << DFC.HDRunby << endl;
      if(!DFC.HDObs.empty())
         DFC.oflog << "Output Rinex 'OBSERVER' is " << DFC.HDObs << endl;
      if(!DFC.HDAgency.empty())
         DFC.oflog << "Output Rinex 'AGENCY' is " << DFC.HDAgency << endl;
      if(!DFC.HDMarker.empty())
         DFC.oflog << "Output Rinex 'MARKER' is " << DFC.HDMarker << endl;
      if(!DFC.HDNumber.empty())
         DFC.oflog << "Output Rinex 'NUMBER' is " << DFC.HDNumber << endl;
      if(DFC.SmoothedRange) DFC.oflog << "'Smoothed range' option is on\n";
      if(DFC.SmoothedPhase) DFC.oflog << "'Smoothed phase' option is on\n";
      if(!DFC.Smooth) DFC.oflog << "No smoothing.\n";
      if(DFC.CAOut) DFC.oflog << "'C/A output' option is on\n";
      if(DFC.DopOut) DFC.oflog << "'Doppler output' option is on\n";

      DFC.oflog << "\nHere is the GDC configuration:\n";
      GDC.PrintParameters(DFC.oflog);
   }

   return 0;
}
catch(gpstk::Exception& e) {
      DFC.oferr << "DiscFix:GetCommandLine caught an exception " << e << endl;
      GPSTK_RETHROW(e);
}
catch (...) {
      DFC.oferr << "DiscFix:GetCommandLine caught an unknown exception\n";
}
   return -1;
}

//------------------------------------------------------------------------------------
void PreProcessArgs(const char *arg, vector<string>& Args, bool& ver)
{
try {
   if(arg[0]=='-' && arg[1]=='f') {
      string filename(arg);
      filename.erase(0,2);
      ifstream infile(filename.c_str());
      if(!infile) {
         DFC.oferr << "Error: could not open options file "
            << filename << endl;
      }
      else {
         char c;
         string buffer;
         while( infile >> buffer) {
            if(buffer[0] == '#') {         // skip to end of line
               while(infile.get(c)) { if(c=='\n') break; }
            }
            else PreProcessArgs(buffer.c_str(),Args,ver); //Args.push_back(buffer);
         }
      }
   }
   else if((arg[0]=='-' && arg[1]=='v') || string(arg)==string("--verbose")) {
      ver = true;
   }
   else Args.push_back(arg);
}
catch(gpstk::Exception& e) {
      DFC.oferr << "DiscFix:PreProcessArgs caught an exception " << e << endl;
      GPSTK_RETHROW(e);
}
catch (...) {
      DFC.oferr << "DiscFix:PreProcessArgs caught an unknown exception\n";
}
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
