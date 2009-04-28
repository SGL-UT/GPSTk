#pragma ident "$Id: RinexMetTest.cpp 1815 2009-03-20 21:12:09Z raindave $"



/**
 * @file rinex_met_read_write.cpp
 * tests gpslib::RinexMetStream, gpslib::RinexMetHeader.
 */

#include "RinexMetBase.hpp"
#include "RinexMetStream.hpp"
#include "RinexMetHeader.hpp"
#include "RinexMetData.hpp"

using namespace std;

/// Returns 0 on success, and input and output files should diff without error.
main(int argc, char *argv[])
{
   if (argc<2)
   {
      cout << "rinex_met_read_write inputfile outputfile" << endl;
      exit(-1);
   }

   try
   {
      cout << "Reading " << argv[1] << endl;
      gpstk::RinexMetStream roffs(argv[1]);
      cout << "Writing " << argv[2] << endl;
      gpstk::RinexMetStream out(argv[2], ios::out);
      gpstk::RinexMetHeader rmh;
      gpstk::RinexMetData rmd;

      cout << "Stream, Header, Data declarations made." << endl;

      roffs >> rmh;

      cout << "Header read in." << endl;

      rmh.dump(cout);

      cout << "Header dumped." << endl;

      out << rmh;

      cout << "Header written out." << endl;

      int i = 0;
      while (roffs >> rmd)
      {
         out << rmd;
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
