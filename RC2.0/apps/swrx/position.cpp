#pragma ident "$Id$"

/* 
Position solution.  Hardcode output data from tracker into arrays below. 

Program takes information from our tracker (subframe information like 
zcount, starting data point of the sf, sf #, prn) and an ephemeris file.  
(We have the values to generate our own ephemeris, just not the formatting code)
We then solve for position.
*/

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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include "BasicFramework.hpp"
#include "CommandOption.hpp"
#include "icd_200_constants.hpp"
#include <iostream>
#include <DayTime.hpp>
#include <GPSEphemerisStore.hpp>
#include <RinexNavStream.hpp>
#include <RinexNavData.hpp>
#include <TropModel.hpp>
#include <IonoModel.hpp>
#include <GPSGeoid.hpp>
#include <PRSolution.hpp>

using namespace gpstk;
using namespace std;

class P : public BasicFramework
{
public:
   P() throw();

   bool initialize(int argc, char *argv[]) throw();

protected:
   virtual void process();

private:
   GPSEphemerisStore bce;
   IonoModel iono;
   DayTime time;

   double zCount;
   int gpsWeek;
   double sampleRate;

   GPSGeoid gm;
   vector<SatID> svVec;
   vector<double> ionoVec;
   Triple antennaPos;
};

//-----------------------------------------------------------------------------
P::P() throw() :
      BasicFramework("position", "A calculation of position using tracker data")
      , sampleRate(16.367667)
{}

bool P::initialize(int argc, char *argv[]) throw()
{
   using namespace gpstk::StringUtils;

   CommandOptionWithAnyArg
      ephFileOption('e', "ephemeris", "Rinex Ephemeris data file name.", true);

   CommandOptionWithAnyArg
      sampleRateOpt('r',"sample-rate",
                    "Specifies the nominal sample rate, in MHz.  The "
                    "default is 16.368 MHz.");

   CommandOptionWithAnyArg
      zCountOpt('z',"z-count",
                "The Z-Count of the subframe to be used.", true);

   CommandOptionWithAnyArg
      gpsWeekOpt('w',"gps-week",
                "The GPSWeek", true);

   if (!BasicFramework::initialize(argc,argv)) 
      return false;

   if (sampleRateOpt.getCount())
   {
      sampleRate = asDouble(sampleRateOpt.getValue().front());
   }
   if(gpsWeekOpt.getCount())
   {
         gpsWeek = asInt(gpsWeekOpt.getValue().front());
   }

   if(zCountOpt.getCount())
   {
         zCount = asDouble(zCountOpt.getValue().front());
         // zCount is the time of transmission of the next subframe.
         zCount -= 6.0; 
         DayTime t(gpsWeek,zCount);
         time = t;
   }
 
   for (int i=0; i < ephFileOption.getCount(); i++)
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

      if (verboseLevel)
         cout << "Read " << fn << " as RINEX nav. " << endl;
   }

   if (verboseLevel>1)
      cout << "Have ephemeris data from " << bce.getInitialTime() 
           << " through " << bce.getFinalTime() << endl;

   if (verboseLevel)
      cout << "Initial time estimate: " << time << endl;

   if (time < bce.getInitialTime() || time > bce.getFinalTime())
      cout << "Warning: Initial time does not appear to be within the provided ephemeris data." << endl;

   return true;
}


