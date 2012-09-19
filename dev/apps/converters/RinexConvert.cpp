#pragma ident "$Id: RinexConvert.cpp 2009-08-25 17:32:43 tvarney $"

//============================================================================//
//                                  License                                   //
//                                                                            //
//  This file is part of GPSTk, the GPS Toolkit.                              //
//                                                                            //
//  The GPSTk is free software; you can redistribute it and/or modify it      //
//  under the terms of the GNU Lesser General Public License as published     //
//  by the Free Software Foundation; either version 2.1 of the License, or    //
//  any later version.                                                        //
//                                                                            //
//  The GPSTk is distributed in the hope that it will be useful, but          //
//  WITHOUT ANY WARRANTY; without even the implied warranty of                //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                      //
//  See the GNU Lesser General Public License for more details.               //
//                                                                            //
//  You should have received a copy of the GNU Lesser General Public License  //
//  along with GPSTk; if not, write to the Free Software Foundation, Inc.,    //
//  51 Franklin Street, Fifth Floor, Boston, MA 02110, USA                   //
//                                                                            //
//  Copyright 2009, The University of Texas at Austin                         //
//                                                                            //
//============================================================================//

#include "RinexConvert.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char** argv)
{
      //Parse the command line options
   int iret = parseCommandLine(argc, argv);
   if (iret < 0)
      return BAD_ARG;

   if (debug)
   {
      cout << "Input Files:" << endl;
      for (int i = 0; i < inputFiles.size(); ++i)
         cout << "   " << inputPath << inputFiles[i] << endl;
      
      cout << "Output Files:" << endl;
      for (int i = 0; i < outputFiles.size(); ++i)
         cout << "   " << outputPath << outputFiles[i] << endl;
   }
      //Get the number of input files and abort if 0
   int numFiles = inputFiles.size();
   if (numFiles == 0)
   {
      cout << "No input files! Aborting..." << endl;
      return NO_INPUT;
   }
   
   std::string filePath, output;
   int numConverted = 0;
   int numBad = 0;
   int numNotRinex = 0;

   for (int index = 0; index < numFiles; ++index)
   {
      filePath = "";
      output = "";
      
         //If an input path was given, add it
      if (inputPath.length() > 0)
         filePath = inputPath;
      
         //Add the file name to the filePath string
      filePath += inputFiles[index];
      
         //Create the output file name iff one was created for this file
      if (outputFiles[index].length() > 0)
      {
            //If an output path was given, add it
         if (outputPath.length() > 0)
            output = outputPath;
         
         output += outputFiles[index];
      }

      // Check file types.

      if (isRinexObsFile(filePath)) // file is a RINEX 2.11 Obs file
      {
         if (verbose)
            cout << inputFiles[index] << ": RINEX 2.11 Obs file" << endl;

         if (convertRinex2ObsFile(filePath, output))
         {
            numConverted++;
            if (verbose)
               cout << "Sucessfully converted " << inputFiles[index]
                    << " to RINEX 3.0" << endl;
         }
         else
         {
            numBad++;
            if (verbose)
               cout << "Could not convert " << inputFiles[index]
                    << " to RINEX 3.0" << endl;
         }
      }
      else if (isRinex3ObsFile(filePath)) // file is a RINEX 3.0 Obs file
      {
         if (verbose)
            cout << inputFiles[index] << ": RINEX 3.0 Obs file" << endl;

         if (convertRinex3ObsFile(filePath, output))
         {
            numConverted++;
            if (verbose)
               cout << "Sucessfully converted " << inputFiles[index]
                    << " to RINEX 2.11" << endl;
         }
         else
         {
            numBad++;
            if (verbose)
               cout << "Could not convert " << inputFiles[index]
                    << " to RINEX 2.11" << endl;
         }
      }
      else if (isRinexNavFile(filePath)) // file is a RINEX 2.11 Nav file
      {
         if (verbose)
            cout << inputFiles[index] << ": RINEX 2.11 Nav file" << endl;

         if (convertRinex2NavFile(filePath, output))
         {
            numConverted++;
            if (verbose)
               cout << "Sucessfully converted " << inputFiles[index]
                    << " to RINEX 3.0" << endl;
         }
         else
         {
            numBad++;
            if (verbose)
               cout << "Could not convert " << inputFiles[index]
                    << " to RINEX 3.0" << endl;
         }
      }
      else if (isRinex3NavFile(filePath)) // file is a RINEX 3.0 Nav file
      {
         if (verbose)
            cout << inputFiles[index] << ": RINEX 3.0 Nav file" << endl;

         if (convertRinex3NavFile(filePath, output))
         {
            numConverted++;
            if (verbose)
               cout << "Sucessfully converted " << inputFiles[index]
                    << " to RINEX 2.11" << endl;
         }
         else
         {
            numBad++;
            if (verbose)
               cout << "Could not convert " << inputFiles[index]
                    << " to RINEX 2.11" << endl;
         }
      }
      else // file is not RINEX 2.11 or 3.0
      {
         numNotRinex++;
         if (verbose)
            cout << "File format undetermined for: " << inputFiles[index] << endl;
      }
   }
   
   cout << "Successfully converted " << numConverted << " of "
        << numFiles << " files." << endl;
   cout << "There were " << numBad << " unconvertible RINEX files." << endl;
   cout << "There were " << numNotRinex << " files not RINEX 2.11 or 3.0." << endl;
}

