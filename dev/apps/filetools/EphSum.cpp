#pragma ident "$Id: //depot/msn/prototype/brent/IGEB_Demo/EphSum.cpp#1 $"
/**
*  Given a PRN ID and a date (DOY, Year), read the appropriate FIC data file(s)
*  and assemble a summary of all ephemerides relevant to the day for the PRN. 
*  
*/
// System
#include <stdio.h>

// gpstk
#include "FileFilterFrame.hpp"
#include "BasicFramework.hpp"
#include "StringUtils.hpp"
#include "GPSEphemerisStore.hpp"

// fic
#include "FICStream.hpp"
#include "FICHeader.hpp"
#include "FICData.hpp"
#include "FICFilterOperators.hpp"

#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavFilterOperators.hpp"

using namespace std;
using namespace gpstk;


class EphSum : public gpstk::BasicFramework
{
public:
   EphSum(const std::string& applName,
              const std::string& applDesc) throw();
   ~EphSum() {}
   virtual bool initialize(int argc, char *argv[]) throw();
   bool checkIOD( const gpstk::EngEphemeris ee, FILE* logfp );
   
protected:
   virtual void process();
   gpstk::CommandOptionWithAnyArg inputOption;
   gpstk::CommandOptionWithAnyArg outputOption;
   gpstk::CommandOptionWithAnyArg prnOption;
   gpstk::CommandOptionNoArg FICOption;
   std::list<long> blocklist;
   std::list<long> prnlist;

   FILE *logfp;
   GPSEphemerisStore ges;
   
   int numFICErrors;
};

int main( int argc, char*argv[] )
{
   try
   {
      EphSum fc("EphSum", "Summarize contents of a navigation message file.");
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

EphSum::EphSum(const std::string& applName, 
                       const std::string& applDesc) throw()
          :BasicFramework(applName, applDesc),
           inputOption('i', "input-file", "The name of the navigation message file(s) to read.", true),
           outputOption('o', "output-file", "The name of the output file to write.", true),
           FICOption('f', "FIC","Assuming FIC input rather than Rinex(default).",false), 
           prnOption('p', "PRNID","The PRN ID of the SV to process",false)
{
   inputOption.setMaxCount(2);
   outputOption.setMaxCount(1);
   prnOption.setMaxCount(1);
   FICOption.setMaxCount(1);
   numFICErrors = 0;
}

bool EphSum::initialize(int argc, char *argv[])
   throw()
{
   if (!BasicFramework::initialize(argc, argv)) return false;
   
   if (debugLevel)
   {
      cout << "Input File(s):  " << inputOption.getValue().front() << endl;
      cout << "Output File: " << outputOption.getValue().front() << endl;
      cout << "PRN ID     : "; 
      if (prnOption.getCount()==0)
      {
         cout << "all" <<  endl;    
      }
      else
      {
         cout << "PRN ID     : " <<    prnOption.getValue().front() << endl;
      }
      if (FICOption.getCount()==0) cout << "Processing FIC input" << endl;
   }
   
      // Open the output file
   logfp = fopen( outputOption.getValue().front().c_str(),"wt");
   if (logfp==0) 
   {
      cout << "Failed to open output file. Exiting." << endl;
      return 1;
   }
   fprintf(logfp,"# Output file from EphSum\n");
   
   return true;   
}

void EphSum::process()
{
   int countByPRN[gpstk::MAX_PRN+1];
   for (int i1=0;i1<=gpstk::MAX_PRN+1;++i1) countByPRN[i1] = 0;
   
   vector<string> values;
   values = inputOption.getValue();
   if (FICOption.getCount()>0)
   {
      FileFilterFrame<FICStream, FICData> input(values);
      for (int it=0;it<values.size();++it)
      {
         fprintf(logfp,"# Processing FIC input specification: %s\n",
                      values[it].c_str());
      }
   
      if(debugLevel)
      {
         cout << " input.getDataCount() after init: " << input.getDataCount() << endl;
         cout << "Setting up output file: "
              << outputOption.getValue().front() << endl;
      }
         // filter the FIC data for the requested block(s)
      std::list<long> blockList;
      blockList.push_back(9);
      input.filter(FICDataFilterBlock(blockList));
   
         //some hand waving for the data conversion
      if(debugLevel)
         cout << "Reading the input data." << endl;
      list<FICData>& ficList = input.getData();
      list<FICData>::iterator itr = ficList.begin();
      int count9 = 0;
      while (itr != ficList.end())
      {
         count9++;
         FICData& r = *itr;
         if ( r.blockNum == 9)
         {
            EngEphemeris ee(r);
            ges.addEphemeris(ee);
         }
         itr++;
      }
   }
   else
   {
      FileFilterFrame<RinexNavStream, RinexNavData> data(values);
      for (int it=0;it<values.size();++it)
      {
         fprintf(logfp,"# Processing Rinex input specification: %s\n",
                      values[it].c_str());
      }
      list<RinexNavData>& rnavlist = data.getData();
      list<RinexNavData>::iterator itr = rnavlist.begin();
      while (itr!=rnavlist.end())
      {
         RinexNavData& r = *itr;
         EngEphemeris ee(r);
         ges.addEphemeris(ee);
         itr++;
      }
   }
   
   string tform = "%04F %6.0g %02m/%02d/%02y %03j %02H:%02M:%02S";
   
   GPSEphemerisStore::EngEphMap eemap;
   long prnid=1;
   long maxprn = gpstk::MAX_PRN;
   if (prnOption.getCount()>0)
   {
      prnid = StringUtils::asInt(prnOption.getValue().front());
      maxprn = prnid;
   }
   for (int i=prnid;i<=maxprn;++i) 
   {
      SatID sat = SatID( i, SatID::systemGPS);
      try
      {
         eemap = ges.getEphMap( sat );
      }
      catch(InvalidRequest)
      {
         // simply go on to the next PRN
         fprintf(logfp,"#\n");
         fprintf(logfp,"#PRN: %02d,  # of eph: NONE\n",i);
         continue;
      }
      
         // Header
      fprintf(logfp,"#\n");
      fprintf(logfp,"#PRN: %02d,  # of eph: %02d\n",i,eemap.size());
      fprintf(logfp,"#PRN !               Xmit                !             Toe/Toc               !            End of Eff             !  IODC   Health\n");
      countByPRN[i] = eemap.size();
      GPSEphemerisStore::EngEphMap::const_iterator ci;
      for (ci=eemap.begin(); ci!=eemap.end(); ++ci)
      {
         EngEphemeris ee = ci->second;
         DayTime endEff = ee.getEphemerisEpoch();
         endEff += 7200;
         fprintf(logfp,"  %02d ! %s ! %s ! %s ! 0x%03X  0x%02X %02d \n",
               i,
               ee.getTransmitTime().printf(tform).c_str(),
               ee.getEphemerisEpoch().printf(tform).c_str(),
               endEff.printf(tform).c_str(),
               ee.getIODC(),
               ee.getHealth(),
               ee.getHealth());
         //fprintf(logfp,"    |                                   | %s |\n",
         //      ee.getEpochTime().printf(tform).c_str());
      }
   }
   
   fprintf(logfp,"#\n#Summary of Counts by PRN\n");
   fprintf(logfp,"# PRN    Count\n");
   for (int i2=1;i2<=gpstk::MAX_PRN;++i2)
   {
      fprintf(logfp,"#  %02d    %5d\n",i2,countByPRN[i2]);
   }
   
   if (debugLevel) cout << "done." << endl;
}
 
