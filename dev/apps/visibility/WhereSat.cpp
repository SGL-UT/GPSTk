#pragma ident "$Id$"
//
//   Computes SV position (Earth-fixed) and SV clock
//   correction.  If given a user position, computes 
//   azimuth, elevation and range to SV.
//
#include <iostream>
#include <iomanip>

#include "RinexNavData.hpp"
#include "DayTime.hpp"
#include "RinexEphemerisStore.hpp"
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"
#include "BCEphemerisStore.hpp"
#include "WGS84Geoid.hpp"
#include "SP3EphemerisStore.hpp"

using namespace std;
using namespace gpstk;

int j=0,incr=900,mm,dd,yy,hh,minu; 	               // because sscanf doesnt read in shorts?...
short year, month, day, hour, minute, myprn;       // for DayTime usage
string OutputFileName,EphFileName;
ofstream ofs;
double x, y, z, ss, correction;
RinexEphemerisStore beEphList;
SP3EphemerisStore peEphList;
RinexNavData data;     
Xvt BCPVT,ANTPVT;
DayTime t,tStart,tEnd,tFile;
WGS84Geoid geoid;
ECEF rxPos;

int main(int argc, char *argv[])
{

CommandOptionNoArg
  helpOption('h',"help", "Print help usage.");

 CommandOptionWithAnyArg 
  BceFiles('b',"broadcast","RINEX navigation file. User may enter multiple files."),
  apcFiles('a',"precice","Precise ephemeris file in SP3 format. User may enter multiple files."),
  PositionOption('u',"position","Antenna position in ECEF (x,y,z) coordinates.  Format as a string: \"X Y Z\". Used to give user-centered data (SV range, azimuth & elevation)."),
  StartTimeOption('s',"start","Starting time for analysis. Format as string: \"MO/DD/YYYY HH:MM:SS\". If no start time is specified, the default is end of file."),
  EndTimeOption('e',"end", "End time for analysis.  Format as string: \"MO/DD/YYYY HH:MM:SS\". If no end time is specified, the default is beginning of file."),
  MatlabOption('o',"output-filename", "Filename for results to be output in MATLAB suitable file.");

 CommandOptionWithNumberArg 
  MyPrnOption('p',"prn","Which SV to analyze.",true),
  TimeIncrementOption('t',"time","Time increment for ephemeris calculation.  Enter increment in seconds.  Default is 900 (15 min).");     

 string appDesc("Computes the ephemeris data for a single prn. Although the -b and -p arguments appear as optional below, one of the two must be used. An ephemeris source must be specified.");
 CommandOptionParser cop(appDesc);
 cop.parseOptions(argc, argv);

 if (helpOption.getCount() || cop.hasErrors())
 {
    if (cop.hasErrors())
    cop.dumpErrors(cout);
    cop.displayUsage(cout);
    exit(0);
 }
 myprn = StringUtils::asInt((MyPrnOption.getValue())[0]); 
 if (MatlabOption.getCount())
 {
    OutputFileName = (MatlabOption.getValue())[0];
    ofs.open(OutputFileName.c_str(), ios::out);
    if (PositionOption.getCount()) { ofs << "#10-bit GPS week, GPS seconds of week,X,Y,Z(m),Clock Corr.(sec), azimuth (deg), elevation (deg), range (m)" << endl; }
 }
 else { ofs << "#10-bit GPS week, GPS seconds of week,X,Y,Z(m),Clock Corr.(sec)" << endl; } 
 if (TimeIncrementOption.getCount())
   	incr = gpstk::StringUtils::asInt((TimeIncrementOption.getValue())[0]);
 if (PositionOption.getCount())
 {
   	sscanf(PositionOption.getValue().front().c_str(),"%lf %lf %lf", &x, &y, &z);
   	ANTPVT.x[0] = x; ANTPVT.x[1] = y; ANTPVT.x[2] = z;
 }
 if (PositionOption.getCount()) {cout << endl << endl << setw(20) << left << " Antenna Position: " << ANTPVT.x[0] << "  " << ANTPVT.x[1] << "  " << ANTPVT.x[2] << endl;}
 
 if (BceFiles.getCount() && apcFiles.getCount() )
 {
    cout << "Enter either a broadcast or precise ephemeris source, but not both" << endl;
    exit(0);
 }
 if ((BceFiles.getCount()==0)&&(apcFiles.getCount()==0))
 {
    cout << "Enter a ephemeris source." << endl;
    exit(0);
 }
 
 bool havePE = true;
 if (BceFiles.getCount())
 {
    EphFileName = (BceFiles.getValue())[0];
    beEphList.loadFiles(BceFiles.getValue());
    beEphList.SearchNear();
    havePE = false;
 }
 if (apcFiles.getCount())
 {
    EphFileName = (apcFiles.getValue())[0];
    peEphList.loadFiles(apcFiles.getValue());
 }
   
 if (StartTimeOption.getCount())
 {
    sscanf(StartTimeOption.getValue().front().c_str(), "%i/%i/%i %i:%i:%lf",&mm,&dd,&yy,&hh,&minu,&ss);
    tStart.setYMDHMS((short)yy, (short)mm, (short)dd, (short)hh, (short)minu, (double)ss);
 }	
 else if(BceFiles.getCount())
 {
    DayTime tFile(beEphList.getInitialTime());	//extra code b/c sscanf reads in int's but setYMDHMS needs shorts
    year = tFile.year();
    month = tFile.month();
    day = tFile.day();
    hour = tFile.hour();
    minute = tFile.minute();
    ss = tFile.second();
    tStart.setYMDHMS(year,month,day,hour,minute,ss);
 }
 else if(apcFiles.getCount())
 {
    DayTime tFile(peEphList.getInitialTime());	//extra code b/c sscanf reads in int's but setYMDHMS needs shorts
    year = tFile.year();
    month = tFile.month();
    day = tFile.day();
    hour = tFile.hour();
    minute = tFile.minute();
    ss = tFile.second();
    tStart.setYMDHMS(year,month,day,hour,minute,ss);
 }
 
 if (EndTimeOption.getCount())
 {
    sscanf(EndTimeOption.getValue().front().c_str(), "%i/%i/%i %i:%i:%lf",&mm,&dd,&yy,&hh,&minu,&ss);
    tEnd.setYMDHMS((short)yy, (short)mm, (short)dd, (short)hh, (short)minu, (double)ss);
 }
 else if(BceFiles.getCount())
 {
    DayTime tFile(beEphList.getFinalTime());		//extra code b/c sscanf reads in int's but setYMDHMS needs shorts
    year = tFile.year();
    month = tFile.month();
    day = tFile.day();
    hour = tFile.hour();
    minute = tFile.minute();
    ss = tFile.second();
    tEnd.setYMDHMS(year,month,day,hour,minute,ss);
 }
 else if(apcFiles.getCount())
 {
    DayTime tFile(peEphList.getFinalTime());		//extra code b/c sscanf reads in int's but setYMDHMS needs shorts
    year = tFile.year();
    month = tFile.month();
    day = tFile.day();
    hour = tFile.hour();
    minute = tFile.minute();
    ss = tFile.second();
    tEnd.setYMDHMS(year,month,day,hour,minute,ss);
 } 

 cout  << setw(20) << left << " Start Time: "
       << tStart << endl;
 cout  << setw(20) << left << " End Time: "
       << tEnd << endl;
 cout  << setw(20) << left << " PRN: "
       << myprn << endl << endl;
 cout  << " Prn " << myprn 
       << " Earth-fixed position and clock information:"
       << endl << endl;
 cout  << setw(20) << left << " Date       Time(UTC)  "
       << setw(20) << " X (meters)"
       << setw(20) << " Y (meters)"
       << setw(20) << " Z (meters)"
       << setw(20) << " Clock Correc (s)"
       << endl;
 cout  << setw(98) << setfill('=') << " =" << endl;
 cout.width(0);
 cout.fill(' '); 

 t = tStart;

 SatID sat(myprn,SatID::systemGPS);
 while (t < tEnd)
 {
    try
    {
      if (BceFiles.getCount()) {BCPVT = beEphList.getSatXvt(sat, t);}
      if (apcFiles.getCount()) {BCPVT = peEphList.getSatXvt(sat, t);}
      cout  << " "  << left << setw(20) << t
            << fixed
            << " "  << left << setw(20) << BCPVT.x[0]
            << " "  << left << setw(20) << BCPVT.x[1]
            << " "  << left << setw(20) << BCPVT.x[2]
            << " "  << left << setw(20) << BCPVT.dtime << endl;
      j++;
    }
    catch (gpstk::EphemerisStore::NoEphemerisFound& e) { ; }
    t += incr;
 }
 if (PositionOption.getCount())
 {
    rxPos = Triple(ANTPVT.x);
    cout  << endl << endl; 
    cout  << " Data for user reference frame:" << endl << endl;;
    cout  << setw(20) << left << " Date       Time(UTC)  "
          << setw(15) << " Azimuth"
          << setw(15) << " Elevation"
          << setw(15) << " Range to SV (m)"
          << endl;
    cout  << setw(70) << setfill('=') << " =" << endl;
    cout.width(0);
    cout.fill(' '); 
    t = tStart;
    while (t < tEnd)
    {
      try 
      {
        if (BceFiles.getCount()) {BCPVT = beEphList.getSatXvt(sat, t);}
        if (apcFiles.getCount()){BCPVT = peEphList.getSatXvt(sat, t);}
	correction = (BCPVT.dtime) * (geoid.c());
       	cout  << fixed
	      << " "  << left << setw(20) <<t
              << " "  << left << setw(15) << ANTPVT.x.azAngle(BCPVT.x)
              << " "  << left << setw(15) << ANTPVT.x.elvAngle(BCPVT.x)
	      << " "  << left << setw(15) << BCPVT.preciseRho(rxPos, geoid, correction)
              << endl;
      }
      catch (gpstk::Exception& e) {;}
      t += incr;
    }
  }
 
 if (MatlabOption.getCount())
 {
    t = tStart;
    while (t < tEnd)
    {
      try 
      {   
        if (BceFiles.getCount()) {BCPVT = beEphList.getSatXvt(sat, t);}
        if (apcFiles.getCount()){BCPVT = peEphList.getSatXvt(sat, t);}        
	correction = (BCPVT.dtime) * (geoid.c());
        ofs << fixed
            << left << setw(6) <<t.GPSfullweek()
            << " "  << left << setw(8) << t.GPSsecond() 
            << " "  << left << setw(20) << BCPVT.x[0]
            << " "  << left << setw(20) << BCPVT.x[1]
            << " "  << left << setw(20) << BCPVT.x[2]
            << " "  << left << setw(10) << BCPVT.dtime;
	    if (PositionOption.getCount())
	    {
	       ofs << fixed
            	   << " "  << left << setw(15) << ANTPVT.x.azAngle(BCPVT.x)
            	   << " "  << left << setw(15) << ANTPVT.x.elvAngle(BCPVT.x)
		   << " "  << left << setw(15) << BCPVT.preciseRho(rxPos, geoid, correction);
            }
	    ofs << endl;
      }
      catch (gpstk::Exception& e) { ; }
      t += incr;
    }
  }
 
 if (j==0) { cout << endl << endl << " No epochs for prn " << myprn << "!" << endl << endl; }
 else { cout << endl <<  " Calculated " << j << " increments for prn " << myprn << " ." << endl<< endl << endl; }

 ofs.close();
 exit(0);

}

