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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2012
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

// Example program Nro 6 for GPSTk
// This program shows a minimalist way to process GPS data
// using GNSS Data Structures (DataStructures.hpp).


#include <iostream>
#include <iomanip>

   // Class for handling satellite observation parameters RINEX files
#include "Rinex3ObsStream.hpp"

   // Classes for handling RINEX Broadcast ephemeris files 
#include "Rinex3NavStream.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"

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

  // YDS Time-class
#include "YDSTime.hpp"


using namespace std;
using namespace gpstk;

int main(void)
{

      //////// Initialization phase ////////

   cout << fixed << setprecision(8);   // Set a proper output format

   Rinex3NavData rNavData;             // Object to store Rinex navigation data
   GPSEphemerisStore bceStore;         // Object to store satellites ephemeris
   Rinex3NavHeader rNavHeader;         // Object to read the header of Rinex
                                       // navigation data files
   IonoModelStore ionoStore;           // Object to store ionospheric models
   IonoModel ioModel;                  // Declare a Ionospheric Model object

      // Create the input observation file stream
   Rinex3ObsStream rin("bahr1620.04o");

      // Create the input navigation file stream
   Rinex3NavStream rnavin("bahr1620.04n");

      // We need to read ionospheric parameters (Klobuchar model) from header
   rnavin >> rNavHeader;

      // Let's feed the ionospheric model (Klobuchar type) from data in the
      // navigation (ephemeris) file header. First, we must check if there are
      // valid ionospheric correction parameters in the header
   if(rNavHeader.valid & Rinex3NavHeader::validIonoCorrGPS)
   {
         // Extract the Alpha and Beta parameters from the header
      double* ionAlpha = rNavHeader.mapIonoCorr["GPSA"].param;
      double* ionBeta  = rNavHeader.mapIonoCorr["GPSB"].param;

         // Feed the ionospheric model with the parameters
      ioModel.setModel(ionAlpha, ionBeta);
   }
   else
   {
      cerr << "WARNING: Navigation file bahr1620.04n "
           << "doesn't have valid ionospheric correction parameters." << endl;
   }

      // Beware: In this case, the same model will be used for the
      // full data span
   ionoStore.addIonoModel(CommonTime::BEGINNING_OF_TIME, ioModel);

      // Storing the ephemeris in "bceStore"
   while (rnavin >> rNavData)
   {
      bceStore.addEphemeris(rNavData);
   }

   bceStore.SearchUser();  // This is the default

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

      cout << static_cast<YDSTime>(gRin.header.epoch).sod
           << " ";   // Output field #1
      cout << solPos.X() << "   ";                    // Output field #2
      cout << solPos.Y() << "   ";                    // Output field #3
      cout << solPos.Z() << "   ";                    // Output field #4
      cout << solPos.longitude() << "   ";            // Output field #5
      cout << solPos.geodeticLatitude() << "   ";     // Output field #6
      cout << solPos.height() << "   ";               // Output field #7

      cout << endl;

   }

   exit(0);

}  // End of 'main()'
