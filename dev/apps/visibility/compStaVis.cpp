#pragma ident "$Id: //depot/msn/prototype/brent/coverage/compStaVis.cpp#19 $"
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
//
// Compute station visiblity over desired period (23:56 default) for a 
// constellation.  compStaVis works off FIC ephemeris, FIC almanac, 
// Rinex nav, Yuma almanac, SEM alamanc, and SP3.
//
// Assumptions:
//
// System
#include <stdio.h>
#include <iostream>
#include <string>
#include <list>

// gpstk
#include "BasicFramework.hpp"
#include "StringUtils.hpp"
#include "CommandOptionWithTimeArg.hpp"
#include "AlmOrbit.hpp"
#include "GPSAlmanacStore.hpp"
#include "YumaAlmanacStore.hpp"
#include "SEMAlmanacStore.hpp"
#include "GPSEphemerisStore.hpp"
#include "icd_200_constants.hpp"
#include "gps_constants.hpp"

// Project
#include "VisSupport.hpp"
#include "StaStats.hpp"

using namespace std;
using namespace gpstk;

class compStaVis : public gpstk::BasicFramework
{
public:
   compStaVis(const std::string& applName,
              const std::string& applDesc) throw();
   ~compStaVis() {}
   virtual bool initialize(int argc, char *argv[]) throw();
   
protected:
   virtual void process();

   gpstk::CommandOptionWithAnyArg intervalOpt;
   gpstk::CommandOptionWithAnyArg outputOpt;
   gpstk::CommandOptionWithAnyArg nFileNameOpt;
   gpstk::CommandOptionWithAnyArg mscFileName;
   gpstk::CommandOptionWithAnyArg minElvOpt;
   gpstk::CommandOptionWithAnyArg typeOpt;
   gpstk::CommandOptionWithAnyArg excludeStation;
   gpstk::CommandOptionWithAnyArg includeStation;
   gpstk::CommandOptionWithAnyArg maxSVOpt;
   gpstk::CommandOptionNoArg healthyOpt;
   gpstk::CommandOptionNoArg detailPrintOpt;
   gpstk::CommandOptionWithTimeArg evalStartTimeOpt;
   gpstk::CommandOptionWithTimeArg evalEndTimeOpt;
   std::list<long> blocklist;
   
   FILE *logfp;

   static const int FIC_ALM;
   static const int FIC_EPH;
   static const int RINEX_NAV;
   static const int Yuma_ALM;
   static const int SEM_ALM;
   static const int SP3;
   int navFileType; 
   
   bool detailPrint;
   bool evalStartTimeSet;
   DayTime evalStartTime;
   bool evalEndTimeSet;
   DayTime evalEndTime;
   
   double intervalInSeconds;
   double minimumElevationAngle;
   int maxSVCount;
   
   GPSAlmanacStore BCAlmList;
   GPSEphemerisStore BCEphList;
   SP3EphemerisStore SP3EphList;
   YumaAlmanacStore YumaAlmStore;
   SEMAlmanacStore SEMAlmStore;

   StaPosList stationPositions;

   typedef map<string,StaStats> StaStatsList;
   StaStatsList staStatsList;
   int epochCount;
   
   DayTime startT;
   DayTime endT;
   bool siderealDay; 

   bool healthyOnly;
   
   void generateHeader( gpstk::DayTime currT );
   void generateTrailer( );
   gpstk::DayTime setStartTime();
   void computeVisibility( gpstk::DayTime currT );
   void printNavFileReferenceTime(FILE* logfp);
};

const int compStaVis::FIC_ALM = 0;
const int compStaVis::FIC_EPH = 1;
const int compStaVis::RINEX_NAV = 2;
const int compStaVis::SP3 = 3;
const int compStaVis::Yuma_ALM = 4;
const int compStaVis::SEM_ALM = 5;

int main( int argc, char*argv[] )
{
   try
   {
      compStaVis fc("compStaVis", "Compute Station Visiblity.");
      if (!fc.initialize(argc, argv)) return(false);
      fc.run();
   }
   catch(gpstk::Exception& exc)
   {
      cerr << exc << endl;
      return 1;
   }
   catch(...)
   {
      cerr << "Caught an unnamed exception. Exiting." << endl;
      return 1;
   }
   return 0;
}

