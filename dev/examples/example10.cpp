#pragma ident "$Id$"
// Example program Nro 10 for GPSTk
// This program shows how to use GNSS Data Structures (GDS) to obtain
// phase-based differential positioning (single-differences).
//
// This example is basically a modification of "example8.cpp". For further
// information please go to the "Plublications" area in the GPSTk website:
//
//    http://www.gpstk.org/bin/view/Documentation/GPSTkPublications
//
// And consult:
//
//   Salazar, D., Hernandez-Pajares, M., Juan, J.M. and J. Sanz. "High accuracy
//   positioning using carrier-phases with the open source GPSTk software".
//   4th. ESA Workshop on Satellite Navigation User Equipment Technologies
//   NAVITEC 2008. Noordwijk. The Netherlands. December 2008.
//
// Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009

#include <iostream>
#include <iomanip>


   // Class for handling satellite observation parameters RINEX files
#include "RinexObsStream.hpp"

   // Class to store satellite precise navigation data
#include "SP3EphemerisStore.hpp"

   // Class in charge of basic GNSS signal modelling
#include "BasicModel.hpp"

   // Class to model the tropospheric delays
#include "TropModel.hpp"

   // Class defining the GNSS data structures
#include "DataStructures.hpp"

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

   // Compute statistical data
#include "PowerSum.hpp"

   // Used to delete satellites in eclipse
#include "EclipsedSatFilter.hpp"

   // Class to compute the Precise Point Positioning (PPP) solution
#include "SolverPPP.hpp"

#include "geometry.hpp"                   // DEG_TO_RAD

#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"
#include "GPSEphemerisStore.hpp"
#include "Synchronize.hpp"
#include "DeltaOp.hpp"


using namespace std;
using namespace gpstk;


