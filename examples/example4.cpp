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

// GPSTk example program #4


   // First, let's include Standard Template Library classes
#include <string>
#include <vector>

   // Classes for handling observations RINEX files (data)
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"
#include "Rinex3ObsStream.hpp"

   // Classes for handling satellite navigation parameters RINEX
   // files (ephemerides)
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3NavStream.hpp"

   // Classes for handling RINEX files with meteorological parameters
#include "RinexMetBase.hpp"
#include "RinexMetData.hpp"
#include "RinexMetHeader.hpp"
#include "RinexMetStream.hpp"

   // Class for handling tropospheric models
#include "TropModel.hpp"

   // Class for storing "broadcast-type" ephemerides
#include "GPSEphemerisStore.hpp"

   // Class for handling RAIM
#include "PRSolution2.hpp"

   // Class defining GPS system constants
#include "GNSSconstants.hpp"


using namespace std;
using namespace gpstk;


int main(int argc, char *argv[])
{

      // Declaration of objects for storing ephemerides and handling RAIM
   GPSEphemerisStore bcestore;
   PRSolution2 raimSolver;

      // Object for void-type tropospheric model (in case no meteorological
      // RINEX is available)
   ZeroTropModel noTropModel;

      // Object for GG-type tropospheric model (Goad and Goodman, 1974)
      // Default constructor => default values for model
   GGTropModel ggTropModel;

      // Pointer to one of the two available tropospheric models. It points
      // to the void model by default
   TropModel *tropModelPtr=&noTropModel;

      // This verifies the ammount of command-line parameters given and
      // prints a help message, if necessary
   if( (argc < 3) || (argc > 4) )
   {
      cerr <<  "Usage:" << endl;
      cerr << "   " << argv[0]
           << " <RINEX Obs file>  <RINEX Nav file>  [<RINEX Met file>]"
           << endl;

      exit (-1);
    }

      // Let's compute an useful constant (also found in "GNSSconstants.hpp")
   const double gamma = (L1_FREQ_GPS/L2_FREQ_GPS)*(L1_FREQ_GPS/L2_FREQ_GPS);

   try
   {

         // Read nav file and store unique list of ephemerides
      Rinex3NavStream rnffs(argv[2]);    // Open ephemerides data file
      Rinex3NavData rne;
      Rinex3NavHeader hdr;

         // Let's read the header (may be skipped)
      rnffs >> hdr;

         // Storing the ephemeris in "bcstore"
      while (rnffs >> rne) bcestore.addEphemeris(rne);

         // Setting the criteria for looking up ephemeris
      bcestore.SearchNear();

         // If provided, open and store met file into a linked list.
      list<RinexMetData> rml;
        
      if( argc == 4 )
      {

         RinexMetStream rms(argv[3]);    // Open meteorological data file
         RinexMetHeader rmh;
            
            // Let's read the header (may be skipped)
         rms >> rmh;

         RinexMetData rmd;
            
            // If meteorological data is provided, let's change pointer to
            // a GG-model object
         tropModelPtr=&ggTropModel;

            // All data is read into "rml", a meteorological data
            // linked list
         while (rms >> rmd) rml.push_back(rmd);

      }  // End of 'if( argc == 4 )'

         // Open and read the observation file one epoch at a time.
         // For each epoch, compute and print a position solution
      Rinex3ObsStream roffs(argv[1]);    // Open observations data file

         // In order to throw exceptions, it is necessary to set the failbit
      roffs.exceptions(ios::failbit);

      Rinex3ObsHeader roh;
      Rinex3ObsData rod;

         // Let's read the header
      roffs >> roh;

         // The following lines fetch the corresponding indexes for some
         // observation types we are interested in. Given that old-style
         // observation types are used, GPS is assumed.
      int indexP1;
      try
      {
         indexP1 = roh.getObsIndex( "P1" );
      }
      catch(...)
      {
         cerr << "The observation file doesn't have P1 pseudoranges." << endl;
         exit(1);
      }

      int indexP2;
      try
      {
         indexP2 = roh.getObsIndex( "P2" );
      }
      catch(...)
      {
         indexP2 = -1;
      }

         // Defining iterator "mi" for meteorological data linked list
         // "rml", and set it to the beginning
      list<RinexMetData>::iterator mi=rml.begin();

         // Let's process all lines of observation data, one by one
      while( roffs >> rod )
      {

            // Find a weather point. Only if a meteorological RINEX file
            // was provided, the meteorological data linked list "rml" is
            // neither empty or at its end, and the time of meteorological
            // records are below observation data epoch.
         while( ( argc==4 )       &&
                ( !rml.empty() )  &&
                ( mi!=rml.end() ) &&
                ( (*mi).time < rod.time ) )
         {

            mi++;    // Read next item in list

               // Feed GG tropospheric model object with meteorological
               // parameters. Take into account, however, that setWeather
               // is not accumulative, i.e., only the last fed set of
               // data will be used for computation
            ggTropModel.setWeather( (*mi).data[RinexMetHeader::TD],
                                    (*mi).data[RinexMetHeader::PR],
                                    (*mi).data[RinexMetHeader::HR] );

         }  // End of 'while( ( argc==4 ) && ...'


            // Apply editing criteria
         if( rod.epochFlag == 0 || rod.epochFlag == 1 )  // Begin usable data
         {

            vector<SatID> prnVec;
            vector<double> rangeVec;

               // Define the "it" iterator to visit the observations PRN map. 
               // Rinex3ObsData::DataMap is a map from RinexSatID to
               // vector<RinexDatum>:
               //      std::map<RinexSatID, vector<RinexDatum> >
            Rinex3ObsData::DataMap::const_iterator it;

               // This part gets the PRN numbers and ionosphere-corrected
               // pseudoranges for the current epoch. They are correspondly fed
               // into "prnVec" and "rangeVec"; "obs" is a public attribute of
               // Rinex3ObsData to get the map of observations
            for( it = rod.obs.begin(); it!= rod.obs.end(); it++ )
            {

                  // The RINEX file may have P1 observations, but the current
                  // satellite may not have them.
               double P1( 0.0 );
               try
               {
                  P1 = rod.getObs( (*it).first, indexP1 ).data;
               }
               catch(...)
               {
                     // Ignore this satellite if P1 is not found
                  continue;
               }

               double ionocorr( 0.0 );

                  // If there are P2 observations, let's try to apply the
                  // ionospheric corrections
               if( indexP2 >= 0 )
               {

                     // The RINEX file may have P2 observations, but the
                     // current satellite may not have them.
                  double P2( 0.0 );
                  try
                  {
                     P2 = rod.getObs( (*it).first, indexP2 ).data;
                  }
                  catch(...)
                  {
                        // Ignore this satellite if P1 is not found
                     continue;
                  }

                     // Vector 'vecData' contains RinexDatum, whose public
                     // attribute "data" indeed holds the actual data point
                  ionocorr = 1.0 / (1.0 - gamma) * ( P1 - P2 );

               }

                  // Now, we include the current PRN number in the first part
                  // of "it" iterator into the vector holding the satellites.
                  // All satellites in view at this epoch that have P1 or P1+P2
                  // observations will be included.
               prnVec.push_back( (*it).first );

                  // The same is done for the vector of doubles holding the
                  // corrected ranges
               rangeVec.push_back( P1 - ionocorr );

                     // WARNING: Please note that so far no further correction
                     // is done on data: Relativistic effects, tropospheric
                     // correction, instrumental delays, etc.

            }  // End of 'for( it = rod.obs.begin(); it!= rod.obs.end(); ...'

               // The default constructor for PRSolution2 objects (like
               // "raimSolver") is to set a RMSLimit of 6.5. We change that
               // here. With this value of 3e6 the solution will have a lot
               // more dispersion.
            raimSolver.RMSLimit = 3e6;

               // In order to compute positions we need the current time, the
               // vector of visible satellites, the vector of corresponding
               // ranges, the object containing satellite ephemerides, and a
               // pointer to the tropospheric model to be applied
            raimSolver.RAIMCompute( rod.time,
                                    prnVec,
                                    rangeVec,
                                    bcestore,
                                    tropModelPtr );

               // Note: Given that the default constructor sets public
               // attribute "Algebraic" to FALSE, a linearized least squares
               // algorithm will be used to get the solutions.
               // Also, the default constructor sets ResidualCriterion to true,
               // so the rejection criterion is based on RMS residual of fit,
               // instead of RMS distance from an a priori position.

               // If we got a valid solution, let's print it

            if( raimSolver.isValid() )
            {
                  // Vector "Solution" holds the coordinates, expressed in
                  // meters in an Earth Centered, Earth Fixed (ECEF) reference
                  // frame. The order is x, y, z  (as all ECEF objects)
               cout << setprecision(12) << raimSolver.Solution[0] << " " ;
               cout << raimSolver.Solution[1] << " " ;
               cout << raimSolver.Solution[2];
               cout << endl ;

            }  // End of 'if( raimSolver.isValid() )'

         } // End of 'if( rod.epochFlag == 0 || rod.epochFlag == 1 )'

      }  // End of 'while( roffs >> rod )'

   }
   catch(Exception& e)
   {
      cerr << e << endl;
   }
   catch (...)
   {
      cerr << "Caught an unexpected exception." << endl;
   }

   exit(0);

}  // End of 'main()'
