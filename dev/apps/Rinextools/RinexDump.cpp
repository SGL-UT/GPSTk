//------------------------------------------------------------------------------------
// RinexDump.cpp Read a RINEX file and dump the observation data for the given
// satellite(s), one satellite per file.
// Input is on the command line, of the form
//    RinexDump <file> <satellite> <obstype(s)>
// Any number of obstypes may appear; if none appear, all are dumped.
// One satellite ID (e.g. G27) may appear; if none appears, all satellites are
// dumped (lots of output!).
// The output file(s) are ASCII column-delimited with week and seconds-of-week
// in the first two columns, followed by 'observation LLI SSI' for each
// observation type. The name of the output file(s) is of the form RDump<sat>.dat
//
// RinexDump is part of the GPS Tool Kit (GPSTK) developed in the
// Satellite Geophysics Group at Applied Research Laboratories,
// The University of Texas at Austin (ARL:UT), and was written by Dr. Brian Tolman.
//
//------------------------------------------------------------------------------------
#pragma ident "$Id$"


/**
 * @file RinexDump.cpp
 * Dump Rinex observation data to a flat file.
 */

//------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "DayTime.hpp"

#include "RinexUtilities.hpp"

#include <vector>
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

   if (argc < 2) {
      cout << "Read a Rinex file and dump the observation data\n"
         "    for the given satellite(s).\n"
         " Usage: RinexDump [-n] <file> [<satellite(s)> <obstype(s)>]\n"
         "    Output is to the screen with one line per satellite/epoch,\n"
         "    columns <week> <sow> <sat> <obs LLI SSI> for each obs type\n"
         "    (1st line echos input, 2nd is column labels).\n"
         "    If <satellite> and/or <obstype> are missing, all are dumped.\n"
         "    Option -n make output purely numeric.\n";
      return -1;
   }

   bool AllNumeric = false;
   int j0=1;
   if(string(argv[1]) == "-n") {
      j0 = 2;
      AllNumeric = true;
   }

try {
   int j;
   bool DumpAll=(argc-j0+1==2),DumpAllObs=false,DumpAllSat=false,lineout;
   RinexObsHeader::RinexObsType ot;
   RinexPrn sat;
   vector<RinexObsHeader::RinexObsType> otlist;
   vector<RinexPrn> satlist;
   RinexObsStream RinFile(argv[j0]);
   RinexObsHeader header;
   RinexObsData obsdata;

   sat.setfill('0');
   RegisterARLUTExtendedTypes();
   //cout << "Registered Obs types are:\n";
   //for(j=0; j<RinexObsHeader::RegisteredRinexObsTypes.size(); j++)
   //   cout << "ROT[" << j << "] = " << RinexObsHeader::RegisteredRinexObsTypes[j]
   //   << endl;

   // parse command line input
   if(!DumpAll) {
      for(j=j0+1; j<argc; j++) {
         sat = StringUtils::asData<RinexPrn>(string(argv[j]));
         ot = RinexObsHeader::convertObsType(argv[j]);
         if(RinexObsHeader::convertObsType(ot) == string("UN")) {
            if(sat.prn == -1) {
               cout << "Error: input argument " << argv[j]
                  << " is not recognized as either satellite or observation type\n";
               continue;
            }
            else { satlist.push_back(sat); }
         }
         else { otlist.push_back(ot); }
      }

      if(otlist.size() == 0) DumpAllObs=true;
      if(satlist.size() == 0) DumpAllSat=true;
   }
   else DumpAllObs=DumpAllSat=true;

   // does the file exist?
   if(!RinFile) {
      cerr << "Error: input file " << argv[j0] << " does not exist.\n";
      return -1;
   }
   RinFile.exceptions(fstream::failbit);

   // is it a Rinex Obs file? ... read the header
   try { RinFile >> header; } catch(gpstk::Exception& e) {
      cerr << "Error: input file " << argv[j0] << " is not a Rinex obs file\n";
      return -2;
   }
   //cout << "Rinex header:\n";
   //header.dump(cout);
   
   if(DumpAllObs) for(j=0; j<header.obsTypeList.size(); j++)
      otlist.push_back(header.obsTypeList[j]);

   // echo input
   cout << "# Rinexdump file: " << argv[j0];
   cout << " Satellites:";
   if(satlist.size()>0) for(j=0; j<satlist.size(); j++) {
      cout << " " << satlist[j];
   }
   else cout << " ALL";
   cout << " Observations:";
   if(!DumpAllObs) for(j=0; j<otlist.size(); j++)
      cout << " " << RinexObsHeader::convertObsType(otlist[j]);
   else cout << " ALL";
   cout << endl;

   // dump the column headers
   cout << "# Week  GPS_sow Sat";
   for(j=0; j<otlist.size(); j++) {
      cout << "            " << RinexObsHeader::convertObsType(otlist[j]) << " L S";
   }
   cout << endl;

   cout << fixed;
   while(RinFile >> obsdata)
   {
      RinexObsData::RinexPrnMap::const_iterator it;
      RinexObsData::RinexObsTypeMap::const_iterator jt;
      if(obsdata.epochFlag != 0 && obsdata.epochFlag != 1)        // not regular data
         continue;
      //else if(obsdata.epochFlag > 1 && obsdata.epochFlag < 6) {    // header records
      //   obsdata.auxHeader.dump(cout);
      //}

      // loop over satellites
      for(it=obsdata.obs.begin(); it != obsdata.obs.end(); ++it) {
         if(!DumpAll && !DumpAllSat && index(satlist, it->first) == -1) continue;
         // loop over obs
         lineout = false;            // set true only when data exists to output
         for(j=0; j<otlist.size(); j++) {
            if((jt=it->second.find(otlist[j])) == it->second.end()) {
               cout << " " << setw(13) << setprecision(3)
                  << 0.0 << " " << 0 << " " << 0;
            }
            else {
               if(!lineout) {       // output a line
                  // time tag
                  cout << setw(4) << obsdata.time.GPSfullweek()
                     << setw(11) << setprecision(3) << obsdata.time.GPSsecond();
                  // satellite
                  cout << " ";
                  if(AllNumeric)
                     cout << setw(3) << it->first.prn;
                  else
                     cout << it->first;
                  lineout = true;
               }
               cout << " " << setw(13) << setprecision(3) << jt->second.data
                  << " " << jt->second.lli << " " << jt->second.ssi;
            }
         }
         if(lineout) cout << endl;
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
