#pragma ident "$Id$"



/**
 * @file rinex_nav_read_write.cpp
 * tests gpslib::Rinex3NavStream, gpslib::Rinex3NavData, gpslib::Rinex3NavHeader.
 */

#include "Rinex3NavBase.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavStream.hpp"

using namespace std;

/// Returns 0 on success, and input and output files should diff without error.
main(int argc, char *argv[])
{
   if (argc<2)
   {
      cout << "rinex_nav_read_write inputfile outputfile" << endl;
      exit(-1);
   }

   try
   {
      cout << "Reading " << argv[1] << endl;
      gpstk::Rinex3NavStream rnffs(argv[1]);
      cout << "Writing " << argv[2] << endl;
      gpstk::Rinex3NavStream out(argv[2], ios::out);
      gpstk::Rinex3NavHeader rnh;
      gpstk::Rinex3NavData rne;

      cout << "Stream, Header, Data declarations made." << endl;

      rnffs >> rnh;

      cout << "Header read in." << endl;

      rnh.dump(cout);

      cout << "Header dumped." << endl;

      out << rnh;

      cout << "Header written out." << endl;

      int i = 0;
      while (rnffs >> rne)
      {
         out << rne;
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
