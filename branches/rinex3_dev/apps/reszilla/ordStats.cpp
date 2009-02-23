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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

#include "OrdApp.hpp"
#include "OrdApp.cpp"
#include "icd_200_constants.hpp"
//#include "util.hpp"
#include <iostream>
#include <list>

#include "OrdApp.hpp"
#include "ElevationRange.hpp"
#include "Stats.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

class OrdStats : public OrdApp
{
public:
   OrdStats() throw();
   
   bool initialize(int argc, char *argv[]) throw();

protected:
   virtual void process();

private:
   CommandOptionWithAnyArg elevBinsOption, statsFileOption;
   CommandOptionWithNumberArg sigmaOption;
   CommandOptionNoArg wonkyOption;
   ElevationRangeList elr;
   double sigmaMult;
   bool useWonky;

};


//-----------------------------------------------------------------------------
// The constructor basically just sets up all the command line options
//-----------------------------------------------------------------------------
OrdStats::OrdStats() throw()
   : OrdApp("ordStats", "Computes ords statistics. "), 
     elevBinsOption('b', "elev-bin", "A range of elevations, used in "
                    " computing the statistical summaries. Repeat to specify"
                    " multiple bins. The default is \"-b 0-10 -b 10-20 -b"
                    " 20-60 -b 10-90\"."),
     sigmaOption('s',"sigma","Multiplier for sigma stripping used in "
                    "statistical computations. The default value is 6."),
     statsFileOption('o',"statsFile","Filename for output of stats only. Stats"
                    " will still be included at the end of the ord file."),
     wonkyOption('w',"wonky","Use wonky data in stats computation. The"
                    " default is to not use such data.")
{}

//-----------------------------------------------------------------------------
bool OrdStats::initialize(int argc, char *argv[]) throw()
{
   return OrdApp::initialize(argc,argv);
}

