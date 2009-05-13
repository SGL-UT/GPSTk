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
//  Copyright Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009
//
//============================================================================

// Example program Nro 9 for GPSTk
//
// This program shows how to use GNSS Data Structures (GDS) and other classes
// to build a reasonable complete application that computes "Precise Point
// Positioning" (PPP).
//
// For details on the PPP algorithm please consult:
//
//    Kouba, J. and P. Heroux. "Precise Point Positioning using IGS Orbit
//       and Clock Products". GPS Solutions, vol 5, pp 2-28. October, 2001.
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

   // Used to decimate data. This is important because RINEX observation
   // data is provided with a 30 s sample rate, whereas SP3 files provide
   // satellite clock information with a 900 s sample rate.
#include "Decimate.hpp"

   // Class to compute the Precise Point Positioning (PPP) solution in
   // forwards-only mode.
#include "SolverPPP.hpp"

   // Class to compute the Precise Point Positioning (PPP) solution in
   // forwards-backwards mode.
#include "SolverPPPFB.hpp"

   // Class to read configuration files.
#include "ConfDataReader.hpp"


using namespace std;
using namespace gpstk;


   // A new class is declared that will handle program behaviour
   // This class inherits from BasicFramework
class example9 : public BasicFramework
{
public:

      // Constructor declaration
   example9(char* arg0);


protected:


      // Method that will take care of processing
   virtual void process();

      // Method that hold code to be run BEFORE processing
   virtual void spinUp();


private:


      // This field represents an option at command line interface (CLI)
   CommandOptionWithArg confFile;

      // If you want to share objects and variables among methods, you'd
      // better declare them here

      // Configuration file reader
   ConfDataReader confReader;


      // Declare our own methods to handle output


      // Method to print solution values
   void printSolution( ofstream& outfile,
                       const  SolverLMS& solver,
                       const  DayTime& time,
                       const  ComputeDOP& cDOP,
                       bool   useNEU,
                       int    numSats,
                       double dryTropo,
                       int    precision = 3 );


      // Method to print model values
   void printModel( ofstream& modelfile,
                    const gnssRinex& gData,
                    int   precision = 4 );


}; // End of 'example9' class declaration



   // Let's implement constructor details
example9::example9(char* arg0)
   :
   BasicFramework(  arg0,
"\nThis program reads GPS receiver data from a configuration file and\n"
"process such data applying a 'Precise Point Positioning' strategy.\n\n"
"Please consult the default configuration file, 'pppconf.txt', for\n"
"further details.\n\n"
"The output file format is as follows:\n\n"
" 1) Year\n"
" 2) Day of year\n"
" 3) Seconds of day\n"
" 4) dx/dLat (m)\n"
" 5) dy/dLon (m)\n"
" 6) dz/dH (m)\n"
" 7) Zenital Tropospheric Delay - zpd (m)\n"
" 8) Covariance of dx/dLat (m*m)\n"
" 9) Covariance of dy/dLon (m*m)\n"
"10) Covariance of dz/dH (m*m)\n"
"11) Covariance of Zenital Tropospheric Delay (m*m)\n"
"12) Number of satellites\n"
"13) GDOP\n"
"14) PDOP\n"
"15) TDOP\n"
"16) HDOP\n"
"17) VDOP\n" ),
      // Option initialization. "true" means a mandatory option
   confFile( CommandOption::stdType,
             'c',
             "conffile",
   " [-c|--conffile]    Name of configuration file ('pppconf.txt' by default).",
             false )
{

      // This option may appear just once at CLI
   confFile.setMaxCount(1);

}  // End of 'example9::example9'



   // Method to print solution values
