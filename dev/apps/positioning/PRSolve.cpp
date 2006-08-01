// PRSolve.cpp  Read a Rinex observation file and compute an autonomous
// pseudorange position solution, using a RAIM-like algorithm to eliminate outliers.
// $Id$

#define RANGECHECK 1        // make Matrix and Vector check limits
#include "Exception.hpp"
#include "DayTime.hpp"
#include "CommandOptionParser.hpp"
#include "CommandOption.hpp"
#include "CommandOptionWithTimeArg.hpp"

#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"
#include "SP3Stream.hpp"
#include "BCEphemerisStore.hpp"
#include "SP3EphemerisStore.hpp"
#include "TropModel.hpp"
#include "Position.hpp"
#include "geometry.hpp" // for DEG_TO_RAD

#include "Matrix.hpp"
#include "PRSolution.hpp"
#include "Stats.hpp"
#include "EphemerisRange.hpp"

#include <time.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace gpstk;
using namespace std;

   // prgm data
string PrgmName("PRSolve");
string PrgmVers("1.7 3/06");

typedef struct Configuration {
      // input files
   string ObsDirectory,NavDirectory;
   vector<string> InputObsName;
   vector<string> InputNavName;
      // configuration
   double rmsLimit;
   double slopeLimit;
   bool algebra;
   int nIter;
   double convLimit;
   int maxReject;
   bool residCrit;
   bool returnatonce;
   double elevLimit;
      // output
   Position knownpos;
   Matrix<double> Rot;
   bool APSout;
   string OutRinexObs;
   string HDPrgm;       // header of output Rinex file
   string HDRunby;
   string HDObs;
   string HDAgency;
   string HDMarker;
   string HDNumber;
   int NrecOut;
   DayTime FirstEpoch,LastEpoch;
   string timeFormat;
   //double DT;
   bool Debug,Verbose;
      // data flow
   double ith;
   DayTime Tbeg, Tend;
      // output files
   string LogFile;
   ofstream oflog;
      // processing
   double DataInt;
   int Freq;
   bool UseCA;
   vector<RinexPrn> ExSV;
   string TropType;
   double T,Pr,RH;
   TropModel *pTropModel;
      // estimate DT from data
   double estdt[9];
   int ndt[9];
} Config;

// data input from command line
Config C;
// data used in program
const double CMPS=299792458.0;
const double CFF=CMPS/10.23e6;
const double F1=154.0;
const double F2=120.0;
const double wl1=CFF/F1;
const double wl2=CFF/F2;
const double alpha=((F1*F1)/(F2*F2) - 1.0);
const double if1r=1.0/(1.0-(F2/F1)*(F2/F1));
const double if2r=1.0/(1.0-(F1/F2)*(F1/F2));
clock_t totaltime;
string Title,filename;
DayTime CurrEpoch(DayTime::BEGINNING_OF_TIME), PrgmEpoch;
DayTime PrevEpoch(DayTime::BEGINNING_OF_TIME);

// data
int Nsvs;
EphemerisStore *pEph;
SP3EphemerisStore SP3EphList;
BCEphemerisStore BCEphList;
SimpleTropModel TMsimple;
SaasTropModel TMsaas;
GGTropModel TMgg;
GGHeightTropModel TMggh;
NBTropModel TMnb;

// Solution and covariance (prsol for RAIM, Solution and Covariance for AutonPRSol)
PRSolution prsol;             // this will always be the RAIM result
Vector<double> Solution;      // this will always be the AutonPRS result
Matrix<double> Covariance;    // this will always be the AutonPRS result

// Solution and residual statistics:
// simple average (S : one file; SS : all files)...
Stats<double> SA[3],SR[3],SSA[3],SSR[3];          // solution (XYZ) Auto and RAIM
Stats<double> SAPR[3],SRPR[3],SSAPR[3],SSRPR[3];  // XYZ residuals
Stats<double> SANE[3],SRNE[3],SSANE[3],SSRNE[3];  // NEU residuals
// ... and weighted average solution, both Auto and RAIM...
Matrix<double> PA,PR,PPA,PPR;        // inverse covariance
Vector<double> zA,zR,zzA,zzR;        // 'state'=(inverse covariance * state)
// ... and weighted average residuals APR,RPR.
Matrix<double> PAPR,PRPR,PANE,PRNE,PPAPR,PPRPR,PPANE,PPRNE;
Vector<double> zAPR,zRPR,zANE,zRNE,zzAPR,zzRPR,zzANE,zzRNE;

//------------------------------------------------------------------------------------
// prototypes
int ReadFile(int nfile);
int SolutionAlgorithm(vector<RinexPrn>& Sats,
                      vector<double>& PRanges,
                      double& RMSresid);
int AfterReadingFiles(void);

void PrintStats(Stats<double> S[3], Matrix<double> &P, Vector<double> &z, string m,
   char c0='X', char c1='Y', char c2='Z');
int GetCommandLine(int argc, char **argv);
void PreProcessArgs(const char *arg, vector<string>& Args, bool& Verbose);

int FillEphemerisStore(const vector<string>& files, SP3EphemerisStore& PE,
  BCEphemerisStore& BCE);

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   totaltime = clock();
   int iret;

      // Title and description
   Title = PrgmName + ", part of the GPSTK ToolKit, Ver " + PrgmVers + ", Run ";
   time_t timer;
   struct tm *tblock;
   timer = time(NULL);
   tblock = localtime(&timer);
   PrgmEpoch.setYMDHMS(1900+tblock->tm_year,1+tblock->tm_mon,
               tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
   Title += PrgmEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S\n");
   cout << Title;

      // get command line
   iret=GetCommandLine(argc, argv);
   if(iret) return iret;

   // get nav files and build EphemerisStore
   int nread = FillEphemerisStore(C.InputNavName, SP3EphList, BCEphList);
   C.oflog << "Added " << nread << " ephemeris files to store.\n";
   SP3EphList.dump(0,C.oflog);
   BCEphList.dump(0,C.oflog);
   if(SP3EphList.size() > 0) pEph=&SP3EphList;
   else if(BCEphList.size() > 0) {
      BCEphList.SearchNear();
      //BCEphList.SearchPast();
      pEph=&BCEphList;
   }
   else {
      C.oflog << "Failed to read ephemeris data. Abort." << endl;
      return -1;
   }

   // assign trop model
   if(C.TropType == string("BL")) C.pTropModel = &TMsimple;
   if(C.TropType == string("SA") || C.TropType == string("NB")) {
      if(C.TropType == string("SA")) C.pTropModel = &TMsaas;
      if(C.TropType == string("NB")) C.pTropModel = &TMnb;
      if(C.knownpos.getCoordinateSystem() != Position::Unknown) {
         C.pTropModel->setReceiverLatitude(C.knownpos.getGeodeticLatitude());
         C.pTropModel->setReceiverHeight(C.knownpos.getHeight());
      }
      else {
         C.pTropModel->setReceiverLatitude(0.0);
         C.pTropModel->setReceiverHeight(0.0);
         C.oflog << "Warning - Saastamoinen and New B tropospheric models require "
            << "latitude, height and day of year - guessing." << endl;
      }
      if(C.Tbeg > DayTime(DayTime::BEGINNING_OF_TIME))
         C.pTropModel->setDayOfYear(C.Tbeg.DOY());
      else if(C.Tend < DayTime(DayTime::END_OF_TIME))
         C.pTropModel->setDayOfYear(C.Tend.DOY());
      else
         C.pTropModel->setDayOfYear(100);
   }
   if(C.TropType == string("GG")) C.pTropModel = &TMgg;
   if(C.TropType == string("GGH")) C.pTropModel = &TMggh;
   C.pTropModel->setWeather(C.T,C.Pr,C.RH);

   // compute rotation XYZ->NEU at known position
   if(C.knownpos.getCoordinateSystem() != Position::Unknown) {
      double lat=C.knownpos.geodeticLatitude() * DEG_TO_RAD;
      double lon=C.knownpos.longitude() * DEG_TO_RAD;
      double ca = ::cos(lat);
      double sa = ::sin(lat);
      double co = ::cos(lon);
      double so = ::sin(lon);
      // Rotation matrix (R*XYZ=NEU) :
      C.Rot = Matrix<double>(3,3);
      // NEU
      C.Rot(2,0) = ca*co; C.Rot(2,1) = ca*so; C.Rot(2,2) = sa;
      C.Rot(1,0) = -so; C.Rot(1,1) = co; C.Rot(1,2) = 0.0;
      C.Rot(0,0) = -sa*co; C.Rot(0,1) = -sa*so; C.Rot(0,2) = ca;
   }

   // initialize global solution and residual statistics
   // not necessary SSA[0].Reset(); SSA[1].Reset(); SSA[2].Reset();
   // not necessary SSR[0].Reset(); SSR[1].Reset(); SSR[2].Reset();
   PPA = Matrix<double>(3,3,0.0);
   PPR = Matrix<double>(3,3,0.0);
   zzA = Vector<double>(3,0.0);
   zzR = Vector<double>(3,0.0);
   if(C.knownpos.getCoordinateSystem() != Position::Unknown) {
      if(C.APSout) {
         // not necessary SSAPR[0].Reset(); SSAPR[1].Reset(); SSAPR[2].Reset();
         // not necessary SSANE[0].Reset(); SSANE[1].Reset(); SSANE[2].Reset();
         PPAPR = Matrix<double>(3,3,0.0);
         PPANE = Matrix<double>(3,3,0.0);
         zzAPR = Vector<double>(3,0.0);
         zzANE = Vector<double>(3,0.0);
      }
      // not necessary SSRPR[0].Reset(); SSRPR[1].Reset(); SSRPR[2].Reset();
      // not necessary SSRNE[0].Reset(); SSRNE[1].Reset(); SSRNE[2].Reset();
      PPRPR = Matrix<double>(3,3,0.0);
      PPRNE = Matrix<double>(3,3,0.0);
      zzRPR = Vector<double>(3,0.0);
      zzRNE = Vector<double>(3,0.0);
   }

   // loop over input files
   nread = 0;
   for(int nfile=0; nfile<C.InputObsName.size(); nfile++) {
      iret = ReadFile(nfile);
      if(iret < 0) break;
      nread++;
   }  // end loop over input files

   if(iret>=0 && nread>0) iret=AfterReadingFiles();

   totaltime = clock()-totaltime;
   C.oflog << "PRSolve timing: " << fixed << setprecision(3)
      << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.\n";
   cout << "PRSolve timing: " << fixed << setprecision(3)
      << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.\n";

   C.oflog.close();

   return iret;
}
//catch(FFStreamError& e) {
   //cout << e;
