#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/checktools/rnwcheck.cpp#1 $"

#include "CheckFrame.hpp"

#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{
   try
   {
      CheckFrame<RinexNavStream, RinexNavData> cf(argv[0],
                                                  std::string("Rinex Nav"));
      
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
