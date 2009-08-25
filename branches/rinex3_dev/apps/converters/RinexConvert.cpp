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
//  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA                   //
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
   if(iret < 0)
      return BAD_ARG;
   
   printTitle(&cout);
   
   
      //Get the number of input files and abort if 0
   int numFiles = inputFiles.size();
   if(numFiles == 0)
   {
      cout << "No input files! Aborting..." << endl;
      return NO_INPUT;
   }
   
   std::string filePath, output;
   int numConverted = 0;
   int numBad = 0;
   int numNotRinex = 0;
   bool retVal;
   for(int index = 0; index < numFiles; ++index)
   {
      filePath = "";
      output = "";
      
         //If an input path was given, add it
      if(inputPath.length() > 0)
         filePath = inputPath;
      
         //Add the file name to the filePath string
      filePath += inputFiles[index];
      
         //Create the output file name iff one was created for this file
      if(outputFiles[index].length() > 0)
      {
            //If an output path was given, add it
         if(outputPath.length() > 0)
            output = outputPath;
         
         output += outputFiles[index];
      }
         //Check file types...
      if(isRinexObsFile(filePath))
      {
            //File is a RINEX 2.11 Observation File
         if(verbose)
         {
            cout << inputFiles[index] << ": Rinex 2.11 Observation File"
                 << endl;
         }
         
         retVal = convertRinex2ObsFile(filePath, output);
         if(retVal)
         {
            if(verbose)
            {
               cout << "Sucessfully Converted " << inputFiles[index]
                    << " to RINEX 3.0" << endl;
            }
            
            numConverted++;
         }
         else
         {
            if(verbose)
            {
               cout << "Could not convert " << inputFiles[index]
                    << " to Rinex 3.0" << endl;
            }
            
            numBad++;
         }
      }
      else if(isRinex3ObsFile(filePath))
      {
            //File is a Rinex 3.0 Observation File
         cout << inputFiles[index] << ": RINEX 3.0 Observation File" << endl;
         retVal = convertRinex3ObsFile(filePath, output);
         if(retVal)
         {
            if(verbose)
            {
               cout << "Sucessfully Converted " << inputFiles[index]
                    << " to RINEX 2.11" << endl;
            }
            
            numConverted++;
         }
         else
         {
            if(verbose)
            {
               cout << "Could not convert " << inputFiles[index]
                    << " to Rinex 2.11" << endl;
            }
            
            numBad++;
         }
      }
      else if(isRinexNavFile(filePath))
      {
            //File is a Rinex 2.11 Navigation File
         if(verbose)
            cout << inputFiles[index] << ": RINEX 2.11 Navigation File" << endl;
         
         retVal = convertRinex2NavFile(filePath, output);
         if(retVal)
         {
            if(verbose)
            {
               cout << "Sucessfully Converted " << inputFiles[index]
                    << " to RINEX 3.0" << endl;
            }
            
            numConverted++;
         }
         else
         {
            if(verbose)
            {
               cout << "Could not convert " << inputFiles[index]
                    << " to Rinex 3.0" << endl;
            }
            
            numBad++;
         }
      }
      else if(isRinex3NavFile(filePath))
      {
            //File is a RINEX 3.0 navigation file
         if(verbose)
            cout << inputFiles[index] << ": RINEX 3.0 Navigation File" << endl;
         
         retVal = convertRinex3NavFile(filePath, output);
         if(retVal)
         {
            if(verbose)
            {
               cout << "Sucessfully Converted " << inputFiles[index]
                    << " to RINEX 2.11" << endl;
            }
            
            numConverted++;
         }
         else
         {
            if(verbose)
            {
               cout << "Could not convert " << inputFiles[index]
                    << " to Rinex 2.11" << endl;
            }
            
            numBad++;
         }
      }
      else
      {
            //File is not a RINEX 2.11 or RINEX 3 file type
         if(verbose)
         {
            cout << "File Format not recognized for file: " << inputFiles[index]
                 << endl;
         }
         
         numNotRinex++;
      }
   }
   
   cout << "Successfully converted " << numConverted << " of "
        << inputFiles.size() << "files.";
   if(numBad > 0)
      cout << numBad << " Bad files" << endl;
   if(numNotRinex > 0)
      cout << numNotRinex << " Not RINEX 2.11 or 3.0" << endl;
}

