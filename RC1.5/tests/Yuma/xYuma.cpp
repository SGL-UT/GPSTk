/*********************************************************************
*  $Id$
*
*  Test program from November 2006.  Written to test the YumaAlmRecord.cpp
*  module..
*
// *********************************************************************/

#include "YumaData.hpp"
#include "YumaStream.hpp"
#include "YumaBase.hpp"
#include "SMODFData.hpp"

using namespace std;
using namespace gpstk;

int main( int argc, char * argv[] )
{
      // Read an existing Yuma almanac file and write it back out.
   try
   {
   YumaStream In("yuma377.txt");
   YumaStream Out("yuma377.dbg", ios::out);
   YumaData Data;
   
   while (In >> Data)
   {
      Out << Data;
   }
   
   YumaStream In2("yuma377.dbg");
   YumaStream Out2("yuma377_2.dbg", ios::out);
   
   while (In2 >> Data)
   {
      Out2 << Data;
   }
   
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

   
   return(0);
}