//}
catch(Exception& e) {
   cout << e;
}
catch (...) {
   cerr << C.oflog << "Unknown error.  Abort." << endl;
}
   return 1;
}  // end main()

//------------------------------------------------------------------------------------
// open the file, read header and check for data; then loop over the epochs
// Return 0 ok, <0 fatal error, >0 non-fatal error (ie skip this file)
// 0 ok, 1 couldn't open file, 2 file doesn't have required data
int ReadFile(int nfile)
{
try {
   bool writeout, first;
   int i,j,iret;
   int inC1,inP1,inP2,inL1,inL2,inD1,inD2,inS1,inS2;     // indexes in rhead
   double dt;
   RinexObsStream ifstr, ofstr;     // input and output Rinex files
   RinexObsHeader rhead, rheadout;  

      // open input file
   filename = C.InputObsName[nfile];
   ifstr.open(filename.c_str(),ios::in);
   if(ifstr.fail()) {
      C.oflog << "Failed to open input file " << filename << ". Abort.\n";
      return 1;
   }
   else C.oflog << "Opened input file " << filename << endl;
   ifstr.exceptions(ios::failbit);

      // open output file
   if(!C.OutRinexObs.empty()) {
      ofstr.open(C.OutRinexObs.c_str(), ios::out);
      if(ofstr.fail()) {
         C.oflog << "Failed to open output file " << C.OutRinexObs << " Abort.\n";
         ifstr.close();
         return 1;
      }
      else C.oflog << "Opened output file " << C.OutRinexObs << endl;
      ofstr.exceptions(ios::failbit);
      writeout = true;
   }
   else writeout = false;

      // read the header
   ifstr >> rhead;
   C.oflog << "Here is the input header for file " << filename << endl;
   rhead.dump(C.oflog);

      // check that file contains C1/P1,P2,L1,L2,D1,D2,S1,S2
   inC1 = inP1 = inP2 = inL1 = inL2 = inD1 = inD2 = inS1 = inS2 = -1;
   for(j=0; j<rhead.obsTypeList.size(); j++) {
      if(rhead.obsTypeList[j] == RinexObsHeader::convertObsType("C1")) inC1=j;
      if(rhead.obsTypeList[j] == RinexObsHeader::convertObsType("L1")) inL1=j;
      if(rhead.obsTypeList[j] == RinexObsHeader::convertObsType("L2")) inL2=j;
      if(rhead.obsTypeList[j] == RinexObsHeader::convertObsType("P1")) inP1=j;
      if(rhead.obsTypeList[j] == RinexObsHeader::convertObsType("P2")) inP2=j;
      if(rhead.obsTypeList[j] == RinexObsHeader::convertObsType("D1")) inD1=j;
      if(rhead.obsTypeList[j] == RinexObsHeader::convertObsType("D2")) inD2=j;
      if(rhead.obsTypeList[j] == RinexObsHeader::convertObsType("S1")) inS1=j;
      if(rhead.obsTypeList[j] == RinexObsHeader::convertObsType("S2")) inS2=j;
   }
   if((inP1==-1 && (!C.UseCA || inC1==-1)) || inP2==-1 || inL1==-1 || inL2==-1
         || inD1==-1 || inD2==-1 || inS1==-1 || inS2==-1) {
      C.oflog << "Warning: file " << filename << " does not contain";
      if(inC1==-1) C.oflog << " C1";
      if(inP1==-1) C.oflog << " P1 (--CA was" << (C.UseCA?"":" not") << " found)";
      if(inP2==-1) C.oflog << " P2";
      if(inL1==-1) C.oflog << " L1";
      if(inL2==-1) C.oflog << " L2";
      if(inD1==-1) C.oflog << " D1";
      if(inD2==-1) C.oflog << " D2";
      if(inS1==-1) C.oflog << " S1";
      if(inS2==-1) C.oflog << " S2";
      C.oflog << endl;
      //ifstr.clear();
      //ifstr.close();
      //return 2;
   }
   if(inP1==-1) {
      if(C.UseCA) inP1=inC1;
      else {
         C.oflog << "ERROR. Abort. Neither P1 data nor option --CA were not found."
            << endl;
         cerr << "ERROR. Abort. Neither P1 data nor option --CA were not found."
            << endl;
         return -1;
      }
   }
   if(inP1 > -1) {
      if(inP2 > -1) C.Freq = 3;
      else C.Freq = 1;
      // TD add 2 as an input option
   }
   C.oflog << "Process frequency " << C.Freq << endl;

      // initialize file solution and residual statistics
      SA[0].Reset(); SA[1].Reset(); SA[2].Reset();
      SR[0].Reset(); SR[1].Reset(); SR[2].Reset();
      PA = Matrix<double>(3,3,0.0);
      PR = Matrix<double>(3,3,0.0);
      zA = Vector<double>(3,0.0);
      zR = Vector<double>(3,0.0);
      if(C.knownpos.getCoordinateSystem() != Position::Unknown) {
         if(C.APSout) {
            SAPR[0].Reset(); SAPR[1].Reset(); SAPR[2].Reset();
            SANE[0].Reset(); SANE[1].Reset(); SANE[2].Reset();
            PAPR = Matrix<double>(3,3,0.0);
            PANE = Matrix<double>(3,3,0.0);
            zAPR = Vector<double>(3,0.0);
            zANE = Vector<double>(3,0.0);
         }
         SRPR[0].Reset(); SRPR[1].Reset(); SRPR[2].Reset();
         SRNE[0].Reset(); SRNE[1].Reset(); SRNE[2].Reset();
         PRPR = Matrix<double>(3,3,0.0);
         PRNE = Matrix<double>(3,3,0.0);
         zRPR = Vector<double>(3,0.0);
         zRNE = Vector<double>(3,0.0);
      }

      // loop over epochs in the file
   first = true;
   while(1) {
         // read next obs
      double RMSrof;
      vector<RinexPrn> Satellites;
      vector<double> Ranges;
      Matrix<double> inform;
      RinexObsData robsd,auxPosData;

      try {
         ifstr >> robsd;
      }
      catch(FFStreamError& e) {
         C.oflog << "Reading obs caught FFStreamError exception : " << e << endl;
         cerr << "Reading obs caught FFStreamError exception : " << e << endl;
         return -2;
      }
      catch(Exception& e) {
         C.oflog << "Reading obs caught GPSTk exception : " << e << endl;
         cerr << "Reading obs caught GPSTk exception : " << e << endl;
         return -2;
      }
      catch(exception& e) {
         C.oflog << "Reading obs caught std exception : " << e.what() << endl;
         cerr << "Reading obs caught std exception : " << e.what() << endl;
         return -2;
      }
      catch(...) {
         C.oflog << "Reading obs caught unknown exception : " << endl;
         cerr << "Reading obs caught unknown exception : " << endl;
         return -2;
      }

         // normal end-of-file
      if(!ifstr.good() || ifstr.eof()) { iret=0; break; }

      for(;;) {
         iret = 0;

         if(C.Debug) C.oflog << "process: " << robsd.time
            << ", Flag " << robsd.epochFlag << ", clk " << robsd.clockOffset << endl;

            // stay within time limits
         if(robsd.time < C.Tbeg) { iret = 1; break; }
         if(robsd.time > C.Tend) { iret = -1; break; }

            // ignore comment blocks ...
         if(robsd.epochFlag != 0 && robsd.epochFlag != 1) { iret = 1; break; }

            // decimate data
            // if Tbeg is still undefined, set it to begin of week
         if(C.ith > 0.0) {
            if(fabs(C.Tbeg-DayTime(DayTime::BEGINNING_OF_TIME)) < 1.e-8)
               C.Tbeg = C.Tbeg.setGPSfullweek(robsd.time.GPSfullweek(),0.0);
            double dt=fabs(robsd.time - C.Tbeg);
            dt -= C.ith*long(0.5+dt/C.ith);
            if(fabs(dt) > 0.25) { iret = 1; break; }
         }

            // save current time
         CurrEpoch = robsd.time;
         if(fabs(C.FirstEpoch-DayTime(DayTime::BEGINNING_OF_TIME)) < 1.e-8)
            C.FirstEpoch=CurrEpoch;

            // loop over satellites
         Nsvs = 0;
         Satellites.clear();
         Ranges.clear();
         RinexObsData::RinexPrnMap::const_iterator it;
         for(it=robsd.obs.begin(); it != robsd.obs.end(); ++it) {
            // loop over sat=it->first, ObsTypeMap=it->second
            int in,n;
            double P1=0,P2=0,L1,L2,D1,D2,S1,S2;
            RinexPrn sat=it->first;
            RinexObsData::RinexObsTypeMap otmap=it->second;

               // pull out the data
            RinexObsData::RinexObsTypeMap::const_iterator jt;
            if(inP1>-1 && (jt=otmap.find(rhead.obsTypeList[inP1])) != otmap.end())
               P1=jt->second.data;
            if(inP2>-1 && (jt=otmap.find(rhead.obsTypeList[inP2])) != otmap.end())
               P2=jt->second.data;
            if(inL1>-1 && (jt=otmap.find(rhead.obsTypeList[inL1])) != otmap.end())
               L1=jt->second.data;
            if(inL2>-1 && (jt=otmap.find(rhead.obsTypeList[inL2])) != otmap.end())
               L2=jt->second.data;
            if(inD1>-1 && (jt=otmap.find(rhead.obsTypeList[inD1])) != otmap.end())
               D1=jt->second.data;
            if(inD2>-1 && (jt=otmap.find(rhead.obsTypeList[inD2])) != otmap.end())
               D2=jt->second.data;
            if(inS1>-1 && (jt=otmap.find(rhead.obsTypeList[inS1])) != otmap.end())
               S1=jt->second.data;
            if(inS2>-1 && (jt=otmap.find(rhead.obsTypeList[inS2])) != otmap.end())
               S2=jt->second.data;
      
            // is the satellite excluded?
            for(i=0; i<C.ExSV.size(); i++)
               if(C.ExSV[i]==sat) continue;
      
            // NB 2 is not yet implemented
            if(C.Freq != 2 && P1<=0) continue;
            if(C.Freq != 1 && P2<=0) continue;

            // if position known and elevation limit given, apply elevation mask
            if(C.knownpos.getCoordinateSystem() != Position::Unknown
                  && C.elevLimit > 0.0) {
               bool ok=true;
               CorrectedEphemerisRange CER;
               try {
                  //double ER =
                  CER.ComputeAtReceiveTime(CurrEpoch, C.knownpos, sat.prn, *pEph);
                  if(CER.elevation < C.elevLimit) ok=false;
                  if(C.Debug) C.oflog << "Ephemeris range is "
                     << setprecision(4) << CER.rawrange << endl;
               }
               catch(EphemerisStore::NoEphemerisFound& nef) {
                  // do not exclude the sat here; PRSolution will...
                  if(C.Debug)
                     C.oflog << "CER did not find ephemeris for " << sat << endl;
               }

               if(!ok) continue;
            }

            // keep this satellite
            Satellites.push_back(sat);
            Ranges.push_back(C.Freq == 3 ? if1r*P1+if2r*P2 :
                            (C.Freq == 2 ? P2 : P1));
            Nsvs++;

         }  // end loop over sats

         if(Nsvs <= 4) {
            if(C.Debug) C.oflog << "Too few satellites" << endl;
            iret = 1;
            break;
         }

         iret = SolutionAlgorithm(Satellites, Ranges, RMSrof);
         if(C.Debug) C.oflog << "SolutionAlgorithm returns " << iret << endl;
         if(iret) break;

            // update LastEpoch and estimate of C.DT
         if(C.LastEpoch > DayTime(DayTime::BEGINNING_OF_TIME)) {
            dt = CurrEpoch-C.LastEpoch;
            for(i=0; i<9; i++) {
               if(C.ndt[i]<=0) { C.estdt[i]=dt; C.ndt[i]=1; break; }
               if(fabs(dt-C.estdt[i]) < 0.0001) { C.ndt[i]++; break; }
               if(i == 8) {
                  int k=0,nl=C.ndt[k];
                  for(j=1; j<9; j++) if(C.ndt[j] <= nl) { k=j; nl=C.ndt[j]; }
                  C.ndt[k]=1; C.estdt[k]=dt;
               }
            }
         }
         C.LastEpoch = CurrEpoch;

         break;
      }  // end for(;;)

      if(C.Debug) C.oflog << "processing returned " << iret << endl;
      if(iret == -1) { iret=0; break; }         // end of file
      if(iret == 1 || iret == -4) continue;     // ignore this epoch

         // accumulate simple statistics, Autonomous and RAIM
      if(C.APSout) {
         SA[0].Add(Solution(0)); SA[1].Add(Solution(1)); SA[2].Add(Solution(2));
         SSA[0].Add(Solution(0)); SSA[1].Add(Solution(1)); SSA[2].Add(Solution(2));
      }
      SR[0].Add(prsol.Solution(0));
      SR[1].Add(prsol.Solution(1));
      SR[2].Add(prsol.Solution(2));
      SSR[0].Add(prsol.Solution(0));
      SSR[1].Add(prsol.Solution(1));
      SSR[2].Add(prsol.Solution(2));

         // accumulate weighted statistics, Auto and RAIM
      if(C.APSout) {
         inform = inverseSVD(Matrix<double>(Covariance,0,0,3,3));
         PA += inform;
         PPA += inform;
         zA += inform * Vector<double>(Solution,0,3);
         zzA += inform * Vector<double>(Solution,0,3);
      }
      inform = inverseSVD(Matrix<double>(prsol.Covariance,0,0,3,3));
      PR += inform;
      PPR += inform;
      zR += inform * Vector<double>(prsol.Solution,0,3);
      zzR += inform * Vector<double>(prsol.Solution,0,3);

      if(!writeout) continue;

         // output to RINEX
      if(first) {                               // edit the output Rinex header
         rheadout = rhead;
         rheadout.date = PrgmEpoch.printf("%04Y/%02m/%02d %02H:%02M:%02S");
         rheadout.fileProgram = PrgmName;
         if(!C.HDRunby.empty()) rheadout.fileAgency = C.HDRunby;
         if(!C.HDObs.empty()) rheadout.observer = C.HDObs;
         if(!C.HDAgency.empty()) rheadout.agency = C.HDAgency;
         if(!C.HDMarker.empty()) rheadout.markerName = C.HDMarker;
         if(!C.HDNumber.empty()) {
            rheadout.markerNumber = C.HDNumber;
            rheadout.valid |= RinexObsHeader::markerNumberValid;
         }
         rheadout.version = 2.1; rheadout.valid |= RinexObsHeader::versionValid;
         rheadout.firstObs = C.FirstEpoch;
         rheadout.valid |= RinexObsHeader::firstTimeValid;
         //rheadout.interval = C.DT;
         //rheadout.valid |= RinexObsHeader::intervalValid;
         //rheadout.lastObs = C.LastEpoch;
         //rheadout.valid |= RinexObsHeader::lastTimeValid;
            // invalidate the table
         if(rheadout.valid & RinexObsHeader::numSatsValid)
            rheadout.valid ^= RinexObsHeader::numSatsValid;
         if(rheadout.valid & RinexObsHeader::prnObsValid)
            rheadout.valid ^= RinexObsHeader::prnObsValid;

         ofstr << rheadout;
         first=false;
      }
      if(iret > 2) {                         // output position first
         auxPosData.time = robsd.time;
         auxPosData.epochFlag = 4;
         auxPosData.numSvs = 2;
         auxPosData.auxHeader.clear();
         ostringstream stst1,stst2;
         stst1 << "XYZT";
         stst1 << fixed << " " << setw(13) << setprecision(3) << prsol.Solution(0);
         stst1 << fixed << " " << setw(13) << setprecision(3) << prsol.Solution(1);
         stst1 << fixed << " " << setw(13) << setprecision(3) << prsol.Solution(2);
         stst1 << fixed << " " << setw(13) << setprecision(3) << prsol.Solution(3);
         auxPosData.auxHeader.commentList.push_back(stst1.str());
         double PDOP=RSS(prsol.Covariance(0,0),
                         prsol.Covariance(1,1),
                         prsol.Covariance(2,2));
         stst2 << "DIAG";
         stst2 << " " << setw(2) << Nsvs
            << " " << fixed << setw(5) << setprecision(2) << PDOP
            << " " << fixed << setw(5) << setprecision(2)
            << RSS(PDOP,Covariance(3,3))
            << " " << fixed << setw(9) << setprecision(3) << RMSrof;
         stst2 << " (N,P-,G-DOP,RMS)";
         auxPosData.auxHeader.commentList.push_back(stst2.str());
         auxPosData.auxHeader.valid |= RinexObsHeader::commentValid;
         ofstr << auxPosData;
      }
      ofstr << robsd;                       // output data to RINEX file

   }  // end while loop over epochs

   if(C.APSout) PrintStats(SA,PA,zA,"Autonomous solution for file " + filename);
   PrintStats(SR,PR,zR,"RAIM solution for file " + filename);
   if(C.knownpos.getCoordinateSystem() != Position::Unknown) {
      if(C.APSout) {
         PrintStats(SAPR,PAPR,zAPR,
            "Autonomous solution residuals for file " + filename);
         PrintStats(SANE,PANE,zANE,
            "Autonomous solution residuals (NEU) for file " + filename,'N','E','U');
      }
      PrintStats(SRPR,PRPR,zRPR,"RAIM solution residuals for file " + filename);
      PrintStats(SRNE,PRNE,zRNE,
         "RAIM solution residuals (NEU) for file " + filename,'N','E','U');
   }

   ifstr.clear();
   ifstr.close();
   ofstr.close();

   return iret;
}
catch(gpstk::Exception& e) {
      GPSTK_RETHROW(e);
}
catch (...) {
      cerr << "PRSolve:ReadFile caught an unknown exception\n";
}
   return -1;
}

