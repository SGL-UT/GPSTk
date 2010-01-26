#pragma ident "$Id"

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
   exit(EXIT_FAILURE);
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
 * Runs through all the modified classes, reading in a RINEX 3 Nav File
 * and dumping its contents.
 */
void testDump(string filename)
{
   // Checking Rinex3EphemerisStore...
   Rinex3EphemerisStore ephstore;
   try
   {
      ephstore.loadFile(filename.c_str());
      ephstore.dump(cout, DETAILLEVEL);
   }
   catch (Exception& e)
   {
      error("Could not open file.");
   }

   // Read file from arguments.
   GalEphemerisStore galstore;
   Rinex3NavData data;
   Rinex3NavHeader header;

   Rinex3NavStream input(filename.c_str());

   if (!input)
   {
      error("Could not open file.");
   }

   // Checking Rinex3NavHeader...
   input >> header;

   header.dump(cout);

   // Add ephemeris.
   while (input >> data)
   {
      // Checking Rinex3NavData...
      if (data.satSys == "E")
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
   catch (Exception& e)
   {
      error("Invalid request!");
   }
}

/**
 * A random test.
 */
void testPlayground(string filename)
{
   GalEphemerisStore galstore;
   Rinex3NavData data;
   Rinex3NavHeader header;

   Rinex3NavStream input(filename.c_str());

   if (!input)
   {
      error("Could not open file.");
   }

   input >> header;

   header.dump(cout);

   while (input >> data)
   {
      if (data.satSys == "E")
      {
         //info("Adding ephemeris...");
         galstore.addEphemeris(data);
      }
      else
      {
         warn("Not a Galileo nav message.");
      }
   }

   newline();
      
   try
   {
      const SatID satID6(6, SatID::systemGalileo);
      const GalEphemerisStore::GalEphMap& ephmap6 = galstore.getEphMap(satID6);

      // Checking GalEphemeris...
      const GalEphemerisStore::GalEphMap::const_iterator b = ephmap6.begin();
      const GalEphemeris eph = b->second;

      cout << "IODnav  = " << eph.getIODnav() << endl;
      cout << "BGDa    = " << eph.getBGDa() << endl;
      cout << "BGDb    = " << eph.getBGDb() << endl;
      cout << "HOWtime = " << eph.getHOWTime(1) << endl;
   }
   catch (Exception& e)
   {
      error("Invalid request!");
   }
}

/**
 * Checks the svXt() and svXvt() methods of GalEphemeris.
 */
void testPosition(string filename)
{
   GalEphemerisStore galstore;
   Rinex3NavData data;
   Rinex3NavHeader header;

   Rinex3NavStream input(filename.c_str());

   if (!input)
   {
      error("Could not open file.");
   }

   input >> header;

   header.dump(cout);

   while (input >> data)
   {
      if (data.satSys == "E")
      {
         info("Adding ephemeris...");
         galstore.addEphemeris(data);
      }
      else
      {
         warn("Not a Galileo nav message.");
      }
   }

   newline();
      
   try
   {
      const SatID satID6(6, SatID::systemGalileo);
      const GalEphemerisStore::GalEphMap& ephmap6 = galstore.getEphMap(satID6);

      const GalEphemerisStore::GalEphMap::const_iterator b = ephmap6.begin();
      const GalEphemeris eph = b->second;

      Xt ephpos = eph.svXt(eph.getEphemerisEpoch());
      cout << "eph.svXt()  = " << ephpos << endl;

      Xvt ephposv = eph.svXvt(eph.getEphemerisEpoch());
      cout << "eph.svXvt() = " << ephposv << endl;
   }
   catch (Exception& e)
   {
      error("Invalid request!");
   }
}

/**
 * Reads a RINEX 3 Nav File and writes it back out. On diff, there should
 * be no differences between the two files.
 *
 * Pulled from FunctionalTests/Rinex3NavTest.cpp.
 */
void testReadWrite(string filename)
{
   try
   {
      Rinex3NavData data;
      Rinex3NavHeader header;
      
      string outputfilename = "TEST-" + filename + ".out";
      
      // Build input/output streams.
      Rinex3NavStream input(filename.c_str());
      Rinex3NavStream output(outputfilename.c_str(), ios::out);

      // Write out input to new file.
      info("Writing input to output files...");
      
      input >> header;
      header.dump(cout);
      output << header;

      while (input >> data)
      {
         output << data;
      }

      info("Done writing input to output files.");
   }
   catch (Exception& e)
   {
      cout << e;
      exit(EXIT_FAILURE);
   }
   catch (...)
   {
      error("Unknown error.");
   }
}

/**
 * Run through classes changed to implement Galileo support.
 *
 * CLASSES
 *  + GalEphemeris
 *  + GalEphemerisStore
 *  + Rinex3EphemerisStore
 *  + Rinex3NavData
 *  + Rinex3NavHeader
 *
 * USAGE
 * As an argument, this takes the filename of a Galileo Nav Message in RINEX3 format.
 *
 * For example:
 * sh$ ./GalEphemerisTest RINEX3SpecGalExample.99N.R3
 */
int main(int argc, char* argv[])
{
   // No filename supplied...
   if (argc == 1)
   {
      cout << "Usage: ./GalEphemerisTest FILE" << endl;

      exit(EXIT_FAILURE);
   }
   
   //testDump(argv[1]);
   //testPlayground(argv[1]);
   testPosition(argv[1]);
   //testReadWrite(argv[1]);
   
   return EXIT_SUCCESS;
}
