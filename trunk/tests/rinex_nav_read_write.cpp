#pragma ident "$Id: //depot/sgl/gpstk/dev/tests/rinex_nav_read_write.cpp#1 $"

/**
 * @file rinex_nav_read_write.cpp
 * tests gpslib::RinexNavStream, gpslib::RinexNavData, gpslib::RinexNavHeader.
 */

#include "RinexNavBase.hpp"
#include "RinexNavData.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavStream.hpp"

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
      gpstk::RinexNavStream rnffs(argv[1]);
      cout << "Writing " << argv[2] << endl;
      gpstk::RinexNavStream out(argv[2], ios::out);
      gpstk::RinexNavHeader rnh;
      gpstk::RinexNavData rne;
      
      rnffs >> rnh;
      out << rnh;
      rnh.dump(cout);

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
