#pragma ident "$Id$"



/**
 * @file rinex_nav_test.cpp
 * tests gpslib::RinexNavStream, gpslib::RinexNavData, gpslib::RinexNavHeader.
 */

//#include "RinexNavBase.hpp"
#include "Rinex3NavStream.hpp"
#include "Rinex3NavHeader.hpp"

using namespace std;
using namespace gpstk;

/// Returns 0 when successful.
main(int argc, char *argv[])
{
   try
   {
      gpstk::Rinex3NavStream rnffs("Rinex3-example.n");
      gpstk::Rinex3NavHeader rnh;

      rnffs.exceptions(fstream::failbit);
      
      rnffs >> rnh;
      rnh.dump(cout);

      gpstk::Rinex3NavStream rnout("Rinex3NavOutput.txt",ios::out);
      
      rnout << rnh;

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
