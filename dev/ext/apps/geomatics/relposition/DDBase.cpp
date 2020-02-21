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
 * @file DDBase.cpp
 * Main program for DDBase - the ARL:UT network precise position estimation
 * using double differenced GPS carrier phase.
 */

//------------------------------------------------------------------------------------
// system includes
#include "TimeString.hpp"
#include <CivilTime.hpp>
#include <time.h>

// GPSTk
//#define RANGECHECK // throw on invalid ranges in Vector and Matrix
#include "Epoch.hpp"

// DDBase
#include "DDBase.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;

//------------------------------------------------------------------------------------
string Version("4.8 5/13/11");
// 4.8  5/13/11 Timetable algorithm 'using' ave time btwn segments for a gap; bug213
// 4.7b 6/23/10 Minor change so NewB trop. model works properly
// 4.7 12/10/08 Fix empty buffers bug (131) in Timetable
// 4.6 12/5/08  Remove clock jump code in ClockModel.cpp
// 4.5 11/11/08 Fixed bug in EditRawDataBuffers.cpp
//     10/31/07 Moved ClockModel back to before Synchronize, and SNR diagnostic output
//               (PlayWithSNRData) from ClockModel to Synchronization.
// 4.4  4/20/07 Removed BINEX support
// 4.3  4/20/07 Add BINEX support - must remain consistent with novaRinex
//4.2c 12/01/06 Reject small segments in EditDDs before calling Robust to avoid error.
//4.2b 12/01/06 Fix DT blunder in 4.2a
//4.2a 12/01/06 Add SNR stochastic model, stochastic model input. Read some data
//               when reading headers, and intelligently choose DT.
// 4.1  2/16/07 Enforce --Freq inside FillRawData(); earlier versions used other data.
//     11/08/06 Move clock and ephemeris processing to after synchronization,
//               pass pointer to ofstream for PRS output.
// 4.0  9/08/06 Convert to SatID and new RinexObs* names
// 3.9  7/27/06 EditDDs: pts between two combined slips not removed, TripD miscomputed
// 3.8  7/24/06 Remove CR at EOL and read line w/o CRLF at EOF
// 3.7  6/01/06 Added L3 and checked L2.
// 3.6  4/01/06 Fixed rotated antenna to work properly.
// 3.5  3/01/06 Catch std exceptions
// 3.4  2/27/06 Iterate slip fixing -- see test/Feb20/go15 for justification
// 3.3  2/22/06 Slip bug introduced in 2.8; revert to slip tolerance 0.5
// 3.2  2/16/06 Ignore unhealthy BC ephemeris, keep BCE in fit interval, screen output
// 3.1  2/02/06 Imp. convergence criterion, bias-fix iteration goes beyond nIter
// 3.0  2/02/06 Fixed: consequences of PosPRS option, MaxGap error
// 2.9 11/06/05 Added option for 'finals.daily' input for EOP
// 2.8 10/10/05 Improved EditDDs, incl changed slip tolerance to 0.9
// 2.7  9/20/05 Added rotated antenna option and ElevationMask routine
// 2.6  8/30/05 Improved stochastic model -- decreased eps and made cos^2
// 2.5  8/11/05

//------------------------------------------------------------------------------------
// TD DDBase -Edit the data within estimation loop..see memo on false slips w/--PosPRS
// TD DDBase -Add data sequentially, but invert less often (input)
// TD DDBase -Be careful how bias states are added..they should be added only when
// TD DDBase  they appear in the data. They don't need to be removed even if they
// TD DDBase  don't have any more data .. they can either be left in there (they won't
// TD DDBase  harm the position states) or the biases can be fixed when the data goes
// TD DDBase  away.
// TD DDBase -When there is no data in the file(s) for a site, it fails by finding PRS
// TD DDBase  equal to (0,0,0)...it should abort more gracefully.
// TD DDBase -When the time step is large, the test for slips using the triple diff
// TD DDBase  should be modified -- TDs are much larger
// TD DDBase -When there is a problem with the data often it fails with a 'slip' in
// TD DDBase  the triple difference at every epoch -- it should abort if #slips is big

//------------------------------------------------------------------------------------
// Global data -- declared extern in DDBase.hpp
string PrgmName("DDBase");    // program name, used in title, desc., and output
string Title;                 // program name and run time, for output
string PrgmDesc;              // program description, used in CommandInput
ofstream oflog;               // output log file stream
clock_t totaltime;            // for timing tests

CommandInput CI;              // all command line input .. see CommandInput.hpp

std::vector<std::string> Baselines;  // *computed* Baselines, (those to output in CI)

CommonTime SolutionEpoch;        // current solution epoch
CommonTime FirstEpoch,LastEpoch; // first and last solution epoch seen
CommonTime MedianEpoch;          // median of first and last, computed in Configure(3)
int Count;                    // current number of data intervals since first epoch
int minCount,maxCount;        // minimum and maximum timetag count seen
int begcount,endcount;        // first and last counts of *good* data in buffers
double wave;                  // wavelength (m) being processed (see CI.Frequency)

XvtStore<SatID> *pEph;        // pointer to ephemeris store (BC or SP3)
EOPStore EOPList;             // store of EarthOrientation parameters
EarthOrientation eorient;     // earth orientation parameters at mean time of dataset

// list of labels, which identify sites, and associated Station objects.
// label should be 4 char, but .. Don't allow a default b/c this ties inputs together.
map<string,Station> Stations;

