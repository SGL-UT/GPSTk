#include <iostream>

#include "ObsArray.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{
   try {

      ObsArray oa;

      cout << oa.add(RinexObsHeader::C1) << endl;
      cout << oa.add(RinexObsHeader::P2) << endl;
      cout << oa.add("C1-P2") << endl;
      cout << "There are " << oa.getIndexCount() << " obs indices." << endl;

      oa.load("netrs027.06o.30s","netrs027.06n");

      cout << "Some C1-P2 values: " << endl;
      for (int i=0; i<0; i++)
         cout << oa(i,2) << " ";
      cout << endl;
      
      

      exit(0);
   }
   catch(Exception& ex)
   {
      cerr << "Exception caught" << endl;
      cerr << ex << endl;
   }
}
