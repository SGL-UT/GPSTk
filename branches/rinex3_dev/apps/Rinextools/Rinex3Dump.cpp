#pragma ident "$Id"

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
 * @file Rinex3Dump.cpp
 * 
 * Read a RINEX file and dump the data for the given satellite(s).
 * 
 * Any number of obstypes may appear in the command; if none appear, all are dumped.
 * Any number of satellite ID (e.g. G27) may appear; if none appear, all are dumped.
 * 
 * The output file is ASCII column-delimited with time, satellite ID and then three
 * columns 'observation LLI SSI' for each observation type.
 */

/**
 * Includes
 */
#include "CommandOptionParser.hpp"
#include "CommandOption.hpp"

#include "CommonTime.hpp"
#include "ObsID.hpp"
#include "RinexSatID.hpp"

#include "Rinex3ObsBase.hpp"
#include "Rinex3ObsData.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsStream.hpp"

#include "RinexUtilities.hpp"
#include "StringUtils.hpp"

#include <string>
#include <vector>

/**
 * Namespaces
 */
using namespace gpstk;
using namespace std;
using namespace StringUtils;

/**
 * Global Variables
 */

// We dislike globals, consider passing things as parameters.

// Set this to True to print debug output. Set by -v flag.
bool               debug = false;
string             delim = " || ";

// These are input from the command line.
bool               allNumeric = false;
bool               dumpPos = false;
vector<string>     filenames;
vector<ObsID>      otList;
string             outputFormat = ("%4F %10.3g");
vector<RinexSatID> satList;

// These are constructed dependant on input from the command line.
bool dumpAllObs = false;
bool dumpAllSat = false;
bool dumpAll = false;

/**
 * Protoypes
 */
void dumpCommandLineOptions();

int getCommandLineOptions(int argc, char **argv) throw (Exception);

template <class T>
int index(const vector<T> v, const T& t);

int main(int argc, char *argv[]);

int processCommandLineOptions();

/**
 * Functions
 */

/**
 * Dumps all command line options for debug's sake.
 */
void dumpCommandLineOptions()
{
   cout << "COMMAND LINE ARGUMENTS" << endl;
   
   // Files
   cout << delim << "Files:" << endl;
   
   for (int i = 0; i < filenames.size(); i++)
   {
      cout << delim << filenames[i] << endl;
   }
   
   cout << delim << endl;
   
   // Format
   cout << delim << "Format: " << outputFormat << endl;
   
   cout << delim << endl;
   
   // Numeric Output (with some ternary operator mess.)
   cout << delim << "Only Numeric Output: ";
   allNumeric ?
        cout << "True" << endl
      : cout << "False" << endl;
   
   cout << delim << endl;
   
   // Obs Types
   cout << delim << "Obs Types:" << endl;
   
   for (int i = 0; i < otList.size(); i++)
   {
      cout << delim << otList[i].asRinex3ID() << endl;
   }
   
   cout << delim << "Dump All Obs Types: ";
   dumpAllObs ?
        cout << "True" << endl
      : cout << "False" << endl;
   
   cout << delim << endl;
   
   // Output Positions (with some ternary operator mess.)
   cout << delim << "Only Output Positions: ";
   dumpPos ?
        cout << "True" << endl
      : cout << "False" << endl;
   
   cout << delim << endl;
   
   // Sat IDs
   cout << delim << "Sat IDs:" << endl;
   
   for (int i = 0; i < satList.size(); i++)
   {
      cout << delim << satList[i].toString() << endl;
   }
   
   cout << delim << "Dump All Sat IDs: ";
   dumpAllSat ?
        cout << "True" << endl
      : cout << "False" << endl;
   
   cout << delim << endl;
   
   cout << delim << "Dump Everything: ";
   dumpAll ?
        cout << "True" << endl
      : cout << "False" << endl;
   
   cout << endl;
}

/**
 * Process command line options.
 * 
 * @param [in] argc The number of arguments.
 * @param [in] argv The command line input.
 * 
 * @return 0 on success.
 */
