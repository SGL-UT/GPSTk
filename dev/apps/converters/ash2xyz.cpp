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

#include <string>
#include <list>
#include <map>
#include <vector>

#include "DayTime.hpp"
#include "GPSWeekSecond.hpp"
#include "TimeConstants.hpp"
#include "Exception.hpp"
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"
#include "GPSEphemerisStore.hpp"
#include "icd_200_constants.hpp"
#include "InOutFramework.hpp"
#include "RinexConverters.hpp"
#include "ObsUtils.hpp"
#include "StringUtils.hpp"
#include "AshtechStream.hpp"
#include "AshtechMBEN.hpp"
#include "AshtechPBEN.hpp"
#include "AshtechALB.hpp"
#include "AshtechEPB.hpp"

using namespace std;
using namespace gpstk;


class Ashtech2XYZ : public gpstk::BasicFramework
{
public:
   Ashtech2XYZ(const string& applName) throw()
      : BasicFramework(applName,"Processes Ashtech Z(Y)-12 observation and "
         "ephemeris data and outputs satellite positions and ionospheric "
         "corrections."),
        inputOpt('i',"input", "Where to get the data from. The default is to "
         "use stdin."),
        outputOpt('o', "output", "Where to send the output. The default "
         "is stdout."),
        weekOpt('w',"week","The full GPS week in which this data starts. Use"
         " this option when the start time of the data being processed is not "
         "during this week."),
        offsetOpt('s',"offset","Output SV positions at a time offset from "
         "the current time. Give a positive or negative integer of seconds."),
        codeOpt('c',"code","Restriction for source of obs data. If no "
         "restriction is given, only observation data collected via L1/L2 Y "
         "code tracking will be used. Options are \"Y\", \"P\", and "
         "\"codeless\"."),
        numPointsOpt('n',"num_points","Width of the exponential filter moving "
         "window, in number of points. Default is 36.")
   {   
	  inputOpt.setMaxCount(1);
	  outputOpt.setMaxCount(1);
	  weekOpt.setMaxCount(1);
	  offsetOpt.setMaxCount(1);
	}

   bool initialize(int argc, char *argv[]) throw()
   {
      using std::basic_ios;

      if (!BasicFramework::initialize(argc,argv)) return false;

      if (debugLevel || verboseLevel)
         cout << "debugLevel: " << debugLevel << endl
              << "verboseLevel: " << verboseLevel << endl;

		// input option
      string fn;
      if (inputOpt.getCount())
         fn = inputOpt.getValue()[0];
      if (fn == "")
      {
         fn = "<stdin>";
         input.basic_ios<char>::rdbuf(cin.rdbuf());
      }
      else
      {
         inputDev.open(fn.c_str(), ios::in);
         input.basic_ios<char>::rdbuf(inputDev.basic_ios<char>::rdbuf());	  
      }
      if (debugLevel || verboseLevel>2)
         cout << "Taking input from " << fn << endl;
         
      // output option
      if (outputOpt.getCount())
      {
         output.open(outputOpt.getValue()[0].c_str(), std::ios::out);
         if (debugLevel || verboseLevel>2)
            cout << "Sending output to" 
                 << outputOpt.getValue()[0]
                 << endl;
      }
      else
      {
         if (debugLevel || verboseLevel>2 )
            cout << "Sending output to stdout" << endl;
         output.copyfmt(std::cout);
         output.clear(std::cout.rdstate());
         output.basic_ios<char>::rdbuf(std::cout.rdbuf());
      }      
      
      // week option (otherwise assume data is from this week)
      DayTime now;
      time.week = now.GPSfullweek();
      time.sow = now.GPSsecond();
      if (weekOpt.getCount())
      {
         time.week = StringUtils::asInt(weekOpt.getValue()[0]);
         time.sow = HALFWEEK;
      } 
      if (debugLevel || verboseLevel)
      	cout << "First guess for time is [" 
      	     << time.printf("%F, %.1g") 
      	     << "] (week, sow)\n";

		// time offset option
		offsetSec = offsetOpt.getCount() ? 
		            StringUtils::asInt(offsetOpt.getValue()[0]) : 0;
  
      if (debugLevel || verboseLevel)
      	cout << "Time offset is " << offsetSec << " sec.\n";

		// set code restriction
		if (codeOpt.getCount())
		{
			if (codeOpt.getValue()[0].c_str() == string("Y"))
				rangeCode = rcYcode;
			else if (codeOpt.getValue()[0].c_str() == string("P"))
				rangeCode = rcPcode;
			else if (codeOpt.getValue()[0].c_str() == string("codeless"))	
				rangeCode = rcCodeless;
			else
			{
				cout << "\nInvalid tracking code specification. Enter Y, P, or \n"
				     << "codeless. (or don't enter anything - the default is Y.) "
				     << "\n\nExiting...\n\n";
				return false;
			}
		}
		else
			rangeCode = rcYcode; // default
			
		if (debugLevel || verboseLevel)
			cout << "Only processing observation data collected via "
			     << StringUtils::asString(rangeCode) << " code tracking\n";

		// filter window width
		numPoints = numPointsOpt.getCount() ? 
		            StringUtils::asInt(numPointsOpt.getValue()[0]) : 36;
		if (debugLevel || verboseLevel)
			cout << "Filter window width is " << numPoints << " points.\n";
		            
      // set debug levels      
		AshtechData::debugLevel = debugLevel;
		if (debugLevel>2)
			AshtechData::hexDump = true;
		if (debugLevel>4)
			MDPHeader::hexDump = true;
      
		// initialize firstEph (1st ephemeris) to true
		firstEph = true;

		// successful exit
		return true;
   }
	   
protected:
   virtual void spinUp()
   {}

