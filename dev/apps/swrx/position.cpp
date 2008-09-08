#pragma ident "$Id$"

/*

TO DO:

parameterize input

convert zcount into time instead of making user figure it out?

pull pseudoranges from PRSolve so we have more than just a final position

use clock error to calculate receive time (a bit tricky since we're assuming
all subframes arrive at the same time, and they don't).

add ionosphere correction?

*/


/* 
Program takes information from our tracker (subframe information like 
zcount, starting data point of the sf, sf #, prn) and an ephemeris file.  
(We have the values to generate our own ephemeris, just not the formatting code)
We then solve for position.  Not sure if we want to stay with that or generate a RINEX file, and I also don't know if I can generate correct pseudoranges
(e.g. ones without 1 ms ambiguity) without solving for position.
*/

#include <iostream>

#include <DayTime.hpp>
#include <CommandOption.hpp>
#include <CommandOptionWithTimeArg.hpp>
#include <CommandOptionParser.hpp>

#include "BasicFramework.hpp"
#include "CommandOption.hpp"
#include "StringUtils.hpp"
#include "icd_200_constants.hpp"

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
   double sampleRate;
   
   GPSEphemerisStore bce;
   IonoModel iono;
   DayTime time;

   GPSGeoid gm;
   ECEF ecef;
   map<SatID, double> range;
   vector<SatID> svVec;
};

//-----------------------------------------------------------------------------
P::P() throw() :
      BasicFramework("position", "A calculation of position using tracker data")
      , sampleRate(16.368)
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

   CommandOptionWithTimeArg
      timeOption('t', "time", "%m/%d/%Y %H:%M:%S",
                 "Time estimate for start of data (MM/DD/YYYY HH:MM:SS).");

   if (!BasicFramework::initialize(argc,argv)) 
      return false;

   if (sampleRateOpt.getCount())
   {
      sampleRate = asDouble(sampleRateOpt.getValue().front());
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

   time = timeOption.getTime()[0];
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
/*   //subframe 2 data points from gnssGood.bin (coords within 60 meters)
   dataPoints[3]=14155198; 
   dataPoints[7]=13996008;
   dataPoints[16]=13994328;
   dataPoints[10]=14074723;
   dataPoints[25]=14143199;
   dataPoints[26]=14089410;
   dataPoints[27]=14010599;*/
/*
  // subframe three data points (not very good pos)
   dataPoints[3]=112360974; 
   dataPoints[7]=112202168;
   dataPoints[16]=112200312;
   dataPoints[10]=112280883;
   dataPoints[25]=112349295;
   dataPoints[26]=112295602;
   dataPoints[27]=112216567;
*/

  // subframe four data points (coords all within 35 meters)
   dataPoints[3]=210566750; 
   dataPoints[7]=210408312;
   dataPoints[16]=210406280;
   dataPoints[10]=210487043;
   dataPoints[25]=210555407;
   dataPoints[26]=210501810;
   dataPoints[27]=210422535;
   // sample usage (for gnssGood.bin and a rinex nav file from the date shown)
   // ./position -e rin_207.08n -t "07/25/2008 01:05:18"

   for(int i=0; i<32;i++)
   {
      if(dataPoints[i] != 0)
      {
         refDataPoint = dataPoints[i];
         break;
      }
      
   }

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
         svVec[i] = temp; // set PRN equal to 0, the SV won't be considered
      }
   }

   if(verboseLevel)
   {
      for(int i = 0; i < 32; i++)
         cout << svVec[i] << " "  << obsVec[i] << endl;
   }

   GGTropModel gg;
   gg.setWeather(20., 1000., 50.);    
   PRSolution prSolver;
   prSolver.RMSLimit = 400;
   prSolver.RAIMCompute(time, svVec, obsVec, bce, &gg);
   Vector<double> sol = prSolver.Solution;

   cout << endl << "Position (ECEF): " << fixed << sol[0] << " " << sol[1] 
        << " " << sol[2] << endl << "Receiver Clock Error: " 
        << sol[3]/gpstk::C_GPS_M << " seconds (convert to current receiver time...includes error caused by blindly picking reference pseudorange)" << endl;
   cout << "Covariance: " << fixed << setprecision(3) << endl 
        << prSolver.Covariance << endl
        << "# good SV's: " << prSolver.Nsvs << endl
        << "RMSResidual: " << prSolver.RMSResidual << " meters" << endl << endl;
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
