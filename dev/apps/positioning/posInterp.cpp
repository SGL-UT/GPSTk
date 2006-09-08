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

/**
 * @file posInterp.cpp
 * Read a RINEX observation file which has receiver position information in auxiliary
 * header comments (e.g. the output of PRSolve), and interpolate these positions to
 * a new (higher) rate, a multiple of the nominal data rate of the input file.
 * Output the input data (unchanged) and the interpolated position information
 * to an output RINEX observation file.
 */

#include "DayTime.hpp"
#include "CommandOptionParser.hpp"
#include "CommandOption.hpp"
#include "CommandOptionWithTimeArg.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "MiscMath.hpp"             // LagrangeIterpolation()
#include "icd_200_constants.hpp"

#include <time.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace gpstk;
using namespace std;

//------------------------------------------------------------------------------------
string PrgmName("posInterp");
string PrgmVers("2.1 8/31/06");

//------------------------------------------------------------------------------------
typedef struct posInterpConfiguration {
      // input
   string ObsDirectory;
   vector<string> InputObsName;
      // output
   // header information for the output RINEX file
   string HDPrgm;
   string HDRunby;
   string HDObs;
   string HDAgency;
   string HDMarker;
   string HDNumber;
   int NrecOut;
   DayTime FirstEpoch,LastEpoch;
   double DT;
   bool Debug,Verbose,DumpMap;
      // data flow
   double ith;
   DayTime Tbeg, Tend;
      // output files
   string LogFile,OutRinexObs;
   ofstream oflog;
      // processing
   int irate;           // number of points to interpolate between epochs + 1;
                        // output has irate times as many position epochs
   double DataInt;
      // estimate DT from the data on the first reading
   double estdt[9];
   int ndt[9];
} PIConfig;

//------------------------------------------------------------------------------------
// data input from command line
PIConfig PIC;
// data used in program
const double F1=L1_MULT;
const double F2=L2_MULT;
const double if1r=1.0/(1.0-(F2/F1)*(F2/F1));
const double if2r=1.0/(1.0-(F1/F2)*(F1/F2));
clock_t totaltime;
string Title;
RinexObsStream ofstr;      // output Rinex files
RinexObsHeader rhead, rheadout;   
int inC1,inP1,inP2,inL1,inL2,inD1,inD2,inS1,inS2;      // indexes in rhead
DayTime CurrEpoch,PrgmEpoch,PrevEpoch;

// map of <epoch,position>
typedef struct pos_info_struct {
   double X,Y,Z,T;
   double PDOP,GDOP,rms;
   int N;
} PosInfo;
map<DayTime,PosInfo> TimePositionMap;
DayTime LastInterpolated;
int Ninterps;

//------------------------------------------------------------------------------------
// prototypes
int ReadFile(int nfile, int reading);
int ProcessHeader(RinexObsStream& ifs, int nfile, int reading);
int ProcessOneEntireEpoch(RinexObsData& ro, int reading);
int InterpolateAndOutput(void);
int AfterReadingFiles(int reading);

int GetCommandLine(int argc, char **argv);
void PreProcessArgs(const char *arg, vector<string>& Args, bool& Debug);

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   totaltime = clock();
   int iret,nfile,reading,nread;

      // Title and description
   Title = PrgmName + ", part of the GPS ToolKit, Ver " + PrgmVers + ", Run ";
   PrgmEpoch.setLocalTime();
   Title += PrgmEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S");
   Title += "\n";
   cout << Title;

      // get command line
   iret=GetCommandLine(argc, argv);
   if(iret) return iret;

   PrevEpoch = DayTime::BEGINNING_OF_TIME;

   // loop over input files - reading them twice
   Ninterps = 0;
   for(reading=1; reading <= 2; reading++) {
      nread = 0;
      for(nfile=0; nfile<PIC.InputObsName.size(); nfile++) {
         iret = ReadFile(nfile,reading);
         if(iret < 0) break;
         nread++;
      }
      // quit if error
      if(iret < 0) break;

      if(nread>0) {
         iret = AfterReadingFiles(reading);
         if(iret < 0) break;
      }

      CurrEpoch = DayTime::BEGINNING_OF_TIME;
   }

   PIC.oflog << PrgmName << " did " << Ninterps << " interpolations" << endl;
   totaltime = clock()-totaltime;
   PIC.oflog << PrgmName << " timing: " << fixed << setprecision(3)
      << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.\n";
   cout << PrgmName << " timing: " << fixed << setprecision(3)
      << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.\n";

   PIC.oflog.close();

   return iret;
}
catch(FFStreamError& e) {
   cout << "FFStream exception:\n" << e << endl;
}
catch(Exception& e) {
   cout << "GPSTK exception:\n" << e << endl;
}
catch (...) {
   cout << "Unknown exception in main." << endl;
}
}   // end main()

