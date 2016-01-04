#include "SinexStream.hpp"
#include "SinexData.hpp"


   /**
    * @file sinex_test.cpp
    * Tests gptk::SinexStream, gpstk::SinexData, gpstk::SinexHeader
    * by reading in a SINEX file and then writing it back out.
    * Sample input files can be found in the data directory.
    * The output file is named "sinex_test.out"
    */

using namespace std;
using namespace gpstk;

   /**
    * Run the test - read in a SINEX file and write it back out.
    *
    * @return 0 if successful
    */
int main(int argc, char *argv[])
{
   if (argc < 2)
   {
      cerr << "Can has Sinex file pls?  Exiting." << endl;
      exit(-1);
   }

   try
   {
      Sinex::Data  data;

      cout << "Reading " << argv[1] << ". . ." << endl;
      Sinex::Stream  input(argv[1]);
      input.exceptions(fstream::eofbit | fstream::failbit);
      input >> data;
      cout << "Done." << endl;

      data.dump(cout);

      cout << "Writing data to sinex_test.out . . . " << endl;
      Sinex::Stream  output("sinex_test.out", ios::out | ios::ate);
      output.exceptions(fstream::eofbit | fstream::failbit);
      output << data;
      cout << "Done." << endl;

      exit(0);
   }
   catch(Exception& e)
   {
      cerr << e;
   }
   catch (...)
   {
      cerr << "Unknown error.  Done." << endl;
   }
   exit(1);

} // main()
