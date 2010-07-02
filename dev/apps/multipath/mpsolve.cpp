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
//  Copyright 2008, The University of Texas at Austin
//
//============================================================================

#include <iostream>
#include <cmath>
#include <valarray>

#include "Exception.hpp"
#include "DayTime.hpp"
#include "CommandOptionParser.hpp"
#include "StringUtils.hpp"
#include "ValarrayUtils.hpp"
#include "ObsArray.hpp"

#include "Matrix.hpp"
#include "MatrixFunctors.hpp"
#include "MatrixOperators.hpp"

// For robust statistics
#include "RobustStats.hpp"
#include "Stats.hpp"

// I added these
#include "SVGImage.hpp"
#include "SurfacePlot.hpp"
#include "Frame.hpp"
#include "BorderLayout.hpp"
#include "HLayout.hpp"
#include "VLayout.hpp"
#include "Fillable.hpp"
#include "FileHunter.hpp"
#include "FileSpec.hpp"
//

// Special to this application
#include "SparseBinnedStats.hpp"
#include "DenseBinnedStats.hpp"

using namespace std;
using namespace gpstk;
using namespace ValarrayUtils;
using namespace StringUtils;

// I added these
using namespace vdraw;
using namespace vplot;
//

void plotAzElSurf(const DenseBinnedStats<double>& mstats,
double minAz, double maxAz, int azBinSize,
double minEl, double maxEl, int elBinSize,
bool view=false);
void dumpRaw(ostream& ostr, const ObsArray& oa, bool numeric);
void writeStats(ostream& ostr, const SparseBinnedStats<double>& sbs, valarray<double>& stDevStats, valarray<double>& meanStats, bool numeric, bool elevation=true, bool plot=true);
int findIntersection(valarray<double>& elevLow, valarray<double>& azimLow, valarray<double>& obsLow, valarray<double>& elevHigh, valarray<double>& azimHigh, valarray<double>& obsHigh, long& idx_i, long& idx_j, double& eint, double& aint, long& intindex);
void removeBiases(ObsArray& oa, bool verbose);

