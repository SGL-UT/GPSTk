#include "ScreenControl.hpp"

using namespace std;
using namespace gpstk;

namespace gpstk 
{
   
   void clearScreen(void)
   {
      cout << "[H[J";
   }

   void printTitle(void)
   {
      cout << "GPSTk Real-Time Data Collection for the Ashtech Z-XII ver. 1.0" << endl;
   }
   
} // namespace gpstk
