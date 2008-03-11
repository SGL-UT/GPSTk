#pragma ident "$Id$"


#include "CheckFrame.hpp"

#include "RinexMetStream.hpp"
#include "RinexMetData.hpp"
#include "RinexMetFilterOperators.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{
   try
   {
      CheckFrame<RinexMetStream, RinexMetData, RinexMetDataFilterTime>
         cf(argv[0], "Rinex Met");
      
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
