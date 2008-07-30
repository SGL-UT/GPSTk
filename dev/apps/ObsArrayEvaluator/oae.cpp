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

// This is a redux of the mpsolve routine developed by pben. The goals are to
// make a tool that will handle all gnss observables as opposed to being limited
// to those available in rinex 2.1.

#include <iostream>

#include "Exception.hpp"
#include "DayTime.hpp"
#include "ObsEphReaderFramework.hpp"
#include "StringUtils.hpp"
#include "ValarrayUtils.hpp"
#include "ObsArray.hpp"
#include "SparseBinnedStats.hpp"

using namespace std;
using namespace gpstk;
using namespace ValarrayUtils;



class ObsArrayEvaluator : public ObsEphReaderFramework
{
public:
   ObsArrayEvaluator()
      : ObsEphReaderFramework("ObsArray Evaluator")
   {};

   void run(int argc, char *argv[]);

   void dumpRaw(const ObsArray& oa, bool numeric);
   void writeStats(const SparseBinnedStats<double>& sbs, 
                   bool numeric, bool elevation=true);
};


int main(int argc, char *argv[])
{
   try
   {
      ObsArrayEvaluator oae;
      oae.run(argc, argv);
      exit(0);
   }   
   catch (Exception& e)
   {
      cout << e << endl;
      exit(-1);
   }
}


void ObsArrayEvaluator::run(int argc, char *argv[])
{
   // Default difference that isolates multipath from the Y code
   string formula="P1-wl1*L1+2/(1-gamma)*(wl1*L1-wl2*L2)";
   formula = "C1Y-wl1*L1Y+2/(1-gamma)*(wl1*L1Y-wl2*L2Y)";

   // Default minimum length for a pass for use solution
   double minPassLength = 300;
   double angInterval = 15; 

   CommandOptionNoArg 
      rawOption('r', "raw","Output raw data instead of descriptive statistics"),
      numericOption('n', "numeric", "Format output for numerical packages"),
      azimuthOption('a', "azimuth", "Compute statistics binned by azimuth "
                    "instead of elevation"),
      debiasOption('z', "debias", "Remove the mean of each pass before "
                   "outputting or computing statistics.");

   CommandOptionWithAnyArg
      binOption('b', "bin", 
                "Defines a bin. Eliminates the default bins. Repeated use "
                "of this option defines additional bins. Value is min,max. "
                "Ex.: -b 10,90"),
      formulaOption('f', "function",
                    "Function to evaluate for the data. The default is the dual "
                    "frequency multipath combination " + formula + ".");

   CommandOptionWithNumberArg 
      lengthOption('l', "length", "Minimum length in seconds for an "
                   "overhead pass to be used. Default value is " + 
                   StringUtils::asString(minPassLength, 1) + " seconds."),
      angWidthOption('w', "width", "Width of angular bins to use. If used, "
                     "defines regular, nonoverlapping bins of azimuth or "
                     "elevation. Default value is " + 
                     StringUtils::asString(angInterval, 2) + " degrees.");

   lengthOption.setMaxCount(1);
   angWidthOption.setMaxCount(1);
   formulaOption.setMaxCount(1);
      
   initialize(argc, argv);

   DayTime now;

   bool numeric = numericOption.getCount()>0;
         
   ObsArray oa;

   if (formulaOption.getCount()>0)
      formula = formulaOption.getValue()[0];

   if ((verboseLevel) && (!numeric))
      output << "Formula: " << formula << endl;

   oa.debugLevel = debugLevel;

   oa.add(formula);
   oa.load(obsFiles, *ephReader.eph, rxPos);
   size_t originalLength = oa.getNumSatEpochs();
      
   if ((!numeric)&& (verboseLevel))
      output << "Editing points with loss of lock indication and pass with "
         "short lengths." << endl;
      
   valarray<bool> removePts = oa.lli;
   if (lengthOption.getCount()>0)
      minPassLength =  StringUtils::asDouble(lengthOption.getValue()[0]);
      
   set<long> allpasses = unique(oa.pass);
   for (set<long>::iterator i=allpasses.begin(); i!=allpasses.end(); i++)
      if (oa.getPassLength(*i)<minPassLength)
         removePts = removePts || (oa.pass==*i);
      
   oa.edit(removePts);
   allpasses = unique(oa.pass);
      
   size_t editedLength = oa.getNumSatEpochs();
   double pct_edited = 100.*(originalLength-editedLength)/originalLength;
   if (!numeric)
      output << "Edited " << (originalLength-editedLength)
           << " points (" << setprecision(2) << pct_edited << "%)."
           << endl;

   if (pct_edited > 50)
   {
      output << "Edited more than 50% of the data. This is bad. I quit." << endl;
      exit(-1);
   }

   if (debiasOption.getCount())
   {
      if (!numeric)
         output << "Removing mean of each pass." << endl;
      
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
   }

   allpasses = unique(oa.pass);
   if (!numeric)
      output << "Data collection interval is " << fixed << setprecision(1)
           << oa.interval << " seconds" << "." <<endl
           << "Overhead passes used: " << allpasses.size() << endl;

   if (rawOption.getCount()>0)
   {
      dumpRaw(oa, numeric);
   }
   else
   {
      bool byAzimuth = (azimuthOption.getCount()>0);
      if (angWidthOption.getCount()>0)
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

      writeStats(sbs, numeric, !byAzimuth);
   }
      

   DayTime then;

   if ( (verboseLevel) && (!numeric))
      output << "Processing complete in " << then - now << " seconds." << endl;
}


