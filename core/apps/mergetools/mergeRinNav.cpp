//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
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

#include "RinexNavStream.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"
#include "RinexNavFilterOperators.hpp"
#include "FileFilterFrameWithHeader.hpp"
#include "SystemTime.hpp"
#include "CivilTime.hpp"

#include "MergeFrame.hpp"

using namespace std;
using namespace gpstk;

class MergeRinNav : public MergeFrame
{
public:
   MergeRinNav(char* arg0)
      : MergeFrame(arg0, 
                   std::string("RINEX Nav"),
                   std::string("Only unique nav subframes will be output and they will be sorted by time."))
   {}

protected:
   virtual void process();
};

void MergeRinNav::process()
{
   std::vector<std::string> files = inputFileOption.getValue();

      // FFF will sort and merge the obs data using
      // a simple time check
   FileFilterFrameWithHeader<RinexNavStream, RinexNavData, RinexNavHeader> 
      fff(files);

      // get the header data
   RinexNavHeaderTouchHeaderMerge merged;
   fff.touchHeader(merged);

      // sort and filter the data
   fff.sort(RinexNavDataOperatorLessThanFull());
   fff.unique(RinexNavDataOperatorEqualsFull());
   
      // set the pgm/runby/date field
   merged.theHeader.fileType = string("NAVIGATION");
   merged.theHeader.fileProgram = std::string("mergeRinNav");
   merged.theHeader.fileAgency = std::string("gpstk");
   merged.theHeader.date = CivilTime(SystemTime()).asString();
   merged.theHeader.version = 2.1;
   merged.theHeader.valid |= gpstk::RinexNavHeader::versionValid;
   merged.theHeader.valid |= gpstk::RinexNavHeader::runByValid;
   merged.theHeader.valid |= gpstk::RinexNavHeader::commentValid;
   merged.theHeader.valid |= gpstk::RinexNavHeader::endValid;

      // write the header
   std::string outputFile = outputFileOption.getValue().front();
   fff.writeFile(outputFile, merged.theHeader);
}

int main(int argc, char* argv[])
{
   try
   {
      MergeRinNav m(argv[0]);
      if (!m.initialize(argc, argv))
         return m.exitCode;
      if (!m.run())
         return m.exitCode;
      
      return m.exitCode;
   }
   catch(Exception& e)
   {
      cout << e << endl;
   }
   catch(std::exception& e)
   {
      cout << e.what() << endl;
   }
   catch(...)
   {
      cout << "unknown error" << endl;
   }
      // only reach this point if an exception was caught
   return BasicFramework::EXCEPTION_ERROR;
}
