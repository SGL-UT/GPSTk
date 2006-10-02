#pragma ident "$Id: $"

#include <iostream>

#include "Exception.hpp"
#include "DayTime.hpp"
#include "CommandOptionParser.hpp"
#include "StringUtils.hpp"
#include "ValarrayUtils.hpp"
#include "ObsArray.hpp"
#include "SparseBinnedStats.hpp"


void dumpRaw(std::ostream& ostr, const gpstk::ObsArray& oa, bool numeric);
void writeStats(std::ostream& ostr, 
                const gpstk::SparseBinnedStats<double>& sbs, 
                bool numeric, bool elevation=true);

using namespace std;
using namespace gpstk;
using namespace ValarrayUtils;


int main(int argc, char *argv[])
{
   
   try {

         // Default difference that isolates multipath 
      std::string mp_formula="P1-wl1*L1+2/(1-gamma)*(wl1*L1-wl2*L2)";
         // Default minimum length for a pass for use solution
      double minPassLength = 300;
      double angInterval = 15; 

      CommandOptionNoArg helpOption('h',"help","Display argument list.",false);
      CommandOptionNoArg verboseOption('v',"verbose",
                         "Verbose display of processing status.",false);
      CommandOptionNoArg rawOption('r',"raw",
                         "Output raw combinations not statistics",false);

      CommandOptionNoArg numericOption('n',"numeric",
                         "Format the output for numerical packages",false);
      CommandOptionNoArg azimuthOption('a',"azimuth",
            "Compute statistics binned by azimuth instead of elevation",false);
      CommandOptionWithAnyArg obsFileOption('o',"obs","RINEX observation file",true);
      CommandOptionWithAnyArg navFileOption('e',"nav",
                              "RINEX navigation (ephemeris) file",true);
      CommandOptionWithAnyArg binOption('b',"bin",
            "Defines a bin. Eliminates the default bins. Repeated use of this option defines additional bins. Value is min,max. Ex.: -b 10,90",false);

      CommandOptionWithAnyArg mpOption('m',"multipath",
            "Dual frequency multipath combination to use. Default is " + 
            mp_formula,false);
      mpOption.setMaxCount(1);

      CommandOptionWithNumberArg 
         lengthOption('l',"length",string("Minimum length in seconds for an ")+
                      string("overhead pass to be used. Default value is ") + 
                      StringUtils::asString(minPassLength, 1) + 
                      string(" seconds."), false);
      lengthOption.setMaxCount(1);

      CommandOptionWithNumberArg 
         angWidthOption('w',"width",string("Width of angular bins to use.")+
              string("If used, defines regular, nonoverlapping bins of ") +
              string("azimuth and/or elevation. Default value is ") + 
              StringUtils::asString(angInterval, 2) + 
              string(" degrees."), false);
      angWidthOption.setMaxCount(1);

      CommandOptionParser cop("GPSTk Multipath Environment Evaluator. Computes statistical model of a dual frequency multipath combination. The model is a function of azimuth and/or elevation. By default the model presented is second order statistics (std. deviation), sorted into bins of elevation.");

     cop.parseOptions(argc, argv);

     if (cop.hasErrors())    {
         cop.dumpErrors(cout);
         cop.displayUsage(cout);
         return 1;
      }
         
      if(helpOption.getCount())
      {
         cop.displayUsage(cout);
         return 0;
      }

      bool verbose=(verboseOption.getCount()>0);
      bool numeric=(numericOption.getCount()>0);
      
      DayTime now;

      if (!numeric)
      {
         cout << "Multipath Environment Evaluation Tool, a GPSTk utility" 
              << endl << endl;
      }

      if ( (verbose) && (!numeric))
      {
         cout << "Loading obs and nav files." << obsFileOption.getValue() << endl;
         cout << "Loading nav files: " << navFileOption.getValue() << endl;
      }
         
      ObsArray oa;

      if (mpOption.getCount()>0)
         mp_formula = mpOption.getValue()[0];

      oa.add(mp_formula);
      oa.load(obsFileOption.getValue(),navFileOption.getValue());
      size_t originalLength = oa.getNumSatEpochs();
      
      if ((!numeric)&& (verbose))
         cout << "Editing points with loss of lock indication and pass with short lengths." << endl;
      
      std::valarray<bool> removePts = oa.lli;
      if (lengthOption.getCount()>0)
         minPassLength =  StringUtils::asDouble(lengthOption.getValue()[0]);
      
      set<long> allpasses = unique(oa.pass);
      for (set<long>::iterator i=allpasses.begin();
           i!=allpasses.end(); i++)
      {
         if (oa.getPassLength(*i)<minPassLength)
         {
            removePts = removePts || (oa.pass==*i);
         }
      }

      oa.edit(removePts);
      
      size_t editedLength = oa.getNumSatEpochs();
      if (!numeric)
      {
         cout << "Edited " << (originalLength-editedLength) << " points (";
         cout << setprecision(2) 
              << 100.*(originalLength-editedLength)/originalLength;
         cout << "%)." << endl;
      }

      if (!numeric)
      {
         cout << "Removing mean of each pass." << endl;
      }
      
      for (set<long>::iterator iPass=allpasses.begin();
          iPass!=allpasses.end() ; iPass++)
      {
        valarray<bool> passMask = (oa.pass==*iPass);
        valarray<double> mpVals = oa.observation[passMask];
        valarray<double> binVals(mpVals.size());
        double mean = mpVals.sum() / mpVals.size();
         mpVals -= mean;
         oa.observation[passMask]=mpVals;
      }

      allpasses = unique(oa.pass);
      if (!numeric)
      {
         if (verbose)
         {   
            cout <<"Using this combination for multipath: " <<mp_formula<<endl;
            cout << "Data collection interval is " << setprecision(3)
                 << oa.interval << " seconds";
            if (oa.intervalInferred) 
               cout << ", inferred from data";
            else
               cout << ", read from file headers";
            cout << "." <<endl;
         }
         cout << "Overhead passes used: "; 
         cout << allpasses.size() << endl;
      }


      if (rawOption.getCount()>0)
      {
         dumpRaw(cout, oa, numeric);
      }
      else
      {
         bool byAzimuth = (azimuthOption.getCount()>0);
         angInterval =  StringUtils::asDouble(angWidthOption.getValue()[0]);
         bool regularIntervals = (byAzimuth || (angWidthOption.getCount()>0));

         SparseBinnedStats<double> sbs;
         if (binOption.getCount()==0)
         {  
            if (!byAzimuth)
            {
               if (!regularIntervals)
               {      
                  sbs.addBin(0,90);
                  sbs.addBin(10,30);
                  sbs.addBin(20,40);
                  sbs.addBin(40,90);
                  sbs.addBin(10,90);
               }
               else
               {
                  for (double d=0; d<90; d+=angInterval)
                     sbs.addBin(d,d+angInterval);
               }
            }
            else
            {
                 for (double d=0; d<359; d+=angInterval)
                     sbs.addBin(d,d+angInterval);
            }
         }
         else
         {
            for (int k=0; k<binOption.getValue().size(); k++)
            {  
               string temp = binOption.getValue()[k];
               string lowerWord = StringUtils::word(temp,0,',');
               string upperWord = StringUtils::word(temp,1,',');
               sbs.addBin(StringUtils::asDouble(lowerWord),
                          StringUtils::asDouble(upperWord));
            }            
         }
         
         for (set<long>::iterator iPass=allpasses.begin();
              iPass!=allpasses.end() ; iPass++)
         {
            valarray<bool> passMask = (oa.pass==*iPass);
            valarray<double> mpVals = oa.observation[passMask];
            valarray<double> binVals(mpVals.size());
            if (!byAzimuth)
               binVals = oa.elevation[passMask];
            else
               binVals = oa.azimuth[passMask];
            
            sbs.addData(mpVals, binVals);
         }

         writeStats(cout, sbs, numeric, !byAzimuth);
      }
      

      DayTime then;

      if ( (verbose) && (!numeric))
         cout << "Processing complete in " << then - now << " seconds." << endl;
      
   }   
   catch (Exception& e)
   {
      cerr << e << endl;
   }
   
   return 0;
}


