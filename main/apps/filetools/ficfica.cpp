#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/filetools/ficfica.cpp#1 $"

/**
 * @file ficfica.cpp Convert FIC files to FICA.
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
#include "FICAStream.hpp"
#include "FICHeader.hpp"
#include "FICData.hpp"
#include "FileFilterFrame.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{
   if (argc != 3)
   {
      cout << "FICFICA" << endl
           << "  converts a binary FIC file to an ASCII FIC file" << endl
           << endl
           << "usage:" << endl
           << "    ficfica inputfile outputfile" << endl
           << endl
           << "where:" << endl
           << "    inputfile: an input binary FIC file name" << endl
           << "    outputfile: an output ASCII FIC file name" << endl;
      return 0;
   }

   FICStream fics(argv[1]);
   FICHeader header;
   header.getRecord(fics);
   fics.close();

   FICAStream out(argv[2], ios::out);
   out << header;
   out.close();

   FileFilterFrame<FICStream, FICData> input(argv[1]);
   FileFilterFrame<FICAStream, FICData> output;
   list<FICData> alist = input.getData();
   output.addData(alist);
   output.writeFile(argv[2], true);

   return 0;
}
