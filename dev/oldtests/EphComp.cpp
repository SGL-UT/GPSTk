#pragma ident "$Id$"
// EphComp.cpp compare BC and SP3 ephemerides
//
#include <iostream>
#include <iomanip>
#include <string>
#include <map>

#include "DayTime.hpp"

#include "RinexEphemerisStore.hpp"
#include "SP3EphemerisStore.hpp"

#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"

/**
 * @file Ephcomp.cpp
 * 
 */
using namespace std;

int main(int argc, char *argv[])
{
   try
   {
      gpstk::CommandOptionNoArg
         helpOption('h', "help", "Print help usage");
      gpstk::CommandOptionWithNumberArg
         prnOption('s', "prn", "Which SV to compare");
      gpstk::CommandOptionWithAnyArg
         sp3Files('p', "precise", "SP3 file",true),
         bceFiles('b', "broadcast", "RINEX nav file",true);

      string appDesc("Computes diferences between broadcast and precise ephemerides.");
      gpstk::CommandOptionParser cop(appDesc);
      cop.parseOptions(argc, argv);

      if (helpOption.getCount() || cop.hasErrors())
      {
         if (cop.hasErrors())
            cop.dumpErrors(cout);
         cop.displayUsage(cout);
         exit(0);
      }

      // load all the SP3 ephemerides files
      gpstk::SP3EphemerisStore SP3EphList;
      SP3EphList.loadFiles(sp3Files.getValue());

      // load the BC ephemerides
      gpstk::RinexEphemerisStore BCEphList;
      BCEphList.loadFiles(bceFiles.getValue());
         
      int prn=13;
      if (prnOption.getCount())
         prn = gpstk::StringUtils::asInt((prnOption.getValue())[0]);

      // choose a time range within the data....
      gpstk::DayTime te(BCEphList.getFinalTime());
      gpstk::DayTime t(BCEphList.getInitialTime());
      //BCEphList.SearchNear();
      gpstk::Xvt SP3PVT,BCPVT;
      while (t < te)
      {
         t += 15.0;
         try
         {
            // get the SP3 & BCE PVT
            SP3PVT = SP3EphList.getPrnXvt(prn,t);
            BCPVT = BCEphList.getPrnXvt(prn,t);
            cout << fixed << t
                 << " " << setw(2) << prn
                 << setprecision(6)
                 << " " << setw(13) << BCPVT.x[0]-SP3PVT.x[0]
                 << " " << setw(13) << BCPVT.x[1]-SP3PVT.x[1]
                 << " " << setw(13) << BCPVT.x[2]-SP3PVT.x[2]
                 << scientific
                 << " " << setw(13) << BCPVT.dtime-SP3PVT.dtime
                 << fixed
                 << " " << setw(13) << BCPVT.v[0]-SP3PVT.v[0]
                 << " " << setw(13) << BCPVT.v[1]-SP3PVT.v[1]
                 << " " << setw(13) << BCPVT.v[2]-SP3PVT.v[2]
                 << scientific
                 << " " << setw(13) << BCPVT.ddtime-SP3PVT.ddtime
                 << endl;
         }
         catch (gpstk::EphemerisStore::NoEphemerisFound& e)
         {
            cerr << t << " " << e << endl;
            continue;
         }

      }
   }
   catch (gpstk::Exception& e)
   {
      cout << e;
      exit(-1);
   }
   catch (...)
   {
      cout << "Caught an unknown exception" << endl;
      exit(-1);
   }

   return 0;
}
