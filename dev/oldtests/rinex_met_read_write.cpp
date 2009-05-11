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
 * @file rinex_met_read_write.cpp
 * tests gpslib::RinexMetData, gpslib::RinexMetHeader and gpslib::RinexMetStream.
 */

#include "RinexMetBase.hpp"
#include "RinexMetData.hpp"
#include "RinexMetHeader.hpp"
#include "RinexMetStream.hpp"

using namespace std;

/// Returns 0 on success. The input file and output file should diff
/// without errors.
main(int argc, char *argv[])
{
   if (argc<3)
   {
      cout << "rinex_met_read_write inputfile outputfile" << endl;
      exit(-1);
   }

   try
   {
      cout << "Reading " << argv[1] << endl;
      gpstk::RinexMetStream roffs(argv[1]);
      cout << "Writing " << argv[2] << endl;
      gpstk::RinexMetStream out(argv[2],ios::out);
//      out.exceptions(fstream::failbit);
      gpstk::RinexMetHeader roh;
      gpstk::RinexMetData roe;



      roffs >> roh;
      out << roh;
      roh.dump(cout);

      while (roffs >> roe)
      {
         out << roe;
      }

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
