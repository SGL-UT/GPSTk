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
//  Copyright Dagoberto Salazar - gAGE ( http://www.gage.es ). 2010
//
//============================================================================

// Example program Nro 14 for GPSTk
//
// This program shows how to use GNSS Data Structures (GDS) and other classes
// to build a reasonable complete application that implements the "Precise
// Orbits Positioning" (POP) strategy.
//
// For details on the POP algorithm please consult:
//
//    Salazar, D., Hernandez-Pajares, M., Juan, J.M. and J. Sanz. "GNSS data
//       management and processing with the GPSTk". GPS Solutions.
//       DOI: 10.1007/s10291-009-0149-9, November, 2009.
//
// In this program we build from part of "example9.cpp" (PPP implementation),
// but modeling and resolution phases are separated:
//
//    - First all stations in the network are preprocesed, and their data will
//      be stored in a "gnssDataMap" data structure.
//
//    - Afterwards, in the resolution phase the "gnssDataMap" object will feed
//      a "SolverGeneral" solver, appropriately configured.
//
// Please be aware that POP processes data from a network of stations, and that
// the data rate typically is a lot higher than PPP. Expect a 5 stations network
// with 30 s observation data to take about 9 minutes to be processed with a
// modern PC (Dual Core, 2 GB RAM) with Linux.
//


// Basic input/output C++ classes
#include <iostream>
#include <iomanip>
#include <fstream>


   // Basic framework for programs in the GPSTk. 'process()' method MUST
   // be implemented
#include "BasicFramework.hpp"

   // Class for handling observation RINEX files
#include "RinexObsStream.hpp"

   // Class to store satellite precise navigation data
#include "SP3EphemerisStore.hpp"

   // Class to store a list of processing objects
#include "ProcessingList.hpp"

   // Class in charge of basic GNSS signal modelling
#include "BasicModel.hpp"

   // Class to model the tropospheric delays
#include "TropModel.hpp"

   // Class defining the GNSS data structures
#include "DataStructures.hpp"

   // Class to filter out satellites without required observables
#include "RequireObservables.hpp"

   // Class to filter out observables grossly out of limits
#include "SimpleFilter.hpp"

   // Class for easily changing reference base from ECEF to NEU
#include "XYZ2NEU.hpp"

   // Class to detect cycle slips using LI combination
#include "LICSDetector2.hpp"
#include "LICSDetector.hpp"

   // Class to detect cycle slips using the Melbourne-Wubbena combination
#include "MWCSDetector.hpp"

   // Class to compute the effect of solid tides
#include "SolidTides.hpp"

   // Class to compute the effect of ocean loading
#include "OceanLoading.hpp"

   // Class to compute the effect of pole tides
#include "PoleTides.hpp"

   // Class to correct observables
#include "CorrectObservables.hpp"

   // Classes to deal with Antex antenna parameters
#include "Antenna.hpp"
#include "AntexReader.hpp"

   // Class to compute the effect of wind-up
#include "ComputeWindUp.hpp"

   // Class to compute the effect of satellite antenna phase center
#include "ComputeSatPCenter.hpp"

   // Class to compute the tropospheric data
#include "ComputeTropModel.hpp"

   // Class to compute linear combinations
#include "ComputeLinear.hpp"

   // This class pre-defines several handy linear combinations
#include "LinearCombinations.hpp"

   // Class to compute Dilution Of Precision values
#include "ComputeDOP.hpp"

   // Class to keep track of satellite arcs
#include "SatArcMarker.hpp"

   // Class to compute gravitational delays
#include "GravitationalDelay.hpp"

   // Class to align phases with code measurements
#include "PhaseCodeAlignment.hpp"

   // Used to delete satellites in eclipse
#include "EclipsedSatFilter.hpp"

   // Class to solve a general equation.
#include "SolverGeneral.hpp"

   // Class to read configuration files.
#include "ConfDataReader.hpp"



