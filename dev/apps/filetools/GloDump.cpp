#pragma ident "$Id:$"
/**
*
*  This program scans ephemeris data and find the largest and smallest
*  value of af0 throughout the period scanned. 
*
*  However, it is not a bad "pattern" for a program that reads each
*  FIC file within a year and "does something" wrt each Block 9.
*/
// System
#include <list>
#include <iostream>
#include <fstream>
#include <cmath>

// gpstk
#include "FileFilterFrame.hpp"
#include "BasicFramework.hpp"
#include "StringUtils.hpp"
#include "gps_constants.hpp"
#include "TimeString.hpp"
#include "TimeConstants.hpp"

// GLONASS Eph Handling
#include "GloEphemeris.hpp"

// Rinex
#include "Rinex3NavStream.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavFilterOperators.hpp"

// Project
using namespace std;
using namespace gpstk;

class GloDump : public gpstk::BasicFramework
{
public:
   GloDump(const std::string& applName,
              const std::string& applDesc) throw();
   ~GloDump() {}
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
   virtual bool initialize(int argc, char *argv[]) throw();
#pragma clang diagnostic pop
protected:
   virtual void process();
   gpstk::CommandOptionWithAnyArg outputOption;
   gpstk::CommandOptionWithAnyArg inputFileOption;
   gpstk::CommandOptionNoArg terseOption;
   gpstk::CommandOptionWithNumberArg slotOption;
   
   ofstream out;
   bool terseFlag;
   list <long> slotFilterList;
};

int main( int argc, char*argv[] )
{
   try
   {
      GloDump fc("GloDump", "");
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
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
GloDump::GloDump(const std::string& applName, 
                       const std::string& applDesc) throw()
          :BasicFramework(applName, applDesc),
           inputFileOption('i',"input-file","The name of the input file to read.",true),
           outputOption('o', "output-file", "The name of the output file to write.", true),
           slotOption( 's', "slotId", "Filter fo this (these) slot(s)", false),
           terseOption( 't', "terse", "one-line output", false),
           terseFlag(false)
{
   inputFileOption.setMaxCount(1); 
   outputOption.setMaxCount(1);
}
#pragma clang diagnostic pop
bool GloDump::initialize(int argc, char *argv[])
   throw()
{
   if (!BasicFramework::initialize(argc, argv)) return false;
   
   if (debugLevel)
   {
      cout << " Input File: " << inputFileOption.getValue().front() << endl;
      cout << "Output File: " << outputOption.getValue().front() << endl;
   }
   
      // Open the output file
   out.open( outputOption.getValue().front().c_str());
   if (out.fail())
   {
      cout<<"Error opening output file"<<endl;
      return false;
   }
   out<<"GloDump"<<endl;
   if (terseOption.getCount()) terseFlag = true;

   if (slotOption.getCount())
   {
      for (size_t i = 0; i < slotOption.getCount(); i ++)
      {
            slotFilterList.push_back(atol(slotOption.getValue()[i].c_str()));
      }
   }
                 
   return true;   
}

void GloDump::process()
{
   bool first = true;
   long recordCount = 0;
#pragma unused(recordCount)

   string fn =inputFileOption.getValue().front();

   cout << "Opening file " << fn << endl;
      
   FileFilterFrame<Rinex3NavStream, Rinex3NavData>
                           data(fn);
   
   int recCount = 0;
   
   if (!slotFilterList.empty())
   data.filter(Rinex3NavDataFilterPRN(slotFilterList));
   
   list<Rinex3NavData>& rnavlist = data.getData();
   list<Rinex3NavData>::iterator itr = rnavlist.begin();
   while (itr!=rnavlist.end())
   {
      Rinex3NavData& r = *itr;
      GloEphemeris ge(r);
      if (first && terseFlag)
         ge.terseHeader(out);
      if (terseFlag) ge.terseDump(out);
      else ge.prettyDump(out);
      recCount++;
      
      itr++;
      first = false;
   }
   out<<"# of records read:"<<recCount<<endl;
      // Write the results out in a form we can suck them into a spreadsheet
      // for plotting and statistics. 
   cout << "All done" << endl;

   out.close();
}



