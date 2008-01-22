#pragma ident "$Id$"



/**
 * @file rinex_obs_test.cpp
 * tests gpslib::RinexObsData, gpslib::RinexObsStream, gpslib::RinexObsHeader.
 */

#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"

using namespace std;

/// Returns 0 on success.
main(int argc, char *argv[])
{
   if (argc<2)
   {
      cout << "Gimme a rinex obs to chew on!  Exiting." << endl;
      exit(-1);
   }

   try
   {
      cout << "Reading " << argv[1] << "." << endl;

      gpstk::RinexObsStream roffs(argv[1]);
      roffs.exceptions(ios::failbit);
      gpstk::RinexObsHeader roh;
      gpstk::RinexObsData roe;
      
      roffs >> roh;
      roh.dump(cout);

      while (roffs >> roe)
      {}

      cout << "Read " << roffs.recordNumber << " epochs.  Done."  << endl;
      exit(0);
   }
   catch(gpstk::FFStreamError& e)
   {
      cout << e;
      exit(1);
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