int getCommandLineOptions(int argc, char **argv) throw (Exception)
{
   try
   {
      // -f, --file
      CommandOptionWithArg dashfile(CommandOption::stdType,
         'f',
         "file",
         "    -f, --file <file>    <file> is a RINEX observation file. This option may be repeated.\n"
         "                         Optional, but may be needed in case of ambiguity.\n");
      
      // --format
      CommandOptionWithArg dashformat(CommandOption::stdType,
         0,
         "format",
         "    --format <format>    The format of the time output. Default is %4F %10.3g.\n");
      dashformat.setMaxCount(1);
      
      // -h, --help
      CommandOptionNoArg dashhelp('h',
         "help",
         "    -h, --help           Prints out this help and exits.\n");
      
      // -n, --num
      CommandOptionNoArg dashnum('n',
         "num",
         "    -n, --num            Make output purely numeric, ie. no header, no system char on satellites.\n");
      
      // -o, --obs
      CommandOptionWithArg dashobs(CommandOption::stdType,
         'o',
         "obs",
         "    -o, --obs <obs>      <obs> is a RINEX observation type (eg. C1C) found in the file header.\n"
         "                         Optional, but may be needed in case of ambiguity.\n");
      
      // -p, --pos
      CommandOptionNoArg dashpos('p',
         "pos",
         "    -p, --pos            Only output positions from aux headers, ie. sat and obs are ignored.\n");
      
      // -s, --sat
      CommandOptionWithArg dashsat(CommandOption::stdType,
         's',
         "sat",
         "    -s, --sat <sat>      <sat> is a RINEX satellite ID (eg. For GPS PRN 31, <sat> = G01).\n"
         "                         Optional, but may be needed in case of ambiguity.\n");

      // -v, --verbose
      CommandOptionNoArg dashverbose('v',
         "verbose",
         "    -v, --verbose        Prints out verbose output.\n");

      CommandOptionRest rest("<file> <obs> <sat>");

      CommandOptionParser parser("Read RINEX file(s) and dump the given obvservation types in columns.\n"
         "Output is to the console, with one time tag and satellite per line.\n"
         "\n"
         "If no satellites are given, all are output. The same holds for observationt types.\n"
         "Output begins with header lines starting with # that identify input and columns.\n"
         "\n"
         "eg. ./Rinex3Dump ARL8262.09o.R3 C1C G01\n");
      
      // Count arguments.
      vector<string> args;
      
      for (int i = 1; i < argc; i++)
      {
         args.push_back(argv[i]);
      }
      
      if (args.size() == 0) args.push_back(string("-h"));
      
      argc = args.size() + 1;
      char **cargs;
      cargs = new char* [argc];
      
      if(!cargs)
      {
         cerr << "Error! Failed to allocate cargs.\n";
         return -1;
      }
      
      cargs[0] = argv[0];
      
      for (int i = 1; i < argc; i++)
      {
         cargs[i] = new char[args[i-1].size()+1];
         
         if (!cargs[i])
         {
            cerr << "Error! Failed to allocate cargs[i].\n";
            return -1;
         }
         
         strcpy(cargs[i], args[i-1].c_str());
      }
      
      // Parse arguments.
      parser.parseOptions(argc, cargs);
      
      delete[] cargs;
      
      // Check for errors.
      if (parser.hasErrors())
      {
         parser.displayUsage(cout, false);
         cerr << "Error! Errors found in input:" << endl;
         parser.dumpErrors(cerr);
         cerr << endl;
         
         return -1;
      }
      
      // Set verbosity.
      if (dashverbose.getCount())
      {
         debug = true;
      }
      
      // Check for help option.
      if (dashhelp.getCount() > 0)
      {
         parser.displayUsage(cout, false);
         
         return -1;
      }
      
      // Get the actual values from the command line.
      vector<string> values;

      ObsID ot;
      RinexSatID sat;
      
      if (dashfile.getCount())
      {
         values = dashfile.getValue();
         
         for (int i = 0; i < values.size(); i++)
         {
            if (debug) cout << "Added file " << values[i] << "." << endl;
            filenames.push_back(values[i]);
         }
      }
      
      if (dashformat.getCount())
      {
         values = dashformat.getValue();
         
         if (debug) cout << "Set format to " << values[0] << "." << endl;
         outputFormat = values[0];
      }
      
      if (dashnum.getCount())
      {
         if (debug) cout << "Set allNumeric = true." << endl;
         allNumeric = true;
      }
      
      if (dashobs.getCount())
      {
         values = dashobs.getValue();
         
         for (int i = 0; i < values.size(); i++)
         {
            try
            {
               ot = ObsID(values[i]);
                              
               if (asString(ot).compare("  ") != 0) // A valid ObsID.
               {
                  if (debug) cout << "Added obs type " << values[i] << "." << endl;
                  otList.push_back(ot);
               }
               else
               {
                  Exception e("Invalid ObsID, in Rinex3Dump.cpp:if (dashobs.getCount()).");
                  GPSTK_THROW(e);
               }
            }
            catch (Exception& e)
            {
               cerr << "Error! Input argument " << values[i] << " is not a valid obs type." << endl;
               return -1;
            }
         }
      }
      
      if (dashpos.getCount())
      {
         dumpPos = true;
      }
      
      if (dashsat.getCount())
      {
         values = dashsat.getValue();
         
         for (int i = 0; i < values.size(); i++)
         {
            sat.fromString(string(values[i]));
            
            if (sat.isValid())
            {
               if (debug) cout << "Added satellite ID " << values[i] << "." << endl;
               satList.push_back(sat);
            }
            else
            {
               cerr << "Error! Input argument " << values[i] << " is not a valid satellite ID." << endl;
               return -1;
            }
         }
      }
      
      // Process the remaining input.
      values = rest.getValue();
      
      bool maybeMoreFilenames = true;
      bool maybeMoreObs = true;
      bool maybeMoreSat = true;
      bool noMatch = true;
      
      for (int i = 0; i < values.size(); i++)
      {
         if (debug) cout << "Figuring out what " << values[i] << " could be..." << endl;
         
         // See if it's a filename...
         if (maybeMoreFilenames)
         {
            if (isRinex3ObsFile(values[i]))
            {
               if (debug) cout << "Added file " << values[i] << "." << endl;
               cout << endl;
               filenames.push_back(values[i]);
               
               continue;
            }
         }
         
         // See if it's an obs type...
         if (maybeMoreObs)
         {
            try
            {
               ot = ObsID(values[i]);
               
               if (asString(ot).compare("  ") != 0) // A valid ObsID.
               {
                  if (debug) cout << "Added obs type " << values[i] << "." << endl;
                  
                  /*
                  if (debug) cout << "               ";
                  if (debug) ot.dump(cout);
                  if (debug) cout << endl;
                  */
                  
                  cout << endl;
                  otList.push_back(ot);
                  
                  // We only allow filenames to come before obs types.
                  maybeMoreFilenames = false;
                  
                  continue;
               }
            }
            catch (Exception& e)
            {
               // Not an ObsID. Nothing to see here. Move along.
            }
         }
         
         // See if it's a sat ID...
         if (maybeMoreSat)
         {
            try
            {
               sat.fromString(string(values[i]));
               
               if (sat.isValid())
               {
                  if (debug) cout << "Added satellite ID " << values[i] << "." << endl;
                  cout << endl;
                  satList.push_back(sat);
                  
                  // We only allow filenames and obs types to come before sat IDs.
                  maybeMoreFilenames = false;
                  maybeMoreObs = false;
                  
                  continue;
               }
            }
            catch (Exception& e)
            {
               // Not a sat ID. Nothing to see here. Move along.
            }
         }
         
         // Didn't match anything.
         if (noMatch)
         {
            cerr << "Error! Could not figure out what input argument " << values[i] << " is." << endl;
            cerr << endl;
            cerr << "       Arguments must be in proper order, ie. <file> <obs> <sat>." << endl;
            cerr << endl;
            cerr << "       See help for more information (-h or --help)." << endl;
            cerr << endl;

            //parser.displayUsage(cout, false);
            
            return -1;
         }
      } // i < values.size()
      
      return 0;
   }
   catch (Exception& e)
   {
      GPSTK_THROW(e);
   }
   
   return -1;
}
// getCommandLineOptions() end

