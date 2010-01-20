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
 * @file RinSum.cpp
 * Read and summarize Rinex observation files, optionally fill header in-place.
 */

#include "MathBase.hpp"
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "RinexNavBase.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"
#include "DayTime.hpp"
#include "SatID.hpp"
#include "RinexSatID.hpp"
#include "CommandOptionParser.hpp"
#include "CommandOption.hpp"
#include "icd_200_constants.hpp"
#include "RinexUtilities.hpp"

#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>

using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
string version("2.8 5/07/09");

// data input from command line
vector<string> InputFiles;
string InputDirectory;
string OutputFile;
ostream* pout;
DayTime BegTime, EndTime;
bool ReplaceHeader=false;     // replace the file header in-place
bool TimeSortTable=false;     // sort the PRN/Obs table on time (else PRN)
bool GPSTimeOutput=false;     // output GPS times (week, sec-of-week)
double doGaps=-1;             // output a list of gaps, assuming interval doGaps
bool debug=false;             // debug output - prints all the data
bool brief=false;             // brief output
bool progress=false;          // output progress info to screen (for GUI)
bool screen=false;            // print to screen even if OutputFile is given

//------------------------------------------------------------------------------------
// class used to store SAT/Obs table
class TableData {
public:
   RinexSatID sat;
   vector<int> nobs;
   double prevC1,prevP1,prevL1;
   DayTime begin,end;
   vector<int> gapcounts;
   TableData(const SatID& p, const int& n)
      { sat=RinexSatID(p); nobs=vector<int>(n); prevC1=prevP1=prevL1=0; };
      // needed for find()
   inline bool operator==(const TableData& d) {return d.sat == sat;}
};
   // for sort()
class TableSATLessThan  {      
public:
   bool operator()(const TableData& d1, const TableData& d2)
      { return d1.sat < d2.sat; }
};
class TableBegLessThan  {
public:
   bool operator()(const TableData& d1, const TableData& d2)
      { return d1.begin < d2.begin; }
};