//------------------------------------------------------------------------------------
// Return 0 ok,
//      <-1 fatal error: -4 no ephemeris
//       -1 end of file,
//        1 skip this epoch,
//        2 output to Rinex,
//        3 output position also
int SolutionAlgorithm(vector<RinexPrn>& Sats,
                      vector<double>& PRanges,
                      double& RMSresid)
{
try {
   int iret,i;
   Matrix<double> inform;

   // fail if not enough data
   if(Nsvs < 4) return 1;

   // compute a position solution with this data
   if(C.Debug) {
      C.oflog << "Satellites and Ranges before Prepare:\n";
      for(i=0; i<PRanges.size(); i++)
         C.oflog << " " << setw(2) << Sats[i] << fixed
            << " " << setw(13) << setprecision(3) << PRanges[i] << endl;
   }

   int niter=C.nIter;
   double conv=C.convLimit;
   vector<bool> UseSats(Sats.size(),true);
   Vector<double> Residual,Slope;

   // configuration
   if(C.Verbose) {
      prsol.pDebugStream = &C.oflog;
      prsol.Debug = true;
   }
   if(C.rmsLimit > 0.0)
      prsol.RMSLimit = C.rmsLimit;
   if(C.slopeLimit > 0.0)
      prsol.SlopeLimit = C.slopeLimit;
   prsol.Algebraic = C.algebra;
   prsol.ResidualCriterion = C.residCrit;
   prsol.ReturnAtOnce = C.returnatonce;
   if(C.maxReject > 0)
      prsol.NSatsReject = C.maxReject;
   if(C.nIter > 0)
      prsol.MaxNIterations = C.nIter;
   else
      niter = prsol.MaxNIterations;
   if(C.convLimit > 0.0)
      prsol.ConvergenceLimit = C.convLimit;
   else
      conv = prsol.ConvergenceLimit;
   //C.oflog << "NSatsReject is " << prsol.NSatsReject << endl;

   // compute using AutonomousSolution - no RAIM algorithm
   if(C.APSout) {
      iret = -4;
      Matrix<double> SVP;
      iret = PRSolution::PrepareAutonomousSolution(CurrEpoch,Sats,PRanges,*pEph,SVP);
      if(iret == -4) {
         C.oflog << "PrepareAutonomousSolution failed to find ANY ephemeris at epoch "
            << CurrEpoch.printf("%04Y/%02m/%02d %02H:%02M:%.3f") << endl;
         return iret;
      }

      if(C.Debug) {
         C.oflog << "Satellites after  Prepare(" << iret << "):";
         for(i=0; i<Sats.size(); i++)
            C.oflog << " " << setw(2) << Sats[i].prn; C.oflog << endl;
         C.oflog << "Matrix SVP(" << SVP.rows() << "," << SVP.cols() << "):\n"
            << fixed << setw(13) << setprecision(3) << SVP << endl;
      }

      for(i=0; i<Sats.size(); i++) UseSats[i] = (Sats[i].prn > 0 ? true : false);

      iret = PRSolution::AutonomousPRSolution(CurrEpoch, UseSats, SVP, C.pTropModel,
         C.algebra, niter, conv, Solution, Covariance, Residual, Slope,
         (C.Verbose ? &C.oflog : NULL));

      C.oflog << "APS " << setw(2) << iret
         << " " << CurrEpoch.printf(C.timeFormat)
         << " " << setw(2) << Nsvs;
      if(iret == 0) C.oflog << fixed
         << " " << setw(16) << setprecision(6) << Solution(0)
         << " " << setw(16) << setprecision(6) << Solution(1)
         << " " << setw(16) << setprecision(6) << Solution(2)
         << " " << setw(14) << setprecision(6) << Solution(3)
         << " " << setw(12) << setprecision(6) << RMS(Residual)
         << " " << fixed << setw(5) << setprecision(1) << max(Slope);
      C.oflog << " " << niter
            << " " << scientific << setw(8) << setprecision(2) << conv;
      for(i=0; i<Sats.size(); i++) C.oflog << " " << setw(3) << Sats[i].prn;
      C.oflog << endl;

      // compute residuals using known position, and output
      if(iret==0 && C.knownpos.getCoordinateSystem() != Position::Unknown) {
         Matrix<double> Cov;
         Vector<double> V(3);

         // compute residuals
         Position pos(Solution(0), Solution(1), Solution(2));
         Position res=pos-C.knownpos;
         Cov = Matrix<double>(Covariance,0,0,3,3);
         V(0) = res.X(); V(1) = res.Y(); V(2) = res.Z();

         C.oflog << "APR " << setw(2) << iret
            << " " << CurrEpoch.printf(C.timeFormat)
            << " " << setw(2) << Nsvs << fixed
            << " " << setw(16) << setprecision(6) << V(0)
            << " " << setw(16) << setprecision(6) << V(1)
            << " " << setw(16) << setprecision(6) << V(2)
            << " " << setw(14) << setprecision(6) << Solution(3)
            << " " << setw(12) << setprecision(6) << RMS(Residual)
            << " " << fixed << setw(5) << setprecision(1) << max(Slope)
            << " " << niter
            << " " << scientific << setw(8) << setprecision(2) << conv;
         for(i=0; i<Sats.size(); i++) C.oflog << " " << setw(3) << Sats[i].prn;
         C.oflog << endl;

         // accumulate statistis
         SAPR[0].Add(V(0)); SAPR[1].Add(V(1)); SAPR[2].Add(V(2));
         SSAPR[0].Add(V(0)); SSAPR[1].Add(V(1)); SSAPR[2].Add(V(2));
         inform = inverseSVD(Cov);
         PAPR += inform;
         PPAPR += inform;
         zAPR += inform * V;
         zzAPR += inform * V;

         // convert to NEU
         V = C.Rot * V;
         Cov = C.Rot * Cov * transpose(C.Rot);

         C.oflog << "ANE " << setw(2) << iret
            << " " << CurrEpoch.printf(C.timeFormat)
            << " " << setw(2) << Nsvs << fixed
            << " " << setw(16) << setprecision(6) << V(0)
            << " " << setw(16) << setprecision(6) << V(1)
            << " " << setw(16) << setprecision(6) << V(2)
            << " " << setw(14) << setprecision(6) << Solution(3)
            << " " << setw(12) << setprecision(6) << RMS(Residual)
            << " " << fixed << setw(5) << setprecision(1) << max(Slope)
            << " " << niter
            << " " << scientific << setw(8) << setprecision(2) << conv;
         for(i=0; i<Sats.size(); i++) C.oflog << " " << setw(3) << Sats[i].prn;
         C.oflog << endl;

         // accumulate statistis
         SANE[0].Add(V(0)); SANE[1].Add(V(1)); SANE[2].Add(V(2));
         SSANE[0].Add(V(0)); SSANE[1].Add(V(1)); SSANE[2].Add(V(2));
         inform = inverseSVD(Cov);
         PANE += inform;
         PPANE += inform;
         zANE += inform * V;
         zzANE += inform * V;

      }  // end output residuals

   }  // end output APS

   // --------------------------------------------------------------
   // now compute again, using RAIM

   iret = -4;
   iret = prsol.RAIMCompute(CurrEpoch, Sats, PRanges, *pEph, C.pTropModel);
   if(iret == -4) {
      C.oflog << "PRS Sol. failed to find ephemeris\n" << endl;
      return iret;
   }
   for(Nsvs=0,i=0; i<Sats.size(); i++)
      if(Sats[i].prn > 0)
         Nsvs++;
   RMSresid = prsol.RMSResidual;

   C.oflog << "RPF " << setw(2) << Sats.size()-Nsvs
      << " " << CurrEpoch.printf(C.timeFormat)
      << " " << setw(2) << Nsvs << fixed
      << " " << setw(16) << setprecision(6) << prsol.Solution(0)
      << " " << setw(16) << setprecision(6) << prsol.Solution(1)
      << " " << setw(16) << setprecision(6) << prsol.Solution(2)
      << " " << setw(14) << setprecision(6) << prsol.Solution(3)
      << " " << setw(12) << setprecision(6) << prsol.RMSResidual
      << " " << fixed << setw(5) << setprecision(1) << prsol.MaxSlope
      << " " << prsol.NIterations
      << " " << scientific << setw(8) << setprecision(2) << prsol.Convergence;
   for(i=0; i<Sats.size(); i++) C.oflog << " " << setw(3) << Sats[i].prn;
   C.oflog << " (" << iret << ")" << (prsol.isValid() ? " V" : " NV")
      << endl;
   //C.oflog << "prsol Sol. returned " << iret << " at " << CurrEpoch << endl;

   // compute residuals using known position, and output
   if(C.knownpos.getCoordinateSystem() != Position::Unknown && iret >= 0) {
      Matrix<double> Cov;
      Vector<double> V(3);

      // compute residuals
      Position pos(prsol.Solution(0), prsol.Solution(1), prsol.Solution(2));
      Position res=pos-C.knownpos;
      Cov = Matrix<double>(prsol.Covariance,0,0,3,3);
      V(0) = res.X(); V(1) = res.Y(); V(2) = res.Z();

      C.oflog << "RPR " << setw(2) << Sats.size()-Nsvs
         << " " << CurrEpoch.printf(C.timeFormat)
         << " " << setw(2) << Nsvs << fixed
         << " " << setw(16) << setprecision(6) << V(0)
         << " " << setw(16) << setprecision(6) << V(1)
         << " " << setw(16) << setprecision(6) << V(2)
         << " " << setw(14) << setprecision(6) << prsol.Solution(3)
         << " " << setw(12) << setprecision(6) << prsol.RMSResidual
         << " " << fixed << setw(5) << setprecision(1) << prsol.MaxSlope
         << " " << prsol.NIterations
         << " " << scientific << setw(8) << setprecision(2) << prsol.Convergence;
      for(i=0; i<Sats.size(); i++) C.oflog << " " << setw(3) << Sats[i].prn;
      C.oflog << " (" << iret << ")" << (prsol.isValid() ? " V" : " NV")
         << endl;

      // accumulate statistics
      SRPR[0].Add(V(0)); SRPR[1].Add(V(1)); SRPR[2].Add(V(2));
      SSRPR[0].Add(V(0)); SSRPR[1].Add(V(1)); SSRPR[2].Add(V(2));
      inform = inverseSVD(Cov);
      PRPR += inform;
      PPRPR += inform;
      zRPR += inform * V;
      zzRPR += inform * V;

      // convert to NEU
      V = C.Rot * V;
      Cov = C.Rot * Cov * transpose(C.Rot);

      C.oflog << "RNE " << setw(2) << Sats.size()-Nsvs
         << " " << CurrEpoch.printf(C.timeFormat)
         << " " << setw(2) << Nsvs << fixed
         << " " << setw(16) << setprecision(6) << V(0)
         << " " << setw(16) << setprecision(6) << V(1)
         << " " << setw(16) << setprecision(6) << V(2)
         << " " << setw(14) << setprecision(6) << prsol.Solution(3)
         << " " << setw(12) << setprecision(6) << prsol.RMSResidual
         << " " << fixed << setw(5) << setprecision(1) << prsol.MaxSlope
         << " " << prsol.NIterations
         << " " << scientific << setw(8) << setprecision(2) << prsol.Convergence;
      for(i=0; i<Sats.size(); i++) C.oflog << " " << setw(3) << Sats[i].prn;
      C.oflog << " (" << iret << ")" << (prsol.isValid() ? " V" : " NV")
         << endl;

      // accumulate statistics
      if(iret == 0) {
         SRNE[0].Add(V(0)); SRNE[1].Add(V(1)); SRNE[2].Add(V(2));
         SSRNE[0].Add(V(0)); SSRNE[1].Add(V(1)); SSRNE[2].Add(V(2));
         inform = inverseSVD(Cov);
         PRNE += inform;
         PPRNE += inform;
         zRNE += inform * V;
         zzRNE += inform * V;
      }
   }
   //C.oflog << endl;

   //
   if(prsol.isValid() && !C.OutRinexObs.empty()) return 3;
   if(!prsol.isValid()) return 1;

   if(!C.OutRinexObs.empty()) return 2;
   return 0;
}
catch(gpstk::Exception& e) {
   GPSTK_RETHROW(e);
}
catch (...) {
   cerr << "PRSolve:SolutionAlgorithm caught an unknown exception\n";
}
   return -1;
}

