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
//  Copyright Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
//
//============================================================================

// Example program Nro 6 for GPSTk
// This program shows a minimalist way to process GPS data
// using GNSS Data Structures (DataStructures.hpp).


#include <iostream>
#include <iomanip>

   // Class for handling satellite observation parameters RINEX files
#include "RinexObsStream.hpp"

   // Classes for handling RINEX Broadcast ephemeris files 
#include "RinexNavStream.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"

   // Class in charge of the GPS signal modelling
#include "ModelObs.hpp"

   // Class to store satellite broadcast navigation data
#include "GPSEphemerisStore.hpp"

   // Class to model the tropospheric delays
#include "TropModel.hpp"

   // Classes to model ans store ionospheric delays
#include "IonoModel.hpp"
#include "IonoModelStore.hpp"

   // Class to solve the equation system using Least Mean Squares
#include "SolverLMS.hpp"

   // Class defining the GNSS data structures
#include "DataStructures.hpp"

   // Class to filter out observables grossly out of limits
#include "SimpleFilter.hpp"


using namespace std;
using namespace gpstk;

int main(void)
{

      //////// Initialization phase ////////

   cout << fixed << setprecision(8);   // Set a proper output format

   RinexNavData rNavData;              // Object to store Rinex navigation data
   GPSEphemerisStore bceStore;         // Object to store satellites ephemeris
   RinexNavHeader rNavHeader;          // Object to read the header of Rinex
                                       // navigation data files
   IonoModelStore ionoStore;           // Object to store ionospheric models
   IonoModel ioModel;                  // Declare a Ionospheric Model object

      // Create the input observation file stream
   RinexObsStream rin("bahr1620.04o");

      // Create the input navigation file stream
   RinexNavStream rnavin("bahr1620.04n");

      // We need to read ionospheric parameters (Klobuchar model) from header
   rnavin >> rNavHeader;

      // Let's feed the ionospheric model (Klobuchar type) with data from the
      // Navigation file header
   ioModel.setModel(rNavHeader.ionAlpha, rNavHeader.ionBeta);
      // Beware: In this case, the same model will be used for the
      // full data span
   ionoStore.addIonoModel(DayTime::BEGINNING_OF_TIME, ioModel);

      // Storing the ephemeris in "bceStore"
   while (rnavin >> rNavData)
   {
      bceStore.addEphemeris(rNavData);
   }

   bceStore.SearchPast();  // This is the default

      // BAHR station nominal position
   Position nominalPos(3633909.1016, 4425275.5033, 2799861.2736);

      // Declare a MOPSTropModel object, setting the defaults
   MOPSTropModel mopsTM( nominalPos.getAltitude(),
                         nominalPos.getGeodeticLatitude(),
                         162 );

      // Declare the modeler object, setting all the parameters in one pass
   ModelObs modelRef(nominalPos, ionoStore, mopsTM, bceStore, TypeID::C1);

      // Declare SolverLMS object
   SolverLMS solver;

      // Declare a simple filter object. By default, it filters C1
   SimpleFilter myFilter;

      // This is the GNSS data structure that will hold all the
      // GNSS-related information
   gnssRinex gRin;

      //////// End of initialization phase ////////


      //////// Processing phase ////////

      // Loop over all data epochs
   while(rin >> gRin)
   {

      try
      {

            // This is the line that will process all the GPS data
         gRin.keepOnlyTypeID(TypeID::C1) >> myFilter >> modelRef >> solver;
            // First: Wipe off all data that we will not use (may be skipped)
            // Second: Filter out observables out of bounds (may be skipped)
            // Third: The resulting data structure will feed the modeler object
            // Fourth: The resulting structure from third step will feed the 
            // solver object

      }
      catch(...)
      {
         cerr << "Exception at epoch: " << gRin.header.epoch << endl;
      }

         // Fifth: Get your results out of the solver object and print them.
         // That is all
      Position solPos( (modelRef.rxPos.X() + solver.solution[0]),
                       (modelRef.rxPos.Y() + solver.solution[1]),
                       (modelRef.rxPos.Z() + solver.solution[2]) );

      cout << gRin.header.epoch.DOYsecond() << " ";   // Output field #1
      cout << solPos.X() << "   ";                    // Output field #2
      cout << solPos.Y() << "   ";                    // Output field #3
      cout << solPos.Z() << "   ";                    // Output field #4
      cout << solPos.longitude() << "   ";            // Output field #5
      cout << solPos.geodeticLatitude() << "   ";     // Output field #6
      cout << solPos.height() << "   ";               // Output field #7

      cout << endl;

   }

   exit(0);

}
