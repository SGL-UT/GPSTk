#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/filetools/navdmp.cpp#3 $"

/**
 * @file navdmp.cpp - print a human readable file from a binary FIC file
 */

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






#include "CommandOptionWithTimeArg.hpp"
#include "FileFilterFrame.hpp"
#include "BasicFramework.hpp"
#include "FICData.hpp"
#include "FICStream.hpp"
#include "FICFilterOperators.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavFilterOperators.hpp"

#include "gps_constants.hpp"

using namespace std;
using namespace gpstk;

class NavDump : public BasicFramework
{
public:
   NavDump(char* arg0);
   bool initialize(int argc, char* argv[]) throw();

protected:
   virtual void process();
      // additional setup will get filtering options from the user
   virtual void additionalSetup();

private:
   void printCurrentFilter();
   void getNewTime(DayTime& dt);
   void getFICBlocks();
   void getSVs();

   CommandOptionWithAnyArg inputFileOption;
   CommandOptionWithAnyArg outputFileOption;
      /// command option to use default values (i.e. no prompting)
   CommandOptionNoArg defaultsOption;
      /// command option for start time for matching
   CommandOptionWithSimpleTimeArg timeOption;
      /// command option for end time for matching
   CommandOptionWithSimpleTimeArg eTimeOption;
      /// meta-option for grouping time options
   CommandOptionGroupAnd seTimeOptions;
      /// command option for PRNs
   CommandOptionWithNumberArg prnOption;
      /// command option for FIC blocks
   CommandOptionWithNumberArg blockOption;
      /// command option for using a RINEX navigation message file (vice FIC)
   CommandOptionNoArg rinexOption;

   DayTime startTime, endTime;
   list<long> prnFilterList;
   list<long> blockFilterList;
   
   bool isRinexInput;
};

NavDump::NavDump(char* arg0)
      : BasicFramework(arg0, "Prints the contents of an FIC or RINEX file into a human readable file and allows filtering of the data."),
        inputFileOption('i',
                        "input",
                        "Name of an input navigation message file",
                        true),
        outputFileOption('o',
                         "output",
                         "Name of an output file",
                         true),
        timeOption('t', "time", "Start time (of data) for processing"),
        eTimeOption('e', "end-time", "End time (of data) for processing"),
        prnOption('p', "prn", "PRN(s) to include"),
        blockOption('b', "block", "FIC block number(s) to process ((9)109"
                    " (Engineering) ephemerides, (62)162 (engineering)"
                    " almanacs)"),
        defaultsOption('a', "all-records", "Unless otherwise specified, use"
                       " default values for record filtration"),
        rinexOption('r',"RINEX", "Assume input file is a RINEX navigation"
                         " message file"),
        startTime(0,0.0),
        endTime(DayTime::END_OF_TIME)
{
   seTimeOptions.addOption(&timeOption);
   seTimeOptions.addOption(&eTimeOption);   
   inputFileOption.setMaxCount(1);
   outputFileOption.setMaxCount(1);
   timeOption.setMaxCount(1);
   eTimeOption.setMaxCount(1);
   isRinexInput = false;
}

void NavDump::printCurrentFilter()
{
   cout << "Current filtering options:" << endl
        << "\tStart time:\t" << startTime << endl
        << "\tEnd time:\t" << endTime << endl
        << "\tPRNs:\t\t";
   if (prnFilterList.empty())
      cout << "using all PRNs";
   else
      copy(prnFilterList.begin(), prnFilterList.end(),
           ostream_iterator<long>(cout, " "));
   if (!isRinexInput)
   {
      cout << endl
           << "\tFIC blocks:\t";
      if (blockFilterList.empty())
         cout << "using all blocks";
      else
         copy(blockFilterList.begin(), blockFilterList.end(),
              ostream_iterator<long>(cout, " "));
   }
   cout << endl;
}

bool NavDump::initialize(int argc, char* argv[]) throw()
{
   using gpstk::StringUtils::asInt;

   if (!BasicFramework::initialize(argc, argv))
      return false;

   if (prnOption.getCount())
   {
      for (int i = 0; i < prnOption.getCount(); i++)
         prnFilterList.push_back(asInt(prnOption.getValue()[i]));
   }

   if (blockOption.getCount())
   {
      for (int i = 0; i < blockOption.getCount(); i++)
         blockFilterList.push_back(asInt(blockOption.getValue()[i]));
   }

   if (timeOption.getCount())
      startTime = timeOption.getTime()[0];
   if (eTimeOption.getCount())
      endTime = eTimeOption.getTime()[0];

   if (rinexOption.getCount())
      isRinexInput = true;
   
   return true;
}

