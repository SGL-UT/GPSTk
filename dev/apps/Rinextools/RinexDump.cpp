#pragma ident "$Id$"

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

//------------------------------------------------------------------------------------
//lgpl-license START
//lgpl-license END

//------------------------------------------------------------------------------------
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "DayTime.hpp"
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

   if(argc < 2 || string(argv[1]) == "--help" || string(argv[1]) == "-h") {
      cout << "Read a Rinex file and dump the observation data\n"
         "    for the given satellite(s).\n"
         " Usage: RinexDump [-n] <file> [<satellite(s)> <obstype(s)> | pos]\n"
         "    Output is to the screen with one line per satellite/epoch,\n"
         "    columns <week> <sow> <sat> <obs LLI SSI> for each obs type\n"
         "    (1st line echos input, 2nd is column labels).\n"
         "    If pos appears, position info from aux. headers in dumped.\n"
         "    If <satellite> and/or <obstype> and pos are missing, all obs are dumped.\n"
         "    -n   make output purely numeric.\n";
      return -1;
   }

   bool AllNumeric = false;
   bool DumpPos = false;
   int i,j0=1;
   for(i=0; i<argc; i++) {
      if(string(argv[i]) == "-n") { j0++; AllNumeric = true; }
      //if(string(argv[i]) == "-pos") { j0++; DumpPos = true; }
   }

try {
   int j;
   bool DumpAll=(argc-j0+1==2),DumpAllObs=false,DumpAllSat=false,lineout;
   string X,Y,Z,T,rms,pdop,gdop,N;
   RinexObsHeader::RinexObsType ot;
   RinexPrn sat;
   string line, word;
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
         if(string(argv[j]) == "pos") {
            DumpPos = true;
            break;
         }
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
   cout << "# Rinexdump File: " << argv[j0] << "  Data:";
   if(DumpPos) {
      cout << " Positions (in auxiliary header comments)";
   }
   else {
      cout << " Satellites:";
      if(satlist.size()>0) for(j=0; j<satlist.size(); j++) {
         cout << " " << satlist[j];
      }
      else cout << " ALL";
      cout << " Observations:";
      if(!DumpAllObs) for(j=0; j<otlist.size(); j++)
         cout << " " << RinexObsHeader::convertObsType(otlist[j]);
      else cout << " ALL";
   }
   cout << endl;

   // dump the column headers
   cout << "# Week  GPS_sow";
   if(DumpPos)
      cout << " NSVs        X (m)         Y (m)         Z (m)       Clk (m)"
            << "  PDOP  GDOP   RMS (m)";
   else {
      cout << " Sat";
      for(j=0; j<otlist.size(); j++) cout << "            "
         << RinexObsHeader::convertObsType(otlist[j]) << " L S";
   }
   cout << endl;

   cout << fixed;
   while(RinFile >> obsdata)
   {
      RinexObsData::RinexPrnMap::const_iterator it;
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
            << setw(14) << X
            << setw(14) << Y
            << setw(14) << Z
            << setw(14) << T
            << setw(6) << pdop
            << setw(6) << gdop
            << setw(10) << rms
            << endl;
      }

      if(DumpPos) continue;

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