void dumpRaw(std::ostream& ostr, const ObsArray& oa, bool numeric)
{
 
   if (numeric)
   { 
      ostr << "# GPS Week, Seconds of week, Sat. id, Sat. system, Pass, ";
      ostr << "Multipath value, LLI indicator, Azimuth, Elevation " << endl;
   }

   for (size_t i=0; i<oa.observation.size(); i++)
   {
      if (!numeric)
      {
         ostr << oa.epoch[i] << " " << oa.satellite[i] << " ";
         ostr << "Pass " << oa.pass[i] << " ";
         ostr << setprecision(12) << oa.observation[i];
         if (oa.validAzEl[i])
         {
            ostr << setprecision(5);
            ostr << " Az " << oa.azimuth[i];
            ostr << " El " << oa.elevation[i];
         }
         if (oa.lli[i])
            ostr << " <- Loss of lock";

         ostr << std::endl;
      }
      else
      {
         ostr << setprecision(4) << oa.epoch[i].GPSfullweek() << " ";
         ostr << setprecision(6) << oa.epoch[i].GPSsow() << " ";
         ostr << oa.satellite[i].id << " ";
         ostr << (int) oa.satellite[i].system << " ";
         ostr << oa.pass[i] << " ";
         ostr << setprecision(12) << oa.observation[i] << " ";
         ostr << (int) oa.lli[i];
         if (oa.validAzEl[i])
         {  
            ostr << setprecision(5);
            ostr << " " << oa.azimuth[i];
            ostr << " " << oa.elevation[i];
         }
         
         ostr << std::endl;
      }  
   }
}

void writeStats(std::ostream& ostr, const SparseBinnedStats<double>& mstats, 
                bool numeric, bool elevation)
{
   std::string angDesc = "elevation";
   if (!elevation) angDesc = "azimuth";

   if(!numeric)
   {
      ostr << endl;
      
      ostr << "Standard deviation of bins sorted by " << angDesc
           << "." << endl << endl;
      
      for (int i=0; i<mstats.stats.size(); i++)
      {
         ostr << "From " << setw(3) << mstats.bins[i].lowerBound;
         ostr <<  " to " << setw(3) << mstats.bins[i].upperBound;
         ostr << ": " << setprecision(3) << mstats.stats[i].StdDev() << endl;
      }
      ostr << endl;

      ostr << "Total points used: " << mstats.usedCount << endl;
      ostr << "         rejected: " << mstats.rejectedCount << endl;
   }
   else
   {
      ostr << "# Bins of " << angDesc << " -- columns are min, max, standard deviation " << endl;
      for (int i=0; i<mstats.stats.size(); i++)
      {
         ostr << setw(3) << mstats.bins[i].lowerBound << " ";
         ostr << setw(3) << mstats.bins[i].upperBound << " ";
         ostr << setprecision(3) << mstats.stats[i].StdDev() << endl;
      }
   }
   
      
   
}
