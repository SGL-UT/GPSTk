/**
 * Includes
 */
#include <iostream>

//#include "GalEphemeris.hpp"         // Included by GalEphStore.hpp
//#include "GalEphemerisStore.hpp"    // Included by Rinex3EphStore.hpp
#include "Rinex3EphemerisStore.hpp"
//#include "Rinex3NavHeader.hpp"      // Included by Rinex3EphStore.hpp
#include "Rinex3NavStream.hpp"
//#include "SatID.hpp"                // Included by GalEphStore.hpp

/**
 * Defines
 */
#define DETAILLEVEL 0

/**
 * Namespaces
 */
using namespace std;
using namespace gpstk;

/**
 * Information Output Functions
 */
void error(string message)
{
   cout << endl << "ERRR: " << message << endl;
   exit(1);
}

void info(string message)
{
   cout << endl << "INFO: " << message << endl;
}

void warn(string message)
{
   cout << endl << "WARN: " << message << endl;
}

void newline()
{
   cout << endl;
}

/**
 * DESCRIPTION
 *    Run through classes changed to implement Galileo support.
 *
 * CLASSES
 *  + GalEphemeris
 *  + GalEphemerisStore
 *  + Rinex3EphemerisStore
 *  + Rinex3NavData
 *  + Rinex3NavHeader
 *
 * USAGE
 *    As an argument, this takes the filename of a Galileo Nav Message in RINEX3 format.
 *
 *    Example:
 *    sh$ ./main RINEX3SpecGalExample.99N.R3
 */
int main(int argc, char* argv[])
{
   // No filename supplied...
   if(argc == 1)
   {
      cout << "Usage: ./main FILE" << endl;

      exit(0);
   }
   
   // Checking Rinex3EphemerisStore...
   Rinex3EphemerisStore ephstore;
   try
   {
      ephstore.loadFile(argv[1]);
      ephstore.dump(cout, DETAILLEVEL);
   }
   catch(Exception& e)
   {
      error("Could not open file.");
   }

   // Read file from arguments.
   GalEphemerisStore galstore;
   Rinex3NavData data;
   Rinex3NavHeader header;

   Rinex3NavStream input(argv[1]);

   if(!input)
   {
      error("Could not open file.");
   }

   // Checking Rinex3NavHeader...
   input >> header;

   header.dump(cout);

   // Add ephemeris.
   while(input >> data)
   {
      // Checking Rinex3NavData...
      if(data.satSys == "E")
      {
         //Checking GalEphemerisStore...
         info("Adding ephemeris...");
         galstore.addEphemeris(data);
      }
      else
      {
         warn("Not a Galileo nav message.");
      }

      data.dump(cout);
   }

   newline();
      
   galstore.dump(cout, DETAILLEVEL);

   // Take a peek at an ephemeris inside galstore.
   try
   {
      const SatID satID6(6, SatID::systemGalileo);
      const GalEphemerisStore::GalEphMap& ephmap6 = galstore.getEphMap(satID6);

      // Checking GalEphemeris...
      const GalEphemerisStore::GalEphMap::const_iterator b = ephmap6.begin();
      const GalEphemeris eph = b->second;

      eph.dump(cout);
   }
   catch(Exception& e)
   {
      error("Invalid request!");
   }

   return 0;
}