int main(void)
{

      /////////////////// INITIALIZATION PART /////////////////////

   cout << fixed << setprecision(4);   // Set a proper output format


      // Create the input observation file stream
   RinexObsStream rin("ebre0300.02o");

      // Create the input observation file stream for REFERENCE STATION
   RinexObsStream rinRef("bell0300.02o");


   //// Broadcast ephemeris part

      // Create the input navigation file stream
   RinexNavStream rnavin("brdc0300.02n");

   RinexNavData rNavData;              // Object to store Rinex navigation data
   GPSEphemerisStore bceStore;         // Object to store satellites ephemeris

      // Storing the ephemeris in "bceStore"
   while (rnavin >> rNavData)
   {
      bceStore.addEphemeris(rNavData);
   }

   bceStore.SearchPast();  // This is the default

   ////


      // EBRE station nominal position
   Position nominalPos(4833520.1852, 41537.0453, 4147461.4963);


      // BELL station nominal position
   Position BnominalPos(4775849.4262, 116814.3084, 4213018.9143);



      // Declare a NeillTropModel object, setting the defaults
   NeillTropModel neillTM( nominalPos.getAltitude(),
                           nominalPos.getGeodeticLatitude(), 30);


      // Declare a NeillTropModel object, setting the defaults (Ref. station)
   NeillTropModel BneillTM( BnominalPos.getAltitude(),
                            BnominalPos.getGeodeticLatitude(), 30);


      // This is the GNSS data structure that will hold all the
      // GNSS-related information
   gnssRinex gRin;
   gnssRinex gRef;


      // Declare base-changing objects: From ECEF to North-East-Up (NEU)
   XYZ2NEU baseChange(nominalPos);
   XYZ2NEU BbaseChange(BnominalPos);


      // Declare a simple filter object to screen PC
   SimpleFilter pcFilter;
   pcFilter.setFilteredType(TypeID::PC);


      // Declare a couple of basic modelers
   BasicModel basic(nominalPos, bceStore);
   BasicModel Bbasic(BnominalPos, bceStore);


      // Objects to mark cycle slips
   LICSDetector2 markCSLI;     // Checks LI cycle slips
   MWCSDetector markCSMW;      // Checks Merbourne-Wubbena cycle slips

   LICSDetector2 BmarkCSLI;     // Checks LI cycle slips
   MWCSDetector BmarkCSMW;      // Checks Merbourne-Wubbena cycle slips


      // Object to compute tidal effects
   SolidTides  solid;

      // Ocean loading model
   OceanLoading ocean("OCEAN-GOT00.dat");

      // Numerical values are x,y pole displacements for Jan/30/2002 (arcsec).
   PoleTides   pole(-0.17153, 0.38661);


      // Vector from EBRE antenna ARP to L1 phase center [UEN]
   Triple offsetL1(0.110, 0.000, 0.000);   // Units in meters

      // Vector from EBRE antenna ARP to L2 phase center [UEN]
   Triple offsetL2(0.128, 0.0000, 0.000);    // Units in meters

      // Vector from monument to antenna ARP [UEN] for EBRE station
   Triple offsetARP(0.0, 0.0, 0.0);    // Units in meters


      // Vector from BELL antenna ARP to L1 phase center [UEN] (TRM29659.00)
   Triple BoffsetL1(0.110, 0.000, 0.000);   // Units in meters
      // Vector from BELL antenna ARP to L2 phase center [UEN] (TRM29659.00)
   Triple BoffsetL2(0.128, 0.0000, 0.000);    // Units in meters
      // Vector from monument to antenna ARP [UEN] for BELL station
   Triple BoffsetARP(0.054, 0.0, 0.0);    // Units in meters


      // Declare an object to correct observables to monument
   CorrectObservables corr(bceStore);
   ((corr.setNominalPosition(nominalPos)).setL1pc(offsetL1)).setL2pc(offsetL2);
   corr.setMonument(offsetARP);

      // Declare an object to correct observables to monument (Ref. station)
   CorrectObservables Bcorr(bceStore);
   Bcorr.setNominalPosition(BnominalPos);
   Bcorr.setL1pc(BoffsetL1);
   Bcorr.setL2pc(BoffsetL2);
   Bcorr.setMonument(BoffsetARP);

      // Objects to compute wind-up effect
   ComputeWindUp windup(bceStore, nominalPos, "PRN_GPS");
   ComputeWindUp Bwindup(bceStore, BnominalPos, "PRN_GPS");


      // Objects to compute satellite antenna phase center effect
   ComputeSatPCenter svPcenter(nominalPos);
   ComputeSatPCenter BsvPcenter(BnominalPos);


      // Objects to compute the tropospheric data
   ComputeTropModel computeTropo(neillTM);
   ComputeTropModel BcomputeTropo(BneillTM);


      // This object defines several handy linear combinations
   LinearCombinations comb;

      // Object to compute linear combinations of data
   ComputeLinear linear1(comb.pcCombWithC1);

   linear1.addLinear(comb.lcCombination);
   linear1.addLinear(comb.pdeltaCombWithC1);
   linear1.addLinear(comb.ldeltaCombination);
   linear1.addLinear(comb.mwubbenaCombWithC1);
   linear1.addLinear(comb.liCombination);

      // Let's use a different object to compute prefit residuals
   ComputeLinear linear2(comb.pcPrefit);
   linear2.addLinear(comb.lcPrefit);


      // Declare an object to process the data using PPP. It is set
      // to use a NEU system
   SolverPPP pppSolver(true);

      // The current processing strategy is "static".
      // The real test for a PPP processing program is to handle coordinates
      // as white noise. In such case, position error should be about 0.25 m or
      // better. Uncomment the following couple of lines to test this.
//   WhiteNoiseModel wnM(100.0);            // 100 m of sigma
//   pppSolver.setCoordinatesModel(&wnM);


      // Object to keep track of satellite arcs
   SatArcMarker markArc;
   markArc.setDeleteUnstableSats(true);
   markArc.setUnstablePeriod(151.0);

      // Objects to compute gravitational delay effects
   GravitationalDelay grDelay(nominalPos);
   GravitationalDelay BgrDelay(BnominalPos);

      // Object to align phase with code measurements
   PhaseCodeAlignment phaseAlign;

      // Object to compute DOP values
   ComputeDOP cDOP;

      // Object to remove eclipsed satellites
   EclipsedSatFilter eclipsedSV;

      // Statistical summary objects
   PowerSum errorVectorStats;


   TypeIDSet tset;
   tset.insert(TypeID::prefitC);
   tset.insert(TypeID::prefitL);

      // Create an object to compute the single differences of prefit residuals
   DeltaOp delta;
      // By default, it will work on code prefit residuals, so we must change
      // the default and provide a set of types to be differenced
   delta.setDiffTypeSet(tset);

      // Create an object to synchronize rover and reference station
      // data streams. This object will take data out from "rinRef" until
      // it is synchronized with data in "gOriginal". Default synchronization
      // tolerance is 1 s.
   Synchronize synchro(rinRef, gRin);

      /////////////////// PROCESING PART /////////////////////


      // Use this variable to select between position printing or model printing
   bool printPosition(true);     // By default, print position and associated
                                 // parameters



      // Loop over all data epochs
   while(rin >> gRin)
   {

      DayTime time(gRin.header.epoch);

         // Compute the effect of solid, oceanic and pole tides
      Triple tides( solid.getSolidTide(time, nominalPos) +
                    ocean.getOceanLoading("EBRE", time)  +
                    pole.getPoleTide(time, nominalPos)     );

         // Compute the effect of solid, oceanic and pole tides
      Triple Btides( solid.getSolidTide(time, BnominalPos) +
                    ocean.getOceanLoading("BELL", time)  +
                    pole.getPoleTide(time, BnominalPos)     );

         // Update observable correction object with tides information
      corr.setExtraBiases(tides);

         // Update observable correction object with tides information
      Bcorr.setExtraBiases(Btides);

      try
      {

            // First, process reference station
         gRef >> synchro         // Synchronize data streams
              >> Bbasic          // Compute the basic components of model
              >> eclipsedSV      // Remove satellites in eclipse
              >> BgrDelay         // Compute gravitational delay
              >> BsvPcenter       // Compute the effect of satellite phase
              >> Bcorr            // Correct observables from tides, etc.
              >> Bwindup          // Compute wind-up effect
              >> BcomputeTropo    // Compute tropospheric effect
              >> linear1         // Compute common linear combinations
              >> pcFilter        // Filter out spurious data
              >> BmarkCSLI        // Mark cycle slips: LI algorithm
              >> BmarkCSMW        // Mark cycle slips: Melbourne-Wubbena
              >> markArc         // Keep track of satellite arcs
              >> linear2;        // Compute prefit residuals

         delta.setRefData(gRef.body);

      }
      catch(SynchronizeException& e)   // THIS IS VERY IMPORTANT IN ORDER TO
      {                                // MANAGE A POSSIBLE DESYNCHRONIZATION!!
         continue;
      }
      catch(...)
      {
         cerr << "Exception when processing reference station data at epoch: "
              << gRef.header.epoch << endl;
      }


         // Rover data processing is done here:
      try
      {

            // The following lines are indeed just one line
         gRin >> basic           // Compute the basic components of model
              >> eclipsedSV      // Remove satellites in eclipse
              >> grDelay         // Compute gravitational delay
              >> svPcenter       // Compute the effect of satellite phase center
              >> corr            // Correct observables from tides, etc.
              >> windup          // Compute wind-up effect
              >> computeTropo    // Compute tropospheric effect
              >> linear1         // Compute common linear combinations
              >> pcFilter        // Filter out spurious data
              >> markCSLI        // Mark cycle slips: LI algorithm
              >> markCSMW        // Mark cycle slips: Melbourne-Wubbena
              >> markArc         // Keep track of satellite arcs
              >> phaseAlign      // Align phases with codes
              >> linear2         // Compute prefit residuals
              >> delta
              >> baseChange      // Prepare to use North-East-UP reference frame
              >> cDOP            // Compute DOP figures
              >> pppSolver;      // Solve equations with a Kalman filter

      }
      catch(Exception& e)
      {
         cerr << "Exception at epoch: " << time << "; " << e << endl;
         continue;
      }
      catch(...)
      {
         cerr << "Unknown exception at epoch: " << time << endl;
         continue;
      }


         // Check if we want to print model or position
      if(printPosition)
      {
            // Print here the position results
         cout << time.DOYsecond()      << "  ";     // Epoch - Output field #1

         cout << pppSolver.getSolution(TypeID::dLat) << "  ";    // dLat  - #2
         cout << pppSolver.getSolution(TypeID::dLon) << "  ";    // dLon  - #3
         cout << pppSolver.getSolution(TypeID::dH) << "  ";      // dH    - #4
         cout << pppSolver.getSolution(TypeID::wetMap) << "  ";  // Tropo - #5

         cout << pppSolver.getVariance(TypeID::dLat) << "  "; // Cov dLat - #6
         cout << pppSolver.getVariance(TypeID::dLon) << "  "; // Cov dLon - #7
         cout << pppSolver.getVariance(TypeID::dH) << "  ";   // Cov dH   - #8
         cout << pppSolver.getVariance(TypeID::wetMap) << "  ";//Cov Tropo- #9

         cout << gRin.numSats()        << "  ";       // Satellite number - #10

         cout << cDOP.getGDOP()        << "  ";                   // GDOP - #11
         cout << cDOP.getPDOP()        << "  ";                   // PDOP - #12
         cout << cDOP.getTDOP()        << "  ";                   // TDOP - #13
         cout << cDOP.getHDOP()        << "  ";                   // HDOP - #14
         cout << cDOP.getVDOP()        << "  ";                   // VDOP - #15

         cout << endl;

            // For statistical purposes we discard the first two hours of data
         if (time.DOYsecond() > 7200.0)
         {
               // Statistical summary
            double errorV( pppSolver.solution[1]*pppSolver.solution[1] +
                           pppSolver.solution[2]*pppSolver.solution[2] +
                           pppSolver.solution[3]*pppSolver.solution[3] );

               // Get module of position error vector
            errorV = std::sqrt(errorV);

               // Add to statistical summary object
            errorVectorStats.add(errorV);
         }

      }  // End of position printing
      else
      {
            // Print here the model results
            // First, define types we want to keep
         TypeIDSet types;
         types.insert(TypeID::L1);
         types.insert(TypeID::L2);
         types.insert(TypeID::P1);
         types.insert(TypeID::P2);
         types.insert(TypeID::PC);
         types.insert(TypeID::LC);
         types.insert(TypeID::rho);
         types.insert(TypeID::dtSat);
         types.insert(TypeID::rel);
         types.insert(TypeID::gravDelay);
         types.insert(TypeID::tropo);
         types.insert(TypeID::dryTropo);
         types.insert(TypeID::dryMap);
         types.insert(TypeID::wetTropo);
         types.insert(TypeID::wetMap);
         types.insert(TypeID::tropoSlant);
         types.insert(TypeID::windUp);
         types.insert(TypeID::satPCenter);
         types.insert(TypeID::satX);
         types.insert(TypeID::satY);
         types.insert(TypeID::satZ);
         types.insert(TypeID::elevation);
         types.insert(TypeID::azimuth);
         types.insert(TypeID::satArc);
         types.insert(TypeID::prefitC);
         types.insert(TypeID::prefitL);
         types.insert(TypeID::dx);
         types.insert(TypeID::dy);
         types.insert(TypeID::dz);
         types.insert(TypeID::dLat);
         types.insert(TypeID::dLon);
         types.insert(TypeID::dH);
         types.insert(TypeID::cdt);

         gRin.keepOnlyTypeID(types);   // Delete the types not in 'types'

            // Iterate through the GNSS Data Structure
         satTypeValueMap::const_iterator it;
         for (it = gRin.body.begin(); it!= gRin.body.end(); it++) 
         {

               // Print epoch
            cout << time.year()        << " ";
            cout << time.DOY()         << " ";
            cout << time.DOYsecond()   << " ";

            cout << cDOP.getGDOP()        << "  ";  // GDOP #4
            cout << cDOP.getPDOP()        << "  ";  // PDOP #5
            cout << cDOP.getTDOP()        << "  ";  // TDOP #6
            cout << cDOP.getHDOP()        << "  ";  // HDOP #7
            cout << cDOP.getVDOP()        << "  ";  // VDOP #8

               // Print satellite information (system and PRN)
            cout << (*it).first << " ";

               // Print model values
            typeValueMap::const_iterator itObs;
            for( itObs  = (*it).second.begin(); 
                 itObs != (*it).second.end();
                 itObs++ )
            {
               bool printNames(true);  // Whether print types' names or not
               if (printNames)
               {
                  cout << (*itObs).first << " ";
               }

               cout << (*itObs).second << " ";

            }  // End for( itObs = ... )

            cout << endl;

         }  // End for (it = gRin.body.begin(); ... )

      }  // End of model printing

   }  // End of 'while(rin >> gRin)...'



      // Print statistical summary in cerr
   if(printPosition)
   {
      cerr << "Module of error vector: Average = "
           << errorVectorStats.average() << " m    Std. dev. = "
           << std::sqrt(errorVectorStats.variance()) << " m" << endl;
   }



   exit(0);       // End of program

}
