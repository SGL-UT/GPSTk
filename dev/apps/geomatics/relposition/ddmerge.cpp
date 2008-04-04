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

/**
 * @file ddmerge.cpp
 * merge two DDBase output data files: DDR and RAW, and write a file just like the
 * input DDR file but with Az and El appended to each line for each site/sat combo.
 */

//------------------------------------------------------------------------------------
// system includes
#include <time.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>

// GPSTk
#include "DayTime.hpp"
#include "StringUtils.hpp"

//------------------------------------------------------------------------------------
using namespace std;
using namespace gpstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
string Version("1.0 7/5/06");
ifstream *filepointer;
const unsigned BUFF_SIZE=1024;
char buffer[BUFF_SIZE];

//------------------------------------------------------------------------------------
class Chunk {
public:
   //string id;        // 'site sat'
   string line;      // current line
   long filepos;     // current filepos = start of next line
   int fpindex;      // index in filepointer array
   bool status;      // true if good
   string Update(void) {         // return current line after updating to next
      string tmp;
      if(!status) return tmp;
      try {
         filepointer[fpindex].getline(buffer,BUFF_SIZE);
      }
      catch(exception& e) {
         cout << "std exception: " << e.what() << endl;
         status = false;
      }
      if(filepointer[fpindex].bad()) {
         cout << "Read error" << endl;
         status = false;
      }
      if(filepointer[fpindex].eof()) {
         cout << "Reached EOF" << endl;
         status = false;
      }
      if(status) {
         tmp = line;
         line = string(buffer);
         stripTrailing(line,'\r');
         filepos = filepointer[fpindex].tellg();
      }
      //else filepointer[fpindex].close();
      
      return tmp;
   }
   string find(int n) {  // update until count in line matches n, return line
      int count;
      string tmp;
      do {
         //if(!status) break;
         tmp = word(line,5);     // TD will be 4 if MJD is output
         count = asInt(tmp);
         if(count >= n) break;
         Update();
      } while(1);
      return line;
   }
};
map<string,Chunk> Chunklist;        // key is 'site sat'

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   int i,n;
   string line,line2,id,rawfile,ddrfile,outfile,site1,site2,sat1,sat2;
   ifstream instr;
   ofstream outstr;
   DayTime CurrEpoch;

   clock_t totaltime = clock();
      // print title and current time to screen
   CurrEpoch.setLocalTime();
   cout << "ddmerge version " << Version << " run " << CurrEpoch << endl;

   if(argc < 4) {
      cout << "Usage: ddmerge <RAWfile> <DDRfile> <output_file>" << endl;
      cout << "    where the two input file are output of DDBase" << endl;
      cout << " ddmerge will take elevation and azimuth data from the RAW" << endl;
      cout << " file and append it to the appropriate line in the DDR file" << endl;
      cout << " and output to the output file" << endl;
      return -1;
   }
   rawfile = string(argv[1]);
   ddrfile = string(argv[2]);
   outfile = string(argv[3]);

      // this must be binary or you get the wrong answers.
   instr.open(rawfile.c_str(),ios::in|ios::binary);
   if(!instr) {
      cout << "Failed to open input file " << rawfile << endl;
      return -1;
   }
   cout << "Opened input file " << rawfile << endl;
   instr.exceptions(fstream::failbit);

   n = 0;
   while(1) {
      try {
         //instr.read((char *)p2, 1); // get one char
         instr.getline(buffer,BUFF_SIZE);
      }
      catch(exception& e) {}
      if(instr.bad()) cout << "Read error" << endl;
      if(instr.eof()) { cout << "Reached EOF" << endl; break; }

      n++;
      line = string(buffer);
      stripTrailing(line,'\r');
      if(word(line,0) == "RAW") {
         id = word(line,1);
         if(id != "site") {
            id += " " + word(line,2);
            if(Chunklist.find(id) == Chunklist.end()) {
               Chunk newchunk;
               newchunk.filepos = instr.tellg();
               newchunk.line = line;
               Chunklist[id] = newchunk;
            }
         }
      }
   }

   instr.clear();
   instr.close();

   // you must use pointers to the streams because storing a stream inside an object
   // that goes into an STL container leads to weird errors...try it.
   filepointer = new ifstream[Chunklist.size()];
   if(!filepointer) { cout << "failed to allocate filepointers" << endl; return -1; }

   map<string,Chunk>::iterator it;
   for(i=0,it=Chunklist.begin(); it != Chunklist.end(); i++,it++) {
      filepointer[i].open(rawfile.c_str(), ios::in|ios::binary);
      if(!filepointer[i]) {
         cout << "Failed to open chunk " << i << endl;
         break;
      }
      filepointer[i].exceptions(fstream::failbit);
      it->second.fpindex = i;
      filepointer[i].seekg(it->second.filepos);
   }

   outstr.open(outfile.c_str(), ios::out);
   if(!outstr) {
      cout << "Failed to open output file " << outfile << endl;
      return -1;
   }
   cout << "Opened output file " << outfile << endl;
   outstr.exceptions(fstream::failbit);

   //for(it=Chunklist.begin(); it != Chunklist.end(); it++) {
   //   if(! it->second.status) continue;
   //   outstr << "Chunk " << it->first << endl;
   //   outstr << it->second.Update() << endl;
   //   outstr << it->second.Update() << endl;
   //   outstr << endl;
   //}

   instr.open(ddrfile.c_str());
   if(!instr) {
      cout << "Failed to open input file " << ddrfile << endl;
      return -1;
   }
   cout << "Opened input file " << ddrfile << endl;
   instr.exceptions(fstream::failbit);

   n = 0;
   while(1) {
      try { instr.getline(buffer,BUFF_SIZE); }
      catch(exception& e) {} //cout << "exception: " << e.what() << endl;
      if(instr.bad()) { cout << "Read error" << endl; break; }
      if(instr.eof()) { cout << "Reached EOF" << endl; break; }

      n++;
      line = string(buffer);
      stripTrailing(line,'\r');
      if(word(line,0) == "RES") {
         site1 = word(line,1);
         if(site1 != "site") {
            site2 = word(line,2);
            sat1 = word(line,3);
            sat2 = word(line,4);
            id = word(line,7);        // TD different for MJD
            n = asInt(id);
            outstr << line;                          // endl below
            // find the corresponding lines in the chunks
            id = site1 + " " + sat1;
            line2 = Chunklist[id].find(n);
            if(Chunklist[id].status) outstr
               << " " << rightJustify(word(line2,11),5)
               << " " << rightJustify(word(line2,12),6);
            id = site1 + " " + sat2;
            line2 = Chunklist[id].find(n);
            if(Chunklist[id].status) outstr
               << " " << rightJustify(word(line2,11),5)
               << " " << rightJustify(word(line2,12),6);
            id = site2 + " " + sat1;
            line2 = Chunklist[id].find(n);
            if(Chunklist[id].status) outstr
               << " " << rightJustify(word(line2,11),5)
               << " " << rightJustify(word(line2,12),6);
            id = site2 + " " + sat2;
            line2 = Chunklist[id].find(n);
            if(Chunklist[id].status) outstr
               << " " << rightJustify(word(line2,11),5)
               << " " << rightJustify(word(line2,12),6);
         }
         else outstr
            << line << "  EL11   AZ11  EL12   AZ12  EL21   AZ21  EL22   AZ22";
      }
      else outstr << line << endl
         << "# ddmerge (v." << Version << ") " << rawfile
         << " " << ddrfile << " " << outfile << " Run " << CurrEpoch;
      outstr << endl;
   }

   instr.close();
   delete[] filepointer;

      // compute run time
   totaltime = clock()-totaltime;
   cout << "ddmerge timing: " << fixed << setprecision(3)
      << double(totaltime)/double(CLOCKS_PER_SEC) << " seconds." << endl;

   return 0;
}
catch(Exception& e) {
   cout << "GPSTk Exception : " << e;
}
catch (...) {
   cout << "Unknown error in ddmerge.  Abort." << endl;
}

      // close files
   return -1;
}   // end main()

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
