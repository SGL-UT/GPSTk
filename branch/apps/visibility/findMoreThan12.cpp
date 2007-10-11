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

#include <iostream>
#include <iomanip>

#include "DayTime.hpp"
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"
#include "EphReader.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char *argv[])
{
  CommandOptionNoArg
    helpOption('h',"help", "Print help usage."),
    verboseOption('v',"verbose","Increase verbosity.");

  CommandOptionWithAnyArg 
    ephFiles('e',"eph-files","Ephemeris source file(s). Can be RINEX nav, SP3, or FIC.",true),
    antennaPosition('p',"position","Antenna position in ECEF (x,y,z) coordinates.  Format as a string: \"X Y Z\".",true);
    
  CommandOptionWithNumberArg 
    minElev('m',"min-elev","Give an integer for the elevation (degrees) above which you want to find more than 12 SVs at a given time.", true);     
      
  CommandOptionParser cop("Find when there are simultaneously more than 12 SVs above a given elevation.");
    cop.parseOptions(argc, argv);
    
  // verify that the correct input was given
  if (helpOption.getCount())
  {
    cop.displayUsage(cout);
    exit(0);
  }
  
  if (cop.hasErrors())
  {
    cop.dumpErrors(cout);
    cop.displayUsage(cout);
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
  EphReader ephReader;
  ephReader.verboseLevel = verbose;
  for (int i=0; i<ephFiles.getCount(); i++)
      ephReader.read(ephFiles.getValue()[i]);
  
  gpstk::XvtStore<SatID>& ephStore = *ephReader.eph;
  
  // get the antenna position
  Xvt antPVT;
  double x,y,z;
  sscanf(antennaPosition.getValue().front().c_str(),"%lf %lf %lf", &x, &y, &z);
  antPVT.x[0] = x; antPVT.x[1] = y; antPVT.x[2] = z;
  ECEF antPos = Triple(antPVT.x);
  
  // get initial and final times for analysis. extra code b/c sscanf reads in int's but setYMDHMS needs shorts
  short year, month, day, hour, minute;
  double seconds;

  DayTime tStartDT = ephStore.getInitialTime();
  year = tStartDT.year();
  month = tStartDT.month();
  day = tStartDT.day();
  hour = tStartDT.hour();
  minute = tStartDT.minute();
  seconds = tStartDT.second();
  DayTime tstart;
  tstart.setYMDHMS(year,month,day,hour,minute,seconds);
  
  DayTime tEndDT = ephStore.getFinalTime();
  year = tEndDT.year();
  month = tEndDT.month();
  day = tEndDT.day();
  hour = tEndDT.hour();
  minute = tEndDT.minute();
  seconds = tEndDT.second();
  DayTime tend;
  tend.setYMDHMS(year,month,day,hour,minute,seconds);
  
  DayTime t = tstart;

  cout << "Start Time: " << tstart << " End Time: " << tend << endl;
  
  while (t < tend)
  {
    short numSVsAboveElv = 0;
    short prn = 1;
    while (prn <= gpstk::MAX_PRN)
    {
      Xvt peXVT;
      bool NoEph = false;
      try
      {
        peXVT = ephStore.getXvt(SatID(prn, SatID::systemGPS),t);
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
      while (prn < gpstk::MAX_PRN)
      {
        bool NoEph = false;
        Xvt peXVT;
        try 
        {
          peXVT = ephStore.getXvt(SatID(prn,SatID::systemGPS),t);
        }
        catch(gpstk::Exception& e) {NoEph = true;}
        double elvAngle = 0;
        double azAngle = 0;
        if (!NoEph)
        {
          try {elvAngle = antPVT.x.elvAngle(peXVT.x);}
          catch(gpstk::Exception& e) {if (verbose) {cout << e << endl;}}
          if ( elvAngle > 0 ) 
          { 
            cout << t << "  PRN " << setw(2) << prn << " : elev: " << elvAngle;
            try {azAngle = antPVT.x.azAngle(peXVT.x);}
            catch(gpstk::Exception& e) {if (verbose) {cout << e << endl;}}
            if ( azAngle > 0 ) { cout << "  azim: " << azAngle; }
            cout << " degrees\n";
          }
        }
        prn++;
      }
    }
    t += 10;  
  }
  exit(0);
} 
      



