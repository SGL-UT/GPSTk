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
/// @file scnBrdcFile.cpp
/// Scan an IGS-generated brdc file in Rinex Nav format, fix various flaws, 
/// and write the results to a new RINEX nav file.  Optionally provide
/// a summary of what was fixed and the final state of the navigation
/// message data set.
///
/// This is also a demonstration of ext/lib/GNSSEph/RationalizeRinexNav and a 
/// test harness for the class RationalizeRinexNav.
///
/// Summary of What is Checked
///  - Fit intervals are consistently set to be in units of hours.
///  - For data sets with even Toc values, the HOW time is set to
///    the Toc - 1/2 fit interval (as per IS-GPS-200 20.3.4.4)
///  - For first data sets in an upload, there's a check to see 
///    that the HOW time has to be within the interval (Toc-2 bours, Toc).
///    If it is not in that interval, the HOW time is moved just inside
///    that period.   This is clearly not exactly correct, but what's 
///    in the file is clearly wrong and we want to at least maintain a 
///    record of the order in which the elements were received. 
///  - For second data sets in an upload, the HOW time is set to
///    the Toc 1/2 fit interval (as per IS-GPS-200 20.3.3.4).
/// The logic for all the above is located in the class RationalizeRinexNav.
///
///
/// January 2018.  Extension to detect and eliminate mis-tagged eph. 
/// After loading all the data sets, and BEFORE doing any of the checking
/// above, do the following for each PRN in sequence.
///    -- For each data set, compute the SV location at at the beginning
///       of the day.  Do this regardless of fit interval.
///    -- For each adjacent pair of data sets, compute the difference 
///       between the beginning of day location for the two sets.
///    -- If any differences exceed a threshold of TBD, do the following.
///      o Compute the average beginningn of day location using all data sets EXCEPT
///        those involved in differences that exceeded the threshold.
///        (Assuming that there are at least two such data sets.)
///      o For the data sets involved in large differences, compute
///        the differences against the average.  For one data set, 
///        the difference should be much larger.   Remove that 
///        data set from the collection.
/// This logic for this is located in the class RationalizRinexNav
/// 
///
// System
#include <iostream>
#include <fstream>
#include <cmath>

// gpstk
#include "BasicFramework.hpp"
#include "RationalizeRinexNav.hpp"
#include "StringUtils.hpp"

using namespace std;
using namespace gpstk; 

class scanBrdcFile : public gpstk::BasicFramework
{
public:
   scanBrdcFile(const std::string& applName,
              const std::string& applDesc) throw();
   ~scanBrdcFile() {}
   virtual bool initialize(int argc, char *argv[]) throw();
   
protected:
   virtual void process();
   gpstk::CommandOptionWithAnyArg inputOption;
   gpstk::CommandOptionWithAnyArg outputOption;
   gpstk::CommandOptionWithAnyArg sumOption;

   ofstream out; 
   bool sumOn; 

      // Variables that track file-specific information
   map<string,string> inAndOutFileNames;
};

int main( int argc, char*argv[] )
{
   try
   {
      scanBrdcFile fc("scanBrdcFile", "");
      if (!fc.initialize(argc, argv)) return(false);
      fc.run();
   }
   catch(gpstk::Exception& exc)
   {
      cout << exc << endl;
      return 1;
   }
   catch(...)
   {
      cout << "Caught an unnamed exception. Exiting." << endl;
      return 1;
   }
   return 0;
}

scanBrdcFile::scanBrdcFile(const std::string& applName, 
                       const std::string& applDesc) throw()
          :BasicFramework(applName, applDesc),
           inputOption('i', "input-file", "The name of the input file(s) to read.", true),
           outputOption('o', "output-file", "The name of the output file(s) to write.", true),
           sumOption('s', "summary", "The name of the summary to write (default=stdout).", false)
{
}

bool scanBrdcFile::initialize(int argc, char *argv[])
   throw()
{
   if (!BasicFramework::initialize(argc, argv)) return false;
   
   if (inputOption.getCount()!=outputOption.getCount())
   {
      cout << "Number of input files (" << inputOption.getCount() 
           <<") and output files (" << outputOption.getCount() << ") must match." << endl; 
      exitCode = OPTION_ERROR; 
      return false;
   }

   if (debugLevel && sumOption.getCount())
   {
      cout << "Summary file: " << sumOption.getValue().front() << endl;
   }
   
      // Open the summary file
   sumOn = false;             // default case
   if (sumOption.getCount())
   {
      out.open( sumOption.getValue().front().c_str());
      if (out.fail())
      {
         cout<<"Error opening output file"<<endl;
         exitCode = OPTION_ERROR; 
         return false;
      }
      sumOn = true; 
   }   

   if (sumOn)
   {
      out << "Input File -> to Output File" << endl; 
      out << "----------------------------" << endl;
   }
   for (int i=0;i<inputOption.getCount();i++)
   {
      string inFn = inputOption.getValue()[i];
      string outFn = outputOption.getValue()[i];
      inAndOutFileNames[inFn] = outFn;
      out << inFn << " -> " << outFn << endl; 
   }
   out << endl;

   return true;   
}

void scanBrdcFile::process()
{
   RationalizeRinexNav rrn;

   map<string,string>::const_iterator citFn;
   for (citFn=inAndOutFileNames.begin();
        citFn!=inAndOutFileNames.end();citFn++)
   {
      string fn = citFn->first;
      if (!rrn.inputFile(fn))
      {
         cout << "Failed to read input file '" << fn << endl;
         cout << "scanBrdcFile will terminate." << endl;
         exitCode = EXIST_ERROR;
         return;
      }
   }

      // Dump as-read summary 
   if (sumOn)
   {
      out << endl << "One-line summaries of data sets as they were read" << endl;
      rrn.dump(out);
   }

      // Remove any data sets that are labelled (tagged) withe
      // the wrong PRN ID
   rrn.removeMisTaggedDataSets();

      // Attempt to fix up the data set
   rrn.rationalize();

   for (citFn=inAndOutFileNames.begin();
        citFn!=inAndOutFileNames.end();citFn++)
   {
      string inFn = citFn->first;
      string outFn = citFn->second;

      rrn.writeOutputFile(inFn,outFn,"scanBrdcFile");
   }

      // Dump final state
   if (sumOn)
   {
      out << endl << "One-line summaries of data sets after rationalization" << endl;
      out << "with actions for each set shown" << endl;
      rrn.outputActionLog(out);
   }

   // As a test, attempt to load and report the errors
   if (sumOn && debugLevel)
   {
      OrbitEphStore oes;
      if (!rrn.loadStore(oes))
      {
         out << "Loaded " << rrn.getNumLoaded() << ". But some failed to load." << endl;
         out << "Load Errors: " << endl;
         out << rrn.getLoadErrorList() << endl;
      }
      else
      {
         out << "All data sets successfully loaded into an OrbitEphStore object." << endl;
      }
   }

   // Writing the new file will be added here
   if (sumOn)
   {
      out << endl << "End of summary " << endl;
      out.close();
   }

}
