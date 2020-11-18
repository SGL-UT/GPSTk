//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#include "SinexStream.hpp"
#include "SinexData.hpp"


   /**
    * @file sinex_test.cpp
    * Tests gptk::SinexStream, gpstk::SinexData, gpstk::SinexHeader
    * by reading in a SINEX file and then writing it back out.
    * Sample input files can be found in the data directory.
    * The output file is named "sinex_test.out"
    */

using namespace std;
using namespace gpstk;

   /**
    * Run the test - read in a SINEX file and write it back out.
    *
    * @return 0 if successful
    */
int main(int argc, char *argv[])
{
   if (argc < 2)
   {
      cerr << "Can has Sinex file pls?  Exiting." << endl;
      exit(-1);
   }

   try
   {
      Sinex::Data  data;

      cout << "Reading " << argv[1] << ". . ." << endl;
      Sinex::Stream  input(argv[1]);
      input.exceptions(fstream::eofbit | fstream::failbit);
      input >> data;
      cout << "Done." << endl;

      data.dump(cout);

      cout << "Writing data to sinex_test.out . . . " << endl;
      Sinex::Stream  output("sinex_test.out", ios::out | ios::ate);
      output.exceptions(fstream::eofbit | fstream::failbit);
      output << data;
      cout << "Done." << endl;

      exit(0);
   }
   catch(Exception& e)
   {
      cerr << e;
   }
   catch (...)
   {
      cerr << "Unknown error.  Done." << endl;
   }
   exit(1);

} // main()