bool convertRinex2ObsFile(std::string& fileName, std::string& outFile)
{
   bool retBool = true;
   
   RinexObsStream obsIn;
   Rinex3ObsStream obsOut;
   try
   {
         //Open the input file
      obsIn.open(fileName.c_str(), ios::in);
      
         //The header and it's converted version
      RinexObsHeader robsHead;
      Rinex3ObsHeader convHead;
      
         //Read in the header data
      obsIn >> robsHead;
      
        //Convert the obs header and test, all in one step;
        //if the header couldn't be converted return false.
      if(!RinexConverter::convertToRinex3(convHead, robsHead))
         return false;
      
         //All of the data contained in the file.
      vector<RinexObsData> robsData;
         //A temporary data object for reading in from the stream.
      RinexObsData temp;
         //Converted data object.
      Rinex3ObsData convData;
         //Last observed epoch
      CommonTime lastEpoch = CommonTime::BEGINNING_OF_TIME;
      
         //Flags for the presence of the different systems
      bool hasGPS, hasGLO, hasGAL, hasGEO;
      hasGPS = hasGLO = hasGAL = hasGEO = false;
      
      while(1)
      {
         try
         {
            obsIn >> temp;
			}
			catch(Exception gpstkEx)
			{
			   if(printExceptions)
			   {
			      cout << "Exception Reading Data:\n"
			           << gpstkEx << "\n" << endl;
			   }
			   continue;
			}
			catch(exception stdEx)
			{
			   if(printExceptions)
			   {
			      cout << "Exception Reading Data:\n"
			           << stdEx.what() << "\n" << endl;
			   }
			   continue;
			}
			catch(...)
			{
			   if(printExceptions)
			      cout << "Exception Reading Data\n" << endl;
			   continue;
			}
         
            //End of file
         if(!obsIn.good() || obsIn.eof())
            break;
         
            //Save the data...
	      robsData.push_back(temp);
         
         if(temp.time > lastEpoch)
            lastEpoch = temp.time;
         
            //Set the system flags
         RinexSatID id;
         RinexObsData::RinexSatMap::const_iterator iter = temp.obs.begin();
         while(iter != temp.obs.end())
         {
               //Check the system code of this satellite
            id = RinexSatID(iter->first);
            if(id.systemChar() == 'G')
               hasGPS = true;
            else if(id.systemChar() == 'R')
               hasGLO = true;
            else if(id.systemChar() == 'E')
               hasGAL = true;
            else if(id.systemChar() == 'S')
               hasGEO = true;
               //Increment iter
            ++iter;
	      }
	   }
	   
	      //Erase any systems that were not present
	   map<string, vector<ObsID> >::const_iterator mapIter;
	   mapIter = convHead.mapObsTypes.find("G");
	   if(!hasGPS && mapIter != convHead.mapObsTypes.end())
	      convHead.mapObsTypes.erase("G");
	   
	   mapIter = convHead.mapObsTypes.find("R");
	   if(!hasGPS && mapIter != convHead.mapObsTypes.end())
	      convHead.mapObsTypes.erase("R");
	   
	   mapIter = convHead.mapObsTypes.find("E");
	   if(!hasGPS && mapIter != convHead.mapObsTypes.end())
	      convHead.mapObsTypes.erase("E");
	   
	   mapIter = convHead.mapObsTypes.find("S");
	   if(!hasGPS && mapIter != convHead.mapObsTypes.end())
	      convHead.mapObsTypes.erase("S");
	   
	      //Close the input stream
	   obsIn.clear();
	   obsIn.close();
	   
	      //Open the output stream
	      //If outFile has any length, use that.
	   if(outFile.length() == 0)
	   {
	      if(outputPath.length() > 0)
	         outFile = outputPath;
	      
	      int lastIndexOf = fileName.find_last_of("\\/");
	      if(lastIndexOf == -1)
	         lastIndexOf = 0;
	      outFile = fileName.substr(lastIndexOf);
		}
		   //Open the file such that it overwrite any existing data...
		obsOut.open(outFile, ios::out | ios::trunc);
	   
	   obsOut << convHead;
	   
      for(int i = 0; i < robsData.size(); ++i)
      {
      	RinexConverter::convertToRinex3(convData, robsData[i], robsHead);
      	obsOut << convData;
		}
		
		obsOut.close();
   }
   catch(Exception gpstkException)
   {
      if(printExceptions)
         cout << "\nGPSTk Exception:" << gpstkException << "\n" << endl;
      
      return false;
   }
   catch(exception stdException)
   {
      if(printExceptions)
         cout << "\nException:" << stdException.what() << "\n" << endl;
      
      return false;
   }
   catch(...)
   {
      if(printExceptions)
         cout << "\nException!\n" << endl;
      
      return false;
   }
}
bool convertRinex3ObsFile(std::string& fileName, std::string& outFile)
{
   if(verbose)
   {
      cout << "RINEX 3 Obs file to RINEX 2 Obs file conversion not supported "
              "yet." << endl;
   }
   
   return false;
}

