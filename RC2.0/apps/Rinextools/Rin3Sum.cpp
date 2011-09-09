#pragma ident "$Id: RinSum.cpp 1461 2008-11-18 19:46:55Z ocibu $"

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
 * @file RinSum.cpp
 * Read and summarize Rinex observation files, optionally fill header in-place.
 */

#define TEST(EXPRESSION) cerr << #EXPRESSION " == " << EXPRESSION << endl;
//#define DEBUG_PRINT

#include <cstring>

#include "MathBase.hpp"

#include "Rinex3ObsBase.hpp"
#include "Rinex3ObsData.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsStream.hpp"
#include "Rinex3NavBase.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3NavStream.hpp"
#include "TimeString.hpp"
#include "CommonTime.hpp"
#include "TimeString.hpp"
#include "CivilTime.hpp"
#include "GPSWeekSecond.hpp"

#include "SatID.hpp"
#include "RinexSatID.hpp"
#include "RinexObsID.hpp"

#include "CommandOptionParser.hpp"
#include "CommandOption.hpp"
#include "CommandOptionWithTimeArg.hpp"

#include "GNSSconstants.hpp"
#include "RinexUtilities.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <time.h>

using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
string version("3.0 13/8/09");

// data input from command line
vector<string> InputFiles;
string InputDirectory;
string OutputFile;
ostream* pout;
CommonTime BegTime, EndTime;
bool ReplaceHeader=false;
bool TimeSortTable=false;
bool GPSTimeOutput=false;
bool debug=false;
bool brief=false;

//------------------------------------------------------------------------------------
// data used for computation
const int SEC_PER_MIN = 60;
const int SEC_PER_HOUR = 3600;

const int ndtmax=15;
double dt,bestdt[ndtmax];
int ndt[ndtmax]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int nepochs,ncommentblocks;

//------------------------------------------------------------------------------------
// class used to store SAT/Obs table
//Struct instead?
struct TableData
{
   RinexSatID sat;
   vector<int> nobs;
   
   double prevC1;
   double prevP1;
   double prevL1;
   
   CommonTime begin;
   CommonTime end;
   
   TableData(const SatID& p, const int& n)
   {
      sat = RinexSatID(p);
      nobs = vector<int>(n);
      prevC1 = 0;
      prevP1 = 0;
      prevL1 = 0;
   }
      // needed for find()
   inline bool operator==(const TableData& d)
   {
      return d.sat == sat;
   }
};
   // for sort()
class TableSATLessThan 
{      
   public:
   bool operator()(const TableData& d1, const TableData& d2)
   {
      return d1.sat < d2.sat;
   }
};
class TableBegLessThan 
{
   public:
   bool operator()(const TableData& d1, const TableData& d2)
   {
      return d1.begin < d2.begin;
   }
};

struct SatSystem
{
   string name;
   
   int indexC1C;
   int indexC2C;
   int indexC1P;
   int indexC2P;
   
   int indexL1C;
   int indexL1P;
   int indexL2C;
   int indexL2P;
   
   SatSystem(string nm = "")
      : indexC1C(-1), indexC2C(-1), indexC1P(-1), indexC2P(-1), indexL1C(-1),
         indexL2C(-1), indexL1P(-1), indexL2P(-1)
   {
      name = nm;
   }
};

