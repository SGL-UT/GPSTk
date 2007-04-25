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

//
//   Computes SV position (Earth-fixed) and SV clock
//   correction.  If given a user position, computes 
//   azimuth, elevation and range to SV.
//

#include <iostream>
#include <iomanip>
#include <fstream>

#include "DayTime.hpp"
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"
#include "WGS84Geoid.hpp"
#include "EphReader.hpp"

using namespace std;
using namespace gpstk;
 

int main(int argc, char *argv[])
{
  CommandOptionNoArg
    helpOption('h',"help", "Print help usage.");

 CommandOptionWithAnyArg 
    ephFiles('e',"eph-files","Ephemeris source file(s). Can be RINEX nav, SP3, or FIC.",true),
    PositionOption('u',"position","Antenna position in ECEF (x,y,z) coordinates.  Format as a string: "
            "\"X Y Z\". Used to give user-centered data (SV range, azimuth & elevation)."),
    StartTimeOption('s',"start","Starting time for analysis. Format as string: \"MO/DD/YYYY HH:MM:SS\"."
            " If no start time is specified, the default is end of                  file."),
    EndTimeOption('f',"end", "End time for analysis.  Format as string: \"MO/DD/YYYY HH:MM:SS\". If no"
            " end time is specified, the default is beginning of file."),
    MatlabOption('o',"output-filename", "Filename for results to be output in MATLAB suitable file.");

 CommandOptionWithNumberArg 
    MyPrnOption('p',"prn","Which SV to analyze.",true),
    TimeIncrementOption('t',"time","Time increment for ephemeris calculation.  Enter increment in "
            "seconds.  Default is 900 (15 min).");     

 CommandOptionParser cop("Computes the ephemeris data for a single prn.");
 cop.parseOptions(argc, argv);

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
 
 short myprn = StringUtils::asInt((MyPrnOption.getValue())[0]); 
 if ((myprn > gpstk::MAX_PRN) || (myprn < 1))
 {
   cerr << "Enter a valid PRN number.\n\n";
   exit(0);
 }
 
 std::ofstream ofs;
 if (MatlabOption.getCount())
 {
    const string fn=MatlabOption.getValue()[0];
      ofs.open(fn.c_str(), ios::out);
    if (PositionOption.getCount()) 
      ofs << "#10-bit GPS week, GPS seconds of week,X,Y,Z(m),Clock Corr.(sec), azimuth (deg), "
          << "elevation (deg), range (m)" << endl; 
 }
 else 
    ofs << "#10-bit GPS week, GPS seconds of week,X,Y,Z(m),Clock Corr.(sec)" << endl;
 
 int incr=900;
 if (TimeIncrementOption.getCount())
     incr = gpstk::StringUtils::asInt((TimeIncrementOption.getValue())[0]);
 
 Xvt ANTPVT;
 if (PositionOption.getCount())
 {
    double x, y, z;
    sscanf(PositionOption.getValue().front().c_str(),"%lf %lf %lf", &x, &y, &z);
    ANTPVT.x[0] = x; ANTPVT.x[1] = y; ANTPVT.x[2] = z;
    cout << endl << endl << setw(20) << left << " Antenna Position: " << ANTPVT.x[0] 
         << "  " << ANTPVT.x[1] << "  " << ANTPVT.x[2] << endl;
 }
 
 // get the ephemeris source(s)
  EphReader ephReader;
  for (int i=0; i<ephFiles.getCount(); i++)
      ephReader.read(ephFiles.getValue()[i]);
  
  gpstk::EphemerisStore& ephStore = *ephReader.eph;
  
  DayTime tStart,tEnd;
  if (StartTimeOption.getCount())
  {
      double ss;
      int mm,dd,yy,hh,minu; 
      sscanf(StartTimeOption.getValue().front().c_str(), "%i/%i/%i %i:%i:%lf",&mm,&dd,&yy,&hh,&minu,&ss);
      tStart.setYMDHMS((short)yy, (short)mm, (short)dd, (short)hh, (short)minu, (double)ss);
  }	
  else
  {
      DayTime tFile(ephStore.getInitialTime());	  //extra code b/c sscanf reads in int's but setYMDHMS needs shorts
      short year = tFile.year();
      short month = tFile.month();
      short day = tFile.day();
      short hour = tFile.hour();
      short minute = tFile.minute();
      double ss = tFile.second();
      tStart.setYMDHMS(year,month,day,hour,minute,ss);
  }
 
  if (EndTimeOption.getCount())
  {
      double ss;
      int mm,dd,yy,hh,minu; 
      sscanf(EndTimeOption.getValue().front().c_str(), "%i/%i/%i %i:%i:%lf",&mm,&dd,&yy,&hh,&minu,&ss);
      tEnd.setYMDHMS((short)yy, (short)mm, (short)dd, (short)hh, (short)minu, (double)ss);
  }
  else
  {
      DayTime tFile(ephStore.getFinalTime());		//extra code b/c sscanf reads in int's but setYMDHMS needs shorts
      short year = tFile.year();
      short month = tFile.month();
      short day = tFile.day();
      short hour = tFile.hour();
      short minute = tFile.minute();
      double ss = tFile.second();
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
  
  DayTime t = tStart;

  SatID sat(myprn,SatID::systemGPS);
  
  int count = 0;
  while (t < tEnd)
  {
      try
      {
        Xvt BCPVT = ephStore.getSatXvt(sat, t);
        cout  << " "  << left << setw(20) << t
              << fixed
              << " "  << left << setw(20) << BCPVT.x[0]
              << " "  << left << setw(20) << BCPVT.x[1]
              << " "  << left << setw(20) << BCPVT.x[2]
              << " "  << left << setw(20) << BCPVT.dtime << endl;
        count++;
      }
      catch (gpstk::EphemerisStore::NoEphemerisFound& e) { ; }
      t += incr;
  }
  if (PositionOption.getCount())
  {
      ECEF rxPos = Triple(ANTPVT.x);
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
          Xvt BCPVT = ephStore.getSatXvt(sat, t);
          WGS84Geoid geoid;
          double correction = (BCPVT.dtime) * (geoid.c());
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
          Xvt BCPVT = ephStore.getSatXvt(sat, t);
          WGS84Geoid geoid;
          double correction = (BCPVT.dtime) * (geoid.c());
          
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
                    << " "  << left << setw(15) << BCPVT.preciseRho(Triple(ANTPVT.x), geoid, correction);
              }
              ofs << endl;
        }
        catch (gpstk::Exception& e) { ; }
        t += incr;
      }
    }
  
  if (count==0) { cout << endl << endl << " No epochs for prn " << myprn << "!" << endl << endl; }
  else { cout << endl <<  " Calculated " << count << " increments for prn " << myprn << " ." << endl<< endl << endl; }
  
  ofs.close();
  exit(0);

}

