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
 * @file bc2sp3.cpp
 * Read RINEX format navigation file(s) and write the data out to an SP3 format file.
 * Potential problems related to discontinuities at change of BCE are ignored.
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "RinexNavStream.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"
#include "XvtStore.hpp"
#include "GPSEphemerisStore.hpp"
#include "SP3Stream.hpp"
#include "SP3Header.hpp"
#include "SP3Data.hpp"
#include "DayTime.hpp"
#include "SatID.hpp"
#include "StringUtils.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char *argv[])
{
   string Usage(
      "Usage: bc2sp3 <RINEX nav file(s)> [options]\n"
      " Read RINEX nav file(s) and write to SP3(a or c) file.\n"
      " Options (defaults):\n"
      "  --in <file>   Read the input file <file> (--in is optional, repeatable) ()\n"
      "  --out <file>  Name the output file <file> (sp3.out)\n"
      "  --tb <time>   Output beginning epoch; <time> = week,sec-of-week (earliest in input)\n"
      "  --te <time>   Output ending epoch; <time> = week,sec-of-week (latest in input)\n"
      "  --outputC     Output version c (no correlation) (otherwise a)\n"
      "  --msg \"...\"   Add ... as a comment to the output header (repeatable)\n"
      "  --verbose     Output to screen: dump headers, data, etc\n"
      "  --help        Print this message and quit\n"
      );
   if(argc < 2) { cout << Usage; return -1; }

   try
   {
      bool verbose=false;
      char version_out='a';
      int i,j,nrec,nfile;
      string fileout("sp3.out");
      vector<string> inputFiles;
      vector<string> comments;
      map<SatID,long> IODEmap;
      DayTime begTime=DayTime::BEGINNING_OF_TIME;
      DayTime endTime=DayTime::END_OF_TIME;
      DayTime tt;
      GPSEphemerisStore BCEph;
      SP3Header sp3header;
      SP3Data sp3data;

      for(i=1; i<argc; i++) {
         if(argv[i][0] == '-') {
            string arg(argv[i]);
            if(arg == string("--outputC")) {
               version_out = 'c';
               if(verbose) cout << " Output version c\n";
            }
            else if(arg == string("--in")) {
               inputFiles.push_back(string(argv[++i]));
               if(verbose) cout << " Input file name "
                  << inputFiles[inputFiles.size()-1] << endl;
            }
            else if(arg == string("--out")) {
               fileout = string(argv[++i]);
               if(verbose) cout << " Output file name " << fileout << endl;
            }
            else if(arg == string("--tb")) {
               arg = string(argv[++i]);
               int wk=StringUtils::asInt(StringUtils::stripFirstWord(arg,','));
               double sow=StringUtils::asDouble(StringUtils::stripFirstWord(arg,','));
               begTime.setGPSfullweek(wk,sow);
               if(verbose) cout << " Begin time "
                  << begTime.printf("%Y/%02m/%02d %2H:%02M:%06.3f = %F/%10.3g")
                  << endl;
            }
            else if(arg == string("--te")) {
               arg = string(argv[++i]);
               int wk=StringUtils::asInt(StringUtils::stripFirstWord(arg,','));
               double sow=StringUtils::asDouble(StringUtils::stripFirstWord(arg,','));
               endTime.setGPSfullweek(wk,sow);
               if(verbose) cout << " End time   "
                  << endTime.printf("%Y/%02m/%02d %2H:%02M:%06.3f = %F/%10.3g")
                  << endl;
            }
            else if(arg == string("--msg")) {
               comments.push_back(string(argv[++i]));
               if(verbose) cout << " Add comment " << comments[comments.size()-1]
                  << endl;
            }
            else if(arg == string("--help")) {
               cout << Usage;
               return -1;
            }
            else if(arg == string("--verbose"))
               verbose = true;
            else
               cout << "Ignore unknown option: " << arg << endl;
         }
         else {
            inputFiles.push_back(string(argv[i]));
            if(verbose) cout << " Input file name "
               << inputFiles[inputFiles.size()-1] << endl;
         }
      }

      if(inputFiles.size() == 0) {
         cout << "Error - no input filename specified. Abort.\n";
         return -1;
      }

      // open the output SP3 file
      SP3Stream outstrm(fileout.c_str(),ios::out);
      outstrm.exceptions(ifstream::failbit);

      for(nfile=0; nfile<inputFiles.size(); nfile++) {
         RinexNavHeader rnh;
         RinexNavData rnd;

         RinexNavStream rns(inputFiles[nfile].c_str());
         if(!rns) {
            cout << "Could not open input file " << inputFiles[nfile] << endl;
            continue;
         }
         rns.exceptions(ifstream::failbit);

         if(verbose) cout << "Reading file " << inputFiles[nfile] << endl;

         rns >> rnh;
         if(verbose) {
            cout << "Input";
            rnh.dump(cout);
         }

         while(rns >> rnd)
            if(rnd.health == 0) BCEph.addEphemeris(rnd);
   
      }

      // time limits, if not given by user
      if(begTime == DayTime::BEGINNING_OF_TIME)
         begTime = BCEph.getInitialTime();
      if(endTime == DayTime::END_OF_TIME)
         endTime = BCEph.getFinalTime();

      // define the data version and the header info
      if(version_out == 'c') {
         // data and header must have the correct version
         sp3data.version = sp3header.version = 'c';

         sp3header.system = SP3SatID();
         sp3header.timeSystem = SP3Header::timeGPS;
         sp3header.basePV = 0.0;
         sp3header.baseClk = 0.0;
      }
      else {
         sp3data.version = sp3header.version = 'a';
      }

      // fill the header
      sp3header.pvFlag = 'V';
      sp3header.time = DayTime::END_OF_TIME;
      sp3header.epochInterval = 900.0;          // hardcoded here only
      sp3header.dataUsed = "BCE";
      sp3header.coordSystem = "WGS84";
      sp3header.orbitType = "   ";
      sp3header.agency = "ARL";

      // determine which SVs, with accuracy, start time, epoch interval,
      // number of epochs, for header
      // this is a pain....
      sp3header.numberOfEpochs = 0;
      tt = begTime;
      while(tt < endTime) {
         bool foundSome = false;
         for(i=1; i<33; i++) {            // for each PRN ...
            SatID sat(i,SatID::systemGPS);
            try { EngEphemeris ee = BCEph.findEphemeris(sat, tt); }
            catch(InvalidRequest& nef) { continue; }

            if(sp3header.satList.find(sat) == sp3header.satList.end()) {
               sp3header.satList[sat] = 0;        // sat accuracy = ?
               IODEmap[sat] = -1;
            }

            if(!foundSome) {
               sp3header.numberOfEpochs++;
               foundSome = true;
               if(tt < sp3header.time) sp3header.time = tt;
            }
         }
         tt += sp3header.epochInterval;
      }

      // add comments
      if(comments.size() > 0) {
         // try to keep existing comments
         for(i=0; i<comments.size(); i++) {
            if(i > 3) {
               cout << "Warning - only 4 comments are allowed in SP3 header.\n";
               break;
            }
            sp3header.comments.push_back(comments[i]);
         }
      }

      // dump the SP3 header
      if(verbose) sp3header.dump(cout);

      // write the header
      outstrm << sp3header;

      // sigmas to output (version c)
      for(j=0; j<4; j++) sp3data.sig[j]=0;   // sigma = ?

      tt = begTime;
      while(tt < endTime) {
         bool epochOut=false;

         for(i=1; i<33; i++) {
            long iode;
            SatID sat(i,SatID::systemGPS);
            Xvt xvt;
            EngEphemeris ee;

            try { ee = BCEph.findEphemeris(sat, tt); }
            catch(InvalidRequest& nef) { continue; }

            sp3data.sat = sat;
            xvt = BCEph.getXvt(sat, tt);

            // epoch
            if(!epochOut) {
               sp3data.time = tt;
               sp3data.flag = '*';
               outstrm << sp3data;
               if(verbose) sp3data.dump(cout);
               epochOut = true;
            }

            // Position
            sp3data.flag = 'P';
            for(j=0; j<3; j++) sp3data.x[j] = xvt.x[j]/1000.0;       // km
            // must remove the relativity correction from Xvt::dtime
            // see EngEphemeris::svXvt() - also convert to microsec
            sp3data.clk = (xvt.dtime - ee.svRelativity(tt)) * 1000000.0;

            //if(version_out == 'c') for(j=0; j<4; j++) sp3data.sig[j]=...
            iode = ee.getIODE();
            if(IODEmap[sat] == -1) IODEmap[sat] = iode;
            if(IODEmap[sat] != iode) {
               sp3data.orbitManeuverFlag = true;
               IODEmap[sat] = iode;
            }
            else sp3data.orbitManeuverFlag = false;

            outstrm << sp3data;
            if(verbose) sp3data.dump(cout);

            // Velocity
            sp3data.flag = 'V';
            for(j=0; j<3; j++) sp3data.x[j] = xvt.v[j]/10.0;         // dm/s
            sp3data.clk = xvt.ddtime;                                // s/s
            //if(version_out == 'c') for(j=0; j<4; j++) sp3data.sig[j]=...

            outstrm << sp3data;
            if(verbose) sp3data.dump(cout);
         }

         tt += sp3header.epochInterval;
      }
      // don't forget this
      outstrm << "EOF" << endl;

      outstrm.close();

      if(verbose) cout << "Wrote " << sp3header.numberOfEpochs << " records" << endl;
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

   return 0;
} 
