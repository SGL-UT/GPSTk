#pragma ident "$Id: RinexObsTest.cpp 1815 2009-03-20 21:12:09Z raindave $"



/**
 * @file rinex_obs_read_write.cpp
 * tests gpslib::RinexObsStream, gpslib::RinexNavHeader.
 */

#include "RinexObsBase.hpp"
#include "RinexObsStream.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"

using namespace std;

/// Returns 0 on success, and input and output files should diff without error.
main(int argc, char *argv[])
{
   if (argc<2)
   {
      cout << "rinex_obs_read_write inputfile outputfile" << endl;
      exit(-1);
   }

   try
   {
      cout << "Reading " << argv[1] << endl;
      gpstk::RinexObsStream roffs(argv[1]);
      cout << "Writing " << argv[2] << endl;
      gpstk::RinexObsStream out(argv[2], ios::out);
      gpstk::RinexObsHeader roh;
      gpstk::RinexObsData roe;

      cout << "Stream, Header, Data declarations made." << endl;

      roffs >> roh;

      cout << "Header read in." << endl;

      roh.dump(cout);

      cout << "Header dumped." << endl;

      out << roh;

      cout << "Header written out." << endl;

      int i = 0;
      while (roffs >> roe)
      {
         out << roe;
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
