#include <iostream>
#include <iomanip>
#include <string>
#include <map>

#include "CommonTime.hpp"
#include "Rinex3NavStream.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3EphemerisStore.hpp"
#include "SatID.hpp"

/**
 * @file petest.cpp
 *
 */

using namespace std;
using namespace gpstk;

int main(int argc, char *argv[])
{
   if (argc<2) {
      cout << "Usage: Rinex3NavStoreTest <Rinex3-format files ...>\n";
      return -1;
   }

   try
   {

      bool firstEpochFound=true;
      CommonTime firstTime;
      CommonTime lastTime;
      SatID firstSat;
         
      int i,ip,it,nf=0,np=0,nt=0;
      Rinex3EphemerisStore EphList;
      for(i=1; i<argc; i++) {
         Rinex3NavHeader header;
         Rinex3NavData data;
         
         // you can't open, close, and reopen a file w/o abort on second open...
         Rinex3NavStream pefile;
         pefile.exceptions(ifstream::failbit);
         cout << "Reading Rinex3Nav file " << argv[i] << "." << endl;
         pefile.open(argv[i],ios::in);

         pefile >> header;
         
         cout << "Dump header:\n";
         header.dump(cout);
         cout << endl;

         ip = it = 0;
         CommonTime t(CommonTime::BEGINNING_OF_TIME);

         while(pefile >> data) {
            if (firstEpochFound)
            {  
               firstSat = data.sat;
               firstTime = data.time;
               lastTime = firstTime;
               
               firstEpochFound=false;
            }

            if (data.time > lastTime) lastTime = data.time;
            
            if(data.time > t) {
               cout << "Epoch " << data.time << endl;
               t = data.time;
               it++; nt++;
            }
            data.dump(cout);
            ip++; np++;
         }
         cout << "\nDone with file " << argv[i] << ": read "
              << ip << " P/V records and " << it << " epochs." << endl;
         pefile.close();
         nf++;

         // add to store
         EphList.loadFile(string(argv[i]));
      }
      
      cout << "\nDone with " << nf << " files: read "
           << np << " P/V records and " << nt << " epochs." << endl;

      EphList.dump();
   }
   catch (Exception& e)
   {
      cout << e;
      exit(-1);
   }
   catch (...)
   {
      cout << "Caught an unknown exception" << endl;
      exit(-1);
   }

   return 0;
}
