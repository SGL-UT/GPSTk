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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2010
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

// Example program Nro 17 for GPSTk
//
// This program shows how to use GNSS Data Structures (GDS) and other classes
// to build a reasonable complete application that implements the "Extended
// Velocity and Acceleration determination method (EVA)" to compute the
// velocity and acceleration of a rover receiver.
//
// For details on the EVA algorithm please consult:
//
//    Salazar, D., Hernandez-Pajares, M., Juan, J.M., Sanz J. and
//       A. Aragon-Angel. "EVA: GPS-based extended velocity and acceleration
//       determination". Journal of Geodesy. Volume 85, Issue 6, pp. 329-340,
//       DOI: 10.1007/s00190-010-0439-6, 2011.
//
//    Salazar, D. (2010). "Precise GPS-based position, velocity and
//       acceleration determination: Algorithms and tools". Ph.D. dissertation.
//       Doctoral Program in Aerospace Science & Technology, Universitat
//       Politecnica de Catalunya, Barcelona, Spain.
//
// For details on the Kennedy method please consult:
//
//    Kennedy, S. (2002). Acceleration Estimation from GPS Carrier Phases for
//       Airborne Gravimetry. PhD thesis, Department of Geomatics Engineering,
//       University of Clagary, Calgary, Alberta, Canada. Report No. 20160.
//
//    Kennedy, S. (2002). Precise Acceleration Determination from Carrier Phase
//       Measurements. In: Proceedings of the 15th International Technical
//       Meeting of the Satellite Division of the Institute of Navigation.
//       ION GPS 2002, Portland, Oregon, USA, pp. 962–972.
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

   // Class to numerically compute the derivatives.
#include "Differentiator.hpp"

   // Class to compute simple weights.
#include "ComputeSimpleWeights.hpp"

   // Classes to find the nominal position of the rover
#include "PCSmoother.hpp"
#include "ModelObs.hpp"
#include "ComputeMOPSWeights.hpp"
#include "SolverWMS.hpp"
#include "CivilTime.hpp"
#include "YDSTime.hpp"

using namespace std;
using namespace gpstk;


   // A new class is declared that will handle program behaviour
   // This class inherits from BasicFramework
class example17 : public BasicFramework
{
public:

      // Constructor declaration
   example17(char* arg0);


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


      // Declare our own methods to handle output

      // Method to print model values
   void printModel( ofstream& modelfile,
                    const gnssRinex& gData,
                    int   precision = 5 );


      // Method to synchronize data streams
   gnssDataMap synchronize( std::map<SourceID, std::list<gnssRinex> >& sMap,
                            SourceID syncStation,
                            double tol );


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

      // Map to store ROVER nominal positions
   std::map<CommonTime, Position> nominalPosMap;


}; // End of 'example17' class declaration


   // Let's implement constructor details
example17::example17(char* arg0)
   :
   BasicFramework(  arg0,
"\nThis program reads GPS receiver data from a configuration file and\n"
"process such data applying a 'Extended Velocity and Acceleration\n"
"determination' (EVA) strategy.\n\n"
"The output file format is as follows:\n\n"
" 1) Seconds of day\n"
" 2) VUp (m/s)\n"
" 3) VEast (m/s)\n"
" 4) VNorth (m/s)\n"
" 5) AUp (m/s^2)\n"
" 6) AEast (m/s^2)\n"
" 7) ANorth (m/s^2)\n"
" 8) Vx (m/s)\n"
" 9) Vy (m/s)\n"
"10) Vz (m/s)\n"
"11) Ax (m/s^2)\n"
"12) Ay (m/s^2)\n"
"13) Az (m/s^2)\n"
"14) cdtdot (m/s)\n"
"15) cdtdot2 (m/s^2)\n"
"16) Number of satellites\n" ),
      // Option initialization. "true" means a mandatory option
   confFile( CommandOption::stdType,
             'c',
             "conffile",
   " [-c|--conffile]    Name of configuration file ('evaconf.txt' by default).",
             false )
{

      // This option may appear just once at CLI
   confFile.setMaxCount(1);

}  // End of 'example17::example17'


   // Method to print model values