//------------------------------------------------------------------------------------
// open the file, read header and check for data; then loop over the epochs
// Return 0 ok, <0 fatal error, >0 non-fatal error (ie skip this file)
// 0 ok, 1 couldn't open file, 2 file doesn't have required data
int ReadFile(int nfile, int reading)
{
try {
   int i,iret;
   RinexObsData rodata;
   RinexObsStream ifstr;

      // open input file
   ifstr.open(PIC.InputObsName[nfile].c_str(),ios::in);
   if(ifstr.fail()) {
      PIC.oflog << "(" << reading << ") Failed to open input file "
         << PIC.InputObsName[nfile] << ". Abort.\n";
      return 1;
   }
   else PIC.oflog << "(" << reading << ") Opened input file "
      << PIC.InputObsName[nfile] << endl;
   ifstr.exceptions(ios::failbit);

      // read header and (on 2nd reading) output
   iret = ProcessHeader(ifstr, nfile, reading);
   if(iret) return iret;

      // loop over epochs in the file
   if(reading == 2)
      LastInterpolated = DayTime::BEGINNING_OF_TIME;

   while(1) {
      try {
         ifstr >> rodata;
      }
      catch(Exception& e) {
         GPSTK_RETHROW(e);
      }
      catch(...) {
         Exception e("Unknown exception in ReadFile() from operator>>");
         GPSTK_THROW(e);
         break;
      }
      if(ifstr.eof()) break;
      if(ifstr.bad()) {
         Exception e("Bad read in ReadFile() from operator>>");
         GPSTK_THROW(e);
      }
      iret = ProcessOneEntireEpoch(rodata,reading);
      if(iret < -1) break;
      if(iret == -1) { iret=0; break; }         // end of file
      if(iret == 1) continue;                   // ignore this epoch
   }

   ifstr.clear();
   ifstr.close();
   
   PIC.oflog << endl << "Finished reading (" << reading
      << ") file " << PIC.InputObsName[nfile] << endl;

   return iret;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch (...) {
   Exception e("Unknown exception");
   GPSTK_THROW(e);
}
   return -1;
}

//------------------------------------------------------------------------------------
int ProcessHeader(RinexObsStream& ifs, int nfile, int reading)
{
try {
      // read the header
   try {
      ifs >> rhead;
   }
   catch(Exception& e) { GPSTK_RETHROW(e); }
   catch(...) {
      Exception e("Unknown exception");
      GPSTK_THROW(e);
   }

   if(reading==1) {                    // dump the input header
      if(PIC.Verbose) {
         PIC.oflog << "Input header from file " << PIC.InputObsName[nfile]
            << " follows:" << endl;
         rhead.dump(PIC.oflog);
      }
   }

   if(reading == 2) {                 // edit the output Rinex header
      rheadout = rhead;
      // the writer does this
      // rheadout.date = PrgmEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S");
      rheadout.fileProgram = PIC.HDPrgm;
      if(!PIC.HDRunby.empty()) rheadout.fileAgency = PIC.HDRunby;
      if(!PIC.HDObs.empty()) rheadout.observer = PIC.HDObs;
      if(!PIC.HDAgency.empty()) rheadout.agency = PIC.HDAgency;
      if(!PIC.HDMarker.empty()) rheadout.markerName = PIC.HDMarker;
      if(!PIC.HDNumber.empty()) {
         rheadout.markerNumber = PIC.HDNumber;
         rheadout.valid |= RinexObsHeader::markerNumberValid;
      }
      rheadout.version = 2.1;
      rheadout.valid |= RinexObsHeader::versionValid;
      rheadout.firstObs = PIC.FirstEpoch;
      rheadout.valid |= RinexObsHeader::firstTimeValid;
      //rheadout.interval = PIC.DT;
      //rheadout.valid |= RinexObsHeader::intervalValid;
      //rheadout.lastObs = PIC.LastEpoch;
      //rheadout.valid |= RinexObsHeader::lastTimeValid;
         // invalidate the table
      if(rheadout.valid & RinexObsHeader::numSatsValid)
         rheadout.valid ^= RinexObsHeader::numSatsValid;
      if(rheadout.valid & RinexObsHeader::prnObsValid)
         rheadout.valid ^= RinexObsHeader::prnObsValid;

      ofstr << rheadout;

         // dump it
      if(PIC.Verbose) {
         PIC.oflog << "Output header: " << endl;
         rheadout.dump(PIC.oflog);
         PIC.oflog << endl;
      }
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch (...) {
   Exception e("Unknown exception");
   GPSTK_THROW(e);
}
   return -1;
}

//------------------------------------------------------------------------------------
// Return 0 ok,
//      <-1 fatal error,
//       -1 end of file,
//        1 skip this epoch,
//        2 output to Rinex,
//        3 output position also
int ProcessOneEntireEpoch(RinexObsData& roe, int reading)
{
try {
   int i,j;
   double dt;

      // stay within time limits
   if(roe.time < PIC.Tbeg) return 1;
   if(roe.time > PIC.Tend) return -1;

      // ignore comment blocks on second reading
   //if(roe.epochFlag != 0 && roe.epochFlag != 1) return 1;

      // decimate data and positions (aux header info)
   if(PIC.ith > 0.0) {
      // if Tbeg is still undefined, set it to begin of week
      if(PIC.Tbeg == DayTime::BEGINNING_OF_TIME)
         PIC.Tbeg = PIC.Tbeg.setGPSfullweek(roe.time.GPSfullweek(),0.0);

      dt = fabs(roe.time - PIC.Tbeg);
      dt -= PIC.ith*long(0.5+dt/PIC.ith);

      if(fabs(dt) > 0.25) return 1;            // TD set tolerance? clock bias?
   }

      // save current time
   PrevEpoch = CurrEpoch;
   CurrEpoch = roe.time;
   if(PIC.FirstEpoch == DayTime::BEGINNING_OF_TIME) { // used in output header
      PIC.FirstEpoch = CurrEpoch;
      PIC.oflog << "First data epoch is "
         << PIC.FirstEpoch.printf("%04Y/%02m/%02d %02H:%02M:%6.3f = %4F %.3g")
         << endl;
   }

   if(reading == 1) { // read positions and add to map

      // look for auxiliary header records only
      if(roe.epochFlag != 4) return 0;

      // pull the info out of the comments
      PosInfo PI;
      for(j=0,i=0; i<roe.auxHeader.commentList.size(); i++) {
         string line = StringUtils::stripTrailing(roe.auxHeader.commentList[i],
                              string("COMMENT"),1);
         string word = StringUtils::stripFirstWord(line);
         if(word == "XYZT") {
            PI.X = StringUtils::asDouble(StringUtils::stripFirstWord(line));
            PI.Y = StringUtils::asDouble(StringUtils::stripFirstWord(line));
            PI.Z = StringUtils::asDouble(StringUtils::stripFirstWord(line));
            PI.T = StringUtils::asDouble(StringUtils::stripFirstWord(line));
            j++;
         }
         else if(word == "DIAG") {
            PI.N = StringUtils::asInt(StringUtils::stripFirstWord(line));
            PI.PDOP = StringUtils::asDouble(StringUtils::stripFirstWord(line));
            PI.GDOP = StringUtils::asDouble(StringUtils::stripFirstWord(line));
            PI.rms = StringUtils::asDouble(StringUtils::stripFirstWord(line));
            j++;
         }
         else { // ignore
         }
      }

      // add to map<DayTime,PosInfo> TimePositionMap;
      TimePositionMap[CurrEpoch] = PI;

      // update LastEpoch and estimate of PIC.DT
      if(PIC.LastEpoch > DayTime::BEGINNING_OF_TIME) {
         double dt = CurrEpoch-PIC.LastEpoch;
         for(i=0; i<9; i++) {
            if(PIC.ndt[i]<=0) { PIC.estdt[i]=dt; PIC.ndt[i]=1; break; }
            if(ABS(dt-PIC.estdt[i]) < 0.0001) { PIC.ndt[i]++; break; }
            if(i == 8) {
               int k=0,nl=PIC.ndt[k];
               for(int j=1; j<9; j++) if(PIC.ndt[j] <= nl) { k=j; nl=PIC.ndt[j]; }
               PIC.ndt[k]=1; PIC.estdt[k]=dt;
            }
         }
      }
      PIC.LastEpoch = CurrEpoch;
   }

   // reading 2 - interpolate and output positions, and output data
   else {
      if(roe.epochFlag == 4)
         // do nothing - InterpolateAndOutput will write this position from the map
         ;
      else {
         InterpolateAndOutput();       // interpolate
         ofstr << roe;                 // output data
      }
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch (...) {
   Exception e("Unknown exception");
   GPSTK_THROW(e);
}
}

//------------------------------------------------------------------------------------
void RinexPositionComments(RinexObsData& psdata, DayTime& tt, int N,
   double& X, double& Y, double& Z, double& T,
   double& PDOP, double& GDOP, double& RMS)
{
try {
   ostringstream stst1,stst2;

   psdata.auxHeader.clear();
   psdata.time = tt;
   psdata.epochFlag = 4;            // mark it as in-line header info
   psdata.numSvs = 2;               // number of comments
   stst1 << "XYZT";
   stst1 << fixed << " " << setw(13) << setprecision(3) << X;
   stst1 << fixed << " " << setw(13) << setprecision(3) << Y;
   stst1 << fixed << " " << setw(13) << setprecision(3) << Z;
   stst1 << fixed << " " << setw(13) << setprecision(3) << T;
   psdata.auxHeader.commentList.push_back(stst1.str());

   stst2 << "DIAG";
   stst2 << " " << setw(2) << N
      << " " << fixed << setw(5) << setprecision(2) << PDOP
      << " " << fixed << setw(5) << setprecision(2) << GDOP
      << " " << fixed << setw(9) << setprecision(3) << RMS
      << " (N,P-,G-Dop,RMS)";

   if(PIC.Debug)
      PIC.oflog << psdata.time.printf("%02M:%04.1f ") << stst1.str()
         << " " << stst2.str() << endl;

   psdata.auxHeader.commentList.push_back(stst2.str());
   psdata.auxHeader.valid |= RinexObsHeader::commentValid;
}
catch(Exception& e) {
   GPSTK_RETHROW(e);
}
catch (...) {
   Exception e("Unknown exception");
   GPSTK_THROW(e);
}
}

//------------------------------------------------------------------------------------
int InterpolateAndOutput(void)
{                        // interpolate positions and output to rinex
try {
   bool Lagrange;
   int i,ipts;
   double PDOP,GDOP,rms,err,dt,Dt,delt,xx,yy,zz,tt;
   DayTime t0,ttag;
   RinexObsData psdata;
   vector<double> times,X,Y,Z,T;
   map<DayTime,PosInfo>::iterator itb,ite,itB,itE,itr;

      // if no previous epoch, nothing to do
   if(PrevEpoch==DayTime::BEGINNING_OF_TIME)
      return 0;

      // find 4 positions on each side of CurrEpoch-1/2dt
   ite = TimePositionMap.lower_bound(CurrEpoch);
   if(ite == TimePositionMap.end())
      return 0;    // no position; get it next time
   if(ite->first - LastInterpolated < 0)
      return 0;   // already done

   itb = ite;
   if(itb == TimePositionMap.begin()) {
      //PIC.oflog << "Warning: cannot interpolate at " << CurrEpoch
         //<< ": before beginning of data" << endl;
      PIC.oflog << "Echo position at first epoch "
         << CurrEpoch.printf("%04Y/%02m/%02d %02H:%02M:%6.3f = %4F %.3g")
         << endl;

      // create the aux header
      // use data from the begin time
      RinexPositionComments(psdata,CurrEpoch,
         itb->second.N,
         itb->second.X,
         itb->second.Y,
         itb->second.Z,
         itb->second.T,
         itb->second.PDOP,
         itb->second.GDOP,
         itb->second.rms);

      // write it out
      ofstr << psdata;

      return 0;
   }
   itb--;

      // itb and ite are now on either side of the times at which to interpolate
   if(PIC.Debug) PIC.oflog << "Interpolate : "
      << itb->first.printf("%02H:%02M:%04.1f") << " to "
      << ite->first.printf("%02H:%02M:%04.1f")
      << " : (" << (ite->first - itb->first) << " sec)" << endl;

      // now expand them out, up to 3 more epochs, watching for gaps TD: 3*DT input
   itB = itb;
   itE = ite;
   for(i=0; i<3; i++) {
      if(itB == TimePositionMap.begin() || (i==0 && itE->first-itB->first > 3*PIC.DT))
         break;

         // increase the end time
      ttag = itE->first;
      itE++;
      if(itE == TimePositionMap.end() || itE->first-ttag > 3*PIC.DT) { itE--; break; }

         // decrease the begin time
      ttag = itB->first;
      itB--;
      if(ttag-itB->first > 3*PIC.DT) { itE--; itB++; break; }

   }

      // fill the arrays for interpolation
   t0 = itB->first;
   ipts = 1;
   itr = itB;
   if(PIC.Debug) PIC.oflog << "Data for interpolation:\n";

   while(1) {
      if(PIC.Debug) PIC.oflog << " " << ipts
         << " " << itr->first.printf("%02M:%04.1f")
         << fixed << setprecision(3)
         << " " << setw(6) << (itr->first-t0)
         << " " << setw(13) << itr->second.X
         << " " << setw(13) << itr->second.Y
         << " " << setw(13) << itr->second.Z
         << ((itr==itb || itr==ite) ? " *":"")
         << endl;

      times.push_back(itr->first - t0);      // sec
      X.push_back(itr->second.X);  // m
      Y.push_back(itr->second.Y);
      Z.push_back(itr->second.Z);
      T.push_back(itr->second.T);
      if(itr == itE) break;
      itr++;
      ipts++;
   }

      // compute the time intervals involved
   ttag = itb->first;
   Dt = ite->first - ttag;          // time interval over which interpolating
   if(Dt > 3*PIC.DT) {
      PIC.oflog << "Warning: cannot interpolate at " << CurrEpoch
         << ": large gap = " << Dt << " seconds." << endl;
      return 0;
   }
   dt = PIC.DT/double(PIC.irate);

      // is there enough data to do Lagrange interpolation?
      // fall back to linear interpolation if have to, but not over long periods
   if(ipts < 2 || (ipts==2 && Dt>3*PIC.DT)) {
      PIC.oflog << "Warning: cannot interpolate at " << CurrEpoch
         << ": not enough data" << endl;
      return 0;              // not enough data
   }
   Lagrange = (ipts==2?false:true);

   // number of interpolations needed to cover Dt, plus 1 endpt
   ipts = int(0.5+Dt/dt);
   //PIC.oflog << "Dt dt and ipts are " << Dt << " " << dt << " " << ipts << endl;

      // loop over the interpolation times you want
   delt = itb->first - t0;          // time since first data point
   for(i=0; i<ipts; i++) {
      ttag += dt;               // itb->first was done last epoch
      delt += dt;

         // use 1. known position if i==ipts-1 (last epoch, ite)
         //     2. Lagrange if you have more than 1 data on each side
         //     3. linear interpolation
      xx = (i==ipts-1 ?
            ite->second.X :
               (Lagrange ?
                  LagrangeInterpolation(times,X,delt,err) :
                     X[0]+(X[1]-X[0])*delt/Dt));
      yy = (i==ipts-1 ?
            ite->second.Y :
               (Lagrange ?
                  LagrangeInterpolation(times,Y,delt,err) :
                     Y[0]+(Y[1]-Y[0])*delt/Dt));
      zz = (i==ipts-1 ?
            ite->second.Z :
               (Lagrange ?
                  LagrangeInterpolation(times,Z,delt,err) :
                     Z[0]+(Z[1]-Z[0])*delt/Dt));
      tt = (i==ipts-1 ?
            ite->second.T :
               (Lagrange ?
                  LagrangeInterpolation(times,T,delt,err) :
                     T[0]+(T[1]-T[0])*delt/Dt));

      // create the aux header
      // use N,DOPs,RMS of _end_ time for all interpolated times
      RinexPositionComments(psdata,ttag,
         ite->second.N,
         xx,yy,zz,tt,
         ite->second.PDOP,
         ite->second.GDOP,
         ite->second.rms);

      // write it out
      ofstr << psdata;

      if(i != ipts-1) Ninterps++;
      LastInterpolated = ttag;
   }

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch (...) {
   Exception e("Unknown exception");
   GPSTK_THROW(e);
}
}

//------------------------------------------------------------------------------------
int AfterReadingFiles(int reading)
{
try {
   int i,j,iret=0;
   double dt;

   if(reading == 1) {

         // compute data interval for this file
      for(j=0,i=1; i<9; i++) { if(PIC.ndt[i]>PIC.ndt[j]) j=i; }
      PIC.DT = PIC.estdt[j];
      PIC.oflog << endl;
      PIC.oflog << "Estimated data interval is " << PIC.DT << " seconds.\n";
      PIC.oflog << "Interpolate to " << PIC.irate << " times the input data rate\n";
      PIC.oflog << "Last data epoch is "
         << PIC.LastEpoch.printf("%04Y/%02m/%02d %02H:%02M:%6.3f = %4F %.3g") << endl;

      if(TimePositionMap.size() == 0) {
         cout << "No position information was found in the input file! Abort.\n";
         PIC.oflog << "No position information was found in the input file! Abort.\n";
         return -1;
      }
      PIC.oflog << endl;

         // dump the map of positions
      if(PIC.DumpMap) {
         PIC.oflog << "Here is all the Time/Position information:\n";
         map<DayTime,PosInfo>::const_iterator itr;
         itr = TimePositionMap.begin();
         i = 0;
         while(itr != TimePositionMap.end()) {
            PIC.oflog << setw(4) << i
               << " " << itr->first.printf("%04Y/%02m/%02d %02H:%02M:%6.3f")
               << setprecision(3)
               << " " << setw(13) << itr->second.X
               << " " << setw(13) << itr->second.Y
               << " " << setw(13) << itr->second.Z
               << " " << setw(13) << itr->second.T
               << " " << setw(7) << itr->second.rms
               << endl;
            itr++;
            i++;
         }
         PIC.oflog << "End of the Time/Position information.\n\n";
      }

         // open output file
      if(!PIC.OutRinexObs.empty()) {
         ofstr.open(PIC.OutRinexObs.c_str(), ios::out);
         if(ofstr.fail()) {
            PIC.oflog << "Failed to open output file " << PIC.OutRinexObs
               << ". Abort.\n";
            return 1;
         }
         else PIC.oflog << "Opened output file " << PIC.OutRinexObs << endl;
         ofstr.exceptions(ios::failbit);
      }
   }
   else if(reading==2) {
      PIC.oflog << "Close the output file\n";
      ofstr.close();
   }

   return iret;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch (...) {
   Exception e("Unknown exception");
   GPSTK_THROW(e);
}
}

//------------------------------------------------------------------------------------
int GetCommandLine(int argc, char **argv)
{
   bool help=false;
   int i,j;
try {
      // defaults
   PIC.Debug = false;
   PIC.DumpMap = false;
   PIC.ith = 0.0;
   PIC.Tbeg = PIC.FirstEpoch = DayTime::BEGINNING_OF_TIME;
   PIC.Tend = DayTime::END_OF_TIME;
   PIC.DT = 0;

   PIC.LogFile = string("pi.log");

   PIC.irate = 4;
   PIC.DataInt = -1.0;
   
   PIC.HDPrgm = PrgmName + string(" v.") + PrgmVers.substr(0,4);
   PIC.HDRunby = string("ARL:UT/SGL/GPSTK");

   for(i=0; i<9; i++) PIC.ndt[i]=-1;

   PIC.ObsDirectory = string(".");

      // -------------------------------------------------
      // required options
   RequiredOption dashi(CommandOption::hasArgument, CommandOption::stdType,
      'o',"obs"," [-o|--obs] <file>   Input Rinex observation file(s)"
      " (e.g. output of PRSolve)");

   RequiredOption dashirate(CommandOption::hasArgument, CommandOption::stdType,
      'm',"mult", " [-m|--mult] <M>     "
      "Interpolation: output has M times as many epochs as input");

      // optional options
   // this is here only so it will show up in help page...
   CommandOption dashf(CommandOption::hasArgument, CommandOption::stdType,
      'f',""," -f<file>            File containing more options");

   CommandOption dashdo(CommandOption::hasArgument, CommandOption::stdType,
      0,"obsdir"," --obsdir <dir>      Directory of input observation file(s)");
   dashdo.setMaxCount(1);

   CommandOption dashith(CommandOption::hasArgument, CommandOption::stdType,
        0,"decimate"," --decimate <dt>     "
        "Decimate data and positions to time interval dt");
   dashith.setMaxCount(1);

   // time
   CommandOption dashbt(CommandOption::hasArgument, CommandOption::stdType,
      0,"BeginTime", " --BeginTime <arg>   "
      "Start time: arg is 'GPSwk,sow' OR 'YYYY,MM,DD,HH,Min,Sec'");
   dashbt.setMaxCount(1);

   CommandOption dashet(CommandOption::hasArgument, CommandOption::stdType,
      0,"EndTime",   " --EndTime <arg>     "
      "End time: arg is 'GPSwk,sow' OR 'YYYY,MM,DD,HH,Min,Sec'");
   dashet.setMaxCount(1);

   //CommandOption dashDT(CommandOption::hasArgument, CommandOption::stdType,
      //0,"DT"," --DT <dt>              Time interval (sec) of data points");
   //dashDT.setMaxCount(1);
   
   CommandOption dashLog(CommandOption::hasArgument, CommandOption::stdType,
      0,"Log"," --Log <file>        Output log file name (pi.log)");
   dashLog.setMaxCount(1);
   
   CommandOption dashRfile(CommandOption::hasArgument, CommandOption::stdType,
      0,"outRinex"," --outRinex <file>     Output Rinex obs file name");
   dashRfile.setMaxCount(1);
   
   CommandOption dashRrun(CommandOption::hasArgument, CommandOption::stdType,
      0,"RunBy"," --RunBy <string>    Output Rinex header 'RUN BY' string");
   dashRrun.setMaxCount(1);
   
   CommandOption dashRobs(CommandOption::hasArgument, CommandOption::stdType,
      0,"Observer"," --Observer <string> Output Rinex header 'OBSERVER' string");
   dashRobs.setMaxCount(1);
   
   CommandOption dashRag(CommandOption::hasArgument, CommandOption::stdType,
      0,"Agency"," --Agency <string>   Output Rinex header 'AGENCY' string");
   dashRag.setMaxCount(1);
   
   CommandOption dashRmark(CommandOption::hasArgument, CommandOption::stdType,
      0,"Marker"," --Marker <string>   Output Rinex header 'MARKER' string");
   dashRmark.setMaxCount(1);
   
   CommandOption dashRnumb(CommandOption::hasArgument, CommandOption::stdType,
      0,"Number"," --Number <string>   Output Rinex header 'NUMBER' string");
   dashRnumb.setMaxCount(1);
   
   CommandOptionNoArg dashdmap(0,"dumpMap",
      " --dumpMap           Dump the Time/Position map to the log file.");

   CommandOptionNoArg dashVerb(0,"verbose",
      " --verbose           Print extended output");
   dashVerb.setMaxCount(1);

   CommandOptionNoArg dashDebug('d',"debug",
      " --debug             Print even more extended output.");
   dashDebug.setMaxCount(1);

   CommandOptionNoArg dashh('h', "help",
      " [-h|--help]         Print syntax and quit.");

   // ... other options
   CommandOptionRest Rest("");

   CommandOptionParser Par("Prgm " + PrgmName +
   " reads a RINEX observation file which has position information\n"
   " in auxiliary header comments (e.g. the output of PRSolve), and interpolates\n"
   " these positions to produce positions at a new (higher) rate, which is a\n"
   " multiple of the nominal data rate of the input file. It then outputs the\n"
   " input data (unchanged) and the interpolated position information to an\n"
   " output RINEX observation file.\n");

      // -------------------------------------------------
      // allow user to put all options in a file
      // could also scan for debug here
   vector<string> Args;
   for(j=1; j<argc; j++) PreProcessArgs(argv[j],Args,PIC.Debug);

   argc = Args.size();
   if(argc==0) Args.push_back(string("-h"));

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
   vector<string> values,field;

      // f never appears because we intercept it above
   //if(dashf.getCount()) { cout << "Option f "; dashf.dumpValue(cout); }

      // do help first
   if(dashh.getCount()) help=true;
   if(dashDebug.getCount()) {
      PIC.Debug=true;
   }
   if(dashVerb.getCount()) {
      PIC.Verbose=true;
   }
   if(dashdmap.getCount()) {
      PIC.DumpMap=true;
   }
   if(dashdo.getCount()) {
      values = dashdo.getValue();
      PIC.ObsDirectory = values[0];
      if(help) cout << "Input obs directory is " << PIC.ObsDirectory << endl;
   }
   if(dashi.getCount()) {
      values = dashi.getValue();
      if(help) cout << "Input Rinex obs files are:\n";
      for(i=0; i<values.size(); i++) {
         PIC.InputObsName.push_back(PIC.ObsDirectory + string("/") + values[i]);
         if(help) cout << "   " << PIC.ObsDirectory + string("/") + values[i] << endl;
      }
   }
   if(dashirate.getCount()) {
      values = dashirate.getValue();
      PIC.irate = StringUtils::asInt(values[0]);
      if(PIC.irate < 0) PIC.irate *= -1;
      if(PIC.irate == 0) {
         cout << "ERROR: Interpolation rate must be positive (-m<M>)\n";
         help = true;
      }
      if(PIC.irate == 1) {
         cout << "WARNING: Interpolation rate must > 1\n";
      }
   }
   if(dashith.getCount()) {
      values = dashith.getValue();
      PIC.ith = StringUtils::asDouble(values[0]);
      if(help) cout << "Ithing values is " << PIC.ith << endl;
   }
   if(dashbt.getCount()) {
      values = dashbt.getValue();
      field.clear();
      while(values[0].size() > 0)
         field.push_back(StringUtils::stripFirstWord(values[0],','));
      if(field.size() == 2)
         PIC.Tbeg.setToString(field[0]+","+field[1], "%F,%g");
      else if(field.size() == 6)
         PIC.Tbeg.setToString(field[0]+","+field[1]+","+field[2]+","+field[3]+","
            +field[4]+","+field[5], "%Y,%m,%d,%H,%M,%S");
      else {
         cout << "Error: invalid --BeginTime input: " << values[0] << endl;
      }
      if(help) cout << "Begin time is " << values[0] << " = "
         << PIC.Tbeg.printf("%Y/%02m/%02d %2H:%02M:%06.3f = %F/%10.3g") << endl;
   }
   if(dashet.getCount()) {
      values = dashet.getValue();
      field.clear();
      while(values[0].size() > 0)
         field.push_back(StringUtils::stripFirstWord(values[0],','));
      if(field.size() == 2)
         PIC.Tend.setToString(field[0]+","+field[1], "%F,%g");
      else if(field.size() == 6)
         PIC.Tend.setToString(field[0]+","+field[1]+","+field[2]+","+field[3]+","
            +field[4]+","+field[5], "%Y,%m,%d,%H,%M,%S");
      else {
         cout << "Error: invalid --EndTime input: " << values[0] << endl;
      }
      if(help) cout << "End time is " << values[0] << " = "
         << PIC.Tend.printf("%Y/%02m/%02d %2H:%02M:%06.3f = %F/%10.3g") << endl;
   }
   //if(dashDT.getCount()) {
      //values = dashDT.getValue();
      //PIC.DT = StringUtils::asDouble(values[0]);
      //if(help) cout << "DT is set to " << PIC.DT << endl;
   //}
   if(dashLog.getCount()) {
      values = dashLog.getValue();
      PIC.LogFile = values[0];
      if(help) cout << "Log file is " << PIC.LogFile << endl;
   }
   if(dashRfile.getCount()) {
      values = dashRfile.getValue();
      PIC.OutRinexObs = values[0];
      if(help) cout << "Output Rinex file name is " << PIC.OutRinexObs << endl;
   }
   if(dashRrun.getCount()) {
      values = dashRrun.getValue();
      PIC.HDRunby = values[0];
      if(help) cout << "Output Rinex 'RUN BY' is " << PIC.HDRunby << endl;
   }
   if(dashRobs.getCount()) {
      values = dashRobs.getValue();
      PIC.HDObs = values[0];
      if(help) cout << "Output Rinex 'OBSERVER' is " << PIC.HDObs << endl;
   }
   if(dashRag.getCount()) {
      values = dashRag.getValue();
      PIC.HDAgency = values[0];
      if(help) cout << "Output Rinex 'AGENCY' is " << PIC.HDAgency << endl;
   }
   if(dashRmark.getCount()) {
      values = dashRmark.getValue();
      PIC.HDMarker = values[0];
      if(help) cout << "Output Rinex 'MARKER' is " << PIC.HDMarker << endl;
   }
   if(dashRnumb.getCount()) {
      values = dashRnumb.getValue();
      PIC.HDNumber = values[0];
      if(help) cout << "Output Rinex 'NUMBER' is " << PIC.HDNumber << endl;
   }

   if(Rest.getCount()) {
      if(help) cout << "Remaining options:" << endl;
      values = Rest.getValue();
      for (i=0; i<values.size(); i++) {
         if(help) cout << values[i] << endl;
      }
   }

   PIC.oflog.open(PIC.LogFile.c_str(),ios::out);
   if(PIC.oflog.fail()) {
      cout << "Failed to open log file " << PIC.LogFile << endl;
   }
   else {
      cout << "Opened log file " << PIC.LogFile << endl;
      PIC.oflog << Title;
   }

      // print config to log
   if(help || PIC.Verbose) {
      PIC.oflog << "Input configuration:\n";
      PIC.oflog << " Obs directory is " << PIC.ObsDirectory << endl;
      PIC.oflog << " Input Rinex obs files are:\n";
      for(i=0; i<PIC.InputObsName.size(); i++)
         PIC.oflog << "  " << PIC.InputObsName[i] << endl;
      PIC.oflog << " Interpolate to " << PIC.irate << " times the input data rate"
         << endl;
      if(PIC.ith > 0) PIC.oflog << " Ithing time interval is " << PIC.ith << endl;
      if(PIC.Tbeg > DayTime::BEGINNING_OF_TIME)
         PIC.oflog << " Begin time is "
            << PIC.Tbeg.printf("%04Y/%02m/%02d %02H:%02M:%.3f")
            << " = " << PIC.Tbeg.printf("%04F/%10.3g") << endl;
      if(PIC.Tend < DayTime::END_OF_TIME)
         PIC.oflog << " End time is "
            << PIC.Tend.printf("%04Y/%02m/%02d %02H:%02M:%.3f")
            << " = " << PIC.Tend.printf("%04F/%10.3g") << endl;
      //PIC.oflog << "DT is set to " << PIC.DT << endl;
      PIC.oflog << " Log file is " << PIC.LogFile << endl;
      if(!PIC.OutRinexObs.empty())
         PIC.oflog << " Output Rinex file name is " << PIC.OutRinexObs << endl;
      if(!PIC.HDRunby.empty())
         PIC.oflog << " Output Rinex 'RUN BY' is " << PIC.HDRunby << endl;
      if(!PIC.HDObs.empty())
         PIC.oflog << " Output Rinex 'OBSERVER' is " << PIC.HDObs << endl;
      if(!PIC.HDAgency.empty())
         PIC.oflog << " Output Rinex 'AGENCY' is " << PIC.HDAgency << endl;
      if(!PIC.HDMarker.empty())
         PIC.oflog << " Output Rinex 'MARKER' is " << PIC.HDMarker << endl;
      if(!PIC.HDNumber.empty())
         PIC.oflog << " Output Rinex 'NUMBER' is " << PIC.HDNumber << endl;
      if(PIC.Verbose) PIC.oflog << " 'Verbose' option is on\n";
      if(PIC.Debug) PIC.oflog << " 'Debug' option is on\n";
      PIC.oflog << "End of the input configuration.\n\n";
   }

   if(help) return 1;
   if(PIC.Debug) PIC.Verbose=true;
   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch (...) {
   Exception e("Unknown exception");
   GPSTK_THROW(e);
}
}

//------------------------------------------------------------------------------------
void PreProcessArgs(const char *arg, vector<string>& Args, bool& dbug)
{
try {
   if(arg[0]=='-' && arg[1]=='f') {
      string filename(arg);
      filename.erase(0,2);
      ifstream infile(filename.c_str());
      if(!infile) {
         cout << "Error: could not open options file "
            << filename << endl;
      }
      else {
         char c;
         string buffer;
         while( infile >> buffer) {
            if(buffer[0] == '#') {         // skip to end of line
               while(infile.get(c)) { if(c=='\n') break; }
            }
            else PreProcessArgs(buffer.c_str(),Args,dbug);
         }
      }
   }
   else if((arg[0]=='-' && arg[1]=='d') || string(arg)==string("--debug")) {
      dbug = true;
   }
   // old versions of args -- deprecated
   else if(string(arg)=="--input") { Args.push_back("--obs"); }
   else if(string(arg)=="--EpochBeg") { Args.push_back("--BeginTime"); }
   else if(string(arg)=="--GPSBeg") { Args.push_back("--BeginTime"); }
   else if(string(arg)=="--EpochEnd") { Args.push_back("--EndTime"); }
   else if(string(arg)=="--GPSEnd") { Args.push_back("--EndTime"); }
   else if(string(arg)=="--output") { Args.push_back("--outRinex"); }
   else {
      Args.push_back(arg);
   }
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch (...) {
   Exception e("Unknown exception");
   GPSTK_THROW(e);
}
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
