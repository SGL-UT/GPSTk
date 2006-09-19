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
//#include <time.h>

using namespace std;
using namespace gpstk;

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
   //clock_t totaltime=clock();

try {
   bool AllNumeric=false, DumpPos=false, help=false;
   bool DumpAll=false,DumpAllObs=false,DumpAllSat=false,ok;
   int i,j;
   string X,Y,Z,T,rms,pdop,gdop,N;
   RinexObsHeader::RinexObsType ot;
   RinexSatID sat;
   string line, word, filename;
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
      else if(word == "-n") AllNumeric = true;
      else if(word == "-sat") {
         sat.fromString(string(argv[++i]));
         if(!sat.isValid()) cout << "Error: input argument " << argv[i]
               << " is not a valid satellite id" << endl;
         else
            satlist.push_back(sat);
      }
      else if(word == "-obs") {
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
      cout << "Read a Rinex file and dump the observation data\n"
         "    for the given satellite(s).\n"
         " Usage: RinexDump [-file] <file> {<input>} [-n]\n"
         "     <input> is {[-sat] <satellite(s)> [-obs] <obstype(s)> | pos}\n"
         "    Output is to the screen with one line per satellite/epoch\n"
         //"     with columns <week> <sow> <sat> <obs LLI SSI> for each obs type\n"
         "    If pos appears, dump only position info from auxiliary headers.\n"
         //"     with columns <week> <sow> <Nsat> <X Y Z Clk PDOP GDOP RMS>\n"
         "    If all <input> is missing, all obs from all sats are dumped.\n"
         "    -n   make output purely numeric.\n"
         "    -obs and -sat are optional but may remove ambiguity (e.g. S8)\n";
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
   cout << "# Rinexdump File: " << filename << "  Data:";
   if(DumpPos) {
      cout << " Positions (in auxiliary header comments)";
   }
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

   if(otlist.size() == 0) {
      cout << " Nothing to do.\n";
      return -1;
   }

   // dump the column headers
   cout << "# Week  GPS_sow";
   if(DumpPos)
      cout << " NSVs        X_(m)         Y_(m)         Z_(m)       Clk_(m)"
            << "  PDOP  GDOP   RMS_(m)";
   else {
      cout << " Sat";
      for(j=0; j<otlist.size(); j++) cout << "            "
         << RinexObsHeader::convertObsType(otlist[j]) << " L S";
   }
   cout << endl;

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
            line = StringUtils::stripTrailing(obsdata.auxHeader.commentList[i],
                                 string("COMMENT"),1);
            word = StringUtils::stripFirstWord(line);
            if(word == "XYZT") {
               X = StringUtils::stripFirstWord(line);
               Y = StringUtils::stripFirstWord(line);
               Z = StringUtils::stripFirstWord(line);
               T = StringUtils::stripFirstWord(line);
               j++;
            }
            else if(word == "DIAG") {
               N = StringUtils::stripFirstWord(line);
               pdop = StringUtils::stripFirstWord(line);
               gdop = StringUtils::stripFirstWord(line);
               rms = StringUtils::stripFirstWord(line);
               j++;
            }
            else { // ignore
            }
         }

         // print it
         if(j==2) cout << setw(4) << obsdata.time.GPSfullweek()
            << setw(11) << setprecision(3) << obsdata.time.GPSsecond()
            << setw(4) << N
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
                  cout << setw(4) << obsdata.time.GPSfullweek()
                     << setw(11) << setprecision(3) << obsdata.time.GPSsecond();
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

   //totaltime = clock()-totaltime;
   //cerr << "RinexDump timing: " << setprecision(3)
      //<< double(totaltime)/double(CLOCKS_PER_SEC) << " seconds.\n";

   return 0;
}
catch(gpstk::FFStreamError& e)
{
   cout << e;
   return 1;
}
catch(gpstk::Exception& e)
{
   cout << e;
   return 1;
}
catch (...)
{
   cout << "unknown error.  Done." << endl;
   return 1;
}
   return -1;
} // main()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