int main(int argc, char *argv[])
{

   try
   {
      // Default difference that isolates multipath
      std::string mp_formula="P1-wl1*L1+2/(1-gamma)*(wl1*L1-wl2*L2)";
      // Default minimum length for a pass for use solution
      double minPassLength = 300;
      double angInterval = 15;
      double upperZeroMeanElevation = 15; 
      

      CommandOptionNoArg helpOption('h',"help","Display argument list",false);
      CommandOptionNoArg verboseOption('v',"verbose",
         "Verbose display of processing status",false);
      CommandOptionNoArg rawOption('r',"raw",
         "Output raw combinations not statistics",false);
      CommandOptionNoArg numericOption('n',"numeric",
         "Format the output for numerical packages",false);
      CommandOptionNoArg azimuthOption('a',"azimuth",
         "Compute statistics binned by azimuth instead of elevation",false);
      CommandOptionNoArg dualFrequencyMethodOption('d',"dfm",
         "Performs dual-frequency method",false);
      CommandOptionNoArg completeOption('c',"complete",
         "Consider multiple inputs as single input",false);

      CommandOptionWithAnyArg obsFileOption('o',"obs","RINEX observation file",true);
      CommandOptionWithAnyArg navFileOption('e',"nav",
         "RINEX navigation (ephemeris) file",true);
      CommandOptionWithAnyArg binOption('b',"bin",
         "Defines a bin. Eliminates the default bins. Repeated use of this option defines additional bins. Value is min,max. Ex.: -b 10,90",false);

      CommandOptionWithAnyArg mpOption('m',"multipath",
         "Dual frequency multipath combination to use. Default is " +
         mp_formula,false);
      mpOption.setMaxCount(1);

      CommandOptionWithAnyArg
	uzOption('u',
		 "upper",
		 "Set the upper limit on elevations assumed to have a zero mean multipath. Units degrees. Default is " +  asString(upperZeroMeanElevation,1)+ " degrees",
		 false);
      uzOption.setMaxCount(1);

      CommandOptionWithAnyArg plotOption('p',"plot",
         "Creates a surface plot with azimuth and elevation bins. The number of azimuth bins and elevation bins must be entered. Value is number of azimuth bins and number of elevation bins. Ex.: -p 36,6",false);

      CommandOptionWithAnyArg fileOption('f',"file",
         "Creates a list of input files meeting a range of date criteria. The day of year and year for the beginning and ending range must be entered. Input is beginning day of year and year, and then ending day of year and year. Ex.: -f 001,2009,007,2010",false);

      CommandOptionWithNumberArg
         lengthOption('l',"length",string("Minimum length in seconds for an ")+
         string("overhead pass to be used. Default value is ") +
         StringUtils::asString(minPassLength, 1) +
         string(" seconds."), false);
      lengthOption.setMaxCount(1);

      CommandOptionWithNumberArg
         angWidthOption('w',"width",string("Width of angular bins to use. ")+
         string("If used, defines regular, nonoverlapping bins of ") +
         string("azimuth and/or elevation. Default value is ") +
         StringUtils::asString(angInterval, 2) +
         string(" degrees."), false);
      angWidthOption.setMaxCount(1);

      CommandOptionNoArg viewOption(0,"view",
         "Launch viewer to see the plot (only valid with plot option).",false);

      CommandOptionParser cop("GPSTk Multipath Environment Evaluator. Computes statistical model of a dual frequency multipath combination. The model is a function of azimuth and/or elevation. By default the model presented is second order statistics (std. deviation), sorted into bins of elevation.");

      cop.parseOptions(argc, argv);

      if (cop.hasErrors())
      {
         cop.dumpErrors(cout);
         cop.displayUsage(cout);
         return 1;
      }

      if(helpOption.getCount())
      {
         cop.displayUsage(cout);
         return 0;
      }

      DayTime now;

      bool verbose=(verboseOption.getCount()>0);
      bool numeric=(numericOption.getCount()>0);
      bool dualfrequencymethod=(dualFrequencyMethodOption.getCount()>0);
      bool complete=(completeOption.getCount()>0);
      bool fileoption=(fileOption.getCount()>0);

      if (!numeric)
      {
         cout << "Multipath Environment Evaluation Tool, a GPSTk utility" << endl << endl;
      }

      if ( (verbose) && (!numeric))
      {
         cout << "Loading obs file(s): " << obsFileOption.getValue() << endl;
         cout << "Loading nav file(s): " << navFileOption.getValue() << endl;
      }

      ObsArray oa;

      if (mpOption.getCount()>0)
      {
         mp_formula = mpOption.getValue()[0];
      }

      oa.add(mp_formula);

      if (uzOption.getCount()>0)
      {
	 upperZeroMeanElevation	= asDouble(uzOption.getValue()[0]);
      }

      vector<string> obsList;
      vector<string> navList;

      if (fileoption)
      {
         string beginningDay;
         string beginningYear;
         string endingDay;
         string endingYear;

         // The fileOption should have a max count of 1 so this loop is unneccesary maybe even dangerous.
         // You still want to use this logic. You don't need the STringUtils namespace anymore as I put that at the top.
         for (int k=0 ; k<fileOption.getValue().size() ; k++)
         {
            string temp = fileOption.getValue()[k];
            beginningDay = StringUtils::word(temp,0,',');
            beginningYear = StringUtils::word(temp,1,',');
            endingDay = StringUtils::word(temp,2,',');
            endingYear = StringUtils::word(temp,3,',');
        
            int beginDOY = StringUtils::asInt(beginningDay);
            int beginY = StringUtils::asInt(beginningYear);
            int endDOY = StringUtils::asInt(endingDay);
            int endY = StringUtils::asInt(endingYear);

            DayTime firstDay = DayTime::BEGINNING_OF_TIME;
            DayTime lastDay = DayTime::END_OF_TIME;
            firstDay.setYDoy(beginY, beginDOY);
            lastDay.setYDoy(endY, endDOY);

            // The program won't run without an obsFileOption , don't need to check
            if (obsFileOption.getCount()>0)
            {
               for (int i=0 ; i<obsFileOption.getCount() ; i++)
               {
                  FileHunter fhobs(obsFileOption.getValue()[i]);
                  vector<string> obsListNew = fhobs.find(firstDay, lastDay, FileSpec::ascending);
                  obsList.insert(obsList.end(), obsListNew.begin(), obsListNew.end());

               }
            }

            if (navFileOption.getCount()>0)
            {
               for (int i=0 ; i<navFileOption.getCount() ; i++)
               {
                  FileHunter fhnav(navFileOption.getValue()[i]);
                  vector<string> navListNew = fhnav.find(firstDay, lastDay, FileSpec::ascending);
                  navList.insert(navList.end(),navListNew.begin(), navListNew.end());
               }
            }
	 }
      }

      if ((verbose) && !numeric)
      {
	cout << endl << "Using this combination for multipath: " <<mp_formula<<endl;
      }

      if (!fileoption)
      {
         obsList = obsFileOption.getValue();
         navList = navFileOption.getValue();
      }
      double fileCounter = 0;

                                 // while processing files
      while (fileCounter<obsList.size())
      {
         if (complete)
         {
            oa.load(obsList,navList);
            fileCounter=obsList.size();
         }
         else
         {
            if (verbose)
               cout << endl << "Processing obs file " << obsList[fileCounter] << endl;
            oa.load(obsList[fileCounter],navList[fileCounter]);
            fileCounter++;
         }

         size_t originalLength = oa.getNumSatEpochs();

         if ((!numeric)&& (verbose))
         {
            cout << "Editing points with loss of lock indication and pass with short lengths." << endl;
         }

                                 // lli stands for: loss of lock indication
         std::valarray<bool> removePts = oa.lli;
         if (lengthOption.getCount()>0)
         {
            minPassLength =  StringUtils::asDouble(lengthOption.getValue()[0]);
         }

         set<long> allpasses = unique(oa.pass);
         for (set<long>::iterator i=allpasses.begin() ; i!=allpasses.end() ; i++)
         {
            if (oa.getPassLength(*i)<minPassLength)
            {
               removePts = removePts || (oa.pass==*i);
            }
         }

         oa.edit(removePts);

         // Now only long passes remain.
         // Next use robust stats to remove cycle slips\
         // Start with a clean slate
         removePts.resize(oa.lli.size());
         removePts = false;

         allpasses = unique(oa.pass);

         // Adjust remaining passes to the median.
         if ((!numeric)&& (verbose))
         {
	   cout << "Computing the median of each pass and adjusting the pass by that value." << endl;
	 }
         
         for (set<long>::iterator i=allpasses.begin() ; 
	      i!=allpasses.end() ; i++)
         {
 	    // Storage for robust statistics
	    double median, mad; 
            
            valarray<bool> thisPass = (oa.pass==*i);
            valarray<double> s = oa.observation[thisPass];
            QSort(&s[0],s.size());
            mad = Robust::MedianAbsoluteDeviation(&s[0],s.size(),median);
            
            valarray<double> mpVals = oa.observation[thisPass];
            mpVals -= median;
            oa.observation[thisPass]=mpVals;
         }

         // Now recompute the MAD
         double allMedian, allMad;
         valarray<double> allmp(oa.observation);
         QSort(&allmp[0], allmp.size());
         allMad = Robust::MedianAbsoluteDeviation(&allmp[0],
						  allmp.size(),allMedian);
         if ((!numeric)&& (verbose))
         {
	   cout << "Median Absolute Deviation (MAD) for all retained points is " << allMad << " meters." << endl;
	 }
	 
         double mMAD = 5.0;
         removePts = removePts || (oa.observation > allMedian+mMAD*allMad)
	   || (oa.observation < allMedian-mMAD*allMad);

         oa.edit(removePts);

         allpasses = unique(oa.pass);
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

                                 // Removes mean of each individual pass
         for (set<long>::iterator iPass=allpasses.begin() ; iPass!=allpasses.end() ; iPass++)
         {
            valarray<bool> passMask = (oa.pass==*iPass);
            valarray<double> mpVals = oa.observation[passMask];
            valarray<double> binVals(mpVals.size());
            double mean = mpVals.sum() / mpVals.size();
            mpVals -= mean;
            oa.observation[passMask]=mpVals;
         }

         // Use the Dual Frequency Method to remove biases between passes.
         if (dualfrequencymethod)
         {                       //  deginning dfm
            removeBiases(oa,verbose);
         }

         // Use the mean of low elevation data as zero.
         valarray<bool> lowObsMask = (oa.elevation <= upperZeroMeanElevation) && (oa.elevation >= 0.);
         valarray<double> mpVals = oa.observation[lowObsMask];
         double lowObsMean = mpVals.sum() / mpVals.size();
         oa.observation -= lowObsMean;

         allpasses = unique(oa.pass);
         if (!numeric)
         {
            if (verbose)
            {
               cout << "Data collection interval is " << setprecision(3)
                  << oa.interval << " seconds";
               if (oa.intervalInferred)
               {
                  cout << ", inferred from data";
               }
               else
               {
                  cout << ", read from file headers";
               }
               cout << "." <<endl;
            }
            cout << "Overhead passes used: ";
            cout << allpasses.size() << endl;
         }

         if (rawOption.getCount()>0)
         {
            dumpRaw(cout, oa, numeric);
         }
         else if (plotOption.getCount()>0)
         {
            using namespace StringUtils;
            static string temp=plotOption.getValue()[0];
            static int numAzimBin = static_cast<int>(asDouble(word(temp,0,',')));
            static int numElevBin = static_cast<int>(asDouble(word(temp,1,',')));

            static double minAz=0, maxAz=360, minEl = 0, maxEl = 90;

            static DenseBinnedStats<double> mstats(numAzimBin, minAz, maxAz,
               numElevBin, minEl, maxEl);

            mstats.addData(oa.observation, oa.azimuth, oa.elevation);

            if (fileCounter==obsList.size())
               plotAzElSurf(mstats,
                  minAz, maxAz, numAzimBin,
                  minEl, maxEl, numElevBin,
                  (viewOption.getCount()>0));

         }
         else
         {
            bool byPlot = (plotOption.getCount()>0);
            bool byAzimuth = (azimuthOption.getCount()>0);
            if (angWidthOption.getCount()>0)
            {
               angInterval =  StringUtils::asDouble(angWidthOption.getValue()[0]);
            }
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
                     {
                        sbs.addBin(d,d+angInterval);
                     }
                  }
               }
               else
               {
                  for (double d=0; d<359; d+=angInterval)
                  {
                     sbs.addBin(d,d+angInterval);
                  }
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

            for (set<long>::iterator iPass=allpasses.begin() ; iPass!=allpasses.end() ; iPass++)
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
            valarray<double> stDevStats(sbs.stats.size());
            valarray<double> meanStats(sbs.stats.size());
            writeStats(cout, sbs, stDevStats, meanStats, numeric, !byAzimuth, byPlot);
         }

      }                          // end while processing files

      if ( (verbose) && (!numeric))
      {
         DayTime then;
         cout << "Processing complete in " << then - now << " seconds." << endl;
      }

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
         if (oa.validAzEl[i])
         {
            ostr << setprecision(4) << oa.epoch[i].GPSfullweek() << " ";
            ostr << setprecision(9) << oa.epoch[i].GPSsow() << " ";
            ostr << oa.satellite[i].id << " ";
            ostr << (int) oa.satellite[i].system << " ";
            ostr << oa.pass[i] << " ";
            ostr << setprecision(12) << oa.observation[i] << " ";
            ostr << (int) oa.lli[i];
            ostr << setprecision(5);
            ostr << " " << oa.azimuth[i];
            ostr << " " << oa.elevation[i];
            ostr << std::endl;
         }
      }
   }
}