//------------------------------------------------------------------------------------
// prototypes
int GetCommandLine(int argc, char **argv) throw(Exception);
void PreProcessArgs(const char *arg, vector<string>& Args) throw(Exception);

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
   try
   {
      int iret, i, j, k, n, ifile, nsats, nclkjumps, L1lli;
      double C1, L1, P1, clkjumpave, clkjumpvar;
      CommonTime lastObsTime, prevObsTime, firstObsTime;
      vector<CommonTime> clkjumpTimes;
      vector<double> clkjumpMillsecs, clkjumpUncertainty;
      vector<int> clkjumpAgree;
      map<char, SatSystem> systems;
      const string calfmt("%04Y/%02m/%02d %02H:%02M:%02S");
      const string gpsfmt("%4F %10.3g");
      const string longfmt("%04Y/%02m/%02d %02H:%02M:%02S = %4F %10.3g");

      BegTime = CommonTime::BEGINNING_OF_TIME;
      EndTime = CommonTime::END_OF_TIME;

         // Title and description
      string Title;
      Title = "RINSUM, part of the GPS ToolKit, Ver " + version + ", Run ";
      time_t timer;
      struct tm *tblock;
      timer = time(NULL);
      tblock = localtime(&timer);
      lastObsTime = CivilTime(1900+tblock->tm_year, 1+tblock->tm_mon, tblock->tm_mday,
                        tblock->tm_hour, tblock->tm_min, tblock->tm_sec);
      //Title += static_cast<CivilTime>(lastObsTime).printf(longfmt);
      Title += printTime(lastObsTime,calfmt);
      cout << Title << endl;

      iret=GetCommandLine(argc, argv);
      if(iret)
         return iret;

      iret = RegisterARLUTExtendedTypes();
      if(iret)
         return iret;

         // open the output file and write to it
      if(!OutputFile.empty())
      {
         pout = new ofstream(OutputFile.c_str(), ios::out);
         if(pout->fail())
         {
            cerr << "Could not open output file " << OutputFile << endl;
            pout = &cout;
         }
         else
         {
            pout->exceptions(ios::failbit);
            *pout << Title;
            cout << "Writing summary to file " << OutputFile << endl;
         }
      }
      else
         pout = &cout;
      
      if(InputFiles.size() > 1)
         sortRinexObsFiles(InputFiles);

         // now open the input files, read the headers and data
      Rinex3ObsHeader rheader;
      Rinex3ObsData robs;
      for(ifile=0; ifile<InputFiles.size(); ifile++)
      {
         string filename;
         if(!InputDirectory.empty())
         {
            filename = InputDirectory;
         }
         filename += InputFiles[ifile];
         Rinex3ObsStream InStream;
         InStream.open(filename.c_str(),ios::in);
         if(!InStream)
         {
            *pout << "File " << filename << " could not be opened.\n";
            continue;
         }
         InStream.exceptions(ios::failbit);
         //if(!isRinex3ObsFile(filename))
         //{
         //   *pout << "File " << filename << " is not a Rinex observation file\n";
         //   if(isRinex3NavFile(filename))
         //   {
         //      *pout << "This file is a Rinex navigation file - try NavMerge\n";
         //   }
         //   continue;
         //}

         prevObsTime = CommonTime::BEGINNING_OF_TIME;
         firstObsTime = CommonTime::BEGINNING_OF_TIME;

         if(!brief)
            *pout << "+++++++++++++ RinSum summary of Rinex obs file "
                  << filename << " +++++++++++++\n";
         else
            *pout << "\nFile name: " << filename << endl;
         
            // input header
         try
         {
            InStream >> rheader;
         }
         catch(gpstk::FFStreamError& e)
         {
            cerr << "Caught an FFStreamError while reading header: "
                 << e.getText(0) << endl;
         }
         catch(gpstk::Exception& e)
         {
            cerr << "Caught a gpstk exception while reading header: "
                 << e.getText(0) << endl;
         }

         if(!brief)
         {
            *pout << "Rinex header:\n";
            rheader.dump(*pout);
         }
         else
         *pout << "Position (XYZ,m) : " << fixed << setprecision(4)
               << rheader.antennaPosition << ".\n";

         if(!rheader.isValid())
         {
            *pout << "Abort: header is invalid\n";
            if(!brief)
            {
               *pout << "\n+++++++++++++ End of RinSum summary of "
                     << filename << " +++++++++++++\n";
            }
            continue;
         }

         //Rinex3ObsStream out(argv[2], ios::out);
         //out << rheader;

         nepochs = ncommentblocks = 0;
         n = 0;
         
         vector<TableData> table;
         map< char, vector<int> > totals;
         
         map<std::string,vector<RinexObsID> >::const_iterator iter;
         for( iter = rheader.mapObsTypes.begin(); iter != rheader.mapObsTypes.end(); ++iter)
         {
               //Set up the system flags
            SatSystem& currentSystem = systems[(iter->first)[0]];
               //Initialize the vectors contained in the map
            totals[(iter->first)[0]] = vector<int>((iter->second).size());
            
            #ifdef DEBUG_PRINT
               cout << "GNSS " << (iter->first) << " is present with "
                    << (iter->second).size() << " observations..." << endl;
            #endif
            
            if((iter->second).size() > n)
               n = (iter->second).size();
            
            for(int index = 0; index < (iter->second).size(); ++index)
            {
               #ifdef DEBUG_PRINT
                  cout << index << "/" << (iter->second).size() << endl;
               #endif
               std::string obsType((iter->second)[index].asString());
               if(obsType == "C1C")
                  currentSystem.indexC1C = i;
               else if(obsType == "C2C")
                  currentSystem.indexC2C = i;
               else if(obsType == "C1P")
                  currentSystem.indexC1P = i;
               else if(obsType == "C2P")
                  currentSystem.indexC2P = i;
               else if(obsType == "L1C")
                  currentSystem.indexL1C = i;
               else if(obsType == "L2C")
                  currentSystem.indexL2C = i;
               else if(obsType == "L1P")
                  currentSystem.indexL1P = i;
               else if(obsType == "L2P")
                  currentSystem.indexL2P = i;
            }
         }

         if(pout == &cout)
            *pout << "Reading the observation data..." << endl;

            // input obs
         while(InStream >> robs)
         {
            if(debug)
            {
               *pout << "Epoch: " << robs.time
                     << ", Flag " << robs.epochFlag
                     << ", Nsat " << robs.obs.size()
                     << ", clk " << fixed << robs.clockOffset << endl;
            }
               // is this a comment?
            if(robs.epochFlag > 1)
            {
               ncommentblocks++;
               //*pout << "inline header info:\n";
               //robs.auxHeader.dump(*pout);
               continue;
            }
            #ifdef DEBUG_PRINT
               cout << endl << endl << robs.time << endl;
            #endif
               // update first/last time seen and check time limits
            lastObsTime = robs.time;
            lastObsTime.setTimeSystem(TimeSystem::Any);
            if(lastObsTime < BegTime)
               continue;
            if(lastObsTime > EndTime)
               break;
            if(firstObsTime == CommonTime::BEGINNING_OF_TIME)
               firstObsTime=lastObsTime;
               //Count this epoch
            nepochs++;
               //Variables to keep track of # of sats and clock jumps
            nsats = nclkjumps = 0;
               //Statistics on the clock jumps
            clkjumpave = clkjumpvar = 0.0;
               //Start loop over satellites
            Rinex3ObsData::DataMap::const_iterator it;
            for(it = robs.obs.begin(); it != robs.obs.end(); ++it)
            {
               vector<TableData>::iterator ptab;
                  //Search for this satellite in the table.
                  //If it exists, good. If not, add it to the table.
                  //(it->first) is a SatID object
                  //(it->second) is a vector of RinexDatum objects
               ptab = find(table.begin(),table.end(),TableData(it->first,n));
               if(ptab == table.end())
               {
                     //Add this satellite to the table
                  table.push_back(TableData(it->first,n));
                  ptab = find(table.begin(),table.end(),TableData(it->first,n));
                  ptab->begin = lastObsTime;
               }
                  //Set the end time for this satellite to the current epoch
               ptab->end = lastObsTime;
               if(debug)
                  *pout << "Sat " << setw(2) << RinexSatID(it->first);

                  //Update Obs data totals
                  //First, find the current system...
               char sysCode = RinexSatID(it->first).systemChar();
               SatSystem& satSystem = (systems.find(sysCode))->second;
               C1 = P1 = L1 = 0;
               #ifdef DEBUG_PRINT
                  cout << "(it->second).size() == " << (it->second).size() << endl;
               #endif
               for(int index = 0; index != (it->second).size(); index++)
               {
                  #ifdef DEBUG_PRINT
                     cout << index;
                  #endif
                     //If this observation is not zero, update it's total count
                  if((it->second)[index].data != 0)
                  {
                     (ptab->nobs)[index]++;   //Per obs
                     #ifdef DEBUG_PRINT
                        cout << ":" << (it->second)[index].data;
                     #endif
                     totals[sysCode][index]++;   //Per system
                  }
                  #ifdef DEBUG_PRINT
                     cout << endl;
                  #endif
                     //Save L1 range and phase for clk jump test below
                     //Because this uses the speed of light, doesn't matter that
                     //it is a GPS constant.
                  if(index == satSystem.indexC1C)
                     C1 = (it->second)[index].data * 1000.0 / C_MPS;
                  if(index == satSystem.indexC1P)
                     P1 = (it->second)[index].data * 1000.0 / C_MPS;
                  if(index == satSystem.indexL1C)
                  {
                     L1 = (it->second)[index].data * 1000.0 / C_MPS;
                     L1lli = (it->second)[index].lli;
                  }
                  if(index == satSystem.indexL1P && L1 == 0)
                  {
                        //Only use this one if the other L1 hasn't been found
                        //Do I need this?
                     L1 = (it->second)[index].data * 1000.0 / C_MPS;
                     L1lli = (it->second)[index].lli;
                  }
                  //Shouldn't need the debug statement.
                  //if(debug) 
                  //*pout << " " << Rinex3ObsHeader::convertObsType(jt->first)
                  //      << " " << setw(13) << setprecision(3) << jt->second.data << " "
                  //      << jt->second.lli << " " << jt->second.ssi;
               }//End loop over observations
               if(debug)
                  *pout << endl;
                  //Test for millisecond clock adjusts -
                  //Sometimes they are applied to range but not phase or vice-versa
                  //I don't know what this section is really doing, so I am assuming
                  //leaving it alone is the best idea.
               if(prevObsTime != CommonTime::BEGINNING_OF_TIME && L1 != 0 && ptab->prevL1 != 0)
               {
                  int nms;
                  double test = 0.0;
                  nsats++;
                     //Should this really be using the GPS L1_WAVELENGTH for
                     //every system?
                  if(P1 != 0 && ptab->prevP1 != 0)
                     test = P1-L1_WAVELENGTH_GPS*L1
                        - (ptab->prevP1-L1_WAVELENGTH_GPS*ptab->prevL1);
                  else if(C1 != 0 && ptab->prevC1 != 0)
                     test = C1-L1_WAVELENGTH_GPS*L1
                        - (ptab->prevC1-L1_WAVELENGTH_GPS*ptab->prevL1);
                  if(fabs(test) > 0.5)
                  {
                        //Test must be > 150 km =~ 1/2 millisecond
                        //Is it nearly an even multiple of 1 millisecond?
                     //test *= 1000.0/C_MPS;  // leave sign on it
                     if(debug)
                        *pout << "possible clock jump: test = "
                                    << setprecision(9) << test;
                     nms = long(test + (test > 0 ? 0.5 : -0.5));
                     if(fabs(test - double(nms)) < 0.001)
                     {
                        if(debug)
                           *pout << " -> " << setprecision(9)
                                 << fabs(test - double(nms));
                        //Keep clkjumpave = sequential average nms, clkjumpvar=variance
                        //if(test < 0) nms *= -1;
                        nclkjumps++;
                        clkjumpave += (double(nms)-clkjumpave)/double(nclkjumps);
                        if(nclkjumps > 1)
                        {
                           clkjumpvar = (clkjumpvar*(nclkjumps-2)
                            + nclkjumps*(double(nms)-clkjumpave)
                            * (double(nms)-clkjumpave) / (nclkjumps-1))
                            / (nclkjumps-1);
                        }
                     }
                     else if(debug)
                        *pout << " - failed.";
                     if(debug && L1lli != 0)
                        *pout << " LLI is set";
                     if(debug)
                        *pout << " " << RinexSatID(it->first)
                              << " " << static_cast<GPSWeekSecond>(lastObsTime).printf("%4F %.3g") << endl;

                  }
               }
               // save C1,L1,P1 for this sat for next time
               ptab->prevC1 = C1;
               ptab->prevL1 = L1;
               ptab->prevP1 = P1;
               #ifdef DEBUG_PRINT
                  cout << endl;
               #endif
            }//End loop over satellites
            //out << robs;

               //If more than half the sats saw a clk jump, call it
            if(nclkjumps > nsats/2)
            {
               if(debug)
                  *pout << "test nclkjumps is " << nclkjumps
                        << " and nsats is " << nsats
                        << ", ave is " << fixed << setprecision(3) << clkjumpave
                        << " and stddev is " << setprecision(3) << sqrt(clkjumpvar)
                        << endl;
               clkjumpTimes.push_back(lastObsTime);
               clkjumpMillsecs.push_back(clkjumpave);
               clkjumpAgree.push_back(nsats-nclkjumps);
               clkjumpUncertainty.push_back(sqrt(clkjumpvar));
            }
            
            if(prevObsTime != CommonTime::BEGINNING_OF_TIME)
            {
               dt = lastObsTime-prevObsTime;
               if(dt > 0.0)
               {
                  for(i = 0; i < ndtmax; i++)
                  {
                     if(ndt[i] <= 0)
                     {
                        bestdt[i]=dt;
                        ndt[i]=1;
                        break;
                     }
                     if(fabs(dt-bestdt[i]) < 0.0001)
                     {
                        ndt[i]++;
                        break;
                     }
                     if(i == ndtmax-1)
                     {
                        k = 0;
                        int nleast = ndt[k];
                        for(j = 1; j < ndtmax; j++)
                        {
                           if(ndt[j] <= nleast)
                           {
                              k = j;
                              nleast = ndt[j];
                           }
                        }
                        ndt[k] = 1;
                        bestdt[k] = dt;
                     }
                  }
               }
               else
               {
                  cerr << " WARNING time tags out of order: "
                       << static_cast<GPSWeekSecond>(prevObsTime).printf("%F/%.0g = ")
                       << static_cast<CivilTime>(prevObsTime).printf("%04Y/%02m/%02d %02H:%02M:%02S")
                       << " > "
                       << static_cast<GPSWeekSecond>(lastObsTime).printf("%F/%.0g = ")
                       << static_cast<CivilTime>(lastObsTime).printf("%04Y/%02m/%02d %02H:%02M:%02S")
                       << endl;
               }
            }
            prevObsTime = lastObsTime;
         }//End loop over epochs in the file
            //Clean up the file handle
         InStream.close();
            //Check that we found some data
         if(nepochs <= 0)
         {
            *pout << "File " << filename << " : no data found. Are time limits wrong?\n";
            continue;
         }

            //Compute interval
         for(i=1,j=0; i < ndtmax; i++)
         {
            if(ndt[i] > ndt[j])
               j = i;
            dt = bestdt[j];
         }
            //Summary info
            //Computed interval 41400 seconds.
            //Computed first epoch: 1528 1800 = 2009/4/19 0:30:00
            //Computed last epoch: 1528 43200 = 2009/4/19 12:00:00
            //Computed time span: 11h 30m 0s = 41400 seconds
         *pout << "Computed interval "
               << fixed << setw(5) << setprecision(2) << dt << " seconds."
               << endl;
         *pout << "Computed first epoch: " << static_cast<GPSWeekSecond>(firstObsTime).printf("%4F %14.7g")
               << " = " << static_cast<CivilTime>(firstObsTime).printf("%04Y/%02m/%02d %02H:%02M:%010.7f")
               << endl;
         *pout << "Computed last  epoch: " << static_cast<GPSWeekSecond>(lastObsTime).printf("%4F %14.7g")
               << " = " << static_cast<CivilTime>(lastObsTime).printf("%04Y/%02m/%02d %02H:%02M:%010.7f")
               << endl;
         
         *pout << "Computed time span: ";
         
         double secs = lastObsTime - firstObsTime;
         int remainder = int(secs);
         CivilTime delta(firstObsTime);
         
         delta.day = remainder / SEC_PER_DAY;
         remainder %= SEC_PER_DAY;
         delta.hour = remainder / SEC_PER_HOUR;
         remainder %= SEC_PER_HOUR;
         delta.minute = remainder / SEC_PER_MIN;
         remainder %= SEC_PER_MIN;
         delta.second = remainder;
         
         
         if(delta.day > 0)
            *pout << delta.day << "d ";
         
         *pout << delta.hour << "h "
               << delta.minute << "m "
               << delta.second << "s = "
               << secs << " seconds." << endl;
         
            //Reusing secs, as it is equivalent to the original expression
            //i = 1+int(0.5+(lastObsTime-firstObsTime)/dt);
         i = 1+int(0.5 + secs / dt);
         
            //There were 1337 epochs ( 89.13% of 1500 possible epochs in this timespan) and 163 inline header blocks.
         if(!brief)
            *pout << "There were " << nepochs << " epochs ("
                  << setprecision(2) << double(nepochs*100)/i
                  << "% of " << i << " possible epochs in this timespan) and "
                  << ncommentblocks << " inline header blocks.\n";
         
            //Sort table
         sort(table.begin(),table.end(),TableSATLessThan());
         
         if(TimeSortTable)
            sort(table.begin(),table.end(),TableBegLessThan());
         
            //Output table
            //Header
         vector<TableData>::iterator tableIter;
         if(table.size() > 0)
            table.begin()->sat.setfill('0');
         
         if(!brief)
         {
            *pout << "\n          Summary of data available in this file: "
                  << "(Totals are based on times and interval)\n";
               //Print the obs types for each system
            map<std::string,vector<RinexObsID> >::const_iterator sysIter;
            sysIter = rheader.mapObsTypes.begin();
            
            for(sysIter; sysIter != rheader.mapObsTypes.end(); ++sysIter)
            {
               RinexSatID sat(sysIter->first);
               *pout << "System " << (sysIter->first)
                  << " = " << sat.systemString() << ":" << endl;
               *pout << "Sat OT:";
               for(k = 0; k < (sysIter->second).size(); k++)
               {
                     //Prints the RINEX 3 code out
                  *pout << setw(7) << (sysIter->second)[k].asString();
               }
               *pout << "   Total             Begin time - End time" << endl;
                  //Print out every satellite with the same system code.
               for(tableIter = table.begin(); tableIter != table.end(); ++tableIter)
               {
                  std::string sysChar;
                  sysChar += (tableIter->sat).systemChar();
                  if((sysIter->first) == sysChar)
                  {
                     *pout << "Sat " << tableIter->sat << " ";
                     int obsSize = (rheader.mapObsTypes.find(sysChar)->second).size();
                     for(k = 0; k < obsSize; k++)
                        *pout << setw(7) << tableIter->nobs[k];
                     
                     *pout << setw(7) << 1+int(0.5+(tableIter->end-tableIter->begin)/dt);
                     
                     if(GPSTimeOutput)
                     {
                        *pout << "  " << static_cast<GPSWeekSecond>(tableIter->begin).printf("%4F %10.3g")
                              << " - " << static_cast<GPSWeekSecond>(tableIter->end).printf("%4F %10.3g") << endl;
                     }
                     else
                     {
                        *pout << "  " << static_cast<CivilTime>(tableIter->begin).printf("%04Y/%02m/%02d %02H:%02M:%04.1f")
                              << " - " << static_cast<CivilTime>(tableIter->end).printf("%04Y/%02m/%02d %02H:%02M:%04.1f")
                              << endl;
                     }
                  }
               }
               
               *pout << "TOTAL   ";
               map<char, vector<int> >::const_iterator totalsIter;
               totalsIter = totals.find((sysIter->first)[0]);
               
               const vector<int>& vec = totalsIter->second;
               for(k = 0; k < vec.size(); k++)
               {
                  *pout << setw(7) << vec[k];
               }
               *pout << endl << endl;
            }
            
            /**pout << "Sat  OT:";
            *pout << "  Total             Begin time - End time\n";
               //Loop over the table
            for(tableIter = table.begin(); tableIter != table.end(); ++tableIter)
            {
               *pout << "Sat " << tableIter->sat << " ";
               std::string sysChar;
               sysChar += (tableIter->sat).systemChar();
               int obsSize = (rheader.mapObsTypes.find(sysChar)->second).size();
               for(k = 0; k < obsSize; k++)
                  *pout << setw(7) << tableIter->nobs[k];
               
                  //Compute total based on times
               *pout << setw(7) << 1+int(0.5+(tableIter->end-tableIter->begin)/dt);
               
               if(GPSTimeOutput)
               {
                  *pout << "  " << static_cast<GPSWeekSecond>(tableIter->begin).printf("%4F %10.3g")
                        << " - " << static_cast<GPSWeekSecond>(tableIter->end).printf("%4F %10.3g") << endl;
               }
               else
               {
                  *pout << "  " << static_cast<CivilTime>(tableIter->begin).printf("%04Y/%02m/%02d %02H:%02M:%04.1f")
                        << " - " << static_cast<CivilTime>(tableIter->end).printf("%04Y/%02m/%02d %02H:%02M:%04.1f")
                        << endl;
               }
            }
            *pout << "TOTAL   ";
            map<char, vector<int> >::const_iterator totalsIter = totals.begin();
            for(totalsIter; totalsIter != totals.end(); ++totalsIter)
            {
               *pout << "System " << (totalsIter->first) << ":" << endl;
               const vector<int>& vec = totalsIter->second;
               for(k = 0; k < vec.size(); k++)
               {
                  *pout << setw(7) << vec[k];
               }
               *pout << endl;
            }*/
         }
         else
         {
               //Print out the satellites in this file...
               //SATs(2):
               // G01
               // R01
            *pout << "SATs(" << table.size() << "):";
            for(tableIter = table.begin(); tableIter != table.end(); ++tableIter)
               *pout << " " << tableIter->sat;
            *pout << endl;
            
               //Print out the obs types for the different systems...
               //System G Obs Types(4):
               // C1C C2C L1
            map<std::string,vector<RinexObsID> >::const_iterator sysIter;
            sysIter = rheader.mapObsTypes.begin();
            for(sysIter; sysIter != rheader.mapObsTypes.end(); ++sysIter)
            {
               string sysCode = (sysIter->first);
               vector<RinexObsID>& vec = rheader.mapObsTypes[sysCode];
               *pout << "System " << sysCode << " Obs types(" << vec.size()
                     << "): ";
               
               for(i = 0; i < vec.size(); i++)
               {
                  //*pout << " " << rheader.obsTypeList[i].type;
                  *pout << " " << vec[i].asString();
               }
               *pout << endl;
            }
         }
         
            //Warnings
         if((rheader.valid & Rinex3ObsHeader::validInterval)
               && fabs(dt-rheader.interval) > 1.e-3)
            *pout << " WARNING: Computed interval is " << setprecision(2)
                  << dt << " sec, while input header has " << setprecision(2)
                  << rheader.interval << " sec.\n";
         if(fabs(firstObsTime-rheader.firstObs) > 1.e-8)
            *pout << " WARNING: Computed first time does not agree with header\n";
         if((rheader.valid & Rinex3ObsHeader::validLastTime)
               && fabs(lastObsTime-rheader.lastObs) > 1.e-8)
            *pout << " WARNING: Computed last time does not agree with header\n";
         
         if(clkjumpTimes.size() > 0)
         {
            *pout << " WARNING: millisecond clock adjusts at these times:\n";
            for(i=0; i<clkjumpTimes.size(); i++)
            {
               *pout << "   "
                     << static_cast<GPSWeekSecond>(clkjumpTimes[i]).printf("%4F %10.3g = ")
                     << static_cast<CivilTime>(clkjumpTimes[i]).printf("%04Y/%02m/%02d %02H:%02M:%06.3f")
                     << " " << setw(5) << setprecision(2) << clkjumpMillsecs[i]
                     << " ms_clock_adjust";
                if(clkjumpAgree[i] > 0 || clkjumpUncertainty[i] > 0.01)
                  *pout << " (low quality determination; data may be irredeemable)";
               *pout << endl;
            }
         }
            // look for 'empty' obs types
         
         map<char, vector<int> >::const_iterator totalsIter = totals.begin();
         for(totalsIter; totalsIter != totals.end(); ++totalsIter)
         {
            std::string sysCode;
            sysCode += totalsIter->first;
            const vector<int>& vec = totalsIter->second;
            for(k = 0; k < vec.size(); k++)
            {
               if(vec[k] <= 0)
                  *pout << " WARNING: ObsType "
                        //<< rheader.mapObsTypes[sysCode][k].type
                        << rheader.mapObsTypes[sysCode][k].asString()
                        << " for system " << sysCode
                        << " should be deleted from header.\n";
            }
         }
         
            ///Not sure how to generate a RINEX 3 obs file....
            ///Will this work without any modifications beyond version info?
         if(ReplaceHeader)
         {
               // modify the header
               ///CHECK RINEX VERSION
            rheader.version = 3.0;
            rheader.valid |= Rinex3ObsHeader::validVersion;
            
            rheader.interval = dt;
            rheader.valid |= Rinex3ObsHeader::validInterval;
            
            rheader.lastObs = lastObsTime;
            rheader.valid |= Rinex3ObsHeader::validLastTime;
            
               // now the table
            rheader.numSVs = table.size();
            rheader.valid |= Rinex3ObsHeader::validNumSats;
            
            rheader.numObsForSat.clear();
            
            for(tableIter=table.begin(); tableIter!=table.end(); ++tableIter)
            {
                  //tableIter defined above
               rheader.numObsForSat.insert(
                  map<SatID, vector<int> >::value_type(tableIter->sat,tableIter->nobs) );
            }
            rheader.valid |= Rinex3ObsHeader::validPrnObs;
            //*pout << "\nNew header\n";
            //rheader.dump(*pout);

               // now re-open the file and replace the header
#ifdef _MSC_VER
            char newname[L_tmpnam];
            if(!tmpnam(newname))
            {
               cerr << "Could not create temporary file name - abort\n";
               return -1;
            }
#else
            char newname[]="RinSumTemp.XXXXXX";
            if(mkstemp(newname)==-1)
            {
               cerr << "Could not create temporary file name - abort\n";
               return -1;
            }
#endif
            remove(newname);

            Rinex3ObsHeader rhjunk;
            Rinex3ObsStream ROutStr(newname, ios::out);
            Rinex3ObsStream InAgain(filename.c_str());
            InAgain.exceptions(ios::failbit);

            InAgain >> rhjunk;
            ROutStr << rheader;
            while(InAgain >> robs)
            {
               lastObsTime = robs.time;
               if(lastObsTime < BegTime)
                  continue;
               if(lastObsTime > EndTime)
                  break;
               ROutStr << robs;
            }
            InAgain.close();
            ROutStr.close();
               // delete original file and rename the temporary
            iret = remove(filename.c_str());
            if(iret)
               *pout << "RinSum: Error: Could not remove existing file: "
                     << filename << endl;
            else {
               iret = rename(newname,filename.c_str());
               if(iret)
               {
                  *pout << "RinSum: Error: Could not rename new file " << newname
                        << " using old name " << filename << endl;
               }
               else
               {
                  *pout << "\nRinSum: Replaced original header with complete one,"
                        << " using temporary file name "
                        << newname << endl;
               }
            }
         }

         if(!brief)
            *pout << "\n+++++++++++++ End of RinSum summary of " << filename
                  << " +++++++++++++\n";
      }

      if(pout != &cout)
      {
         ((ofstream *)pout)->close();
         delete pout;
      }

      return 0;
   }
   catch(gpstk::FFStreamError& e)
   {
      cerr << "FFStreamError: " << e;
   }
   catch(gpstk::Exception& e)
   {
      cerr << "Exception: " << e;
   }
   catch (...)
   {
      cerr << "Unknown exception.  Abort." << endl;
   }
   return 1;
}   // end main()

