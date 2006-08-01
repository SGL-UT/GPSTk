#pragma ident "$Id$"


// gpstk
#include "FileFilterFrame.hpp"
#include "BasicFramework.hpp"
#include "StringUtils.hpp"
#include "FICStream.hpp"
#include "FICHeader.hpp"
#include "FICData.hpp"
#include "FICFilterOperators.hpp"
#include "BCEphemerisStore.hpp"
#include "EngEphemeris.hpp"

using namespace std;
using namespace gpstk;

class MinSfTest : public gpstk::BasicFramework
{
public:
   MinSfTest(const std::string& applName,
              const std::string& applDesc) throw();
   ~MinSfTest() {}
   virtual bool initialize(int argc, char *argv[]) throw();
protected:
   virtual void process();
   gpstk::CommandOptionWithAnyArg inputOption;
   gpstk::CommandOptionWithAnyArg outputOption;
   std::list<long> blocklist;
   
   BCEphemerisStore bc109;
   BCEphemerisStore minRaw;
   std::ofstream fp;
   
   void testXvt( short PRN, gpstk::DayTime dt );
};

int main( int argc, char*argv[] )
{
   try
   {
      MinSfTest fc("MinSfTest", "Process one (or more) FIC files.");
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
      cout << "Caught an unnamed exception. Exiting.";
      return 1;
   }
   return 0;
}

MinSfTest::MinSfTest(const std::string& applName, 
                       const std::string& applDesc) throw()
          :BasicFramework(applName, applDesc),
           inputOption('i', "input-file", "The name of the FIC file to raead.", true),
           outputOption('o', "output-file", "The name of the output file to write.", true)
{
   inputOption.setMaxCount(1);
   outputOption.setMaxCount(1);
}

bool MinSfTest::initialize(int argc, char *argv[])
   throw()
{
   if (!BasicFramework::initialize(argc, argv)) return false;
   if (debugLevel)
   {
      cout << "Input File:  " << inputOption.getValue().front() << endl;
      cout << "Output File: " << outputOption.getValue().front() << endl;
   }
   return true;   
}