compStaVis::compStaVis(const std::string& applName, 
                       const std::string& applDesc) throw()
          :BasicFramework(applName, applDesc),
           intervalOpt('p',"int","Interval in seconds.", false),
           nFileNameOpt('n',"nav","Name of navigation file" , true),
           outputOpt('o', "output-file", "Name of the output file to write.", true),
           mscFileName('c',"mscfile","Name of MS coordinates files", true),
           minElvOpt('e', "minelv","Minimum elevation angle", false),
           excludeStation('x',"exclude","Exclude station",false),
           includeStation('i',"include","Include station",false),
           detailPrintOpt('D',"detail","Print SV count for each interval",false),
           maxSVOpt('m',"max-SV","Maximum # of SVs tracked simultaneously",false),
           healthyOpt('h',"healthy","Consider only healthy SVs (require FIC ephemeris or Rinex nav file",false),
           evalStartTimeOpt('s',"start-time","%m/%d/%y %H:%M","Start time of evaluation (\"m/d/y H:M\") ",false),
           evalEndTimeOpt('z',"end-time","%m/%d/%y %H:%M","End time of evaluation (\"m/d/y H:M\")",false),
           typeOpt('t', "navFileType", "FALM, FEPH, RNAV, YUMA, SEM, or SP3 ", false)
{
   intervalOpt.setMaxCount(1);
   nFileNameOpt.setMaxCount(3);        // 3 to allow for SP3 input
   outputOpt.setMaxCount(1);
   minElvOpt.setMaxCount(1);
   mscFileName.setMaxCount(1);
   typeOpt.setMaxCount(1);
   detailPrintOpt.setMaxCount(1);
   maxSVOpt.setMaxCount(1);
   healthyOpt.setMaxCount(1);
   evalStartTimeOpt.setMaxCount(1);
   evalEndTimeOpt.setMaxCount(1);
   epochCount = 0;
   evalStartTimeSet = false;
}

bool compStaVis::initialize(int argc, char *argv[])
   throw()
{
   if (!BasicFramework::initialize(argc, argv)) return false;
   
      // Open the output file
   logfp = fopen( outputOpt.getValue().front().c_str(),"wt");
   if (logfp==0) 
   {
      cerr << "Failed to open output file. Exiting." << endl;
      return false;
   }
   
   vector<string> values;
   intervalInSeconds = 60.0;        // default
   if (intervalOpt.getCount()>0)
   {
      values = intervalOpt.getValue();
      intervalInSeconds = StringUtils::asInt( values[0] );
   }

   navFileType = FIC_ALM;              // default case
   if (typeOpt.getCount()!=0)
   {
      values = typeOpt.getValue();
      if (values[0].compare("FEPH")==0)      navFileType = FIC_EPH;
      else if (values[0].compare("RNAV")==0) navFileType = RINEX_NAV;
      else if (values[0].compare("SP3")==0)  navFileType = SP3;
      else if (values[0].compare("FALM")==0) navFileType = FIC_ALM;
      else if (values[0].compare("YUMA")==0) navFileType = Yuma_ALM;
      else if (values[0].compare("SEM")==0)  navFileType = SEM_ALM;
      else
      {
         cerr << "Invalid value for nav file type.  Must be one of " << endl;
         cerr << "   'FALM', 'FEPH', 'RNAV', 'YUMA', 'SEM', or 'SP3'. " << endl;
         cerr << "Fatal error.  compStaVis will terminate." << endl;
         return false;
      }
   }
   
   minimumElevationAngle = 10.0;
   if (minElvOpt.getCount()!=0)
   {
      values = minElvOpt.getValue();
      minimumElevationAngle = StringUtils::asDouble( values[0] ); 
   }
   
   detailPrint = false;
   if (detailPrintOpt.getCount()!=0) detailPrint = true;

   maxSVCount = 12;
   if (maxSVOpt.getCount()!=0)
   {
      values = maxSVOpt.getValue();
      maxSVCount = StringUtils::asInt( values[0] );
   }

   healthyOnly = false;   
   if (healthyOpt.getCount()!=0)
   {
      if (navFileType!= FIC_EPH &&
          navFileType!= RINEX_NAV)
      {
         cerr << "Invalid value for nav file type.";
         cerr << "  To determine SV health must use 'FEPH' or 'RNAV'. " << endl;
         cerr << "Fatal error.  compStaVis will terminate." << endl;
         return false;
      }
      healthyOnly = true;
   }
   
      // If the user SPECIFIED a start time for the evaluation, store that
      // time and set the flag.
   evalStartTimeSet = false;
   evalStartTime=DayTime::BEGINNING_OF_TIME;
   if (evalStartTimeOpt.getCount()!=0) 
   {
      if (debugLevel) cout << "Reading start time from command line." << endl;
      std::vector<DayTime> tvalues = evalStartTimeOpt.getTime();
      evalStartTime = tvalues[0];
      evalStartTimeSet = true;
      
         // Reinit YumaAlmStore to know the time of interest..
      if (navFileType==Yuma_ALM) YumaAlmStore = YumaAlmanacStore( evalStartTime );
      if (navFileType==SEM_ALM) SEMAlmStore = SEMAlmanacStore( evalStartTime );
   }
   
      // If the user SPECIFIED an end time for the evaluation, store that
      // time and set the flag.
   evalEndTimeSet = false;
   evalEndTime=DayTime::END_OF_TIME;
   if (evalEndTimeOpt.getCount()!=0) 
   {
      if (debugLevel) cout << "Reading end time from command line." << endl;
      std::vector<DayTime> tvalues = evalEndTimeOpt.getTime();
      evalEndTime = tvalues[0];
      evalEndTimeSet = true;
   }
   
   return true;   
}

