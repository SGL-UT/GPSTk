//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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

/*
The start of a program that takes correlation delays, a rough estimate of
time, a rough estimate of receiver position, and an SV ephemeris. It then
tries to estimate the pseudoranges from the correlation delays.
*/

#include <iostream>

#include <CommonTime.hpp>
#include <CommandOption.hpp>
#include <CommandOptionWithTimeArg.hpp>
#include <CommandOptionParser.hpp>

#include <GPSEphemerisStore.hpp>
#include <RinexNavStream.hpp>
#include <RinexNavData.hpp>
#include <TropModel.hpp>
#include <IonoModel.hpp>
#include <GPSEllipsoid.hpp>
#include <PRSolution2.hpp>
#include <Position.hpp>
#include "Xvt.hpp"

using namespace gpstk;
using namespace std;
using namespace gpstk::StringUtils;

int main(int argc, char *argv[])
{
   int verbosity = 1;
   Triple antennaPos;

   CommandOptionWithAnyArg
      ephFileOption('e', "ephemeris", "Rinex Ephemeris data file name.", true);

   CommandOptionNoArg
      helpOption('h', "help", "Print usage. Repeat for more info. "),

      verbosityOption('v', "verbosity", "Increase the verbosity level. The default is 0.");

   CommandOptionWithAnyArg
      antennaPosOption('p', "position", "Initial estimate of the antenna position in ECEF. Only needs to be good to the km level.");

   CommandOptionWithTimeArg
      timeOption('t', "time", "%m/%d/%Y %H:%M:%S",
                 "Time estimate for start of data (MM/DD/YYYY HH:MM:SS).");

   string appDesc("Performs a simple nav solution from correlation delays.");
   CommandOptionParser cop(appDesc);
   cop.parseOptions(argc, argv);

   if (helpOption.getCount() || cop.hasErrors())
   {
      if (cop.hasErrors() && helpOption.getCount()==0)
      {
         cop.dumpErrors(cout);
         cout << "Use -h for help." << endl;
      }
      else
      {
         cop.displayUsage(cout);
      }
      exit(0);
   }

   if (verbosityOption.getCount())
      verbosity = asInt(verbosityOption.getValue()[0]);


   if (antennaPosOption.getCount())
   {
      string aps = antennaPosOption.getValue()[0];
      if (numWords(aps) != 3)
      {
         cout << "Please specify three coordinates in the antenna postion." << endl;
         exit(-1);
      }
      else
         for (int i=0; i<3; i++)
            antennaPos[i] = asDouble(word(aps, i));
   }

   GPSEphemerisStore bce;
   IonoModel iono;
   for (int i=0; i < (int)ephFileOption.getCount(); i++)
   {
      string fn = ephFileOption.getValue()[i];
      RinexNavStream rns(fn.c_str(), ios::in);
      rns.exceptions(ifstream::failbit);

      RinexNavHeader hdr;
      rns >> hdr;
      iono = IonoModel(hdr.ionAlpha, hdr.ionBeta);

      RinexNavData rnd;
      while (rns >> rnd)
         bce.addEphemeris(rnd);

      if (verbosity)
         cout << "Read " << fn << " as RINEX nav. " << endl;
   }

   if (verbosity>1)
      cout << "Have ephemeris data from " << bce.getInitialTime() 
           << " through " << bce.getFinalTime() << endl;

   CommonTime time = timeOption.getTime()[0];
   if (verbosity)
      cout << "Initial time estimate: " << time << endl;

   if (time < bce.getInitialTime() || time > bce.getFinalTime())
      cout << "Warning: Initial time does not appear to be within the provided ephemeris data." << endl;


   GPSEllipsoid gm;
   Position ecef(antennaPos);
   map<SatID, double> range;
   vector<SatID> svVec;
   vector<double> expVec, ionoVec;
   for (int i=1; i<=32; i++)
   {
      SatID sv(i, SatID::systemGPS);
      try 
      {
         Xvt svpos = bce.getXvt(sv, time);
         double el = antennaPos.elvAngle(svpos.x);
         double az = antennaPos.azAngle(svpos.x);

         double pr = svpos.preciseRho(ecef, gm, 0);
         double ic = iono.getCorrection(time, ecef, el, az);

         expVec.push_back(pr);
         svVec.push_back(sv);
         ionoVec.push_back(ic);
      }
      catch (Exception& e)
      {}
   }

   // Replace this with the observed delays...
   vector<double> obsVec(expVec);

   try 
   {
      GGTropModel gg;
      gg.setWeather(20., 1000., 50.);    
      PRSolution2 prSolver;
      prSolver.RMSLimit = 400;
      prSolver.RAIMCompute(time, svVec, obsVec, bce, &gg);
      Vector<double> sol = prSolver.Solution;
      cout << "solution:" << fixed << sol << endl;
   }   
   catch (Exception& e)
   {
      cout << "Caught exception:" << e << endl;
   }
}