vector<ObsFile> ObsFileList;  // list of all observation files

map<DDid,DDData> DDDataMap;   // buffered DDs

//------------------------------------------------------------------------------------
int OutputRawData(void);                     // DataOutput.cpp
// prototypes -- this module only

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
      // ------------------------------------------------------------------
      // START
   totaltime = clock();
   int iret;
   Epoch CurrEpoch;

      // Title title and version
   Title = PrgmName + ", ARL:UT DD phase estimation processor, Ver " + Version;
      // PrgmDesc description
   PrgmDesc = " Prgm " + PrgmName +
   " will read GPS data from any number of RINEX obs files and process them\n"
   " in a double-differenced carrier phase estimation algorithm to produce precise\n"
   " estimates of relative positions. Input is on the command line, or of the same\n"
   " format in a file (see -f<file> below). "
   "DDBase is built on the GPS Toolkit (GPSTk).\n"
   " NB. Input option --DT <data_interval_(seconds)> is optional but recommended.\n"
   " NB. Stations are defined, and many inputs for each are identified, by a label\n"
   "  (called station label or id below), which is case sensitive and must be used\n"
   "  consistently throughout. It cannot be 'X','Y' or 'Z' nor contain '-' or '_';\n"
   "  four characters work best.\n"
   " NB. There must be at least two stations defined, with observation file(s)\n"
   "  provided for each, and at least one station must be fixed.\n"
   " Options may be given in an input file (see -f<file>); the '#' character marks\n"
   " a comment, to EOL. All input options are shown below, followed by a\n"
   " description, and the default value, if there is one, in ().\n";

      // get current time
   CurrEpoch.setLocalTime();
   time_t timer;
   struct tm *tblock;
   timer = time(NULL);
   tblock = localtime(&timer);
   CurrEpoch=CivilTime(1900+tblock->tm_year,1+tblock->tm_mon,
               tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
      // print title and current time to screen
   Title += printTime(CurrEpoch,", Run %04Y/%02m/%02d %02H:%02M:%02S");
   cout << Title << endl;

   for(;;) {      // a convenience
         // ------------------------------------------------------------------
         // get command line input; -99 is 'help' return
      if((iret = CI.GetCmdInput(argc, argv))) break;

         // ------------------------------------------------------------------
         // test command input for validity
      if((iret = CI.ValidateCmdInput())) break;

         // dump command input to log
      if(CI.Verbose) {
         cout << "Output is directed to log file " << CI.LogFile << endl;
         CI.Dump(oflog);
      }

         // if 'validate' switch is on, quit here
      if(CI.Validate) break;

         // ------------------------------------------------------------------
         // Configure #1
      if((iret = Configure(1))) break;

         // ------------------------------------------------------------------
         // Open and read all files, compute PR solution, edit and buffer raw data
      if((iret = ReadAndProcessRawData())) break;

         // ------------------------------------------------------------------
         // Edit buffers
      if((iret = EditRawDataBuffers())) break;

         // ------------------------------------------------------------------
         // Output raw data buffers
      if((iret = OutputRawDataBuffers())) break;

         // ------------------------------------------------------------------
         // Configure #2
      if((iret = Configure(2))) break;

         // ------------------------------------------------------------------
         // clock processing
      if((iret = ClockModel())) break;

         // ------------------------------------------------------------------
         // synchronization of data to epoch (SolutionEpoch)
      if((iret = Synchronization())) break;

         // ------------------------------------------------------------------
         // correct ephemeris range, elevation, and compute phase windup
      if((iret = RecomputeFromEphemeris())) break;

         // ------------------------------------------------------------------
         // Orbit processing
      if((iret = EphemerisImprovement())) break;

         // ------------------------------------------------------------------
         // output 'raw' data here
      OutputRawData();

         // ------------------------------------------------------------------
         // Compute or read the timetable
      if((iret = Timetable())) break;

         // ------------------------------------------------------------------
         // Compute double differences, and buffer
      if((iret = DoubleDifference())) break;

         // ------------------------------------------------------------------
         // Edit double differences
      if((iret = EditDDs())) break;

         // ------------------------------------------------------------------
         // Configure #3 : prepare estimation
      if((iret = Configure(3))) break;

         // ------------------------------------------------------------------
         // Estimation
      if((iret = Estimation())) break;

      break;
   }  // end for(;;)

      // END --------------------------------------------------------------
      // error condition?  -99 is 'normal' help return from GetCmdInput
   if(iret != -99) {
      if(iret) {
         cerr << PrgmName << " terminating with error code " << iret << endl;
         oflog << PrgmName << " terminating with error code " << iret << endl;
      }
         // compute run time
      totaltime = clock()-totaltime;
      cout << PrgmName << " timing: " << fixed << setprecision(3)
         << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds." << endl;
      oflog << PrgmName << " timing: " << fixed << setprecision(3)
         << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds." << endl;
   }

   return iret;
}
catch(Exception& e) {
   cerr << "GPSTk Exception : " << e;
   oflog << "GPSTk Exception : " << e;
}
catch (...) {
   cerr << "Unknown error in DDBase.  Abort." << endl;
   oflog << "Unknown error in DDBase.  Abort." << endl;
}

      // close files
   oflog.close();
   return -1;
}   // end main()

//------------------------------------------------------------------------------------
