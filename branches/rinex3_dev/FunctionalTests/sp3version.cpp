#pragma ident "$Id$"

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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

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
#include "CivilTime.hpp"
#include "SP3SatID.hpp"

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
      cout << "    --format <c>  Output version <c> (c, b, or default a)\n";
      cout << "                  NB. for a->c, generate random correlations\n";
      cout << "    --msg \"...\"   Add ... as a comment to the output header\n";
      cout << "    --verbose     Output to screen: dump headers, data, etc\n";
      return -1;
   }

   try
   {
      bool verbose=false;
      char version_out='a',version_in;
      int i,n=0;
      string filein,fileout("sp3.out");
      CommonTime currentTime=CommonTime::BEGINNING_OF_TIME;
      SP3Header header;
      SP3Data data;
      vector<string> comments;

      for(i=1; i<argc; i++) {
         if(argv[i][0] == '-') {
            string arg(argv[i]);
            if(arg == string("--format"))
               version_out = string(argv[++i])[0];
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

      if(verbose) cout << "Created stream" << endl;

      // read the header
      instrm >> header;
      if(verbose) {
         cout << "Input header: ";
         header.dump(cout);
         cout << endl;
      }
      version_in = header.versionChar();

      // add comments
      if(comments.size() > 0) {
         // try to keep existing comments
         for(i=0; i<4-comments.size(); i++)
            comments.push_back(header.comments[i]);
         header.comments.clear();
         for(i=0; i<comments.size(); i++) {
            header.comments.push_back(comments[i]);
         }
      }

      // prepare to write the header
      if(version_out == 'b')
         header.setVersion(SP3Header::SP3b);
      if(version_out == 'c') {
         header.setVersion(SP3Header::SP3c);
         header.system = SP3SatID();
         header.timeSystem = SP3Header::timeGPS;
         header.basePV = 1.25;     // make these up ... a real app would
         header.baseClk = 1.025;   // assign them based on what SP3Data will hold
      }
      if(verbose) {
         cout << "Output header (ver " << version_out << ") ";
         header.dump(cout);
         cout << endl;
      }

      // write the header
      outstrm << header;

      n = 0;     // count records
      while(instrm >> data) {
         // data has now been read in
         // ...handle the data
         if(verbose) {
            cout << "\nInput : ";
            data.dump(cout,(header.versionChar()=='c'));
         }

         // if correlationFlag has been set, there is new correlation data
         if(data.correlationFlag) {
            cout << "Input sdev";
            for(i=0; i<4; i++) cout << " " << data.sdev[i];
            cout << endl;
            cout << "Input correl";
            for(i=0; i<6; i++) cout << " " << data.correlation[i];
            cout << endl;
         }

         // output
         // make up some data...a real app would have this data
         if(version_in == 'a' && version_out == 'c') {
            // sigmas on the P|V rec
            for(i=0; i<4; i++) data.sig[i] = int(99*unitrand());
            // flags on the P line
            if(data.RecType == 'P') {
               data.clockEventFlag = (unitrand() > 0.5);
               data.clockPredFlag = (unitrand() > 0.5);
               data.orbitManeuverFlag = (unitrand() > 0.5);
               data.orbitPredFlag = (unitrand() > 0.5);
            }
            // write out the correlation records ... maybe
            if(unitrand() > 0.5) {
               // set the correlation flag for output
               data.correlationFlag = true;
               for(i=0; i<4; i++) data.sdev[i] = int(999*unitrand());
               for(i=0; i<6; i++) data.correlation[i] = int(99999*unitrand());
               cout << " (Make up sdev";
               for(i=0; i<4; i++) cout << " " << data.sdev[i];
               //cout << ")" << endl;
               cout << " and correl";
               for(i=0; i<6; i++) cout << " " << data.correlation[i];
               cout << ")" << endl;
            }
            else data.correlationFlag = false;
         }

         // write the data P|V record, and if correlationFlag, the EP|EV record
         if(verbose) {
            cout << "Output: ";
            data.dump(cout,(version_out=='c'));
         }
         outstrm << data;

         // count records
         n++;

         // prepare for the next read
         // must reset before input, since same data is for input and output
         data.correlationFlag = false;
      }

      instrm.close();
      outstrm.close();

      if(verbose) cout << "Read " << n << " records" << endl;
   }
   catch (Exception& e)
   {
      cout << e.what();
      return -1;
   }
   catch (std::exception& e)
   {
      cout << "Std exception: " << e.what();
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
