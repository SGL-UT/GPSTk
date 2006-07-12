#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/DataAvailability/daa.cpp#1 $"

/** @file Performs a data availability analysis of the input data. In general,
    availability is determined by station and satellite position.
*/

//lgpl-license START
//lgpl-license END

#include "DataAvailabilityAnalyzer.hpp"

using namespace std;

int main(int argc, char *argv[])
{
   try
   {
      DataAvailabilityAnalyzer crap(argv[0]);

      if (!crap.initialize(argc, argv))
         exit(0);

      crap.run();
   }
   catch (gpstk::Exception &exc)
   { cout << exc << endl; }
   catch (std::exception &exc)
   { cout << "Caught std::exception " << exc.what() << endl; }
   catch (...)
   { cout << "Caught unknown exception" << endl; }
}
