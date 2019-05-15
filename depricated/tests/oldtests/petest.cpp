//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <map>

#include "DayTime.hpp"
#include "SP3Stream.hpp"
#include "SP3Data.hpp"
#include "SP3Header.hpp"
#include "SP3EphemerisStore.hpp"
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
      cout << "Usage: petest <SP3-format files ...>\n";
      return -1;
   }

   try
   {

      bool firstEpochFound=true;
      DayTime firstTime;
      DayTime lastTime;
      SatID firstSat;
         
      int i,ip,it,nf=0,np=0,nt=0;
      SP3EphemerisStore EphList;
      for(i=1; i<argc; i++) {
         SP3Header header;
         SP3Data data;
         
         // you can't open, close, and reopen a file w/o abort on second open...
         SP3Stream pefile;
         pefile.exceptions(ifstream::failbit);
         cout << "Reading SP3 file " << argv[i] << "." << endl;
         pefile.open(argv[i],ios::in);

         pefile >> header;
         data.version = header.version;
         
         //cout << "Dump header:\n";
         //header.dump(cout);
         //cout << endl;

         ip = it = 0;
         DayTime t(DayTime::BEGINNING_OF_TIME);

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
      DayTime tt = firstTime + (lastTime-firstTime)/2.;
      SatID tsat = firstSat;
      Xvt PVT;
      for(i=0; i<300; i++) {
         tt += 30.0;
         PVT = EphList.getXvt(tsat,tt);

         if (true) 
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