bool convertRinex2ObsFile(std::string& fileName, std::string& outFile)
{
   bool retBool = true;

   try
   {
      // Open the input & output files.

      if (debug) cout << "Trying to open input file:" << fileName << endl;
      RinexObsStream obsIn(fileName.c_str(), ios::in);
      if (!obsIn) return false;
      else if (debug) cout << "...opened" << endl;

      if (outFile.length() == 0)
      {
         if (outputPath.length() > 0)
            outFile = outputPath;
         
         int lastIndexOf = fileName.find_last_of("\\/");
         if (lastIndexOf == -1)
            lastIndexOf = 0;
         outFile = fileName.substr(lastIndexOf);
      }
      
      if (debug) cout << "Trying to open output file: " << outFile << endl;
      Rinex3ObsStream obsOut(outFile.c_str(), ios::out);
      if (!obsOut) return false;
      else if (debug) cout << "...opened" << endl;

      // Declare the header and its converted version.

      RinexObsHeader robsHead;
      Rinex3ObsHeader convHead;

      // Read in the header data.

      if (debug) cout << "Reading in header..." << endl;
      obsIn >> robsHead;
      if (debug) cout << "...finished" << endl;

      // Convert the obs header and test, all in one step.
      // If the header couldn't be converted return false.

      if (debug) cout << "Converting header..." << endl;
      if (!RinexConverter::convertToRinex3(convHead, robsHead)) return false;
      if (debug) cout << "...finished" << endl;

      // Write out the converted header data.

      obsOut << convHead;

      if (debug)
      {
         convHead.dump(cout);
/*
         cout << "        Version: " << convHead.version << endl;
         cout << "      File Type: " << convHead.fileType << endl;
         cout << "         System: " << convHead.system << endl;
         cout << "   File Program: " << convHead.fileProgram << endl;
         cout << "    File Agency: " << convHead.fileAgency << endl;
         cout << "           Date: " << convHead.date << endl;
         cout << "    Marker Name: " << convHead.markerName << endl;
         cout << "  Marker Number: " << convHead.markerNumber << endl;
         cout << "    Marker Type: " << convHead.markerType << endl;
         cout << "       Observer: " << convHead.observer << endl;
         cout << "         Agency: " << convHead.agency << endl;
         cout << "        Rec. No: " << convHead.recNo << endl;
         cout << "      Rec. Type: " << convHead.recType << endl;
         cout << "   Rec. Version: " << convHead.recVers << endl;
         cout << "        Ant. No: " << convHead.antNo << endl;
         cout << "      Ant. Type: " << convHead.antType << endl;
         cout << "  Ant. Position: " << convHead.antennaPosition << endl;
         cout << " Ant. Delta HEN: " << convHead.antennaDeltaHEN << endl;
         cout << "       Interval: " << convHead.interval << endl;
         cout << "Receiver Offset: " << convHead.receiverOffset << endl;
         cout << "   Leap Seconds: " << convHead.leapSeconds << endl;
         cout << "        Num SVs: " << convHead.numSVs << endl;
         cout << "          Valid: " << convHead.valid << endl;
*/
      }

      // All of the data contained in the file.
      vector<RinexObsData> robsData;
      // A temporary data object for reading in from the stream.
      RinexObsData temp;
      // Converted data object.
      Rinex3ObsData convData;
      // Last observed epoch
      CommonTime lastEpoch = CommonTime::BEGINNING_OF_TIME;

      // Flags for the presence of the different systems
      bool hasGPS, hasGLO, hasGAL, hasGEO;
      hasGPS = hasGLO = hasGAL = hasGEO = false;

      if (debug) cout << "Start reading in data..." << endl;

      while(1)
      {
         try
         {
            obsIn >> temp;
         }
         catch (Exception gpstkEx)
         {
            if (printExceptions)
               cout << "Exception Reading Data:" << endl
                    << gpstkEx << endl;
            continue;
         }
         catch (exception stdEx)
         {
            if (printExceptions)
               cout << "Exception Reading Data:" << endl
                    << stdEx.what() << endl;
            continue;
         }
         catch (...)
         {
            if (printExceptions)
               cout << "Exception Reading Data" << endl;
            continue;
         }

         // Break for bad data or end of file.
         if (!obsIn.good() || obsIn.eof()) break;

         // Otherwise, save the data.
         robsData.push_back(temp);

         if (temp.time > lastEpoch)
            lastEpoch = temp.time;

         // Set the system flags.

         RinexSatID id;
         RinexObsData::RinexSatMap::const_iterator iter = temp.obs.begin();
         while(iter != temp.obs.end())
         {
            id = RinexSatID(iter->first);

            if      (id.systemChar() == 'G')
               hasGPS = true;
            else if (id.systemChar() == 'R')
               hasGLO = true;
            else if (id.systemChar() == 'E')
               hasGAL = true;
            else if (id.systemChar() == 'S')
               hasGEO = true;

            ++iter;
         }
      }

/// For now, comment out the following.  While logical, we will abide by the
/// philosophy that this converter sould simply read in and write out, not
/// alter the data in any way.
/*
      // Erase any systems that were not present.
      map<string, vector<ObsID> >::const_iterator mapIter;
      mapIter = convHead.mapObsTypes.find("G");
      if (!hasGPS && mapIter != convHead.mapObsTypes.end())
         convHead.mapObsTypes.erase("G");

      mapIter = convHead.mapObsTypes.find("R");
      if (!hasGPS && mapIter != convHead.mapObsTypes.end())
         convHead.mapObsTypes.erase("R");

      mapIter = convHead.mapObsTypes.find("E");
      if (!hasGPS && mapIter != convHead.mapObsTypes.end())
         convHead.mapObsTypes.erase("E");

      mapIter = convHead.mapObsTypes.find("S");
      if (!hasGPS && mapIter != convHead.mapObsTypes.end())
         convHead.mapObsTypes.erase("S");

      // close the input stream
      obsIn.clear();
      obsIn.close();
*/

      for (int i = 0; i < robsData.size(); ++i)
      {
         RinexConverter::convertToRinex3(convData, robsData[i], robsHead);
         obsOut << convData;
      }

//      obsOut.flush();
      obsOut.close();
   }
   catch(Exception gpstkException)
   {
      if (printExceptions)
         cout << "GPSTk Exception:" << gpstkException << endl;
      return false;
   }
   catch(exception stdException)
   {
      if (printExceptions)
         cout << "Exception:" << stdException.what() << endl;
      return false;
   }
   catch(...)
   {
      if (printExceptions) cout << "Exception!" << endl;
      return false;
   }
}

