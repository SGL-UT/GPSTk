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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
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
#include <set>

#include "CommonTime.hpp"
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"
#include "WGS84Ellipsoid.hpp"
#include "EphReader.hpp"
#include "CivilTime.hpp"
#include "TimeString.hpp"
#include "Xvt.hpp"
using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;
 

int main(int argc, char *argv[])
{
  CommandOptionNoArg
    helpOption('h',"help", "Print help usage.");

  CommandOptionWithAnyArg 
    ephFiles('e',"eph-files","Ephemeris source file(s). Can be RINEX nav, SP3, or FIC.",true),
    positionOpt('u',"position","Antenna position in ECEF (x,y,z) coordinates.  "
                "Format as a string: \"X Y Z\". Used to give user-centered data "
                "(SV range, azimuth & elevation) when SV is in view."),
    startTimeOpt('\0',"start","Ignore data before this time. Format as string: "
                 "\"MO/DD/YYYY HH:MM:SS\"."),
    endTimeOpt('\0',"end", "Ignore data after this time. Format as string: "
               "\"MO/DD/YYYY HH:MM:SS\"."),
    formatOpt('f',"time-format","CommonTime format specifier used for times in the output. "
              "The default is \"%4Y %3j %02H:%02M:%04.1f\".");

  CommandOptionWithNumberArg 
    prnOpt('p',"prn","Which SVs to analyze. Repeat option for multiple satellites. "
           "If this option is not specified, all ephemeris data will be processed."),
    incrementOpt('t',"time","Time increment for ephemeris calculation. "
                 "Enter increment in seconds. Default is 900 (15 min).");     

  CommandOptionParser cop("Computes ephemeris data.");
  cop.parseOptions(argc, argv);

  if (helpOption.getCount())
    {
      cop.displayUsage(cerr);
      exit(0);
    }

  if (cop.hasErrors())
    {
      cop.dumpErrors(cerr);
      cop.displayUsage(cerr);
      exit(0);
    }

  cout << "Scanning over prnSet." << endl;
  set<int> prnSet;
  size_t index;
  for (index = 0; index < prnOpt.getCount(); index++)
    {
      int prn = asInt(prnOpt.getValue()[index]);
      if ((prn > gpstk::MAX_PRN) || (prn < 1))
        {
          cerr << "An invalid PRN number was entered.\n";
          exit(0);
        }
      else
        {
          prnSet.insert(prn);
        }
    }
  cout << "Scan complete, size = " << prnSet.size() << endl;

  cout << "Scanning over PRNs indices." << endl;
  if (index == 0)
    for (index = 1; index <= gpstk::MAX_PRN; index++)
      prnSet.insert(index);
  cout << "Scan complete, size = " << prnSet.size() << endl;

  std::string timeFormat;
  if (formatOpt.getCount())
    timeFormat = formatOpt.getValue()[0];
  else
//    timeFormat = "%4Y %3j %02H:%02M:%04.1f";
    timeFormat = "%02m/%02d/%04Y %02H:%02M:%04.1f";
  cout << "Set timeFormat to " << timeFormat << endl;

  cout << "positionOpt has count = " << positionOpt.getCount() << endl;

  cout << "# time, PRN, X(m), Y(m), Z(m), Clock Correction(ms)";

  Xvt antXvt;	
  if (positionOpt.getCount())
    {
      double x, y, z;
      sscanf(positionOpt.getValue().front().c_str(),"%lf %lf %lf", &x, &y, &z);
      antXvt.x[0] = x; antXvt.x[1] = y; antXvt.x[2] = z;
      cout << ", Azimuth(deg), Elevation(deg), Range(m)";
    }
  cout << endl;

  int incr=900;
  if (incrementOpt.getCount())
    incr = asInt(incrementOpt.getValue()[0]);

  // get the ephemeris source(s)
  EphReader ephReader;
  for (size_t i=0; i<ephFiles.getCount(); i++)
    {
      ephReader.read(ephFiles.getValue()[i]);
      cout << "File read by EphReader." << endl;
    }
  XvtStore<SatID>& ephStore = *ephReader.eph;

  CivilTime tS, tE;
  CommonTime tStart,tEnd;
  if (startTimeOpt.getCount())
    {
      double ss;
      int mm,dd,yy,hh,minu; 
      sscanf(startTimeOpt.getValue().front().c_str(), 
             "%d/%d/%d %d:%d:%lf",&mm,&dd,&yy,&hh,&minu,&ss);
      tS=CivilTime((short)yy, (short)mm, (short)dd, (short)hh, 
                       (short)minu, (double)ss, TimeSystem::GPS);
      cout << tS << endl;
    }
  else
    {
      //extra code b/c sscanf reads in int's but CivilTime needs shorts
      cout << ephStore.getInitialTime() << endl;
      CommonTime tFile(ephStore.getInitialTime());	  
      short year = static_cast<CivilTime>(tFile).year;
      short month = static_cast<CivilTime>(tFile).month;
      short day = static_cast<CivilTime>(tFile).day;
      short hour = static_cast<CivilTime>(tFile).hour;
      short minute = static_cast<CivilTime>(tFile).minute;
      double ss = static_cast<CivilTime>(tFile).second;
      tS = CivilTime(year,month,day,hour,minute,ss, TimeSystem::GPS);
      tStart = tS.convertToCommonTime();
      cout << tStart << endl;
    }

  if (endTimeOpt.getCount())
    {
      double ss;
      int mm,dd,yy,hh,minu; 
      sscanf(endTimeOpt.getValue().front().c_str(), 
             "%d/%d/%d %d:%d:%lf",&mm,&dd,&yy,&hh,&minu,&ss);
      tE = CivilTime((short)yy, (short)mm, (short)dd, (short)hh, (short)minu, 
                     (double)ss, TimeSystem::GPS);
      tEnd = tE.convertToCommonTime();
      cout << tEnd << endl;
    }
  else
    {
      //extra code b/c sscanf reads in int's but CivilTime needs shorts
      CommonTime tFile(ephStore.getFinalTime());	
      short year = static_cast<CivilTime>(tFile).year;
      short month = static_cast<CivilTime>(tFile).month;
      short day = static_cast<CivilTime>(tFile).day;
      short hour = static_cast<CivilTime>(tFile).hour;
      short minute = static_cast<CivilTime>(tFile).minute;
      double ss = static_cast<CivilTime>(tFile).second;
      tE = CivilTime(year,month,day,hour,minute,ss, TimeSystem::GPS);
      tEnd = tE.convertToCommonTime();
      cout << tEnd << endl;
    }

  CommonTime t = tStart;

  while (t <= tEnd)
    {
      for(set<int>::iterator i = prnSet.begin(); i != prnSet.end(); i++ )
        {
          SatID thisSat(*i,SatID::systemGPS); 			
          try
            {
              Xvt xvt = ephStore.getXvt(thisSat, t);
              cout << printTime(t,timeFormat) << right << fixed  << setprecision(3)  
                   << " " << setw(3)  <<  *i
                   << " " << setw(14) << xvt.x[0]
                   << " " << setw(14) << xvt.x[1]
                   << " " << setw(14) << xvt.x[2]
                   << " " << setprecision(6) << setw(10)  << ((xvt.clkdrift + xvt.relcorr)*1000);

              WGS84Ellipsoid ellipsoid;
              double correction = ((xvt.clkdrift + xvt.relcorr)) * (ellipsoid.c());

              if ( abs(antXvt.x[0]) < 1 || antXvt.x.elvAngle(xvt.x) < 0 )
                {
                  cout << right
                       << " "  << setw(8) << "-"
                       << " "  << setw(8) << "-"
                       << " "  << setw(15) << "-" << endl;	
                }
              else
                {
                  cout << right << fixed << setprecision(3)
                       << " "  << setw(8) << antXvt.x.azAngle(xvt.x)
                       << " "  << setw(8) << antXvt.x.elvAngle(xvt.x)
                       << " "  << setw(15) 
                       << xvt.preciseRho(antXvt.x, ellipsoid, correction) << endl;
                }
            }
          catch(...)
            {
              ;
            }
        }

      t += incr;
    }

  exit(0);
}