void ObsArrayEvaluator::dumpRaw(
   const ObsArray& oa, bool numeric)
{ 
   if (numeric)
   { 
      output << "# GPS Week, Seconds of week, Sat. id, Sat. system, Pass, ";
      output << "Multipath value, LLI indicator, Azimuth, Elevation " << endl;
   }

   for (size_t i=0; i<oa.observation.size(); i++)
   {
      if (!numeric)
      {
         output << oa.epoch[i] << " " << oa.satellite[i] << " ";
         output << "Pass " << oa.pass[i] << " ";
         output << setprecision(12) << oa.observation[i];
         if (oa.validAzEl[i])
         {
            output << setprecision(5);
            output << " Az " << oa.azimuth[i];
            output << " El " << oa.elevation[i];
         }
         if (oa.lli[i])
            output << " <- Loss of lock";

         output << endl;
      }
      else
      {
         if (oa.validAzEl[i])
         {  
            output << setprecision(4) << oa.epoch[i].GPSfullweek() << " ";
            output << setprecision(9) << oa.epoch[i].GPSsow() << " ";
            output << oa.satellite[i].id << " ";
            output << (int) oa.satellite[i].system << " ";
            output << oa.pass[i] << " ";
            output << setprecision(12) << oa.observation[i] << " ";
            output << (int) oa.lli[i];
            output << setprecision(5);
            output << " " << oa.azimuth[i];
            output << " " << oa.elevation[i];
            output << endl;
         }
      }  
   }
}


void ObsArrayEvaluator::writeStats(
   const SparseBinnedStats<double>& mstats, 
   bool numeric, bool elevation)
{
   string angDesc = "elevation";
   if (!elevation) angDesc = "azimuth";

   if(!numeric)
   {
      output << endl;
      
      output << "Statistics of bins sorted by " << angDesc << "." << endl
           << endl
           << "                   average  stddev" << endl;
      
      for (int i=0; i<mstats.stats.size(); i++)
         output << setprecision(0) << fixed
              << "From " << setw(3) << mstats.bins[i].lowerBound
              <<  " to " << setw(3) << mstats.bins[i].upperBound
              << showpoint
              << ": " << setprecision(5) << setw(9) <<mstats.stats[i].Average()
              << "  " << setprecision(3) << setw(5) << mstats.stats[i].StdDev()
              << noshowpoint
              << endl;
      output << endl;

      output << "Total points used: " << mstats.usedCount << endl
           << "         rejected: " << mstats.rejectedCount << endl;
   }
   else
   {
      output << "# Bins of " << angDesc << " -- columns are bin min, bin max, average, standard deviation " << endl;
      for (int i=0; i<mstats.stats.size(); i++)
      {
         output << mstats.bins[i].lowerBound << " "
              << mstats.bins[i].upperBound << " "
              << setprecision(5) << mstats.stats[i].Average() << " "
              << setprecision(3) << mstats.stats[i].StdDev() << endl;
      }
   }
}