void compStaVis::printNavFileReferenceTime(FILE* logfp)
{
   string tform2 = "%02m/%02d/%02y DOY %03j, GPS Week %F, DOW %w, %02H:%02M:%02S";
   DayTime t;
   
      // If the user did not specify a start time for the evaulation, find the
      // epoch time of the navigation data set and work from that.
      // In the case of almanac data, the "initial time" is derived from the 
      // earliest almanac reference time minus a half week.  Therefore, we
      // add the halfweek back in.  
   switch(navFileType)
   {
         // For ephemeris, initial time is earliest beginning of effectivty.
      case FIC_EPH:
      case RINEX_NAV:
         fprintf(logfp,"  Ephemeris effectivity\n");
         fprintf(logfp,"     Earliest             : %s\n",
                 BCEphList.getInitialTime().printf(tform2).c_str());
         fprintf(logfp,"     Latest               : %s\n",
                 BCEphList.getFinalTime().printf(tform2).c_str());
         break;
            
      case FIC_ALM:
         t = BCAlmList.getInitialTime();
         t += DayTime::HALFWEEK;
         fprintf(logfp,"  Almanac reference time\n");
         fprintf(logfp,"     Earliest             : %s\n",
                       t.printf(tform2).c_str());
         t = BCAlmList.getFinalTime();
         t -= DayTime::HALFWEEK;
         fprintf(logfp,"     Latest               : %s\n",
                       t.printf(tform2).c_str());
         break;

      case Yuma_ALM:
         t = YumaAlmStore.getInitialTime();
         t += DayTime::HALFWEEK;
         fprintf(logfp,"  Almanac reference time\n");
         fprintf(logfp,"     Earliest             : %s\n",
                       t.printf(tform2).c_str());
         t = YumaAlmStore.getFinalTime();
         t -= DayTime::HALFWEEK;
         fprintf(logfp,"     Latest               : %s\n",
                       t.printf(tform2).c_str());
         break;

      case SEM_ALM:
         t = SEMAlmStore.getInitialTime();
         t += DayTime::HALFWEEK;
         fprintf(logfp,"  Almanac reference time\n");
         fprintf(logfp,"     Earliest             : %s\n",
                       t.printf(tform2).c_str());
         t = SEMAlmStore.getFinalTime();
         t -= DayTime::HALFWEEK;
         fprintf(logfp,"     Latest               : %s\n",
                       t.printf(tform2).c_str());
         break;
         
      case SP3:
      {
         DayTime begin = SP3EphList.getInitialTime();
         DayTime end = SP3EphList.getFinalTime();
         fprintf(logfp,"  Ephemeris effectivity\n");
         fprintf(logfp,"     Earliest             : %s\n",
                 begin.printf(tform2).c_str());
         fprintf(logfp,"     Latest               : %s\n",
                 end.printf(tform2).c_str());
         break;
      }   
   }
   return;
}

