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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================


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

<<<<<<< .working
   // Class for handling tropospheric models
#include "TropModel.hpp"
=======
// Class for handling tropospheric models
#include "TropModel.hpp"
>>>>>>> .merge-right.r3070

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

<<<<<<< .working
      // Object for void-type tropospheric model (in case no meteorological
      // RINEX is available)
=======
   // Object for void-type tropospheric model (in case no meteorological RINEX
   // is available)
>>>>>>> .merge-right.r3070
   ZeroTropModel noTropModel;

<<<<<<< .working
      // Object for GG-type tropospheric model (Goad and Goodman, 1974)
      // Default constructor => default values for model
   GGTropModel ggTropModel;

      // Pointer to one of the two available tropospheric models. It points
      // to the void model by default
=======
   // Object for GG-type tropospheric model (Goad and Goodman, 1974)
   GGTropModel ggTropModel;   // Default constructor => default values for model
   // Pointer to one of the two available tropospheric models. It points to
   // the void model by default
>>>>>>> .merge-right.r3070
   TropModel *tropModelPtr=&noTropModel;

      // This verifies the ammount of command-line parameters given and
      // prints a help message, if necessary
   if( (argc < 3) || (argc > 4) )
   {
      cerr <<  "Usage:" << endl;
      cerr << "   " << argv[0]
           << " <RINEX Obs file>  <RINEX Nav file>  [<RINEX Met file>]"
           << endl;

<<<<<<< .working
=======
   // This verifies the ammount of command-line parameters given and prints a help
   // message, if necessary
   if ((argc < 3) || (argc>4))
   {
      cerr <<  "Usage:" << endl;
      cerr << "   " << argv[0] << " <RINEX Obs file>  <RINEX Nav file>  [<RINEX Met file>]" << endl;
>>>>>>> .merge-right.r3070
      exit (-1);
    }

<<<<<<< .working
      // Let's compute an useful constant (also found in "GNSSconstants.hpp")
   const double gamma = (L1_FREQ_GPS/L2_FREQ_GPS)*(L1_FREQ_GPS/L2_FREQ_GPS);

=======
   // Let's compute an useful constant (also found in "icd_200_constants.hpp")
   const double gamma = (L1_FREQ / L2_FREQ)*(L1_FREQ / L2_FREQ);

>>>>>>> .merge-right.r3070
   try
<<<<<<< .working
   {

         // Read nav file and store unique list of ephemerides
      Rinex3NavStream rnffs(argv[2]);    // Open ephemerides data file
      Rinex3NavData rne;
      Rinex3NavHeader hdr;

         // Let's read the header (may be skipped)
=======
   {
      // Read nav file and store unique list of ephemeredes
      RinexNavStream rnffs(argv[2]);    // Open ephemerides data file
      RinexNavData rne;
      RinexNavHeader hdr;

      // Let's read the header (may be skipped)
>>>>>>> .merge-right.r3070
      rnffs >> hdr;

         // Storing the ephemeris in "bcstore"
      while (rnffs >> rne) bcestore.addEphemeris(rne);

         // Setting the criteria for looking up ephemeris
      bcestore.SearchNear();
<<<<<<< .working

         // If provided, open and store met file into a linked list.
=======

      // If provided, open and store met file into a linked list.
>>>>>>> .merge-right.r3070
      list<RinexMetData> rml;
        
      if( argc == 4 )
      {

         RinexMetStream rms(argv[3]);    // Open meteorological data file
         RinexMetHeader rmh;
            
            // Let's read the header (may be skipped)
         rms >> rmh;

         RinexMetData rmd;
<<<<<<< .working
            
            // If meteorological data is provided, let's change pointer to
            // a GG-model object
=======
         // If meteorological data is provided, let's change pointer to
         // a GG-model object
>>>>>>> .merge-right.r3070
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

<<<<<<< .working
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
=======
      // Defining iterator "mi" for meteorological data linked list "rml", and
      // set it to the beginning
>>>>>>> .merge-right.r3070
      list<RinexMetData>::iterator mi=rml.begin();

         // Let's process all lines of observation data, one by one
      while( roffs >> rod )
      {

<<<<<<< .working
            // Find a weather point. Only if a meteorological RINEX file
            // was provided, the meteorological data linked list "rml" is
            // neither empty or at its end, and the time of meteorological
            // records are below observation data epoch.
         while( ( argc==4 )       &&
                ( !rml.empty() )  &&
                ( mi!=rml.end() ) &&
                ( (*mi).time < rod.time ) )
=======
      // Let's process all lines of observation data, one by one
      while (roffs >> rod)
      {

         // Find a weather point. Only if a meteorological RINEX file was
         // provided, the meteorological data linked list "rml" is neither empty
         // or at its end, and the time of meteorological records are below
         // observation data epoch.
         while ( (argc==4) &&
                 (!rml.empty()) &&
                 (mi!=rml.end()) &&
                 ((*mi).time < rod.time) )
>>>>>>> .merge-right.r3070
         {

            mi++;    // Read next item in list

<<<<<<< .working
               // Feed GG tropospheric model object with meteorological
               // parameters. Take into account, however, that setWeather
               // is not accumulative, i.e., only the last fed set of
               // data will be used for computation
            ggTropModel.setWeather( (*mi).data[RinexMetHeader::TD],
                                    (*mi).data[RinexMetHeader::PR],
                                    (*mi).data[RinexMetHeader::HR] );
=======
            // Feed GG tropospheric model object with meteorological parameters
            // Take into account, however, that setWeather is not accumulative,
            // i.e., only the last fed set of data will be used for computation
            ggTropModel.setWeather((*mi).data[RinexMetHeader::TD],
                                   (*mi).data[RinexMetHeader::PR],
                                   (*mi).data[RinexMetHeader::HR]);
         }
>>>>>>> .merge-right.r3070

<<<<<<< .working
         }  // End of 'while( ( argc==4 ) && ...'


            // Apply editing criteria
         if( rod.epochFlag == 0 || rod.epochFlag == 1 )  // Begin usable data
         {

            vector<SatID> prnVec;
=======

         // Apply editing criteria
         if  (rod.epochFlag == 0 || rod.epochFlag == 1)   // Begin usable data
	     {
	        vector<SatID> prnVec;
>>>>>>> .merge-right.r3070
            vector<double> rangeVec;

<<<<<<< .working
               // Define the "it" iterator to visit the observations PRN map. 
               // Rinex3ObsData::DataMap is a map from RinexSatID to
               // vector<RinexDatum>:
               //      std::map<RinexSatID, vector<RinexDatum> >
            Rinex3ObsData::DataMap::const_iterator it;
=======
            // Let's define the "it" iterator to visit the observations PRN map
            // RinexSatMap is a map from SatID to RinexObsTypeMap:
            //      std::map<SatID, RinexObsTypeMap>
            RinexObsData::RinexSatMap::const_iterator it;
>>>>>>> .merge-right.r3070

<<<<<<< .working
               // This part gets the PRN numbers and ionosphere-corrected
               // pseudoranges for the current epoch. They are correspondly fed
               // into "prnVec" and "rangeVec"; "obs" is a public attribute of
               // Rinex3ObsData to get the map of observations
            for( it = rod.obs.begin(); it!= rod.obs.end(); it++ )
=======
            // This part gets the PRN numbers and ionosphere-corrected
            // pseudoranges for the current epoch. They are correspondly fed
            // into "prnVec" and "rangeVec"
            // "obs" is a public attribute of RinexObsData to get the map
            // of observations
            for (it = rod.obs.begin(); it!= rod.obs.end(); it++)
>>>>>>> .merge-right.r3070
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

<<<<<<< .working
               double ionocorr( 0.0 );
=======
               /////////////////////////////////////////////////
               //
               //    What did we do in the former code lines?:
               //
               // For each observation data epoch (rod), if valid
               // (rod.epochFlag = 0 or 1):
               // - use "it" iterator to visit the RinexObsTypeMap of each
               //   satellite,
               // - and then use "itP1" and "itP2" iterators to visit the
               //   observation data (RinexDatum) according to their type
               //  (RinexObsType)
               //
               /////////////////////////////////////////////////
>>>>>>> .merge-right.r3070

                  // If there are P2 observations, let's try to apply the
                  // ionospheric corrections
               if( indexP2 >= 0 )
               {

<<<<<<< .working
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
=======
               // The "second" field of a RinexPrnMap (it) is a
               // RinexObsTypeMap (otmap)
               otmap = (*it).second;
>>>>>>> .merge-right.r3070

<<<<<<< .working
                     // Vector 'vecData' contains RinexDatum, whose public
                     // attribute "data" indeed holds the actual data point
                  ionocorr = 1.0 / (1.0 - gamma) * ( P1 - P2 );
=======
               // Let's find a P1 observation inside the RinexObsTypeMap that
               // is "otmap"
               itP1 = otmap.find(RinexObsHeader::P1);
>>>>>>> .merge-right.r3070

<<<<<<< .working
               }
=======
               // If "itP1" is not the last type of observation, there may be
               // a P2 observation and the double-frequency ionospheric
               // corrections may be applied
               if (itP1!=otmap.end())
	           {
                  double ionocorr = 0;
>>>>>>> .merge-right.r3070

<<<<<<< .working
                  // Now, we include the current PRN number in the first part
                  // of "it" iterator into the vector holding the satellites.
                  // All satellites in view at this epoch that have P1 or P1+P2
                  // observations will be included.
               prnVec.push_back( (*it).first );

=======
                  // Now, let's find a P2 observation inside the
                  // RinexObsTypeMap that is "otmap"
                  itP2 = otmap.find(RinexObsHeader::P2);
                  // If we indeed found a P2 observation, let's apply the
                  // ionospheric corrections
                  if (itP2!=otmap.end())
                     // The "second" part of a RinexObsTypeMap is a RinexDatum,
                     // whose public attribute "data" indeed holds the actual
                     // data point
                     ionocorr = 1./(1.-gamma)*((*itP1).second.data-(*itP2).second.data);
                  // Now, we include the current PRN number in the first part
                  // of "it" (a RinexPrnMap) into the vector holding PRN numbers.
                  // All satellites in view at this epoch that also have P1 and
                  // P2 observations will be included
                  prnVec.push_back((*it).first);
>>>>>>> .merge-right.r3070
                  // The same is done for the vector of doubles holding the
                  // corrected ranges
               rangeVec.push_back( P1 - ionocorr );

<<<<<<< .working
                     // WARNING: Please note that so far no further correction
                     // is done on data: Relativistic effects, tropospheric
                     // correction, instrumental delays, etc.
=======
                  // WARNING: Please note that so far no further correction is
                  // done on data: Relativistic effects, tropospheric correction,
                  // instrumental delays, etc.
	           }
>>>>>>> .merge-right.r3070

            }  // End of 'for( it = rod.obs.begin(); it!= rod.obs.end(); ...'

<<<<<<< .working
               // The default constructor for PRSolution2 objects (like
               // "raimSolver") is to set a RMSLimit of 6.5. We change that
               // here. With this value of 3e6 the solution will have a lot
               // more dispersion.
=======
            // The default constructor for PRSolution objects (like "raimSolver")
            // is to set a RMSLimit of 6.5. We change that here. With this value
            // of 3e6 the solution will have a lot more dispersion
>>>>>>> .merge-right.r3070
            raimSolver.RMSLimit = 3e6;

<<<<<<< .working
               // In order to compute positions we need the current time, the
               // vector of visible satellites, the vector of corresponding
               // ranges, the object containing satellite ephemerides, and a
               // pointer to the tropospheric model to be applied
            raimSolver.RAIMCompute( rod.time,
                                    prnVec,
                                    rangeVec,
                                    bcestore,
                                    tropModelPtr );
=======
            // In order to compute positions we need the current time, the
            // vector of visible satellites, the vector of corresponding ranges,
            // the object containing satellite ephemerides and a pointer to the
            // tropospheric model to be applied
	        raimSolver.RAIMCompute(rod.time,prnVec,rangeVec, bcestore,  tropModelPtr);
>>>>>>> .merge-right.r3070

<<<<<<< .working
               // Note: Given that the default constructor sets public
               // attribute "Algebraic" to FALSE, a linearized least squares
               // algorithm will be used to get the solutions.
               // Also, the default constructor sets ResidualCriterion to true,
               // so the rejection criterion is based on RMS residual of fit,
               // instead of RMS distance from an a priori position.
=======
            // Note: Given that the default constructor sets public attribute
            // "Algebraic" to FALSE, a linearized least squares algorithm will
            // be used to get the solutions.
            // Also, the default constructor sets ResidualCriterion to true, so
            // the rejection criterion is based on RMS residual of fit, instead
            // of RMS distance from an a priori position.
>>>>>>> .merge-right.r3070

               // If we got a valid solution, let's print it

            if( raimSolver.isValid() )
            {
<<<<<<< .working
                  // Vector "Solution" holds the coordinates, expressed in
                  // meters in an Earth Centered, Earth Fixed (ECEF) reference
                  // frame. The order is x, y, z  (as all ECEF objects)
=======
               // Vector "Solution" holds the coordinates, expressed in meters
               // in an Earth Centered, Earth Fixed (ECEF) reference frame. The
               // order is x, y, z  (as all ECEF objects)
>>>>>>> .merge-right.r3070
               cout << setprecision(12) << raimSolver.Solution[0] << " " ;
               cout << raimSolver.Solution[1] << " " ;
               cout << raimSolver.Solution[2];
               cout << endl ;
<<<<<<< .working
=======
            }


 	     } // End usable data
>>>>>>> .merge-right.r3070

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

<<<<<<< .working
}  // End of 'main()'
=======
      exit(0);

}





>>>>>>> .merge-right.r3070