bool convertRinex3ObsFile(std::string& fileName, std::string& outFile)
{
   if (verbose)
   {
      cout << "RINEX 3 Obs to RINEX 2 Obs conversion not yet supported."
           << endl;
   }
   return false;
}

bool convertRinex2NavFile(std::string& fileName, std::string& outFile)
{
   if (verbose)
   {
      cout << "RINEX 2 Nav to RINEX 3 Nav conversion not yet supported."
           << endl;
   }
   return false;
}

bool convertRinex3NavFile(std::string& fileName, std::string& outFile)
{
   if (verbose)
   {
      cout << "RINEX 3 Nav to RINEX 2 Nav conversion not yet supported."
           << endl;
   }
   return false;
}

//============================================================================//
//============================================================================//

int parseCommandLine(int argc, char** argv)
{
//============================================================================//
//                                Set Defaults                                //
   inputPath = "";
   outputPath = "";
   verbose = false;
   printExceptions = false;
   debug = true;
//============================================================================//
//                               Create Options                               //
   
   RequiredOption filesOpt(CommandOption::hasArgument, CommandOption::stdType,
                        'f',"files",
                        " [-f|--file] <input[:output]> "
                        "Input/Output file pair. Output file is optional");
   
   CommandOption inPathOpt(CommandOption::hasArgument, CommandOption::stdType,
                        'i',"inpath",
                        " [-i|--inpath] <path>         "
                        "Path to search for input files");
   CommandOption outPathOpt(CommandOption::hasArgument, CommandOption::stdType,
                        'o',"outpath",
                        " [-o|--outpath] <path>        "
                        "Path to output files to");
   
   CommandOptionWithNumberArg verboseOpt(
                        'v',"verbose",
                        " [-v|--verbose] <0...3>       "
                        "Prints additional information to std::out\n"
                        "                                    0: Normal\n"
                        "                                    1: Prints extra information\n"
                        "                                    2: Prints extra information and Exceptions\n"
                        "                                    3: Prints all of the above and debug info");
   
   CommandOption helpOpt(CommandOption::noArgument, CommandOption::stdType,
                        'h',"help",
                        " [-h|--help]                  "
                        "Prints this help message and quits");
   CommandOption licenseOpt(CommandOption::noArgument, CommandOption::stdType,
                        'l',"license",
                        " [-l|--license]               "
                        "Prints licensing information about this program");
   
//============================================================================//
//                                Set up Parser                               //
   CommandOptionParser parser(description);
   inPathOpt.setMaxCount(1);
   outPathOpt.setMaxCount(1);
//============================================================================//
//                               Parse Arguments                              //
   parser.parseOptions(argc, argv);
//============================================================================//
//                                 Get Counts                                 //
   vector<string> arguments;
   
   if (argc == 0 || helpOpt.getCount() > 0 || filesOpt.getCount() == 0)
   {
      parser.displayUsage(cout,false);
      return -1;
   }
   if (parser.hasErrors())
   {
      parser.displayUsage(cout,false);
      return -1;
   }
   if (licenseOpt.getCount() > 0)
   {
      return -1;
   }
   if (verboseOpt.getCount() > 0)
   {
      arguments = verboseOpt.getValue();
      int level = StringUtils::asInt(arguments[arguments.size() - 1]);
      if (level > 3 || level < 0)
      {
         cout << "[-v|--verbose] takes a number argument from 0 to 3, given "
              << level << endl;
         parser.displayUsage(cout, true);
         return -1;
      }
      if (level != 0)
      {
         verbose = true;
         --level;
      }
      if (level != 0)
      {
         printExceptions = true;
         --level;
      }
      if (level != 0)
      {
         debug = true;
         --level;
      }

   }
   if (outPathOpt.getCount() > 0)
   {
      arguments = outPathOpt.getValue();
      outputPath = arguments[arguments.size() - 1];

      char lastChar = outputPath[outputPath.length() - 1];
      if ( !(lastChar == '\\' || lastChar == '/') )
      {
        // Don't know how to tell what OS we are on.
      	outputPath += "/";
      }
   }
   if (inPathOpt.getCount() > 0)
   {
      arguments = inPathOpt.getValue();
      inputPath = arguments[arguments.size() - 1];

      char lastChar = inputPath[inputPath.length() - 1];
      if ( !(lastChar == '\\' || lastChar == '/' ) )
      	inputPath += "/";
   }

   // I know filesopt has some, already checked getCount()
   arguments = filesOpt.getValue();
   int indexColon;
   for (int i = 0; i < arguments.size(); ++i)
   {
      indexColon = arguments[i].find_first_of(":");
      if (indexColon < 0)
      {
         inputFiles.push_back(arguments[i]);
         outputFiles.push_back(string());
         continue;
      }
      else
      {
         inputFiles.push_back(arguments[i].substr(0,indexColon));
         outputFiles.push_back(arguments[i].substr(indexColon+1));
         continue;
      }
   }

   return arguments.size();
}