void NavDump::additionalSetup()
{
   int option = 0;
   string line;

   if (defaultsOption.getCount() ||
       (seTimeOptions.getCount() && blockOption.getCount() &&
        prnOption.getCount()))
      return;

   while (option != 5)
   {
      cout << endl;
      printCurrentFilter();

      cout << endl
           << "Choose an option by number then push enter:" << endl
           << "\t1) Change the start time" << endl
           << "\t2) Change the end time" << endl
           << "\t3) Select specific PRNs" << endl;
      if (!isRinexInput)
      {
      cout << "\t4) Select specific FIC block numbers" << endl;
      }
      cout << "\t5) Process the file" << endl
           << "use ctrl-c to exit" << endl
           << "? ";
      
      getline(cin, line);
      istringstream optionstr(line);
      optionstr >> option;
      cout << endl;

      switch(option)
      {
         case 1:
            cout << "Entering a new start time..." << endl;
            getNewTime(startTime);
            option = 0;
            break;
         case 2:
            cout << "Entering a new end time..." << endl;
            getNewTime(endTime);
            option = 0;
            break;
         case 3:
            getSVs();
            option = 0;
            break;
         case 4:
            if (!isRinexInput) getFICBlocks();
            option = 0;
            break;
         case 5:
            break;
         default:
            cout << '\"' << line << "\" is an invalid option" << endl;
            option = 0;
            break;
      }

      if (startTime > endTime)
         cout << endl
              << "Please check the start and end times because all the data will be filtered" << endl
              << "with this setting (startTime > endTime)." << endl;
   }

   cout << "processing..." << endl;
}

void NavDump::getFICBlocks()
{
   int block;
   string line;
   cout << "Enter a list of FIC blocks to search for separated by spaces." << endl
        << "The old list will be discarded." << endl
        << "   9 : Ephemeris - engineering units." << endl
        << " 109 : Ephemeris - as broadcast." << endl
        << "  62 : Almanac - engineering units." << endl
        << " 162 : Almanac - as broadcast." << endl
        << "Enter '0' for all blocks - any other blocks entered will be ignored." << endl
        << "? ";
   getline(cin, line);
   istringstream is(line);
   while (is >> block)
      if (block == 0)
      {
         blockFilterList.clear();
         return;
      }
      else 
         switch(block)
         {
            case 9:
            case 109:
            case 62:
            case 162:
               blockFilterList.push_back(block);
               break;
            default:
               break;
         }
}

void NavDump::getSVs()
{
   int prn;
   string line;
   cout << "Enter a list of PRNs separated by spaces to search for." << endl
        << "The old list will be discarded." << endl
        << "Enter '0' for all PRNs - any other PRNs listed will be ignored." << endl
        << "? ";
   getline(cin, line);
   istringstream is(line);
   while (is >> prn)
      if (prn == 0)
      {
         prnFilterList.clear();
         return;
      }
      else if ( (prn > 0) && (prn <= MAX_PRN) )
      {
         prnFilterList.push_back(prn);
      }
}

void NavDump::getNewTime(DayTime& dt)
{
   short week = -1;
   double SOW;
   string buf;
   short done = 0;
   
   while (!done)
   {
      cout << " Enter full GPS week: ";
      getline(cin, buf);
      istringstream instr(buf);
      instr >> week;
      if (week != -1)
         done = 1;
      else 
         cout << " Error entering week.  Please try again." << endl;
   }
   
      // Now reset flag and get SOW
   done = 0;
   while (!done)
   {
      cout << " Enter GPS seconds of week: ";
      getline(cin,buf);
      istringstream instr(buf);
      instr >> SOW;
      if ((SOW >= 0.0L) && (SOW < 604800.0L) )
         done = 1;
      else 
         cout << " Error entering SOW.  Please try again." << endl;
   }
   dt.setGPSfullweek(week, SOW);
}

void NavDump::process()
{
   
   ofstream out;
   out.open(outputFileOption.getValue()[0].c_str());
   if (out.fail())
   {
      cout << "Opening output file " << outputFileOption.getValue()[0] 
           << " failed." << endl
           << "   navdump is ending..." << endl
           << endl;
      return;
   }

   if (!isRinexInput)
   {
         // filter the data...  first by block number, then by PRN
      FileFilterFrame<FICStream, FICData> data(inputFileOption.getValue()[0]);
      if (!blockFilterList.empty())
         data.filter(FICDataFilterBlock(blockFilterList));
      if (!prnFilterList.empty())
         data.filter(FICDataFilterPRN(prnFilterList));
      
      list<FICData>& ficlist = data.getData();
      list<FICData>::iterator itr = ficlist.begin();
      while (itr != ficlist.end())
      {
         (*itr).prettyDump(out);
         itr++;
      }
   }
   else     // Rinex navigation message data
   {
      FileFilterFrame<RinexNavStream, RinexNavData> 
                             data(inputFileOption.getValue()[0]);
      if (!prnFilterList.empty())
         data.filter(RinexNavDataFilterPRN(prnFilterList));

      list<RinexNavData>& rnavlist = data.getData();
      list<RinexNavData>::iterator itr = rnavlist.begin();
      while (itr!=rnavlist.end())
      {
         RinexNavData& r = *itr;
         EngEphemeris ee(r);
         ee.dump(out);
         itr++;
      }
   }
}

int main(int argc, char* argv[])
{
   try
   {
      NavDump nd(argv[0]);
      if (!nd.initialize(argc, argv))
         return 0;
      if (!nd.run())
         return 1;
      
      return 0;
   }
   catch(Exception& e)
   {
      cout << e << endl;
   }
   catch(std::exception& e)
   {
      cout << "Caught std::exception " << e.what() << endl;
   }
   catch(...)
   {
      cout << "Caught unknown exception" << endl;
   }

   return 1;
}