//------------------------------------------------------------------------------------
int AfterReadingFiles(void)
{
try {
   if(C.APSout) {
      PrintStats(SSA,PPA,zzA,"Autonomous solution for all files");
      if(C.knownpos.getCoordinateSystem() != Position::Unknown) {
         PrintStats(SSAPR,PPAPR,zzAPR,"Autonomous position residuals for all files");
         PrintStats(SSANE,PPANE,zzANE,
            "Autonomous position residuals (NEU) for all files",'N','E','U');
      }
   }

   PrintStats(SSR,PPR,zzR,"RAIM solution for all files");
   if(C.knownpos.getCoordinateSystem() != Position::Unknown) {
      PrintStats(SSRPR,PPRPR,zzRPR,"RAIM position residuals for all files");
      PrintStats(SSRNE,PPRNE,zzRNE,"RAIM position residuals (NEU) for all files",
         'N','E','U');
   }

   cout << "Weighted average RAIM solution for all files" << endl << fixed;
   if(SSR[0].N() > 0) {
      Matrix<double> Cov=inverse(PPR);
      Vector<double> Sol=Cov * zzR;
      cout << setw(16) << setprecision(6) << Sol << endl;
      cout << "Covariance of RAIM solution for all files" << endl;
      cout << setw(16) << setprecision(6) << Cov << endl;
   }
   else cout << " No data!" << endl;

      // compute data interval for this file
   int i,j;
   double dt;
   for(j=0,i=1; i<9; i++) { if(C.ndt[i]>C.ndt[j]) j=i; }
   //C.DT = C.estdt[j];
   C.oflog << endl;
   C.oflog << "Estimated data interval is " << C.estdt[j] << " seconds.\n";
   C.oflog << "First epoch is "
      << C.FirstEpoch.printf("%04Y/%02m/%02d %02H:%02M:%.3f = %04F %10.3g") << endl;
   C.oflog << "Last epoch is "
      << C.LastEpoch.printf("%04Y/%02m/%02d %02H:%02M:%.3f = %04F %10.3g") << endl;

   return 0;
}
catch(gpstk::Exception& e) {
      GPSTK_RETHROW(e);
}
catch (...) {
      cerr << "PRSolve:AfterReadingFiles caught an unknown exception\n";
}
   return -1;
}