void example9::printSolution( ofstream& outfile,
                              const SolverLMS& solver,
                              const DayTime& time,
                              const ComputeDOP& cDOP,
                              bool  useNEU,
                              int   numSats,
                              double dryTropo,
                              int   precision )
{

      // Prepare for printing
   outfile << fixed << setprecision( precision );


      // Print results
   outfile << time.year()        << "  ";    // Year           - #1
   outfile << time.DOY()         << "  ";    // DayOfYear      - #2
   outfile << time.DOYsecond()   << "  ";    // SecondsOfDay   - #3

   if( useNEU )
   {

      outfile << solver.getSolution(TypeID::dLat) << "  ";       // dLat  - #4
      outfile << solver.getSolution(TypeID::dLon) << "  ";       // dLon  - #5
      outfile << solver.getSolution(TypeID::dH) << "  ";         // dH    - #6
         // We add 0.1 meters to 'wetMap' because 'NeillTropModel' sets a
         // nominal value of 0.1 m. Also to get the total we have to add the
         // dry tropospheric delay value
                                                                 // ztd - #7
      outfile << solver.getSolution(TypeID::wetMap) + 0.1 + dryTropo << "  ";

      outfile << solver.getVariance(TypeID::dLat) << "  ";   // Cov dLat  - #8
      outfile << solver.getVariance(TypeID::dLon) << "  ";   // Cov dLon  - #9
      outfile << solver.getVariance(TypeID::dH) << "  ";     // Cov dH    - #10
      outfile << solver.getVariance(TypeID::wetMap) << "  "; // Cov ztd   - #11

   }
   else
   {

      outfile << solver.getSolution(TypeID::dx) << "  ";         // dx    - #4
      outfile << solver.getSolution(TypeID::dy) << "  ";         // dy    - #5
      outfile << solver.getSolution(TypeID::dz) << "  ";         // dz    - #6
         // We add 0.1 meters to 'wetMap' because 'NeillTropModel' sets a
         // nominal value of 0.1 m. Also to get the total we have to add the
         // dry tropospheric delay value
                                                                 // ztd - #7
      outfile << solver.getSolution(TypeID::wetMap) + 0.1 + dryTropo << "  ";

      outfile << solver.getVariance(TypeID::dx) << "  ";     // Cov dx    - #8
      outfile << solver.getVariance(TypeID::dy) << "  ";     // Cov dy    - #9
      outfile << solver.getVariance(TypeID::dz) << "  ";     // Cov dz    - #10
      outfile << solver.getVariance(TypeID::wetMap) << "  "; // Cov ztd   - #11

   }

   outfile << numSats << "  ";    // Number of satellites - #12

   outfile << cDOP.getGDOP()        << "  ";  // GDOP - #13
   outfile << cDOP.getPDOP()        << "  ";  // PDOP - #14
   outfile << cDOP.getTDOP()        << "  ";  // TDOP - #15
   outfile << cDOP.getHDOP()        << "  ";  // HDOP - #16
   outfile << cDOP.getVDOP()        << "  ";  // VDOP - #17

      // Add end-of-line
   outfile << endl;


   return;


}  // End of method 'example9::printSolution()'



   // Method to print model values
void example9::printModel( ofstream& modelfile,
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

}  // End of method 'example9::printModel()'





   // Method that will be executed AFTER initialization but BEFORE processing
void example9::spinUp()
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
         confReader.open( "pppconf.txt" );
      }
      catch(...)
      {

         cerr << "Problem opening default configuration file 'pppconf.txt'"
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


}  // End of method 'example9::spinUp()'



   // Method that will really process information
