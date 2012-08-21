#pragma ident "$Id:$"
/**
*
*/
// System
#include <stdio.h>
#include <cmath>

// gpstk
#include "FileFilterFrame.hpp"
#include "BasicFramework.hpp"
#include "StringUtils.hpp"
#include "OrbElemFIC9.hpp"
#include "GPSOrbElemStore.hpp"

// fic
#include "FICStream.hpp"
#include "FICHeader.hpp"
#include "FICData.hpp"
#include "FICFilterOperators.hpp"

// Project

using namespace std;
using namespace gpstk;

class xTestRationalize : public gpstk::BasicFramework
{
public:
   xTestRationalize(const std::string& applName,
              const std::string& applDesc) throw();
   ~xTestRationalize() {}
   virtual bool initialize(int argc, char *argv[]) throw();
   
protected:
   virtual void process();
   gpstk::CommandOptionWithAnyArg outputOption;
   gpstk::CommandOptionWithAnyArg inputFileOption;
   std::list<long> blocklist;
   
   FILE *logfp;
};

int main( int argc, char*argv[] )
{
   try
   {
      xTestRationalize fc("xTestRationalize", "");
      if (!fc.initialize(argc, argv)) return(false);
      fc.run();
   }
   catch(gpstk::Exception& exc)
   {
      cout << exc << endl;
      return 1;
   }
   catch(...)
   {
      cout << "Caught an unnamed exception. Exiting." << endl;
      return 1;
   }
   return 0;
}

xTestRationalize::xTestRationalize(const std::string& applName, 
                       const std::string& applDesc) throw()
          :BasicFramework(applName, applDesc),
           inputFileOption('i',"input-file","Input FIC file.",true),
           outputOption('o', "output-file", "The name of the output file to write.", true)
{
   inputFileOption.setMaxCount(1); 
   outputOption.setMaxCount(1);
}

bool xTestRationalize::initialize(int argc, char *argv[])
   throw()
{
   if (!BasicFramework::initialize(argc, argv)) return false;
   
   if (debugLevel)
   {
      cout << "Output File: " << outputOption.getValue().front() << endl;
   }
                
   return true;   
}

void xTestRationalize::process()
{

      // Open the output stream
   char ofn[100];
   strcpy( ofn, outputOption.getValue().front().c_str());
   ofstream out( ofn, ios::out );
   if (!out) 
   {
      cerr << "Failed to open output file. Exiting." << endl;
      exit(1);
   }

      // Define the GPSOrbElemStore
   GPSOrbElemStore oeStore;
  
      // Set up the FIC data filter
   std::list<long> blockList;
   blockList.push_back(9);

   string fn = inputFileOption.getValue().front();    
   FileFilterFrame<FICStream, FICData> input(fn);

      // Filter the FIC data for the requested vlock(s)
   input.filter(FICDataFilterBlock(blockList));

   list<FICData>& ficList = input.getData();
   list<FICData>::iterator itr = ficList.begin();
   while (itr != ficList.end())
   {
      FICData& r = *itr;
      OrbElemFIC9 oe(r);
      oeStore.addOrbElem(oe);
      itr++;
   }  // End of FIC Block loop

   out << "Output from xTestRationalize.  " << endl;
   out << "Input File: " << fn << endl;
   out << "Number of Records Read: " << ficList.size() << endl << endl;
   out << "Dump of GPSOrbElemStore BEFORE the call to GPSOrbElemStore.rationalize( )" << endl;
   oeStore.dump(out, 2);
   oeStore.rationalize( );
   out << endl << endl;
   out << "Dump of GPSOrbElemStore AFTER the call to GPSOrbElemStore.rationalize( )" << endl;
   oeStore.dump(out, 2);

}



