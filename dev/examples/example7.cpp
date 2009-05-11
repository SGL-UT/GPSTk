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

// Example program Nro 7 for GPSTk
// This program shows several different ways to process GPS data
// using GNSS Data Structures (DataStructures.hpp).
// Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008

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

   // Classes to model and store ionospheric delays
#include "IonoModel.hpp"
#include "IonoModelStore.hpp"

   // Class to solve the equation system using Least Mean Squares
#include "SolverLMS.hpp"

   // Class to solve the equation system using Weighted-Least Mean Squares
#include "SolverWMS.hpp"

   // Class to solve equations systems using a simple code-based Kalman filter
#include "CodeKalmanSolver.hpp"

   // Class defining the GNSS data structures
#include "DataStructures.hpp"

   // Class to filter out observables grossly out of limits
#include "SimpleFilter.hpp"

   // Class for easily changing reference base from ECEF to NEU
#include "XYZ2NEU.hpp"

   // Class to detect cycle slips using just one frequency
#include "OneFreqCSDetector.hpp"

   // Class to detect cycle slips using LI combination
#include "LICSDetector.hpp"

   // Class to detect cycle slips using the Melbourne-Wubbena combination
#include "MWCSDetector.hpp"

   // Class to compute weights according to Appendix J of MOPS C (RTCA/DO-229C)
#include "ComputeMOPSWeights.hpp"

   // Class to smooth code observables (by default, C1)
#include "CodeSmoother.hpp"

   // Class to smooth the PC combination
#include "PCSmoother.hpp"

   // Classes to compute several combinations
#include "ComputePC.hpp"
#include "ComputeLC.hpp"
#include "ComputeLI.hpp"
#include "ComputeMelbourneWubbena.hpp"

   // Class to compute single differences between receiver stations
#include "DeltaOp.hpp"

   // Class to synchronize two GNSS Data Structures data streams.
#include "Synchronize.hpp"

#include "geometry.hpp"                   // DEG_TO_RAD


using namespace std;
using namespace gpstk;