void MinSfTest::process()
{
   if (debugLevel)
      cout << "Setting up input file: " 
           <<  inputOption.getValue().front() << endl;
   FileFilterFrame<FICStream, FICData>
      input(inputOption.getValue().front());
   
   printf(" input.getDataCount() after init: %d\n", input.getDataCount());
   if(debugLevel)
      cout << "Setting up output file: "
           << outputOption.getValue().front() << endl;
           
   fp.open( outputOption.getValue().front().c_str() );
   if ( !fp.is_open() )
   {
      printf(" Failed to open output file.\n");
      exit(1);
   }
           
      // filter the FIC data for the requested vlock(s)
   std::list<long> blockList;
   blockList.push_back(109);
   input.filter(FICDataFilterBlock(blockList));
   input.sort(FICDataOperatorLessThanBlock109());
   input.unique(FICDataUniqueBlock109());
   
      //some hand waving for the data conversion
   if(debugLevel)
      cout << "Reading the input data." << endl;
   list<FICData>& ficList = input.getData();
   list<FICData>::iterator itr = ficList.begin();
   
   DayTime earliest( DayTime::END_OF_TIME );
   DayTime latest( DayTime::BEGINNING_OF_TIME );
   int count = 0;
   int numMismatches = 0;
   int numMismatchEph = 0; 
   while (itr != ficList.end())
   {
      EngEphemeris ee(*itr);

      bc109.addEphemeris( ee );

      DayTime ct = ee.getEpochTime();
      if (ct>latest) latest = ct;
      if (ct<earliest) earliest = ct;
      
         // Following code simulates a situation where only words 3-10
         // and the estimated time of receipt are available.
      DayTime timeOfReceipt = ee.getTransmitTime();
      FICData& fic = *itr;
      long sf1min[8];
      long sf2min[8];
      long sf3min[8];
      int wrdCnt = 8;
      int i;
      for (i=0; i<wrdCnt; ++i) sf1min[i] = fic.i[4+i];   
      for (i=0; i<wrdCnt; ++i) sf2min[i] = fic.i[14+i];   
      for (i=0; i<wrdCnt; ++i) sf3min[i] = fic.i[24+i];  

      EngEphemeris eeMin;
      short PRNID = (short) fic.i[1];
      eeMin.addIncompleteSF1Thru3( sf1min, sf2min, sf3min,
             (long) timeOfReceipt.GPSsecond(), timeOfReceipt.GPSfullweek(),
             PRNID, 0 );
      minRaw.addEphemeris( eeMin );
      
         // Compare non-orbit portions of the two objects
      bool mismatch = false;
      for (int i=1; i<=3; ++i)
      {
         if (!ee.isData(i) || !eeMin.isData(i))
         {
            mismatch = true;
            fp << "ERROR: not all subframes are claimed available.";
         }
      }
      
      if (ee.getIODC()!=eeMin.getIODC()) 
      {
         mismatch = true;
         fp << "ERROR: IODCs do not match."; 
      }
      if (ee.getIODE()!=eeMin.getIODE()) 
      {
         mismatch = true;
         fp << "ERROR: IODCs do not match."; 
      }
      if (ee.getFitInterval() != eeMin.getFitInterval() )
      {
         mismatch = true;
         fp << "ERROR: fit intervals do not match.";
      }
      if (ee.getCodeFlags()!=eeMin.getCodeFlags() )
      {
         mismatch = true;
         fp << "ERROR: code flags do not match.";
      }
      if (ee.getL2Pdata()!=eeMin.getL2Pdata() )
      {
         mismatch = true;
         fp << "ERROR: L2P data flags do not match.";
      }
      if (ee.getAccuracy()!=eeMin.getAccuracy() )
      {
         mismatch = true;
         fp << "ERROR: accuracy values do not match.";
      }
      if (ee.getAccFlag()!=eeMin.getAccFlag() )
      {
         mismatch = true;
         fp << "ERROR: accuracy flags do not match.";
      }
      if (ee.getHealth()!=eeMin.getHealth() )
      {
         mismatch = true;
         fp << "ERROR: health values do not match.";
      }
      if (ee.getFitInt()!=eeMin.getFitInt() )
      {
         mismatch = true;
         fp << "ERROR: Fit interval values do not match.";
      }

      if (mismatch)
      {
         fp << " PRNID: " << PRNID << ", IODC: 0x " << hex << ee.getIODC() << dec << endl;
         numMismatches++;
      }
      
      itr++;
      count++;
   }
   cout << "Number of Block 109 records read: " << count << endl;
   if (numMismatches!=0)
   {
      printf("Errors detected.  Some ephemerides did not match in both forms.\n");
      printf("Number of mismatches: %d\n",numMismatches);
   }
   fp << "Number of mismatches detected: " << numMismatches << endl;
   if (debugLevel) cout << "done." << endl;
   

      // Generate test positions for PRN 1 and PRN 31 at
      // earliest epoch, latest epoch, and middle of the time span.
   try
   {
      double timeDuration = latest - earliest;
      DayTime middle( earliest );
      middle += (timeDuration/2);

      short IODC109 = -1;
      short IODCMin = -1;
      short PRNID = 1;
      printf("--- PRN 1 Examples ---\n");
      printf("\nEarliest time\n");
      fp << "--- PRN 1 Examples ---" << endl;
      fp << endl << "Earliest time" << endl;
      testXvt( PRNID, earliest );
      printf("\nMiddle time\n");
      fp << endl << "Middle time" << endl;
      testXvt( PRNID, middle );
      printf("\nLatest time\n");
      fp << endl << "Latest time" << endl;
      testXvt( PRNID, latest );

      PRNID = 31;      
      printf("\n--- PRN 31 Examples ---\n");
      printf("\nEarliest time\n");
      fp << endl << "--- PRN 31 Examples ---" << endl;
      fp << endl << "Earliest time" << endl;
      testXvt( PRNID, earliest );
      printf("\nMiddle time\n");
      fp << endl << "Middle time" << endl;
      testXvt( PRNID, middle );
      printf("\nLatest time\n");
      fp << endl << "Latest time" << endl;
      testXvt( PRNID, latest );
   }
   catch (gpstk::Exception& e)
   {
      cout << e << endl;
   }
   
   printf("\nBCEphemerisStore directly from existing FIC handlers.\n");
   fp << endl << "BCEphemerisStore directly from existing FIC handlers." << endl;
   bc109.dump(1, fp);
   printf("BCEphemerisStore from new method.\n");
   fp << endl << "BCEphemerisStore from new method." << endl;
   minRaw.dump(1, fp);
   
      // write the file data
   if (debugLevel) cout << "Conversion complete." << endl;
}

void MinSfTest::testXvt( short PRN, gpstk::DayTime dt )
{
      short IODC109;
      short IODCMin;
      Xvt xvt109 = bc109.getPrnXvt( PRN, dt, IODC109 );
      Xvt xvtMin = minRaw.getPrnXvt( PRN, dt, IODCMin );
      printf("               X(m)             Y(m)            Z(m)      IODC\n");
      printf("  109  %15.3lf  %15.3lf %15.3lf  0x%03X\n",
         xvt109.x[0],xvt109.x[1],xvt109.x[2],IODC109);
      printf("  Min  %15.3lf  %15.3lf %15.3lf  0x%03X\n",
         xvtMin.x[0],xvtMin.x[1],xvtMin.x[2],IODCMin);
         
      
      fp << "               X(m)           Y(m)           Z(m)       IODC" << endl;
      fp << "  109  ";
      fp << setprecision(3) << fixed;
      fp << setw(15) << xvt109.x[0];
      fp << setw(15) << xvt109.x[1];
      fp << setw(15) << xvt109.x[2];
      fp << "   0x" << setw(3) << setfill('0') << uppercase << hex << IODC109;
      fp << setfill(' ') << endl;
      fp << "  Min  ";
      fp << setprecision(3) << fixed;
      fp << setw(15) << xvtMin.x[0];
      fp << setw(15) << xvtMin.x[1];
      fp << setw(15) << xvtMin.x[2];
      fp << "   0x" << setw(3) << setfill('0') << uppercase << hex << IODCMin << dec;
      fp << setfill(' ') << endl;
}



