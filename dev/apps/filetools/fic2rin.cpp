#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/filetools/fic2rin.cpp#2 $"

/**
 * @file fic2rin.cpp Convert FIC files to RINEX.
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






#include "FICStream.hpp"
#include "FICHeader.hpp"
#include "FICData.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"
#include "FICFilterOperators.hpp"
#include "RinexNavFilterOperators.hpp"
#include "FileFilterFrame.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{
   if (argc != 3)
   {
      cout << "fic2rin" << endl
           << "  converts a binary FIC file to a Rinex Nav file" << endl
           << endl
           << "usage:" << endl
           << "    fic2rin inputfile outputfile" << endl
           << endl
           << "where:" << endl
           << "    inputfile: an input binary FIC file name" << endl
           << "    outputfile: an output Rinex Nav file name" << endl;
      return 0;
   }
   // What is up
   FileFilterFrame<FICStream, FICData> input(argv[1]);
   list<FICData> alist = input.getData();
   
      // write the header info
   RinexNavStream rns(argv[2], ios::out|ios::trunc);
   RinexNavHeader rnh;
   rnh.fileType = "Navigation";
   rnh.fileProgram = "fic2rin";
   rnh.fileAgency = "";
   ostringstream ostr;
   ostr << DayTime();
   rnh.date = ostr.str();
   rnh.version = 2.1;
   rnh.valid |= RinexNavHeader::versionValid;
   rnh.valid |= RinexNavHeader::runByValid;
   rnh.valid |= RinexNavHeader::endValid;
   rns.header = rnh;
   rnh.putRecord(rns);
   rns.close();

      // filter the FIC data for block 9
   list<long> blockList;
   blockList.push_back(9);
   input.filter(FICDataFilterBlock(blockList));
   input.sort(FICDataOperatorLessThanBlock9());
   input.unique(FICDataUniqueBlock9());

      // some hand waving for the data conversion
   list<RinexNavData> rndList;
   list<FICData>& ficList = input.getData();
   list<FICData>::iterator itr = ficList.begin();
   while (itr != ficList.end())
   {
         // use TOE and transmit week number to determine time
      DayTime time;
      time.setGPSfullweek(short((*itr).f[5]), (double)(*itr).f[33]);
         // this station number is bogus, but it's unused so it should be ok
      EngEphemeris ee(*itr);
      rndList.push_back(RinexNavData(ee));
      itr++;
   }

      // write the file data
   FileFilterFrame<RinexNavStream, RinexNavData> output;
   output.addData(rndList);
   output.sort(RinexNavDataOperatorLessThanFull());
   output.writeFile(argv[2], true);

   return 0;
}
