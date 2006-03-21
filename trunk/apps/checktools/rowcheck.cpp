#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/checktools/rowcheck.cpp#1 $"

#include "CheckFrame.hpp"

#include "RinexObsStream.hpp"
#include "RinexObsData.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{
   try
   {
      CheckFrame<RinexObsStream, RinexObsData> cf(argv[0],
                                                  std::string("Rinex Obs"));
      
      if (!cf.initialize(argc, argv))
         return 0;
      if (!cf.run())
         return 1;
      
      return 0;   
   }
   catch(gpstk::Exception& e)
   {
      cout << e << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl;
   }
   catch(...)
   {
      cout << "unknown error" << endl;
   }
   return 1;
}