   virtual void process()
   {
      AshtechData hdr;
      AshtechPBEN pben;
      AshtechMBEN mben;
      AshtechEPB  epb;
      AshtechALB  alb;

		// maps of the recent dual frequency observations for each SV
		map<gpstk::SatID, TimePhaseVec> phaseMap;
		map<gpstk::SatID, RangePairVec> rangeMap;
		
		// these vectors will store the most recently computed iono error and 
		// error rate for each SV. They are initialized to 0, so until we have
		// enough points to run the filter, 0.000 will be output
		vector<double> currentErrors(gpstk::MAX_PRN, 0);
      vector<double> currentRates(gpstk::MAX_PRN, 0);
      
      unsigned short fc=0;
      vector<MDPObsEpoch> hint(33);
      bool firstPBEN = false;
      
      while (input >> hdr)
      {
         if (debugLevel>1)
            cout << "---" << endl;

         if (pben.checkId(hdr.id) && (input >> pben) && pben)
         {
            if (debugLevel>3)
               pben.dump(cout);
            
            double dt = pben.sow - time.sow;
            time.sow = pben.sow;
            firstPBEN = true;

            if (std::abs(dt) > HALFWEEK && !firstPBEN)
               time.week++;
         }
         else if (mben.checkId(hdr.id) && (input >> mben) && mben)
         {
            if (debugLevel>2)
            {
               cout << "---\n";
               mben.dump(cout);
            }
            
				// if we don't have a PBEN message, we can't resolve time
            if (firstPBEN != true)
            	continue;
            	
            // use the time set in the PBEN as a hint, resolve time exactly
            DayTime hintTime = DayTime(time.week, time.sow);
            DayTime tempTime = hintTime;
   	   	double  sow1     = tempTime.GPSsecond();
	      	int     sow2     = static_cast<int>(sow1/1800);
   	   	double  sow3     = static_cast<double>(sow2 * 1800);
      		double  sow_mben = 0.05 * mben.seq;
		      double  sow4     = sow3 + sow_mben;
      
		      if (sow4 < sow1) // Assume that time only moves forward
 		        sow4 += 1800;
 		     	
 		     	// this is the time for this epoch
 		     	tempTime.setGPS(tempTime.GPSfullweek(), sow4);

				// this is the satellite ID for this PRN
				SatID satID(mben.svprn,SatID::systemGPS);
				
				// get the phase values	
			   double phaseL1 = 0; // phase on L1 (cycles)
			   double phaseL2 = 0; // phase on L2 (cycles)
			   
				if (mben.p1.full_phase)
					phaseL1 = mben.p1.full_phase;
				else if (debugLevel > 3)
					cout << "No L1 phase value for PRN " << mben.svprn
					     << " at time " << tempTime << endl;
				
				if (mben.p2.full_phase)
					phaseL2 = mben.p2.full_phase;
				else if (debugLevel > 3)
					cout << "No L2 phase value for PRN " << mben.svprn
					     << " at time " << tempTime << endl;
				
				// if missing data, go to next epoch
				if (!(phaseL1 && phaseL2))
					continue;
					
				// get the pseudorange values
				double prL1 = 0;    // pseudorange on L1 (m)
			   double prL2 = 0;    // pseudorange on L2 (m)

				// note that raw_range is in seconds
				if (mben.p1.raw_range)
					prL1 = mben.p1.raw_range * C_GPS_M;
				else if (debugLevel > 3)
					cout << "No L1 range value for PRN " << mben.svprn
					     << " at time " << tempTime << endl;
				
				if (mben.p2.raw_range)
					prL2 = mben.p2.raw_range * C_GPS_M;
				else if (debugLevel > 3)
					cout << "No L2 range value for PRN " << mben.svprn
					     << " at time " << tempTime << endl;
				
				// if missing data, or if values are unreasonable, go to next epoch
				if (!(prL1 && prL2))
					continue;
				else if (prL1 < 1e6 || prL2 < 1e6)	// sanity check on range
				{
					if (debugLevel > 1 )
						cout << "Bad pseudorange value: " << satID << "\t" 
						     << tempTime << "\tL1: " << fixed << prL1 << "\tL2: "
						     << prL2 << endl;
					continue;
				}
				
				// if we have enough phase points, run filter for phase rate
				map<gpstk::SatID, TimePhaseVec>::iterator iter1;
				iter1 = phaseMap.find(satID);
				if ((*iter1).second.size() == (numPoints+1))
				{
					// initial filter result will be the initial iono rate value
					TimePhaseVec tpVec = (*iter1).second;
					DayTime t0 = tpVec[0].first;
					DayTime t1 = tpVec[1].first;
					// FIX should diff the MJDs, then convert to seconds
					double deltaT = t1.GPSsow() - t0.GPSsow();
					double yPrevL1 = (tpVec[1].second.first-tpVec[0].second.first)/
											deltaT;  // cycles/sec
					double yPrevL2 = (tpVec[1].second.second-tpVec[0].second.second)/
											deltaT;  // cycles/sec							
					double yCurrL1;
					double yCurrL2;
					double a = 1.000/(numPoints + 1);
					for (int index = 2; index < tpVec.size(); index++)
					{
						double xCurrL1 = tpVec[index].second.first;    // L1, cycles
						double xCurrL2 = tpVec[index].second.second;   // L2, cycles
						double xPrevL1 = tpVec[index-1].second.first;  // L1, cycles
						double xPrevL2 = tpVec[index-1].second.second; // L2, cycles					
						double deltaT  = tpVec[index].first.GPSsow() - 
						                 tpVec[index-1].first.GPSsow(); // seconds
						yCurrL1 = a*((xCurrL1-xPrevL1)/deltaT)+(1-a)*yPrevL1;
						yCurrL2 = a*((xCurrL2-xPrevL2)/deltaT)+(1-a)*yPrevL2; //cycl/s
					
						yPrevL1 = yCurrL1;
						yPrevL2 = yCurrL2;
					}
					
					// compute iono error with filter results
            	double x1 = yCurrL1 * gpstk::L1_WAVELENGTH; // m/s
            	double x2 = yCurrL2 * gpstk::L2_WAVELENGTH; // m/s
                        
            	double ionoErrorRate = (x1 - x2)/
                                      ((gpstk::L1_FREQ/gpstk::L2_FREQ)*
                                       (gpstk::L1_FREQ/gpstk::L2_FREQ) - 1);
					ionoErrorRate *= 1000;	// mm/sec					
					
					// store value
					currentRates[satID.id - 1] = ionoErrorRate;
					
					// remove the oldest measurement, which would be at index 0
					vector<TimePhasePair> prevVector = (*iter1).second;
					(*iter1).second.clear();
					
					for (int index = 0; index < (numPoints - 1); index++)
						(*iter1).second.push_back(prevVector[index + 1]);		
				}
							
				// check current phase values and store
				PhasePair phasePair(phaseL1, phaseL2);	
				TimePhasePair timePhasePair(tempTime, phasePair);
				if (phaseMap.find(satID) == phaseMap.end())
				{
					// this is the first element for this SV
					vector<TimePhasePair> tempVector;
					tempVector.push_back(timePhasePair);
					phaseMap[satID] = tempVector;
				}
				else
				{
					map<gpstk::SatID, TimePhaseVec>::iterator iter;
					iter = phaseMap.find(satID);
					
					// use the previous phase values to computer iono error rate
					int lastIndex = (*iter).second.size() - 1;				
					TimePhasePair lastTPPair = (*iter).second[lastIndex];
					DayTime lastTime = lastTPPair.first;
					PhasePair lastPPair = lastTPPair.second;
					double dL1 = phaseL1 - lastPPair.first;             // cycles
					double dL2 = phaseL2 - lastPPair.second;            // cycles
					double dt  = tempTime.GPSsow() - lastTime.GPSsow(); // sec
					
					double x1 = (dL1/dt) * gpstk::L1_WAVELENGTH;        // m/s
            	double x2 = (dL2/dt) * gpstk::L2_WAVELENGTH;        // m/s
            	
            	double ionoErrorRate = (x1 - x2)/
                                      ((gpstk::L1_FREQ/gpstk::L2_FREQ)*
                                       (gpstk::L1_FREQ/gpstk::L2_FREQ) - 1);
					ionoErrorRate *= 1000;	                            // mm/sec
					
					// if phase values produce a resonable iono rate, store
					if (abs(ionoErrorRate) < 10)
						(*iter).second.push_back(timePhasePair);
					else
					{
						(*iter).second.clear();
						
						if (debugLevel)
							cout << "Rate(mm/s) = " << ionoErrorRate 
							     << "\tRejecting phase values(L1,L2): " 
							     << fixed << phaseL1 << "\t" << phaseL2 
					           << "\t" << satID << "\t" << tempTime << endl;
					}
				}

				// if we have enough range points, run filter for iono error
				map<gpstk::SatID, RangePairVec>::iterator iter2;
				iter2 = rangeMap.find(satID);
				if ((*iter2).second.size() == numPoints)
				{
					// initial filter result will be the actual value
					RangePairVec rpVec = (*iter2).second;
					double yPrev = (rpVec[0].first - rpVec[0].second)/
                               ((gpstk::L1_FREQ/gpstk::L2_FREQ)*
                                (gpstk::L1_FREQ/gpstk::L2_FREQ) - 1); // m					
					double yCurr;
					double a = 1.000/(numPoints + 1);
					for (int index = 1; index < rpVec.size(); index++)
					{
						double xCurr;
						xCurr = (rpVec[index].first - rpVec[index].second)/
                           ((gpstk::L1_FREQ/gpstk::L2_FREQ)*
                            (gpstk::L1_FREQ/gpstk::L2_FREQ) - 1); // m							
						yCurr = a*xCurr + (1-a)*yPrev;
						yPrev = yCurr;
					}

					// filter result is iono error for this epoch for this SV
					double ionoError  = yCurr; // m

					// store error
					currentErrors[satID.id - 1] = ionoError; // m
				
					// remove oldest measurement, which would be at index 0....
					vector<RangePair> prevVector = (*iter2).second;
					(*iter2).second.clear();
					// .... so keep values after index 1
					for (int index = 1; index < numPoints; index++)
						(*iter2).second.push_back(prevVector[index]);
		
				}
				
				// check range values for this epoch
				double ionoError = (prL1 - prL2)/
                               ((gpstk::L1_FREQ/gpstk::L2_FREQ)*
    		                       (gpstk::L1_FREQ/gpstk::L2_FREQ) - 1); // m
    		   if ( (ionoError>-15) && (ionoError<0) )
    		   {
    		   	RangePair rangePair(prL1, prL2);
					if (rangeMap.find(satID) == rangeMap.end())
					{
						vector<RangePair> tempVector;
						tempVector.push_back(rangePair);
						rangeMap[satID] = tempVector;
					}
					else
					{
						map<gpstk::SatID, RangePairVec>::iterator iter;
						iter = rangeMap.find(satID);
						(*iter).second.push_back(rangePair);
					}
				}
				else if (debugLevel)
					cout << "Error(m) = " << ionoError << "\tRejecting pseudorange "
					     << "values(L1,L2): " << fixed << prL1 << "\t" << prL2 
					     << "\t" << satID << "\t" << tempTime << endl;

				// if we have gotten eph data for this SV and time, we can 
				// find the position. If so, then output results for this epoch
				DayTime xvtTime = tempTime + offsetSec;
				try
				{	
					// get poisition of SV, possibly at offset time
            	DayTime xvtTime = tempTime + offsetSec;
               Xvt xvt = gpsEphStore.getXvt(satID,xvtTime);
 					
					// ouput data
					output << xvtTime.printf("%4Y/%03j/%02H:%02M:%04.1f")
               		<< fixed << right
                  	<< ", " << setw(8) << offsetSec
                 		<< ", " << setw(2) << (int)mben.svprn
                   	<< ", " << setprecision(4) << setw(10) 
                   	<< currentErrors[satID.id - 1]
                   	<< ", " << setprecision(4) << setw(10)  
                   	<< currentRates[satID.id - 1]
                   	<< ", " << setprecision(2) << setw(12) << xvt.x[0]
                   	<< ", " << setprecision(2) << setw(12) << xvt.x[1]
                   	<< ", " << setprecision(2) << setw(12) << xvt.x[2];
            	if (debugLevel)
            		output << fixed << right
            		       << ", " << setprecision(2) << setw(12) << prL1
            		       << ", " << setprecision(2) << setw(12) << prL2
            		       << ", " << setprecision(2) << setw(12) << phaseL1
            		       << ", " << setprecision(2) << setw(12) << phaseL2;
            	output << endl;
           	}
           	catch(...)
            {
            	if (debugLevel > 1)
               	cout << "---\nCould not output data for " 
                       << satID << " at " << xvtTime << endl;
				}          
		 	}       // else if (mben.checkId(hdr.id) && (input >> mben) && mben)
         else if (epb.checkId(hdr.id) && (input >> epb) && epb)
			{
				// using functionality from MDP classes
            if (debugLevel > 2)
               epb.dump(cout);
            	
            MDPNavSubframe nav;
            nav.carrier = ccL1;
            nav.range = rcCA;
            nav.nav = ncICD_200_2;
            nav.prn = epb.prn;

            for (int s=1; s<=3; s++)
				{
            	for (int w=1; w<=10; w++)
                  nav.subframe[w] = epb.word[s][w];
               long sow = nav.getHOWTime();
               if (sow>FULLWEEK || sow<0)
                  continue;
               
               DayTime t = DayTime(time.week, nav.getHOWTime()) - 6;
               nav.freshnessCount = fc++;
               nav.time = t;
                				
					if (firstEph && debugLevel)
						cout << "---\nGot first nav SF" << endl;
				
					MDPNavSubframe tmp = nav;		
				
					// First try the data assuming it is already upright
      			tmp.cooked = true;
      			bool parityGood = tmp.checkParity();
      			if (!parityGood)
      			{
         			if (debugLevel && firstEph)
            			cout << "Raw subframe (not cooked)\n";
         			nav.cooked = false;
         			nav.cookSubframe();
         			parityGood = nav.checkParity();
      			}
      			else if (debugLevel && firstEph)
            		cout << "Cooked subframe\n";

      			firstEph=false;

      			if (!parityGood)
      			{
         			if (debugLevel)
            			cout << "Parity error\n";
         			return;
      			}

      			short sfid = nav.getSFID();
      			if (sfid > 3)
      			{
						cout << "Bad SF ID, sfid > 3\n";
         			return;
					}
      			short week = nav.time.GPSfullweek();

	      		sow = nav.getHOWTime();
   	   		if (sow > DayTime::FULLWEEK)
      			{
        		 		if (debugLevel)
            			cout << "Bad week, sow > DayTime::FULLWEEK\n";
         			return;
      			}

      			if (debugLevel>2)
         			nav.dump(cout);
         		
      			DayTime howTime(week, sow);
      			NavIndex ni(RangeCarrierPair(nav.range, nav.carrier), nav.prn);
      			ephData[ni] = nav;
	      		ephPageStore[ni][sfid] = nav;
         		EngEphemeris engEph;
         		
   	   		if (makeEngEphemeris(engEph, ephPageStore[ni]))
      			{
	         		gpsEphStore.addEphemeris(engEph);
   	      		ephPageStore[ni].clear();
      			}
				} 	// for (int s=1; s<=3; s++)
      	} 		// else if (epb.checkId(hdr.id) && (input >> epb) && epb)
		}    		// while (input >> hdr)
	}       		// virtual void process()

   virtual void shutDown()
   {}

   GPSWeekSecond time;

private:
   AshtechStream input;
   int offsetSec;
   ifstream inputDev;
   ofstream output;
   CommandOptionWithAnyArg inputOpt, outputOpt, codeOpt;
   CommandOptionWithNumberArg weekOpt, offsetOpt, numPointsOpt;
   GPSEphemerisStore gpsEphStore;
   bool firstEph;
   RangeCode rangeCode;
   DayTime lastTime;
   int numPoints;
   
   typedef pair<RangeCode, CarrierCode> RangeCarrierPair;
   typedef pair<RangeCarrierPair, short> NavIndex;
   typedef map<NavIndex, MDPNavSubframe> NavMap;
   NavMap ephData;
   map<NavIndex, EphemerisPages> ephPageStore;
   
   typedef pair<double, double> PhasePair;
   typedef pair<DayTime, PhasePair> TimePhasePair;
   typedef pair<double, double> RangePair;
   typedef vector<TimePhasePair> TimePhaseVec;
   typedef vector<RangePair> RangePairVec;
};


int main(int argc, char *argv[])
{
   Ashtech2XYZ crap(argv[0]);
   
   if (!crap.initialize(argc, argv))
      exit(0);
   
   crap.run();
}
