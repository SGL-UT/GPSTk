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
//============================================================================

/**
 * @file rinex_nav_test.cpp
 * tests gpslib::RinexNavStream, gpslib::RinexNavData, gpslib::RinexNavHeader.
 */

//#include "RinexNavBase.hpp"
#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"
#include "RinexMetStream.hpp"

using namespace std;

/// Returns 0 when successful.
main(int argc, char *argv[])
{
   if (argc<2)
   {
      cout << "Gimme a rinex nav to chew on!  Exiting." << endl;
      exit(-1);
   }

   try
   {
      cout << "Reading " << argv[1] << "." << endl;

      gpstk::RinexNavStream rnffs(argv[1]);
      gpstk::RinexNavHeader rnh;
      gpstk::RinexNavData rne;

      rnffs.exceptions(fstream::failbit);
      
      rnffs >> rnh;
      rnh.dump(cout);

      int i = 0;
      while (rnffs >> rne)
      {
         i++;
      }

      cout << "Read " << i << " records.  Done."  << endl;
      exit(0);
   }
   catch(gpstk::Exception& e)
   {
      cout << e;
      exit(1);
   }
   catch (...)
   {
      cout << "unknown error.  Done." << endl;
      exit(1);
   }

   exit(0);
} // main()
