#pragma ident "$Id$"


// mergeFIC
// Merge and sort FIC files

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
#include "FICData.hpp"
#include "FICFilterOperators.hpp"
#include "FileFilterFrameWithHeader.hpp"

#include "MergeFrame.hpp"

using namespace std;
using namespace gpstk;

class MergeFIC : public MergeFrame
{
public:
   MergeFIC(char* arg0) : 
         MergeFrame(arg0, 
                    std::string("FIC"),
                    std::string("No filtering is performed on FIC merges.  The resulting file may have some duplicate data."))
      {}   

protected:
   virtual void process();
};

void MergeFIC::process()
{
   std::vector<std::string> files = inputFileOption.getValue();

      // FFF will sort and merge the FIC data using a simple time check
   FileFilterFrameWithHeader<FICStream, FICData, FICHeader> 
      fff(files);

      // arbitrarily take the first FIC header as the header for the merged 
      // file
   std::string outputFile = outputFileOption.getValue().front();
   fff.writeFile(outputFile, fff.frontHeader());
}

int main(int argc, char* argv[])
{
   try
   {
      MergeFIC m(argv[0]);
      if (!m.initialize(argc, argv))
         return 0;
      if (!m.run())
         return 1;
      
      return 0;
   }
   catch(gpstk::Exception& e)
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