/**
 * Finds the index of the first occurence of an item in a vector..
 * 
 * @param [in] v The vector<T> to search.
 * @param [in] t The object to look for.
 * 
 * @return The first index in which t occurs, otherwise -1.
 */
template <class T>
int index(const vector<T> v, const T& t)
{
   for (int i = 0; i < v.size(); i++)
   {
      if (v[i] == t) return i;
   }
   
   return -1;
}
// index() end

/**
 * The main program.
 * 
 * The output files generated should match perfectly to the input files on diff.
 * 
 * @return 0 on success.
 */
int main(int argc, char *argv[])
{
   try
   {
      int e; // Error code.
      
      e = getCommandLineOptions(argc, argv);
      if (e) return e; // If there's an error, die.
      
      e = processCommandLineOptions();
      if (e) return e; // If there's an error, die.
      
      if (debug) dumpCommandLineOptions();
      
      e = RegisterARLUTExtendedTypes();
      if (e) return e; // If there's an error, die.
      
      // Begin dumping input files...
      string filename;
      Rinex3ObsData data;
      Rinex3ObsHeader header;
      
      for (int i = 0; i < filenames.size(); i++)
      {
         filename = filenames[i];
         
         Rinex3ObsStream rinFile(filename.c_str());
         rinFile.exceptions(fstream::failbit);
         
         // Try to read the header of rinFile.
         try
         {
            rinFile >> header;
         }
         catch (Exception& e)
         {
            cerr << "Error! Input file " << filename << " is not a valid Rinex3 Obs file." << endl;
            return -1;
         }
         
         if (dumpAllObs)
         {
            otList.clear();
            
            // Using Obs Type map
            /*
            map<string, vector<ObsID> >::const_iterator map_iter;
            
            if (debug) cout << "Obs Types found in " << filename << ":" << endl;
            
            for (map_iter = header.mapObsTypes.begin(); map_iter != header.mapObsTypes.end(); map_iter++)
            {
               if (debug) cout << delim << map_iter->first << " Observation Types (" << map_iter->second.size() << "):" << endl;
               
               for (int k = 0; k < map_iter->second.size(); k++)
               {
                  if (debug) cout << delim << delim << "Type #" << k+1 << " = " << asRinex3ID(map_iter->second[k]) << endl;
                  
                  otList.push_back(map_iter->second[k]);
               }
            }
            */
            
            // Using Obs Type vector
            if (debug) cout << header.numObs << " Obs Types found in " << filename << ":" << endl;
            
            for (int k = 0; k < header.obsTypeList.size(); k++)
            {
               //if (debug) cout << delim << header.obsTypeList[k] << endl;
               if (debug) cout << delim;
               if (debug) header.obsTypeList[k].dump(cout);
               if (debug) cout << endl;
            }
            
         }
         else
         {
            // Check that the obs types exist in the header.
            map<string, vector<ObsID> >::const_iterator map_iter;
            vector<ObsID>::iterator obs_iter;
            
            for (obs_iter = otList.begin(); obs_iter != otList.end(); )
            {
               
            }
         }
         
      } // i < filenames.size()
   }
   catch (Exception& e)
   {
      cerr << "Error! Exception!" << endl << e << endl;
   }
   
   return 0;
}
// main() end

/**
 * Goes through input arguments and processes them.
 * 
 * @return 0 on success.
 */
int processCommandLineOptions()
{
   if (otList.size() == 0) dumpAllObs = true;
   
   if (satList.size() == 0) dumpAllSat = true;
   
   if (dumpAllObs && dumpAllSat) dumpAll = true;
   
   // If there are files, sort them based on the "begin time" in their headers.
   if (filenames.size() > 0)
   {
      sortRinex3ObsFiles(filenames);
   }
   else
   {
      cerr << "Error! No filenames found." << endl;
      return -1;
   }
   
   // Check if every file "exists".
   // NB. What does it mean for a file to "exist"?
   string filename;
   
   for (int i = 0; i < filenames.size(); i++)
   {
      filename = filenames[i];
      
      Rinex3ObsStream rinFile(filename.c_str());
      
      if (filename.empty() || !rinFile)
      {
         cerr << "Error! Input file " << filename << " does not exist." << endl;
         return -1;
      }
   }
   
   return 0;
}
// processCommandLineOptions() end