/*********************************************************************
*  $Id$
*
*  Test program from November 2006.  Written to test the SEMAlmRecord.cpp
*  module..
*
// *********************************************************************/

#include "SEMData.hpp"
#include "SEMStream.hpp"
#include "SEMHeader.hpp"
#include "SEMBase.hpp"

using namespace std;
using namespace gpstk;

int main( int argc, char * argv[] )
{
      // Read an existing SEM almanac file and write it back out.
   try
   {
   SEMStream In("sem387.txt");
   SEMStream Out("sem.dbg", ios::out);
   SEMHeader Header;
   SEMData Data;
   
   In >> Header;
   Out << Header;
   while (In >> Data)
   {
      Out << Data;
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
