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
 * Read a RINEX file and dump the observation data for the given
 * satellite(s), one satellite per file.
 * Input is on the command line, of the form
 *    RinexDump <file> <satellite> <obstype(s)>
 * Any number of obstypes may appear; if none appear, all are dumped.
 * One satellite ID (e.g. G27) may appear; if none appears, all satellites are dumped.
 * The output file(s) are ASCII column-delimited with week and seconds-of-week
 * in the first two columns, followed by 'observation LLI SSI' for each
 * observation type. The name of the output file(s) is of the form RDump<sat>.dat
 */

#include "RinexObsBase.hpp"
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
   bool AllNumeric=false, DumpPos=false, help=false;
   bool DumpAll=false,DumpAllObs=false,DumpAllSat=false,ok;
   int i,j;
   string X,Y,Z,T,rms,pdop,gdop,N,outputFormat=string("%4F %10.3g");
   RinexObsHeader::RinexObsType ot;
   RinexSatID sat;
   string line, word, filename, leftpad=string(""), rightpad=string("");
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
      //cout << "arg = " << word << endl;
      if(word == "pos") DumpPos = true;
      else if(word == "-h" || word == "--help") help = true;
      else if(word == "-file") filename = string(argv[++i]);
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
      else {
         //cout << " try making it an obs type: " << word << endl;
         ot = RinexObsHeader::convertObsType(argv[i]);
         if(RinexObsHeader::convertObsType(ot) != string("UN"))
            otlist.push_back(ot);
         else {
            //cout << " try making it a sat : " << word << endl;
            try {
               sat.fromString(string(argv[i]));
               if(sat.isValid()) { satlist.push_back(sat); continue; }
            }
            catch(Exception& e) { ; }
            //cout << " make it a filename : " << word << endl;
            filename = string(argv[i]);
         }
      }
   }

   if(argc < 2 || help) {
      cout << 
"Read a RINEX file and dump the data for the given observation types and satellites.\n"
"Output is to the screen with one time tag and satellite per line\n"
" Usage: RinexDump [--file] <file> [[--obs] obstype ..] [[--sat] sat ..] [options]\n"
"  --obs and --sat are optional but may be needed to remove ambiguity (e.g. S8).\n"
"  If no satellites are given, all are output; likewise for observation types.\n"
"  Output begins with header lines (starting with #) identifying input and columns.\n"
" Options are:\n"
"    -n or --num   make output purely numeric (no header, no system char on sats)\n"
"    pos           output only positions from aux headers; sat and obs are ignored.\n"
"    --format <f>  output times in (DayTime) format (" << outputFormat << ")\n"
"    -h or --help  print this and quit.\n"
" E.g. RinexDump test2820.06o L1 L2 G17\n";
      return -1;
   }

   if(otlist.size() == 0) DumpAllObs = true;
   if(satlist.size() == 0) DumpAllSat = true;
   if(DumpAllObs && DumpAllSat) DumpAll = true;

   // does the file exist?
   RinexObsStream RinFile(filename.c_str());
   if(filename.empty() || !RinFile) {
      cerr << "Error: input file " << filename << " does not exist.\n";
      return -1;
   }
   RinFile.exceptions(fstream::failbit);

   // is it a Rinex Obs file? ... read the header
   try { RinFile >> header; }
   catch(gpstk::Exception& e) {
      cerr << "Error: input file " << filename << " is not a Rinex obs file\n";
      return -2;
   }
   //cout << "Rinex header:\n";
   //header.dump(cout);

   // check that obs types are in header
   vector<RinexObsHeader::RinexObsType>::iterator it;
   for(it=otlist.begin(); it !=otlist.end(); ) {
      ok = false;
      for(j=0; j<header.obsTypeList.size(); j++) {
         if(*it == header.obsTypeList[j]) { ok = true; break; }
      }
      if(!ok) {
         cout << "Warning: " << *it << " not found in header\n";
         it = otlist.erase(it);
      }
      else it++;
   }
   
   if(DumpAllObs) for(j=0; j<header.obsTypeList.size(); j++)
      otlist.push_back(header.obsTypeList[j]);

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
      return -1;
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
      
      if(DumpPos) cout << " NSVs        X_(m)         Y_(m)         Z_(m)"
            << "       Clk_(m)  PDOP  GDOP   RMS_(m)";
      else {
         cout << " Sat";
         for(j=0; j<otlist.size(); j++) cout << "            "
            << RinexObsHeader::convertObsType(otlist[j]) << " L S";
      }
      cout << endl;
   }

   cout << fixed;
   while(RinFile >> obsdata)
   {
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
      }

      if(DumpPos) continue;

      // loop over satellites
      for(it=obsdata.obs.begin(); it != obsdata.obs.end(); ++it) {
         // convert to RinexSatID to get the fill character
         RinexSatID sat = it->first;
         if(!DumpAll && !DumpAllSat && index(satlist, sat) == -1) continue;
         // loop over obs
         ok = false;            // set true only when data exists to output
         for(j=0; j<otlist.size(); j++) {
            if((jt=it->second.find(otlist[j])) == it->second.end()) {
               cout << " " << setw(13) << setprecision(3)
                  << 0.0 << " " << 0 << " " << 0;
            }
            else {
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
               cout << " " << setw(13) << setprecision(3) << jt->second.data
                  << " " << jt->second.lli << " " << jt->second.ssi;
            }
         }
         if(ok) cout << endl;
      }
   }

   return 0;
}
catch(gpstk::FFStreamError& e) { cout << e; }
catch(gpstk::Exception& e) { cout << e; }
catch (...) { cout << "unknown error.  Done." << endl; }
   return -1;
} // main()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
