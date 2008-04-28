#pragma ident "$Id$"

/**
 * @file navsum.cpp - print a human readable file from a binary FIC file
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
#include "DayTime.hpp"

#include "gps_constants.hpp"

using namespace std;
using namespace gpstk;

class NavSum : public BasicFramework
{
public:
   NavSum(char* arg0);
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
   gpstk::DayTime buildXMitTime(const uint32_t word2, const int week );
   void printSummary( ofstream& out );

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

   DayTime startTime, endTime;
   list<long> prnFilterList;
   list<long> blockFilterList;
   
      // counters
   int totalsByBlock[4];
   static const int BLK9;
   static const int BLK109;
   static const int BLK62;
   static const int BLK162;
   static const string blockStr[4];
   int totalsByPRN[gpstk::MAX_PRN+1][2];
   int totalsBySVID[64][2];

};

const int NavSum::BLK9 = 0;
const int NavSum::BLK109 = 1;
const int NavSum::BLK62 = 2;
const int NavSum::BLK162 = 3;
const string NavSum::blockStr[]   = {"  9", "109", " 62", "162"};

NavSum::NavSum(char* arg0)
      : BasicFramework(arg0, "Lists the block contents of a FIC file and prints"
                        " summary count information."),
        inputFileOption('i', "input", "Name of an input FIC file", true),
        outputFileOption('o', "output", "Name of an output file", true),
        timeOption('t', "time", "Start time (of data) for processing"),
        eTimeOption('e', "end-time", "End time (of data) for processing"),
        prnOption('p', "prn", "PRN(s) to include"),
        blockOption('b', "block", "FIC block number(s) to process ((9)109"
                    " (Engineering) ephemerides, (62)162 (engineering)"
                    " almanacs)"),
        defaultsOption('a', "all-records", "Unless otherwise specified, use"
                       " default values for record filtration"),
        startTime(0,0.0),
        endTime(DayTime::END_OF_TIME)
{
   seTimeOptions.addOption(&timeOption);
   seTimeOptions.addOption(&eTimeOption);   
   inputFileOption.setMaxCount(1);
   outputFileOption.setMaxCount(1);
   timeOption.setMaxCount(1);
   eTimeOption.setMaxCount(1);

   int cnt;
   for (cnt=0;cnt<4;++cnt)
   {
      totalsByBlock[cnt] = 0;
   }
   for (cnt=0;cnt<gpstk::MAX_PRN+1;++cnt)
   {
      totalsByPRN[cnt][BLK9] = 0;
      totalsByPRN[cnt][BLK109] = 0;
   }
   for (cnt=0;cnt<64;++cnt)
   {
      totalsBySVID[cnt][BLK62] = 0;
      totalsBySVID[cnt][BLK162] = 0;
   }
}

void NavSum::printCurrentFilter()
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
   cout << endl
        << "\tFIC blocks:\t";
   if (blockFilterList.empty())
      cout << "using all blocks";
   else
      copy(blockFilterList.begin(), blockFilterList.end(),
           ostream_iterator<long>(cout, " "));
   cout << endl;
}

bool NavSum::initialize(int argc, char* argv[]) throw()
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
   
   return true;
}

void NavSum::additionalSetup()
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
           << "\t3) Select specific PRNs" << endl
           << "\t4) Select specific FIC block numbers" << endl
           << "\t5) Process the file" << endl
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
            getFICBlocks();
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
              << "Please check the start and end times because all the data "
              << "will be filtered" << endl
              << "with this setting (startTime > endTime)." << endl;
   }

   cout << "processing..." << endl;
}

void NavSum::getFICBlocks()
{
   int block;
   string line;
   cout << "Enter a list of FIC blocks to search for separated by spaces.\n" 
        << "The old list will be discarded." << endl
        << "   9 : Ephemeris - engineering units." << endl
        << " 109 : Ephemeris - as broadcast." << endl
        << "  62 : Almanac - engineering units." << endl
        << " 162 : Almanac - as broadcast." << endl
        << "Enter '0' for all blocks - any other blocks entered will be "
        << "ignored.\n ? ";
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

void NavSum::getSVs()
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

void NavSum::getNewTime(DayTime& dt)
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

void NavSum::process()
{
   try
   {
   ofstream out;
   out.open(outputFileOption.getValue()[0].c_str());
   if (out.fail())
   {
      cout << "Opening output file " << outputFileOption.getValue()[0] 
           << " failed." << endl
           << "   navsum is ending..." << endl
           << endl;
      return;
   }

      // filter the data...  first by block number, then by PRN
   FileFilterFrame<FICStream, FICData> data(inputFileOption.getValue()[0]);
   if (!blockFilterList.empty())
      data.filter(FICDataFilterBlock(blockFilterList));
   if (!prnFilterList.empty())
      data.filter(FICDataFilterPRN(prnFilterList));
      
   out << "Block#       PRN or                Transmit            !        Toe/Toa" << endl;
   out << "in set Type   SVID   mm/dd/yy DOY hh:mm:ss Week    SOW ! mm/dd/yy DOY HH:MM:SS" << endl;
   std::string xmitFmt("%02m/%02d/%02y %03j %02H:%02M:%02S %4F %6.0g");
   std::string epochFmt("%02m/%02d/%02y %03j %02H:%02M:%02S");
   DayTime XMitT;
   DayTime EpochT;
   uint32_t temp;
   int PRNID; 
   int xmitPRN;
   int xMitWeek;
   int EpochWeek;
   char line[100];
   string linestr;
      
   int count = 0;
   list<FICData>& ficlist = data.getData();
   list<FICData>::iterator itr = ficlist.begin();
   while (itr != ficlist.end())
   {
		FICData& r = *itr;
      count++;
      int blockType = r.blockNum;
      double diff = 0.0;
      double Toe = 0.0;
      double HOW = 0.0;
      double xMitSOW = 0.0;
      double iMitSOW = 0;
      long IODC = 0;
      int fit = 0;
      switch (blockType)
      {
			case 9:
				PRNID = (short) r.f[19];
				HOW = r.f[2];
				Toe = r.f[33];
				xMitWeek = (int) r.f[5];
				IODC = ((long) r.f[9]) / 2048;
				fit = (int) r.f[34];
				EpochWeek = xMitWeek;
				diff = Toe - HOW;
				if (diff < -1.0 * (double) DayTime::HALFWEEK) EpochWeek++;
				if (diff > (double) DayTime::HALFWEEK) xMitWeek--;
				XMitT = DayTime( xMitWeek, HOW-6.0 );
				EpochT = DayTime( EpochWeek, Toe );
				sprintf(line," %5d  %3d    %02d    %s ! %s 0x%03X %1d",
					count,blockType,PRNID,
					XMitT.printf(xmitFmt).c_str(),
					EpochT.printf(epochFmt).c_str(),
					IODC,
					fit);
				linestr = string(line);
				out << linestr << endl;
				totalsByBlock[BLK9]++;
				totalsByPRN[PRNID][BLK9]++;
				break;
               
			case 109:
				PRNID = (int) r.i[1];
				xMitWeek = (int) r.i[0];
				temp = (uint32_t) r.i[3];
				XMitT = buildXMitTime( temp, xMitWeek );
				sprintf(line," %5d  %3d    %02d    %s !",
					count,blockType,PRNID,
					XMitT.printf(xmitFmt).c_str() );
				linestr = string(line);
				out << linestr << endl;
				totalsByBlock[BLK109]++;
				totalsByPRN[PRNID][BLK109]++;
				break;
               
			case 62:
				PRNID = r.i[3];
				xMitWeek = (int) r.i[5];
				EpochWeek = (int) r.i[0];
				iMitSOW = r.i[1];
				if (iMitSOW<0)
				{
					iMitSOW += gpstk::DayTime::FULLWEEK;
					xMitWeek--;
				}
				xMitSOW = (double) iMitSOW;
				XMitT = DayTime( xMitWeek, xMitSOW );
				if (PRNID>0 && PRNID<33)
				{
					EpochT = DayTime( EpochWeek, r.f[8] );
					sprintf(line," %5d  %3d    %02d    %s ! %s",
						count,blockType,PRNID,
						XMitT.printf(xmitFmt).c_str(),
						EpochT.printf(epochFmt).c_str() );
				}
				else
				{
					sprintf(line," %5d  %3d    %02d    %s !",
						count,blockType,PRNID,
						XMitT.printf(xmitFmt).c_str() );
				}
				linestr = string(line);
				out << linestr << endl;
				totalsByBlock[BLK62]++;
				totalsBySVID[PRNID][BLK62]++;
				break;
               
			case 162:
				PRNID = r.i[0];
				xMitWeek = r.i[14];
				EpochWeek = r.i[13];
				temp = (uint32_t) r.i[2];
				xmitPRN = r.i[11];
				XMitT = buildXMitTime( temp, xMitWeek );
				sprintf(line," %5d  %3d    %02d    %s !                        %02d",
					count,blockType,PRNID,
					XMitT.printf(xmitFmt).c_str(),
					xmitPRN);
				linestr = string(line);
				out << linestr << endl;
				totalsByBlock[BLK162]++;
				totalsBySVID[PRNID][BLK162]++;
				break;
		}
         
		itr++;
   }
   printSummary( out );
   }
   catch (Exception& exc)
   {
      cerr << exc;
      exit(1);
   }
   catch (...)
   {
      cerr << "Caught unknown exception" << endl;
      exit(1);
   }
}

void NavSum::printSummary( ofstream& out )
{
   out << endl << endl;
   out << "Summary of data processed" << endl;
   out << "Block Type Summary" << endl;
   out << "Type   # Blocks Found" << endl;
   
   int n;
   char line[100];
   std::string linestr;
   std::string fmt1 = " %3s         %6d";
   for (n=0;n<4;++n)
   {
      sprintf(line, fmt1.c_str() , blockStr[n].c_str(), totalsByBlock[n]);
      linestr = string(line);
      out << linestr << endl;
   }
   
   out << endl << "Ephemeris Blocks by PRN" << endl;
   out << "PRN Block      Num" << endl;
   for (n=1;n<(gpstk::MAX_PRN+1);++n)
   {
      sprintf( line," %02d     9       %4d\n %02d   109       %4d",
         n, totalsByPRN[n][0], n, totalsByPRN[n][1]);
      linestr = string(line);
      out << linestr << endl;
   }
   
   out << endl << "Almanac Blocks by SVID" << endl;
   out << "SVID Block      Num" << endl;
   for (n=1;n<64;++n)
   {
      if (n>32 && n<51) continue;
      sprintf( line,"  %02d    62       %4d\n  %02d   162       %4d",
         n,totalsBySVID[n][0],n,totalsBySVID[n][1]);
      linestr = string(line);
      out << linestr << endl;
   }
}

gpstk::DayTime NavSum::buildXMitTime(const uint32_t word2, const int week )
{
   int useweek = week;
   uint32_t temp = word2;
   temp >>= 13;    // 32 - 2 - 17 = 32 - 19 = 13;
   long SOW = (long) (temp * 6) - 6;
   if (SOW<0) 
   {
      SOW += gpstk::DayTime::FULLWEEK;
      useweek--;
   }
   double XmitSOW = (double) SOW;
   return ( DayTime( useweek, XmitSOW) ); 
}


int main(int argc, char* argv[])
{
   try
   {
      NavSum nd(argv[0]);
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