void writeStats(std::ostream& ostr, const SparseBinnedStats<double>& mstats, valarray<double>& stDevStats, valarray<double>& meanStats,
bool numeric, bool elevation, bool plot)
{
   // Beginning of code I added
   if (plot)
   {
      std::string angDesc = "azimuth and elevation";
      // Worry about output code later.
      for (int i=0; i<mstats.stats.size(); i++)
      {
         stDevStats[i] = mstats.stats[i].StdDev();
         meanStats[i] = mstats.stats[i].Average();
      }
   }
   else
   {
      //End of code I added (do not forget to remove a bracket below if this code is removed)

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
   }                             // This is the bracket that is associated with the code I added in writeStats
}


int findIntersection(valarray<double>& elevLow, valarray<double>& azimLow, valarray<double>& obsLow, valarray<double>& elevHigh, valarray<double>& azimHigh, valarray<double>& obsHigh, long& idx_i, long& idx_j, double& eint, double& aint, long& intindex)
{
   // Segmentation fault will occur if findIntersection attempts to find an intersection with a pass of size equal to one since a minimum of two points are required to find an intersection
   if (elevLow.size()<=1 || elevHigh.size()<=1)
   {
      return(0);
   }

   //Variable initialization
   valarray<double> e1slice(2),
      a1slice(2),
      o1slice(2),
      e2slice(2),
      a2slice(2),
      o2slice(2),
      ratio(2);
   double inv[2][2];
   long startLow=0,
      startHigh=0;

                                 // This loop and the one below allow each elevation and azimuth angle for passLow to be compared to each elevation and azimuth angle for passHigh
   while (startLow<(elevLow.size()-1))
   {
      e1slice = elevLow[slice(startLow,2,1)];
      a1slice = azimLow[slice(startLow,2,1)];
      o1slice = obsLow[slice(startLow,2,1)];

      if ((e1slice[0]-e1slice[1])>80)
      {
         e1slice[1] = e1slice[1] + 90;
      }

      else if ((e1slice[0]-e1slice[1])<-80)
      {
         e1slice[0] = e1slice[0] + 90;
      }

      else if ((a1slice[0]-a1slice[1])>330)
      {
         a1slice[1] = a1slice[1] + 360;
      }

      else if ((a1slice[0]-a1slice[1])<-330)
      {
         a1slice[0] = a1slice[0] + 360;
      }

      startHigh = 0;

                                 // This loop and the one below allow each elevation and azimuth angle for passLow to be compared to each elevation and azimuth angle for passHigh
      while (startHigh<(elevHigh.size()-1))
      {
         e2slice = elevHigh[slice(startHigh,2,1)];
         a2slice = azimHigh[slice(startHigh,2,1)];
         o2slice = obsHigh[slice(startHigh,2,1)];

         if ((e2slice[0]-e2slice[1])>80)
         {
            e2slice[1]=e2slice[1]+90;
         }

         else if ((e2slice[0]-e2slice[1])<-80)
         {
            e2slice[0]=e2slice[0]+90;
         }

         else if ((a2slice[0]-a2slice[1])>330)
         {
            a2slice[1]=a2slice[1]+360;
         }

         else if ((a2slice[0]-a2slice[1])<-330)
         {
            a2slice[0]=a2slice[0]+360;
         }

                                 // Ensures division by zero does not occur (although extremely unlikely)
         if ( (e1slice[0]-e1slice[1])*(a2slice[1]-a2slice[0])-(a1slice[0]-a1slice[1])*(e2slice[1]-e2slice[0]) != 0 )
         {
            // Calculate inverse of two-by-two matrix and ratios.
            inv[0][0] = 1/((e1slice[0]-e1slice[1])*(a2slice[1]-a2slice[0])-(a1slice[0]-a1slice[1])*(e2slice[1]-e2slice[0]))*(a2slice[1]-a2slice[0]);
            inv[0][1] = 1/((e1slice[0]-e1slice[1])*(a2slice[1]-a2slice[0])-(a1slice[0]-a1slice[1])*(e2slice[1]-e2slice[0]))*(-(e2slice[1]-e2slice[0]));
            inv[1][0] = 1/((e1slice[0]-e1slice[1])*(a2slice[1]-a2slice[0])-(a1slice[0]-a1slice[1])*(e2slice[1]-e2slice[0]))*(-(a1slice[0]-a1slice[1]));
            inv[1][1] = 1/((e1slice[0]-e1slice[1])*(a2slice[1]-a2slice[0])-(a1slice[0]-a1slice[1])*(e2slice[1]-e2slice[0]))*(e1slice[0]-e1slice[1]);
            ratio[0] = inv[0][0]*(e1slice[0]-e2slice[0])+inv[0][1]*(a1slice[0]-a2slice[0]);
            ratio[1] = inv[1][0]*(e1slice[0]-e2slice[0])+inv[1][1]*(a1slice[0]-a2slice[0]);

            if (ratio[0]>=0 && ratio[0]<=1 && ratio[1]>=0 && ratio[1]<=1)
            {
               eint = e1slice[0] + ratio[0] * ( e1slice[1] - e1slice[0] );
               aint = a1slice[0] + ratio[0] * ( a1slice[1] - a1slice[0] );
               idx_i=startLow;
               idx_j=startHigh;
               return(1);        // If intersection is found, return the integer 1
            }
         }

         startHigh = startHigh + 1;
      }                          // end of while (startHigh<(elevHigh.size()-1))

      startLow = startLow + 1;
   }                             // end of while (startLow<(elevLow.size()-1))
   return(0);
}                                // end of findIntersection


