#include <iostream>
#include <iomanip>
#include <string>
#include <map>

#include "DayTime.hpp"
#include "SP3Stream.hpp"
#include "SP3Data.hpp"
#include "SP3Header.hpp"
#include "SP3EphemerisStore.hpp"

/**
 * @file petest.cpp
 *
 */

using namespace std;

int main(int argc, char *argv[])
{
   if (argc<2) {
      cout << "Usage: petest <SP3-format files ...>\n";
      return -1;
   }

   try
   {
      int i,ip,it,nf=0,np=0,nt=0;
      gpstk::SP3EphemerisStore EphList;
      for(i=1; i<argc; i++) {
         gpstk::SP3Header header;
         gpstk::SP3Data data;
         // you can't open, close, and reopen a file w/o abort on second open...
         gpstk::SP3Stream pefile;
         pefile.exceptions(ifstream::failbit);
         cout << "Reading SP3 file " << argv[i] << "." << endl;
         pefile.open(argv[i],ios::in);

         pefile >> header;
         //cout << "Dump header:\n";
         //header.dump(cout);
         //cout << endl;

         ip = it = 0;
         gpstk::DayTime t(gpstk::DayTime::BEGINNING_OF_TIME);
         while(pefile >> data) {
            if(data.time > t) {
               //cout << "Epoch " << data.time << endl;
               t = data.time;
               it++; nt++;
            }
            //data.dump(cout);
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

      //EphList.dump(2);

      unsigned long ref;
      // choose a time tag within the data....
      gpstk::DayTime tt(2004,2,29,3,0,0.0);
      gpstk::Xvt PVT;
      for(i=0; i<300; i++) {
         tt += 30.0;
         PVT = EphList.getPrnXvt(31,tt);

         if (false) 
            cout << "LI " << tt << " P " << fixed
                 << setw(13) << setprecision(6) << PVT.x[0] << " "
                 << setw(13) << setprecision(6) << PVT.x[1] << " "
                 << setw(13) << setprecision(6) << PVT.x[2] << " "
                 << setw(13) << setprecision(6) << PVT.dtime
                 << " V "
                 << setw(13) << setprecision(6) << PVT.v[0] << " "
                 << setw(13) << setprecision(6) << PVT.v[1] << " "
                 << setw(13) << setprecision(6) << PVT.v[2] << " "
                 << setw(13) << setprecision(6) << PVT.ddtime
                 << endl;
      }
   }
   catch (gpstk::Exception& e)
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
