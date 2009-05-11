#pragma ident "$Id$"
// Example program Nro 5 for GPSTk
// This program shows how to use some high-level GPSTk classes
// Dagoberto Salazar - gAGE. 2006

// Basic input/output C++ class
#include <iostream>

// Classes for handling observations RINEX files (data)
#include "RinexObsBase.hpp"
#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"

// Class to extract C1 data from RinexObsData objects
#include "ExtractC1.hpp"

// Classes for handling satellite navigation parameters RINEX files (Broadcast ephemerides)
#include "RinexNavBase.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavData.hpp"
#include "RinexNavStream.hpp"

// Class to store satellite broadcast navigation data
#include "GPSEphemerisStore.hpp"

// Class to model GPS data for a mobile receiver
#include "ModeledPR.hpp"

// Class to model the tropospheric delays
#include "TropModel.hpp"

// Classes to model ans store ionospheric delays
#include "IonoModel.hpp"
#include "IonoModelStore.hpp"

// Class to solve the equations system using a Weighted Least Mean Square method
#include "SolverWMS.hpp"

// Class to compute the weights to be used for each satellite
#include "MOPSWeight.hpp"

// Basic framework for programs in the GPSTk. process() method MUST be implemented
#include "BasicFramework.hpp"

#include "geometry.hpp"                   // DEG_TO_RAD



using namespace std;
using namespace gpstk;


// A new class is declared that will handle program behaviour
// This class inherits from BasicFramework
class example5 : public BasicFramework
{
public:
    // Constructor declaration
    example5(char* arg0);

protected:
    // Method that will take care of processing
    virtual void process();

    // Method that hold code to be run BEFORE processing
    virtual void spinUp();

private:
    // These field represent options at command line interface (CLI)
    CommandOptionWithArg dataFile;
    CommandOptionWithArg navFile;

    // If you want to share objects and variables among methods, you'd better declare them here
    RinexObsStream rObsFile;            // Object to read Rinex observation data files
    RinexObsData rData;                 // Object to store Rinex observation data
    RinexNavStream rNavFile;            // Object to read Rinex navigation data files
    RinexNavData rNavData;              // Object to store Rinex navigation data
    RinexNavHeader rNavHeader;          // Object to read the header of Rinex navigation data files
    IonoModelStore ionoStore;           // Object to store ionospheric models
    GPSEphemerisStore bceStore;          // Object to store ephemeris
    ModeledPR modelPR;                  // Declare a ModeledReferencePR object
    MOPSTropModel mopsTM;               // Declare a MOPSTropModel object
    ExtractC1 obsC1;                    // Declare an ExtractData object
    bool useFormerPos;                  // Flag indicating if we have an a priori position
    Position formerPosition;            // Object to store the former position
    IonoModel ioModel;                  // Declare a Ionospheric Model object
    SolverWMS solver;                   // Declare an object to apply WMS method
    MOPSWeight mopsWeights;             // Object to compute satellites' weights

};


// Let's implement constructor details
example5::example5(char* arg0)
      : BasicFramework(arg0, "\nProgram to print the position solution in ECEF and longitude," 
                             " latitude, height, based in C1 and given a RINEX observations file"
                             " and a RINEX broadcast navigation file.\n\n"
                             "The output is: \n"
                             "  Time(sec)  X(m)  Y(m) Z(m)  Lon(deg)  "
                             "  Lat(deg)  Height(m)\n"),
        // Option initialization. "true" means a mandatory option
        dataFile(CommandOption::stdType, 'i', "datainput", 
                        " [-i|--datainput]      Name of RINEX observations file.", true), 
        navFile(CommandOption::stdType, 'n', "navinput", 
                        " [-n|--navinput]      Name of RINEX broadcast navigation file.", true)
{
    // These options may appear just once at CLI
    dataFile.setMaxCount(1);
    navFile.setMaxCount(1);
} // End of constructor details


// Method that will be executed AFTER initialization but BEFORE processing
void example5::spinUp()
{

    // From now on, some parts may look similar to example3.cpp and example4.cpp
    // Activate failbit to enable exceptions
    rObsFile.exceptions(ios::failbit);
    // First, data RINEX reading object
    try
    {
        rObsFile.open(dataFile.getValue()[0].c_str(), std::ios::in);
    }
    catch(...)
    {
        cerr << "Problem opening file " << dataFile.getValue()[0].c_str() << endl;
        cerr << "Maybe it doesn't exist or you don't have proper read permissions." << endl;
        exit (-1);
    }


    // Activate failbit to enable exceptions
    rNavFile.exceptions(ios::failbit);
    // Read nav file and store unique list of ephemerides
    try
    {
        rNavFile.open(navFile.getValue()[0].c_str(), std::ios::in);
    }
    catch(...)
    {
        cerr << "Problem opening file " << navFile.getValue()[0].c_str() << endl;
        cerr << "Maybe it doesn't exist or you don't have proper read permissions." << endl;
        exit (-1);
    }


    // We will need to read ionospheric parameters (Klobuchar model) from header
    rNavFile >> rNavHeader;

    // Let's feed the ionospheric model (Klobuchar type) from data in the Navigation file header
    ioModel.setModel(rNavHeader.ionAlpha, rNavHeader.ionBeta);
    // WARNING-WARNING-WARNING: In this case, the same model will be used for the full data span
    ionoStore.addIonoModel(DayTime::BEGINNING_OF_TIME, ioModel);

    // Storing the ephemeris in "bceStore"
    while (rNavFile >> rNavData) bceStore.addEphemeris(rNavData);
    // Setting the criteria for looking up ephemeris
    bceStore.SearchPast();  // This is the default

    // This is set to true if the former computed positon will be used as a priori position
    useFormerPos = false;   // At first, we don't have an a priori position

    // Prepare for printing later on
    cout << fixed << setprecision(8);

}// End of example5::spinUp()


