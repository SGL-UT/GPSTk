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

#include "Rinex3ObsBase.hpp"
#include "Rinex3ObsData.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsStream.hpp"
#include "CivilTime.hpp"
#include "GNSSconstants.hpp"
#include <iostream>


using namespace std;
using namespace gpstk;


   // ISO C++ forbids declaration of `main' with no type
int main(int argc, char *argv[])
{

   int myprn;

   if( argc<2 )
   {
      cout << "Required argument is a RINEX obs file." << endl;
      exit(-1);
   }

   cout << "Name your PRN of interest (by number: 1 through 32): ";
   cin  >> myprn;

   double gamma = (L1_FREQ_GPS / L2_FREQ_GPS)*(L1_FREQ_GPS / L2_FREQ_GPS);

   try
   {

      cout << "Reading " << argv[1] << "." << endl;

         // Declare RINEX observation file streams and data objects
         // -------------------------------------------------------
      Rinex3ObsStream roffs(argv[1]);

         // It is necessary to set the failbit in order to throw exceptions
      roffs.exceptions(ios::failbit);
      Rinex3ObsHeader roh;
      Rinex3ObsData roe;
      RinexDatum dataobj;

         // Read the RINEX header (don't skip this step)
         // --------------------------------------------
      roffs >> roh;

         // Print RINEX header to terminal screen
         // -------------------------------------
      roh.dump(cout);

         // The following lines fetch the corresponding indexes for some
         // observation types we are interested in
      int indexP1( roh.getObsIndex( "P1" ) );
      int indexP2( roh.getObsIndex( "P2" ) );

         // Loop through epochs and process data for each.
         // ----------------------------------------------
      while( roffs >> roe )
      {

            // Let's use the CivilTime class to print time
         CivilTime civtime( roe.time );

         cout << civtime  << " ";

            // Make a GPSTK SatID object for your PRN so you can search it
            // -----------------------------------------------------------
         SatID prn( myprn, SatID::systemGPS );

            // Check to see if your PRN is in view at this epoch (ie.
            // search for the PRN).
            // -----------------------------------------------------------
         Rinex3ObsData::DataMap::iterator pointer = roe.obs.find(prn);
         if( pointer == roe.obs.end() )
         {
            cout << "PRN " << myprn << " not in view " << endl;
         }
         else
         {
               // Get P1, P2 and L1 observations
               // Here there are three equivalent ways to get the RinexDatum
               // from the RinexObsData object

               // The first one is a fast but DANGEROUS method, because there
               // is a chance of unawarely change the contents of "roe.obs".
               // -----------------------------------------------------------
            dataobj = roe.obs[prn][indexP1];
            double P1 = dataobj.data;

               // The second method is secure but a little slower.
               // This should be your preferred method
               // -----------------------------------------------------------
            dataobj = roe.getObs(prn, indexP2);
            double P2 = dataobj.data;

               // The third method is also secure but it is the slowest.
               // On the other hand it has the advantage that it doesn't need
               // a prior call to method 'Rinex3ObsHeader::getObsIndex()'
               // -----------------------------------------------------------
            dataobj = roe.getObs(prn, "L1", roh);
            double L1 = dataobj.data;

               // Compute multipath
               // -----------------
            double mu = P1 -L1*(C_MPS/L1_FREQ_GPS) -2*(P1 -P2)/(1-gamma);

               // The following line makes sure that you get a proper output
               // format. The line above with "roh.dump" sets this, but just
               // in case...
            cout << fixed << setw(7) << setprecision(3);

            cout << " PRN " << myprn
                 << " biased multipath " <<  mu << endl;

         }  // End of 'if( pointer == roe.obs.end() )'

      }  // End of 'while (roffs >> roe)'

      cout << "Read " << roffs.recordNumber << " epochs.  Cheers."  << endl;

      exit(0);

   } // End of 'try' block
   catch(FFStreamError& e)
   {
      cout << e;
      exit(1);
   }
   catch(Exception& e)
   {
      cout << e;
      exit(1);
   }
   catch (...)
   {
      cout << "unknown error.  I don't feel so well..." << endl;
      exit(1);
   }

   exit(0);

}  // End of 'main()'
