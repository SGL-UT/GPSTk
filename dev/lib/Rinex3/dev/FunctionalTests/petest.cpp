#pragma ident "$Id$"

#include <iostream>
#include <iomanip>
#include <string>
#include <map>

#include "CommonTime.hpp"
#include "SP3Stream.hpp"
#include "SP3Data.hpp"
#include "SP3Header.hpp"
#include "SP3EphemerisStore.hpp"
#include "SatID.hpp"

/**
 * @file petest.cpp
 */

using namespace std;
using namespace gpstk;

int main(int argc, char *argv[])
{
   if(argc<2) {
      cout << "Usage: petest <SP3-format files ...>\n";
      return -1;
   }

   try
   {
      bool firstEpochFound=true;
      CommonTime firstTime;
      CommonTime lastTime;
      SatID firstSat;

      int i,ip,it,nf=0,np=0,nt=0;
      SP3EphemerisStore EphList;

      // don't reject anything....
      EphList.rejectBadPositions(false);
      EphList.rejectBadClocks(false);

      // loop over file names on the command line
      for(i=1; i<argc; i++) {
         SP3Header header;
         SP3Data data;
         
         // you can't open, close, and reopen a file w/o abort on second open...
         SP3Stream pefile;
         // SP3Stream does this ... pefile.exceptions(ifstream::failbit);
         cout << "Reading SP3 file " << argv[i] << "." << endl;
         pefile.open(argv[i],ios::in);

         pefile >> header;
         
         header.dump(cout);

         ip = it = 0;
         CommonTime ttag(CommonTime::BEGINNING_OF_TIME);

         while(pefile >> data) {
            data.dump(cout, header.version==SP3Header::SP3c);

            if(firstEpochFound && data.RecType == '*') {  
               firstTime = data.time;
               lastTime = firstTime;
            }
            else if(firstEpochFound) {
               firstSat = data.sat;
               firstEpochFound=false;
            }

            if(data.time > lastTime) lastTime = data.time;
            
            if(data.time > ttag) {
               ttag = data.time;
               it++; nt++;
            }
            ip++; np++;
         }
         cout << "\nDone with file " << argv[i] << ": read "
              << ip << " P/V records and " << it << " epochs." << endl;
         pefile.close();
         nf++;

         // add to store
         cout << "\nNow load the file using SP3Ephemeris::loadFile()" << endl;
         EphList.loadFile(string(argv[i]));
      }
      
      cout << "\nDone with " << nf << " files: read "
           << np << " P/V records and " << nt << " epochs." << endl;

      cout << "Interpolation order is " << EphList.getInterpolationOrder() << endl;
      cout << "Set order to 17" << endl;
      EphList.setInterpolationOrder(17);
      EphList.dump(cout, 2);

/*
      unsigned long ref;
      // choose a time tag within the data....
      CommonTime tt = firstTime + 3600.; // + (lastTime-firstTime)/2.;
      SatID tsat = firstSat;
      Xvt PVT;
      for(i=0; i<300; i++) {
         tt += 30.0;
         PVT = EphList.getXvt(tsat,tt);

         if(false) 
            cout << "LI " << tt << " P " << fixed
                 << setw(16) << setprecision(6) << PVT.x[0] << " "
                 << setw(16) << setprecision(6) << PVT.x[1] << " "
                 << setw(16) << setprecision(6) << PVT.x[2] << " "
                 << setw(10) << setprecision(6) << PVT.dtime
                 << " V "
                 << setw(12) << setprecision(6) << PVT.v[0] << " "
                 << setw(12) << setprecision(6) << PVT.v[1] << " "
                 << setw(12) << setprecision(6) << PVT.v[2] << " "
                 << setw(12) << setprecision(6) << PVT.ddtime
                 << endl;
      }
*/
   }
   catch(Exception& e) {
      cout << e.what();
      return -1;
   }
   catch(...) {
      cout << "Caught an unknown exception" << endl;
      return -1;
   }

   return 0;
}