using namespace std;
using namespace gpstk;


   // A new class is declared that will handle program behaviour
   // This class inherits from BasicFramework
class example14 : public BasicFramework
{
public:

      // Constructor declaration
   example14(char* arg0);


protected:


      // Method that will take care of processing
   virtual void process();


      // Method that hold code to be run BEFORE processing
   virtual void spinUp();


      // Method that hold code to be run AFTER processing
   virtual void shutDown();


private:


      // This field represents an option at command line interface (CLI)
   CommandOptionWithArg confFile;

      // If you want to share objects and variables among methods, you'd
      // better declare them here

      // Configuration file reader
   ConfDataReader confReader;


      // Method to print model values
   void printModel( ofstream& modelfile,
                    const gnssRinex& gData,
                    int   precision = 5 );


      // Map to store dry tropospheric delay value
   std::map<SourceID, double> tropoMap;

      // Master station
   SourceID master;

      // Reference stations set
   std::set<SourceID> refStationSet;

      // Rover receiver
   SourceID rover;

      // Data structure
   gnssDataMap gdsMap;


}; // End of 'example14' class declaration



   // Let's implement constructor details
example14::example14(char* arg0)
   :
   BasicFramework(  arg0,
"\nThis program reads GPS receiver data from a configuration file and\n"
"process such data applying a 'Precise Orbits Positioning' strategy.\n\n"
"The output file format is as follows:\n\n"
" 1) Seconds of day\n"
" 2) dLat (m)\n"
" 3) dLon (m)\n"
" 4) dH (m)\n"
" 5) Zenital Tropospheric Delay - zpd (m)\n" ),
      // Option initialization. "true" means a mandatory option
   confFile( CommandOption::stdType,
             'c',
             "conffile",
   " [-c|--conffile]    Name of configuration file ('popconf.txt' by default).",
             false )
{

      // This option may appear just once at CLI
   confFile.setMaxCount(1);

}  // End of 'example14::example14'



   // Method to print model values
void example14::printModel( ofstream& modelfile,
                           const gnssRinex& gData,
                           int   precision )
{

      // Prepare for printing
   modelfile << fixed << setprecision( precision );

      // Get epoch out of GDS
   DayTime time(gData.header.epoch);

      // Iterate through the GNSS Data Structure
   for ( satTypeValueMap::const_iterator it = gData.body.begin();
         it!= gData.body.end();
         it++ )
   {

         // Print epoch
      modelfile << time.year()         << "  ";    // Year           #1
      modelfile << time.DOY()          << "  ";    // DayOfYear      #2
      modelfile << time.DOYsecond()    << "  ";    // SecondsOfDay   #3

         // Print satellite information (Satellite system and ID number)
      modelfile << (*it).first << " ";             // System         #4
                                                   // ID number      #5

         // Print model values
      for( typeValueMap::const_iterator itObs  = (*it).second.begin();
           itObs != (*it).second.end();
           itObs++ )
      {
            // Print type names and values
         modelfile << (*itObs).first << " ";
         modelfile << (*itObs).second << " ";

      }  // End of 'for( typeValueMap::const_iterator itObs = ...'

      modelfile << endl;

   }  // End for (it = gData.body.begin(); ... )

}  // End of method 'example14::printModel()'



   // Method that will be executed AFTER initialization but BEFORE processing
void example14::spinUp()
{

      // Check if the user provided a configuration file name
   if ( confFile.getCount() > 0 )
   {

         // Enable exceptions
      confReader.exceptions(ios::failbit);

      try
      {

            // Try to open the provided configuration file
         confReader.open( confFile.getValue()[0] );

      }
      catch(...)
      {

         cerr << "Problem opening file "
              << confFile.getValue()[0]
              << endl;
         cerr << "Maybe it doesn't exist or you don't have proper "
              << "read permissions." << endl;

         exit (-1);

      }  // End of 'try-catch' block

   }
   else
   {

      try
      {
            // Try to open default configuration file
         confReader.open( "popconf.txt" );
      }
      catch(...)
      {

         cerr << "Problem opening default configuration file 'pop.conf'"
              << endl;
         cerr << "Maybe it doesn't exist or you don't have proper read "
              << "permissions. Try providing a configuration file with "
              << "option '-c'."
              << endl;

         exit (-1);

      }  // End of 'try-catch' block

   }  // End of 'if ( confFile.getCount() > 0 )'


      // If a given variable is not found in the provided section, then
      // 'confReader' will look for it in the 'DEFAULT' section.
   confReader.setFallback2Default(true);


}  // End of method 'example14::spinUp()'



   // Method that will really process information