//------------------------------------------------------------------------------------
// prototypes
int GetCommandLine(int argc, char **argv) throw(Exception);
void PreProcessArgs(const char *arg, vector<string>& Args) throw(Exception);

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   int iret,i,j,k,n,ifile,nsats,nclkjumps,L1lli;
   double C1,L1,P1,clkjumpave,clkjumpvar;
   DayTime last,prev,ftime;
   vector<DayTime> clkjumpTimes;
   vector<double> clkjumpMillsecs,clkjumpUncertainty;
   vector<int> clkjumpAgree;
   // data used for computation
   const int ndtmax=15;
   double compDT,dt,bestdt[ndtmax];
   int ncompDT,ndt[ndtmax],ncount;
   int nepochs,ncommentblocks;

   BegTime = DayTime::BEGINNING_OF_TIME;  // init. here to avoid static init.problem
   EndTime = DayTime::END_OF_TIME;

      // Title and description
   string Title;
   Title = "RinSum, part of the GPS ToolKit, Ver " + version + ", Run ";
   time_t timer;
   struct tm *tblock;
   timer = time(NULL);
   tblock = localtime(&timer);
   last.setYMDHMS(1900+tblock->tm_year,1+tblock->tm_mon,
               tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
   Title += last.printf("%04Y/%02m/%02d %02H:%02M:%02S\n");
   cout << Title;

   iret=GetCommandLine(argc, argv);
   if(iret) return iret;

   iret = RegisterARLUTExtendedTypes();
   if(iret) return iret;

      // open the output file and write to it
   if(!OutputFile.empty()) {
      pout = new ofstream(OutputFile.c_str(),ios::out);
      if(pout->fail()) {
         cerr << "Could not open output file " << OutputFile << endl;
         pout = &cout;
      }
      else {
         pout->exceptions(ios::failbit);
         *pout << Title;
         cout << "Writing summary to file " << OutputFile << endl;
      }
   }
   else {
      pout = &cout;
      screen = false;
   }

      // add path to input file names
   if(!InputDirectory.empty()) for(ifile=0; ifile<InputFiles.size(); ifile++) {
      InputFiles[ifile] = InputDirectory + "/" + InputFiles[ifile];
   }

      // sort the input file names on header first time
   if(InputFiles.size() > 1) sortRinexObsFiles(InputFiles);

   if(InputFiles.size() == 0) {
      *pout << "File(s) do not exist or are not RINEX observation\n";
      if(screen) cout << "File(s) do not exist or are not RINEX observation\n";
      return -1;
   }

      // now open the input files, read the headers and data
   RinexObsHeader rheader;
   RinexObsData robs;
   for(ifile=0; ifile<InputFiles.size(); ifile++) {
      int nprogress;
      if(progress) cout << "PROGRESS " << (nprogress=5) << endl << flush;

      string filename = InputFiles[ifile];
      RinexObsStream InStream(filename.c_str());
      if(!InStream.is_open()) {
         *pout << "File " << filename << " could not be opened.\n";
         if(screen) cout << "File " << filename << " could not be opened.\n";
         continue;
      }
      //else *pout << "File " << filename << " has been successfully opened.\n";
      InStream.exceptions(ios::failbit);

      // get file size
      long begin,end,filesize,bytesperepoch=1300,totN;
      begin = InStream.tellg();
      InStream.seekg(0,ios::end);
      end = InStream.tellg();
      InStream.seekg(0,ios::beg);
      filesize = end-begin;
      totN = filesize/bytesperepoch;

      prev = DayTime::BEGINNING_OF_TIME;
      ftime = DayTime::BEGINNING_OF_TIME;

      if(!brief) {
         *pout << "+++++++++++++ RinSum summary of Rinex obs file "
            << filename << " +++++++++++++\n";
         if(screen) cout << "+++++++++++++ RinSum summary of Rinex obs file "
            << filename << " +++++++++++++\n";
      }
      else {
         *pout << "\nFile name: " << filename << endl;
         if(screen) cout << "\nFile name: " << filename << endl;
      }
      
         // input header
      try {
         InStream >> rheader;
      }
      catch(gpstk::FFStreamError& e) {
         cerr << "Caught an FFStreamError while reading header: "
            << e.getText(0) << endl;
      }
      catch(gpstk::Exception& e) {
         cerr << "Caught a gpstk exception while reading header: "
            << e.getText(0) << endl;
      }

      if(!brief) {
         *pout << "Rinex header:\n"; rheader.dump(*pout);
         if(screen) { cout << "Rinex header:\n"; rheader.dump(cout); }
      }
      else {
         *pout << "Position (XYZ,m) : " << fixed << setprecision(4)
            << rheader.antennaPosition << ".\n";
         if(screen) cout << "Position (XYZ,m) : " << fixed << setprecision(4)
            << rheader.antennaPosition << ".\n";
      }

      if(!rheader.isValid()) {
         *pout << "Abort: header is invalid\n";
         if(screen) cout << "Abort: header is invalid\n";
         if(!brief) {
            *pout << "\n+++++++++++++ End of RinSum summary of " << filename
               << " +++++++++++++\n";
            if(screen) cout << "\n+++++++++++++ End of RinSum summary of "
               << filename << " +++++++++++++\n";
         }
         continue;
      }

      //RinexObsStream out(argv[2], ios::out);
      //out << rheader;

      // initialize
      nepochs = ncommentblocks = 0;
      for(i=0; i<ndtmax; i++) ndt[i]=-1;
      n = rheader.obsTypeList.size();
      vector<TableData> table;
      vector<int> totals(n);

      if(pout == &cout) *pout << "Reading the observation data..." << endl;

         // input obs
      if(progress) cout << "PROGRESS " << (nprogress+=5) << endl << flush;
      while(InStream >> robs)
      {
         if(debug) *pout << "Epoch: " << robs.time
            << ", Flag " << robs.epochFlag
            << ", Nsat " << robs.obs.size()
            << ", clk " << fixed << robs.clockOffset << endl;

          // is this a comment?
         if(robs.epochFlag > 1) {
            ncommentblocks++;
            //*pout << "inline header info:\n";
            //robs.auxHeader.dump(*pout);
            continue;
         }

         // update first and last time seen, check time limits, count epochs
         last = robs.time;
         if(last < BegTime) continue;
         if(last > EndTime) break;
         if(ftime == DayTime::BEGINNING_OF_TIME) ftime=last;
         nepochs++;
         nsats = nclkjumps = 0;  // count sats and signs clock jumps have occurred
         clkjumpave = clkjumpvar = 0.0;
         if(doGaps > 0) {
            ncount = int(0.5+(last-ftime)/doGaps); // compute the gapcount
            if(debug) *pout << "Gap count at " << robs.time << " is "
               << ncount << endl;
            // test after 50 epochs - wrong doGaps is disasterous
            if(ncompDT == 50 && fabs(compDT - doGaps) > 1.0) {
               *pout << "WARNING: --gaps interval does not match "
                  << "computed data interval. *** Turn off --gaps ***\n";
               doGaps = -1;
            }
         }

         // loop over satellites
         RinexObsData::RinexSatMap::const_iterator it;
         RinexObsData::RinexObsTypeMap::const_iterator jt;
         for(it=robs.obs.begin(); it != robs.obs.end(); ++it) {
            // update the table
            vector<TableData>::iterator ptab;
            ptab = find(table.begin(),table.end(),TableData(it->first,n));
            if(ptab == table.end()) {        // sat not found in table - create one
               table.push_back(TableData(it->first,n));
               ptab = find(table.begin(),table.end(),TableData(it->first,n));
               ptab->begin = last;
               if(doGaps > 0) {
                  ptab->gapcounts.push_back(ncount);      // start time
                  ptab->gapcounts.push_back(ncount-1);    // end time
               }
            }
            // update list of gap times
            if(doGaps > 0) {
               i = ptab->gapcounts.size() - 1;        // index of the current end time
               if(ncount == ptab->gapcounts[i] + 1) { // no gap
                  ptab->gapcounts[i] = ncount;
               }
               else {                                 // found a gap
                  ptab->gapcounts.push_back(ncount);  // start time
                  ptab->gapcounts.push_back(ncount);  // end time
               }
            }
            // update end time for this sat
            ptab->end = last;
            if(debug) *pout << "Sat " << setw(2) << RinexSatID(it->first);

            // loop over obs types
            C1 = P1 = L1 = 0;
            for(jt=it->second.begin(); jt!=it->second.end(); jt++) {
               // find the index for this obs type
               for(k=0; k<n; k++) if(rheader.obsTypeList[k] == jt->first) break;
               // count this obs
               if(jt->second.data != 0) {
                  ptab->nobs[k]++;      // per obs
                  totals[k]++;
               }
               // save L1 range and phase for clk jump test below
               if(jt->first==RinexObsHeader::C1) C1 = jt->second.data*1000.0/C_GPS_M;
               if(jt->first==RinexObsHeader::P1) P1 = jt->second.data*1000.0/C_GPS_M;
               if(jt->first == RinexObsHeader::L1) {
                  L1 = jt->second.data * 1000.0/C_GPS_M;
                  L1lli = jt->second.lli;
               }
               // dump this data
               if(debug) *pout << " " << RinexObsHeader::convertObsType(jt->first)
                  << " " << setw(13) << setprecision(3) << jt->second.data << " "
                  << jt->second.lli << " " << jt->second.ssi;
            }  // end loop over obs types
            if(debug) *pout << endl;

            // test for millisecond clock adjusts -
            // sometimes they are applied to range but not phase or vice-versa
            if(prev != DayTime::BEGINNING_OF_TIME && L1 != 0 && ptab->prevL1 != 0) {
               int nms;
               double test;
               nsats++;
               if(P1 != 0 && ptab->prevP1 != 0)
                  test = P1-L1_WAVELENGTH*L1
                     - (ptab->prevP1-L1_WAVELENGTH*ptab->prevL1);
               else if(C1 != 0 && ptab->prevC1 != 0)
                  test = C1-L1_WAVELENGTH*L1
                     - (ptab->prevC1-L1_WAVELENGTH*ptab->prevL1);
               else
                  test = 0.0;
               if(fabs(test) > 0.5) {      // test must be > 150 km =~ 1/2 millisecond
                  // is it nearly an even multiple of 1 millisecond?
                  //test *= 1000.0/C_GPS_M;  // leave sign on it
                  if(debug) *pout << "possible clock jump: test = "
                                 << setprecision(9) << test;
                  nms = long(test + (test > 0 ? 0.5 : -0.5));
                  if(fabs(test - double(nms)) < 0.001) {
                     if(debug) *pout << " -> " << setprecision(9)
                              << fabs(test - double(nms));
                     // keep clkjumpave = sequential average nms, clkjumpvar=variance
                     //if(test < 0) nms *= -1;
                     nclkjumps++;
                     clkjumpave += (double(nms)-clkjumpave)/double(nclkjumps);
                     if(nclkjumps > 1)
                        clkjumpvar = (clkjumpvar*(nclkjumps-2)
                         + nclkjumps*(double(nms)-clkjumpave)*(double(nms)-clkjumpave)
                            /(nclkjumps-1))/(nclkjumps-1);
                  }
                  else if(debug) *pout << " - failed.";
                  if(debug && L1lli != 0) { *pout << " LLI is set"; }
                  if(debug) *pout << " " << RinexSatID(it->first)
                     << " " << last.printf("%4F %.3g") << endl;
               }
            }
            // save C1,L1,P1 for this sat for next time
            ptab->prevC1 = C1;
            ptab->prevL1 = L1;
            ptab->prevP1 = P1;

         }  // end loop over sats

         //out << robs;

         // if more than half the sats saw a clk jump, call it
         if(nclkjumps > nsats/2) {
            if(debug) *pout << "test nclkjumps is " << nclkjumps
               << " and nsats is " << nsats
               << ", ave is " << fixed << setprecision(3) << clkjumpave
               << " and stddev is " << setprecision(3) << sqrt(clkjumpvar)
               << endl;
            clkjumpTimes.push_back(last);
            clkjumpMillsecs.push_back(clkjumpave);
            clkjumpAgree.push_back(nsats-nclkjumps);
            clkjumpUncertainty.push_back(sqrt(clkjumpvar));
         }

         if(prev != DayTime::BEGINNING_OF_TIME) {
            dt = last-prev;
            if(dt > 0.0) {
               for(i=0; i<ndtmax; i++) {
                  if(ndt[i] <= 0) { bestdt[i]=dt; ndt[i]=1; break; }
                  if(fabs(dt-bestdt[i]) < 0.0001) { ndt[i]++; break; }
                  if(i == ndtmax-1) {
                     k = 0;
                     int nleast=ndt[k];
                     for(j=1; j<ndtmax; j++) if(ndt[j] <= nleast) {
                        k=j; nleast=ndt[j];
                     }
                     ndt[k]=1; bestdt[k]=dt;
                  }
               }
               // update computed dt -- for gaps
               if(doGaps > 0) {
                  for(i=1,j=0; i<ndtmax; i++) if(ndt[i]>ndt[j]) j=i;
                  compDT = bestdt[j];
                  ncompDT = ndt[j];
               }
            }
            else {
               cerr << " WARNING time tags out of order: "
                  //<< " prev >= curr : "
                  << prev.printf("%F/%.0g = %04Y/%02m/%02d %02H:%02M:%02S")
                  << " >= "
                  << last.printf("%F/%.0g = %04Y/%02m/%02d %02H:%02M:%02S")
                  << endl;
            }
         }
         prev = last;

         if(progress && nepochs % 500 == 0)
            cout << "PROGRESS " << (nprogress=10+85*nepochs/totN) << endl << flush;
      }  // end loop over epochs in the file
      InStream.close();

      if(progress && ifile > 0)
         cout << "PROGRESS " << (nprogress=95) << endl << flush;

         // check that we found some data
      if(nepochs <= 0) {
         *pout << "File " << filename << " : no data found. Are time limits wrong?\n";
         if(screen) cout << "File "
            << filename << " : no data found. Are time limits wrong?\n";
         continue;
      }

         // compute interval
      for(i=1,j=0; i<ndtmax; i++) if(ndt[i]>ndt[j]) j=i;
      compDT = bestdt[j];
 
      ostringstream oss;

         // summary info
      oss << "Computed interval "
         << fixed << setw(5) << setprecision(2) << compDT << " seconds." << endl;
      oss << "Computed first epoch: " << ftime.printf("%4F %14.7g") << " = "
            << ftime.printf("%04Y/%02m/%02d %02H:%02M:%010.7f") << endl;
      oss << "Computed last  epoch: " << last.printf("%4F %14.7g") << " = "
            << last.printf("%04Y/%02m/%02d %02H:%02M:%010.7f") << endl;

      oss << "Computed time span:";
      double secs=last-ftime;
      int iday = int(secs/86400.0);
      if(iday > 0) oss << " " << iday << "d";
      DayTime delta;
      delta.setSecOfDay(secs - iday*86400);
      oss << " " << delta.hour() << "h "
         << delta.minute() << "m "
         << delta.second() << "s = "
         << secs << " seconds\nComputed file size: "
         << filesize << " bytes." << endl;

      i = 1+int(0.5+(last-ftime)/compDT);
      if(!brief) oss << "There were " << nepochs << " epochs ("
         << setprecision(2) << double(nepochs*100)/i
         << "% of " << i << " possible epochs in this timespan) and "
         << ncommentblocks << " inline header blocks.\n";

         // sort table
      sort(table.begin(),table.end(),TableSATLessThan());
      if(TimeSortTable) sort(table.begin(),table.end(),TableBegLessThan());

         // output table
         // header
      vector<TableData>::iterator tit;
      if(table.size() > 0) table.begin()->sat.setfill('0');
      if(!brief) {
         oss << "\n          Summary of data available in this file: "
            << "(Totals are based on times and interval)\n";
         oss << "Sat  OT:";
         for(k=0; k<n; k++)
            oss << setw(7) << rheader.obsTypeList[k].type;
         oss << "  Total             Begin time - End time\n";
            // loop
         for(tit=table.begin(); tit!=table.end(); ++tit) {
            oss << "Sat " << tit->sat << " ";
            for(k=0; k<n; k++) oss << setw(7) << tit->nobs[k];
            // compute total based on times
            oss << setw(7) << 1+int(0.5+(tit->end-tit->begin)/compDT);
            if(GPSTimeOutput) {
               oss << "  " << tit->begin.printf("%4F %10.3g")
                  << " - " << tit->end.printf("%4F %10.3g") << endl;
            }
            else {
               oss
                  << "  " << tit->begin.printf("%04Y/%02m/%02d %02H:%02M:%04.1f")
                  << " - " << tit->end.printf("%04Y/%02m/%02d %02H:%02M:%04.1f")
                  << endl;
            }
         }
         oss << "TOTAL   "; for(k=0; k<n; k++) oss << setw(7) << totals[k];
         oss << endl;
      }
      else {
         oss << "SATs(" << table.size() << "):";
         for(tit=table.begin(); tit!=table.end(); ++tit)
            oss << " " << tit->sat;
         oss << endl;

         oss << "Obs types(" << rheader.obsTypeList.size() << "): ";
         for(i=0; i<rheader.obsTypeList.size(); i++)
            oss << " " << rheader.obsTypeList[i].type;
         oss << endl;
      }

      // output gaps
      if(doGaps > 0) {
         oss << "\n Summary of gaps in the data in this file, "
            << "assuming interval " << doGaps << " sec.\n"
            << "  (count is number of intervals from computed first epoch)\n";
         oss << "    Sat  beg -  end (count,size) ... :\n";
            // loop
         for(tit=table.begin(); tit!=table.end(); ++tit) {
            k = tit->gapcounts.size()-1;
            if(debug) {
               oss << "Dump " << tit->sat;
               for(i=0; i<=k; i++) oss << " " << tit->gapcounts[i];
               oss << endl;
            }
            oss << "Sat " << tit->sat << " " << setw(4) << tit->gapcounts[0]
               << " - " << setw(4) << tit->gapcounts[k];
            for(i=1; i<=k-2; i+=2) {
               oss << " (" << tit->gapcounts[i]+1  // begin of gap
                  << "," << tit->gapcounts[i+1]-tit->gapcounts[i]-1 << ")";  // size
            }
            oss << endl;
         }
      }

         // warnings
      if((rheader.valid & RinexObsHeader::intervalValid)
            && fabs(compDT-rheader.interval) > 1.e-3)
         oss << " WARNING: Computed interval is " << setprecision(2)
            << compDT << " sec, while input header has " << setprecision(2)
            << rheader.interval << " sec.\n";
      if(fabs(ftime-rheader.firstObs) > 1.e-8)
         oss << " WARNING: Computed first time does not agree with header\n";
      if((rheader.valid & RinexObsHeader::lastTimeValid)
            && fabs(last-rheader.lastObs) > 1.e-8)
         oss << " WARNING: Computed last time does not agree with header\n";

      if(clkjumpTimes.size() > 0) {
         oss << " WARNING: millisecond clock adjusts at these times:\n";
         for(i=0; i<clkjumpTimes.size(); i++) {
            oss << "   "
             << clkjumpTimes[i].printf("%4F %10.3g = %04Y/%02m/%02d %02H:%02M:%06.3f")
             << " " << setw(5) << setprecision(2) << clkjumpMillsecs[i]
             << " ms_clock_adjust";
             if(clkjumpAgree[i] > 0 || clkjumpUncertainty[i] > 0.01)
               oss << " (low quality determination; data may be irredeemable)";
            oss << endl;
         }
      }
         // look for 'empty' obs types
      for(k=0; k<n; k++) {
         if(totals[k] <= 0) oss << " WARNING: ObsType "
            << rheader.obsTypeList[k].type
            << " should be deleted from header.\n";
      }

      // print
      *pout << oss.str();
      if(screen) cout << oss.str();

      if(ReplaceHeader) {
            // modify the header
         rheader.version = 2.1; rheader.valid |= RinexObsHeader::versionValid;
         rheader.interval = compDT; rheader.valid |= RinexObsHeader::intervalValid;
         rheader.lastObs = last; rheader.valid |= RinexObsHeader::lastTimeValid;
            // now the table
         rheader.numSVs = table.size(); rheader.valid |= RinexObsHeader::numSatsValid;
         rheader.numObsForSat.clear();
         for(tit=table.begin(); tit!=table.end(); ++tit) {      // tit defined above
            rheader.numObsForSat.insert(
               map<SatID, vector<int> >::value_type(tit->sat,tit->nobs) );
         }
         rheader.valid |= RinexObsHeader::prnObsValid;
         //*pout << "\nNew header\n";
         //rheader.dump(*pout);

            // now re-open the file and replace the header
#ifdef _MSC_VER
         char newname[L_tmpnam];
         if(!tmpnam(newname)) {
            cerr << "Could not create temporary file name - abort\n";
            return -1;
         }
#else
         char newname[]="RinSumTemp.XXXXXX";
         if(mkstemp(newname)==-1) {
            cerr << "Could not create temporary file name - abort\n";
            return -1;
         }
#endif
         remove(newname);

         RinexObsHeader rhjunk;
         RinexObsStream ROutStr(newname, ios::out);
         RinexObsStream InAgain(filename.c_str());
         InAgain.exceptions(ios::failbit);

         InAgain >> rhjunk;
         ROutStr << rheader;
         while(InAgain >> robs) {
            last = robs.time;
            if(last < BegTime) continue;
            if(last > EndTime) break;
            ROutStr << robs;
         }
         InAgain.close();
         ROutStr.close();
            // delete original file and rename the temporary
         ostringstream oss2;
         iret = remove(filename.c_str());
         if(iret) oss2 << "RinSum: Error: Could not remove existing file: "
            << filename << endl;
         else {
            iret = rename(newname,filename.c_str());
            if(iret) oss2 << "RinSum: Error: Could not rename new file " << newname
               << " using old name " << filename << endl;
            else oss2 << "\nRinSum: Replaced original header with complete one,"
               << " using temporary file name "
               << newname << endl;
         }
         *pout << oss2.str();
         if(screen) cout << oss2.str();
      }

      if(!brief) {
         *pout << "\n+++++++++++++ End of RinSum summary of " << filename
            << " +++++++++++++\n";
         if(screen) cout << "\n+++++++++++++ End of RinSum summary of " << filename
            << " +++++++++++++\n";
      }

   }  // end loop over input files

   if(pout != &cout) {
      ((ofstream *)pout)->close();
      delete pout;
   }

   return 0;
}
catch(gpstk::FFStreamError& e) { cerr << "FFStreamError: " << e; }
catch(gpstk::Exception& e) { cerr << "Exception: " << e; }
catch (...) { cerr << "Unknown exception.  Abort." << endl; }
   return 1;
}   // end main()