void P::process()
{
   for (int i=1; i < 33; i++)
   {
      SatID sv(i, SatID::systemGPS);
      svVec.push_back(sv);
   }

   vector<int> dataPoints(32);
   float refDataPoint;
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// Following tables hold the data sets that we have for now.
// NOW CARDATA.bin
/*
// subframe three data points from gnssDavisHouseCar2.bin: (z=360198) CRUMMY
   dataPoints[1]=29487862; 
   dataPoints[9]=29415434;
   dataPoints[14]=29393435;
   dataPoints[23]=29360589;
   dataPoints[25]=29365069;
   dataPoints[28]=29328671;
   dataPoints[29]=29471399;
*/
// position -e rin269.08n -z 360204 -w 1498
/*
// sf3 
// position -e rin269.08n -z 360204 -r 8.184 -w 1498
// GOOD results: rms 55 rmsknown 62
   dataPoints[1]=14743933; 
   dataPoints[9]=14707720;
   dataPoints[14]=14696721;
   dataPoints[17]=14745155;
   dataPoints[23]=14680297;
   dataPoints[25]=14682538;
   dataPoints[28]=14662516;
   dataPoints[29]=14735701;
*/
/*
// sf4 GOOD ALSO
   dataPoints[1]=63847008; 
   dataPoints[9]=63810816;
   dataPoints[14]=63799663;
   dataPoints[17]=63848083;
   dataPoints[23]=63783288;
   dataPoints[25]=63785487;
   dataPoints[28]=63765493;
   dataPoints[29]=63838790;
*/
/*
// sf 5   90 meters
   dataPoints[1]=112950083; 
   dataPoints[9]=112913912;
   dataPoints[14]=112902598;
   dataPoints[17]=112951011;
   dataPoints[23]=112886279;
   dataPoints[25]=112888436;
   dataPoints[28]=112868477;
   dataPoints[29]=112941879;
*/
/*
// sf 1  140 m, 220 m (only 7 sats)
   dataPoints[1]=162053158; 
   dataPoints[9]=162017008;
   dataPoints[14]=162005540;
   dataPoints[17]=162053932;
   dataPoints[23]=161989277;
   dataPoints[25]=161991385;
   dataPoints[29]=162044968;
*/
/*
// sf 2   90m and 130m
   dataPoints[1]=211156226; 
   dataPoints[9]=211120111;
   dataPoints[14]=211108482;
   dataPoints[17]=211156860;
   dataPoints[23]=211092268;
   dataPoints[25]=211094333;
   dataPoints[29]=211148057;
*/
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

   long int total = 0;
   int numberSVs = 0;
   for(int i=0; i<32;i++)
   {

      /* // This code uses first PRN found as reference data point.
      if(dataPoints[i] != 0)
      {
         refDataPoint = dataPoints[i];
         break;
      }*/

      total += dataPoints[i];
      if(dataPoints[i] != 0)
         numberSVs++;
   }
   refDataPoint = total/numberSVs; // average data point to be reference.
   
   vector<double> obsVec(32);
   for(int i=0; i<32; i++)
   {
      if(dataPoints[i] != 0)
      {
         // 0.073 is an arbitrary guessed time of flight
         obsVec[i] = gpstk::C_GPS_M*(0.073 - (refDataPoint - 
                                dataPoints[i])/(1000*sampleRate*1000));
      }  
      else
      {
         SatID temp(0, SatID::systemGPS); 
         svVec[i] = temp; // set SatID equal to 0, the SV won't be considered
      }
   }

   if(verboseLevel)
   {
      for(int i = 0; i < 32; i++)
         cout << svVec[i] << " "  << obsVec[i] << endl;
   }

//-----------------------------------------------------------------------------
// Calculate initial position solution.
   GGTropModel gg;
   gg.setWeather(30., 1000., 50.);    
   PRSolution prSolver;
   prSolver.RMSLimit = 400;
   prSolver.RAIMCompute(time, svVec, obsVec, bce, &gg);
   Vector<double> sol = prSolver.Solution;

   cout << endl << "Position (ECEF): " << fixed << sol[0] << " " << sol[1] 
        << " " << sol[2] << endl << "Clock Error (includes that caused by guess): " 
        << sol[3]*1000/gpstk::C_GPS_M << " ms" << endl;
   cout << "# good SV's: " << prSolver.Nsvs << endl
        << "RMSResidual: " << prSolver.RMSResidual << " meters" << endl << endl;
//----------------------------------------------------------------------------- 
// Calculate Ionosphere correction.
   antennaPos[0] = sol[0];
   antennaPos[1] = sol[1];
   antennaPos[2] = sol[2];
   ECEF ecef(antennaPos);
   for (int i=1; i<=32; i++)
   {
      SatID sv(i, SatID::systemGPS);
      try 
      {
         Xvt svpos = bce.getXvt(sv, time);
         double el = antennaPos.elvAngle(svpos.x);
         double az = antennaPos.azAngle(svpos.x);
         double ic = iono.getCorrection(time, ecef, el, az); // in meters
         ionoVec.push_back(ic);
      }
      catch (Exception& e)
      {}
   }
   if(verboseLevel)
   {
      for(int i = 0; i < 32; i++)
      {
         cout << svVec[i] << " "  << obsVec[i] << " " << ionoVec[i] << endl;
         
      }
   }
   for(int i=0;i<32;i++)
   {
      obsVec[i] -= sol[3]; // convert pseudoranges to ranges
      obsVec[i] += ionoVec[i]; // make iono correction to ranges.
   }

//----------------------------------------------------------------------------- 
// Recalculate position using time corrected by clock error + ionosphere.
   time -= (sol[3] / gpstk::C_GPS_M);
   GGTropModel gg2;
   gg2.setWeather(30.,1000., 20.); /*(Temp(C),Pressure(mbar),Humidity(%))*/    
   PRSolution prSolver2;
   prSolver2.RMSLimit = 400;
   prSolver2.RAIMCompute(time, svVec, obsVec, bce, &gg2);
   Vector<double> sol2 = prSolver2.Solution;
   cout << "Recomputing position with refined time and ionosphere correction:" 
        << fixed << setprecision(6);
   cout << endl << "Position (ECEF): " << fixed << sol2[0] << " " << sol2[1] 
        << " " << sol2[2] << endl << "Clock Error: " 
        << sol2[3]*1e6/gpstk::C_GPS_M << " us" << endl;
   cout << "# good SV's: " << prSolver2.Nsvs << endl
        << "RMSResidual: " << prSolver2.RMSResidual << " meters" << endl;

//-----------------------------------------------------------------------------
// Following block will make PRSolve compute residual from a known hardcoded
// position
   PRSolution prSolver3; 
   vector<double> S;
/*
   S.push_back(-756736.1300); // my house
   S.push_back(-5465547.0217);
   S.push_back(3189100.6012);
*/

   S.push_back(-740314.1444); // ARLSW antenna
   S.push_back(-5457066.8902);
   S.push_back(3207241.5759);

   S.push_back(0.0);
   prSolver3.Solution = S;
   prSolver3.ResidualCriterion = false;
   prSolver3.RMSLimit = 400;
   prSolver3.RAIMCompute(time, svVec, obsVec, bce, &gg2);
   cout << "RMSResidual from known position: " << prSolver3.RMSResidual
        << " meters" << endl << endl;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      P crap;
      if (!crap.initialize(argc, argv))
         exit(0);
      crap.run();
   }
   catch (gpstk::Exception &exc)
   { cerr << exc << endl; }
   catch (std::exception &exc)
   { cerr << "Caught std::exception " << exc.what() << endl; }
   catch (...)
   { cerr << "Caught unknown exception" << endl; }
}