void plotAzElSurf(const DenseBinnedStats<double>& mstats,
double minAz, double maxAz, int azBinSize,
double minEl, double maxEl, int elBinSize,
bool launchViewer)
{

                                 // Output file
   SVGImage vgwindow( "multipathSurfacePlot.svg",
      8.5*2*PTS_PER_INCH,        // Upper right x
      4.5*2*PTS_PER_INCH );      // Upper right y

   // Use frames to set up the place to put the legend and surface plot
   Frame f(vgwindow);
   // First make a border
   BorderLayout bl(f,0.5*PTS_PER_INCH);
   Frame cf = bl.getFrame(0);
   VLayout titleBodySplitter( cf, 0.95 );
   Frame ff=titleBodySplitter.getFrame(0);
   Frame titleFrame=titleBodySplitter.getFrame(1);
   HLayout vl( ff, 0.95 );
   Frame plotf = vl.getFrame(0), keyf = vl.getFrame(1);

   TextStyle hdrStyle( 0.3*PTS_PER_INCH,
      TextStyle::BOLD,
      Color::BLACK,
      TextStyle::SANSSERIF );

                                 // Text
   titleFrame << Text( "Multipath Surface Plot",
      titleFrame.cx(),           // x coordinate of center of frame
      titleFrame.cy(),           // y coordinate of center of frame
      hdrStyle,
      Text::CENTER );

   Palette p( Color::GREY, -2, 2);
   p.setColor( 0.00, Color(Color::WHITE)   );
   p.setColor( 0.01, Color(Color::RED)   );
   p.setColor( 0.25, Color(Color::YELLOW));
   p.setColor( 0.50 , Color(Color::WHITE) );
   p.setColor( 0.75, Color(Color::CYAN)  );
   p.setColor( 1.00, Color(Color::BLUE)  );

   SurfacePlot sp( azBinSize, elBinSize, p);

   sp.setColorLabel( "Mean" );   // Label for color axis
   sp.setXAxis( 0.0, 360.0 );    // Minimum and maximum for x axis
   //sp.setXLabel( "This is a label" );
   sp.setYAxis( 0.0, 90.0 );     // Minimum and maximum for y axis

   for (size_t i = 0; i<azBinSize; i++)
      for (size_t j = 0; j < elBinSize; j++)
   {
      //      if (mstats.stats[i][j].N()>0)
      sp.set( j, i, mstats.stats[i][j].Average() );
   }

   sp.drawPlot(&plotf);
   sp.drawKey(&keyf);

   if (launchViewer)
      vgwindow.view();
}