//------------------------------------------------------------------------------------
int GetCommandLine(int argc, char **argv) throw(Exception)
{
   try
   {
      bool help = false;
      int j;
         // required options
      
         // optional
      CommandOption dashi(CommandOption::hasArgument, CommandOption::stdType,
         'i',"input"," [-i|--input] <file>  Input RINEX observation file name(s)");
      //dashi.setMaxCount(1);
      
         // optional options
         // this only so it will show up in help page...
      CommandOption dashf(CommandOption::hasArgument, CommandOption::stdType,
         'f',""," [-f|--file] <file>   file containing more options");
      
      CommandOption dasho(CommandOption::hasArgument, CommandOption::stdType,
         'o',"output"," [-o|--output] <file> Output the summary to a file named <file>");
      dasho.setMaxCount(1);
      
      CommandOption dashp(CommandOption::hasArgument, CommandOption::stdType,
         'p',"path"," [-p|--path] <path>   Find the input file(s) in this directory");
      dashp.setMaxCount(1);
      
      CommandOptionNoArg dashr('R', "Replace",
         " [-R|--Replace]       Replace input file header with a full one, in place.");
      dashr.setMaxCount(1);
      
      CommandOptionNoArg dashs('s', "sort",
         " [-s|--sort]          Sort the SAT/Obs table on begin time.");
      
      CommandOptionNoArg dashg('g', "gps",
         " [-g|--gps]           Print times in the SAT/Obs table as GPS times.");
      
      // time
      // times - don't use CommandOptionWithTimeArg
      CommandOption dashbt(CommandOption::hasArgument, CommandOption::stdType,
         0,"start", " --start <time>       Start time: <time> is 'GPSweek,sow' OR "
         "'YYYY,MM,DD,HH,Min,Sec'");
      dashbt.setMaxCount(1);
      
      CommandOption dashet(CommandOption::hasArgument, CommandOption::stdType,
         0,"stop", " --stop <time>        Stop time: <time> is 'GPSweek,sow' OR "
         "'YYYY,MM,DD,HH,Min,Sec'");
      dashet.setMaxCount(1);
      
      CommandOptionNoArg dashb('b', "brief",
         " [-b|--brief]         produce a brief (6-line) summary.");
      
      // help and debug
      CommandOptionNoArg dashh('h', "help",
         " [-h|--help]          print this help page and quit.");
      CommandOptionNoArg dashd('d', "debug",
         " [-d|--debug]         print debugging info.");
      
      // ... other options
      CommandOptionRest Rest("<filename(s)>");
      
      CommandOptionParser Par(
         "Prgm RINSUM reads a Rinex file and summarizes it content.\n"
         " It can optionally fill the header of the input file.\n"
         " [either <filenames> or --input required; put <filenames> after options].\n"
         );
      
      // allow user to put all options in a file
      // could also scan for debug here
      vector<string> Args;
      for(j=1; j<argc; j++)
      {
         PreProcessArgs(argv[j],Args);
      }
      if(Args.size()==0)
         Args.push_back(string("-h"));
      
      argc = Args.size()+1;
      char **CArgs;
      CArgs = new char * [argc];
      if(!CArgs)
      {
         cerr << "Failed to allocate CArgs\n";
         return -1;
      }
      CArgs[0] = argv[0];
      for(j=1; j<argc; j++)
      {
         CArgs[j] = new char[Args[j-1].size()+1];
         if(!CArgs[j])
         {
            cerr << "Failed to allocate CArgs[j]\n";
            return -1;
         }
         strcpy(CArgs[j],Args[j-1].c_str());
      }
      
      Par.parseOptions(argc, CArgs);
      delete[] CArgs;
      
         // get help option first
      if(dashh.getCount() > 0)
      {
         Par.displayUsage(cout,false);
         help = true;   //return 1;
      }
      
      if(Par.hasErrors())
      {
         cerr << "\nErrors found in command line input:\n";
         Par.dumpErrors(cerr);
         cerr << "...end of Errors\n\n";
         Par.displayUsage(cout,false);
         help = true; // return -1;
      }
      
         // get values found on command line
      string msg;
      vector<string> values, field;
      
         // f never appears because we intercept it above
      //if(dashf.getCount()) { cout << "Option f "; dashf.dumpValue(cout); }
      
      if(dashi.getCount())
      {
         InputFiles = dashi.getValue();
         if(help)
         {
            cout << "Input: input files (--input) are:\n";
            for(int i=0; i<InputFiles.size(); i++)
            {
               cout << "   " << InputFiles[i] << endl;
            }
         }
      }
      if(dasho.getCount())
      {
         values = dasho.getValue();
         OutputFile = values[0];
         if(help)
            cout << "Input: output file is " << OutputFile << endl;
      }
      if(dashp.getCount())
      {
         values = dashp.getValue();
         InputDirectory = values[0];
         if(InputDirectory[InputDirectory.length() - 1] != '/')
            InputDirectory += "/";
         if(help)
            cout << "Input: set path to " << InputDirectory << endl;
      }

      if(dashr.getCount())
      {
         ReplaceHeader=true;
         if(help)
            cout << "Input: replace header in output" << endl;
      }
      if(dashs.getCount())
      {
         TimeSortTable=true;
         if(help)
            cout << "Input: sort the SAT/Obs table" << endl;
      }
      if(dashg.getCount())
      {
         GPSTimeOutput=true;
         if(help)
            cout << "Input: output in GPS time" << endl;
      }
      // times
      // TD put try  {} around setToString and catch invalid formats...
      if(dashbt.getCount())
      {
         values = dashbt.getValue();
         msg = values[0];
         field.clear();
         while(msg.size() > 0)
         {
            field.push_back(stripFirstWord(msg,','));
         }
         if(field.size() == 2)
         {
            //BegTime.setToString(field[0]+","+field[1], "%F,%g");
            GPSWeekSecond temp(asInt(field[0]), asDouble(field[1]));
            BegTime = temp;
         }
         else if(field.size() == 6)
         {
            //BegTime.setToString(field[0]+","+field[1]+","+field[2]+","
            //   +field[3]+","+field[4]+","+field[5], "%Y,%m,%d,%H,%M,%S");
            CivilTime temp(asInt(field[0]), asInt(field[1]), asInt(field[2]),
                           asInt(field[3]), asInt(field[4]), asInt(field[5]));
            BegTime = temp;
         }
         else
         {
            cerr << "Error: invalid --start input: " << values[0] << endl;
         }
         if(help)
         {
            cout << " Input: begin time " << values[0] << " = "
                 << static_cast<CivilTime>(BegTime).printf("%Y/%02m/%02d %2H:%02M:%06.3f =")
                 << static_cast<GPSWeekSecond>(BegTime).printf(" %F/%10.3g")
                 << endl;
         }
      }
      if(dashet.getCount())
      {
         values = dashet.getValue();
         msg = values[0];
         field.clear();
         while(msg.size() > 0)
         {
            field.push_back(stripFirstWord(msg,','));
         }
         if(field.size() == 2)
         {
            //EndTime.setToString(field[0]+","+field[1], "%F,%g");
            GPSWeekSecond temp(asInt(field[0]), asDouble(field[1]));
            EndTime = temp;
         }
         else if(field.size() == 6)
         {
            //EndTime.setToString(field[0]+","+field[1]+","+field[2]+","
            //   +field[3]+","+field[4]+","+field[5], "%Y,%m,%d,%H,%M,%S");
            CivilTime temp(asInt(field[0]), asInt(field[1]), asInt(field[2]),
                           asInt(field[3]), asInt(field[4]), asInt(field[5]));
            EndTime = temp;
         }
         else
         {
            cerr << "Error: invalid --stop input: " << values[0] << endl;
         }
         if(help)
         {
            cout << " Input: end time " << values[0] << " = "
                 << static_cast<CivilTime>(EndTime).printf("%Y/%02m/%02d %2H:%02M:%06.3f = ")
                 << static_cast<GPSWeekSecond>(EndTime).printf("%F/%10.3g")
                 << endl;
         }
      }
      
      if(dashb.getCount())
      {
         brief = true;
         if(help)
            cout << "Input: found the brief flag" << endl;
      }

      if(dashd.getCount())
      {
         debug = true;
         if(help)
            cout << "Input: found the debug flag" << endl;
      }

      if(Rest.getCount())
      {
         values = Rest.getValue();
         if(help)
            cout << "Input: input files are:\n";
         for (int i=0; i<values.size(); i++)
         {
            if(help)
               cout << "  " << values[i] << endl;
            InputFiles.push_back(values[i]);
         }
      }

      if(debug && help)
      {
         cout << "\nTokens on command line (" << Args.size() << ") are:"
              << endl;
         for(j=0; j<Args.size(); j++)
            cout << Args[j] << endl;
      }
      if(help)
         return 1;

      return 0;
   }
   catch(Exception& e)
   {
      GPSTK_RETHROW(e);
   }
   catch(exception& e)
   {
      Exception E("std except: "+string(e.what()));
      GPSTK_THROW(E);
   }
   catch(...)
   {
      Exception e("Unknown exception");
      GPSTK_THROW(e);
   }
   return -1;
}