//-----------------------------------------------------------------------------
void OrdStats::process()
{   
   // get elevation ranges, if specified
   if (elevBinsOption.getCount())
   {
      for (int i=0; i<elevBinsOption.getCount(); i++)
      {
         string pr = elevBinsOption.getValue()[i];
         float minElev = asFloat(pr);
         stripFirstWord(pr, '-');
         float maxElev = asFloat(pr);
         elr.push_back( ElevationRange(minElev, maxElev) );
      }
   }
   else
   {
      elr.push_back( ElevationRange( 0, 10) );
      elr.push_back( ElevationRange(10, 20) );
      elr.push_back( ElevationRange(20, 60) );
      elr.push_back( ElevationRange(60, 90) );
      elr.push_back( ElevationRange(10, 90) );
   }
   if (sigmaOption.getCount())
      sigmaMult = asDouble(sigmaOption.getValue().front());
   else
      sigmaMult = 6;
   std::ofstream extraOutput;   
   if (statsFileOption.getCount())
   {
      const string fn = statsFileOption.getValue()[0];
      extraOutput.open(fn.c_str(), ios::out);
   }  
   
   if (wonkyOption.getCount())
      useWonky = true;
   else 
      useWonky = false;
      
   // initialize some counters
   float totalEpochCount = 0; // cnt of total # of epochs from input
   float totalORDCount   = 0; // cnt of total # ord ORDs
   float wonkyEpochCount = 0; // cnt of entire epochs that are wonky
   float wonkyORDCount   = 0; // cnt of individual ords that are wonky 
   
   // read in data from the ord file to map of ORDEpochs
   ORDEpochMap oem;
   while (input)
   {
      ORDEpoch ordEpoch = read(input);
      
      // increment wonky counters accordingly
      if (ordEpoch.wonky)
        wonkyEpochCount++;
      totalEpochCount++;    
      oem[ordEpoch.time] = ordEpoch;
      write(output, ordEpoch);   
   }   
   
   // output clock offsets greater than 1ms
   output << "#  Time \t\t\tOffsets > 1ms\n"
          << "# ------\t\t\t-------------\n";  
   if (statsFileOption.getCount())
      extraOutput << "Time \t\t\tOffsets > 1ms\n"
          << "------\t\t\t-------------\n";         
   bool foundBigOffset = false;
   // find offsets > 1ms and get one more wonky count
   ORDEpochMap::iterator iter;
   for (iter = oem.begin(); iter != oem.end(); iter++)
   {
      try
      {
         if (!iter->second.clockOffset.is_valid())
            continue;
        const double offset = iter->second.clockOffset;
        if (abs(offset) > (C_GPS_M/1000))
        {
          foundBigOffset = true;
          output << ">b  " << iter->second.time << "\t\t"
                << setprecision(5) << setw(12) 
                << iter->second.clockOffset << endl;
          if (statsFileOption.getCount())
            extraOutput << iter->second.time << "\t"
                        << setprecision(5) << setw(12) 
                        << iter->second.clockOffset << endl;
        }       
        
        ORDEpoch::ORDMap::const_iterator pi;
        for (pi = iter->second.ords.begin(); 
            pi != iter->second.ords.end(); pi++)
        {
          totalORDCount++;
          const unsigned wonk = pi->second.wonky;
          if (wonk)
            wonkyORDCount++;        
        }
      }
      catch (gpstk::Exception &exc)
      {
         cerr << "Caught exception: " << exc << endl;
      }
   }  
   
   
   if (!foundBigOffset)
      output << "#     No offsets greater than 1 millisecond found.\n";
   if ((!foundBigOffset) && statsFileOption.getCount())
      extraOutput << "     No offsets greater than 1 millisecond found.\n";
   
   // output wonky stats
   output << "# wonky epochs   total   % wonky epochs   # wonky ords   total "
          << "ords   % wonky ords\n"
          << "# ------------   -----   --------------   ------------"
          << "   ----------   ------------\n";   
   char b1[200];
        // the high # after % symbol is just kinda lazy formatting...
   sprintf(b1, ">w %8.0f  %9.0f  %12.2f  %12.0f  %12.0f  %12.2f",
           wonkyEpochCount, totalEpochCount, 
           (100*(wonkyEpochCount/totalEpochCount)),
           wonkyORDCount,totalORDCount,
           (100*(wonkyORDCount/totalORDCount)));
   output << b1 << endl; 
              
   if (statsFileOption.getCount())
   {
      extraOutput << "wonky epochs   total   % wonky epochs   # wonky ords"
             << "   total ords   % wonky ords\n"
             << "------------   -----   --------------   ------------  "
             << " ----------   ------------\n";
      sprintf(b1, "%8.0f  %9.0f  %12.2f  %12.0f  %12.0f  %12.2f", 
              wonkyEpochCount,
              totalEpochCount, (100*(wonkyEpochCount/totalEpochCount)),
              wonkyORDCount,totalORDCount, 
              (100*(wonkyORDCount/totalORDCount)));
      extraOutput << b1 << endl;    
   }
   
   // print some header info   
   output << "#  elev\t  stddev      mean      # obs   # bad"
          << "   max    strip\n"
          << "#  ----\t  ------      ----      -----   -----"
          << "  -----   -----\n"; 
   if (statsFileOption.getCount())
   {
      extraOutput << "elev\t  stddev    mean      # obs   # bad"
                  << "   max    strip\n"
                  << "----\t  ------    ----      -----   -----"
                  << "  -----   -----\n";
   }
   
   // compute stats for each elevation range
   for (ElevationRangeList::const_iterator i = elr.begin(); 
        i != elr.end(); i++)
   {
      ElevationRange er = *i;
      float minElevation = er.first;
      float maxElevation = er.second;
      
      Stats<double> fp;
      ORDEpochMap::iterator iter;
      for (iter = oem.begin(); iter != oem.end(); iter++)
      {
         const DayTime& t = iter->first;
         ORDEpoch::ORDMap::const_iterator pi;
         for (pi = iter->second.ords.begin(); 
              pi != iter->second.ords.end(); pi++)
         {
            const float el = pi->second.getElevation();
            const double ord = pi->second.getORD();
            if (el>minElevation && el<maxElevation)
               fp.Add(ord);
         }
      } 
      double strip = sigmaMult * fp.StdDev();
      Stats<double> good, bad;
      for (iter = oem.begin(); iter != oem.end(); iter++)
      {
         const DayTime& t = iter->first;
         ORDEpoch::ORDMap::const_iterator pi;
         for (pi = iter->second.ords.begin(); 
             pi != iter->second.ords.end(); pi++)
         {
            const float el = pi->second.getElevation();
            const double ord = pi->second.getORD();
            const unsigned wonk = pi->second.wonky;
            if (el>minElevation && el<maxElevation)
            {
               double mag=std::abs(ord);
               if (wonk && !useWonky)
                  bad.Add(ord);
               else if (mag < strip)
                  good.Add(ord);
               else
                  bad.Add(ord);
            }
         }
      }
      
      char b1[200];
      char zero = good.Average() < good.StdDev()/sqrt((float)good.N())?'0':' ';
      double max = std::max(std::abs(good.Maximum()),
                   std::abs(good.Minimum()));
      sprintf(b1, ">r %2d-%2d  %8.5f  %8.3f  %7d  %6d  %6.2f  %6.2f",
           (int)minElevation, (int)maxElevation,
           good.StdDev()/sqrt((float)2), good.Average(),
           good.N(), bad.N(), max, strip);
      output << b1 << endl; 
      
      if (statsFileOption.getCount())
      {
        sprintf(b1, "%2d-%2d  %8.5f  %8.3f  %7d  %6d  %6.2f  %6.2f",
               (int)minElevation, (int)maxElevation,
               good.StdDev()/sqrt((float)2), good.Average(),
               good.N(), bad.N(), max, strip);
        extraOutput << b1 << endl;   
      }
   } 
}

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      OrdStats crap;
      if (!crap.initialize(argc, argv))
         exit(0);
      crap.run();
   }
   catch (gpstk::Exception &exc)
   { cout << exc << endl; }
   catch (std::exception &exc)
   { cerr << "Caught std::exception " << exc.what() << endl; }
   catch (...)
   { cerr << "Caught unknown exception" << endl; }
}

