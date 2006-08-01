#pragma ident "$Id$"



/**
 * @file rinex_nav_test.cpp
 * tests gpslib::RinexNavStream, gpslib::RinexNavData, gpslib::RinexNavHeader.
 */

//#include "RinexNavBase.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"
#include "RinexMetStream.hpp"

using namespace std;

/// Returns 0 when successful.
main(int argc, char *argv[])
{
   if (argc<2)
   {
      cout << "Gimme a rinex nav to chew on!  Exiting." << endl;
      exit(-1);
   }

   try
   {
      cout << "Reading " << argv[1] << "." << endl;

      gpstk::RinexNavStream rnffs(argv[1]);
      gpstk::RinexNavHeader rnh;
      gpstk::RinexNavData rne;

      rnffs.exceptions(fstream::failbit);
      
      rnffs >> rnh;
      rnh.dump(cout);

      int i = 0;
      while (rnffs >> rne)
      {
         i++;
      }

      cout << "Read " << i << " records.  Done."  << endl;
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