//------------------------------------------------------------------------------------
int GetCommandLine(int argc, char **argv) throw(Exception)
{
try {
   bool help=false;
   int j;
      // required options

      // optional
   CommandOption dashi(CommandOption::hasArgument, CommandOption::stdType,
      'i',"input"," [-i|--input] <file>  Input RINEX observation file names ()");
   //dashi.setMaxCount(1);

      // optional options
      // this only so it will show up in help page...
   CommandOption dashf(CommandOption::hasArgument, CommandOption::stdType,'f',
      "file"," [-f|--file] <file>   file containing more options ()");

   CommandOption dasho(CommandOption::hasArgument, CommandOption::stdType,'o',
      "output"," [-o|--output] <file> Output the summary to a file named <file> ()");
   dasho.setMaxCount(1);
   
   CommandOption dashp(CommandOption::hasArgument, CommandOption::stdType, 'p',
      "path"," [-p|--path] <path>   Find the input file(s) in this directory (.)");
   dashp.setMaxCount(1);

   CommandOptionNoArg dashr('R', "Replace",
      " [-R|--Replace]       Replace input file header with a full one, in place ()");
   dashr.setMaxCount(1);

   CommandOptionNoArg dashs('s', "sort",
      " [-s|--sort]          Sort the SAT/Obs table on begin time (don't)");

   CommandOptionNoArg dashg('g', "gps",
      " [-g|--gps]           Print times in the SAT/Obs table as GPS times (don't)");

   CommandOption dashgap(CommandOption::hasArgument, CommandOption::stdType,0,"gaps",
      " --gaps <dt>          Print a table of gaps in the data, assuming interval dt"
      " (don't)");

   // time
   // times - don't use CommandOptionWithTimeArg
   CommandOption dashbt(CommandOption::hasArgument, CommandOption::stdType,
      0,"start", " --start <time>       Start time: <time> is 'GPSweek,sow' OR "
      "'YYYY,MM,DD,HH,Min,Sec' ()");
   dashbt.setMaxCount(1);

   CommandOption dashet(CommandOption::hasArgument, CommandOption::stdType,
      0,"stop", " --stop <time>        Stop time: <time> is 'GPSweek,sow' OR "
      "'YYYY,MM,DD,HH,Min,Sec' ()");
   dashet.setMaxCount(1);

   CommandOptionNoArg dashb('b', "brief",
      " [-b|--brief]         produce a brief (6-line) summary (don't)");

   // help and debug
   CommandOptionNoArg dashh('h', "help",
      " [-h|--help]          print this help page and quit (don't)");
   CommandOptionNoArg dashd('d', "debug",
      " [-d|--debug]         print debugging info (don't)");

   // ... other options
   CommandOptionRest Rest("<filename(s)>");

   CommandOptionParser Par(
      "Prgm RinSum reads a Rinex file and summarizes it content. It can also\n"
      " (option) fill in the header of the input file. NB. Either <filenames>\n"
      " or --input is required; put <filenames> after all options.\n"
      );

   // allow user to put all options in a file
   // could also scan for debug here
   vector<string> Args;
   for(j=1; j<argc; j++) PreProcessArgs(argv[j],Args);

   if(Args.size()==0)
      Args.push_back(string("-h"));

   argc = Args.size()+1;
   char **CArgs;
   CArgs = new char * [argc];
   if(!CArgs) { cerr << "Failed to allocate CArgs\n"; return -1; }
   CArgs[0] = argv[0];
   for(j=1; j<argc; j++) {
      CArgs[j] = new char[Args[j-1].size()+1];
      if(!CArgs[j]) { cerr << "Failed to allocate CArgs[j]\n"; return -1; }
      strcpy(CArgs[j],Args[j-1].c_str());
   }

   Par.parseOptions(argc, CArgs);
   delete[] CArgs;

      // get help option first
   if(dashh.getCount()) {
      Par.displayUsage(cout,false);
      help = true;   //return 1;
   }

   if(Par.hasErrors()) {
      cerr << "\nErrors found in command line input:\n";
      Par.dumpErrors(cerr);
      cerr << "...end of Errors\n\n";
      Par.displayUsage(cout,false);
      help = true; // return -1;
   }
   
      // get values found on command line
   string msg;
   vector<string> values,field;

      // f never appears because we intercept it above
   //if(dashf.getCount()) { cout << "Option f "; dashf.dumpValue(cout); }

   if(dashi.getCount()) {
      InputFiles = dashi.getValue();
      if(help) {
         cout << "Input: input files (--input) are:\n";
         for(int i=0; i<InputFiles.size(); i++)
            cout << "   " << InputFiles[i] << endl;
      }
   }
   if(dasho.getCount()) {
      values = dasho.getValue();
      OutputFile = values[0];
      if(help) cout << "Input: output file is " << OutputFile << endl;
   }
   if(dashp.getCount()) {
      values = dashp.getValue();
      InputDirectory = values[0];
      if(help) cout << "Input: set path to " << InputDirectory << endl;
   }

   if(dashr.getCount()) {
      ReplaceHeader=true;
      if(help) cout << "Input: replace header in output" << endl;
   }
   if(dashs.getCount()) {
      TimeSortTable=true;
      if(help) cout << "Input: sort the SAT/Obs table" << endl;
   }
   if(dashg.getCount()) {
      GPSTimeOutput=true;
      if(help) cout << "Input: output in GPS time" << endl;
   }
   if(dashgap.getCount()) {
      values = dashgap.getValue();
      doGaps = asDouble(values[0]);
      if(help) cout << "Input: output list of gaps, assuming data time interval "
         << doGaps << endl;
   }
   // times
   // TD put try  {} around setToString and catch invalid formats...
   if(dashbt.getCount()) {
      values = dashbt.getValue();
      msg = values[0];
      field.clear();
      while(msg.size() > 0)
         field.push_back(stripFirstWord(msg,','));
      if(field.size() == 2)
         BegTime.setToString(field[0]+","+field[1], "%F,%g");
      else if(field.size() == 6)
         BegTime.setToString(field[0]+","+field[1]+","+field[2]+","+field[3]+","
            +field[4]+","+field[5], "%Y,%m,%d,%H,%M,%S");
      else {
         cerr << "Error: invalid --start input: " << values[0] << endl;
      }
      if(help) cout << " Input: begin time " << values[0] << " = "
         << BegTime.printf("%Y/%02m/%02d %2H:%02M:%06.3f = %F/%10.3g") << endl;
   }
   if(dashet.getCount()) {
      values = dashet.getValue();
      msg = values[0];
      field.clear();
      while(msg.size() > 0)
         field.push_back(stripFirstWord(msg,','));
      if(field.size() == 2)
         EndTime.setToString(field[0]+","+field[1], "%F,%g");
      else if(field.size() == 6)
         EndTime.setToString(field[0]+","+field[1]+","+field[2]+","+field[3]+","
            +field[4]+","+field[5], "%Y,%m,%d,%H,%M,%S");
      else {
         cerr << "Error: invalid --stop input: " << values[0] << endl;
      }
      if(help) cout << " Input: end time " << values[0] << " = "
         << EndTime.printf("%Y/%02m/%02d %2H:%02M:%06.3f = %F/%10.3g") << endl;
   }

   if(dashb.getCount()) {
      brief = true;
      if(help) cout << "Input: found the brief flag" << endl;
   }

   if(dashd.getCount()) {
      debug = true;
      if(help) cout << "Input: found the debug flag" << endl;
   }

   if(Rest.getCount()) {
      values = Rest.getValue();
      if(help) cout << "Input: input files are:\n";
      for(int i=0; i<values.size(); i++) {
         if(help) cout << "  " << values[i] << endl;
         InputFiles.push_back(values[i]);
      }
   }

   if(debug && help) {
      cout << "\nTokens on command line (" << Args.size() << ") are:" << endl;
      for(j=0; j<Args.size(); j++) cout << Args[j] << endl;
   }
   if(help) return 1;

   return 0;
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
   return -1;
}

//------------------------------------------------------------------------------------
// Pull out -f<f> and --file <f> and deprecated options
void PreProcessArgs(const char *arg, vector<string>& Args) throw(Exception)
{
try {
   static bool found_cfg_file=false;

   if(found_cfg_file || (arg[0]=='-' && arg[1]=='f')) {
      string filename(arg);
      if(!found_cfg_file) filename.erase(0,2); else found_cfg_file = false;
      ifstream infile(filename.c_str());
      if(!infile) {
         cout << "Error: could not open options file " << filename << endl;
         return;
      }

      bool again_cfg_file=false;
      char c;
      string buffer,word;
      while(1) {
         getline(infile,buffer);
         stripTrailing(buffer,'\r');

         // process the buffer before checking eof or bad b/c there can be
         // a line at EOF that has no CRLF...
         while(!buffer.empty()) {
            word = firstWord(buffer);
            if(again_cfg_file) {
               word = "-f" + word;
               again_cfg_file = false;
               PreProcessArgs(word.c_str(),Args);
            }
            else if(word[0] == '#') { // skip to end of line
               buffer = "";
            }
            else if(word == "--file" || word == "-f")
               again_cfg_file = true;
            else if(word[0] == '"') {
               word = stripFirstWord(buffer,'"');
               buffer = "dummy " + buffer;            // to be stripped later
               PreProcessArgs(word.c_str(),Args);
            }
            else
               PreProcessArgs(word.c_str(),Args);

            word = stripFirstWord(buffer);      // now remove it from buffer
         }
         if(infile.eof() || !infile.good()) break;
      }
   }
   else if(string(arg) == "--file" || string(arg) == "-f")
      found_cfg_file = true;
   // old versions of args -- deprecated
   else if(string(arg)==string("--EpochBeg")) { Args.push_back("--start"); }
   else if(string(arg)==string("--GPSBeg")) { Args.push_back("--start"); }
   else if(string(arg)==string("--EpochEnd")) { Args.push_back("--stop"); }
   else if(string(arg)==string("--GPSEnd")) { Args.push_back("--stop"); }
   // undocumented args
   else if(string(arg)==string("--progress")) progress = true;
   else if(string(arg)==string("--screen")) screen = true;
   // regular arg
   else Args.push_back(arg);
}
catch(Exception& e) { GPSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GPSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GPSTK_THROW(e); }
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
