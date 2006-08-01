#pragma ident "$Id$"


// mergeRinObs
// Merge and sort rinex observation files

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






#include "RinexObsStream.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "RinexObsFilterOperators.hpp"
#include "FileFilterFrameWithHeader.hpp"

#include "MergeFrame.hpp"

using namespace std;
using namespace gpstk;

class MergeRinObs : public MergeFrame
{
public:
   MergeRinObs(char* arg0)
      : MergeFrame(arg0, 
                   std::string("RINEX Obs"),
                   std::string("The output will be sorted by time. This program assumes all the input files are from the same station."))
   {}

protected:
   virtual void process();
};

void MergeRinObs::process()
{
   std::vector<std::string> files = inputFileOption.getValue();

      // FFF will sort and merge the obs data using
      // a simple time check
   FileFilterFrameWithHeader<RinexObsStream, RinexObsData, RinexObsHeader> 
      fff(files);

      // get the header data
   RinexObsHeaderTouchHeaderMerge merged;
   fff.touchHeader(merged);

      // sort and filter the data using the obs set from the merged header
   fff.sort(RinexObsDataOperatorLessThanFull(merged.obsSet));
   fff.unique(RinexObsDataOperatorEqualsSimple());
   
      // set the time of first obs in the header
   merged.theHeader.firstObs = fff.front().time;

      // set the pgm/runby/date field
   merged.theHeader.fileProgram = std::string("mergeRinObs");
   merged.theHeader.fileAgency = std::string("gpstk");
   merged.theHeader.date = DayTime().asString();

      // write the file
   std::string outputFile = outputFileOption.getValue().front();
   fff.writeFile(outputFile, merged.theHeader);
}

int main(int argc, char* argv[])
{
   try
   {
      MergeRinObs m(argv[0]);
      if (!m.initialize(argc, argv))
         return 0;
      if (!m.run())
         return 1;
      
      return 0;
   }
   catch(Exception& e)
   {
      cout << e << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl;
   }
   catch(...)
   {
      cout << "unknown error" << endl;
   }
   return 1;
}
