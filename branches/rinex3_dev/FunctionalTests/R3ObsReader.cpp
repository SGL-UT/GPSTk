#pragma ident "$Id: R3ObsReader.cpp 1815 2009-03-20 21:12:09Z raindave $"

/**
 * @file R3ObsReader.cpp
 */

#include <map>

#include "icd_gps_constants.hpp"
#include "CommonTime.hpp"
#include "CivilTime.hpp"
#include "Rinex3ObsBase.hpp"
#include "Rinex3ObsStream.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"
#include "SatID.hpp"
#include "ObsID.hpp"

using namespace std;
using namespace gpstk;

static const double lam1 = gpstk::C_GPS_M / 1575420000.0;
static const double lam2 = gpstk::C_GPS_M / 1227600000.0;
static const double lam3 = gpstk::C_GPS_M / 1176450000.0;

double lam1sq, lam2sq, lam3sq;

std::map<CommonTime, Rinex3ObsData::DataMap> obsMap;

/// Returns 0 on success, and input and output files should diff without error.
main(int argc, char *argv[])
{
   if (argc<1)
   {
      cout << "R3ObsReader inputfile" << endl;
      exit(-1);
   }

   try
   {
      lam1sq = lam1*lam1;
      lam2sq = lam2*lam2;
      lam3sq = lam3*lam3;

      cout << "Reading from " << argv[1] << endl;
      gpstk::Rinex3ObsStream r3os(argv[1],ios::in);
      gpstk::Rinex3ObsHeader r3oh;
      gpstk::Rinex3ObsData   r3od;

      r3os >> r3oh;
      r3oh.dump(cout);
      int iL1 = -1, iL2 = -1, iL5 = -1;
      vector<ObsID> obsList = r3oh.mapObsTypes["G"];
      cout << "Obs list is size " << obsList.size() << endl;
      for (int i = 0; i < obsList.size(); i++)
      {
         if (obsList[i].asRinex3ID() == "L1C") iL1 = i;
         if (obsList[i].asRinex3ID() == "L2C") iL2 = i;
         if (obsList[i].asRinex3ID() == "L5Q") iL5 = i;
      }
      cout << "indices = " << iL1+1 << "," << iL2+1 << "," << iL5+1 << endl;

      int i = 0;
      while (r3os >> r3od)
      {
         CommonTime ct = r3od.time;
         ct.setTimeSystem(TimeSystem::GPS);
         obsMap[ct] = r3od.obs;
         i++;
      }
      cout << "Read " << i << " records.  Done."  << endl;

      std::map<CommonTime, Rinex3ObsData::DataMap>::const_iterator it;
      for (it = obsMap.begin(); it != obsMap.end(); it++)
      {
         const Rinex3ObsData::DataMap& dataMap = it->second;
//         cout << " Time " << CivilTime(it->first)
//              << " has " << dataMap.size() << " entries." << std::endl;

         SatID sid(1,SatID::systemGPS);

         Rinex3ObsData::DataMap::const_iterator datait;
         for (datait=dataMap.begin(); datait != dataMap.end(); datait++)
         {
            if (datait->first == sid)
            {
//               cout << "Found PRN 1 at time " << CivilTime(it->first) << endl;
               double obsL1C, obsL2C, obsL5Q, ML123;
               obsL1C = (datait->second)[iL1].data;
               obsL2C = (datait->second)[iL2].data;
               obsL5Q = (datait->second)[iL5].data;
               ML123 = lam3sq*(obsL1C-obsL2C) + lam2sq*(obsL5Q-obsL1C) + lam1sq*(obsL2C-obsL5Q);
               cout << "M_L123 = " << ML123 << endl;
            }
         }
      }

      exit(0);
   }
   catch(gpstk::Exception& e)
   {
      cout << e;
      exit(1);
   }
   catch (...)
   {
      cout << "unknown error.  Done." << endl;
      exit(1);
   }

   exit(0);
} // main()