void example9::process()
{

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
      cout << "Starting processing for station: '" << station << "'." << endl;


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


         // Declare a "SP3EphemerisStore" object to handle precise ephemeris
      SP3EphemerisStore SP3EphList;

         // Set flags to reject satellites with bad or absent positional
         // values or clocks
      SP3EphList.rejectBadPositions(true);
      SP3EphList.rejectBadClocks(true);

         // Read if we should check for data gaps.
      if ( confReader.getValueAsBoolean( "checkGaps", station ) )
      {
         SP3EphList.enableDataGapCheck();
         SP3EphList.setGapInterval(
                     confReader.getValueAsDouble("SP3GapInterval",station) );
      }

         // Read if we should check for too wide interpolation intervals
      if ( confReader.getValueAsBoolean( "checkInterval", station ) )
      {
         SP3EphList.enableIntervalCheck();
         SP3EphList.setMaxInterval(
                     confReader.getValueAsDouble("maxSP3Interval",station) );
      }


         // Load all the SP3 ephemerides files from variable list
      string sp3File;
      while ( (sp3File = confReader.fetchListValue("SP3List",station) ) != "" )
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


         // Object to decimate data
      Decimate decimateData(900.0, 5.0, SP3EphList.getInitialTime());
      pList.push_back(decimateData);       // Add to processing list


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

         // We will need this value later for printing
      double drytropo( neillTM.dry_zenith_delay() );


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


         // Get if we want results in ECEF or NEU reference system
      bool isNEU( confReader.getValueAsBoolean( "USENEU", station ) );


         // Declare solver objects
      SolverPPP   pppSolver(isNEU);
      SolverPPPFB fbpppSolver(isNEU);


         // Get if we want 'forwards-backwards' or 'forwards' processing only
      int cycles( confReader.getValueAsInt("forwardBackwardCycles", station) );


         // Get if we want to process coordinates as white noise
      bool isWN( confReader.getValueAsBoolean( "coordinatesAsWhiteNoise",
                                               station ) );


         // White noise stochastic model
      WhiteNoiseModel wnM(100.0);      // 100 m of sigma


         // Decide what type of solver we will use for this station
      if ( cycles > 0 )
      {

            // In this case, we will use the 'forwards-backwards' solver

            // Check about coordinates as white noise
         if ( isWN )
         {
               // Reconfigure solver
            fbpppSolver.setCoordinatesModel(&wnM);
         }

            // Add solver to processing list
         pList.push_back(fbpppSolver);

      }
      else
      {

            // In this case, we will use the 'forwards-only' solver

            // Check about coordinates as white noise
         if ( isWN )
         {
               // Reconfigure solver
            pppSolver.setCoordinatesModel(&wnM);
         }

            // Add solver to processing list
         pList.push_back(pppSolver);

      }  // End of 'if ( cycles > 0 )'


         // Object to compute tidal effects
      SolidTides solid;


         // Configure ocean loading model
      OceanLoading ocean;
      ocean.setFilename( confReader.getValue( "oceanLoadingFile", station ) );


         // Numerical values (xp, yp) are pole displacements (arcsec).
      double xp( confReader.fetchListValueAsDouble( "poleDisplacements",
                                                    station ) );
      double yp( confReader.fetchListValueAsDouble( "poleDisplacements",
                                                    station ) );
         // Object to model pole tides
      PoleTides pole;
      pole.setXY( xp, yp );


         // This is the GNSS data structure that will hold all the
         // GNSS-related information
      gnssRinex gRin;


         // Prepare for printing
      int precision( confReader.getValueAsInt( "precision", station ) );


         // Let's open the output file
      string outName(confReader.getValue( "outputFile", station ) );

      ofstream outfile;
      outfile.open( outName.c_str(), ios::out );

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


         //// *** Now comes the REAL forwards processing part *** ////


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

               // Let's process data. Thanks to 'ProcessingList' this is
               // very simple and compact: Just one line of code!!!.
            gRin >> pList;

         }
         catch(DecimateEpoch& d)
         {
               // If we catch a DecimateEpoch exception, just continue.
            continue;
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

            // Check what type of solver we are using
         if ( cycles < 1 )
         {

               // This is a 'forwards-only' filter. Let's print to output
               // file the results of this epoch
            printSolution( outfile,
                           pppSolver,
                           time,
                           cDOP,
                           isNEU,
                           gRin.numSats(),
                           drytropo,
                           precision );

         }  // End of 'if ( cycles < 1 )'



         // The given epoch hass been processed. Let's get the next one

      }  // End of 'while(rin >> gRin)'


         // Close current Rinex observation stream
      rin.close();


         // If we printed the model, we must close the file
      if ( printmodel )
      {
            // Close model file for this station
         modelfile.close();
      }


         // Clear content of SP3 ephemerides object
      SP3EphList.clear();



         //// *** Forwards processing part is over *** ////



         // Now decide what to do: If solver was a 'forwards-only' version,
         // then we are done and should continue with next station.
      if ( cycles < 1 )
      {

            // Close output file for this station
         outfile.close();

            // We are done with this station. Let's show a message
         cout << "Processing finished for station: '" << station
              << "'. Results in file: '" << outName << "'." << endl;

            // Go process next station
         continue;

      }


         //// *** If we got here, it is a 'forwards-backwards' solver *** ////


         // Now, let's do 'forwards-backwards' cycles
      try
      {

         fbpppSolver.ReProcess(cycles);

      }
      catch(Exception& e)
      {

            // If problems arose, issue an message and skip receiver
         cerr << "Exception at reprocessing phase: " << e << endl;
         cerr << "Skipping receiver '" << station << "'." << endl;

            // Close output file for this station
         outfile.close();

            // Go process next station
         continue;

      }  // End of 'try-catch' block



         // Reprocess is over. Let's finish with the last processing

         // Loop over all data epochs, again, and print results
      while( fbpppSolver.LastProcess(gRin) )
      {

         DayTime time(gRin.header.epoch);

         printSolution( outfile,
                        fbpppSolver,
                        time,
                        cDOP,
                        isNEU,
                        gRin.numSats(),
                        drytropo,
                        precision );

      }  // End of 'while( fbpppSolver.LastProcess(gRin) )'


         // We are done. Close and go for next station

         // Close output file for this station
      outfile.close();


         // We are done with this station. Let's show a message
      cout << "Processing finished for station: '" << station
           << "'. Results in file: '" << outName << "'." << endl;


   }  // end of 'while ( (station = confReader.getEachSection()) != "" )'

   return;

}  // End of 'example9::process()'



   // Main function
int main(int argc, char* argv[])
{

   try
   {

      example9 program(argv[0]);

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
