#pragma ident "$Id: //depot/sgl/gpstk/dev/tests/rinex_obs_read_write.cpp#1 $"

/**
 * @file rinex_obs_read_write.cpp
 * tests gpslib::RinexObsData, gpslib::RinexObsStream, gpslib::RinexObsHeader.
 */
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"

using namespace std;

/// Returns 0 on success.  Input and output files should diff wihtout error.
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
      roffs.exceptions(ios::failbit);
      gpstk::RinexObsHeader roh;
      gpstk::RinexObsData roe;
      
      roffs >> roh;
      out << roh;
      roh.dump(cout);

      while (roffs >> roe)
      {
         out << roe;
      }

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
