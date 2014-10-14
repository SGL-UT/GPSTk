//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file sp3version.cpp
 * Read an SP3 format file (any version) and write the data out to another,
 * specifying the SP3 version. This code is intended for testing and as a pattern
 * for SP3 I/O and analysis programs; currently in converting SP3a to SP3c it
 * inserts random data.
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "SP3Stream.hpp"
#include "SP3Header.hpp"
#include "SP3Data.hpp"
#include "CommonTime.hpp"
#include "SatID.hpp"

using namespace std;
using namespace gpstk;

double unitrand(void) { return double(rand())/RAND_MAX; }

int main(int argc, char *argv[])
{
   if(argc < 2)
   {
      cout << "Usage: sp3version <SP3 file> [options]\n";
      cout << " Read an SP3 file (either a or c format) and write it to another\n";
      cout << "    file, also in either a or c format.\n";
      cout << " Options:\n";
      cout << "    --in <file>   Read the input file <file> ()\n";
      cout << "    --out <file>  Name the output file <file> (sp3.out)\n";
      cout << "    --outputC     Output version c (otherwise a)\n";
      cout << "                  NB. for a->c, generate random correlations\n";
      cout << "    --msg \"...\"   Add ... as a comment to the output header\n";
      cout << "    --verbose     Output to screen: dump headers, data, etc\n";
      return -1;
   }

   try
   {
      bool verbose=false;
      SP3Header::Version version_in, version_out;
      int i,n;
      string filein,fileout("sp3.out");
      CommonTime currentTime=CommonTime::BEGINNING_OF_TIME;
      SP3Header sp3header;
      SP3Data sp3data;
      vector<string> comments;

      for(i=1; i<argc; i++) {
         if(argv[i][0] == '-') {
            string arg(argv[i]);
            if(arg == string("--outputC"))
               version_out = SP3Header::SP3c;   //'c';
            else if(arg == string("--in"))
               filein = string(argv[++i]);
            else if(arg == string("--out"))
               fileout = string(argv[++i]);
            else if(arg == string("--msg"))
               comments.push_back(string(argv[++i]));
            else if(arg == string("--verbose"))
               verbose = true;
            else
               cout << "Ignore unknown option: " << arg << endl;
         }
         else {
            filein = string(argv[i]);
         }
      }

      if(filein.empty()) {
         cout << "Error - no filename specified. Abort.\n";
         return -1;
      }
      if(verbose) cout << "Reading file " << filein << endl;

      SP3Stream instrm(filein.c_str());
      instrm.exceptions(ifstream::failbit);

      SP3Stream outstrm(fileout.c_str(),ios::out);
      outstrm.exceptions(ifstream::failbit);

      // read the header
      instrm >> sp3header;
      if(verbose) {
         cout << "Input ";
         sp3header.dump(cout);
         cout << endl;
      }
      version_in = sp3header.version;

      // add comments
      if(comments.size() > 0) {
         // try to keep existing comments
         for(i=0; i<4-int(comments.size()); i++)
            comments.push_back(sp3header.comments[i]);
         sp3header.comments.clear();
         for(i=0; i<int(comments.size()); i++) {
            sp3header.comments.push_back(comments[i]);
         }
      }

      // prepare to write the header
      if(version_out == SP3Header::SP3c) {
         sp3header.version = SP3Header::SP3c; //'c';
         sp3header.system = SP3SatID();
         sp3header.timeSystem = TimeSystem::GPS;
         sp3header.basePV = 1.25;     // make these up ... a real app would
         sp3header.baseClk = 1.025;   // assign them based on what SP3Data will hold
      }
      if(verbose) {
         cout << "Output ";
         sp3header.dump(cout);
         cout << endl;
      }

      // write the header
      outstrm << sp3header;

      // for reading and writing, sp3data MUST have the version of the header;
      // this is crucial for version 'c'
      //sp3data.version = version_in;          // for input

      n = 0;     // count records
      while(instrm >> sp3data) {
         // data has now been read in
         // ...handle the data
         if(verbose) { cout << "Input:\n"; sp3data.dump(cout); }
         // if correlationFlag has been set, there is new correlation data
         if(sp3data.correlationFlag) {
            cout << "Input sdev";
            for(i=0; i<4; i++) cout << " " << sp3data.sdev[i];
            cout << endl;
            cout << "Input correl";
            for(i=0; i<6; i++) cout << " " << sp3data.correlation[i];
            cout << endl;
         }

         // output
         // write the epoch record
         if(sp3data.time > currentTime) {
            char saveRecType = sp3data.RecType;
            sp3data.RecType = '*';
            //outstrm << sp3data;
            sp3data.RecType = saveRecType;
            currentTime = sp3data.time;
         }

            // make up some data...a real app would have this data
         if(version_in == SP3Header::SP3a && version_out == SP3Header::SP3c) {
            // sigmas on the P|V rec
            for(i=0; i<4; i++) sp3data.sig[i] = int(99*unitrand());
            // RecType on the P line
            if(sp3data.RecType == 'P') {
               sp3data.clockEventFlag = (unitrand() > 0.5);
               sp3data.clockPredFlag = (unitrand() > 0.5);
               sp3data.orbitManeuverFlag = (unitrand() > 0.5);
               sp3data.orbitPredFlag = (unitrand() > 0.5);
            }
            // write out the correlation records ... maybe
            if(unitrand() > 0.5) {
               // set the RecType for output
               sp3data.correlationFlag = true;
               for(i=0; i<4; i++) sp3data.sdev[i] = int(9999*unitrand());
               for(i=0; i<6; i++) sp3data.correlation[i] = int(99999999*unitrand());
               cout << "Output sdev";
               for(i=0; i<4; i++) cout << " " << sp3data.sdev[i];
               cout << endl;
               cout << "Output correl";
               for(i=0; i<6; i++) cout << " " << sp3data.correlation[i];
               cout << endl;
            }
            else sp3data.correlationFlag = false;
         }

         // write the data P|V record, and if correlationFlag, the EP|EV record
         if(verbose) { cout << "Output:\n"; sp3data.dump(cout); }
         outstrm << sp3data;

         // count records
         n++;

         // prepare for the next read
         //sp3data.version = version_in;
         // must reset before input, since same sp3data is for input and output
         sp3data.correlationFlag = false;
      }

      // don't forget this
      //outstrm << "EOF" << endl;

      instrm.close();
      outstrm.close();

      if(verbose) cout << "Read " << n << " records" << endl;
   }
   catch (Exception& e)
   {
      cout << e;
      return -1;
   }
   catch (...)
   {
      cout << "Caught an unknown exception" << endl;
      return -1;
   }

   cout << "Done."  << endl;
   return 0;
} 