gpstk::DayTime compStaVis::setStartTime()
{
   DayTime retDT = DayTime( 621, 0.0 );     // 12/1/1991
   DayTime initialTime;
   DayTime finalTime;
   
   switch(navFileType)
   {
      case FIC_EPH:
      case RINEX_NAV:
         initialTime = BCEphList.getInitialTime();
         finalTime   = BCEphList.getFinalTime();
         break;
            
      case FIC_ALM:
         initialTime = BCAlmList.getInitialTime();
         finalTime   = BCAlmList.getFinalTime();
         break;

      case Yuma_ALM:
         initialTime = YumaAlmStore.getInitialTime();
         finalTime   = YumaAlmStore.getFinalTime();
         break;

      case SEM_ALM:
         initialTime = SEMAlmStore.getInitialTime();
         finalTime   = SEMAlmStore.getFinalTime();
         break;

         // If loading "day at a time" files, will need 
         // three days to cover middle day.  We need to 
         // find the middle of whatever period was loaded
         // and back up to the beginning of that day.
      case SP3:
      {
         initialTime = SP3EphList.getInitialTime();
         finalTime = SP3EphList.getFinalTime();
         break;
      }   
   }
   double diff = finalTime - initialTime;
   retDT = initialTime;
   retDT += diff/2.0;
   retDT = DayTime( retDT.year(), retDT.DOY(), 0.0 );
   return(retDT);
}        


void compStaVis::process()
{
   if (verboseOption)
   {
      cout << "Loading navigation message data from ";
      int nfiles = nFileNameOpt.getCount();
      vector<std::string> names = nFileNameOpt.getValue();
      for (int i1=0;i1<nfiles;++i1) 
      {
         if (i1>0) cout << ", ";
         cout << names[i1];
      }
      cout << "." << endl;
   }
   switch(navFileType)
   {
      case FIC_EPH: VisSupport::readFICNavData(nFileNameOpt,BCAlmList,BCEphList); break;
      case FIC_ALM: VisSupport::readFICNavData(nFileNameOpt,BCAlmList,BCEphList); break;
      case RINEX_NAV: VisSupport::readRINEXNavData(nFileNameOpt,BCEphList); break;
      case Yuma_ALM: VisSupport::readYumaData(nFileNameOpt,YumaAlmStore); break; 
      case SEM_ALM:  VisSupport::readSEMData(nFileNameOpt,SEMAlmStore); break;
      case SP3: VisSupport::readPEData(nFileNameOpt,SP3EphList); break;
      default:
         cerr << "Unknown navigation file type in process()." << endl;
         cerr << "Fatal error. compStaVis will halt." << endl; exit(1);
   }
   
      // Determine day of interest
   if (debugLevel) cout << "Setting evaluation start time: ";
   startT = evalStartTime;
   if (!evalStartTimeSet) startT = setStartTime();
   if (debugLevel) cout << startT.printf("%02m/%02d/%02y DOY %03j, GPS Week %F, DOW %w, %02H:%02M.") << endl;
   
      // If no end time commanded, compute for 23h 56m (GPS ground track repeat)
   if (debugLevel) cout << "Setting evaluation end time: ";
   siderealDay = true;
   endT = startT + ( (double) DayTime::SEC_DAY - 240.0);
   if (evalEndTimeSet) endT = evalEndTime;
   if ((int)(endT-startT)!=(int)(DayTime::SEC_DAY-240)) siderealDay = false;
   if (debugLevel) 
   {
      cout << endT.printf("%02m/%02d/%02y DOY %03j, GPS Week %F, DOW %w, %02H:%02M.") << endl;
      cout << "Sidereal Day flag : " << siderealDay << endl;
   }
   DayTime currT = startT;   
   
      // Get coordinates for the stations
   if (debugLevel) cout << "Reading station coordinate file." << endl;
   stationPositions = VisSupport::getStationCoordinates( mscFileName,
                                                         startT, 
                                                         includeStation, 
                                                         excludeStation );
   
      // Initialize the station statistics objects
   StaPosList::const_iterator vci;
   for (vci=stationPositions.begin();vci!=stationPositions.end();++vci)
   {
       string name = (string) vci->first;
       StaStats temp = StaStats( name, maxSVCount, 0 );
       pair<string,StaStats> node( name, temp );
       staStatsList.insert( node );
   }
   
      // Generate the header
   generateHeader( startT ); 
      
      // For each interval, calculate SV-station visibility
   if (debugLevel) cout << "Entering calculation loop." << endl;
   long lastValue = -1;
   while (currT <= endT) 
   {
      if (debugLevel)
      {
         long sec = (long) currT.GPSsecond();
         long newValue = sec / 3600;
         if (newValue!=lastValue)
         {
            if (currT.hour()==0) cout << endl << currT.printf("%02m/%02d/%04Y ");
            cout << currT.printf("%02H:, ");
            lastValue = newValue;
         }
      }
      computeVisibility( currT );
      currT += intervalInSeconds;
      epochCount++;
   }
   
   if (debugLevel) cout << endl << "Generating trailer." << endl;
   generateTrailer( );
  
   fclose(logfp);
   
}

