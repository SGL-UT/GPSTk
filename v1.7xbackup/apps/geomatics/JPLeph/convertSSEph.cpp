#pragma ident "$Id$"

/**
 * @file convertSSEph.cpp
 * Read JPL ephemeris files in ascii/binary format, and write in binary/ascii.
 * Ascii files can be downloaded from JPL website.
 */

// ======================================================================
// This software was developed by Applied Research Laboratories, The
// University of Texas at Austin under contract to an agency or agencies
// within the U.S. Department of Defense. The U.S. Government retains all
// rights to use, duplicate, distribute, disclose, or release this
// software.
// 
// Copyright 2008 The University of Texas at Austin
// ======================================================================

//------------------------------------------------------------------------------------
// system includes
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <time.h>

// GPSTk
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "DayTime.hpp"
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
   DayTime CurrEpoch;         // defaults to current local time
   SolarSystem eph;

   // program name, title and version
   PrgmName = string("convertSSEph");
   Title = PrgmName + ", file conversion tool for JPL ephemeris, version "
      + Version + ", " + CurrEpoch.printf("Run %04Y/%02m/%02d at %02H:%02M:%02S");

   // default command line input
   bool ok=true;
   bool verbose=false,debug=false;
   string headerFilename,logFilename,outputFilename;
   vector<string> dataFilenames;

   // parse the command line input
   for(i=0; i<argc; i++) {
      string word = string(argv[i]);
      if(argc == 1 || word == "-h" || word == "--help") {
         cout << PrgmName
         << " reads a JPL planetary ephemeris in the form of an ASCII header file\n"
         << "and one or more ASCII data files and writes the data to a single "
         << "binary file\nfor use by other programs.\n"
         << "Note that on Windows, arguments with embedded commas must be quoted.\n"
         << " Usage: " << PrgmName << " [options]\n Options are:\n"
         << "   --log <file>         name of optional log file\n"
         << "   --header <file>      name of ASCII JPL header file, e.g. header.403\n"
         << "   --data <file[,file]> names of ASCII JPL data files, e.g. ascp2000.403\n"
         << "   --output <file>      name of output binary file\n"
         << "   --verbose            print info to the log file.\n"
         << "   --debug              print debugging info to the log file.\n"
         << "   --help               print this and quit.\n"
         ;
         cout << endl;
         return 0;
      }
      else if(i == 0) continue;
      else if(word == "-d" || word == "--debug") debug = true;
      else if(word == "-v" || word == "--verbose") verbose = true;
      else if(word == "--log") logFilename = string(argv[++i]);
      else if(word == "--header") headerFilename = string(argv[++i]);
      else if(word == "--data") {
         string field = string(argv[++i]);
         while( !(word = stripFirstWord(field,',')).empty())
            dataFilenames.push_back(word);
      }
      else if(word == "--output") outputFilename = string(argv[++i]);
   }

   // test input
   if(headerFilename.empty()) {
      LOG(ERROR) << "Must specify a header file";
      ok = false;
   }
   if(dataFilenames.size() == 0) {
      LOG(ERROR) << "Must specify data file name(s)";
      ok = false;
   }
   if(outputFilename.empty()) {
      LOG(ERROR) << "Must specify an output file name";
      ok = false;
   }
   if(!ok) return -1;

   // set up the log file
   if(!logFilename.empty()) {
      // choose the log file
      logstrm.open(logFilename.c_str(),ios_base::out);
      ConfigureLOG::Stream() = &logstrm;
      // if not the above, output is to stderr

      cout << Title << endl;
      cout << "Output logged in file " << logFilename << endl;
   }
      // set the maximum level to be logged
   if(debug)
      ConfigureLOG::ReportingLevel() = ConfigureLOG::FromString("DEBUG");
   else if(verbose)
      ConfigureLOG::ReportingLevel() = ConfigureLOG::FromString("VERBOSE");
   else
      ConfigureLOG::ReportingLevel() = ConfigureLOG::FromString("INFO");
      // = any of ERROR,WARNING,INFO,DEBUG,DEBUGn (n=1,2,3,4,5,6,7)
   //cout << "Reporting in main is "
   //   << ConfigureLOG::ToString(ConfigureLOG::ReportingLevel()) << endl;
   ConfigureLOG::ReportLevels() = ConfigureLOG::ReportTimeTags() = true;

   // display title in log file
   LOG(INFO) << Title;

   // read header file
   eph.readASCIIheader(headerFilename);
   LOG(VERBOSE) << "Finished reading ASCII header " << headerFilename;
   LOG(INFO) << "Ephemeris number from header is " << eph.JPLNumber();

   // read the data files
   eph.readASCIIdata(dataFilenames);
   for(i=0; i<dataFilenames.size(); i++)
      LOG(VERBOSE) << "Finished reading ASCII data " << dataFilenames[i];
   LOG(INFO) << "Ephemeris number from data is " << eph.JPLNumber();

   // dump to a file
   LOG(INFO) << "Dump ASCII header to csse.header.asc";
   ofstream ofs;
   ofs.open("csse.header.asc",ios_base::out);
   eph.writeASCIIheader(ofs);
   ofs.close();

   LOG(INFO) << "Dump ASCII data to csse.data.asc";
   ofs.open("csse.data.asc",ios_base::out);
   eph.writeASCIIdata(ofs);
   ofs.close();

   // write the whole thing out to a binary file
   LOG(INFO) << "Write to binary file " << outputFilename;
   eph.writeBinaryFile(outputFilename);
   LOG(INFO) << "Finished writing binary file.";

   // read it back in
   LOG(INFO) << "Read from binary file " << outputFilename;
   eph.readBinaryFile(outputFilename);
   LOG(INFO) << "Finished reading binary file " << outputFilename;

   // dump to a file
   LOG(INFO) << "Dump ASCII header to csse.header.bin.asc";
   ofs.open("csse.header.bin.asc",ios_base::out);
   eph.writeASCIIheader(ofs);
   ofs.close();

   LOG(INFO) << "Dump ASCII data to csse.data.bin.asc";
   ofs.open("csse.data.bin.asc",ios_base::out);
   eph.writeASCIIdata(ofs);
   ofs.close();
   LOG(INFO) << "Now compare the outputs by differencing";
   LOG(INFO) << " Try 'diff csse.data.asc csse.data.bin.asc'";
   LOG(INFO) << " and 'diff csse.data.asc csse.data.bin.asc'";

      // compute run time
   totaltime = clock()-totaltime;
   LOG(INFO) << PrgmName << " timing: " << fixed << setprecision(9)
      << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.";
   if(LOGstrm != cout) cout << PrgmName << " timing: " << fixed << setprecision(9)
      << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds." << endl;

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
