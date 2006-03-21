#pragma ident "$Id: //depot/sgl/gpstk/dev/tests/rinex_met_test.cpp#1 $"

#include "RinexMetBase.hpp"
#include "RinexMetData.hpp"
#include "RinexMetHeader.hpp"
#include "RinexMetStream.hpp"

/**
 * @file rinex_met_test.cpp
 * Tests gpslib::RinexMetStream, gpslib::RinexMetData, gpslib::RinexMetHeader.
 */

using namespace std;

/// returns 0 if successful.
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

      gpstk::RinexMetStream roffs(argv[1]);

         // reading the header data isn't necessary
//      gpstk::RinexMetHeader roh;
      gpstk::RinexMetData roe;

      roffs.exceptions(fstream::failbit);

//      roffs >> roh;
//      roh.dump(cout);

      while (roffs >> roe)
      {}

      cout << "done" << endl;
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