void compStaVis::generateHeader( gpstk::DayTime currT )
{
   DayTime now;
   string tform = "%02m/%02d/%02y DOY %03j, GPS Week %F, DOW %w";
   fprintf(logfp,"compStaVis output file.  Generated at %s\n",
           now.printf("%02H:%02m on %02m/%02d/%02y").c_str() );
   fprintf(logfp,"Program arguments\n");
   fprintf(logfp,"  Navigation file         : ",nFileNameOpt.getValue().front().c_str());
   vector<std::string> values = nFileNameOpt.getValue();
   for (int i=0; i<nFileNameOpt.getCount(); ++i) 
      fprintf(logfp,"%s  ",values[i].c_str());
   fprintf(logfp,"\n");
   fprintf(logfp,"  Day of interest         : %s\n",currT.printf(tform).c_str());
   fprintf(logfp,"  Minimum elv ang         : %5.0f degrees\n",minimumElevationAngle);
   fprintf(logfp,"  Evaluation interval     : %5.0f sec\n",intervalInSeconds);
   fprintf(logfp,"  Only consider healthy SV: ");
   if (healthyOnly) fprintf(logfp,"TRUE\n");
    else           fprintf(logfp,"no\n");
   fprintf(logfp,"  Station coordinates file: %s\n",mscFileName.getValue().front().c_str());
   printNavFileReferenceTime(logfp);
   fprintf(logfp,"  Start time of evaluation: %s\n",startT.printf(tform+", %02H:%02M:%02S").c_str());
   fprintf(logfp,"  End time of evaluation  : %s\n",endT.printf(tform+", %02H:%02M:%02S").c_str());
   if (siderealDay)
      fprintf(logfp,"  Evaluation covers one sidereal day.\n");
   
      // Print list of stations
   if (includeStation.getCount() || excludeStation.getCount() )
   {
      fprintf(logfp,"\n  Stations included in the analysis\n");
      fprintf(logfp," Abbr       XYZ(km)\n");
      StaPosList::const_iterator si;
      for (si=stationPositions.begin();si!=stationPositions.end();++si)
      {
         string mnemonic = (string) si->first;
         ECEF coordinates = (ECEF) si->second;
         fprintf(logfp," %4s  %10.3lf  %10.3lf  %10.3lf\n",
              mnemonic.c_str(),
              coordinates[0]/1000.0,
              coordinates[1]/1000.0,
              coordinates[2]/1000.0 );
      }
      fprintf(logfp,"Number of Stations: %d\n\n",stationPositions.size());
   }
   else fprintf(logfp,"  All stations in coordinates file were included in the analysis.");
   
   if (detailPrint)
   {
      StaPosList::const_iterator si;
      fprintf(logfp,"\n DOY:HH:MM:SS");
      for (si=stationPositions.begin();si!=stationPositions.end();++si)
      {
         string mnemonic = (string) si->first;
         fprintf(logfp,"  %4s",mnemonic.c_str());
      }
      fprintf(logfp,"   Max   Min");
      if (stationPositions.size()==1) fprintf(logfp,"    List of SV PRN IDs");
      fprintf(logfp,"\n");
   }
   
}

void compStaVis::generateTrailer( )
{
   fprintf(logfp,"\n\n Summary statistics by station\n");
   fprintf(logfp,"                 !       Minimum         !       Maximum         !\n");
   fprintf(logfp,"Station  Avg#SVs ! #SVs Dur(min)  #Occur ! #SVs Dur(min)  #Occur ! #Mins>%02dSVs\n",
           maxSVCount);
   
   int dum2 = (int) intervalInSeconds;
   
   StaStatsList::const_iterator sslCI;
   for (sslCI =staStatsList.begin();
        sslCI!=staStatsList.end(); ++sslCI)
   {
      //string staName = sslCI->first;
      StaStats ss = sslCI->second;
      std::string dummy = ss.getStr( dum2 );
      fprintf(logfp,"%s\n",dummy.c_str());
   }

   //cout << "Printing elv table header" << endl;
   fprintf(logfp,"\n\n Number of Observations by Elevation Angle\n");
   fprintf(logfp,"  Sta#  Total# 00-04 05-09 10-15 15-19 20-24 25-29 30-34 35-39 40-44 45-49 50-54 55-59 60-64 65-69 70-74 75-79 80-84 85-90\n");
   //cout << "Dumping elv angle table" << endl;
   for (sslCI =staStatsList.begin();
        sslCI!=staStatsList.end(); ++sslCI)
   {
      //string staName = sslCI->first;
      StaStats ss = sslCI->second;
      std::string dummy = ss.getElvBinValues();
      fprintf(logfp,"%s\n",dummy.c_str());
   }
}

