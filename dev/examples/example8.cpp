// Example program Nro 8 for GPSTk
// This program shows how to use GNSS Data Structures (GDS) to obtain
// "Precise Point Positioning" (PPP).
// Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008

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
#include "LICSDetector.hpp"

   // Class to detect cycle slips using the Melbourne-Wubbena combination
#include "MWCSDetector.hpp"

   // Class to compute weights according to Appendix J of MOPS C (RTCA/DO-229C)
#include "ComputeMOPSWeights.hpp"

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

   // Class to compute the tropospheric data
#include "ComputeTropModel.hpp"

   // Class to compute linear combinations
#include "ComputeLinear.hpp"

   // This class pre-defines several handy linear combinations
#include "LinearCombinations.hpp"

   // Class to compute the Precise Point Positioning (PPP) solution
#include "SolverPPP.hpp"

#include "geometry.hpp"                   // DEG_TO_RAD


using namespace std;
using namespace gpstk;


int main(void)
{

      /////////////////// INITIALIZATION PART /////////////////////

   cout << fixed << setprecision(3);   // Set a proper output format


      // Create the input observation file stream
   RinexObsStream rin("onsa2240.05o");


      // load all the SP3 ephemerides files
   SP3EphemerisStore SP3EphList;
   SP3EphList.loadFile("igs13354.sp3");
   SP3EphList.loadFile("igs13355.sp3");
   SP3EphList.loadFile("igs13356.sp3");


      // ONSA station nominal position
   Position nominalPos(3370658.5419, 711877.1496, 5349786.9542);


      // Declare a NeillTropModel object, setting the defaults
   NeillTropModel neillTM( nominalPos.getAltitude(),
                           nominalPos.getGeodeticLatitude(), 224);


      // This is the GNSS data structure that will hold all the 
      // GNSS-related information
   gnssRinex gRin;


      // Declare a base-changing object: From ECEF to North-East-Up (NEU)
   XYZ2NEU baseChange(nominalPos);


      // This object will compute the appropriate MOPS weights
   ComputeMOPSWeights mopsW(nominalPos, SP3EphList);


      // Declare a simple filter object to screen PC
   SimpleFilter pcFilter;
   pcFilter.setFilteredType(TypeID::PC);


      // Declare a basic modeler
   BasicModel basic(nominalPos, SP3EphList);


      // Objects to mark cycle slips
   LICSDetector markCSLI;      // Checks LI cycle slips
   MWCSDetector markCSMW;      // Checks Merbourne-Wubbena cycle slips


      // Object to compute tidal effects
   SolidTides  solid;

      // Ocean loading model
   OceanLoading ocean("ONSA-GOT00.dat");

      // Numerical values are x,y pole displacements for Aug/12/2005 (arcsec).
   PoleTides   pole(0.020840, 0.427601);


      // Vector from ONSA antenna ARP to L1 phase center [UEN] (AOAD/M_B)
   Triple offsetL1(0.0780, 0.000, 0.000);   // Units in meters

      // Vector from ONSA antenna ARP to L2 phase center [UEN] (AOAD/M_B)
   Triple offsetL2(0.096, 0.0000, 0.000);    // Units in meters

      // Vector from monument to antenna ARP [UEN] for ONSA station
   Triple offsetARP(0.9950, 0.0, 0.0);    // Units in meters


      // Declare an object to correct observables to monument
   CorrectObservables corr(SP3EphList);
   ((corr.setNominalPosition(nominalPos)).setL1pc(offsetL1)).setL2pc(offsetL2);
   corr.setMonument(offsetARP);


      // Object to compute wind-up effect
   ComputeWindUp windup(SP3EphList, nominalPos,"PRN_GPS");


      // Object to compute the tropospheric data
   ComputeTropModel computeTropo(neillTM);


      // This object defines several handy linear combinations
   LinearCombinations comb;

      // Object to compute linear combinations of data
   ComputeLinear linear(comb.pcCombination);

   linear.addLinear(comb.lcCombination);
   linear.addLinear(comb.pcPrefit);
   linear.addLinear(comb.lcPrefit);
   linear.addLinear(comb.pdeltaCombWithC1);
   linear.addLinear(comb.ldeltaCombination);
   linear.addLinear(comb.mwubbenaCombWithC1);
   linear.addLinear(comb.liCombination);


      // Declare an object to process the data using PPP. It is set
      // to use a NEU system
   SolverPPP pppSolver(true);


      /////////////////// PROCESING PART /////////////////////


      // Loop over all data epochs
   while(rin >> gRin)
   {

      DayTime time(gRin.header.epoch);

         // Compute the effect of solid, oceanic and pole tides
      Triple tides( solid.getSolidTide(time, nominalPos) +
                    ocean.getOceanLoading("ONSA", time)  +
                    pole.getPoleTide(time, nominalPos)     );

         // Update observable correction object with tides information
      corr.setExtraBiases(tides);

      try
      {

         gRin >> basic
              >> corr
              >> windup
              >> computeTropo
              >> linear
              >> markCSLI
              >> markCSMW
              >> pcFilter
              >> mopsW
              >> baseChange
              >> pppSolver;

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


         // Print here the results
      cout << time.DOYsecond()      << "  ";  // epoch - Output field #1
      cout << pppSolver.solution[1] << "  ";  // dLat  - Output field #2
      cout << pppSolver.solution[2] << "  ";  // dLon  - Output field #3
      cout << pppSolver.solution[3] << "  ";  // dH    - Output field #4
      cout << pppSolver.solution[0] << "  ";  // Tropo - Output field #5

      cout << endl;

   }

   exit(0);
}
