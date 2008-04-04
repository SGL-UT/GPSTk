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
 * @file RinexDump.cpp
 * Dump Rinex observation data to a flat file.
 * Read a RINEX file and dump the data for the given satellite(s).
 * Any number of obstypes may appear in the command; if none appear, all are dumped.
 * Any number of satellite ID (e.g. G27) may appear; if none appears, all are dumped.
 * The output file is ASCII column-delimited with time, satellite ID and then three
 * columns 'observation LLI SSI' for each observation type.
 */

#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "DayTime.hpp"
#include "RinexSatID.hpp"
#include "StringUtils.hpp"
#include "RinexUtilities.hpp"

#include <vector>
#include <string>

using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
// find the index of first occurance of item t (of type T) in vector<T> v;
// i.e. v[index]=t  Return -1 if t is not found.
template<class T> int index(const std::vector<T> v, const T& t) 
{
   for(int i=0; i<v.size(); i++) {
      if(v[i] == t) return i;
   }
   return -1;
}

//------------------------------------------------------------------------------------
// Returns 0 on success.  Input and output files should diff without error.
int main(int argc, char *argv[])
{
try {
   bool debug=false;
   bool AllNumeric=false, DumpPos=false, help=false;
   bool DumpAll=false,DumpAllObs=false,DumpAllSat=false,ok;
   int i,j;
   string X,Y,Z,T,rms,pdop,gdop,N,outputFormat=string("%4F %10.3g");
   RinexObsHeader::RinexObsType ot;
   RinexSatID sat;
   string line, word, filename, leftpad=string(""), rightpad=string("");
   vector<string> filenames;
   vector<RinexObsHeader::RinexObsType> otlist;
   vector<RinexSatID> satlist;
   RinexObsHeader header;
   RinexObsData obsdata;

   sat.setfill('0');
   RegisterARLUTExtendedTypes();
   //cout << "Registered Obs types are:\n";
   //for(j=0; j<RinexObsHeader::RegisteredRinexObsTypes.size(); j++)
   //   cout << "ROT[" << j << "] = " << RinexObsHeader::RegisteredRinexObsTypes[j]
   //   << endl;

   // parse command line input
   for(i=1; i<argc; i++) {
      word = string(argv[i]);
      if(debug) cout << "arg = " << word << endl;
      if(word == "pos") DumpPos = true;
      else if(word == "-h" || word == "--help") help = true;
      else if(word == "-f" || word == "-file" || word == "--file") {
         filename = string(argv[++i]);
         filenames.push_back(filename);
      }
      else if(word == "-n" || word == "--num") AllNumeric = true;
      else if(word == "--format") outputFormat = string(argv[++i]);
      else if(word == "-sat" || word == "--sat") {
         sat.fromString(string(argv[++i]));
         if(!sat.isValid()) cout << "Error: input argument " << argv[i]
               << " is not a valid satellite id" << endl;
         else
            satlist.push_back(sat);
      }
      else if(word == "-obs" || word == "--obs") {
         ot = RinexObsHeader::convertObsType(argv[++i]);
         if(RinexObsHeader::convertObsType(ot) == string("UN"))
            cout << "Error: input argument " << argv[i]
               << " is not a valid obs type" << endl;
         else 
            otlist.push_back(ot);
      }
      else {         // try to figure out what it is...
         if(debug) cout << " try making it a RINEX obs file: " << word << endl;
         if(isRinexObsFile(word)) {
            filenames.push_back(word);
            continue;
         }

         if(debug) cout << " try making it an obs type: " << word << endl;
         ot = RinexObsHeader::convertObsType(argv[i]);
         if(RinexObsHeader::convertObsType(ot) != string("UN")) {
            otlist.push_back(ot);
            continue;
         }

         if(debug) cout << " try making it a sat : " << word << endl;
         try {
            sat.fromString(word);
            if(sat.isValid()) {
               satlist.push_back(sat);
               continue;
            }
         }
         catch(Exception& e) { ; }

         cout << "Unknown argument, ignore: " << word << endl;
      }
   }

   if(argc < 2 || help) {
      cout << 
"Read RINEX file(s) and dump the given observation types in columns.\n"
"Output is to the screen, with one time tag and one satellite per line.\n"
" Usage: RinexDump [options] file obs sat [pos]\n"
"  If no satellites are given, all are output; likewise for observation types.\n"
"  Output begins with header lines (starting with #) identifying input and columns.\n"
" Options are:\n"
"    pos           output only positions from aux headers; sat and obs are ignored.\n"
"    --num or -n   make output purely numeric (no header, no system char on sats)\n"
"    --format <f>  output times in (DayTime) format (default " << outputFormat << ")\n"
"    --file <file> file is a RINEX observation file; this option may be repreated.\n"
"    --obs <obs>   obs is a RINEX observation type (e.g. P1) found in the file header.\n"
"    --sat <sat>   sat is a RINEX satellite id (e.g. G31 for GPS PRN 31)\n"
"      [--file, --obs and --sat are optional but may be needed to remove ambiguity.]\n"
"    --help or -h  print this and quit.\n"
" E.g. RinexDump test2820.06o L1 L2 G17\n";
      return -1;
   }

   if(otlist.size() == 0) DumpAllObs = true;
   if(satlist.size() == 0) DumpAllSat = true;
   if(DumpAllObs && DumpAllSat) DumpAll = true;

   vector<RinexObsHeader::RinexObsType>::iterator it;

   if(filenames.size() == 0) cerr << "Error - no file names specified.\n";
   if(debug || filenames.size() == 0) {
      cout << "RinexDump read the following from the command line:\n";
      for(i=0; i<filenames.size(); i++)
         cout << " File: " << filenames[i] << endl;
      cout << " Observation types:";
      if(otlist.size() == 0) cout << " all";
      else for(i=0; i<otlist.size(); i++)
         cout << " " << RinexObsHeader::convertObsType(otlist[i]);
      cout << endl;
      cout << " Satellites:";
      if(satlist.size() == 0) cout << " all";
      else for(i=0; i<satlist.size(); i++) cout << " " << satlist[i];
      cout << endl;
   }

   if(filenames.size() == 0) return -1;

   // sort the file names on the begin time in the header
   if(filenames.size() > 1) sortRinexObsFiles(filenames);

   // loop over input files
   for(int nfile=0; nfile < filenames.size(); nfile++) {
      filename = filenames[nfile];

      // does the file exist?
      RinexObsStream RinFile(filename.c_str());
      if(filename.empty() || !RinFile) {
         cerr << "Error: input file " << filename << " does not exist.\n";
         continue; //return -1;
      }
      RinFile.exceptions(fstream::failbit);

      // is it a Rinex Obs file? ... read the header
      try { RinFile >> header; }
      catch(Exception& e) {
         cerr << "Error: input file " << filename << " is not a Rinex obs file\n";
         continue; //return -2;
      }
      //cout << "Rinex header:\n";
      //header.dump(cout);

      // check that obs types are in header
      for(it=otlist.begin(); it !=otlist.end(); ) {
         ok = false;
         for(j=0; j<header.obsTypeList.size(); j++) {
            if(*it == header.obsTypeList[j]) { ok = true; break; }
         }
         if(!ok) {
            cout << "Warning: " << *it << " not found in header of file "
               << filename << endl;
            it = otlist.erase(it);
         }
         else it++;
      }
   
      if(DumpAllObs) {
         otlist.clear();
         for(j=0; j<header.obsTypeList.size(); j++)
            otlist.push_back(header.obsTypeList[j]);
      }

      // echo input
      if(!AllNumeric) {
         cout << "# Rinexdump File: " << filename;
         if(DumpPos) cout << " Positions (in auxiliary header comments)";
         else {
            cout << "   Satellites:";
            if(satlist.size() > 0)
               for(j=0; j<satlist.size(); j++) { cout << " " << satlist[j]; }
            else cout << " ALL";
            cout << "   Observations:";
            if(!DumpAllObs) for(j=0; j<otlist.size(); j++)
               cout << " " << RinexObsHeader::convertObsType(otlist[j]);
            else cout << " ALL";
         }
         cout << endl;
      }

      if(otlist.size() == 0) {
         cout << " Nothing to do.\n";
         continue; //return -1;
      }

      // dump the column headers
      if(!AllNumeric) {
         // figure out widths
         DayTime Now;
         string ts;
         ts = "# Time (" + outputFormat + ")";
         int n = ts.size() - Now.printf(outputFormat).size();
         if(n < 0) rightpad = leftJustify(string(""),-n-1);
         else leftpad = leftJustify(string(""),n);
         cout << ts;
         
         if(DumpPos) cout << " NSVs        X(m)          Y(m)          Z(m)"
               << "        Clk(m)   PDOP  GDOP   RMS(m)";
         else {
            cout << " Sat";
            for(j=0; j<otlist.size(); j++) cout << "            "
               << RinexObsHeader::convertObsType(otlist[j]) << " L S";
         }
         cout << endl;
      }
   
      cout << fixed;
      while(RinFile >> obsdata) {
         RinexObsData::RinexSatMap::const_iterator it;
         RinexObsData::RinexObsTypeMap::const_iterator jt;

         // if dumping regular data, skip auxiliary header, etc
         if(!DumpPos && obsdata.epochFlag != 0 && obsdata.epochFlag != 1)
            continue;

         // dump position data
         if(DumpPos && obsdata.epochFlag == 4) {
            // loop over comments in the header data
            X=Y=Z=T=pdop=gdop=rms=N=string();
            for(j=0,i=0; i<obsdata.auxHeader.commentList.size(); i++) {
               line = stripTrailing(obsdata.auxHeader.commentList[i],
                                    string("COMMENT"),1);
               word = stripFirstWord(line);
               if(word == "XYZT") {
                  X = stripFirstWord(line);
                  Y = stripFirstWord(line);
                  Z = stripFirstWord(line);
                  T = stripFirstWord(line);
                  j++;
               }
               else if(word == "DIAG") {
                  N = stripFirstWord(line);
                  pdop = stripFirstWord(line);
                  gdop = stripFirstWord(line);
                  rms = stripFirstWord(line);
                  j++;
               }
               else { // ignore
               }
            }
   
            // print it
            if(j==2) cout << leftpad << obsdata.time.printf(outputFormat) << rightpad
               << setw(4) << N
               << setprecision(3)
               << " " << setw(13) << X
               << " " << setw(13) << Y
               << " " << setw(13) << Z
               << " " << setw(13) << T
               << " " << setw(5) << pdop
               << " " << setw(5) << gdop
               << " " << setw(9) << rms
               << endl;

         } // end if dumping position data

         if(DumpPos) continue;

         // loop over satellites
         for(it=obsdata.obs.begin(); it != obsdata.obs.end(); ++it) {
            // convert to RinexSatID to get the fill character
            RinexSatID sat = it->first;
            if(!DumpAll && !DumpAllSat && index(satlist, sat) == -1) continue;
            // loop over obs
            ok = false;            // set true only when data exists to output
            for(j=0; j<otlist.size(); j++) {
               if(!ok) {       // output a line
                  // time tag
                  cout << leftpad << obsdata.time.printf(outputFormat) << rightpad;
                  // satellite
                  cout << " ";
                  if(AllNumeric)
                     cout << setw(3) << sat.id;
                  else
                     cout << sat;
                  ok = true;
               }
               if((jt=it->second.find(otlist[j])) == it->second.end()) {
                  cout << " " << setw(13) << setprecision(3)
                     << 0.0 << " " << 0 << " " << 0;
               }
               else {
                  cout << " " << setw(13) << setprecision(3) << jt->second.data
                     << " " << jt->second.lli << " " << jt->second.ssi;
               }
            }
            if(ok) cout << endl;
         } // end loop over satellites
      } // end loop over obs data in file

      RinFile.close();
   } // end loop over input files

   return 0;
}
catch(FFStreamError& e) { cout << e; }
catch(Exception& e) { cout << e; }
catch (...) { cout << "unknown error.  Done." << endl; }
   return -1;
} // main()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
