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
//  Copyright 2006, The University of Texas at Austin
//
//============================================================================

#include "RinexMetBase.hpp"
#include "RinexMetData.hpp"
#include "RinexMetHeader.hpp"
#include "RinexMetStream.hpp"

/**
 * @file rinex_met_test.cpp
 * Tests gpslib::RinexMetStream, gpslib::RinexMetData, gpslib::RinexMetHeader.
 */

using namespace std;

/// returns 0 if successful.
main(int argc, char *argv[])
{
   if (argc<2)
   {
      cout << "Gimme a rinex obs to chew on!  Exiting." << endl;
      exit(-1);
   }

   try
   {
      cout << "Reading " << argv[1] << "." << endl;

      gpstk::RinexMetStream roffs(argv[1]);

         // reading the header data isn't necessary
//      gpstk::RinexMetHeader roh;
      gpstk::RinexMetData roe;

      roffs.exceptions(fstream::failbit);

//      roffs >> roh;
//      roh.dump(cout);

      while (roffs >> roe)
      {}

      cout << "done" << endl;
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