//------------------------------------------------------------------------------------
// Pull out -f<f> and --file <f> and deprecated options
void PreProcessArgs(const char *arg, vector<string>& Args)
   throw(Exception)
{
   try {
      static bool found_cfg_file = false;

      if(found_cfg_file || (arg[0]=='-' && arg[1]=='f'))
      {
         string filename(arg);
         
         if(!found_cfg_file)
            filename.erase(0,2);
         else
            found_cfg_file = false;
         
         ifstream infile(filename.c_str());
         if(!infile)
         {
            cout << "Error: could not open options file " << filename << endl;
            return;
         }

         bool again_cfg_file = false;
         char c;
         string buffer, word;
         while(1)
         {
            getline(infile,buffer);
            stripTrailing(buffer,'\r');

               // process the buffer before checking eof or bad b/c there can be
               // a line at EOF that has no CRLF...
            while(!buffer.empty())
            {
               word = firstWord(buffer);
               if(again_cfg_file)
               {
                  word = "-f" + word;
                  again_cfg_file = false;
                  PreProcessArgs(word.c_str(),Args);
               }
               else if(word[0] == '#')
               { // skip to end of line
                  buffer = "";
               }
               else if(word == "--file" || word == "-f")
                  again_cfg_file = true;
               else if(word[0] == '"')
               {
                  word = stripFirstWord(buffer,'"');
                  buffer = "dummy " + buffer;            // to be stripped later
                  PreProcessArgs(word.c_str(),Args);
               }
               else
                  PreProcessArgs(word.c_str(),Args);

               word = stripFirstWord(buffer);      // now remove it from buffer
            }
            if(infile.eof() || !infile.good())
               break;
         }
      }
      else if(string(arg) == "--file" || string(arg) == "-f")
         found_cfg_file = true;
      // old versions of args -- deprecated
      else if(string(arg)==string("--EpochBeg"))
      {
         Args.push_back("--start");
      }
      else if(string(arg)==string("--GPSBeg"))
      {
         Args.push_back("--start");
      }
      else if(string(arg)==string("--EpochEnd"))
      {
         Args.push_back("--stop");
      }
      else if(string(arg)==string("--GPSEnd"))
      {
         Args.push_back("--stop");
      }
      // regular arg
      else
         Args.push_back(arg);
   }
   catch(Exception& e)
   {
      GPSTK_RETHROW(e);
   }
   catch(exception& e)
   {
      Exception E("std except: "+string(e.what()));
      GPSTK_THROW(E);
   }
   catch(...)
   {
      Exception e("Unknown exception");
      GPSTK_THROW(e);
   }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