void example14::process()
{

      /////////////////
      // Put here what you want to apply to ALL stations
      /////////////////


   //// vvvv Ephemeris handling vvvv

      // Declare a "SP3EphemerisStore" object to handle precise ephemeris
   SP3EphemerisStore SP3EphList;

      // Set flags to reject satellites with bad or absent positional
      // values or clocks
   SP3EphList.rejectBadPositions(true);
   SP3EphList.rejectBadClocks(true);

      // Read if we should check for data gaps.
   if ( confReader.getValueAsBoolean( "checkGaps", "DEFAULT" ) )
   {
      SP3EphList.enableDataGapCheck();
      SP3EphList.setGapInterval(
                  confReader.getValueAsDouble("SP3GapInterval", "DEFAULT" ) );
   }

      // Read if we should check for too wide interpolation intervals
   if ( confReader.getValueAsBoolean( "checkInterval", "DEFAULT" ) )
   {
      SP3EphList.enableIntervalCheck();
      SP3EphList.setMaxInterval(
                  confReader.getValueAsDouble("maxSP3Interval", "DEFAULT" ) );
   }


      // Load all the SP3 ephemerides files from variable list
   string sp3File;
   while ( (sp3File = confReader.fetchListValue("SP3List", "DEFAULT" ) ) != "" )
   {

         // Try to load each ephemeris file
      try
      {

         SP3EphList.loadFile( sp3File );

      }
      catch (FileMissingException& e)
      {
            // If file doesn't exist, issue a warning
         cerr << "SP3 file '" << sp3File << "' doesn't exist or you don't "
              << "have permission to read it. Skipping it." << endl;

         continue;

      }

   }  // End of 'while ( (sp3File = confReader.fetchListValue( ... "

   //// ^^^^ Ephemeris handling ^^^^



   //// vvvv Tides handling vvvv

      // Object to compute tidal effects
   SolidTides solid;


      // Configure ocean loading model
   OceanLoading ocean;
   ocean.setFilename( confReader.getValue( "oceanLoadingFile", "DEFAULT" ) );


      // Numerical values (xp, yp) are pole displacements (arcsec).
   double xp( confReader.fetchListValueAsDouble( "poleDisplacements",
                                                 "DEFAULT" ) );
   double yp( confReader.fetchListValueAsDouble( "poleDisplacements",
                                                 "DEFAULT" ) );

      // Object to model pole tides
   PoleTides pole;
   pole.setXY( xp, yp );

   //// ^^^^ Tides handling ^^^^



   //// Starting preprocessing for ALL stations ////


      // We will read each section name, which is equivalent to station name
      // Station names will be read in alphabetical order
   string station;
   while ( (station = confReader.getEachSection()) != "" )
   {

         // We will skip 'DEFAULT' section because we are waiting for
         // a specific section for each receiver. However, if data is
         // missing we will look for it in 'DEFAULT' (see how we use method
         // 'setFallback2Default()' of 'ConfDataReader' object in 'spinUp()'
      if( station == "DEFAULT" )
      {
         continue;
      }


         // Show a message indicating that we are starting with this station
      cerr << "Starting processing for station: '" << station << "'." << endl;


         // Create input observation file stream
      RinexObsStream rin;

         // Enable exceptions
      rin.exceptions(ios::failbit);

         // Try to open Rinex observations file
      try
      {

            // Open Rinex observations file in read-only mode
         rin.open( confReader("rinexObsFile", station), std::ios::in );

      }
      catch(...)
      {

         cerr << "Problem opening file '"
              << confReader.getValue("rinexObsFile", station)
              << "'." << endl;

         cerr << "Maybe it doesn't exist or you don't have "
              << "proper read permissions."
              << endl;

         cerr << "Skipping receiver '" << station << "'."
              << endl;

            // Close current Rinex observation stream
         rin.close();

         continue;

      }  // End of 'try-catch' block



         // Load station nominal position
      double xn(confReader.fetchListValueAsDouble("nominalPosition",station));
      double yn(confReader.fetchListValueAsDouble("nominalPosition",station));
      double zn(confReader.fetchListValueAsDouble("nominalPosition",station));
         // The former peculiar code is possible because each time we
         // call a 'fetchListValue' method, it takes out the first element
         // and deletes it from the given variable list.

      Position nominalPos( xn, yn, zn );


         // Create a 'ProcessingList' object where we'll store
         // the processing objects in order
      ProcessingList pList;


         // This object will check that all required observables are present
      RequireObservables requireObs;
      requireObs.addRequiredType(TypeID::P2);
      requireObs.addRequiredType(TypeID::L1);
      requireObs.addRequiredType(TypeID::L2);

         // This object will check that code observations are within
         // reasonable limits
      SimpleFilter pObsFilter;
      pObsFilter.setFilteredType(TypeID::P2);

         // Read if we should use C1 instead of P1
      bool usingC1( confReader.getValueAsBoolean( "useC1", station ) );
      if ( usingC1 )
      {
         requireObs.addRequiredType(TypeID::C1);
         pObsFilter.addFilteredType(TypeID::C1);
      }
      else
      {
         requireObs.addRequiredType(TypeID::P1);
         pObsFilter.addFilteredType(TypeID::P1);
      }

         // Add 'requireObs' to processing list (it is the first)
      pList.push_back(requireObs);


         // IMPORTANT NOTE:
         // It turns out that some receivers don't correct their clocks
         // from drift.
         // When this happens, their code observations may drift well beyond
         // what it is usually expected from a pseudorange. In turn, this
         // effect causes that "SimpleFilter" objects start to reject a lot of
         // satellites.
         // Thence, the "filterCode" option allows you to deactivate the
         // "SimpleFilter" object that filters out C1, P1 and P2, in case you
         // need to.
      bool filterCode( confReader.getValueAsBoolean( "filterCode", station ) );

         // Check if we are going to use this "SimpleFilter" object or not
      if( filterCode )
      {
         pList.push_back(pObsFilter);       // Add to processing list
      }


         // This object defines several handy linear combinations
      LinearCombinations comb;


         // Object to compute linear combinations for cycle slip detection
      ComputeLinear linear1;

         // Read if we should use C1 instead of P1
      if ( usingC1 )
      {
         linear1.addLinear(comb.pdeltaCombWithC1);
         linear1.addLinear(comb.mwubbenaCombWithC1);
      }
      else
      {
         linear1.addLinear(comb.pdeltaCombination);
         linear1.addLinear(comb.mwubbenaCombination);
      }
      linear1.addLinear(comb.ldeltaCombination);
      linear1.addLinear(comb.liCombination);
      pList.push_back(linear1);       // Add to processing list


         // Objects to mark cycle slips
      LICSDetector2 markCSLI2;         // Checks LI cycle slips
      pList.push_back(markCSLI2);      // Add to processing list
      MWCSDetector  markCSMW;          // Checks Merbourne-Wubbena cycle slips
      pList.push_back(markCSMW);       // Add to processing list


         // Object to keep track of satellite arcs
      SatArcMarker markArc;
      markArc.setDeleteUnstableSats(true);
      markArc.setUnstablePeriod(151.0);
      pList.push_back(markArc);       // Add to processing list


         // Declare a basic modeler
      BasicModel basic(nominalPos, SP3EphList);

         // Set the minimum elevation
      basic.setMinElev(confReader.getValueAsDouble("cutOffElevation",station));

         // If we are going to use P1 instead of C1, we must reconfigure 'basic'
      if ( !usingC1 )
      {
         basic.setDefaultObservable(TypeID::P1);
      }


         // Add to processing list
      pList.push_back(basic);


         // Object to remove eclipsed satellites
      EclipsedSatFilter eclipsedSV;
      pList.push_back(eclipsedSV);       // Add to processing list


         // Object to compute gravitational delay effects
      GravitationalDelay grDelay(nominalPos);
      pList.push_back(grDelay);       // Add to processing list


         // Vector from monument to antenna ARP [UEN], in meters
      double uARP(confReader.fetchListValueAsDouble( "offsetARP", station ) );
      double eARP(confReader.fetchListValueAsDouble( "offsetARP", station ) );
      double nARP(confReader.fetchListValueAsDouble( "offsetARP", station ) );
      Triple offsetARP( uARP, eARP, nARP );


         // Declare some antenna-related variables
      Triple offsetL1( 0.0, 0.0, 0.0 ), offsetL2( 0.0, 0.0, 0.0 );
      AntexReader antexReader;
      Antenna receiverAntenna;

         // Check if we want to use Antex information
      bool useantex( confReader.getValueAsBoolean( "useAntex", station ) );
      if( useantex )
      {
            // Feed Antex reader object with Antex file
         antexReader.open( confReader.getValue( "antexFile", station ) );

            // Get receiver antenna parameters
         receiverAntenna =
            antexReader.getAntenna( confReader.getValue( "antennaModel",
                                                         station ) );

      }


         // Object to compute satellite antenna phase center effect
      ComputeSatPCenter svPcenter(nominalPos);
      if( useantex )
      {
            // Feed 'ComputeSatPCenter' object with 'AntexReader' object
         svPcenter.setAntexReader( antexReader );
      }

      pList.push_back(svPcenter);       // Add to processing list


         // Declare an object to correct observables to monument
      CorrectObservables corr(SP3EphList);
      corr.setNominalPosition(nominalPos);
      corr.setMonument( offsetARP );

         // Check if we want to use Antex patterns
      bool usepatterns(confReader.getValueAsBoolean("usePCPatterns", station ));
      if( useantex && usepatterns )
      {
         corr.setAntenna( receiverAntenna );

            // Should we use elevation/azimuth patterns or just elevation?
         corr.setUseAzimuth(confReader.getValueAsBoolean("useAzim", station));
      }
      else
      {
            // Fill vector from antenna ARP to L1 phase center [UEN], in meters
         offsetL1[0] = confReader.fetchListValueAsDouble("offsetL1", station);
         offsetL1[1] = confReader.fetchListValueAsDouble("offsetL1", station);
         offsetL1[2] = confReader.fetchListValueAsDouble("offsetL1", station);

            // Vector from antenna ARP to L2 phase center [UEN], in meters
         offsetL2[0] = confReader.fetchListValueAsDouble("offsetL2", station);
         offsetL2[1] = confReader.fetchListValueAsDouble("offsetL2", station);
         offsetL2[2] = confReader.fetchListValueAsDouble("offsetL2", station);

         corr.setL1pc( offsetL1 );
         corr.setL2pc( offsetL2 );

      }


      pList.push_back(corr);       // Add to processing list


         // Object to compute wind-up effect
      ComputeWindUp windup( SP3EphList,
                            nominalPos,
                            confReader.getValue( "satDataFile", station ) );
      pList.push_back(windup);       // Add to processing list


         // Declare a NeillTropModel object, setting its parameters
      NeillTropModel neillTM( nominalPos.getAltitude(),
                              nominalPos.getGeodeticLatitude(),
                              confReader.getValueAsInt("dayOfYear", station) );


         // Object to compute the tropospheric data
      ComputeTropModel computeTropo(neillTM);
      pList.push_back(computeTropo);       // Add to processing list


         // Object to compute ionosphere-free combinations to be used
         // as observables in the PPP processing
      ComputeLinear linear2;

         // Read if we should use C1 instead of P1
      if ( usingC1 )
      {
            // WARNING: When using C1 instead of P1 to compute PC combination,
            //          be aware that instrumental errors will NOT cancel,
            //          introducing a bias that must be taken into account by
            //          other means. This won't be taken into account in this
            //          example.
         linear2.addLinear(comb.pcCombWithC1);
      }
      else
      {
         linear2.addLinear(comb.pcCombination);
      }
      linear2.addLinear(comb.lcCombination);
      pList.push_back(linear2);       // Add to processing list


         // Declare a simple filter object to screen PC
      SimpleFilter pcFilter;
      pcFilter.setFilteredType(TypeID::PC);

         // IMPORTANT NOTE:
         // Like in the "filterCode" case, the "filterPC" option allows you to
         // deactivate the "SimpleFilter" object that filters out PC, in case
         // you need to.
      bool filterPC( confReader.getValueAsBoolean( "filterPC", station ) );

         // Check if we are going to use this "SimpleFilter" object or not
      if( filterPC )
      {
         pList.push_back(pcFilter);       // Add to processing list
      }


         // Object to align phase with code measurements
      PhaseCodeAlignment phaseAlign;
      pList.push_back(phaseAlign);       // Add to processing list


         // Object to compute prefit-residuals
      ComputeLinear linear3(comb.pcPrefit);
      linear3.addLinear(comb.lcPrefit);
      pList.push_back(linear3);       // Add to processing list


         // Declare a base-changing object: From ECEF to North-East-Up (NEU)
      XYZ2NEU baseChange(nominalPos);
         // We always need both ECEF and NEU data for 'ComputeDOP', so add this
      pList.push_back(baseChange);


         // Object to compute DOP values
      ComputeDOP cDOP;
      pList.push_back(cDOP);       // Add to processing list



         // This is the GNSS data structure that will hold all the
         // GNSS-related information
      gnssRinex gRin;



         // Let's check if we are going to print the model
      bool printmodel( confReader.getValueAsBoolean( "printModel", station ) );

      string modelName;
      ofstream modelfile;

         // Prepare for model printing
      if( printmodel )
      {
         modelName = confReader.getValue( "modelFile", station );
         modelfile.open( modelName.c_str(), ios::out );
      }


         // *** The REAL preprocessing of a SPECIFIC station is done here *** //


         // Loop over all data epochs
      while(rin >> gRin)
      {

            // Store current epoch
         DayTime time(gRin.header.epoch);

            // Compute solid, oceanic and pole tides effects at this epoch
         Triple tides( solid.getSolidTide( time, nominalPos )  +
                       ocean.getOceanLoading( station, time )  +
                       pole.getPoleTide( time, nominalPos )    );


            // Update observable correction object with tides information
         corr.setExtraBiases(tides);

         try
         {

               // Let's preprocess data. Thanks to 'ProcessingList' this is
               // very simple and compact: Just one line of code!!!.
            gRin >> pList;

         }
         catch(Exception& e)
         {
            cerr << "Exception for receiver '" << station <<
                    "' at epoch: " << time << "; " << e << endl;
            continue;
         }
         catch(...)
         {
            cerr << "Unknown exception for receiver '" << station <<
                    " at epoch: " << time << endl;
            continue;
         }


            // Ask if we are going to print the model
         if ( printmodel )
         {
            printModel( modelfile,
                        gRin );

         }


            // Store observation data
         gdsMap.addGnssRinex(gRin);


         // The given epoch has been preprocessed. Let's get the next one

      }  // End of 'while(rin >> gRin)'


         // Get source
         // NOTE: 'station' is a 'string', and we need a 'SourceID'
      SourceID source( gRin.header.source );


         // Store dry tropospheric delay value to use it in the solution
      tropoMap[ source ] = neillTM.dry_zenith_delay();



         //// Here ends the preprocessing of all data for the given station ////


         // Let's check what kind of station this is
      if( confReader.getValueAsBoolean( "masterStation", station ) )
      {
         master = source;
      }
      else
      {
         if( confReader.getValueAsBoolean( "roverStation", station ) )
         {
            rover = source;
         }
         else
         {
            if( confReader.getValueAsBoolean( "refStation", station ) )
            {
                  // Note that 'reference' stations form a set
               refStationSet.insert( source );
            }
         }
      }

         // Close current Rinex observation stream
      rin.close();


         // If we printed the model, we must close the file
      if ( printmodel )
      {
            // Close model file for this station
         modelfile.close();
      }


         // We are done with this station. Let's show a message
      cerr << "Processing finished for station: '" << station;
      if ( printmodel )
      {
         cerr << "'. Model in file: '" << modelName;
      }
      cerr << "'." << endl;


   }  // End of 'while ( (station = confReader.getEachSection()) != "" )'


   //// End of preprocessing for ALL stations ////


      // Clear content of SP3 ephemerides object
   SP3EphList.clear();


      // The rest of the processing will be in method 'example14::shutDown()'
   return;

}  // End of 'example14::process()'