//------------------------------------------------------------------------------------
void PrintStats(Stats<double> S[3], Matrix<double> &P, Vector<double> &z, string msg,
   char c0, char c1, char c2)
{
   C.oflog << endl;
   C.oflog << "Simple statistics on " << msg << endl << fixed;
   C.oflog << c0 << " : " << setw(16) << setprecision(6) << S[0] << endl;
   C.oflog << c1 << " : " << setw(16) << setprecision(6) << S[1] << endl;
   C.oflog << c2 << " : " << setw(16) << setprecision(6) << S[2] << endl;

   //C.oflog << endl;
   C.oflog << "Weighted average " << msg << endl << fixed;
   if(S[0].N() > 0) {
      Matrix<double> Cov=inverse(P);
      Vector<double> Sol=Cov * z;
      C.oflog << setw(16) << setprecision(6) << Sol << "    " << S[0].N() << endl;
      C.oflog << "Covariance of " << msg << endl;
      C.oflog << setw(16) << setprecision(6) << Cov << endl;
   }
   else C.oflog << " No data!" << endl;
}

//------------------------------------------------------------------------------------
int GetCommandLine(int argc, char **argv)
{
   bool help=false;
   int i,j;
try {
      // defaults
   C.Debug = C.Verbose = false;
   C.ith = 0.0;
   C.Tbeg = C.FirstEpoch = DayTime(DayTime::BEGINNING_OF_TIME);
   C.Tend = DayTime(DayTime::END_OF_TIME);
   //C.DT = 0;

   C.rmsLimit = -1.0;            // PRSolution() has a default
   C.slopeLimit = -1.0;          // PRSolution() has a default
   C.algebra = false;
   C.residCrit = true;
   C.returnatonce = false;
   C.maxReject = -1;             // PRSolution() has a default
   C.nIter = -1;                 // PRSolution() has a default
   C.convLimit = -1.0;           // PRSolution() has a default
   C.elevLimit = 0.0;

   C.LogFile = string("prs.log");

   C.APSout = false;
   C.UseCA = false;
   C.DataInt = -1.0;
   C.TropType = string("BL");
   C.T = 20.0;
   C.Pr = 980.0;
   C.RH = 50.0;
   
   C.HDPrgm = PrgmName + string(" v.") + PrgmVers.substr(0,4);
   C.HDRunby = string("ARL:UT/SGL/GPSTK");

   C.timeFormat = string("%4F %10.3g");

   for(i=0; i<9; i++) C.ndt[i]=-1;

   C.ObsDirectory = string("");
   C.NavDirectory = string("");

      // -------------------------------------------------
      // -------------------------------------------------
      // required options
   RequiredOption dashi(CommandOption::hasArgument, CommandOption::stdType,
      'o',"obs"," [-o|--obs]<file>     Input Rinex observation file(s)");

   RequiredOption dashn(CommandOption::hasArgument, CommandOption::stdType,'n',"nav",
      " [-n|--nav]<file>     Input navigation (ephemeris) file(s) (Rinex or SP3)");

      // optional options
   // this only so it will show up in help page...
   CommandOption dashf(CommandOption::hasArgument, CommandOption::stdType,
      'f',"","# Input:\n -f<file>             File containing more options");

   CommandOption dashdo(CommandOption::hasArgument, CommandOption::stdType,
      0,"obsdir"," --obsdir <dir>       Directory of input observation file(s)");
   dashdo.setMaxCount(1);

   CommandOption dashdn(CommandOption::hasArgument, CommandOption::stdType,
      0,"navdir"," --navdir <dir>       Directory of input navigation file(s)");
   dashdn.setMaxCount(1);

   CommandOption dashith(CommandOption::hasArgument, CommandOption::stdType,
      0,"decimate"," --decimate <dt>      Decimate data to time interval dt");
   dashith.setMaxCount(1);

   // time
   CommandOptionWithTimeArg dasheb(0,"EpochBeg","%Y,%m,%d,%H,%M,%f",
      " --EpochBeg <arg>     Start time, arg is of the form YYYY,MM,DD,HH,Min,Sec");
   CommandOptionWithTimeArg dashgb(0,"GPSBeg","%F,%g",
      " --GPSBeg <arg>       Start time, arg is of the form GPSweek,GPSsow");

   CommandOptionWithTimeArg dashee(0,"EpochEnd","%Y,%m,%d,%H,%M,%f",
      " --EpochEnd <arg>     End time, arg is of the form YYYY,MM,DD,HH,Min,Sec");
   CommandOptionWithTimeArg dashge(0,"GPSEnd","%F,%g",
      " --GPSEnd <arg>       End time, arg is of the form GPSweek,GPSsow");

   // allow ONLY one start time (use startmutex(true) if one is required)
   CommandOptionMutex startmutex(false);
   startmutex.addOption(&dasheb);
   startmutex.addOption(&dashgb);
   CommandOptionMutex stopmutex(false);
   stopmutex.addOption(&dashee);
   stopmutex.addOption(&dashge);

   CommandOptionNoArg dashCA(0,"CA",
      " --CA                 Use C/A code pseudorange if P1 is not available");
   dashCA.setMaxCount(1);
   
   //CommandOption dashDT(CommandOption::hasArgument, CommandOption::stdType,
      //0,"DT"," --DT <dt>              Time interval (sec) of data points");
   //dashDT.setMaxCount(1);
   
   // --------------------------------------------------------------------------------

   CommandOption dashrms(CommandOption::hasArgument, CommandOption::stdType,
      0,"RMSlimit", "# Configuration:\n --RMSlimit <rms>     "
      "Upper limit on RMS post-fit residuals (m) for a good solution");
   dashrms.setMaxCount(1);

   CommandOption dashslop(CommandOption::hasArgument, CommandOption::stdType,
      0,"SlopeLimit",
      " --SlopeLimit <s>     Upper limit on RAIM 'slope' for a good solution");
   dashslop.setMaxCount(1);

   CommandOptionNoArg dashAlge(0,"Algebra",
      " --Algebra            Use algebraic algorithm (otherwise linearized LS)");
   dashAlge.setMaxCount(1);

   CommandOptionNoArg dashrcrt(0,"DistanceCriterion", " --DistanceCriterion  "
      "Use distance from a priori as convergence criterion (else RMS)");
   dashrcrt.setMaxCount(1);

   CommandOptionNoArg dashrone(0,"ReturnAtOnce"," --ReturnAtOnce       "
      "Return as soon as a good solution is found");
   dashrone.setMaxCount(1);

   CommandOption dashnrej(CommandOption::hasArgument, CommandOption::stdType,
      0,"NReject", " --NReject <n>        Maximum number of satellites to reject");
   dashnrej.setMaxCount(1);

   CommandOption dashNit(CommandOption::hasArgument, CommandOption::stdType,0,"NIter",
      " --NIter <n>          Maximum iteration count (linearized LS algorithm)");
   dashNit.setMaxCount(1);

   CommandOption dashConv(CommandOption::hasArgument, CommandOption::stdType,0,"Conv",
      " --Conv <c>           Minimum convergence criterion (m) (LLS algorithm)");
   dashConv.setMaxCount(1);

   CommandOption dashElev(CommandOption::hasArgument, CommandOption::stdType,
      0,"MinElev",
      " --MinElev <el>       Minimum elevation angle (deg) (only if --PosXYZ)");
   dashElev.setMaxCount(1);

   CommandOption dashXprn(CommandOption::hasArgument, CommandOption::stdType,
      0,"XPRN"," --XPRN <prn>         Exclude this satellite.");

   CommandOption dashTrop(CommandOption::hasArgument, CommandOption::stdType,
      0,"Trop"," --Trop <model,T,P,H> Trop model (one of BL,SA,NB,GG,GGH (cf.GPSTk)),"
      "\n                         with OPTIONAL weather Temp(C),Press(mb),RH(%)");
   dashTrop.setMaxCount(1);

   // --------------------------------------------------------------------------------

   CommandOption dashLog(CommandOption::hasArgument, CommandOption::stdType,
      0,"Log","# Output:\n --Log <file>         Output log file name (prs.log).");
   dashLog.setMaxCount(1);
   
   CommandOption dashXYZ(CommandOption::hasArgument, CommandOption::stdType,
      0,"PosXYZ", " --PosXYZ <X,Y,Z>     "
      "Known position (ECEF,m), used to compute output residuals.");
   dashXYZ.setMaxCount(1);
   
   CommandOptionNoArg dashAPSout(0,"APSout",
      " --APSout             Output autonomous pseudorange solution (APS - no RAIM)");
   dashAPSout.setMaxCount(1);

   CommandOption dashForm(CommandOption::hasArgument, CommandOption::stdType,
      0,"TimeFormat", " --TimeFormat <fmt> "
      "Output time format (ala DayTime) (default: " + C.timeFormat + ")");
   dashForm.setMaxCount(1);

   CommandOption dashRfile(CommandOption::hasArgument, CommandOption::stdType,
      0,"RinexFile","# Rinex output:\n"
      " --RinexFile <file>   Output Rinex obs file name");
   dashRfile.setMaxCount(1);
   
   CommandOption dashRrun(CommandOption::hasArgument, CommandOption::stdType,
      0,"RunBy"," --RunBy <string>     Output Rinex header 'RUN BY' string");
   dashRrun.setMaxCount(1);
   
   CommandOption dashRobs(CommandOption::hasArgument, CommandOption::stdType,
      0,"Observer"," --Observer <string>  Output Rinex header 'OBSERVER' string");
   dashRobs.setMaxCount(1);
   
   CommandOption dashRag(CommandOption::hasArgument, CommandOption::stdType,
      0,"Agency"," --Agency <string>    Output Rinex header 'AGENCY' string");
   dashRag.setMaxCount(1);
   
   CommandOption dashRmark(CommandOption::hasArgument, CommandOption::stdType,
      0,"Marker"," --Marker <string>    Output Rinex header 'MARKER' string");
   dashRmark.setMaxCount(1);
   
   CommandOption dashRnumb(CommandOption::hasArgument, CommandOption::stdType,
      0,"Number"," --Number <string>    Output Rinex header 'NUMBER' string");
   dashRnumb.setMaxCount(1);
   
   CommandOptionNoArg dashVerb(0,"verbose",
      "# Help:\n --verbose            Print extended output");
   dashVerb.setMaxCount(1);

   CommandOptionNoArg dashDebug(0,"debug",
      " --debug              Print very extended output.");
   dashDebug.setMaxCount(1);

   CommandOptionNoArg dashh('h', "help",
     " [-h|--help]          Print syntax and quit.");

   // ... other options
   CommandOptionRest Rest("");

   CommandOptionParser Par(
   "Prgm PRSolve reads one or more Rinex observation files, plus one or more\n"
   "   navigation (ephemeris) files, and computes an autonomous pseudorange\n"
   "   position solution, using a RAIM-like algorithm to eliminate outliers.\n"
   "   Output is to the log file, and also optionally to a Rinex obs file with\n"
   "   the position solutions in auxiliary header blocks.\n");

      // -------------------------------------------------
      // allow user to put all options in a file
      // could also scan for debug here
   vector<string> Args;
   for(j=1; j<argc; j++) PreProcessArgs(argv[j],Args,C.Verbose);

   argc = Args.size();
   if(argc==0) Args.push_back(string("-h"));
//cout << "List after PreProcessArgs\n";
//for(i=0; i<argc; i++) cout << i << " " << Args[i] << endl;

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
//cout << "List passed to parser\n";
//for(i=0; i<argc; i++) cout << i << " " << CArgs[i] << endl;
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
   vector<string> values;
      // f never appears because we intercept it above
   //if(dashf.getCount()) { cout << "Option f "; dashf.dumpValue(cout); }
      // do help first
   if(dashh.getCount()) help=true;
   if(dashDebug.getCount()) C.Debug=C.Verbose=true;
   if(dashVerb.getCount()) C.Verbose=true;

   if(dashdo.getCount()) {
      values = dashdo.getValue();
      C.ObsDirectory = values[0];
      if(help) cout << "Input obs directory is " << C.ObsDirectory << endl;
   }
   if(dashdn.getCount()) {
      values = dashdn.getValue();
      C.NavDirectory = values[0];
      if(help) cout << "Input nav directory is " << C.NavDirectory << endl;
   }
   if(dashi.getCount()) {
      values = dashi.getValue();
      if(help) cout << "Input Rinex obs files are:\n";
      for(i=0; i<values.size(); i++) {
         if(!C.ObsDirectory.empty())
            C.InputObsName.push_back(C.ObsDirectory + string("/") + values[i]);
         else
            C.InputObsName.push_back(values[i]);
         if(help) cout << "   " << C.ObsDirectory + string("/") + values[i] << endl;
      }
   }
   if(dashn.getCount()) {
      values = dashn.getValue();
      if(help) cout << "Input Rinex nav files are:\n";
      for(i=0; i<values.size(); i++) {
         if(!C.NavDirectory.empty())
            C.InputNavName.push_back(C.NavDirectory + string("/") + values[i]);
         else
            C.InputNavName.push_back(values[i]);
         if(help) cout << "  " << C.NavDirectory + string("/") + values[i] << endl;
      }
   }

   if(dashith.getCount()) {
      values = dashith.getValue();
      C.ith = StringUtils::asDouble(values[0]);
      if(help) cout << "Ithing values is " << C.ith << endl;
   }
   if(dasheb.getCount()) {
      values = dasheb.getValue();
      C.Tbeg.setToString(values[0], "%Y,%m,%d,%H,%M,%S");
      if(help) cout << "Begin time is "
         << C.Tbeg.printf("%04Y/%02m/%02d %02H:%02M:%.3f") << endl;
   }
   if(dashgb.getCount()) {
      values = dashgb.getValue();
      C.Tbeg.setToString(values[0], "%F,%g");
      if(help) cout << "Begin time is " << C.Tbeg.printf("%04F/%10.3g") << endl;
   }
   if(dashee.getCount()) {
      values = dashee.getValue();
      C.Tend.setToString(values[0], "%Y,%m,%d,%H,%M,%S");
      if(help) cout << "End time is "
         << C.Tend.printf("%04Y/%02m/%02d %02H:%02M:%.3f") << endl;
   }
   if(dashge.getCount()) {
      values = dashge.getValue();
      C.Tend.setToString(values[0], "%F,%g");
      if(help) cout << "End time is " << C.Tend.printf("%04F/%10.3g") << endl;
   }
   if(dashCA.getCount()) {
      C.UseCA = true;
      if(help) cout << "'Use C/A' flag is set\n";
   }
   //if(dashDT.getCount()) {
      //values = dashDT.getValue();
      //C.DT = StringUtils::asDouble(values[0]);
      //if(help) cout << "DT is set to " << C.DT << endl;
   //}

   if(dashrms.getCount()) {
      values = dashrms.getValue();
      C.rmsLimit = StringUtils::asDouble(values[0]);
      if(help) cout << "RMS limit is set to " << C.rmsLimit << endl;
   }
   if(dashslop.getCount()) {
      values = dashslop.getValue();
      C.slopeLimit = StringUtils::asDouble(values[0]);
      if(help) cout << "Slope limit is set to " << C.slopeLimit << endl;
   }
   if(dashAlge.getCount()) {
      C.algebra = true;
      if(help) cout << "'Algebraic' option is on\n";
   }
   if(dashrcrt.getCount()) {
      C.residCrit = false;
      if(help) cout << "'ResidualCriterion' option is false\n";
   }
   if(dashrone.getCount()) {
      C.returnatonce = true;
      if(help) cout << "'Return at once' option is true\n";
   }
   if(dashnrej.getCount()) {
      values = dashnrej.getValue();
      C.maxReject = StringUtils::asInt(values[0]);
      if(help) cout << "Max N rejected satellites is set to " << C.maxReject << endl;
   }
   if(dashNit.getCount()) {
      values = dashNit.getValue();
      C.nIter = StringUtils::asInt(values[0]);
      if(help) cout << "Max N Iterations is set to " << C.nIter << endl;
   }
   if(dashElev.getCount()) {
      values = dashElev.getValue();
      C.elevLimit = StringUtils::asDouble(values[0]);
      if(help) cout << "Elevation limit is set to " << C.convLimit << " deg" << endl;
   }
   if(dashConv.getCount()) {
      values = dashConv.getValue();
      C.convLimit = StringUtils::asDouble(values[0]);
      if(help) cout << "Convergence limit is set to " << C.convLimit << endl;
   }

   if(dashXYZ.getCount()) {
      values = dashXYZ.getValue();
      vector<string> field;
      for(i=0; i<values.size(); i++) {
         field.clear();
         while(values[i].size() > 0)
            field.push_back(StringUtils::stripFirstWord(values[i],','));
         if(field.size() < 3) {
            C.oflog << "Error: less than four fields in --PosXYZ input: "
               << values[i] << endl;
            cerr << "Error: less than four fields in --PosXYZ input: "
               << values[i] << endl;
            continue;
         }
         Position p(StringUtils::asDouble(field[0]),
                    StringUtils::asDouble(field[1]),
                    StringUtils::asDouble(field[2]));
         C.knownpos = p;
         if(help) cout << " Input: known XYZ position "
            << field[0] << " " << field[1] << " " << field[2] << endl;
      }
   }
   if(dashAPSout.getCount()) C.APSout=true;
   if(dashForm.getCount()) {
      values = dashForm.getValue();
      C.timeFormat = values[0];
      if(help) cout << " Input: time format " << C.timeFormat << endl;
   }
   if(dashXprn.getCount()) {
      values = dashXprn.getValue();
      for(i=0; i<values.size(); i++) {
         RinexPrn p=StringUtils::asData<RinexPrn>(values[i]);
         if(help) cout << "Exclude satellite " << p << endl;
         C.ExSV.push_back(p);
      }
   }
   if(dashTrop.getCount()) {
      values = dashTrop.getValue();
      vector<string> field;
      while(values[0].size() > 0)
         field.push_back(StringUtils::stripFirstWord(values[0],','));
      if(field.size() != 1 && field.size() != 4) {
         C.oflog << "Error: invalid fields after --Trop input: "
            << values[0] << endl;
         cerr << "Error: invalid fields after --Trop input: "
            << values[0] << endl;
      }
      else {
         field[0] = StringUtils::upperCase(field[0]);
         C.TropType = field[0];
         if(help) cout << " Input: trop model: " << C.TropType;
         if(field.size() == 4) {
            C.T = StringUtils::asDouble(field[1]);
            C.Pr = StringUtils::asDouble(field[2]);
            C.RH = StringUtils::asDouble(field[3]);
            if(help) cout << " and weather (T,P,RH): "
               << C.T << "," << C.Pr << "," << C.RH;
         }
         if(help) cout << endl;
      }
   }
   if(dashLog.getCount()) {
      values = dashLog.getValue();
      C.LogFile = values[0];
      if(help) cout << "Log file is " << C.LogFile << endl;
   }
   if(dashRfile.getCount()) {
      values = dashRfile.getValue();
      C.OutRinexObs = values[0];
      if(help) cout << "Output Rinex file name is " << C.OutRinexObs << endl;
   }
   if(dashRrun.getCount()) {
      values = dashRrun.getValue();
      C.HDRunby = values[0];
      if(help) cout << "Output Rinex 'RUN BY' is " << C.HDRunby << endl;
   }
   if(dashRobs.getCount()) {
      values = dashRobs.getValue();
      C.HDObs = values[0];
      if(help) cout << "Output Rinex 'OBSERVER' is " << C.HDObs << endl;
   }
   if(dashRag.getCount()) {
      values = dashRag.getValue();
      C.HDAgency = values[0];
      if(help) cout << "Output Rinex 'AGENCY' is " << C.HDAgency << endl;
   }
   if(dashRmark.getCount()) {
      values = dashRmark.getValue();
      C.HDMarker = values[0];
      if(help) cout << "Output Rinex 'MARKER' is " << C.HDMarker << endl;
   }
   if(dashRnumb.getCount()) {
      values = dashRnumb.getValue();
      C.HDNumber = values[0];
      if(help) cout << "Output Rinex 'NUMBER' is " << C.HDNumber << endl;
   }

   if(Rest.getCount()) {
      if(help) cout << "Remaining options:" << endl;
      values = Rest.getValue();
      for (i=0; i<values.size(); i++) {
         if(help) cout << values[i] << endl;
         //C.InputObsName.push_back(values[i]);
      }
   }
   //if(C.Verbose && help) {
   // cout << "\nTokens on command line (" << Args.size() << ") are:" << endl;
   // for(unsigned j=0; j<Args.size(); j++) cout << Args[j] << endl;
   //}

   C.oflog.open(C.LogFile.c_str(),ios::out);
   if(C.oflog.fail()) {
      cout << "Failed to open log file " << C.LogFile << endl;
   }
   else {
      cout << "Opened log file " << C.LogFile << endl;
      C.oflog << Title;
   }

      // print config to log
   C.oflog << "\nHere is the input configuration:\n";
   C.oflog << " Input Rinex obs files are:\n";
   for(i=0; i<C.InputObsName.size(); i++) {
      C.oflog << "   " << C.InputObsName[i] << endl;
   }
   C.oflog << " Input Obs directory is " << C.ObsDirectory << endl;
   C.oflog << " Input Nav directory is " << C.NavDirectory << endl;
   C.oflog << " Ithing time interval is " << C.ith << endl;
   if(C.Tbeg > DayTime(DayTime::BEGINNING_OF_TIME)) C.oflog << " Begin time is "
      << C.Tbeg.printf("%04Y/%02m/%02d %02H:%02M:%.3f")
      << " = " << C.Tbeg.printf("%04F/%10.3g") << endl;
   if(C.Tend < DayTime(DayTime::END_OF_TIME)) C.oflog << " End time is "
      << C.Tend.printf("%04Y/%02m/%02d %02H:%02M:%.3f")
      << " = " << C.Tend.printf("%04F/%10.3g") << endl;
   if(C.UseCA) C.oflog << " 'Use C/A' flag is set\n";
   //C.oflog << " DT is set to " << C.DT << endl;
   if(C.ExSV.size()) {
      C.oflog << " Exclude satellites";
      for(i=0; i<C.ExSV.size(); i++) C.oflog << " " << C.ExSV[i];
      C.oflog << endl;
   }
   C.oflog << " Trop model: " << C.TropType
      << " and weather (T,P,RH): " << C.T << "," << C.Pr << "," << C.RH << endl;
   C.oflog << " Log file is " << C.LogFile << endl;
   if(C.APSout) C.oflog << " Output autonomous solution (no RAIM) - APS,etc.\n";
   C.oflog << " Output format for time tags (cf. class DayTime) is "
      << C.timeFormat << endl;
   if(C.knownpos.getCoordinateSystem() != Position::Unknown)
      C.oflog << " Output residuals: known position is\n   " << C.knownpos.printf(
         "ECEF(m) %.4x %.4y %.4z\n     = %A deg N %L deg E %h m\n");
   if(!C.OutRinexObs.empty()) C.oflog << " Output Rinex file name is "
      << C.OutRinexObs << endl;
   if(!C.HDRunby.empty()) C.oflog << " Output Rinex 'RUN BY' is "
      << C.HDRunby << endl;
   if(!C.HDObs.empty()) C.oflog << " Output Rinex 'OBSERVER' is "
      << C.HDObs << endl;
   if(!C.HDAgency.empty()) C.oflog << " Output Rinex 'AGENCY' is "
      << C.HDAgency << endl;
   if(!C.HDMarker.empty()) C.oflog << " Output Rinex 'MARKER' is "
      << C.HDMarker << endl;
   if(!C.HDNumber.empty()) C.oflog << " Output Rinex 'NUMBER' is "
      << C.HDNumber << endl;
   C.oflog << " ------ PRSolution configuration (-1 means use PRSolution default) :"
      << endl;
   C.oflog << " Solution limit parameters are "
      << C.rmsLimit << " = RMS residuals (m) limit, and "
      << C.slopeLimit << " = RAIM 'slope' limit" << endl;
   C.oflog << " Algebraic algorithm is turned "
      << (C.algebra ? "ON ":"OFF ") << endl;
   C.oflog << " Residual criterion is '"
      << (C.residCrit ? "RMS residuals" : "Distance from apriori") << "'" << endl;
   C.oflog << " Return-at-once option is "
      << (C.returnatonce ? "on" : "off") << endl;
   C.oflog << " Maximum # of satellites to reject is " << C.maxReject << endl;
   C.oflog << " Minimum elevation angle is " << C.elevLimit << " degrees." << endl;
   C.oflog << " LLS convergence parameters are " << C.nIter << " iterations and "
      << scientific << setprecision(3) << C.convLimit << " RSS convergence (m)"
      << endl;
   C.oflog << "End of input configuration summary" << endl;

   if(help) return 1;
   return 0;
}
catch(gpstk::Exception& e) {
      GPSTK_RETHROW(e);
}
catch (...) {
      cerr << "PRSolve:GetCommandLine caught an unknown exception\n";
}
   return -1;
}

