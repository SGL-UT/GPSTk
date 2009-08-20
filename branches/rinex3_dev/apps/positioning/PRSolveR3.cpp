// $Id: PRSolve.cpp 2042 2009-07-20 20:37:07Z raindave $

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
 * @file PRSolve.cpp  Read a RINEX observation file and compute an autonomous GPS
 * pseudorange position solution, using a RAIM-like algorithm to eliminate outliers.
 */

#define RANGECHECK 1        //make Matrix and Vector check limits
//#define DEBUG_PRINT_SATS_PASSED
//#define DEBUG_PRINT_SATS_USED
//#define DEBUG_PRINT_WARNINGS
//#define DEBUG_PRINT_RETURNS
//#define DEBUG_PRINT_TIME
/******************************************************************************/
/** **                       ** *** Includes *** **                       ** **/
/******************************************************************************/
#include <cstring>

#include "Exception.hpp"
#include "StringUtils.hpp"

#include "TimeSystem.hpp"
#include "CommonTime.hpp"
#include "GPSWeekSecond.hpp"
#include "YDSTime.hpp"

#include "RinexSatID.hpp"
#include "CommandOptionParser.hpp"
#include "CommandOption.hpp"
#include "CommandOptionWithCommonTimeArg.hpp"

#include "Rinex3ObsData.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsStream.hpp"
#include "Rinex3NavStream.hpp"
#include "Rinex3NavData.hpp"
#include "RinexMetStream.hpp"
#include "RinexMetHeader.hpp"
#include "RinexMetData.hpp"
#include "SP3Stream.hpp"
#include "GPSEphemerisStore.hpp"
#include "SP3EphemerisStore.hpp"
#include "TropModel.hpp"
#include "Position.hpp"
#include "geometry.hpp" //for DEG_TO_RAD

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


using namespace std;
using namespace gpstk;
using namespace StringUtils;

   // prgm data
string PrgmName("PRSolveR3");
string PrgmVers("0.9 8/6");

#ifndef PRSOLVE_R3_VARS
/******************************************************************************/
/** **                      ** *** Prototypes *** **                      ** **/
/******************************************************************************/
int readFile(int nfile)
            throw(Exception);
int solutionAlgorithm(vector<SatID>& Sats, vector<double>& PRanges,
                      double& RMSresid)
            throw(Exception);
int afterReadingFiles(void)
            throw(Exception);
void printStats(Stats<double> S[3], Matrix<double> &P, Vector<double> &z,
                  long n, string m, char c0='X', char c1='Y', char c2='Z')
            throw(Exception);
void setWeather(CommonTime& time, TropModel *pTropModel);

int getCommandLine(int argc, char **argv)
            throw(Exception);
void dumpConfiguration(ostream& os)
            throw(Exception);
void preprocessArgs(const char *arg, vector<string>& Args)
            throw(Exception);
int fillEphemerisStore(const vector<string>& files, /**/SP3EphemerisStore& PE,/**/
                        GPSEphemerisStore& BCE)
            throw(Exception);

/******************************************************************************/
/** **                         ** *** Data *** **                         ** **/
/******************************************************************************/
struct SystemFlags{
   int inC1C;
   int inC1P;
   int inC2P;
   int inL1;
   int inL2;
   
   bool good;
   
   //SP3EphemerisStore sp3EphStore;
   
   SatID::SatelliteSystem sys;
   
   SystemFlags()
   {
      inC1C = inC1P = inC2P = inL1 = inL2 = -1;
      good = false;
      sys = SatID::systemUnknown;
   }
   
   SystemFlags(SatID::SatelliteSystem system)
   {
      inC1C = inC1P = inC2P = inL1 = inL2 = -1;
      good = true;
      sys = system;
   }
};
//data input from command line
typedef struct Configuration {
      //input files
   string ObsDirectory,NavDirectory,MetDirectory;
   vector<string> InputObsName;
   vector<string> InputNavName;
   vector<string> InputMetName;
      //configuration
   double rmsLimit;
   double SlopeLimit;
   bool algebra;
   int nIter;
   double convLimit;
   int maxReject;
   bool residCrit;
   bool returnatonce;
   
   ///GPS/GLONASS stuff
   bool allSystems;   ///< Use all available systems. Default option.
   bool useGPS;   ///< Use GPS data from the header.
   bool useGlonass;   ///< Use GLONASS data from header.
   ///\GPS/GLONASS stuff
   
   double elevLimit;
      //output
   Position knownpos;
   Matrix<double> Rot;
   bool APSout;
   string ordFile;
   string OutRinexObs;
   string HDPrgm;       //header of output RINEX file
   string HDRunby;
   string HDObs;
   string HDAgency;
   string HDMarker;
   string HDNumber;
   int NrecOut;
   CommonTime FirstEpoch,LastEpoch;
   string timeFormat;
   bool Debug,Verbose;
      //data flow
   double ith;
   CommonTime Tbeg, Tend;
      //output files
   string LogFile;
   ofstream oflog,oford;
      //processing
   double DataInt;
   int Freq;
   bool UseCA,ForceCA;
   vector<SatID> ExSV;
   string TropType;
   double defaultT,defaultPr,defaultRH;
   TropModel *pTropModel;
   list<RinexMetData> MetStore;
      //estimate DT from data
   double estdt[9];
   int ndt[9];
} Config;

Config C;

//data used in program
const double CMPS=299792458.0;
const double CFF=CMPS/10.23e6;
const double F1=154.0;
const double F2=120.0;
const double wl1=CFF/F1;
const double wl2=CFF/F2;
const double F1F2=(F1/F2)*(F1/F2);
const double alpha=(F1F2 - 1.0);
const double if1r=1.0/(1.0-(F2/F1)*(F2/F1));
const double if2r=1.0/(1.0-(F1/F2)*(F1/F2));
clock_t totaltime;
string Title,filename;
CommonTime CurrEpoch, PrgmEpoch, PrevEpoch;

//data
bool sp3;
int solSize;
std::map<char,SystemFlags> gnssSystems;
int Nsvs;
XvtStore<SatID> *pEph;
ZeroTropModel TMzero;
SimpleTropModel TMsimple;
SaasTropModel TMsaas;
GGTropModel TMgg;
GGHeightTropModel TMggh;
NBTropModel TMnb;

//Solution and covariance (prsol for RAIM, Solution and Covariance for AutonPRSol)
PRSolution prsol;             //this will always be the RAIM result
Vector<double> Solution;      //this will always be the AutonPRS result
Matrix<double> Covariance;    //this will always be the AutonPRS result

//Solution and residual statistics:
//total number of epochs
long nS,nSS;
//simple average (S : one file; SS : all files)...
Stats<double> SA[3],SR[4],SSA[3],SSR[4];          //solution (XYZ) Auto and RAIM
Stats<double> SAPR[3],SRPR[3],SSAPR[3],SSRPR[3];  //XYZ residuals
Stats<double> SANE[3],SRNE[3],SSANE[3],SSRNE[3];  //NEU residuals
//... and weighted average solution, both Auto and RAIM...
Matrix<double> PA,PR,PPA,PPR;        //inverse covariance
Vector<double> zA,zR,zzA,zzR;        //'state'=(inverse covariance * state)
//... and weighted average residuals APR,RPR.
Matrix<double> PAPR,PRPR,PANE,PRNE,PPAPR,PPRPR,PPANE,PPRNE;
Vector<double> zAPR,zRPR,zANE,zRNE,zzAPR,zzRPR,zzANE,zzRNE;
#endif

