#include <iostream>
#include <iomanip>

#include "DayTime.hpp"
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"
#include "WGS84Geoid.hpp"
#include "SP3EphemerisStore.hpp"
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "RinexNavData.hpp"
#include "RinexEphemerisStore.hpp"
#include "BCEphemerisStore.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char *argv[])
{
  CommandOptionNoArg
    helpOption('h',"help", "Print help usage."),
    verboseOption('v',"verbose","Increase verbosity.");

  CommandOptionWithAnyArg 
    apcFile('a',"apc-file","Precise ephemeris file in SP3 format. User may enter multiple files."),
    beFile('b',"be-file","RINEX navigation file. User may enter multiple files."),
    antennaPosition('p',"position","Antenna position in ECEF (x,y,z) coordinates.  Format as a string: \"X Y Z\".",true);
    
  CommandOptionWithNumberArg 
    minElev('e',"elev","Give an integer for the elevation (degrees) above which you want to find more than 12 SVs at a given time.", true);     
      
  CommandOptionParser cop("Finds when there are simultaneously more than 12 SVs above a given elevation. Although the -a and -b arguments are listed as optional below, one must be specified. An ephemeris source must be given.");
    cop.parseOptions(argc, argv);
  // verify that the correct input was given
  if (helpOption.getCount() || cop.hasErrors())
  {
    if (cop.hasErrors())
    cop.dumpErrors(cout);
    cop.displayUsage(cout);
    exit(0);
  }
  if ( (apcFile.getCount())&&(beFile.getCount()) )
  {
    cout << "Enter a precise OR broadcast ephemeris source, but not both." << endl;
    exit(0);
  }
  if ( (apcFile.getCount()==0)&&(beFile.getCount()==0) )
  {
    cout << "Please enter an ephemeris source file." << endl;
    exit(0);
  }
  // output intermediate info?
  bool verbose = false;
  if (verboseOption.getCount()) { verbose = true; }
  // get the minimum elevation
  int minEl = gpstk::StringUtils::asInt((minElev.getValue())[0]);
  if (minEl < 0 )
  {
    cout << "Please enter a positive elevation. " << endl;
    exit(0);
  }
  // get the ephemeris source(s)
  SP3EphemerisStore peEphList;
  RinexEphemerisStore beEphList;
  if (apcFile.getCount()) {peEphList.loadFiles(apcFile.getValue());}
  if (beFile.getCount()) {beEphList.loadFiles(beFile.getValue());}
  // get the antenna position
  Xvt antPVT;
  double x,y,z;
  sscanf(antennaPosition.getValue().front().c_str(),"%lf %lf %lf", &x, &y, &z);
  antPVT.x[0] = x; antPVT.x[1] = y; antPVT.x[2] = z;
  ECEF antPos = Triple(antPVT.x);
  Xvt peXVT;
  // get initial and final times for analysis. extra code b/c sscanf reads in int's but setYMDHMS needs shorts
  short year, month, day, hour, minute;
  double seconds;
  DayTime tStartDT, tEndDT, tstart, tend, t;
  if (apcFile.getCount()) {tStartDT = peEphList.getInitialTime(); }
  if (beFile.getCount()) {tStartDT = beEphList.getInitialTime(); }
  year = tStartDT.year();
  month = tStartDT.month();
  day = tStartDT.day();
  hour = tStartDT.hour();
  minute = tStartDT.minute();
  seconds = tStartDT.second();
  tstart.setYMDHMS(year,month,day,hour,minute,seconds);
  if (apcFile.getCount()) {tEndDT = peEphList.getFinalTime(); }
  if (beFile.getCount()) {tEndDT = beEphList.getFinalTime(); }
  year = tEndDT.year();
  month = tEndDT.month();
  day = tEndDT.day();
  hour = tEndDT.hour();
  minute = tEndDT.minute();
  seconds = tEndDT.second();
  tend.setYMDHMS(year,month,day,hour,minute,seconds);
  // start at the first ephemeris time and crank through. evaluate every minute
  t = tstart;
  cout << "Start Time: " << tstart << " End Time: " << tend << endl;
  while (t < tend)
  {
    short numSVsAboveElv = 0;
    short prn = 1;
    while (prn < 33)
    {
      bool NoEph = false;
      try 
      {
        if (apcFile.getCount()) {peXVT = peEphList.getPrnXvt(prn,t);}
        if (beFile.getCount()) {peXVT = beEphList.getPrnXvt(prn,t);}
      }
      catch(gpstk::Exception& e) 
      {
        if (verbose) {cout << e << endl;}
        NoEph = true;
      }
      double elvAngle = 0;
      if (!NoEph)
      {
        try {elvAngle = antPVT.x.elvAngle(peXVT.x);}
        catch(gpstk::Exception& e) {if (verbose) {cout << e << endl;}}
        if ( elvAngle > minEl ) { numSVsAboveElv++; }
      }
      prn++;
    }
    if (numSVsAboveElv > 12)
    { 
      cout << "Found " << numSVsAboveElv << " SVs above " << minEl << " degrees at " << t << endl;
      prn = 1;
      while (prn < 33)
      {
        bool NoEph = false;
        try 
        {
          if (apcFile.getCount()) {peXVT = peEphList.getPrnXvt(prn,t);}
          if (beFile.getCount()) {peXVT = beEphList.getPrnXvt(prn,t);}
        }
        catch(gpstk::Exception& e) {NoEph = true;}
        double elvAngle = 0;
        if (!NoEph)
        {
          try {elvAngle = antPVT.x.elvAngle(peXVT.x);}
          catch(gpstk::Exception& e) {if (verbose) {cout << e << endl;}}
          if ( elvAngle > 0 ) { cout << t << " - PRN " << prn << " at " << elvAngle << " degrees elevation. " << endl; }
        }
        prn++;
      }
    }
    t += 10;  
  }
  exit(0);
} 
      