//------------------------------------------------------------------------------------
void PreProcessArgs(const char *arg, vector<string>& Args, bool& ver)
{
try {
   if(arg[0]=='-' && arg[1]=='f') {
      string fname(arg);
      fname.erase(0,2);
      cout << "Found a file of options: " << fname << endl;
      ifstream infile(fname.c_str());
      if(!infile) {
         cerr << "Error: could not open options file "
            << fname << endl;
      }
      else {
         char c;
         string buffer,word;
         while(1) {
            getline(infile,buffer);
            if(infile.eof() || !infile.good()) break;

            while(1) {
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
               PreProcessArgs(word.c_str(),Args,ver); //Args.push_back(buffer);
               if(buffer.empty()) break;
            }
         }
      }
   }
   else if((arg[0]=='-' && arg[1]=='v') || string(arg)==string("--verbose")) {
      ver = true;
      cout << "Found the verbose switch" << endl;
   }
   else Args.push_back(arg);
}
catch(gpstk::Exception& e) {
      GPSTK_RETHROW(e);
}
catch (...) {
      cerr << "PRSolve:PreProcessArgs caught an unknown exception\n";
}
}

//------------------------------------------------------------------------------------
bool isSP3File(const string& file)
{
   SP3Header header;
   SP3Stream strm(file.c_str());
   strm.exceptions(fstream::failbit);
   try { strm >> header; } catch(gpstk::Exception& e) { return false; }
   strm.close();
   return true;
}
bool isRinexNavFile(const string& file)
{
   RinexNavHeader header;
   RinexNavStream rnstream(file.c_str());
   rnstream.exceptions(fstream::failbit);
   try { rnstream >> header; } catch(gpstk::Exception& e) { return false; }
   rnstream.close();
   return true;
}
int FillEphemerisStore(const vector<string>& files, SP3EphemerisStore& PE,
   BCEphemerisStore& BCE)
{
   try {
      int nread=0;
      RinexNavHeader rnh;
      RinexNavData rne;
      for(int i=0; i<files.size(); i++) {
         if(files[i].empty()) throw Exception("File name is empty");
         RinexNavStream strm(files[i].c_str());
         if(!strm) throw Exception("Could not open file " + files[i]);
         strm.close();
         if(isRinexNavFile(files[i])) {
            RinexNavStream RNFileIn(files[i].c_str());
            RNFileIn.exceptions(fstream::failbit);
            try {
               RNFileIn >> rnh;
               while (RNFileIn >> rne)
               {
                  if(rne.health == 0)
                     BCE.addEphemeris(rne);
               }
               nread++;
            }
            catch(gpstk::Exception& e) {
               cerr << "Caught Exception while reading Rinex Nav file " << files[i]
                  << " : " << e << endl;
               continue;
            }
         }
         else if(isSP3File(files[i])) {
            try {
               PE.loadFile(files[i]);
            }
            catch(gpstk::Exception& e) {
               cerr << "Caught Exception while reading SP3 Nav file " << files[i]
                  << " : " << e << endl;
               continue;
            }
            nread++;
         }
         else throw Exception("File " + files[i] + " is neither BCE nor PE file.");
      }
      return nread;
   }
   catch(gpstk::Exception& e) {
      GPSTK_RETHROW(e);
   }
   catch (...) {
      cerr << "PRSolve:FillEphemerisStore caught an unknown exception\n";
   }
   return -1;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