int main(int argc, char **argv)
{
   try {
      totaltime = clock();
      int iret;

      C.Debug = true;

         // initialization
      CurrEpoch = PrevEpoch = CommonTime::BEGINNING_OF_TIME;
      SP3EphemerisStore SP3EphList;
      GPSEphemerisStore BCEphList;

         // Title and description
      Title = PrgmName + ", part of the GPS ToolKit, Ver " + PrgmVers + ", Run ";
      time_t timer;
      struct tm *tblock;
      timer = time(NULL);
      tblock = localtime(&timer);
      PrgmEpoch = CivilTime(1900+tblock->tm_year,1+tblock->tm_mon,
                            tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec,
                            TimeSystem::GPS);
      Title += (static_cast<CivilTime>(PrgmEpoch)).printf("%02m/%02d/%04Y %02H:%02M:%02S %P");
      cout << Title << endl;

         // get command line
      iret = getCommandLine(argc, argv);
      if (iret < 0) return iret;
      
      if ((C.useGlonass || C.allSystems) && C.algebra)
      {
         cout << "Cannot use Algebraic Solution method with Glonass data..." << endl;
         C.algebra = false;
		}
      
         // Add systems to use
      int numSvSystems = 0;
      if (C.useGPS || C.allSystems)
      {
         cout << "Using GPS Sats..." << endl;
         gnssSystems['G'] = SystemFlags(SatID::systemGPS);
         ++numSvSystems;
      }
      if (C.useGlonass || C.allSystems)
      {
      	cout << "Using Glonass Sats..." << endl;
         gnssSystems['R'] = SystemFlags(SatID::systemGlonass);
         ++numSvSystems;
      }
      if (numSvSystems > 1)
         solSize = 5;
      else
         solSize = 4;
      
      // NB save iret until after dumpConfiguration()

         // update configuration of PRSolution
      if (C.Verbose)
      {
         prsol.pDebugStream = &C.oflog;
         prsol.Debug = true;
      }
      prsol.RMSLimit = C.rmsLimit;
      prsol.SlopeLimit = C.SlopeLimit;
      prsol.Algebraic = C.algebra;
      prsol.ResidualCriterion = C.residCrit;
      prsol.ReturnAtOnce = C.returnatonce;
      prsol.NSatsReject = C.maxReject;
      prsol.MaxNIterations = prsol.NIterations = C.nIter;
      prsol.ConvergenceLimit = C.convLimit;

         // iret comes from getCommandLine
      if (iret == 0)
         dumpConfiguration(C.oflog);
      else
      {
         #ifdef DEBUG_PRINT_RETURNS
         cout << "Returning from main at line " << __LINE__ << endl;
         #endif
         return iret;
      }
      // get nav files and build EphemerisStore
      int nread = fillEphemerisStore(C.InputNavName, /**/SP3EphList,/**/ BCEphList);
      C.oflog << "Added " << nread << " ephemeris files to store.\n";
      SP3EphList.dump(C.oflog,0);
      BCEphList.dump(C.oflog,0);
      if (SP3EphList.size() > 0)
      {
         pEph = &SP3EphList;
         
         typedef std::map<SatID, std::map<CommonTime, Xvt> > EphMap;
         EphMap sp3EphMap = SP3EphList.getEphemerisMap();
         EphMap::iterator ephIter = sp3EphMap.begin();
         while(ephIter != sp3EphMap.end())
         {
            cout << "EphMap contains data for SatID \"" << (ephIter->first) << "\"" << endl;
            ephIter++;
         }
      }
      else if (BCEphList.size() > 0 && !sp3)
      {
         BCEphList.SearchNear();
         //BCEphList.SearchPast();
         pEph=&BCEphList;
      }
      else
      {
         C.oflog << "Failed to read ephemeris data. Abort." << endl;
         #ifdef DEBUG_PRINT_RETURNS
         cout << "Returning from main at line " << __LINE__ << endl;
         #endif
         return -1;
      }

      // get met files and build MetStore
      if (C.InputMetName.size() > 0)
      {
         for (int i=0; i<C.InputMetName.size(); i++)
         {
            RinexMetStream metstrm(C.InputMetName[i].c_str());
            RinexMetHeader methead;
            RinexMetData metdata;

            metstrm >> methead;

            while(metstrm >> metdata)
               C.MetStore.push_back(metdata);

            metstrm.close();
         }  // end loop over InputMetName

         // sort the store on time
         C.MetStore.sort();

         // dump the met data
         if (C.Debug)
         {
            C.oflog << "Dump of meteorological data store ("
                    << C.MetStore.size() << "):\n";
            list<RinexMetData>::const_iterator it = C.MetStore.begin();
            for ( ; it != C.MetStore.end(); it++)
            {
               CivilTime civtime(it->time);
               C.oflog
                  << civtime.printf("%02m/%02d/%04Y %02H:%02M:%02S %P")
                  << fixed << setprecision(1);
               cout
                  << civtime.printf("%02m/%02d/%04Y %02H:%02M:%02S %P");
               RinexMetData::RinexMetMap::const_iterator jt=it->data.begin();
               for ( ; jt != it->data.end(); jt++)
               {
                  C.oflog << "  " << RinexMetHeader::convertObsType(jt->first)
                          << " = " << setw(6) << jt->second;
               }
               C.oflog << endl;
            }
            C.oflog << "End dump of meteorological data store." << endl;
         }

      }  // end InputMetName processing

      // assign trop model
      if (C.TropType == string("ZR")) C.pTropModel = &TMzero;
      if (C.TropType == string("BL")) C.pTropModel = &TMsimple;
      if (C.TropType == string("SA") || C.TropType == string("NB"))
      {
         if (C.TropType == string("SA")) C.pTropModel = &TMsaas;
         if (C.TropType == string("NB")) C.pTropModel = &TMnb;
         if (C.knownpos.getCoordinateSystem() != Position::Unknown)
         {
            C.pTropModel->setReceiverLatitude(C.knownpos.getGeodeticLatitude());
            C.pTropModel->setReceiverHeight(C.knownpos.getHeight());
         }
         else
         {
            C.pTropModel->setReceiverLatitude(0.0);
            C.pTropModel->setReceiverHeight(0.0);
            C.oflog << "Warning - Saastamoinen and New B tropospheric models require "
                    << "latitude, height and day of year - guessing." << endl;
         }
         if (C.Tbeg > CommonTime::BEGINNING_OF_TIME)
            C.pTropModel->setDayOfYear((static_cast<YDSTime>(C.Tbeg)).doy);
         else if (C.Tend < CommonTime::END_OF_TIME)
            C.pTropModel->setDayOfYear((static_cast<YDSTime>(C.Tend)).doy);
         else
            C.pTropModel->setDayOfYear(100);
      }
      if (C.TropType == string("GG") ) C.pTropModel = &TMgg;
      if (C.TropType == string("GGH")) C.pTropModel = &TMggh;
      // set the default weather in the model
      C.pTropModel->setWeather(C.defaultT,C.defaultPr,C.defaultRH);

      // compute rotation XYZ->NEU at known position
      if (C.knownpos.getCoordinateSystem() != Position::Unknown)
      {
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

      if (!C.ordFile.empty())
      {
         if (C.knownpos.getCoordinateSystem() == Position::Unknown)
         {
            C.oflog << "Error - ORD output to file (" << C.ordFile << ") requires "
               << " --PosXYZ input. Abort output of ORDs." << endl;
            C.ordFile = string();
         }
         else {
            C.oford.open(C.ordFile.c_str(),ios::out);
            if (C.oford.fail())
            {
               C.oflog << "Failed to open ORD file " << C.ordFile << endl;
               C.ordFile = string();
            }
            else C.oford
               << "#   sat week seconds_wk ok? elev       ORD(C/A)       ORD(P)" << endl;
         }
      }

      // initialize global solution and residual statistics
      nSS = 0;
      PPA = Matrix<double>(3,3,0.0);
      PPR = Matrix<double>(3,3,0.0);
      zzA = Vector<double>(3,0.0);
      zzR = Vector<double>(3,0.0);
      if (C.knownpos.getCoordinateSystem() != Position::Unknown)
      {
         if (C.APSout)
         {
            PPAPR = Matrix<double>(3,3,0.0);
            PPANE = Matrix<double>(3,3,0.0);
            zzAPR = Vector<double>(3,0.0);
            zzANE = Vector<double>(3,0.0);
         }
         PPRPR = Matrix<double>(3,3,0.0);
         PPRNE = Matrix<double>(3,3,0.0);
         zzRPR = Vector<double>(3,0.0);
         zzRNE = Vector<double>(3,0.0);
      }

      // loop over input files
      nread = 0;
      for (int nfile=0; nfile<C.InputObsName.size(); nfile++)
      {
         iret = readFile(nfile);
         if (iret < 0)
            break;
         nread++;
      }  // end loop over input files

      if (iret>=0 && nread>0)
         iret=afterReadingFiles();

      totaltime = clock()-totaltime;
      C.oflog << "PRSolve timing: " << fixed << setprecision(3)
              << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.\n";
      cout << "\nPRSolve timing: " << fixed << setprecision(3)
           << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.\n";

      C.oflog.close();
      C.oford.close();
      
      #ifdef DEBUG_PRINT_RETURNS
      cout << "Returning from main at line " << __LINE__ << endl;
      #endif
      return iret;
   }
   catch(Exception& e)
   {
      cout << e;
   }
   catch(...)
   {
      cerr << C.oflog << "Unknown error.  Abort." << endl;
   }
   return 1;
}  //End main()

//------------------------------------------------------------------------------------
// Open the file, read header and check for data; then loop over the epochs
// Return 0 ok, <0 fatal error, >0 non-fatal error (ie skip this file)
// 0 ok, 1 couldn't open file, 2 file doesn't have required data
/**
 * To add a new system to the checks, follow 3 steps.
 * 1) Add a boolean value to the Configuration struct.
 * 2) Add a command line argument to enable that boolean and modify the all option.
 * 3) Add an if statment that checks that boolean and indexes the given system.
 */
int readFile(int nfile) throw(Exception)
{
   try
   {
      bool writeout, first;
      int i,j,k,iret;
      
      double dt;
         //The input and output Rinex 3 files
      Rinex3ObsStream ifstr, ofstr;
      Rinex3ObsHeader rhead, rheadout;
      
      //cout << "Reading Files..." << endl;
      
         //Open the Input file
      filename = C.InputObsName[nfile];
      ifstr.open(filename.c_str(),ios::in);
      if (ifstr.fail())
      {
         C.oflog << "Failed to open input file " << filename << ". Abort.\n";
         return 1;
      }
      else C.oflog << "Opened input file " << filename << endl;
      ifstr.exceptions(ios::failbit);

         //Open the output file
      if (!C.OutRinexObs.empty())
      {
         ofstr.open(C.OutRinexObs.c_str(), ios::out);
         if (ofstr.fail())
         {
            C.oflog << "Failed to open output file " << C.OutRinexObs << " Abort.\n";
            ifstr.close();
            return 1;
         }
         else C.oflog << "Opened output file " << C.OutRinexObs << endl;
         ofstr.exceptions(ios::failbit);
         writeout = true;
      }
      else writeout = false;

         //Read the Header
      ifstr >> rhead;
      C.oflog << "Header for the file " << filename << ":" << endl;
      rhead.dump(C.oflog);
      
      std::vector<ObsID> obsTypeList;
      std::map<char,SystemFlags>::iterator iter;
      map<std::string,vector<ObsID> >::iterator mapIter;
      for (iter = gnssSystems.begin(); iter != gnssSystems.end(); iter++)
      {
         SystemFlags& sysflags = iter->second;
         std::string str = "";
         str += iter->first;
         mapIter = rhead.mapObsTypes.find(str);
         if (mapIter == rhead.mapObsTypes.end())
         {
            #ifdef DEBUG_PRINT_WARNINGS
            cout << "Could not find system flag at line  " << __LINE__ << endl;
            #endif
            continue;
         }
         obsTypeList = mapIter->second;
         for (j = 0; j < obsTypeList.size(); j++)
         {
            std::string str(obsTypeList[j].asRinex3ID());
            if (str == "C1C")
               sysflags.inC1C=j;
            if (str == "C1P")
               sysflags.inC1P=j;
            if (str == "C2P")
               sysflags.inC2P=j;
            //Note: might want to keep each flag for the L1 and L2 variants
            if (str.substr(0,2) == "L1" && sysflags.inL1 == -1)
               sysflags.inL1=j;
            if (str.substr(0,2) == "L2" && sysflags.inL2 == -1)
               sysflags.inL2=j;
         }
         
      }
      
      int hasC1C, hasC1P, hasC2P, hasL1, hasL2;
      hasC1C = hasC1P = hasC2P = hasL1 = hasL2 = 0;
      for (iter = gnssSystems.begin(); iter != gnssSystems.end(); iter++)
      {
         SystemFlags& sysflags = iter->second;
         if (sysflags.inC1C > -1)
            hasC1C++;
         if (sysflags.inC1P > -1)
            hasC1P++;
         if (sysflags.inC2P > -1)
            hasC2P++;
         if (sysflags.inL1 > -1)
            hasL1++;
         if (sysflags.inL2 > -1)
            hasL2++;
      }
      
      if ((hasC1P == 0 && (!C.UseCA || hasC1C == 0))
            || (hasC1C == 0 && C.ForceCA)
            || hasC2P == 0 || hasL1 == 0 || hasL2 == 0)
      {
         C.oflog << "Warning: file " << filename << " does not contain";
         
         if (hasC1C == 0) C.oflog << " C1" << " (forceCA is "
                                    << (C.ForceCA?"T":"F") << ")";
         if (hasC1P == 0) C.oflog << " P1" << " (useCA is " << (C.UseCA?"T":"F")
                                    << ")";
         if (hasC2P == 0) C.oflog << " P2";
         if (hasL1 == 0) C.oflog << " L1";
         if (hasL2 == 0) C.oflog << " L2";
         
         C.oflog << endl;
      }
      if (C.ForceCA)
      {
         if (hasC1C > 0)
         {
            for (iter = gnssSystems.begin(); iter != gnssSystems.end(); iter++)
            {
               SystemFlags& sysflags = iter->second;
               if (sysflags.inC1C == -1)
               {
                  ///DROP THE SYSTEM
                  sysflags.good = false;
                  break;
               }
               sysflags.inC1P = sysflags.inC1C;
            }
         }
         else
         {
            C.oflog << "ERROR. Abort. --forceCA was found but C1 data is not found.\n";
            cout << "ERROR. Abort. --forceCA was found but C1 data is not found.\n";
            return -1;
         }
      }
      else if (hasC1P == 0)
      {
         if (C.UseCA && hasC1C > 0)
         {
            for (iter = gnssSystems.begin(); iter != gnssSystems.end(); iter++)
            {
               SystemFlags& sysflags = iter->second;
               if (sysflags.inC1C == -1)
               {
                  sysflags.good = false;
                  break;
               }
               sysflags.inC1P = sysflags.inC1C;
            }
         }
         else if (C.UseCA && hasC1C == 0)
         {
            C.oflog << "ERROR. Abort. Neither P1 nor C1 data found (--useCA is set).\n";
            cout << "ERROR. Abort. Neither P1 nor C1 data found (--useCA is set).\n";
            return -1;
         }
         else if (C.Freq != 2 && !C.UseCA && hasC1C > 0)
         {
            C.oflog << "ERROR. Abort. P1 data not found (C1 data found: add --useCA)\n";
            cout << "ERROR. Abort. P1 data not found (C1 data found: add --useCA)\n";
            return -1;
         }
         else if (C.Freq != 2)
         {
            C.oflog << "ERROR. Abort. Neither P1 nor C1 data found.\n";
            cout << "ERROR. Abort. Neither P1 nor C1 data found.\n";
            cout << "rhead.obsTypeList.size() == " << rhead.obsTypeList.size() << endl;
            return -1;
         }
      }

      // determine which frequency to process
      if (C.Freq != 1 && hasC2P == 0)
      {
         C.oflog << "WARNING. Unable to process L" << C.Freq << " data - no L2." << endl;
         C.Freq = 1;
      }
      C.oflog << "Process frequency " << C.Freq << endl;
      
         // initialize file solution and residual statistics
      nS = 0;
      SA[0].Reset(); SA[1].Reset(); SA[2].Reset();
      SR[0].Reset(); SR[1].Reset(); SR[2].Reset();
      PA = Matrix<double>(3,3,0.0);
      PR = Matrix<double>(3,3,0.0);
      zA = Vector<double>(3,0.0);
      zR = Vector<double>(3,0.0);

      if (C.knownpos.getCoordinateSystem() != Position::Unknown)
      {
         if (C.APSout)
         {
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
      while(1)
      {
            // read next obs
         double RMSrof;
         vector<SatID> Satellites;
         vector<double> Ranges,vC1,vP1,vP2;
         Matrix<double> inform;
         Rinex3ObsData robsd,auxPosData;

         try
         {
            ifstr >> robsd;
         }
         catch(FFStreamError& e)
         {
            C.oflog << "Reading obs caught FFStreamError exception : " << e << endl;
            cout << "Reading obs caught FFStreamError exception : " << e << endl;
            return -2;
         }
         catch(Exception& e)
         {
            C.oflog << "Reading obs caught GPSTk exception : " << e << endl;
            cout << "Reading obs caught GPSTk exception : " << e << endl;
            return -2;
         }
         catch(exception& e)
         {
            C.oflog << "Reading obs caught std exception : " << e.what() << endl;
            cout << "Reading obs caught std exception : " << e.what() << endl;
            return -2;
         }
         catch(...)
         {
            C.oflog << "Reading obs caught unknown exception : " << endl;
            cout << "Reading obs caught unknown exception : " << endl;
            return -2;
         }

            // normal end-of-file
         if (!ifstr.good() || ifstr.eof())
         {
            iret = 0;
            break;
         }

         for (;;)
         {
            iret = 0;

            if (C.Debug)
               C.oflog << "process: " << robsd.time
                       << ", Flag " << robsd.epochFlag
                       << ", clk " << robsd.clockOffset << endl;

               //Stay within time limits
            if (robsd.time < C.Tbeg)
            {
               #ifdef DEBUG_PRINT_WARNINGS
               cout << "robsd.time < C.Tbeg" << endl;
               cout << "Line: " << __LINE__ << "\n";
               #endif
               iret =  1;
               break;
            }
            if (robsd.time > C.Tend)
            {
               #ifdef DEBUG_PRINT_WARNINGS
               cout << "robsd.time > C.Tend" << endl;
               cout << "Line: " << __LINE__ << "\n";
               #endif
               iret = -1;
               break;
            }

               //Ignore comment blocks ...
            if (robsd.epochFlag != 0 && robsd.epochFlag != 1)
            {
               iret = 1;
               break;
            }

               //Decimate data
               //If Tbeg is still undefined, set it to begin of week
            if (C.ith > 0.0)
            {
               if (fabs(C.Tbeg-CommonTime::BEGINNING_OF_TIME) < 1.0e-8)
               {
                  C.Tbeg = robsd.time;
               }
               double dt = fabs(robsd.time - C.Tbeg);
               dt -= C.ith*long(0.5+dt/C.ith);
               if (fabs(dt) > 0.25)
               {
                  iret = 1;
                  break;
               }
            }

               //Save current time
            CurrEpoch = robsd.time;
            if (fabs(C.FirstEpoch-CommonTime::BEGINNING_OF_TIME) < 1.0e-8)
               C.FirstEpoch = CurrEpoch;

               //Loop over satellites
            Nsvs = 0;
            Satellites.clear();
            Ranges.clear();
            vC1.clear();
            vP1.clear();
            vP2.clear();

            Rinex3ObsData::DataMap::const_iterator it;
            for (it=robsd.obs.begin(); it != robsd.obs.end(); ++it)
            {
               //Loop over the DataMap, then pull data from the resulting vector
               //based on the associated SystemFlags for it's system
               int in,n, index;
               double C1 = 0, P1 = 0, P2 = 0, L1,L2;
               RinexSatID sat = it->first;
               std::vector<Rinex3ObsData::RinexDatum> datum = it->second;
               iter = gnssSystems.find(sat.systemChar());
               if (iter == gnssSystems.end())
               {
                  continue;
               }
               SystemFlags& sysflags = iter->second;
               
               if (!sysflags.good)
               {
                  continue;
               }
                  //Pull out the data
               if (sysflags.inC1C > -1)
                  C1 = datum[sysflags.inC1C].data;
               if (sysflags.inC1P > -1)
                  P1 = datum[sysflags.inC1P].data;
               if (sysflags.inC2P > -1)
                  P2 = datum[sysflags.inC2P].data;
               if (sysflags.inL1 > -1)
                  L1 = datum[sysflags.inL1].data;
               if (sysflags.inL2 > -1)
                  L2 = datum[sysflags.inL2].data;
               
               
               bool ok = true;
               for (i=0; i<C.ExSV.size(); i++)
               {
                  if (C.ExSV[i] == sat)
                  {
                     #ifdef DEBUG_PRINT_WARNINGS
                     cout << "Excluding " << sat << endl;
                     cout << "Line: " << __LINE__ << "\n";
                     #endif
                     ok = false;
                     break;
                  }
               }
               
               if (!ok)
               {
               	#ifdef DEBUG_PRINT_WARNINGS
                  cout << "LOC " << __LINE__ << endl;
                  #endif
                  continue;
               }
               //NB do not exclude negative P, as some clocks can go far
               if (C.Freq != 2 && P1==0.0)
               {
               	#ifdef DEBUG_PRINT_WARNINGS
               	cout << "Using Frequency 1: " << sat << " does not have P1 data..." << endl;
               	cout << "Line " << __LINE__ << endl;
                  #endif
                  cout << "inC1C=" << sysflags.inC1C << " C1=" << C1 << endl;
                  cout << "inC1P=" << sysflags.inC1P << " P1=" << P1 << endl;
                  cout << "inC2P=" << sysflags.inC2P << " P2=" << P2 << endl;
                  cout << "inL1=" << sysflags.inL1 << " L1=" << L1 << endl;
                  cout << "inL2=" << sysflags.inL2 << " L2=" << L2 << endl;
                  continue;
               }
               if (C.Freq != 1 && P2==0.0)
               {
                  #ifdef DEBUG_PRINT_WARNINGS
               	cout << "Using Frequency 2: " << sat << " does not have P2 data..." << endl;
               	cout << "Line " << __LINE__ << endl;
                  #endif
                  continue;
               }
               //If position known and elevation limit given, apply elevation mask
               if (C.knownpos.getCoordinateSystem() != Position::Unknown
                   && C.elevLimit > 0.0)
               {
                  bool ok = true;
                  CorrectedEphemerisRange CER;
                  try
                  {
                     //Double ER =
                        CER.ComputeAtReceiveTime(CurrEpoch, C.knownpos, sat, *pEph);
                     if (CER.elevation < C.elevLimit)
                        ok = false;
                     if (C.Debug)
                        C.oflog << "Ephemeris range is "
                                << setprecision(4) << CER.rawrange << endl;
                  }
                  catch(InvalidRequest& nef)
                  {
                     //Do not exclude the sat here; PRSolution will...
                     if (C.Debug)
                        C.oflog << "CER did not find ephemeris for " << sat << endl;
                     #ifdef DEBUG_PRINT_WARNINGS
                     cout << "Caught InvalidRequest at line " << __LINE__ << endl;
                     cout << nef << endl;
                     #endif
                  }

                  if (!ok)
                  {
                     continue;
						}
               }

               //Keep this satellite
               Satellites.push_back(sat);
               Ranges.push_back(C.Freq == 3 ? if1r*P1+if2r*P2 :
                               (C.Freq == 2 ? P2 : P1));
               if (!C.ordFile.empty())
               {
                  //TD check vs Freq
                  vC1.push_back(C1);
                  vP1.push_back(P1);
                  vP2.push_back(P2);
               }
               Nsvs++;

            }  //End loop over sats
            
            //We now have 5 variables, so 5 sats are needed
            if (Nsvs <= 5)
            {
               if (C.Debug)
                  C.oflog << "Too few satellites" << endl;
               #ifdef DEBUG_PRINT_WARNINGS
               cout << "Too few satellites (" << Nsvs << ")..." << endl;
               #endif
               iret = 1;
               break;
            }
            
            nS++;
            nSS++;
            
            iret = solutionAlgorithm(Satellites, Ranges, RMSrof);
            
            if (C.Debug)
               C.oflog << "solutionAlgorithm returns " << iret << endl;
            if (iret)
               break;
            
               //Update LastEpoch and estimate of DT
            if (C.LastEpoch > CommonTime::BEGINNING_OF_TIME)
            {
               dt = CurrEpoch - C.LastEpoch;
               for (i=0; i<9; i++)
               {
                  if (C.ndt[i]<=0)
                  {
                     C.estdt[i] = dt;
                     C.ndt[i] = 1;
                     break;
                  }
                  if (fabs(dt-C.estdt[i]) < 0.0001)
                  {
                     C.ndt[i]++;
                     break;
                  }
                  if (i == 8)
                  {
                     int k = 0, nl = C.ndt[k];
                     for (j=1; j<9; j++)
                     {
                        if (C.ndt[j] <= nl)
                        {
                           k=j;
                           nl=C.ndt[j];
                        }
                     }
                     C.ndt[k] = 1; C.estdt[k] = dt;
                  }
               }
            }
            C.LastEpoch = CurrEpoch;

            break;
         }  // end for (;;)

         if (C.Debug)
            C.oflog << "processing returned " << iret << endl;
         if (iret == -1)
         {     //End of File
            iret=0;
            break;
         }
         if (iret == -4)
         {
            #ifdef DEBUG_PRINT_WARNINGS
            cout << "No Ephemeris for epoch: line " << __LINE__ << endl;
            #endif
            continue;   // ignore this epoch - no ephemeris
         }
         if (iret == 1)
         {
            #ifdef DEBUG_PRINT_WARNINGS
            cout << "Fatal Error at line " << __LINE__ << " (not enough sats)" << endl;
            #endif
            continue;   //Ignore this epoch - fatal error
         }
            //Write out ORDs
         if (!C.ordFile.empty())
         {
            int n = 0;
            double clk = 0.0;
            for (i=0; i<Satellites.size(); i++)
            {
               SatID sat=Satellites[i];
                  //Don't allow bad sats b/c it can corrupt TropModel
               if (sat.id < 0) continue;

               CorrectedEphemerisRange CER;
               try
               {
                  CER.ComputeAtTransmitTime(CurrEpoch, vP1[i], C.knownpos, sat, *pEph);
               }
               catch(InvalidRequest& nef)
               {
                  continue;
               }

               //Compute ionosphere - note that P1-R-RI == P2-R-RI*(F1/F2)**2
               double RI = (vP2[i]-vP1[i])/alpha;
               double tc = C.pTropModel->correction(C.knownpos,CER.svPosVel.x,CurrEpoch);
               double R = 0.0;
               if ( sat.system == SatID::systemGPS )
                  R = CER.rawrange + prsol.Solution(3)
                      - CER.svclkbias - CER.relativity + tc;
               else if ( sat.system == SatID::systemGlonass ) // include system time offset
                  R = CER.rawrange + prsol.Solution(3) + prsol.Solution(4)
                      - CER.svclkbias - CER.relativity + tc;

               //Output
               C.oford << "ORD"
                  << " G" << setw(2) << setfill('0') << sat.id << setfill(' ')
                  << " " << (static_cast<CivilTime>(CurrEpoch)).printf(C.timeFormat)
                  << " " << 1 << fixed << setprecision(3)
                  << " " << setw(6) << CER.elevation
                  << " " << setw(13) << vC1[i] - R - RI
                  << " " << setw(13) << vP1[i] - R - RI
                  << endl;

               clk += vP1[i] - (CER.rawrange - CER.svclkbias - CER.relativity + tc) - RI;
               n++;
            }
            //Output a clock record, clk = average range residual from known pos
            if (n > 0)
            {
               clk /= double(n);
               C.oford << "CLK"
                  << " " << (static_cast<CivilTime>(CurrEpoch)).printf(C.timeFormat)
                  << " " << setw(2) << n
                  << " " << fixed << setprecision(3)
                  << " " << setw(13) << clk
                  << endl;
            }
         }

            //Accumulate simple statistics, Autonomous and RAIM
         if (C.APSout)
         {
            SA[0].Add(Solution(0)); SA[1].Add(Solution(1)); SA[2].Add(Solution(2));
            SSA[0].Add(Solution(0)); SSA[1].Add(Solution(1)); SSA[2].Add(Solution(2));
         }
         SR[0].Add(prsol.Solution(0));
         SR[1].Add(prsol.Solution(1));
         SR[2].Add(prsol.Solution(2));
         if ( prsol.Solution.size() == 5 )
            SR[3].Add(prsol.Solution(4)/C_GPS_M*1e9); // GPS-GLO time offset (ns), mixed case only
         SSR[0].Add(prsol.Solution(0));
         SSR[1].Add(prsol.Solution(1));
         SSR[2].Add(prsol.Solution(2));
         if ( prsol.Solution.size() == 5 )
         {
            SSR[3].Add(prsol.Solution(4)/C_GPS_M*1e9); // GPS-GLO time offset (ns), mixed case only
         }

            //Accumulate weighted statistics, Auto and RAIM
         if (C.APSout)
         {
            inform = inverseSVD(Matrix<double>(Covariance,0,0,3,3));
            PA  += inform;
            PPA += inform;
            zA  += inform * Vector<double>(Solution,0,3);
            zzA += inform * Vector<double>(Solution,0,3);
         }
         inform = inverseSVD(Matrix<double>(prsol.Covariance,0,0,3,3));
         PR  += inform;
         PPR += inform;
         zR  += inform * Vector<double>(prsol.Solution,0,3);
         zzR += inform * Vector<double>(prsol.Solution,0,3);

         if (!writeout)
            continue;

            //Output to RINEX
         if (first)
         {     //Edit the output RINEX header
            rheadout = rhead;
            rheadout.date = (static_cast<CivilTime>(PrgmEpoch)).printf("%02m/%02d/%04Y %02H:%02M:%02S %P");
            rheadout.fileProgram = PrgmName;
            
            if (!C.HDRunby.empty())
               rheadout.fileAgency = C.HDRunby;
            if (!C.HDObs.empty())
               rheadout.observer = C.HDObs;
            if (!C.HDAgency.empty())
               rheadout.agency = C.HDAgency;
            if (!C.HDMarker.empty())
               rheadout.markerName = C.HDMarker;
            if (!C.HDNumber.empty())
            {
               rheadout.markerNumber = C.HDNumber;
               rheadout.valid |= Rinex3ObsHeader::validMarkerNumber;
            }
            
            rheadout.version = 3.00; rheadout.valid |= Rinex3ObsHeader::validVersion;
            rheadout.firstObs = C.FirstEpoch;
            rheadout.valid |= Rinex3ObsHeader::validFirstTime;
            
               //Invalidate the table
            if (rheadout.valid & Rinex3ObsHeader::validNumSats)
               rheadout.valid ^= Rinex3ObsHeader::validNumSats;
            
            if (rheadout.valid & Rinex3ObsHeader::validPrnObs)
               rheadout.valid ^= Rinex3ObsHeader::validPrnObs;

            ofstr << rheadout;
            first=false;
         }
         if (iret > 2)
         {                         //Output position first
            auxPosData.time = robsd.time;
            auxPosData.epochFlag = 4;
            auxPosData.numSVs = 2;              //Must be sure only 2 lines are written
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
            auxPosData.auxHeader.valid |= Rinex3ObsHeader::validComment;
            ofstr << auxPosData;
         }
         ofstr << robsd;                       //Output data to RINEX file

      }  //End while loop over epochs

         //Only print per file if there is more than one file
      if (C.InputObsName.size() > 1)
      {
         if (C.APSout) printStats(SA,PA,zA,nS,"Autonomous solution for file " + filename);
         printStats(SR,PR,zR,nS,"RAIM solution for file " + filename);
         if (C.knownpos.getCoordinateSystem() != Position::Unknown)
         {
            if (C.APSout)
            {
               printStats(SAPR,PAPR,zAPR,nS,
                  "Autonomous solution residuals for file " + filename);
               printStats(SANE,PANE,zANE,nS,
                  "Autonomous solution residuals (NEU) for file "+ filename,'N','E','U');
            }
            printStats(SRPR,PRPR,zRPR,nS,"RAIM solution residuals for file " + filename);
            printStats(SRNE,PRNE,zRNE,nS,
               "RAIM solution residuals (NEU) for file " + filename,'N','E','U');
         }
         //Print to screen
         cout << "\nWeighted average RAIM solution for file: "
              << filename << endl << fixed;
         cout << " (" << nS << " total epochs, with "
              << SR[0].N() << " good, " << nS-SR[0].N() << " rejected.)\n";
         if (SR[0].N() > 0)
         {
            Matrix<double> Cov=inverse(PR);
            Vector<double> Sol = Cov * zR;
            cout << setw(16) << setprecision(6) << Sol << endl;
            cout << "Covariance of RAIM solution for file " << filename << endl;
            cout << setw(16) << setprecision(6) << Cov << endl;
         }
         else cout << " No data!" << endl;
      }

      ifstr.clear();
      ifstr.close();
      ofstr.close();

      return iret;
   }
   catch(Exception& e)
   {
      GPSTK_RETHROW(e);
   }
   catch(exception& e)
   {
      Exception E("std except: "+string(e.what()));
      GPSTK_THROW(E);
   }
   catch(...)
   {
      Exception e("Unknown exception");
      GPSTK_THROW(e);
   }
   return -1;
}

//------------------------------------------------------------------------------------
// Return 0 ok,
//      <-1 fatal error: -4 no ephemeris
//       -1 end of file,
//        1 skip this epoch,
//        2 output to RINEX,
//        3 output position also
int solutionAlgorithm(vector<SatID>& Sats,
                      vector<double>& PRanges,
                      double& RMSresid)
   throw(Exception)
{
   #ifdef DEBUG_PRINT_SATS_PASSED
      cout << "Sats to work on :" << endl;
      for (int i = 0; i < Sats.size(); ++i)
      {
         cout << i << " : " << Sats[i] << endl;
      }
   #endif
   
   try
   {
      int iret,i;
      Matrix<double> inform;
      
      //Fail if not enough data
      if (Nsvs < 5)
      {
         #ifdef DEBUG_PRINT_WARNINGS
            cout << "Not Enough Satellites..." << endl;
            cout << "Line: " << __LINE__ << "\n";
         #endif
         return 1;
      }
      
      //Compute a position solution with this data
      if (C.Debug)
      {
         C.oflog << "Satellites and Ranges before Prepare:\n";
         for (i=0; i<PRanges.size(); i++)
         {
            C.oflog << " " << setw(2) << RinexSatID(Sats[i]) << fixed
                    << " " << setw(13) << setprecision(3) << PRanges[i] << endl;
         }
      }
      
      int niter = C.nIter;
      double conv = C.convLimit;
      vector<bool> UseSats(Sats.size(),true);
      Vector<double> Residual,Slope;
      
      //If met data available, update weather in trop model
      if (C.InputMetName.size() > 0)
         setWeather(CurrEpoch,C.pTropModel);
      
      //Compute using AutonomousPRSolution - no RAIM algorithm
      if (C.APSout)
      {
         iret = -4;
         Matrix<double> SVP;
         ///Check PrepareAutonomousSolution()
         ///Here...
         iret = PRSolution::PrepareAutonomousSolution(CurrEpoch,Sats,PRanges,*pEph,SVP);
         
         if (iret == -4)
         {
            #ifdef DEBUG_PRINT_WARNINGS
               cout << "No Ephemeris!" << endl;
               cout << "Line: " << __LINE__ << "\n";
            #endif
            C.oflog << "PrepareAutonomousSolution failed to find ANY ephemeris at epoch "
                    << (static_cast<CivilTime>(CurrEpoch)).printf("%02m/%02d/%04Y %02H:%02M:%02S %P")
                    << endl;
            return iret;
         }
         
         if (C.Debug)
         {
            C.oflog << "Satellites after  Prepare(" << iret << "):";
            for (i=0; i<Sats.size(); i++)
            {
               C.oflog << " " << setw(2) << Sats[i].id; C.oflog << endl;
            }
            C.oflog << "Matrix SVP(" << SVP.rows() << "," << SVP.cols() << "):\n"
                    << fixed << setw(13) << setprecision(3) << SVP << endl;
         }
         
         for (i=0; i<Sats.size(); i++)
         {
            UseSats[i] = (Sats[i].id > 0 ? true : false);
            #ifdef DEBUG_PRINT_SATS_USED
               cout << "UseSats[i] = ";
               (UseSats[i] ? cout << "true" : cout << "false");
               cout << endl;
            #endif
         }
         
         ///Check AutonomousPRSolution()
         iret = PRSolution::AutonomousPRSolution(CurrEpoch, UseSats, SVP,
                                                   C.pTropModel, C.algebra,
                                                   niter, conv, Solution,
                                                   Covariance, Residual, Slope,
                                                   (C.Debug ? &C.oflog : NULL));

         C.oflog << "APS " << setw(2) << iret
                 << " " << (static_cast<CivilTime>(CurrEpoch)).printf(C.timeFormat)
                 << " " << setw(2) << Nsvs;
         
         if (iret == 0)
         {
            C.oflog << fixed
                    << " " << setw(16) << setprecision(6) << Solution(0)
                    << " " << setw(16) << setprecision(6) << Solution(1)
                    << " " << setw(16) << setprecision(6) << Solution(2)
                    << " " << setw(14) << setprecision(6) << Solution(3)
                    << " " << setw(12) << setprecision(6) << RMS(Residual)
                    << " " << fixed << setw(5) << setprecision(1) << max(Slope);
         }
         
         C.oflog << " " << niter
                 << " " << scientific << setw(8) << setprecision(2) << conv;
         
         for (i=0; i<Sats.size(); i++)
            C.oflog << " " << setw(3) << Sats[i].id;
         
         C.oflog << endl;
         
         //Compute residuals using known position, and output
         if (iret==0 && C.knownpos.getCoordinateSystem() != Position::Unknown)
         {
            Matrix<double> Cov;
            Vector<double> V(3);
            
            //Compute position residuals using known position
            Position pos(Solution(0), Solution(1), Solution(2));
            Position res = pos - C.knownpos;
            Cov = Matrix<double>(Covariance,0,0,3,3);
            V(0) = res.X();
            V(1) = res.Y();
            V(2) = res.Z();
            
            C.oflog << "APR " << setw(2) << iret
                    << " " << (static_cast<CivilTime>(CurrEpoch)).printf(C.timeFormat)
                    << " " << setw(2) << Nsvs << fixed
                    << " " << setw(16) << setprecision(6) << V(0)
                    << " " << setw(16) << setprecision(6) << V(1)
                    << " " << setw(16) << setprecision(6) << V(2)
                    << " " << setw(14) << setprecision(6) << Solution(3)
                    << " " << setw(12) << setprecision(6) << RMS(Residual)
                    << " " << fixed << setw(5) << setprecision(1) << max(Slope)
                    << " " << niter
                    << " " << scientific << setw(8) << setprecision(2) << conv;
            
            for (i=0; i<Sats.size(); i++)
               C.oflog << " " << setw(3) << Sats[i].id;
            
            C.oflog << endl;
            
            //Accumulate statistics
            SAPR[0].Add(V(0)); SAPR[1].Add(V(1)); SAPR[2].Add(V(2));
            SSAPR[0].Add(V(0)); SSAPR[1].Add(V(1)); SSAPR[2].Add(V(2));
            
            inform = inverseSVD(Cov);
            
            PAPR  += inform;
            PPAPR += inform;
            zAPR  += inform * V;
            zzAPR += inform * V;
            
            //Convert to NEU
            V = C.Rot * V;
            Cov = C.Rot * Cov * transpose(C.Rot);
            
            C.oflog << "ANE " << setw(2) << iret
                    << " " << (static_cast<CivilTime>(CurrEpoch)).printf(C.timeFormat)
                    << " " << setw(2) << Nsvs << fixed
                    << " " << setw(16) << setprecision(6) << V(0)
                    << " " << setw(16) << setprecision(6) << V(1)
                    << " " << setw(16) << setprecision(6) << V(2)
                    << " " << setw(14) << setprecision(6) << Solution(3)
                    << " " << setw(12) << setprecision(6) << RMS(Residual)
                    << " " << fixed << setw(5) << setprecision(1) << max(Slope)
                    << " " << niter
                    << " " << scientific << setw(8) << setprecision(2) << conv;
            
            for (i=0; i<Sats.size(); i++)
               C.oflog << " " << setw(3) << Sats[i].id;
            
            C.oflog << endl;
            
            //Accumulate statistis
            SANE[0].Add(V(0)); SANE[1].Add(V(1)); SANE[2].Add(V(2));
            SSANE[0].Add(V(0)); SSANE[1].Add(V(1)); SSANE[2].Add(V(2));
            
            inform = inverseSVD(Cov);
            
            PANE  += inform;
            PPANE += inform;
            zANE  += inform * V;
            zzANE += inform * V;
            
         }  //End output residuals

      }  //End output APS

      //------------------------------------------------------------------------
      ///Check RAIMCompute()
      //Now compute again, using RAIM
      CurrEpoch.setTimeSystem(TimeSystem::Any);
      #ifdef DEBUG_PRINT_TIME
         cout << CurrEpoch << endl;
      #endif
      ///Here...
      iret = prsol.RAIMCompute(CurrEpoch, Sats, PRanges, *pEph, C.pTropModel);

      for (Nsvs=0,i=0; i<Sats.size(); i++)
      {
         if (Sats[i].id > 0)
         {
            #ifdef DEBUG_PRINT_SATS_USED
            cout << "Using Sat " << Sats[i] << "\n";
            #endif
            Nsvs++;
         }
      }
      RMSresid = prsol.RMSResidual;

      //Output
      C.oflog << "RPF " << setw(2) << Sats.size()-Nsvs
              << " " << (static_cast<CivilTime>(CurrEpoch)).printf(C.timeFormat)
              << " " << setw(2) << Nsvs << fixed
              << " " << setw(16) << setprecision(6) << prsol.Solution(0)
              << " " << setw(16) << setprecision(6) << prsol.Solution(1)
              << " " << setw(16) << setprecision(6) << prsol.Solution(2)
              << " " << setw(14) << setprecision(6) << prsol.Solution(3);
      if (prsol.Solution.size() == 5)
         C.oflog << " " << setw(16) << setprecision(6) << prsol.Solution(4);
      C.oflog << " " << setw(12) << setprecision(6) << prsol.RMSResidual
              << " " << setw(5) << setprecision(1) << prsol.MaxSlope
              << " " << prsol.NIterations
              << " " << scientific << setw(8) << setprecision(2)
              << prsol.Convergence;
      
      for (i=0; i<Sats.size(); i++)
         C.oflog << " " << setw(3) << Sats[i].id;
      
      C.oflog << " (" << iret;
      if (C.Verbose)
      {
         C.oflog << "PRS returned " << iret << " at "
                 << static_cast<CivilTime>(CurrEpoch).printf(C.timeFormat)
                 << ", meaning ";
         if (iret==2) C.oflog
            << " solution is found, but it is not good (RMS residual exceed limits)";
         if (iret==1) C.oflog
            << " solution is found, but it is suspect (slope is large)";
         if (iret==0) C.oflog << " ok";
         if (iret==-1) C.oflog
            << " algorithm failed to converge";
         if (iret==-2) C.oflog
            << " singular problem, no solution is possible";
         if (iret==-3) C.oflog
            << " not enough good data, < 5 sats, 4-sat sol is ok if V at EOL";
         if (iret==-4) C.oflog
            << " failed to find any ephemeris";
      }
      C.oflog << ")" << (prsol.isValid() ? " V" : " NV") << endl;
      
      //Compute residuals using known position, and output
      if (C.knownpos.getCoordinateSystem() != Position::Unknown && iret >= 0)
      {
         Matrix<double> Cov;
         Vector<double> V(3);
         
         //Compute residuals
         Position pos(prsol.Solution(0), prsol.Solution(1), prsol.Solution(2));
         Position res = pos - C.knownpos;
         Cov = Matrix<double>(prsol.Covariance,0,0,3,3);
         V(0) = res.X(); V(1) = res.Y(); V(2) = res.Z();
         
         {   //C.oflog stuff
         C.oflog << "RPR " << setw(2) << Sats.size()-Nsvs
                 << " " << (static_cast<CivilTime>(CurrEpoch)).printf(C.timeFormat)
                 << " " << setw(2) << Nsvs << fixed
                 << " " << setw(16) << setprecision(6) << V(0)
                 << " " << setw(16) << setprecision(6) << V(1)
                 << " " << setw(16) << setprecision(6) << V(2)
                 << " " << setw(14) << setprecision(6) << prsol.Solution(3)
                 << " " << setw(12) << setprecision(6) << prsol.RMSResidual
                 << " " << fixed << setw(5) << setprecision(1) << prsol.MaxSlope
                 << " " << prsol.NIterations
                 << " " << scientific << setw(8) << setprecision(2)
                 << prsol.Convergence;
         
         for (i=0; i<Sats.size(); i++)
            C.oflog << " " << setw(3) << Sats[i].id;
         
         C.oflog << " (" << iret << ")" << (prsol.isValid() ? " V" : " NV")
                 << endl;
         }
         
         //Accumulate statistics
         SRPR[0].Add(V(0)); SRPR[1].Add(V(1)); SRPR[2].Add(V(2));
         SSRPR[0].Add(V(0)); SSRPR[1].Add(V(1)); SSRPR[2].Add(V(2));
         
         inform = inverseSVD(Cov);
         
         PRPR  += inform;
         PPRPR += inform;
         zRPR  += inform * V;
         zzRPR += inform * V;

         //Convert to NEU
         V = C.Rot * V;
         Cov = C.Rot * Cov * transpose(C.Rot);

         C.oflog << "RNE " << setw(2) << Sats.size()-Nsvs
                 << " " << (static_cast<CivilTime>(CurrEpoch)).printf(C.timeFormat)
                 << " " << setw(2) << Nsvs << fixed
                 << " " << setw(16) << setprecision(6) << V(0)
                 << " " << setw(16) << setprecision(6) << V(1)
                 << " " << setw(16) << setprecision(6) << V(2)
                 << " " << setw(14) << setprecision(6) << prsol.Solution(3)
                 << " " << setw(12) << setprecision(6) << prsol.RMSResidual
                 << " " << fixed << setw(5) << setprecision(1) << prsol.MaxSlope
                 << " " << prsol.NIterations
                 << " " << scientific << setw(8) << setprecision(2)
                 << prsol.Convergence;
         
         for (i=0; i<Sats.size(); i++)
            C.oflog << " " << setw(3) << Sats[i].id;
         
         C.oflog << " (" << iret << ")" << (prsol.isValid() ? " V" : " NV")
                 << endl;

         //Accumulate statistics
         if (iret == 0)
         {
            SRNE[0].Add(V(0)); SRNE[1].Add(V(1)); SRNE[2].Add(V(2));
            SSRNE[0].Add(V(0)); SSRNE[1].Add(V(1)); SSRNE[2].Add(V(2));
            
            inform = inverseSVD(Cov);
            
            PRNE  += inform;
            PPRNE += inform;
            zRNE  += inform * V;
            zzRNE += inform * V;
         }
      }
      
      ///Output data here:
      if(prsol.Solution.size() == 5)
      {
         long day;
         long sod;
         double fraction;
         CurrEpoch.get(day, sod, fraction);
         cout << sod << " " << ( prsol.Solution(4) / C_GPS_M * 1.0e9 ) << endl;
      }
      
      if (prsol.isValid() && !C.OutRinexObs.empty())
      {
         #ifdef DEBUG_PRINT_WARNINGS
         cout << "prsol is valid, but C.OutRinexObs is not empty?" << endl;
         cout << "Line: " << __LINE__ << "\n";
         #endif
         return 3;
      }
      if (!prsol.isValid())
      {
      	#ifdef DEBUG_PRINT_WARNINGS
      	cout << "prsol is not valid" << endl;
      	cout << "Line: " << __LINE__ << "\n";
      	#endif
         return 1;
      }
      if (!C.OutRinexObs.empty())
      {
         #ifdef DEBUG_PRINT_WARNINGS
         cout << "C.OutRinexObs is not empty???" << endl;
         cout << "Line: " << __LINE__ << "\n";
         #endif
         return 2;
      }
      return 0;
   }
   catch(Exception& gpstkException)
   {
      GPSTK_RETHROW(gpstkException);
   }
   catch(exception& stdException)
   {
      Exception gpstkException("std except: "+string(stdException.what()));
      GPSTK_THROW(gpstkException);
   }
   catch(...)
   {
      Exception e("Unknown exception");
      GPSTK_THROW(e);
   }
   return -1;
}

//------------------------------------------------------------------------------------
int afterReadingFiles(void) throw(Exception)
{
   try
   {
         //Only print stats on all files if there is more than one
      if (C.APSout)
      {
         printStats(SSA,PPA,zzA,nSS,"Autonomous solution for all files");
         if (C.knownpos.getCoordinateSystem() != Position::Unknown)
         {
            printStats(SSAPR,PPAPR,zzAPR,nSS,
               "Autonomous position residuals for all files");
            printStats(SSANE,PPANE,zzANE,nSS,
               "Autonomous position residuals (NEU) for all files",'N','E','U');
         }
      }

//      cout << SSR[0].N() << "  " << SSR[3] << endl;
      printStats(SSR,PPR,zzR,nSS,"RAIM solution for all files");
      if (C.knownpos.getCoordinateSystem() != Position::Unknown)
      {
         printStats(SSRPR,PPRPR,zzRPR,nSS,"RAIM position residuals for all files");
         printStats(SSRNE,PPRNE,zzRNE,nSS,"RAIM position residuals (NEU) for all files",
            'N','E','U');
      }

      //Print to screen
      cout << "\nWeighted average RAIM solution for file: "
           << (C.InputObsName.size() > 1 ? "all files" : C.InputObsName[0])
           << endl << fixed;
      cout << " (" << nSS << " total epochs, with "
           << SSR[0].N() << " good, " << nSS-SSR[0].N() << " rejected.)\n";
      if (SSR[0].N() > 0)
      {
         Matrix<double> Cov=inverse(PPR);
         Vector<double> Sol = Cov * zzR;
         cout << setw(16) << setprecision(6) << Sol << endl;
         cout << "Covariance of RAIM solution for file: "
              << (C.InputObsName.size() > 1 ? "all files" : C.InputObsName[0])
              << endl;
         cout << setw(16) << setprecision(6) << Cov << endl;
      }
      else cout << " No data!" << endl;

         // compute data interval for this file
      int i,j;
      double dt;
      for (j=0,i=1; i<9; i++)
      {
         if (C.ndt[i]>C.ndt[j])
            j=i;
      }
      C.oflog << endl;
      C.oflog << "Estimated data interval is " << C.estdt[j] << " seconds.\n";
      C.oflog << "First epoch is "
              << (static_cast<CivilTime>(C.FirstEpoch)).printf("%02m/%02d/%04Y %02H:%02M:%02S %P") << endl;
      C.oflog << "Last  epoch is "
              << (static_cast<CivilTime>(C.LastEpoch)).printf("%02m/%02d/%04Y %02H:%02M:%02S %P") << endl;

      return 0;
   }
   catch(Exception& gpstkException)
   {
      GPSTK_RETHROW(gpstkException);
   }
   catch(exception& stdException)
   {
      Exception E("std except: "+string(stdException.what()));
      GPSTK_THROW(E);
   }
   catch(...)
   {
      Exception e("Unknown exception");
      GPSTK_THROW(e);
   }
   return -1;
}

//------------------------------------------------------------------------------
void printStats( Stats<double> S[], Matrix<double> &P, Vector<double> &z,
                 long ng, string msg, char c0, char c1, char c2 )
   throw(Exception)
{
   try
   {
      C.oflog << endl;
      C.oflog << "Simple statistics on " << msg << endl << fixed;
      C.oflog << c0  << " : " << setw(16) << setprecision(6) << S[0] << endl;
      C.oflog << c1  << " : " << setw(16) << setprecision(6) << S[1] << endl;
      C.oflog << c2  << " : " << setw(16) << setprecision(6) << S[2] << endl;
      C.oflog << "T" << " : " << setw(16) << setprecision(6) << S[3] << endl;

      C.oflog << "\nWeighted average " << msg << endl << fixed;
      if (S[0].N() > 0)
      {
         Matrix<double> Cov = inverse(P);
         Vector<double> Sol = Cov * z;
         C.oflog << setw(16) << setprecision(6) << Sol << "    " << S[0].N() << endl;
         C.oflog << "Covariance of " << msg << endl;
         C.oflog << setw(16) << setprecision(6) << Cov << endl;
      }
      else C.oflog << " No data!" << endl;
   }
   catch(Exception& e)
   {
      GPSTK_RETHROW(e);
   }
   catch(exception& e)
   {
      Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
   catch(...)
   {
      Exception e("Unknown exception");
      GPSTK_THROW(e);
   }
}

//------------------------------------------------------------------------------------
void setWeather(CommonTime& time, TropModel *pTropModel)
{
   static list<RinexMetData>::iterator it=C.MetStore.begin();
   static list<RinexMetData>::iterator nextit;
   static CommonTime currentTime = CommonTime::BEGINNING_OF_TIME;
   double dt;

   while(it != C.MetStore.end())
   {
      //Point to the next epoch after the current epoch
      nextit = it;
      nextit++;
      
      //Is the current epoch (it->time) the right one?
      //if (time is before next but after current - just right) ||
      //  (there is no next, but time is within 15 minutes of the current epoch)
      if ((nextit != C.MetStore.end() && time < nextit->time && time >= it->time)
         || (nextit == C.MetStore.end() && (dt=time-it->time) >= 0.0 && dt < 900.0))
      {
         // set the weather - replace default with current value, if it exists
         // but skip if it has already been done
         if (it->time == currentTime)
            break;
         currentTime = it->time;

         if (C.Debug)
            C.oflog << "Reset weather at " << time << " to " << it->time
                    << " " << it->data[RinexMetHeader::TD]
                    << " " << it->data[RinexMetHeader::PR]
                    << " " << it->data[RinexMetHeader::HR] << endl;

         //[if 'it' is declared const_iterator, why does this discard qualifier??]
         if (it->data.count(RinexMetHeader::TD) > 0)
            C.defaultT = it->data[RinexMetHeader::TD];
         if (it->data.count(RinexMetHeader::PR) > 0)
            C.defaultPr = it->data[RinexMetHeader::PR];
         if (it->data.count(RinexMetHeader::HR) > 0)
            C.defaultRH = it->data[RinexMetHeader::HR];

         pTropModel->setWeather(C.defaultT, C.defaultPr, C.defaultRH);

         break;
      }

      //No, this is not the right epoch; but should we increment the iterator?
      else if (nextit != C.MetStore.end() && time >= nextit->time)
      {     //Yes, time is at or beyond the next epoch
         it++;
      }

      //Do nothing, because time is before the next epoch
      else break;
   }
}

//------------------------------------------------------------------------------------
void dumpConfiguration(ostream& os) throw(Exception)
{
   try
   {
      int i;
         // print config to log
      os << "\nHere is the PRSolve configuration:\n";
      os << " # Input:\n";
      os << " Obs directory is '" << C.ObsDirectory << "'" << endl;
      os << " RINEX observation files are:\n";
      for (i=0; i<C.InputObsName.size(); i++)
      {
         os << "   " << C.InputObsName[i] << endl;
      }
      os << " Nav directory is '" << C.NavDirectory << "'" << endl;
      os << " navigation files are:\n";
      for (i=0; i<C.InputNavName.size(); i++)
      {
         os << "   " << C.InputNavName[i] << endl;
      }
      if (C.InputMetName.size() > 0)
      {
         os << " Met directory is '" << C.MetDirectory << "'" << endl;
         os << " RINEX meteorological files are:\n";
         for (i=0; i<C.InputMetName.size(); i++)
         {
            os << "   " << C.InputMetName[i] << endl;
         }
      }
      else os << " No input meteorological data\n";
      os << " Ithing time interval is " << C.ith << endl;
      if (C.Tbeg > CommonTime::BEGINNING_OF_TIME)
         os << " Begin time is "
            << (static_cast<CivilTime>(C.Tbeg)).printf("%02m/%02d/%04Y %02H:%02M:%02S %P")
            << " = " << C.Tbeg << endl;
      if (C.Tend < CommonTime::END_OF_TIME)
         os << " End time is "
            << (static_cast<CivilTime>(C.Tend)).printf("%02m/%02d/%04Y %02H:%02M:%02S %P")
            << " = " << C.Tend << endl;
      if (C.UseCA) os << " 'Use C/A' flag is set\n";
      if (C.ForceCA) os << " 'Force C/A' flag is set\n";

      os << " # Configuration:\n";
      os << " Process frequency L" << C.Freq;
      if (C.Freq == 3) os << ", which is the ionosphere-free combination of L1 and L2";
      os << "." << endl;
      os << " Minimum elevation angle is " << C.elevLimit << " degrees." << endl;
      if (C.ExSV.size())
      {
         RinexSatID p;
         p.setfill('0');
         os << " Exclude satellites";
         for (i=0; i<C.ExSV.size(); i++)
         {
            p = C.ExSV[i];
            os << " " << p;
         }
         os << endl;
      }
      os << " Trop model: " << C.TropType << " and weather (T,P,RH): "
         << C.defaultT << "," << C.defaultPr << "," << C.defaultRH << endl;
      os << " ------ PRSolution configuration:" << endl;
      os << "  Limit on RMS solution residual (m) = " << prsol.RMSLimit << endl;
      os << "  Limit on RAIM 'slope' = " << prsol.SlopeLimit << endl;
      os << "  Use algebraic algorithm is "
         << (prsol.Algebraic ? "true" : "false") << endl;
      os << "  Residual criterion is "
         << (prsol.ResidualCriterion ? "RMS residuals":"distance from apriori") << endl;
      os << "  Return-at-once option is "
         << (prsol.ReturnAtOnce ? "on" : "off") << endl;
      os << "  Maximum number of rejected satellites is "
         << (prsol.NSatsReject == -1 ? "unlimited" : asString(prsol.NSatsReject))
         << endl;
      os << "  Maximum iterations in linearized least squares (LLS) is "
         << prsol.MaxNIterations << endl;
      os << "  RSS convergence criterion (meters) in LLS is "
         << prsol.ConvergenceLimit << endl;
      os << " ------ End of PRSolution configuration." << endl;

      os << " # Output:\n";
      os << " Log file is " << C.LogFile << endl;
      if (C.knownpos.getCoordinateSystem() != Position::Unknown)
         os << " Output residuals: known position is\n   "
            << C.knownpos.printf("ECEF(m) %.4x %.4y %.4z\n     = %A deg N %L deg E %h m\n");
      if (!C.ordFile.empty())
         os << " Output ORDs to file " << C.ordFile << endl;
      os << " Output tags RPF";
      if (C.knownpos.getCoordinateSystem() != Position::Unknown) os << " RPR RNE";
      if (C.APSout) os << " APS";
      if (C.APSout && C.knownpos.getCoordinateSystem() != Position::Unknown)
         os << " APR ANE";
      os << endl;
      os << " Output format for time tags (cf. class CommonTime) is "
         << C.timeFormat << endl;

      os << " # RINEX output:\n";
      if (!C.OutRinexObs.empty()) os << " Output RINEX file name is "
         << C.OutRinexObs << endl;
      if (!C.HDRunby.empty()) os << " Output RINEX 'RUN BY' is "
         << C.HDRunby << endl;
      if (!C.HDObs.empty()) os << " Output RINEX 'OBSERVER' is "
         << C.HDObs << endl;
      if (!C.HDAgency.empty()) os << " Output RINEX 'AGENCY' is "
         << C.HDAgency << endl;
      if (!C.HDMarker.empty()) os << " Output RINEX 'MARKER' is "
         << C.HDMarker << endl;
      if (!C.HDNumber.empty()) os << " Output RINEX 'NUMBER' is "
         << C.HDNumber << endl;

      os << "End of PRSolve configuration summary" << endl << endl;

   }
   catch(Exception& e)
   {
      GPSTK_RETHROW(e);
   }
   catch(exception& e)
   {
      Exception E("std except: "+string(e.what( )));
      GPSTK_THROW(E);
   }
   catch(...)
   {
      Exception e("Unknown exception");
      GPSTK_THROW(e);
   }
}

//------------------------------------------------------------------------------------
// Pull out --verbose -f<f> and --file <f> options
void preprocessArgs(const char *arg, vector<string>& Args)
   throw(Exception)
{
   try
   {
      static bool found_cfg_file=false;

      if (found_cfg_file || (arg[0 ]== '-' && arg[1] == 'f'))
      {
         string filename(arg);
         
         if (!found_cfg_file)
            filename.erase(0,2);
         else
            found_cfg_file = false;
         
         ifstream infile(filename.c_str());
         if (!infile)
         {
            cout << "Error: could not open options file " << filename << endl;
            return;
         }

         bool again_cfg_file=false;
         char c;
         string buffer,word;
         
         while(1)
         {
            getline(infile,buffer);
            stripTrailing(buffer,'\r');

            // process the buffer before checking eof or bad b/c there can be
            // a line at EOF that has no CRLF...
            while(!buffer.empty())
            {
               word = firstWord(buffer);
               if (again_cfg_file)
               {
                  word = "-f" + word;
                  again_cfg_file = false;
                  preprocessArgs(word.c_str(),Args);
               }
               else if (word[0] == '#')
               { // skip to end of line
                  buffer = "";
               }
               else if (word == "--file" || word == "-f")
                  again_cfg_file = true;
               else if (word[0] == '"')
               {
                  word = stripFirstWord(buffer,'"');
                  buffer = "dummy " + buffer;            // to be stripped later
                  preprocessArgs(word.c_str(),Args);
               }
               else
                  preprocessArgs(word.c_str(),Args);

               word = stripFirstWord(buffer);      // now remove it from buffer
            }
            if (infile.eof() || !infile.good())
               break;
         }
      }
      else if ((arg[0]=='-' && arg[1]=='v') || string(arg)==string("--verbose"))
      {
         C.Verbose = true;
         cout << "Found the verbose switch" << endl;
      }
      else if (string(arg) == "--file" || string(arg) == "-f")
         found_cfg_file = true;
      // deprecated args
      else if (string(arg)==string("--EpochBeg"))
      { 
         Args.push_back("--BeginTime");
      }
      else if (string(arg)==string("--GPSBeg"))
      { 
         Args.push_back("--BeginTime");
      }
      else if (string(arg)==string("--EpochEnd"))
      {
         Args.push_back("--EndTime");
      }
      else if (string(arg)==string("--GPSEnd"))
      {
         Args.push_back("--EndTime");
      }
      else if (string(arg)==string("--RinexFile"))
      {
         Args.push_back("--outRinex");
      }
      else if (string(arg)==string("--XPRN"))
      {
         Args.push_back("--exSat");
      }
      // regular arg
      else Args.push_back(arg);
   }
   catch(Exception& e)
   {
      GPSTK_RETHROW(e);
   }
   catch(exception& e)
   {
      Exception E("std except: "+string(e.what()));
      GPSTK_THROW(E);
   }
   catch(...)
   {
      Exception e("Unknown exception");
      GPSTK_THROW(e);
   }
}

//------------------------------------------------------------------------------------
// TD found in src/RinexUtilities

bool isSP3File(const string& file)
{
   SP3Header header;
   SP3Stream strm(file.c_str());
   strm.exceptions(fstream::failbit);
   try
   {
      strm >> header;
   }
   catch(gpstk::Exception& e)
   {
      return false;
   }
   strm.close();
   return true;
}

bool isRinexNavFile(const string& file)
{
   Rinex3NavHeader header;
   Rinex3NavStream rnstream(file.c_str());
   rnstream.exceptions(fstream::failbit);
   try
   {
      rnstream >> header;
   }
   catch(gpstk::Exception& e)
   {
      return false;
   }
   rnstream.close();
   return true;
}

int fillEphemerisStore(const vector<string>& files,
                        SP3EphemerisStore& PE,
                        GPSEphemerisStore& BCE)
   throw(Exception)
{
   try
   {
      int nread=0;
      Rinex3NavHeader rnh;
      Rinex3NavData rne;
      for (int i=0; i<files.size(); i++)
      {
         if (files[i].empty()) throw Exception("File name is empty");
         Rinex3NavStream strm(files[i].c_str());
         if (!strm) throw Exception("Could not open file " + files[i]);
         strm.close();
         if (isRinexNavFile(files[i]))
         {
            Rinex3NavStream RNFileIn(files[i].c_str());
            RNFileIn.exceptions(fstream::failbit);
            try
            {
               RNFileIn >> rnh;
               while (RNFileIn >> rne)
               {
                  if (rne.health == 0)
                     BCE.addEphemeris(rne);
               }
               nread++;
            }
            catch(gpstk::Exception& e)
            {
               cerr << "Caught Exception while reading RINEX Nav file " << files[i]
                    << " : " << e << endl;
               continue;
            }
         }
         else if (isSP3File(files[i]))
         {
            try
            {/// /// ///
               PE.loadFile(files[i]);
            }/// /// ///
            catch(gpstk::Exception& e)
            {
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
   catch(Exception& e)
   {
      GPSTK_RETHROW(e);
   }
   catch(exception& e)
   {
      Exception E("std except: "+string(e.what()));
      GPSTK_THROW(E);
   }
   catch(...)
   {
      Exception e("Unknown exception");
      GPSTK_THROW(e);
   }
   return -1;
}

//------------------------------------------------------------------------------------
int getCommandLine(int argc, char **argv) throw(Exception)
{
   try {
      bool ok,help=false;
      int i,j;
         //Defaults
      C.Debug = C.Verbose = false;
      C.ith = 0.0;
      C.Tbeg = C.FirstEpoch = CommonTime::BEGINNING_OF_TIME;
      C.Tend = CommonTime::END_OF_TIME;
      
      C.allSystems = true;
      C.useGPS = C.useGlonass = false;
      
         //Configuration of PRSolution
      C.rmsLimit = prsol.RMSLimit;
      C.SlopeLimit = prsol.SlopeLimit;
      C.algebra = prsol.Algebraic;
      C.residCrit = prsol.ResidualCriterion;
      C.returnatonce = prsol.ReturnAtOnce;
      C.maxReject = prsol.NSatsReject;
      C.nIter = prsol.MaxNIterations;
      C.convLimit = prsol.ConvergenceLimit;

      C.Freq = 3;
      C.elevLimit = 0.0;

      C.LogFile = string("prs.log");
      C.ordFile = string();

      C.APSout = false;
      C.UseCA = false;
      C.ForceCA = false;
      C.DataInt = -1.0;
      C.TropType = string("BL");
      C.defaultT = 20.0;
      C.defaultPr = 980.0;
      C.defaultRH = 50.0;
      
      C.HDPrgm = PrgmName + string(" v.") + PrgmVers.substr(0,4);
      C.HDRunby = string("GPSTk");

      C.timeFormat = string("%4F %10.3g");

      for (i=0; i<9; i++)
         C.ndt[i]=-1;

      C.ObsDirectory = string("");
      C.NavDirectory = string("");
      C.MetDirectory = string("");

         // -------------------------------------------------
         // -------------------------------------------------
         // required options
      RequiredOption dashi(CommandOption::hasArgument, CommandOption::stdType,
         'o',"obs"," [-o|--obs] <file>    Input RINEX observation file(s)");

      RequiredOption dashn(CommandOption::hasArgument, CommandOption::stdType,
         'n',"nav"," [-n|--nav] <file>    Input navigation file(s) [RINEX or SP3]");

         // optional options
      // this only so it will show up in help page...
      CommandOption dashf(CommandOption::hasArgument, CommandOption::stdType,
         'f',"","# Input:\n [-f|--file] <file>   File containing more options ()");

      CommandOption dashdo(CommandOption::hasArgument, CommandOption::stdType,
         0,"obsdir",
         " --obsdir <dir>       Directory of input RINEX observation file(s) (.)");
      dashdo.setMaxCount(1);

      CommandOption dashdn(CommandOption::hasArgument, CommandOption::stdType,
         0,"navdir"," --navdir <dir>       Directory of input navigation file(s) (.)");
      dashdn.setMaxCount(1);

      CommandOption dashdm(CommandOption::hasArgument, CommandOption::stdType,
         0,"metdir",
         " --metdir <dir>       Directory of input RINEX meteorological file(s) (.)");
      dashdm.setMaxCount(1);

      CommandOption dashm(CommandOption::hasArgument, CommandOption::stdType,
         'm',"met"," [-m|--met] <file>    Input RINEX meteorological file(s) ()");

      CommandOption dashith(CommandOption::hasArgument, CommandOption::stdType,
         0,"decimate"," --decimate <dt>      Decimate data to time interval dt ()");
      dashith.setMaxCount(1);

      // time
      // times - don't use CommandOptionWithTimeArg
      CommandOption dashbt(CommandOption::hasArgument, CommandOption::stdType,
         0,"BeginTime", " --BeginTime <arg>    Start time: arg is "
         "'GPSweek,sow' OR 'YYYY,MM,DD,HH,Min,Sec' ()");
      dashbt.setMaxCount(1);

      CommandOption dashet(CommandOption::hasArgument, CommandOption::stdType,
         0,"EndTime", " --EndTime <arg>      End time: arg is 'GPSweek,sow' OR "
         "'YYYY,MM,DD,HH,Min,Sec' ()");
      dashet.setMaxCount(1);

      CommandOptionNoArg dashCA(0,"useCA", string(" --useCA              ")
         + string("Use C/A code pseudorange if P1 is not available (")
         + (C.UseCA ? "true" : "false") + ")");
      dashCA.setMaxCount(1);
      
      CommandOptionNoArg dashfCA(0,"forceCA", string(" --forceCA            ")
         + string("Use C/A code pseudorange regardless of P1 availability (")
         + (C.ForceCA ? "true" : "false") + ")");
      dashfCA.setMaxCount(1);
      
      // --------------------------------------------------------------------------------

      CommandOption dashFreq(CommandOption::hasArgument, CommandOption::stdType,
         0,"Freq", "# Configuration:\n"
         " --Freq <f>           Frequency to process: 1, 2 or 3 for L1, L2 or "
         "iono-free combo (" + asString(C.Freq) + ")");
      dashFreq.setMaxCount(1);

      CommandOption dashElev(CommandOption::hasArgument, CommandOption::stdType,
         0,"MinElev",
         " --MinElev <el>       Minimum elevation angle (deg) [only if --PosXYZ] ("
         + asString(C.elevLimit,2) + ")");
      dashElev.setMaxCount(1);

      CommandOption dashXsat(CommandOption::hasArgument, CommandOption::stdType,
         0,"exSat"," --exSat <sat>        Exclude this satellite ()");

      CommandOption dashTrop(CommandOption::hasArgument, CommandOption::stdType,0,"Trop",
         " --Trop <model,T,P,H> Trop model [one of ZR,BL,SA,NB,GG,GGH "
         "(cf. gpstk::TropModel)],\n                        with optional "
         "weather [T(C),P(mb),RH(%)] ("
         + C.TropType + "," + asString(C.defaultT,0)
         + "," + asString(C.defaultPr,0) + "," + asString(C.defaultRH,0) + ")");
      dashTrop.setMaxCount(1);

      CommandOption dashrms(CommandOption::hasArgument, CommandOption::stdType,
         0,"RMSlimit", "# PRSolution configuration:\n --RMSlimit <rms>     "
         "Upper limit on RMS post-fit residuals (m) ("
         + asString(prsol.RMSLimit,2) + ")");
      dashrms.setMaxCount(1);

      CommandOption dashslop(CommandOption::hasArgument, CommandOption::stdType,
         0,"SlopeLimit",
         " --SlopeLimit <s>     Upper limit on RAIM 'slope' ("
         + asString(int(prsol.SlopeLimit)) + ")");
      dashslop.setMaxCount(1);

      CommandOptionNoArg dashAlge(0,"Algebra",
         string(" --Algebra            ")
         + string("Use algebraic algorithm, else linearized least squares ()"));
      dashAlge.setMaxCount(1);

      CommandOptionNoArg dashrcrt(0,"DistanceCriterion", string(" --DistanceCriterion  ")
       +string("Use distance from given position (--PosXYZ) as convergence\n")
       +string("                         criterion, else RMS residual-of-fit ()"));
      dashrcrt.setMaxCount(1);

      CommandOptionNoArg dashrone(0,"ReturnAtOnce",string(" --ReturnAtOnce       ")
        +string("Return as soon as a good solution is found (")
        +(prsol.ReturnAtOnce ? string("true") : string("false")) + string(")") );
      dashrone.setMaxCount(1);

      CommandOption dashnrej(CommandOption::hasArgument, CommandOption::stdType,
         0,"NReject", " --NReject <n>        Maximum number of satellites to reject ("
         + (prsol.NSatsReject == -1 ? "no limit" : asString(prsol.NSatsReject))
         + ")");
      dashnrej.setMaxCount(1);

      CommandOption dashNit(CommandOption::hasArgument, CommandOption::stdType,0,"NIter",
         " --NIter <n>          Maximum iteration count in linearized LS ("
         + asString(prsol.MaxNIterations) + ")");
      dashNit.setMaxCount(1);

      CommandOption dashConv(CommandOption::hasArgument, CommandOption::stdType,0,"Conv",
         " --Conv <c>           "
         "Minimum convergence criterion in estimation ("
         + doub2sci(prsol.ConvergenceLimit,8,2,false) + ")");
      dashConv.setMaxCount(1);

      // --------------------------------------------------------------------------------

      CommandOption dashLog(CommandOption::hasArgument, CommandOption::stdType,
         0,"Log","# Output:\n --Log <file>         Output log file name ("
         + C.LogFile + ")");
      dashLog.setMaxCount(1);
      
      CommandOption dashXYZ(CommandOption::hasArgument, CommandOption::stdType,
         0,"PosXYZ", " --PosXYZ <X,Y,Z>     "
         "Known position (ECEF,m), for computing residuals and ORDs ()");
      dashXYZ.setMaxCount(1);
      
      CommandOptionNoArg dashAPSout(0,"APSout", string(" --APSout             ")
         + string("Output autonomous pseudorange solution [tag APS, no RAIM] ()"));
      dashAPSout.setMaxCount(1);

      CommandOption dashORDs(CommandOption::hasArgument, CommandOption::stdType,
         0,"ORDs", " --ORDs <file>        "
         "ORDs (Observed Range Deviations) output file [PosXYZ req'd] ("
         + C.ordFile + ")");
      dashORDs.setMaxCount(1);

      CommandOption dashForm(CommandOption::hasArgument, CommandOption::stdType,
         0,"TimeFormat", " --TimeFormat <fmt>   "
         "Format for time tags in output (cf gpstk::CommonTime) (" + C.timeFormat + ")");
      dashForm.setMaxCount(1);

      CommandOption dashRfile(CommandOption::hasArgument, CommandOption::stdType,
         0,"outRinex","# RINEX output:\n"
         " --outRinex <file>    Output RINEX observation file name ()");
      dashRfile.setMaxCount(1);
      
      CommandOption dashRrun(CommandOption::hasArgument, CommandOption::stdType,
         0,"RunBy"," --RunBy <string>     Output RINEX header 'RUN BY' string ("
         + C.HDRunby + ")");
      dashRrun.setMaxCount(1);
      
      CommandOption dashRobs(CommandOption::hasArgument, CommandOption::stdType,
         0,"Observer"," --Observer <string>  Output RINEX header 'OBSERVER' string ()");
      dashRobs.setMaxCount(1);
      
      CommandOption dashRag(CommandOption::hasArgument, CommandOption::stdType,
         0,"Agency"," --Agency <string>    Output RINEX header 'AGENCY' string ()");
      dashRag.setMaxCount(1);
      
      CommandOption dashRmark(CommandOption::hasArgument, CommandOption::stdType,
         0,"Marker"," --Marker <string>    Output RINEX header 'MARKER' string ()");
      dashRmark.setMaxCount(1);
      
      CommandOption dashRnumb(CommandOption::hasArgument, CommandOption::stdType,
         0,"Number"," --Number <string>    Output RINEX header 'NUMBER' string ()");
      dashRnumb.setMaxCount(1);
      
      CommandOptionNoArg dashVerb(0,"verbose",
         "# Help:\n --verbose            Print extended output (don't)");
      dashVerb.setMaxCount(1);

      CommandOptionNoArg dashDebug(0,"debug",
         " --debug              Print very extended output (don't)");
      dashDebug.setMaxCount(1);

      CommandOptionNoArg dashh('h', "help",
        " [-h|--help]          Print syntax and quit (don't)");
      
      ///System command options
      CommandOptionNoArg dashAllSystems(0,"AllSystems",
         " --AllSystems         Uses all systems that are present in the header.");
      CommandOptionNoArg dashGPS(0,"useGPS",
         " --UseGps             Uses GPS data from the header file.");
      CommandOptionNoArg dashGlonass(0,"useGlonass",
         " --useGlonass         Uses Glonass data from the header file.");

      // ... other options
      CommandOptionRest Rest("");

      CommandOptionParser Par(
      "Prgm PRSolve reads one or more RINEX observation files, plus one or more\n"
      "   navigation (ephemeris) files, and computes an autonomous GPS pseudorange\n"
      "   position solution, using a RAIM-like algorithm to eliminate outliers.\n"
      "   Output is to a log file, and also optionally to a RINEX obs file with\n"
      "   the position solutions in comments in auxiliary header blocks.\n"
      "   In the log file, results appear one epoch per line with the format:\n"
      "   TAG Nrej week sow Nsat X Y Z T RMS slope nit conv sat sat .. (code) [N]V\n"
      "   TAG denotes solution (X Y Z T) type:\n"
      "       RPF  Final RAIM ECEF XYZ solution\n"
      "       RPR  Final RAIM ECEF XYZ solution residuals [only if --PosXYZ given]\n"
      "       RNE  Final RAIM North-East-Up solution residuals [only if --PosXYZ]\n"
      "       APS  Autonomous ECEF XYZ solution [only if --APSout given]\n"
      "       APR  Autonomous ECEF XYZ solution residuals [only if both --APS & --Pos]\n"
      "       ANE  Autonomous North-East-Up solution residuals [only if --APS & --Pos]\n"
      "   and where Nrej = number of rejected sats, (week,sow) = GPS time tag,\n"
      "   Nsat = # sats used, XYZT = position+time solution(or residuals),\n"
      "   RMS = RMS residual of fit, slope = RAIM slope, nit = # of iterations,\n"
      "   conv = convergence factor, 'sat sat ...' lists all sat. PRNs (- : rejected),\n"
      "   code = return value from PRSolution::RAIMCompute(), and NV means NOT valid.\n"
      "   NB. Default values appear in () after optional arguments below.\n"
      );

         // -------------------------------------------------
         // allow user to put all options in a file
         // could also scan for debug here
      vector<string> Args;
      for (j=1; j<argc; j++) preprocessArgs(argv[j],Args);

      if (Args.size()==0)
         Args.push_back(string("-h"));

         // pass the rest
      argc = Args.size()+1;
      char **CArgs=new char*[argc];
      if (!CArgs)
      {
         cout << "Failed to allocate CArgs\n";
         return -1;
      }
      CArgs[0] = argv[0];
      for (j=1; j<argc; j++)
      {
         CArgs[j] = new char[Args[j-1].size()+1];
         if (!CArgs[j])
         {
            cout << "Failed to allocate CArgs[j]\n";
            return -1;
         }
         strcpy(CArgs[j],Args[j-1].c_str());
      }

      Par.parseOptions(argc, CArgs);

         // -------------------------------------------------
      if (dashh.getCount() > 0)
      {
         Par.displayUsage(cout,false);
         help = true;
      }

      if (!help && Par.hasErrors())
      {
         cout << "\nErrors found in command line input:\n";
         Par.dumpErrors(cout);
         cout << "...end of Errors\n\n";
         help = true;
      }
      
         // -------------------------------------------------
         // get values found on command line
      string stemp;
      vector<string> values,field;
         // do help first
      if (dashAllSystems.getCount())
      {
         C.allSystems = true;
      }
      if (dashGPS.getCount())
      {
         C.allSystems = false;
         C.useGPS = true;
      }
      if (dashGlonass.getCount())
      {
         C.allSystems = false;
         C.useGlonass = true;
      }
      
      if (dashh.getCount())
         help=true;
      if (dashDebug.getCount())
         C.Debug=C.Verbose=true;
      if (dashVerb.getCount())
         C.Verbose=true;

      if (dashdo.getCount())
      {
         values = dashdo.getValue();
         C.ObsDirectory = values[0];
         if (help) cout << "Input obs directory is " << C.ObsDirectory << endl;
      }
      if (dashdn.getCount())
      {
         values = dashdn.getValue();
         C.NavDirectory = values[0];
         if (help) cout << "Input nav directory is " << C.NavDirectory << endl;
      }
      if (dashdm.getCount())
      {
         values = dashdm.getValue();
         C.MetDirectory = values[0];
         if (help) cout << "Input met directory is " << C.MetDirectory << endl;
      }
      if (dashi.getCount())
      {
         values = dashi.getValue();
         if (help) cout << "Input RINEX obs files are:\n";
         for (i=0; i<values.size(); i++)
         {
            if (!C.ObsDirectory.empty())
               C.InputObsName.push_back(C.ObsDirectory + string("/") + values[i]);
            else
               C.InputObsName.push_back(values[i]);
            if (help) cout << "   " << C.ObsDirectory + string("/") + values[i] << endl;
         }
      }
      if (dashn.getCount())
      {
         values = dashn.getValue();
         if (help) cout << "Input RINEX nav files are:\n";
         for (i=0; i<values.size(); i++)
         {
            if (!C.NavDirectory.empty())
               C.InputNavName.push_back(C.NavDirectory + string("/") + values[i]);
            else
               C.InputNavName.push_back(values[i]);
            if (help)
               cout << "  " << C.NavDirectory + string("/") + values[i] << endl;
         }
      }
      if (dashm.getCount())
      {
         values = dashm.getValue();
         if (help) cout << "Input RINEX met files are:\n";
         for (i=0; i<values.size(); i++)
         {
            if (!C.MetDirectory.empty())
               C.InputMetName.push_back(C.MetDirectory + string("/") + values[i]);
            else
               C.InputMetName.push_back(values[i]);
            if (help)
               cout << "  " << C.MetDirectory + string("/") + values[i] << endl;
         }
      }

      if (dashith.getCount())
      {
         values = dashith.getValue();
         C.ith = asDouble(values[0]);
         if (help)
            cout << "Ithing values is " << C.ith << endl;
      }
      // times
      // TD put try {} around setToString and catch invalid formats...
      if (dashbt.getCount())
      {
         ok = true;
         values = dashbt.getValue();
         stemp = values[0];
         field.clear();
         while(stemp.size() > 0)
         {
            field.push_back(stripFirstWord(stemp,','));
         }
         if (field.size() == 2)
         {
            try
            {
               GPSWeekSecond wksec(asInt(field[0]),asDouble(field[1]),TimeSystem::GPS); // *****
               C.Tbeg = wksec.convertToCommonTime();
            }
            catch(Exception& e)
            {
               ok=false;
            }
         }
         else if (field.size() == 6)
         {
            try
            {
               CivilTime civtime(asInt(field[0]),asInt(field[1]),asInt(field[2]),
                                 asInt(field[3]),asInt(field[4]),asDouble(field[5]),
                                 TimeSystem::GPS); // *****
               C.Tbeg = civtime.convertToCommonTime();
            }
            catch(Exception& e)
            {
               ok=false;
            }
         }
         else { ok = false; }
         if (!ok)
         {
            cerr << "Error: invalid --BeginTime input: " << values[0] << endl;
         }
         else if (help)
            cout << " Input: begin time " << values[0] << " = "
                 << (static_cast<CivilTime>(C.Tbeg)).printf("%02m/%02d/%04Y %02H:%02M:%02S %P")
                 << endl;
      }
      if (dashet.getCount())
      {
         ok = true;
         values = dashet.getValue();
         field.clear();
         stemp = values[0];
         while(stemp.size() > 0)
         {
            field.push_back(stripFirstWord(stemp,','));
         }
         if (field.size() == 2)
         {
            try
            {
               GPSWeekSecond wksec(asInt(field[0]),asDouble(field[1]),TimeSystem::GPS); // *****
               C.Tend = wksec.convertToCommonTime();
            }
            catch(Exception& e)
            {
               ok=false;
            }
         }
         else if (field.size() == 6) 
         {
            try 
            {
               CivilTime civtime(asInt(field[0]),asInt(field[1]),asInt(field[2]),
                                 asInt(field[3]),asInt(field[4]),asDouble(field[5]),
                                 TimeSystem::GPS); // *****
               C.Tend = civtime.convertToCommonTime();
            }
            catch(Exception& e)
            {
               ok=false;
            }
         }
         else
         {
            ok = false;
         }
         if (!ok)
         {
            cerr << "Error: invalid --EndTime input: " << values[0] << endl;
         }
         else if (help)
            cout << " Input: end time " << values[0] << " = "
                 << (static_cast<CivilTime>(C.Tend)).printf("%02m/%02d/%04Y %02H:%02M:%02S %P") << endl;
      }
      if (dashCA.getCount())
      {
         C.UseCA = true;
         if (help) cout << "'Use C/A' flag is set\n";
      }
      if (dashfCA.getCount())
      {
         C.ForceCA = true;
         /**if (help)**/ cout << "'Force C/A' flag is set\n";
      }

      if (dashrms.getCount())
      {
         values = dashrms.getValue();
         C.rmsLimit = asDouble(values[0]);
         if (help) cout << "RMS limit is set to " << C.rmsLimit << endl;
      }
      if (dashslop.getCount())
      {
         values = dashslop.getValue();
         C.SlopeLimit = asDouble(values[0]);
         if (help) cout << "Slope limit is set to " << C.SlopeLimit << endl;
      }
      if (dashAlge.getCount())
      {
         C.algebra = true;
         if (help) cout << "'Algebraic' option is on\n";
      }
      if (dashrcrt.getCount())
      {
         C.residCrit = false;
         if (help) cout << "'ResidualCriterion' option is false\n";
      }
      if (dashrone.getCount())
      {
         C.returnatonce = true;
         if (help) cout << "'Return at once' option is true\n";
      }
      if (dashnrej.getCount())
      {
         values = dashnrej.getValue();
         C.maxReject = asInt(values[0]);
         if (help)
            cout << "Max N rejected satellites is set to " << C.maxReject << endl;
      }
      if (dashNit.getCount())
      {
         values = dashNit.getValue();
         C.nIter = asInt(values[0]);
         if (help)
            cout << "Max N Iterations is set to " << C.nIter << endl;
      }
      if (dashFreq.getCount())
      {
         values = dashFreq.getValue();
         i = asInt(values[0]);
         if (i == 1 || i == 2 || i == 3)
         {
            C.Freq = i;
            if (help)
               cout << "Frequency is set to " << C.Freq << endl;
         }
         else cerr << "Error: invalid frequency" << endl;
      }
      if (dashElev.getCount())
      {
         values = dashElev.getValue();
         C.elevLimit = asDouble(values[0]);
         if (help) cout << "Elevation limit is set to " << C.convLimit << " deg" << endl;
      }
      if (dashConv.getCount())
      {
         values = dashConv.getValue();
         C.convLimit = asDouble(values[0]);
         if (help) cout << "Convergence limit is set to " << C.convLimit << endl;
      }

      if (dashXYZ.getCount())
      {
         values = dashXYZ.getValue();
         for (i=0; i<values.size(); i++)
         {
            field.clear();
            while(values[i].size() > 0)
            {
               field.push_back(stripFirstWord(values[i],','));
            }
            if (field.size() < 3)
            {
               cerr << "Error: less than four fields in --PosXYZ input: "
                    << values[i] << endl;
               continue;
            }
            Position p(asDouble(field[0]), asDouble(field[1]), asDouble(field[2]));
            C.knownpos = p;
            
            if (help)
               cout << " Input: known XYZ position "
                    << field[0] << " " << field[1] << " " << field[2] << endl;
         }
      }
      if (dashAPSout.getCount())
         C.APSout=true;
      if (dashForm.getCount())
      {
         values = dashForm.getValue();
         C.timeFormat = values[0];
         if (help)
            cout << " Input: time format " << C.timeFormat << endl;
      }
      if (dashORDs.getCount())
      {
         values = dashORDs.getValue();
         C.ordFile = values[0];
         if (help)
            cout << " Input: output ORDs to file " << C.ordFile << endl;
      }
      if (dashXsat.getCount())
      {
         values = dashXsat.getValue();
         for (i=0; i<values.size(); i++)
         {
            RinexSatID p(values[i]);
            C.ExSV.push_back(SatID(p));
            
            if (help)
               cout << "Exclude satellite " << p << endl;
         }
      }
      if (dashTrop.getCount())
      {
         values = dashTrop.getValue();
         field.clear();
         
         while(values[0].size() > 0)
         {
            field.push_back(stripFirstWord(values[0],','));
         }
         
         if (field.size() != 1 && field.size() != 4)
         {
            cerr << "Error: invalid fields after --Trop input: "
                 << values[0] << endl;
         }
         else
         {
            field[0] = upperCase(field[0]);
            C.TropType = field[0];
            
            if (help)
               cout << " Input: trop model: " << C.TropType;
            
            if (field.size() == 4)
            {
               C.defaultT = asDouble(field[1]);
               C.defaultPr = asDouble(field[2]);
               C.defaultRH = asDouble(field[3]);
               if (help)
                  cout << " and weather (T,P,RH): "
                       << C.defaultT << ","
                       << C.defaultPr << ","
                       << C.defaultRH;
            }
            if (help)
               cout << endl;
         }
      }
      if (dashLog.getCount())
      {
         values = dashLog.getValue();
         C.LogFile = values[0];
         if (help)
            cout << "Log file is " << C.LogFile << endl;
      }
      if (dashRfile.getCount())
      {
         values = dashRfile.getValue();
         C.OutRinexObs = values[0];
         if (help)
            cout << "Output RINEX file name is " << C.OutRinexObs << endl;
      }
      if (dashRrun.getCount())
      {
         values = dashRrun.getValue();
         C.HDRunby = values[0];
         if (help)
            cout << "Output RINEX 'RUN BY' is " << C.HDRunby << endl;
      }
      if (dashRobs.getCount())
      {
         values = dashRobs.getValue();
         C.HDObs = values[0];
         if (help)
            cout << "Output RINEX 'OBSERVER' is " << C.HDObs << endl;
      }
      if (dashRag.getCount())
      {
         values = dashRag.getValue();
         C.HDAgency = values[0];
         if (help)
            cout << "Output RINEX 'AGENCY' is " << C.HDAgency << endl;
      }
      if (dashRmark.getCount())
      {
         values = dashRmark.getValue();
         C.HDMarker = values[0];
         if (help)
            cout << "Output RINEX 'MARKER' is " << C.HDMarker << endl;
      }
      if (dashRnumb.getCount())
      {
         values = dashRnumb.getValue();
         C.HDNumber = values[0];
         if (help)
            cout << "Output RINEX 'NUMBER' is " << C.HDNumber << endl;
      }

      if (Rest.getCount())
      {
         if (help) cout << "Remaining options:" << endl;
         values = Rest.getValue();
         for (i=0; i<values.size(); i++)
         {
            if (help)
               cout << values[i] << endl;
         }
      }

      if (help)
         return 1;

      C.oflog.open(C.LogFile.c_str(),ios::out);
      if (C.oflog.fail())
      {
         cout << "Failed to open log file " << C.LogFile << endl;
         return -2;
      }
      else
      {
         cout << "Opened log file " << C.LogFile << endl;
         C.oflog << Title;
      }

      return 0;
   }
   catch(Exception& e)
   {
      GPSTK_RETHROW(e);
   }
   catch(exception& e)
   {
      Exception E("std except: "+string(e.what( )));
      GPSTK_THROW(E);
   }
   catch(...)
   {
      Exception e("Unknown exception");
      GPSTK_THROW(e);
   }
   return -1;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
