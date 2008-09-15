#pragma ident "$Id$"

/*
TO DO:

parameterize input (want to input each subframe, or try to correlate with tracker output files?)

pull pseudoranges from PRSolve so we have more than just a final position
*/


/* 
Program takes information from our tracker (subframe information like 
zcount, starting data point of the sf, sf #, prn) and an ephemeris file.  
(We have the values to generate our own ephemeris, just not the formatting code)
We then solve for position.  Not sure if we want to stay with that or generate a
 RINEX file.
*/

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

/*
   //subframe 2 data points from gnssGood.bin (coords within 60 meters)
   dataPoints[3]=14155198; 
   dataPoints[7]=13996008;
   dataPoints[16]=13994328;
   dataPoints[10]=14074723;
   dataPoints[25]=14143199;
   dataPoints[26]=14089410;
   dataPoints[27]=14010599;
*/
/*
  // subframe three data points (not very good pos)
   dataPoints[3]=112360974; 
   dataPoints[7]=112202168;
   dataPoints[16]=112200312; //***
   dataPoints[10]=112280883;
   dataPoints[25]=112349295;
   dataPoints[26]=112295602; //***
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
   refDataPoint = total/numberSVs; // average data point.
   
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
   
// Recalculate position using time corrected by clock error.
   time += (0.073 + sol[3] / gpstk::C_GPS_M);
   GGTropModel gg2;
   gg2.setWeather(30.,1000., 50.); /*(Temp(C),Pressure(mbar),Humidity(%))*/    
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

// Following block will make PRSolve compute residual from a known hardcoded
// position
   PRSolution prSolver3; 
   vector<double> S;
   S.push_back(-756736.1300);
   S.push_back(-5465547.0217);
   S.push_back(3189100.6012);
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
