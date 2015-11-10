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
 * @file testSSEph.cpp
 * Read the binary solar system ephemeris file (created by convertSSEph) and the
 * JPL test file, compute the coordinates listed in test file and compare the results
 * with the values given in the test file. Based on testeph.f on the JPL ftp site.
 * Ascii files can be downloaded from JPL website.
 */

//------------------------------------------------------------------------------------
// system includes
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <time.h>

// GPSTk
#include "TimeString.hpp"
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "CommonTime.hpp"
#include "SystemTime.hpp"
#include "SolarSystem.hpp"
#include "logstream.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

//------------------------------------------------------------------------------------
string Version("1.0 9/10/07");
// 1.0

//------------------------------------------------------------------------------------
// TD 

//------------------------------------------------------------------------------------
// global data
string PrgmName;              // name of this program - must match Jamfile
string Title;                 // name, version and runtime
ofstream logstrm;

//------------------------------------------------------------------------------------
// prototypes -- this module only

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   clock_t totaltime;            // for timing
   totaltime = clock();

   // locals
   int i,iret=0;
   CommonTime CurrEpoch = SystemTime();
   SolarSystem SSEphemeris;

   // program name, title and version
   PrgmName = string("testSSEph");
   Title = PrgmName + ", test program for JPL ephemeris, version "
      + Version + ", " + printTime(CurrEpoch,"Run %04Y/%02m/%02d at %02H:%02M:%02S");

   // default command line input
   bool verbose=false,debug=false;
   string inputFilename,testFilename,logFilename;

   // parse the command line input
   for(i=0; i<argc; i++) {
      string word = string(argv[i]);
      if(argc == 1 || word == "-h" || word == "--help") {
         cout << "Program " << PrgmName
            << " reads a binary JPL planetary ephemeris file, created by "
            << "convertSSEph,\n  and a test file, downloaded from the JPL ftp site, "
            << "containing times and planets\n  with JPL-generated ephemeris "
            << "coordinate values. The coordinates are computed using\n  the binary "
            << "file and the SolarSystem class, and compared with the JPL values;\n  "
            << "any difference larger than 10^-13 is noted with the word 'Failure' "
            << "at EOL.\n  Note that some large coordinate values may differ at the "
            << "level of 10^-13 because the\n  size of double precision is barely "
            << "able to hold that much precision; compare the\n  computed value "
            << "with the JPL value (copied as a string) in the output file.\n"
            << "\n Usage: " << PrgmName << " [options]\n Options are:\n"
            << "   --log <file>   name of optional log file (otherwise stderr)\n"
            << "   --file <file>  name of binary SS ephemeris file\n"
            << "   --test <file>  name of JPL test file (e.g. testpo.403)\n"
            << "   --verbose      print info to the log file.\n"
            << "   --debug        print debugging info to the log file.\n"
            << "   --help         print this and quit.\n"
            ;
         return 0;
      }
      else if(i == 0) continue;
      else if(word == "-d" || word == "--debug") debug = true;
      else if(word == "-v" || word == "--verbose") verbose = true;
      else if(word == "--log") logFilename = string(argv[++i]);
      else if(word == "--file") inputFilename = string(argv[++i]);
      else if(word == "--test") testFilename = string(argv[++i]);
   }

   // test input
   if(inputFilename.empty()) {
      LOG(ERROR) << "Must specify an input binary file name";
      return -1;
   }
   if(testFilename.empty()) {
      LOG(ERROR) << "Must specify an input test file name";
      return -1;
   }

   if(!logFilename.empty()) {
      // choose the log file
      logstrm.open(logFilename.c_str(),ios_base::out);
      ConfigureLOG::Stream() = &logstrm;
      // if not the above, output is to stderr

      cout << Title << endl;
      cout << "Output is logged to file " << logFilename << endl;
   }

      // set the maximum level to be logged
   ConfigureLOG::ReportLevels() = ConfigureLOG::ReportTimeTags() = true;
   if(debug)
      ConfigureLOG::ReportingLevel() = ConfigureLOG::FromString("DEBUG");
   else if(verbose)
      ConfigureLOG::ReportingLevel() = ConfigureLOG::FromString("VERBOSE");
   else
      ConfigureLOG::ReportingLevel() = ConfigureLOG::FromString("INFO");
      // = any of ERROR,WARNING,INFO,VERBOSE,DEBUG,DEBUGn (n=1,2,3,4,5,6,7)
   //cout << "Reporting in main is "
   //   << ConfigureLOG::ToString(ConfigureLOG::ReportingLevel()) << endl;

   // display title in the log file
   LOG(INFO) << Title;

   // now read the binary file, and read selected records
   // use the binary to test using the JPL file testpo.<EPH#>
   LOG(VERBOSE) << "Initialize with file " << inputFilename;
   SSEphemeris.initializeWithBinaryFile(inputFilename);
   LOG(VERBOSE) << "End Initialize";
   LOG(INFO) << "Ephemeris number is " << SSEphemeris.JPLNumber();

   bool foundEOT=false;
   int target,center,coord;
   double JD,PV[6],value,diff;
   SolarSystem::Planet Target,Center;
   ifstream istrm;

   istrm.open(testFilename.c_str(),ios::in);
   if(!istrm.is_open()) {
      LOG(ERROR) << "Could not open test file " << testFilename;
   }
   else while(1) {
      string line,word;
      getline(istrm,line);
      stripTrailing(line,'\r');
      strip(line);
      if(line.empty())
         ;
      else if(line == string("EOT"))
         foundEOT = true;
      else if(!foundEOT)
         ;
      else {
         word = stripFirstWord(line);     // DEPHEM
         word = stripFirstWord(line);     // date in YYYY.MM.DD form
         JD = for2doub(stripFirstWord(line));
         target = asInt(stripFirstWord(line));
         center = asInt(stripFirstWord(line));
         coord = asInt(stripFirstWord(line)) - 1;  // my coords are 0-5, theirs 1-6
         word = stripFirstWord(line);
         value = for2doub(word);
         word = rightJustify(word,25);

         Target = SolarSystem::Planet(target);
         Center = SolarSystem::Planet(center);
         iret = SSEphemeris.computeState(JD, Target, Center, PV, false);
         if(iret == -1 || iret == -2) continue;   // time is not in file

         diff = fabs(PV[coord]-value);
         LOG(INFO) << fixed << setprecision(1) << setw(9) << JD
            << " " << setw(2) << target //<< " " << setw(2) << Target
            << " " << setw(2) << center //<< " " << setw(2) << Center
            << " " << setw(1) << coord+1
            << " " << scientific << setprecision(5) << setw(13) << diff
            << " " << word
            << " " << fixed << setprecision(20) << setw(25) << PV[coord]
            << " " << iret
            << (diff > 1.e-13 ? " Failure" : "")
            ;
      }

      if(istrm.eof() || !istrm.good()) break;
   }

   if(iret) { LOG(INFO) << PrgmName << " terminating with error code " << iret
      << ", which means " <<
      (iret == 0 ? "OK" : 
      (iret == -1 ? "last time in file was before first time in ephemeris" :
      (iret == -2 ? "time is beyond end time of ephemeris" :
      (iret == -3 ? "file reading failed" : "ephemeris file is corrupted")))) ;}

      // compute run time
   totaltime = clock()-totaltime;
   LOG(INFO) << PrgmName << " timing: " << fixed << setprecision(9)
      << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.";
   //if(LOGstrm != cout) cout << PrgmName << " timing: " << fixed << setprecision(9)
   //   << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds." << endl;

   return iret;
}
catch(Exception& e) {
   LOG(ERROR) << "GPSTk Exception : " << e.what();
}
catch (...) {
   LOG(ERROR) << "Unknown error in " << PrgmName << ".  Abort." << endl;
}
   return -1;
}   // end main()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