int main(void)
{

      ////////// Initialization phase //////////

      //////////// COMMON OBJECTS //////////////

   cout << fixed << setprecision(3);   // Set a proper output format

   RinexNavData rNavData;              // Object to store Rinex navigation data
   GPSEphemerisStore bceStore;         // Object to store satellites ephemeris
   RinexNavHeader rNavHeader;          // Object to read the header of Rinex
                                       // navigation data files
   IonoModelStore ionoStore;           // Object to store ionospheric models
   IonoModel ioModel;                  // Declare a Ionospheric Model object

      // Create the input observation file stream
      // This is a fixed station, but here it will play as "rover"
   RinexObsStream rin("ebre030a.02o");
      // Please note that data was collected in year 2002, when the Sun
      // was very active

      // Create the input navigation file stream
   RinexNavStream rnavin("brdc0300.02n");

      // We need to read ionospheric parameters (Klobuchar model) from header
   rnavin >> rNavHeader;

      // Let's feed the ionospheric model (Klobuchar type) from data in the
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

      // EBRE station nominal position
   Position nominalPos(4833520.3800, 41536.8300, 4147461.2800);

      // Declare a MOPSTropModel object, setting the defaults
   MOPSTropModel mopsTM( nominalPos.getAltitude(),
                         nominalPos.getGeodeticLatitude(),
                         30 );

      // Declare the modeler object, setting all the parameters in one pass
      // Given that in this example we are using a fixed GPS station with known
      // coordinates, you could have used the "ModeledReferencePR" class, which
      // is a little bit simpler.
      // However, for a rover is more appropriate to use a "ModelObs" object
      // because it allows to update the apriori position more easily (and it
      // may automatically compute one, if needed, using Bancroft's method)
   ModelObs model(nominalPos, ionoStore, mopsTM, bceStore, TypeID::C1);

      // On the other hand, the usual way to use "ModelObs" is setting just the
      // models in the constructor, and calling method "Prepare()" later, like
      // in the following lines:
      // ModelObs model(ionoStore, mopsTM, bceStore, TypeID::C1);
      // model.Prepare(nominalPos);       // Set the reference position

      // Declare a simple filter object. By default, it filters C1 with
      // default limits
   SimpleFilter myFilter;

      // This is the GNSS data structure that will hold all the
      // GNSS-related information
   gnssRinex gOriginal;

      ////////////////////////////////////////



      //////////// CASE #1 OBJECTS ////////////

      // Declare a SolverLMS object
   SolverLMS solver;

      ////////////////////////////////////////



      //////////// CASE #2 OBJECTS ////////////

      // Declare a base-changing object: From ECEF to North-East-Up (NEU)
   XYZ2NEU baseChange(nominalPos);

      // For some examples we need to reconfigure the solver in order
      // to use a NEU system
   TypeIDSet typeSet;
   typeSet.insert(TypeID::dLat);
   typeSet.insert(TypeID::dLon);
   typeSet.insert(TypeID::dH);
   typeSet.insert(TypeID::cdt);

      // This is the proper equation structure to use with a NEU system
   gnssEquationDefinition newEq(TypeID::prefitC, typeSet);

      // Declare another SolverLMS object, but configure it to use a
      // topocentric reference system (North-East-Up: NEU)
   SolverLMS solverNEU;
   solverNEU.setDefaultEqDefinition(newEq);    // NEU reconfiguration

      ////////////////////////////////////////



      //////////// CASE #3 OBJECTS ////////////

      // This object will compute the appropriate MOPS weights
   ComputeMOPSWeights mopsW(nominalPos, bceStore);

      // Declare a solver object using Weighted-Least-Mean-Squares and
      // a topocentric reference system (NEU)
   SolverWMS solverWMS;
   solverWMS.setDefaultEqDefinition(newEq);    // NEU reconfiguration

      ////////////////////////////////////////



      //////////// CASE #4 OBJECTS ////////////

      // Let's declare a cycle slip detector using just one frequency
   OneFreqCSDetector markCSC1;

      // Declare an object to smooth code (C1 by default)
   CodeSmoother smoothC1;
   smoothC1.setMaxWindowSize(8);   // Configure smoother for 30 s sampling data

      ////////////////////////////////////////



      //////////// CASE #5 OBJECTS ////////////

      // Let's declare another cycle slip detector using just one frequency
   OneFreqCSDetector markCSC1case5;

      // Declare another object to smooth code (C1 by default)
   CodeSmoother smoothC1case5;
   smoothC1case5.setMaxWindowSize(8);   // Configure for 30 s sampling data

      // The core of this case is to add a new equation to the equation system
      // Such equation states that there are NO changes in height for the
      // rover.
      //                          dH = 0
      //
      // Add a "fake" satellite to identify the new equation: Sat #1 of
      // system "UserDefined"
   SatID satEq(1,SatID::systemUserDefined);

      // Declare and fill a "typeValueMap" object that will hold
      // the equation data
   typeValueMap equTVMap;
   equTVMap[TypeID::prefitC] = 0.0;    // Code prefit residual is zero
   equTVMap[TypeID::dLat]    = 0.0;    // Geometry matrix dLat coefficient is
                                       // zero
   equTVMap[TypeID::dLon]    = 0.0;    // Geometry matrix dLon coefficient is
                                       // zero
   equTVMap[TypeID::dH]      = 1.0;    // Geometry matrix dH coefficient is
                                       // 1.0!!!
   equTVMap[TypeID::cdt]     = 0.0;    // Geometry matrix cdt coefficient is
                                       // zero

      // Assign a relatively high weight to this information (typical
      // MOPS weights range from 0.01 to 0.04)
      // This means that this equation is very important for us, but it is
      // NOT ABSOLUTELY TRUE. Some variation is allowed
      // Given that weights are indeed (1/variances), if we assign to our new
      // equation a confidence of 0.5 m of sigma, it means that we should use
      // a weight of (1/(0.5^2)) = 4 m^(-2)
   equTVMap[TypeID::weight]  = 4.0;

      ////////////////////////////////////////



      //////////// CASE #6 OBJECTS ////////////

      // Object to compute the PC (ionosphere-free) combination.
   ComputePC getPC;
      // Use C1 instead of P1. P1 observables are declared in available RINEX
      // files, but often they are indeed missing (like in this case). When
      // that happens, this step is mandatory
   getPC.useC1();

      // Declare a simple filter object to screen PC
   SimpleFilter pcFilter;
   pcFilter.setFilteredType(TypeID::PC);

      // Declare the modeler object for PC, setting all the parameters
      // in one pass
   ModelObs modelPC(nominalPos, mopsTM, bceStore, TypeID::PC, false);
      // Take notice that PC combination doesn't use ionosphere modelling, nor
      // TGD computation.
      // WARNING: When using C1 instead of P1 to compute PC combination, be
      // aware that instrumental errors will NOT cancel, introducing a bias
      // that must be taken into account by other means. This will not work out
      // in this example.

      ////////////////////////////////////////



      //////////// CASE #7 OBJECTS ////////////

      // Objects to compute several common combinations.
   ComputeLC getLC;
   ComputeLI getLI;
   ComputeMelbourneWubbena getMW;
   getMW.useC1();      // Use C1 instead of P1

      // Objects to mark cycle slips
   LICSDetector markCSLI;      // Checks LI cycle slips
   MWCSDetector markCSMW;      // Checks Merbourne-Wubbena cycle slips

      // Object to smooth the PC combination. Defaults are usually fine
   PCSmoother smoothPC;

      ////////////////////////////////////////



      //////////// CASE #8 OBJECTS ////////////

      // Objects to mark cycle slips
   LICSDetector markCSLIcase8;      // Checks LI cycle slips
   MWCSDetector markCSMWcase8;      // Checks Merbourne-Wubbena cycle slips

      // Object to smooth the PC combination. Defaults are usually fine
   PCSmoother smoothPCcase8;

      ////////////////////////////////////////



      //////////// CASE #9 OBJECTS ////////////

      // Objects to mark cycle slips
   LICSDetector markCSLIcase9;      // Checks LI cycle slips
   MWCSDetector markCSMWcase9;      // Checks Merbourne-Wubbena cycle slips

      // Object to smooth the PC combination. Defaults are usually fine
   PCSmoother smoothPCcase9;

      // Declare a new Kalman solver, already reconfigured for NEU system
   CodeKalmanSolver solverK9(newEq);

      ////////////////////////////////////////



      //////////// CASE #10 OBJECTS ////////////

      // This is the GNSS data structure that will hold the
      // reference station data
   gnssRinex gRef;

      // Create the input observation file stream for REFERENCE STATION
   RinexObsStream rinRef("bell030a.02o");

      // BELL reference station nominal position
   Position nominalPosRef(4775849.6200, 116814.1000, 4213018.7100);

      // Declare a MOPSTropModel object for the reference station, setting
      // the defaults
   MOPSTropModel mopsTMRef( nominalPosRef.getAltitude(),
                            nominalPosRef.getGeodeticLatitude(),
                            30 );

      // Declare the appropriate modeler object for a reference station
   ModelObsFixedStation modelRef( nominalPosRef,
                                  ionoStore,
                                  mopsTMRef,
                                  bceStore,
                                  TypeID::C1 );

      // Create an object to compute the single differences of prefit residuals
   DeltaOp delta;      // By default, it will work on code prefit residuals

      // Create an object to synchronize rover and reference station
      // data streams. This object will take data out from "rinRef" until
      // it is synchronized with data in "gOriginal". Default synchronization
      // tolerance is 1 s.
   Synchronize synchro(rinRef, gOriginal);

      //////////////////////////////////////////////



      //////////// CASE #12 OBJECTS ////////////

      // Declare a new Kalman solver, already reconfigured for NEU system
   CodeKalmanSolver solverK12(newEq);

      ////////////////////////////////////////

      //////// End of initialization phase  ////////


      //////// Processing phase ////////

      // Loop over all data epochs
   while(rin >> gOriginal)
   {


         // Let's output the time stamp (in seconds of day)
      cout << gOriginal.header.epoch.DOYsecond() << "  ";   // Output field #1


   //////////////////////////// CASE #1  ////////////////////////////

         // This case is a common C1 + Least Mean Squares solver
         // (LMS) processing

         // Let's make a working copy
      gnssRinex gRin1(gOriginal);

      try
      {

            // This is the line that will process all the GPS data
         gRin1 >> myFilter >> model >> solver;

            // - First, a basic filter to screen out very bad observables
            // - Second, apply a model to the observables (ionosphere,
            //   troposphere, relativity, etc.)
            // - Third, solve the equations using a simple Least-Mean-Squares
            //   solver
      }
      catch(...)
      {
         cerr << "Case 1. Exception at epoch: " << gRin1.header.epoch << endl;
      }


         // Get your results out of the solver object. In ECEF system
         // by default
      Position solPos( (model.rxPos.X() + solver.solution[0]),
                       (model.rxPos.Y() + solver.solution[1]),
                       (model.rxPos.Z() + solver.solution[2]) );

         // Let's change results to a North-East-Up (NEU) reference frame
         // Compute the difference regarding the nominal position
      Position diffPos;
      diffPos = solPos - nominalPos;
      double azimuth = nominalPos.azimuthGeodetic(solPos);
      double elev = nominalPos.elevationGeodetic(solPos);
      double magnitude = RSS(diffPos.X(), diffPos.Y(), diffPos.Z());

         // Print results of case #1 in a topocentrical, North-East-Up
         // reference frame
         // Latitude change, output field #2
      cout << magnitude*cos(azimuth*DEG_TO_RAD)*cos(elev*DEG_TO_RAD) << "  ";
        // Longitude change, output field #3
      cout << magnitude*sin(azimuth*DEG_TO_RAD)*cos(elev*DEG_TO_RAD) << "  ";
         // Altitude change, output field #4
      cout << magnitude*sin(elev*DEG_TO_RAD) << "  ";

   ////////////////////////// END OF CASE #1  //////////////////////////



   //////////////////////////// CASE #2  ////////////////////////////

         // This is exactly the same as CASE #1, but using a nice class
         // to change the reference frame: ECEF -> NEU

         // Let's make a working copy
      gnssRinex gRin2(gOriginal);

      try
      {

         gRin2 >> myFilter >> model >> baseChange >> solverNEU;

      }
      catch(...)
      {
         cerr << "Case 2. Exception at epoch: " << gRin2.header.epoch << endl;
      }

      cout << solverNEU.solution(0) << "  ";  // dLat - Output field #5
      cout << solverNEU.solution(1) << "  ";  // dLon - Output field #6
      cout << solverNEU.solution(2) << "  ";  // dH   - Output field #7


         // Quite easier with respect to CASE #1, isn't it?  ;-)

         // - "baseChange" object changes reference frame from ECEF to NEU
         // - "solverNEU" is a simple Least-Mean-Squares solver, but
         //    reconfigured to solve the dLat, dLon, dH, cdt (NEU) system
         //    instead of the dx, dy, dz, cdt (ECEF) system
         // - The other steps are exactly the same as case #1, and results
         //   MUST match
         // - If you want to see an even easier method to report the solution,
         //   please see Case #3.

         // By the way, if you want to inspect what is inside the body of a
         // given GNSS data structure, you may write something like:
         //
         //      gRin2.body.dump(cout, 1);

   ////////////////////////// END OF CASE #2  //////////////////////////



   //////////////////////////// CASE #3  ////////////////////////////

         // In this case we process data using C1 + Weighted Least Mean Squares
         // solver (WMS)

         // Let's make a working copy
      gnssRinex gRin3(gOriginal);

      try
      {

         gRin3 >> myFilter >> model >> mopsW >> baseChange >> solverWMS;
            // The "mopsW" object computes weights based on MOPS algorithm
            // The "solverWMS" object solves the system using Weighted Least
            // Mean Squares. It is already configured to work with NEU system.
      }
      catch(...)
      {
         cerr << "Case 3. Exception at epoch: " << gRin3.header.epoch << endl;
      }

         // An alternative way to report the solution is to access it
         // using the TypeID's defined in the "gnssEquationDefinition" object
         // assigned to the solver.
         // With this method we avoid the possibility of getting the wrong
         // type of solution from the "solution" vector.
      cout << solverWMS.getSolution(TypeID::dLat) << "  ";  // dLat - Field #8
      cout << solverWMS.getSolution(TypeID::dLon) << "  ";  // dLon - Field #9
      cout << solverWMS.getSolution(TypeID::dH) << "  ";    // dH   - Field #10

   ////////////////////////// END OF CASE #3  //////////////////////////



   //////////////////////////// CASE #4  ////////////////////////////

         // This case does about the same as a modern GPS aircraft receiver,
         // except for SBAS corrections and RAIM: C1smoothed + MOPS weights
         // + WMS

         // Let's make a working copy
      gnssRinex gRin4(gOriginal);

      try
      {

         gRin4 >> myFilter >> markCSC1 >> smoothC1 >> model >> mopsW
               >> baseChange >> solverWMS;
            // The "markCSC1" object will try to detect cycle slips using just
            // one frequency data (C1 and L1 observables), marking the CS flags
            // Then, "smoothC1" will use the former information to smooth C1
            // observations using phase data (L1)

            // BEWARE: Both cycle slip detectors and "smoothers" are objects
            // that store their internal state, so you MUST NOT use the SAME
            // object to process DIFFERENT data streams
      }
      catch(...)
      {
         cerr << "Case 4. Exception at epoch: " << gRin4.header.epoch << endl;
      }

      cout << solverWMS.getSolution(TypeID::dLat) << "  ";  // dLat - Field #11
      cout << solverWMS.getSolution(TypeID::dLon) << "  ";  // dLon - Field #12
      cout << solverWMS.getSolution(TypeID::dH) << "  ";    // dH   - Field #13

   ////////////////////////// END OF CASE #4  //////////////////////////



   //////////////////////////// CASE #5  ////////////////////////////

         // This case is like the former, but now let's suppose that one of
         // the unknowns is indeed known: In this case dH is constant an equal
         // to zero (i.e.: the "rover" doesn't change altitude), and we assign
         //  a high "weight" to this information.

         // Let's make a working copy
      gnssRinex gRin5(gOriginal);

      try
      {
            // First, the typical processing up to the change of reference
            // frame. Please note that all changes are stored in gRin5 GNSS
            // data structure
         gRin5 >> myFilter >> markCSC1case5 >> smoothC1case5 >> model
               >> mopsW >> baseChange;
            // Remember that both cycle slip detectors and "smoothers" are
            // objects that store their internal state, so you MUST NOT use
            // the SAME object to process DIFFERENT data streams (please
            // compare with case #4)

            // Now, let's insert the new equation data, including its
            // corresponding weight
         gRin5.body[satEq] = equTVMap;

            // Let's continue processing data as usual
         gRin5 >> solverWMS;

      }
      catch(...)
      {
         cerr << "Case 5. Exception at epoch: " << gRin5.header.epoch << endl;
      }

      cout << solverWMS.getSolution(TypeID::dLat) << "  ";  // dLat - Field #14
      cout << solverWMS.getSolution(TypeID::dLon) << "  ";  // dLon - Field #15
      cout << solverWMS.getSolution(TypeID::dH) << "  ";    // dH   - Field #16

   ////////////////////////// END OF CASE #5  //////////////////////////



   //////////////////////////// CASE #6  ////////////////////////////

         // This case uses de PC combination plus a WMS solver

         // Let's make a working copy
      gnssRinex gRin6(gOriginal);

      try
      {

         gRin6 >> getPC >> pcFilter >> modelPC >> mopsW
               >> baseChange >> solverWMS;
            // First, we need to compute the PC combination with "getPC" and
            // insert it into the "gRin6" data structure.
            // Then, use "pcFilter" to take out grossly out of range
            // results in PC.
            // After that, use an specific model ("modelPC") for this
            // combination. It doesn't use ionospheric model nor TGD.
            // The remaining steps are similar to the other cases.
      }
      catch(...)
      {
         cerr << "Case 6. Exception at epoch: " << gRin6.header.epoch << endl;
      }

      cout << solverWMS.getSolution(TypeID::dLat) << "  ";  // dLat - Field #17
      cout << solverWMS.getSolution(TypeID::dLon) << "  ";  // dLon - Field #18
      cout << solverWMS.getSolution(TypeID::dH) << "  ";    // dH   - Field #19

   ////////////////////////// END OF CASE #6  //////////////////////////



   //////////////////////////// CASE #7  ////////////////////////////

         // This case uses the smoothed-PC combination plus WMS

         // Let's make a working copy
      gnssRinex gRin7(gOriginal);

      try
      {

         gRin7 >> getPC >> getLC >> getLI >> getMW >> markCSLI >> markCSMW
               >> smoothPC >> pcFilter >> modelPC >> mopsW >> baseChange
               >> solverWMS;
            // In addition to PC, we will also neet LC ("getLC"), LI ("getLI")
            // and MW ("getMW") combinations:
            //
            // - LC (as well as PC) is needed by "smoothPC" in order to smooth
            //   PC data. Also, the smoother works better with cycle slip
            //   information, and therefore:
            //
            //   - LI feeds "markCSLI": The LI-based cycle slip detector
            //   - MW feeds "markCSMW": The MW-based cycle slip detector
            //
            // - The remaining steps are essentially the same
      }
      catch(...)
      {
         cerr << "Case 7. Exception at epoch: " << gRin7.header.epoch << endl;
      }

      cout << solverWMS.getSolution(TypeID::dLat) << "  ";  // dLat - Field #20
      cout << solverWMS.getSolution(TypeID::dLon) << "  ";  // dLon - Field #21
      cout << solverWMS.getSolution(TypeID::dH) << "  ";    // dH   - Field #22

   ////////////////////////// END OF CASE #7  //////////////////////////



   //////////////////////////// CASE #8  ////////////////////////////

         // This case uses the smoothed-PC combination + WMS + information
         // about dH (constant and equal to zero with a high confidence).
         // It is a mix of the former case (#7) and case #5.

         // Let's make a working copy
      gnssRinex gRin8(gOriginal);

      try
      {
            // First, let's process data up to the change of reference frame
         gRin8 >> getPC >> getLC >> getLI >> getMW >> markCSLIcase8
               >> markCSMWcase8 >> smoothPCcase8 >> pcFilter >> modelPC
               >> mopsW >> baseChange;
            // Remember that both cycle slip detectors and "smoothers" are
            // objects that store their internal state, so you MUST NOT use
            // the SAME object to process DIFFERENT data streams (compare with
            // case #7).

            // Now, let's insert the new equation data, including its weight.
            // It is the same equation as case #5.
         gRin8.body[satEq] = equTVMap;

            // Let's continue processing data as usual
         gRin8 >> solverWMS;

      }
      catch(...)
      {
         cerr << "Case 8. Exception at epoch: " << gRin8.header.epoch << endl;
      }

      cout << solverWMS.getSolution(TypeID::dLat) << "  ";  // dLat - Field #23
      cout << solverWMS.getSolution(TypeID::dLon) << "  ";  // dLon - Field #24
      cout << solverWMS.getSolution(TypeID::dH) << "  ";    // dH   - Field #25

   ////////////////////////// END OF CASE #8  //////////////////////////



   //////////////////////////// CASE #9  ////////////////////////////

         // This case uses the smoothed-PC combination, exactly like case #7,
         // but solves the equation system using a simple Kalman filter.

         // Let's make a working copy
      gnssRinex gRin9(gOriginal);

      try
      {

         gRin9 >> getPC >> getLC >> getLI >> getMW >> markCSLIcase9
               >> markCSMWcase9 >> smoothPCcase9 >> pcFilter >> modelPC
               >> mopsW >> baseChange >> solverK9;
            // VERY IMPORTANT: Note that in this case the coordinates are
            // handled as constants, whereas the receiver clock is modeled as
            // white noise.
      }
      catch(...)
      {
         cerr << "Case 9. Exception at epoch: " << gRin9.header.epoch << endl;
      }

      cout << solverK9.getSolution(TypeID::dLat) << "  ";  // dLat - Field #26
      cout << solverK9.getSolution(TypeID::dLon) << "  ";  // dLon - Field #27
      cout << solverK9.getSolution(TypeID::dH) << "  ";    // dH   - Field #28

   ////////////////////////// END OF CASE #9  //////////////////////////



   //////////////////////////// CASE #10  ////////////////////////////


         // This is like cases #1 and #2, but using DGPS techniques instead.

         // Let's make a working copy of rover data
      gnssRinex gRin10(gOriginal);


         // First, let's synchronize and process reference station data
      try
      {
         gRef >> synchro >> myFilter >> modelRef;
            // Please note that the FIRST STEP is to synchronize "gRef", the
            // reference station data stream, with "gOriginal" (or with gRin10,
            // which is the same), the rover receiver data stream.
            //
            // Also, remember that in simple DGPS the differences are computed
            // on code prefit residuals, so "modelRef" object is mandatory.

            // The "delta" object will take care of proper differencing.
            // We must tell it which GNSS data structure will be used
            // as reference
         delta.setRefData(gRef.body);

      }
      catch(SynchronizeException& e)   // THIS IS VERY IMPORTANT IN ORDER TO
      {                                // MANAGE A POSSIBLE DESYNCHRONIZATION!!
         cout << endl;
         continue;
      }
      catch(...)
      {
         cerr << "Case 10. Exception when processing reference station data \
at epoch: " << gRef.header.epoch << endl;
      }


         // Rover data processing is done here:
      try
      {

         gRin10 >> myFilter >> model >> delta >> baseChange >> solverNEU;
            // This is very similar to cases #1 and #2, but we insert a "delta"
            // object that will adjust code prefit residuals BEFORE solving the
            // system of equations.
      }
      catch(...)
      {
         cerr << "Case 10. Exception at epoch: " << gRin10.header.epoch
              << endl;
      }

      cout << solverNEU.getSolution(TypeID::dLat) << "  ";  // dLat - Field #29
      cout << solverNEU.getSolution(TypeID::dLon) << "  ";  // dLon - Field #30
      cout << solverNEU.getSolution(TypeID::dH) << "  ";    // dH   - Field #31

   ////////////////////////// END OF CASE #10  //////////////////////////



   //////////////////////////// CASE #11  ////////////////////////////


         // This is like case #10 (DGPS), but now let's apply a WMS solver
         // on data

         // Let's make a working copy
         gnssRinex gRin11(gOriginal);


         // Please note that data streams are already synchronized, and
         // "delta" object may be reused with the same reference data
         // obtained from Case #10.

      try
      {

         gRin11 >> myFilter >> model >> delta >> mopsW >> baseChange
                >> solverWMS;
            // Like case #10, but now with "mopsW" and "solverWMS"
      }
      catch(...)
      {
         cerr << "Case 11. Exception at epoch: " << gRin11.header.epoch
              << endl;
      }

      cout << solverWMS.getSolution(TypeID::dLat) << "  ";  // dLat - Field #32
      cout << solverWMS.getSolution(TypeID::dLon) << "  ";  // dLon - Field #33
      cout << solverWMS.getSolution(TypeID::dH) << "  ";    // dH   - Field #34

   ////////////////////////// END OF CASE #11  //////////////////////////



   //////////////////////////// CASE #12  ////////////////////////////


         // This is like case #11 (DGPS), but now let's apply a simple
         // Kalman filter on data

         // Let's make a working copy
         gnssRinex gRin12(gOriginal);


         // Please note that data streams are already synchronized, and
         // "delta" object may be reused with the same reference data
         // obtained from Case #10.

      try
      {

         gRin12 >> myFilter >> model >> delta >> mopsW >> baseChange
                >> solverK12;
            // Like case #11, but now with "solverK12"
            // VERY IMPORTANT: Note that in this case the coordinates are
            // handled as constants, whereas the receiver clock is modeled as
            // white noise.
      }
      catch(...)
      {
         cerr << "Case 12. Exception at epoch: " << gRin12.header.epoch
              << endl;
      }

      cout << solverK12.getSolution(TypeID::dLat) << "  ";  // dLat - Field #35
      cout << solverK12.getSolution(TypeID::dLon) << "  ";  // dLon - Field #36
      cout << solverK12.getSolution(TypeID::dH) << "  ";    // dH   - Field #37

   ////////////////////////// END OF CASE #12  //////////////////////////


         // End of data processing for this epoch
      cout << endl;

   }

   exit(0);

}