void compStaVis::computeVisibility( gpstk::DayTime currT )
{
   gpstk::ECEF SVpos[gpstk::MAX_PRN+1];
   bool SVAvail[gpstk::MAX_PRN+1];
   int SVHealth[gpstk::MAX_PRN+1];
   Xvt SVxvt;
   
      // Compute SV positions for this epoch
   int PRNID;
   for (PRNID=1;PRNID<=gpstk::MAX_PRN;++PRNID)
   {
      SVAvail[PRNID] = false;
      SVHealth[PRNID] = 0;
      try
      {
         SatID satid(PRNID, SatID::systemGPS);
         switch(navFileType)
         {
            case FIC_EPH:
            case RINEX_NAV:
               SVxvt = BCEphList.getXvt( satid, currT );
               SVHealth[PRNID] = BCEphList.getSatHealth( satid, currT ); 
               break;
            
            case FIC_ALM:
               SVxvt = BCAlmList.getXvt( satid, currT );
               break;
            
            case Yuma_ALM:
               SVxvt = YumaAlmStore.getXvt( satid, currT );
               break;
 
            case SEM_ALM:
               SVxvt = SEMAlmStore.getXvt( satid, currT );
               break;
              
            case SP3:
               SVxvt = SP3EphList.getXvt( satid, currT );
               break;
               
            default:
               cerr << "Unknown navigation file type in computeVisibility()." << endl;
               cerr << "Fatal error. compStaVis will halt." << endl;
               exit(1);
         }
         SVpos[PRNID] = SVxvt.x; 
         SVAvail[PRNID] = true;
      }
      catch(InvalidRequest& e)
      { 
         continue;
      }
   }
   
   if (detailPrint) fprintf(logfp,"%s ",currT.printf("T%03j:%02H:%02M:%02S").c_str());

   string SVList;       // We'll build a list of SVs in view in this string
                        // We'll only use it if there's only one station selected.
   
      // Now count number of SVs visible at each station
   int maxNum = 0;
   int minNum = gpstk::MAX_PRN + 1; 
   StaPosList::const_iterator splCI;
   for (splCI =stationPositions.begin();
        splCI!=stationPositions.end();
        ++splCI)
   {
      int numVis = 0;
      double elv = 0.0;
      
         // Look up the appropriate StaStats object
      string staName = splCI->first;
      StaStatsList::iterator sslI = staStatsList.find( staName );
      if (sslI==staStatsList.end())
      {
         cerr << "Missing station stats structure for station " << staName << endl;
         cerr << "Fatal error.  compStaVis will terminate." << endl;
         exit(0);
      }
      StaStats& ss = sslI->second;

      SVList = "";
      char SVform[10];
      ECEF staPos = splCI->second;
      for (PRNID=1;PRNID<=gpstk::MAX_PRN;++PRNID)
      {
         // Debug
         if (currT.hour()==0 && (PRNID==2 || PRNID==7)) 
         {
            cerr << "PRNID: " << PRNID <<
                    "SVAvail:" << SVAvail[PRNID] <<
                    "SVHealth:" << SVHealth[PRNID] << endl;
         }
         if (SVAvail[PRNID])
         {
            elv = staPos.elvAngle( SVpos[PRNID] );
            if (elv>=minimumElevationAngle)
            {
               if (!healthyOnly ||
                  (healthyOnly && SVHealth[PRNID]==0))
               {
                  numVis++;
                  ss.addToElvBins( elv );
                  sprintf(SVform," %02d",PRNID);
                  SVList += SVform;
               }
               else
               {
                  sprintf(SVform," %02d(HLTH)",PRNID);
                  SVList += SVform;
               }
            }
         }
      }
      if (detailPrint) fprintf(logfp,"    %2d",numVis);
      if (numVis>maxNum) maxNum = numVis;
      if (numVis<minNum) minNum = numVis;
      
      //cout << " Calling addEpochInfo() for station " << staNum << ", station " << ss.getStaNum() << endl;
      ss.addEpochInfo( numVis, epochCount ); 
   }
   if (detailPrint)
   {
      fprintf(logfp,"    %2d    %2d",maxNum,minNum);
      if (stationPositions.size()==1) fprintf(logfp,"   %s",SVList.c_str());
      fprintf(logfp,"\n");
   }
}