void example17::printModel( ofstream& modelfile,
                           const gnssRinex& gData,
                           int   precision )
{

      // Prepare for printing
   modelfile << fixed << setprecision( precision );

      // Get epoch out of GDS
   CommonTime time(gData.header.epoch);

      // Iterate through the GNSS Data Structure
   for ( satTypeValueMap::const_iterator it = gData.body.begin();
         it!= gData.body.end();
         it++ )
   {

         // Print epoch
      YDSTime yds(time);
      modelfile << yds.year         << "  ";    // Year           #1
      modelfile << yds.doy          << "  ";    // DayOfYear      #2
      modelfile << yds.sod          << "  ";    // SecondsOfDay   #3

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

}  // End of method 'example17::printModel()'



   // Method that will be executed AFTER initialization but BEFORE processing
void example17::spinUp()
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
         confReader.open( "evaconf.txt" );
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


}  // End of method 'example17::spinUp()'



   // Method that will really process information
void example17::process()
{

      /////////////////
      // What we want to apply to ALL the stations must be here
      /////////////////


   int precision( confReader.getValueAsInt( "precision", "DEFAULT" ) );


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


   // (1) ////////////////////////


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

         // Check which kind of station this is
      bool isMaster( confReader.getValueAsBoolean( "masterStation", station ) );
      bool isRover( confReader.getValueAsBoolean( "roverStation", station ) );
      bool isReference( confReader.getValueAsBoolean( "refStation", station ) );

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
      markCSLI2.setDeltaTMax(
               confReader.getValueAsDouble( "csLIdeltaT", station ) );
      markCSLI2.setMaxBufferSize(
               confReader.getValueAsInt( "csLIMaxBuffer", station ) );

      pList.push_back(markCSLI2);      // Add to processing list

      MWCSDetector  markCSMW;          // Checks Merbourne-Wubbena cycle slips
      markCSMW.setDeltaTMax(
               confReader.getValueAsDouble( "csMWdeltaT", station ) );

      pList.push_back(markCSMW);       // Add to processing list

         // Object to keep track of satellite arcs
      SatArcMarker markArc;
      markArc.setDeleteUnstableSats(
                  confReader.getValueAsBoolean( "deleteUnstable", station ) );

      markArc.setUnstablePeriod(
                  confReader.getValueAsDouble( "unstablePeriod", station ) );

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

         // Definition to compute prefit residual for velocity
      gnssLinearCombination lcVel;

         // Definition to compute prefit residual of LC
      lcVel.header                     = TypeID::dummy0;
      lcVel.body[TypeID::LC]           = +1.0;
      lcVel.body[TypeID::rel]          = -1.0;
      lcVel.body[TypeID::gravDelay]    = -1.0;
      lcVel.body[TypeID::satPCenter]   = -1.0;
      lcVel.body[TypeID::tropoSlant]   = -1.0;
         // Coefficient for LC windUp is LC wavelenght/2*PI
      lcVel.body[TypeID::windUp]       = -0.1069533781421467/TWO_PI;

         // Add to 'linear3' object
      linear3.addLinear( lcVel );
      pList.push_back(linear3);       // Add to processing list

         // Object to compute simple weights
      ComputeSimpleWeights simpleWeights;
      pList.push_back(simpleWeights);       // Add to processing list

         // Object to compute DOP values
      ComputeDOP cDOP;
      pList.push_back(cDOP);       // Add to processing list

         // This is the GNSS data structure that will hold all the
         // GNSS-related information
      gnssRinex gRin;


         //// MODEL PRINTING

         // Let's check if we are going to print the model
      bool printmodel( confReader.getValueAsBoolean( "printModel", station ) );

      string modelName;
      ofstream modelfile;

         // Prepare for model printing
      if( printmodel &&
          !isRover )
      {
         modelName = confReader.getValue( "modelFile", station );
         modelfile.open( modelName.c_str(), ios::out );
         modelfile << fixed << setprecision( precision );
      }


      //////////// OBJECTS FOR THE ROVER ////////////

         // Objects to mark cycle slips
      LICSDetector2 markCSLIPC;         // Checks LI cycle slips
      markCSLIPC.setDeltaTMax( 5.0 );

      MWCSDetector markCSMWPC;          // Checks Merbourne-Wubbena cycle slips
      markCSMWPC.setDeltaTMax( 5.0 );


         // Object to smooth the PC combination. Defaults are usually fine
      PCSmoother smoothPC;

         // Modeler object for PC
      ModelObs modelPC( nominalPos, neillTM, SP3EphList, TypeID::PC, false );

         // This object will compute the appropriate MOPS weights
      ComputeMOPSWeights mopsW( nominalPos, SP3EphList );

         // Solver
      SolverWMS solverPC;

      ////////////////////////////////////////


      //// *** Now it comes the REAL preprocessing part *** ////


         // Loop over all data epochs
      while(rin >> gRin)
      {

            // Store current epoch
         CommonTime time(gRin.header.epoch);

         if( isRover )
         {

               // Compute an approximate position
            gnssRinex gRinPC( gRin );

            try
            {

               gRinPC >> requireObs >> linear1
                      >> markCSLIPC >> markCSMWPC >> linear2
                      >> smoothPC >> pcFilter
                      >> modelPC >> mopsW >> solverPC;

            }
            catch(Exception& e)
            {
               cerr << "Exception preprocessing ROVER receiver at epoch: "
                    << CivilTime(time) << "; " << e << endl;
               continue;
            }
            catch(...)
            {
               cerr << "Unknown exception preprocessing ROVER receiver"
                    << " at epoch: " << CivilTime(time) << endl;
               continue;
            }

               // Get corrected correction
            Position solPos(nominalPos.X() + solverPC.getSolution(TypeID::dx),
                            nominalPos.Y() + solverPC.getSolution(TypeID::dy),
                            nominalPos.Z() + solverPC.getSolution(TypeID::dz));

               // Update nominal position
            nominalPos = solPos;

               // We must store nominal position because it will be used later
            nominalPosMap[ time ] = nominalPos;

               // Update modeler and other objects
            basic.rxPos = nominalPos;
            grDelay.setNominalPosition( nominalPos );
            svPcenter.setNominalPosition( nominalPos );
            corr.setNominalPosition( nominalPos );
            windup.setNominalPosition( nominalPos );
            neillTM.setReceiverHeight( nominalPos.height() );
            neillTM.setReceiverLatitude( nominalPos.geodeticLatitude() );
            mopsW.setPosition( nominalPos );
            modelPC.setDefaultTropoModel( neillTM );
            modelPC.Prepare( nominalPos );

            if( confReader.getValueAsBoolean( "computeTides", "ROVER" ) )
            {

                  // Compute solid, oceanic and pole tides effects at this epoch
               Triple tides( solid.getSolidTide( time, nominalPos )  +
                             ocean.getOceanLoading( "LLIV", time )  +
                             pole.getPoleTide( time, nominalPos )    );

                  // Update observable correction object with tides information
               corr.setExtraBiases(tides);
            }

         }
         else
         {

               // Compute solid, oceanic and pole tides effects at this epoch
            Triple tides( solid.getSolidTide( time, nominalPos )  +
                          ocean.getOceanLoading( station, time )  +
                          pole.getPoleTide( time, nominalPos )    );

               // Update observable correction object with tides information
            corr.setExtraBiases(tides);

         }


         try
         {

               // Let's process data. Thanks to 'ProcessingList' this is
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
         if ( printmodel &&
              !isRover )
         {
            printModel( modelfile, gRin );
         }

            // Store observation data
         gdsMap.addGnssRinex(gRin);

         // The given epoch has been processed. Let's get the next one

      }  // End of 'while(rin >> gRin)'

         // Get source
         // NOTE: 'station' is a 'string', and we need a 'SourceID'
      SourceID source( gRin.header.source );

         // Store dry tropospheric delay value to use it in the solution
      tropoMap[ source ] = neillTM.dry_zenith_delay();

         // Let's check what kind of station this is
      if( isMaster )
      {
         master = source;
      }
      else
      {
         if( isRover )
         {
            rover = source;
         }
         else
         {
            if( isReference )
            {
               refStationSet.insert( source );
            }
         }
      }

         // Close current Rinex observation stream
      rin.close();

         // We are done with this station. Let's show a message
      cerr << "Processing finished for station: '" << station << "'." << endl;

   }  // End of 'while ( (station = confReader.getEachSection()) != "" )'


   ///// End of (1)


      // Clear content of SP3 ephemerides object
   SP3EphList.clear();


      // The rest of the processing will be in method 'example17::shutDown()'

   cerr << "End of Preprocessing" << endl;

   return;

}  // End of 'example17::process()'


   // Method 'example17::shutDown()'
