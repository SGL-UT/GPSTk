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
 * @file rinex_obs_read_write.cpp
 * tests gpslib::RinexObsData, gpslib::RinexObsStream, gpslib::RinexObsHeader.
 */
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"

using namespace std;

/// Returns 0 on success.  Input and output files should diff wihtout error.
main(int argc, char *argv[])
{

   if (argc<2)
   {
      cout << "rinex_obs_read_write inputfile outputfile" << endl;
      exit(-1);
   }

   try
   {
      cout << "Reading " << argv[1] << endl;
      gpstk::RinexObsStream roffs(argv[1]);
      cout << "Writing " << argv[2] << endl;
      gpstk::RinexObsStream out(argv[2], ios::out);
      roffs.exceptions(ios::failbit);
      gpstk::RinexObsHeader roh;
      gpstk::RinexObsData roe;
      
      roffs >> roh;
      out << roh;
      roh.dump(cout);

      while (roffs >> roe)
      {
         out << roe;
      }

      cout << "Read " << roffs.recordNumber << " epochs.  Done."  << endl;
      exit(0);
   }
   catch(gpstk::FFStreamError& e)
   {
      cout << e;
      exit(1);
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