bool convertRinex2NavFile(std::string& fileName, std::string& outFile)
{
   if(verbose)
   {
      cout << "RINEX 2 Nav file to RINEX 3 Nav file conversion not supported "
              "yet." << endl;
   }
   
   return false;
}
bool convertRinex3NavFile(std::string& fileName, std::string& outFile)
{
   if(verbose)
   {
      cout << "RINEX 3 Nav file to RINEX 2 Nav file conversion not supported "
              "yet." << endl;
   }
   
   return false;
}

void printTitle(ostream* out)
{
	   //Print the title of the program and other information to cout
   *out << programName << ", part of the GPSTk, Version "
   *out << "Created by " << author << ", " << date << "\n";
        << version << "\n";
   *out << license << "\n";
   
   *out << endl;
}

int parseCommandLine(int argc, char** argv)
{
//============================================================================//
//                                Set Defaults                                //
   inputPath = "";
   outputPath = "";
   verbose = false;
   printExceptions = false;
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
   CommandOption helpOpt(CommandOption::noArgument, CommandOption::stdType,
                        'h',"help",
                        " [-h|--help]                  "
                        "Prints this help message and quits");
   CommandOption licenseOpt(CommandOption::noArgument, CommandOption::stdType,
                        'l',"license",
                        " [-l|--license]               "
                        "Prints licensing information about this program");
   
   CommandOptionWithNumberArg verboseOpt(
                        'v',"verbose",
                        " [-v|--verbose] <0...2>       "
                        "Prints additional information to std::out");
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
	
	if(argc == 0 || helpOpt.getCount() > 0 || filesOpt.getCount() == 0)
	{
		printTitle(&cout);
		parser.displayUsage(cout,false);
		return -1;
	}
	if(parser.hasErrors())
	{
	   printTitle(&cout);
	   parser.displayUsage(cout,false);
		return -1;
	}
	if(licenseOpt.getCount() > 0)
	{
		printTitle(&cout);
		return -1;
	}
	if(verboseOpt.getCount() > 0)
	{
		arguments = verboseOpt.getValue();
		int level = StringUtils::asInt(arguments[arguments.size() - 1]);
		switch(level)
		{
			case 0:
			   verbose = false;
			   printExceptions = false;
			   break;
			case 1:
			   verbose = true;
			   printExceptions = false;
			   break;
			case 2:
			   verbose = true;
			   printExceptions = false;
			   break;
			default:
			   printTitle(&cout);
			   cout << "[-v|--verbose] takes a number argument from 0 to 2, given "
			        << level << endl;
			   parser.displayUsage(cout, true);
		      return -1;
		}
	}
	if(outPathOpt.getCount() > 0)
	{
		arguments = verboseOpt.getValue();
		outputPath = arguments[arguments.size() - 1];
	}
	if(inPathOpt.getCount() > 0)
	{
		arguments = verboseOpt.getValue();
		inputPath = arguments[arguments.size() - 1];
	}
	
	//I know filesopt has some...
	arguments = filesOpt.getValue();
	int indexColon;
	for(int i = 0; i < arguments.size(); ++i)
	{
		indexColon = arguments[i].find_first_of(":");
		if(indexColon < 0)
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