void example17::shutDown()
{

      ///// Start of derivatives computation

   double samplingP( confReader.getValueAsDouble( "samplingPeriod",
                                                  "DEFAULT" ) );

   double diffTol( confReader.getValueAsDouble( "diffTolerance",
                                                "DEFAULT" ) );

      // Declare 'Differentiator' objects

      // The following object will differentiate "prefitL" and put the result
      // into "LCdot". This is not exactly right (it should be "prefitLdot" or
      // something like that), but it is good enough for out plans.
   Differentiator diff1( TypeID::dummy0, TypeID::LCdot, samplingP, diffTol );
   Differentiator diff2( TypeID::LCdot, TypeID::LCdot2, samplingP, diffTol );
   Differentiator diff3( TypeID::rho, TypeID::rhodot, samplingP, diffTol );

      // Objects to compute satellite velocities
   Differentiator diff5( TypeID::satX, TypeID::satVX, samplingP, diffTol );
   Differentiator diff6( TypeID::satY, TypeID::satVY, samplingP, diffTol );
   Differentiator diff7( TypeID::satZ, TypeID::satVZ, samplingP, diffTol );

      // Objects to compute satellite accelerations
   Differentiator diff8( TypeID::satVX, TypeID::satAX, samplingP, diffTol );
   Differentiator diff9( TypeID::satVY, TypeID::satAY, samplingP, diffTol );
   Differentiator diff10( TypeID::satVZ, TypeID::satAZ, samplingP, diffTol );

   cerr << "Starting to compute derivatives" << endl;

      // Compute derivatives
   diff1.Process(gdsMap);
   diff2.Process(gdsMap);
   diff3.Process(gdsMap);

   diff5.Process(gdsMap);
   diff6.Process(gdsMap);
   diff7.Process(gdsMap);

   diff8.Process(gdsMap);
   diff9.Process(gdsMap);
   diff10.Process(gdsMap);

      ///// End of derivatives computation

   cerr << "End of Differencing" << endl;

   cerr << "Starting velocity prefits computation" << endl;

      // We will need a new data structure
   gnssDataMap newDataMap;


   while( !gdsMap.empty() )
   {

         // Extract the first epoch of data from the map
      gnssDataMap gdsData( gdsMap.frontEpoch() );

         // We need to iterate through all the stations inside 'gdsData'
      for( gnssDataMap::const_iterator gdmIt = gdsData.begin();
           gdmIt != gdsData.end();
           ++gdmIt )
      {

            // Iterate through all the receivers
         for( sourceDataMap::const_iterator sdmIt = (*gdmIt).second.begin();
              sdmIt != (*gdmIt).second.end();
              ++sdmIt )
         {

               // Get data structure corresponding to current SourceID
            gnssRinex gRin( gdsData.getGnssRinex( (*sdmIt).first ) );

               // Get all the satellites inside this structure
            SatIDSet satSet( gRin.getSatID() );

               // Iterate through all the satellites
            for( SatIDSet::const_iterator itSat = satSet.begin();
                 itSat != satSet.end();
                 ++itSat )
            {

                  // Declare a variable for 'prefit'
               double prefit( 0.0 );

               try
               {

                     // Get geometry-related values
                  double dxQM( - gRin.getValue( (*itSat), TypeID::dx ) );
                  double dyQM( - gRin.getValue( (*itSat), TypeID::dy ) );
                  double dzQM( - gRin.getValue( (*itSat), TypeID::dz ) );

                     // Get satellite velocity-related values
                  double satVXQ( gRin.getValue( (*itSat), TypeID::satVX ) );
                  double satVYQ( gRin.getValue( (*itSat), TypeID::satVY ) );
                  double satVZQ( gRin.getValue( (*itSat), TypeID::satVZ ) );

                     // Get the differentiated prefilter residual
                  double prefitLCdotQM(gRin.getValue((*itSat), TypeID::LCdot));

                  prefit = prefitLCdotQM - ( dxQM * satVXQ
                                           + dyQM * satVYQ
                                           + dzQM * satVZQ );

                     // Insert the new value into data structure
                  gRin( (*itSat) )( TypeID::LCdot ) = prefit;

               }
               catch(...)
               {
                     // Skip this satellite if it doesn't have all the data
                  continue;
               }

            }  // End of 'for( SatIDSet::const_iterator itSat = ...'

               // Insert the current gRin into new gnssDataMap
            newDataMap.addGnssRinex( gRin );

         }  // End of 'for( sourceDataMap::const_iterator sdmIt = ...'

      }  // End of 'for( gnssDataMap::const_iterator gdmIt =  ...'

         // Remove first element
      gdsMap.pop_front_epoch();

   }  // End of 'while( !gdsMap.empty() )'

   cerr << "End of velocity prefits computation" << endl;

      // Declare stochastic models to be used
   WhiteNoiseModel velocitiesModel(200.0);

      // This variables are, by default, SourceID-indexed
   Variable vx( TypeID::dx, &velocitiesModel, true, false, 200.0 );
   Variable vy( TypeID::dy, &velocitiesModel, true, false, 200.0 );
   Variable vz( TypeID::dz, &velocitiesModel, true, false, 200.0 );

   Variable cdt( TypeID::cdt );
   cdt.setDefaultForced(true);    // Force the default coefficient (1.0)

      // This variable will be SatID-indexed only
   Variable satClock( TypeID::dtSat );
   satClock.setSourceIndexed(false);
   satClock.setSatIndexed(true);
      // NOTE: Coefficient should be -1.0, but it is O.K. like this
   satClock.setDefaultForced(true);    // Force the default coefficient (1.0)

      // This will be the independent term for phase equations
   Variable prefitL( TypeID::LCdot );

      // Rover phase equation description
   Equation equLCRover( prefitL );
   equLCRover.addVariable(vx);
   equLCRover.addVariable(vy);
   equLCRover.addVariable(vz);
   equLCRover.addVariable(cdt);
   equLCRover.addVariable(satClock);

      // Set the source of the equation
   equLCRover.header.equationSource = rover;

      // Reference stations phase equation description
   Equation equLCRef( prefitL );
   equLCRef.addVariable(cdt);
   equLCRef.addVariable(satClock);

      // Set the source of the equation
   equLCRef.header.equationSource = Variable::someSources;

      // Add all reference stations
   for( std::set<SourceID>::const_iterator itSet = refStationSet.begin();
        itSet != refStationSet.end();
        ++itSet )
   {
      equLCRef.addSource2Set( (*itSet) );
   }

      // Master station phase equation description
   Equation equLCMaster( prefitL );
   equLCMaster.addVariable(satClock);

      // Set the source of the equation
   equLCMaster.header.equationSource = master;

      // Setup equation system
   EquationSystem system;
   system.addEquation(equLCRover);
   system.addEquation(equLCRef);
   system.addEquation(equLCMaster);

   SolverGeneral solverGen(system);


      //// Now, the variables, equations and solver for acceleration


      // Declare stochastic models to be used
   WhiteNoiseModel accelModel(20.0);

      // This variables are, by default, SourceID-indexed
   Variable ax( TypeID::dx, &accelModel, true, false, 20.0 );
   Variable ay( TypeID::dy, &accelModel, true, false, 20.0 );
   Variable az( TypeID::dz, &accelModel, true, false, 20.0 );

   Variable cdtdot2( TypeID::cdt );
   cdtdot2.setDefaultForced(true);    // Force the default coefficient (1.0)

      // This variable will be SatID-indexed only
   Variable satClockdot2( TypeID::dtSat );
   satClockdot2.setSourceIndexed(false);
   satClockdot2.setSatIndexed(true);
      // NOTE: Coefficient should be -1.0, but it is O.K. like this
   satClockdot2.setDefaultForced(true);  // Force the default coefficient (1.0)

      // This will be the independent term
   Variable prefitLdot2( TypeID::LCdot2 );

      // Rover phase equation description
   Equation equLCRoverdot2( prefitLdot2 );
   equLCRoverdot2.addVariable(ax);
   equLCRoverdot2.addVariable(ay);
   equLCRoverdot2.addVariable(az);
   equLCRoverdot2.addVariable(cdtdot2);
   equLCRoverdot2.addVariable(satClockdot2);

      // Set the source of the equation
   equLCRoverdot2.header.equationSource = rover;

      // Reference stations phase equation description
   Equation equLCRefdot2( prefitLdot2 );
   equLCRefdot2.addVariable(cdtdot2);
   equLCRefdot2.addVariable(satClockdot2);

      // Set the source of the equation
   equLCRefdot2.header.equationSource = Variable::someSources;

      // Add all reference stations
   for( std::set<SourceID>::const_iterator itSet = refStationSet.begin();
        itSet != refStationSet.end();
        ++itSet )
   {
      equLCRefdot2.addSource2Set( (*itSet) );
   }

      // Master station phase equation description
   Equation equLCMasterdot2( prefitLdot2 );
   equLCMasterdot2.addVariable(satClockdot2);

      // Set the source of the equation
   equLCMasterdot2.header.equationSource = master;

      // Setup equation system
   EquationSystem systemAccel;
   systemAccel.addEquation(equLCRoverdot2);
   systemAccel.addEquation(equLCRefdot2);
   systemAccel.addEquation(equLCMasterdot2);

   SolverGeneral solverAccel( systemAccel );


      //// We are done with the acceleration solver


      // File to store nominal positions
   ofstream nomPosFile ("rover-nominalPos-va.dat");

      // Prepare for printing
   int precision( confReader.getValueAsInt( "precision", "DEFAULT" ) );
   cout << fixed << setprecision( precision );
   nomPosFile << fixed << setprecision( precision );

      // Let's check if we are going to print the model
   bool printmodel( confReader.getValueAsBoolean( "printModel", "ROVER" ) );

   string modelName;
   ofstream modelfile;

      // Prepare for model printing
   if( printmodel )
   {
      modelName = confReader.getValue( "modelFile", "ROVER" );
      modelfile.open( modelName.c_str(), ios::out );
      modelfile << fixed << setprecision( precision );
   }

   cerr << "Starting velocity/acceleration solution computation" << endl;

   while( !newDataMap.empty() )
   {

         // Get data
      gnssDataMap gds( newDataMap.frontEpoch() );

         // New data structure, just for acceleration
      gnssDataMap gdsAccel( gds );

         // This data structure will be processed for acceleration
      gnssDataMap newgdsAccel;

         // Remove first element
      newDataMap.pop_front_epoch();

      gnssDataMap::const_iterator it( gds.begin() );

      CommonTime workEpoch( (*it).first );

         // We will need the data from the rover
      gnssRinex gRover( gds.getGnssRinex( rover ) );

         // Check if there are enough satellites in view
      if( gRover.numSats() >= 5 )
      {

         try
         {
               // Compute the solution for velocity
            solverGen.Process( gds );
         }
         catch(...)
         {
            cerr << ".";

            continue;
         }

            // Extract velocity solutions
         double vx( solverGen.getSolution(TypeID::dx, rover) );
         double vy( solverGen.getSolution(TypeID::dy, rover) );
         double vz( solverGen.getSolution(TypeID::dz, rover) );

            // Now, let's compute accelerations
            // We need to iterate through all the stations inside 'gdsAccel'
         for( gnssDataMap::const_iterator gdmIt = gdsAccel.begin();
              gdmIt != gdsAccel.end();
              ++gdmIt )
         {

               // Iterate through all the receivers
            for( sourceDataMap::const_iterator sdmIt = (*gdmIt).second.begin();
                 sdmIt != (*gdmIt).second.end();
                 ++sdmIt )
            {

                  // Get data structure corresponding to current SourceID
               gnssRinex gRin( gdsAccel.getGnssRinex( (*sdmIt).first ) );

                  // Get all the satellites inside this structure
               SatIDSet satSet( gRin.getSatID() );

                  // Iterate through all the satellites
               for( SatIDSet::const_iterator itSat = satSet.begin();
                    itSat != satSet.end();
                    ++itSat )
               {

                     // Declare a variable for 'prefit'
                  double prefit( 0.0 );

                  try
                  {

                        // Get geometry-related values
                     double dxQM( - gRin.getValue( (*itSat), TypeID::dx ) );
                     double dyQM( - gRin.getValue( (*itSat), TypeID::dy ) );
                     double dzQM( - gRin.getValue( (*itSat), TypeID::dz ) );

                     double rhoQM(   gRin.getValue( (*itSat), TypeID::rho ) );
                     double rhodotQM(gRin.getValue( (*itSat), TypeID::rhodot ));

                        // Get satellite velocity-related values
                     double satVXQ( gRin.getValue( (*itSat), TypeID::satVX ) );
                     double satVYQ( gRin.getValue( (*itSat), TypeID::satVY ) );
                     double satVZQ( gRin.getValue( (*itSat), TypeID::satVZ ) );

                        // Get satellite acceleration-related values
                     double satAXQ( gRin.getValue( (*itSat), TypeID::satAX ) );
                     double satAYQ( gRin.getValue( (*itSat), TypeID::satAY ) );
                     double satAZQ( gRin.getValue( (*itSat), TypeID::satAZ ) );

                        // Get the differentiated prefilter residual
                     double preLCdot2( gRin.getValue((*itSat),TypeID::LCdot2) );

                     double correction( 0.0 );

                        // Rover receiver will be handled a little different
                     if( (*sdmIt).first == rover )
                     {

                           // Compute relative speeds
                        double vQM2( ( satVXQ - vx ) * ( satVXQ - vx )
                                   + ( satVYQ - vy ) * ( satVYQ - vy )
                                   + ( satVZQ - vz ) * ( satVZQ - vz ) );

                        correction = ( vQM2 - ( rhodotQM * rhodotQM ) ) / rhoQM;
                     }
                     else
                     {

                           // Compute relative speeds
                        double vQM2( ( satVXQ * satVXQ )
                                   + ( satVYQ * satVYQ )
                                   + ( satVZQ * satVZQ ) );

                        correction = ( vQM2 - ( rhodotQM * rhodotQM ) ) / rhoQM;
                     }

                     prefit = preLCdot2 - ( dxQM * satAXQ
                                          + dyQM * satAYQ
                                          + dzQM * satAZQ )
                                        - correction;

                        // Insert the new value into data structure
                     gRin( (*itSat) )( TypeID::LCdot2 ) = prefit;

                  }
                  catch(...)
                  {
                        // Skip this satellite if it doesn't have all the data
                     continue;
                  }


               }  // End of 'for( SatIDSet::const_iterator itSat = ...'

                  // Insert the current gRin into new gnssDataMap
               newgdsAccel.addGnssRinex( gRin );

            }  // End of 'for( sourceDataMap::const_iterator sdmIt = ...'

         }  // End of 'for( gnssDataMap::const_iterator gdmIt =  ...'


         try
         {
               // Compute the solution for acceleration
            solverAccel.Process( newgdsAccel );
         }
         catch(...)
         {
            cerr << "x";

            continue;
         }

            // Extract acceleration solutions
         double ax( solverAccel.getSolution(TypeID::dx, rover) );
         double ay( solverAccel.getSolution(TypeID::dy, rover) );
         double az( solverAccel.getSolution(TypeID::dz, rover) );

            // Ask if we are going to print the model
         if ( printmodel )
         {
            gnssRinex gRoverModel( gds.getGnssRinex( rover ) );
            printModel( modelfile, gRoverModel );
         }

         try
         {

            Position nomPos( nominalPosMap[workEpoch] );

            double roverLat( nomPos.getGeodeticLatitude() );
            double roverLon( nomPos.getLongitude() );

               // Print nominal positions to their file
            nomPosFile << YDSTime(workEpoch).sod      << " "         // #1
                       << nomPos.X()                  << " "         // #2
                       << nomPos.Y()                  << " "         // #3
                       << nomPos.Z()                  << " "         // #4
                       << roverLat                    << " "         // #5
                       << roverLon                    << " "         // #6
                       << nomPos.getHeight()          << endl;       // #7

            Triple vECEF(vx, vy, vz);
            Triple vUEN( (vECEF.R3(roverLon)).R2(-roverLat) );

            Triple aECEF(ax, ay, az);
            Triple aUEN( (aECEF.R3(roverLon)).R2(-roverLat) );

            cout << YDSTime(workEpoch).sod            << " "  // seconds - #1
                 << vUEN[0]                           << " "  // vUp     - #2
                 << vUEN[1]                           << " "  // vEast   - #3
                 << vUEN[2]                           << " "  // vNorth  - #4
                 << aUEN[0]                           << " "  // aUp     - #5
                 << aUEN[1]                           << " "  // aEast   - #6
                 << aUEN[2]                           << " "  // aNorth  - #7
                 << vx                                << " "  // vx      - #8
                 << vy                                << " "  // vy      - #9
                 << vz                                << " "  // vz      - #10
                 << ax                                << " "  // ax      - #11
                 << ay                                << " "  // ay      - #12
                 << az                                << " "  // az      - #13
                 << solverGen.getSolution(TypeID::cdt, rover) << " "//cdtdot #14
                 << solverAccel.getSolution(TypeID::cdt,rover)<<" "//cdtdot2 #15
                 << gRover.numSats()                  << " "; // numSats - #16

            cout << endl;

         }
         catch(...)
         {
            cerr << "Exception printing solution for ROVER receiver at epoch: "
                 << workEpoch.DOYsecond() << endl;

            continue;
         }

      }

   }  // End of 'while( !newDataMap.empty() )'

      // Close file for nominal positions
   nomPosFile.close();

      // If we printed the model, we must close the file
   if ( printmodel )
   {
         // Close model file for this station
      modelfile.close();
   }


   cerr << endl;

   //////////////////////////////

}  // End of 'example17::shutDown()'


   // Main function
int main(int argc, char* argv[])
{

   try
   {

      example17 program(argv[0]);

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