// Method that will really process information
void example5::process()
{

    // Let's read the observations RINEX, epoch by epoch
    while (rObsFile >> rData)
    {
        if ( (rData.epochFlag == 0 || rData.epochFlag == 1) && (rData.numSvs > 3) )  // Begin usable data with enough number of satellites
        {
            int validSats = 0;   // Number of satellites with valid data in this epoch
            int prepareResult;
            double rxAltitude;  // Receiver altitude for tropospheric model
            double rxLatitude;  // Receiver latitude for tropospheric model

            // We need extract C1 data from this epoch. Skip epoch if not enough data (4 SV at least)
            if ( obsC1.getData(rData) < 4) {
                useFormerPos = false;  // The former position will not be valid next time
                continue;
            }


            if (useFormerPos) {  // If possible, use former position as a priori
                prepareResult = modelPR.Prepare(formerPosition);
                // We need to seed this kind of tropospheric model with receiver altitude
                rxAltitude = formerPosition.getAltitude();
                rxLatitude = formerPosition.getGeodeticLatitude();
            }
            else {  // Use Bancroft method is no a priori position is available
                cerr << "Bancroft method was used at epoch " << rData.time.DOYsecond() << endl;
                prepareResult = modelPR.Prepare(rData.time, obsC1.availableSV, obsC1.obsData, bceStore);
                // We need to seed this kind of tropospheric model with receiver altitude
                rxAltitude = modelPR.rxPos.getAltitude();
                rxLatitude = modelPR.rxPos.getGeodeticLatitude();
            }

            if (prepareResult) {  // If there were problems with Prepare(), skip this epoch
                useFormerPos = false;  // The former position will not be valid next time
                continue;
            }
            // If there were no problems, let's feed the tropospheric model
            mopsTM.setReceiverHeight(rxAltitude);
            mopsTM.setReceiverLatitude(rxLatitude);
            mopsTM.setDayOfYear(rData.time.DOY());


            // Now, let's compute the GPS model for our observable (C1)
            validSats = modelPR.Compute(rData.time, obsC1.availableSV, obsC1.obsData, bceStore, &mopsTM, &ionoStore);


            // Only get into further computations if there are enough satellites
            if (validSats >= 4) {

                // Now, let's solve the navigation equations using the WMS method
                try {
                    // First, compute the satellites' weights
                    int goodSv = mopsWeights.getWeights(rData.time, modelPR.availableSV, bceStore, modelPR.ionoCorrections, modelPR.elevationSV, modelPR.azimuthSV, modelPR.rxPos);

                    // Some minimum checking is in order
                    if ( goodSv != (int)modelPR.prefitResiduals.size() ) continue;

                    // Then, solve the system
                    solver.Compute(modelPR.prefitResiduals, modelPR.geoMatrix, mopsWeights.weightsVector);
                }
                catch(InvalidSolver& e) {
                    cerr << "Couldn't solve equations system at epoch " << rData.time.DOYsecond() << endl;
                    cerr << e << endl;
                    useFormerPos = false;  // The former position will not be valid next time
                    continue;
                }

                // With "solver", we got the difference vector between a priori position and
                // real position. Then, let's convert the solution to a Position object
                Position solPos( (modelPR.rxPos.X() + solver.solution[0]), (modelPR.rxPos.Y() + solver.solution[1]), (modelPR.rxPos.Z() + solver.solution[2]) );

                // Print results
                cout << rData.time.DOYsecond()  << "   ";   // Output field #1
                cout << solPos.X() << "   ";                // Output field #2
                cout << solPos.Y() << "   ";                // Output field #3
                cout << solPos.Z() << "   ";                // Output field #4
                cout << solPos.longitude() << "   ";        // Output field #5
                cout << solPos.geodeticLatitude() << "   "; // Output field #6
                cout << solPos.height() << "   ";           // Output field #7
                cout << endl;

                formerPosition = solPos;
                useFormerPos = true;  // Next time, former position will be used as a priori

            }  // End of if (validSats...
            else {
                useFormerPos = false;  // The former position will not be valid next time
            }
        } // End of "if" for usable data
        else {
            useFormerPos = false;  // The former position will not be valid next time
        }
    } // End of while

    return;

}// End of example5::process()


// Main function
int main(int argc, char* argv[])
{
   try
   {
      example5 program(argv[0]);
      if (!program.initialize(argc, argv))
         return 0;
      if (!program.run())
         return 1;

      return 0;
   }
   catch(Exception& e)
   {
      cout << "Problem: " << e << endl;
      return 1;
   }
   catch(...)
   {
      cout << "Unknown error." << endl;
      return 1;
   }
   return 0;
}