void example14::shutDown()
{

      //// In the resolution part we start configuring the general solver ////

      // Declare stochastic models to be used
      // The coordinates are handled as white noise with 100 m of sigma
   WhiteNoiseModel coordinatesModel(100.0);
   TropoRandomWalkModel tropoModel;
   PhaseAmbiguityModel ambiModel;

      // This variables are, by default, SourceID-indexed
   Variable dLat( TypeID::dLat, &coordinatesModel, true, false, 100.0 );
   Variable dLon( TypeID::dLon, &coordinatesModel, true, false, 100.0 );
   Variable dH( TypeID::dH, &coordinatesModel, true, false, 100.0 );

   Variable cdt( TypeID::cdt );
   cdt.setDefaultForced(true);    // Force the default coefficient (1.0)

   Variable tropo( TypeID::wetMap, &tropoModel, true, false, 10.0  );


      // The following variable is, SourceID and SatID-indexed
   Variable ambi( TypeID::BLC, &ambiModel, true, true );
   ambi.setDefaultForced(true);        // Force the default coefficient (1.0)

      // This variable will be SatID-indexed only
   Variable satClock( TypeID::dtSat );
   satClock.setSourceIndexed(false);
   satClock.setSatIndexed(true);
   satClock.setDefaultForced(true);    // Force the default coefficient (1.0)

      // This will be the independent term for code equations
   Variable prefitC( TypeID::prefitC );

      // This will be the independent term for phase equations
   Variable prefitL( TypeID::prefitL );


      // Rover code equation description
   Equation equPCRover( prefitC );
   equPCRover.addVariable(dLat);
   equPCRover.addVariable(dLon);
   equPCRover.addVariable(dH);
   equPCRover.addVariable(cdt);
   equPCRover.addVariable(tropo);
   equPCRover.addVariable(satClock);

      // Set the source of the equation
   equPCRover.header.equationSource = rover;

      // Rover phase equation description
   Equation equLCRover( prefitL );
   equLCRover.addVariable(dLat);
   equLCRover.addVariable(dLon);
   equLCRover.addVariable(dH);
   equLCRover.addVariable(cdt);
   equLCRover.addVariable(tropo);
   equLCRover.addVariable(ambi);
   equLCRover.addVariable(satClock);

      // Rover phase equation has more weight
   equLCRover.setWeight(10000.0);     // 100.0 * 100.0

      // Set the source of the equation
   equLCRover.header.equationSource = rover;



      // Reference stations code equation description
   Equation equPCRef( prefitC );
   equPCRef.addVariable(cdt);
   equPCRef.addVariable(tropo);
   equPCRef.addVariable(satClock);

      // Set the source of the equation
   equPCRef.header.equationSource = Variable::someSources;

      // Reference stations phase equation description
   Equation equLCRef( prefitL );
   equLCRef.addVariable(cdt);
   equLCRef.addVariable(tropo);
   equLCRef.addVariable(ambi);
   equLCRef.addVariable(satClock);

      // Reference station phase equation has more weight
   equLCRef.setWeight(10000.0);     // 100.0 * 100.0

      // Set the source of the equation
   equLCRef.header.equationSource = Variable::someSources;

      // Add all reference stations. Remember that they form a set
   for( std::set<SourceID>::const_iterator itSet = refStationSet.begin();
        itSet != refStationSet.end();
        ++itSet )
   {
      equPCRef.addSource2Set( (*itSet) );
      equLCRef.addSource2Set( (*itSet) );
   }


      // Master station code equation description
   Equation equPCMaster( prefitC );
   equPCMaster.addVariable(tropo);
   equPCMaster.addVariable(satClock);

      // Set the source of the equation
   equPCMaster.header.equationSource = master;

      // Master station phase equation description
   Equation equLCMaster( prefitL );
   equLCMaster.addVariable(tropo);
   equLCMaster.addVariable(ambi);
   equLCMaster.addVariable(satClock);

      // Master station phase equation has more weight
   equLCMaster.setWeight(10000.0);     // 100.0 * 100.0

      // Set the source of the equation
   equLCMaster.header.equationSource = master;


      // Setup equation system
   EquationSystem system;
   system.addEquation(equPCRover);
   system.addEquation(equLCRover);
   system.addEquation(equPCRef);
   system.addEquation(equLCRef);
   system.addEquation(equPCMaster);
   system.addEquation(equLCMaster);

   SolverGeneral solverGen(system);


      //// The general solver is configured ////


      //// Let's solve the equation system ////

      // Prepare for printing
   int precision( confReader.getValueAsInt( "precision", "DEFAULT" ) );
   cout << fixed << setprecision( precision );


      // Repeat while there is preprocesed data available
   while( !gdsMap.empty() )
   {

         // Get data out of GDS, but only the first epoch
      gnssDataMap gds( gdsMap.frontEpoch() );

         // Remove first element (i.e., we remove the first epoch)
      gdsMap.pop_front_epoch();

         // Extract current epoch
      DayTime workEpoch( (*gds.begin()).first );


         // Compute the solution. This is it!!!
      solverGen.Process( gds );


         // Let's print
      try
      {
         cout << workEpoch.DOYsecond() << " "                           // #1
              << solverGen.getSolution( TypeID::dLat, rover ) << " "    // #2
              << solverGen.getSolution( TypeID::dLon, rover ) << " "    // #3
              << solverGen.getSolution( TypeID::dH,   rover ) << " "    // #4
              << solverGen.getSolution( TypeID::wetMap, rover )
                 + 0.1 + tropoMap[ rover ] << endl;                     // #5
      }
      catch(...)
      {
         cerr << "Exception for receiver '" << rover <<
                 " at epoch: " << workEpoch.DOYsecond() << endl;

         continue;
      }

   }  // End of 'while( !gdsMap.empty() )'


   //// We are done ////


}  // End of 'example14::shutDown()'



   // Main function
int main(int argc, char* argv[])
{

   try
   {

      example14 program(argv[0]);

         // We are disabling 'pretty print' feature to keep
         // our description format
      if ( !program.initialize(argc, argv, false) )
      {
         return 0;
      }

      if ( !program.run() )
      {
         return 1;
      }

      return 0;

   }
   catch(Exception& e)
   {

      cerr << "Problem: " << e << endl;

      return 1;

   }
   catch(...)
   {

      cerr << "Unknown error." << endl;

      return 1;

   }

   return 0;

}  // End of 'main()'