void removeBiases(ObsArray& oa, bool verbose)
{
   size_t editedLength = oa.getNumSatEpochs();
   // Variable initialization
   long idx_i, idx_j, isize, jsize;
   double eint, aint;

   int stride = 4;
   long intindex = 0;

                                 // Creates a unique list of passes
   set<long> passList = unique(oa.pass);

                                 // Creates an iterator to step through the list of passes
   set<long>::iterator i_itr = passList.begin();

                                 // Creates a vector with length equal to the number of passes
   valarray<int> boolean(passList.size());

   for (size_t i=0 ; i<passList.size() ; i++)
   {
      boolean[i]=0;              // Sets values in the vector boolean to 0, which indicates an intersection hasn't been found for the particular passes (we haven't searched for intersections yet, so we should assume they do not occur!)
   }

   // The size of H and y is equal to the maximum number of intersections possible for the number of passes
   Matrix<int> H((passList.size()-1)*(passList.size())/2,passList.size());

   Vector<double> y((passList.size()-1)*(passList.size())/2);

   // Sets values of H equal to 0
   for (size_t m=0 ; m<((passList.size()-1)*(passList.size())/2) ; m++)
   {
                                 //
      for (size_t n=0 ; n<passList.size() ; n++)
      {
         H[m][n]=0;
      }
   }

                                 // Sets lower pass
   for (size_t i=0 ; i<(passList.size()-1) ; i++, i_itr++)
   {
                                 // Creates an index of relevant values for current lower pass in regards to the vectors found in ObsArray
      valarray<bool> i_idx = (oa.pass == *i_itr);

      // Creates vectors of data for current lower pass
      valarray<double> iel=oa.elevation[i_idx],
         iaz=oa.azimuth[i_idx],
         iobs=oa.observation[i_idx];

                                 // Sets the iterator for the higher pass equal to the iterator for the lower pass
      set<long>::iterator j_itr = i_itr;
      j_itr++;                   // Adds one to the iterator for the higher pass

      isize=(iel.size()/stride); // Based on the stride, tells us how many values to expect in our slice

                                 // 0 used to be start
      valarray<double> ielsub=iel[slice(0,isize,stride)],
                                 // 0 used to be start
         iazsub=iaz[slice(0,isize,stride)],
                                 // 0 used to be start
         iobssub=iobs[slice(0,isize,stride)];

                                 // Sets higher pass
      for (size_t j=i+1; j<passList.size(); j++, j_itr++ )
      {
                                 // Creates index of relevant values for current higher pass in regards to the vectors found in ObsArray
         valarray<bool>   j_idx = (oa.pass == *j_itr);

         // Creates vectors of data for current higher pass
         valarray<double> jel=oa.elevation[j_idx],
            jaz=oa.azimuth[j_idx],
            jobs=oa.observation[j_idx];

         jsize=(jel.size()/stride);

                                 // 0 used to be start
         valarray<double> jelsub=jel[slice(0,jsize,stride)],
                                 // 0 used to be start
            jazsub=jaz[slice(0,jsize,stride)],
                                 // 0 used to be start
            jobssub=jobs[slice(0,jsize,stride)];

                                 // If an intersection is found, enter
         if (findIntersection(ielsub, iazsub, iobssub, jelsub, jazsub, jobssub, idx_i, idx_j, eint, aint, intindex)==1)
         {
                                 // Reassign the elevation angles for the current lower pass
            valarray<double> ielsub=iel[slice(idx_i*stride,stride+1,1)],
                                 // Reassign the azimuth angles for the current lower pass
               iazsub=iaz[slice(idx_i*stride,stride+1,1)],
                                 // Reassign the observations for the current lower pass
               iobssub=iobs[slice(idx_i*stride,stride+1,1)],
                                 // Reassign the elevation angles for the current higher pass
               jelsub=jel[slice(idx_j*stride,stride+1,1)],
                                 // Reassign the azimuth angles for the current higher pass
               jazsub=jaz[slice(idx_j*stride,stride+1,1)],
                                 // Reassign the observations for the current higher pass
               jobssub=jobs[slice(idx_j*stride,stride+1,1)];
                                 // If an intersection is found again, enter
            if (findIntersection(ielsub, iazsub, iobssub, jelsub, jazsub, jobssub, idx_i, idx_j, eint, aint, intindex)==1)
            {
               if (abs(iobssub[idx_i]-jobssub[idx_j])<5)
               {
                  boolean[i]=1;
                  boolean[j]=1;
                  H[intindex][i]=1;
                  H[intindex][j]=-1;
                  y[intindex]=iobssub[idx_i]-jobssub[idx_j];
                  intindex++;
               }
            }                    // found again
         }                       // found first time
      }                          // inner search
   }                             // outer search

   if (intindex==0) // If no intersections were found, the function must be exited. Otherwise, a segmentation fault will occur.
   {
      if (verbose)
      {
         cout << "This particular file contained no pass intersections; therefore, the DFM was unable to be performed." << endl;
      }
      return;
   }

   Matrix<int> Hnew(intindex,passList.size());
   Vector<double> ynew(intindex);

   for (size_t m=0 ; m<intindex ; m++)
   {
      ynew[m]=y[m];
      for (size_t n=0 ; n<passList.size() ; n++)
      {
         Hnew[m][n]=H[m][n];
      }
   }

   Matrix<int> Hnewnew(intindex,boolean.sum());

   for (size_t n=0 ; n<passList.size() ; n++)
   {
      if (H[1][n]==1)
      {
         boolean[n]=0;
      }
   }

   long colind = 0;

   for (size_t n=0 ; n<boolean.size() ; n++)
   {
      if (boolean[n]==1)
      {
         for (size_t m=0 ; m<intindex ; m++)
         {
            Hnewnew[m][colind]=Hnew[m][n];
         }
         colind++;
      }
   }

   Matrix<double> Hnewnewnew(intindex,colind);

   for (size_t m=0 ; m<intindex ; m++)
   {
      for (size_t n=0 ; n<colind ; n++)
      {
         Hnewnewnew[m][n]=Hnewnew[m][n];
      }
   }

   SVD<double> cow;
   cow(Hnewnewnew);
   cow.backSub(ynew);

   Vector<double> xhat(intindex);
   xhat=ynew.resize(colind);

   //cout << xhat << endl;

                                 // Creates an iterator to step through the list of passes
   set<long>::iterator k_itr=passList.begin();

   int xhat_itr=0;

   // The following two 'for' loops are used to step through the passes.
                                 // Sets lower pass
   for (size_t k=0 ; k<(passList.size()) ; k++, k_itr++)
   {
                                 // Creates an index of relevant values for current lower pass in regards to the vectors found in ObsArray
      valarray<bool> k_idx = (oa.pass == *k_itr);

      // Creates vectors of data for current lower pass
      valarray<double> kel=oa.elevation[k_idx],
         kaz=oa.azimuth[k_idx],
         kobs=oa.observation[k_idx];

      Vector<double> kelvec(kel.size()),
         kazvec(kaz.size()),
         kobsvec(kobs.size());

      kelvec=kel;
      kazvec=kaz;
      kobsvec=kobs;

      if (boolean[k]==1)
      {
         for (size_t m=0 ; m<editedLength ; m++)
         {
            if (oa.pass[m]==*k_itr && abs(xhat[xhat_itr])<10.0)
            {
               oa.observation[m] = oa.observation[m] + xhat[xhat_itr];
            }
         }
         xhat_itr=xhat_itr+1;
      }
   }
}
