#pragma ident "$Id$"

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
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// Following tables hold the data sets that we have for now.
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
/*
  // subframe four data points (coords all within 35 meters)
   dataPoints[3]=210566750; 
   dataPoints[7]=210408312;
   dataPoints[16]=210406280;
   dataPoints[10]=210487043;
   dataPoints[25]=210555407;
   dataPoints[26]=210501810;
   dataPoints[27]=210422535;
   // sample usage (for gnssGood.bin and a rinex nav file from the date shown)
   // ./position -e rin207.08n -z 435924 -w 1489
*/
//---------------------------------------------------------------------------
/*
// subframe three data points from gnss.bin (simulation file): FAKE EPH?
   dataPoints[4]=41122671; 
   dataPoints[9]=40962424;
   dataPoints[14]=41008214;
   dataPoints[16]=40999190;
   dataPoints[17]=41024757;
   dataPoints[20]=41020645;
   dataPoints[21]=41172620;
   dataPoints[28]=41074850;
*/
//---------------------------------------------------------------------------
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
// subframe four data points from gnssDavisHouseCar2.bin: (z=360204) GOOD
/*
// position -e rin269.08n -z 360204 -w 1498
   dataPoints[1]=127694006; 
   dataPoints[9]=127621626;
   dataPoints[14]=127599323;
   dataPoints[23]=127566573;
   dataPoints[25]=127570973;
   dataPoints[28]=127530991;
   dataPoints[29]=127677575;
*/
/*
// subframe five dp's         PRETTY GOOD
   dataPoints[1]=225900166; 
   dataPoints[9]=225827834;
   dataPoints[14]=225805195;
   dataPoints[23]=225772557;
   dataPoints[25]=225776877;
   dataPoints[28]=225736943;
   dataPoints[29]=225883751;
*/
/*
// subframe one dp's   360216   CRUMMY
   dataPoints[1]=4106310; 
   dataPoints[9]=4034026;
   dataPoints[14]=4011083;
   dataPoints[23]=3982189;
   dataPoints[25]=3982765;
   dataPoints[28]=3942911;
   dataPoints[29]=4089927;
*/
/*
// subframe three dp's (z=360228)   CRUMMY
   dataPoints[1]=29487862; 
   dataPoints[9]=29415434;
   dataPoints[14]=29393435;
   dataPoints[23]=29360589;
   dataPoints[25]=29365069;
   dataPoints[28]=29328671;
   dataPoints[29]=29471399;
*/
//---------------------------------------------------------------------------
// Data Points from ARL-SW: 
// position -e rin273Sep29.08n -z 149460 -w 1499
// position -e s011273a.08n -z 149478 -w 1499
/*
// SF5 - zcount says 636966, but all or part of it is inverted...
   dataPoints[2]=63032180; 
   dataPoints[6]=62890253;
   dataPoints[7]=62986055;
   dataPoints[10]=63047187;
   dataPoints[12]=62918683;
   dataPoints[18]=62930554;
   dataPoints[22]=63017141;
   dataPoints[24]=62909420;
   dataPoints[26]=62929226;
*/

// SF1
   dataPoints[2]=161238404; 
   dataPoints[6]=161096221;
   dataPoints[7]=161191911;
   dataPoints[10]=161253027;
   dataPoints[12]=161124843;
   dataPoints[18]=161136666;
   dataPoints[22]=161223381;
   dataPoints[24]=161115452;
   dataPoints[26]=161135146;

/*
// SF2
   dataPoints[2]=259448260; 
   dataPoints[6]=259302189;
   dataPoints[7]=259401399;
   dataPoints[10]=259458867;
   dataPoints[12]=259331019;
   dataPoints[18]=259342778;
   dataPoints[22]=259433269;
   dataPoints[24]=259321468;
   dataPoints[26]=259341066;
*/
/*
// SF3
   dataPoints[2]=357650836; 
   dataPoints[6]=357508157;
   dataPoints[7]=357603607;
   dataPoints[10]=357664723;
   dataPoints[12]=357537179;
   dataPoints[18]=357548874;
   dataPoints[22]=357635877;
   dataPoints[24]=357527500;
   dataPoints[26]=357546986;
*/
//---------------------------------------------------------------------------
// Data Points from ARL-SW - 6oct08: 
// position -e u112280a.08n -z 153168 -w 1500
// GETTING SEG FAULT ON ALL OF THESE NOT SURE WHY
/*
// SF2
   dataPoints[6]=36002136; 
   dataPoints[7]=35965978;
   dataPoints[10]=36030694;
   dataPoints[16]=36084115;
   dataPoints[24]=36072115;
   dataPoints[26]=35983593;
   dataPoints[27]=36062260;
*/
/*
// SF3
   dataPoints[6]=134208248; 
   dataPoints[7]=134175562;
   dataPoints[10]=134236678;
   dataPoints[16]=134289971;
   dataPoints[24]=134278275;
   dataPoints[26]=134189625;
   dataPoints[27]=134268212;
*/
/*
// SF4
   dataPoints[6]=232418008; 
   dataPoints[7]=232377866;
   dataPoints[10]=232442646;
   dataPoints[16]=232495811;
   dataPoints[24]=232484451;
   dataPoints[26]=232395673;
   dataPoints[27]=232474164;
*/
/*
// SF5
   dataPoints[6]=330620488; 
   dataPoints[7]=330583818;
   dataPoints[10]=330648614;
   dataPoints[16]=330701667;
   dataPoints[24]=330690611;
   dataPoints[26]=330601721;
   dataPoints[27]=330680116;
*/
/*
// SF1
   dataPoints[6]=428826600; 
   dataPoints[7]=428789770;
   dataPoints[10]=428854598;
   dataPoints[16]=428907523;
   dataPoints[24]=428896771;
   //dataPoints[26]=;
   dataPoints[27]=428886068;
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
