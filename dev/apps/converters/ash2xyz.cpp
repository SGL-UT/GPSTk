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

#include "MDPStream.hpp"
#include "MDPNavSubframe.hpp"
#include "MDPObsEpoch.hpp"

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
         "\"codeless\".")
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
		if (offsetOpt.getCount())
         offsetSec = StringUtils::asInt(weekOpt.getValue()[0]); 
      else
         offsetSec = 0;
         
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
      
      bool firstPBEN = false;

      AshtechData hdr;
      AshtechPBEN pben;
      AshtechMBEN mben;
      AshtechEPB  epb;
      AshtechALB  alb;
      unsigned short fc=0;
      vector<MDPObsEpoch> hint(33);
      short svCount = 0;

      while (input >> hdr)
      {
         if (debugLevel>1)
            cout << "---" << endl;

         if (pben.checkId(hdr.id) && (input >> pben) && pben)
         {
            if (debugLevel>4)
               pben.dump(cout);
            
            double dt = pben.sow - time.sow;
            time.sow = pben.sow;
            svCount = 0;
            firstPBEN = true;

            if (std::abs(dt) > HALFWEEK && !firstPBEN)
               time.week++;
         }
         else if (mben.checkId(hdr.id) && (input >> mben) && mben)
         {
            if (debugLevel>1)
            {
               cout << "---\n";
               mben.dump(cout);
             }
            if (svCount==0)
               svCount = mben.left+1;

            if (firstPBEN != true)
            	continue;
            	
            hint[mben.svprn].time = DayTime(time.week, time.sow);
            hint[mben.svprn].numSVs = svCount;
            MDPObsEpoch moe = makeMDPObsEpoch(mben, hint[mben.svprn]);
            moe.freshnessCount = fc++;
            hint[mben.svprn] = moe;

            if (debugLevel>1)
            {
             	cout << "---\n"
              	     << "MDPObsEpoch::dump():\n";
              	moe.dump(cout);
				}
            // compute the errors for this PRN and epoch
            MDPObsEpoch::ObsMap::const_iterator i;
               
			   double prL1;   // pseudorange on L1Z (m)
			   double prL2;   // pseudorange on L2Z (m)
			   double doppL1; // Doppler on L1Z (Hz)
			   double doppL2; // Doppler on L2Z (Hz)

            for (i = moe.obs.begin(); i != moe.obs.end(); i++)
            {
               const MDPObsEpoch::Observation& obs=i->second;

               if (obs.carrier == ccL1 && obs.range == rangeCode)
               {
                	prL1   = obs.pseudorange;
                 	doppL1 = obs.snr;
               }
               
               if (obs.carrier == ccL2 && obs.range == rangeCode)
               {
                  prL2   =  obs.pseudorange;
                  doppL2 = obs.snr;
               }
            }

            double ionoError  = (prL1 - prL2)/
                               ((gpstk::L1_FREQ/gpstk::L2_FREQ)*
                                (gpstk::L1_FREQ/gpstk::L2_FREQ) - 1); // m

            // iono rate is essentially the same eqn, but substitute 
            // (doppler * wavelenghth) for pseudorange
              
            double x1 = doppL1 * gpstk::L1_WAVELENGTH; // m/s
            double x2 = doppL2 * gpstk::L2_WAVELENGTH; // m/s
                        
            double ionoErrorRate = (x1 - x2)/
                                   ((gpstk::L1_FREQ/gpstk::L2_FREQ)*
                                   (gpstk::L1_FREQ/gpstk::L2_FREQ) - 1); // m/s
                                   
            if (debugLevel > 4)
              	cout << "---\nx1 (m/s): " << setprecision(4) << x1 << endl
              	     << "x2 (m/s): "      << setprecision(4) << x2 << endl;
                                                
            if (debugLevel > 2)
               cout << "---\nIono errors for PRN " << moe.prn << " at " 
                    << moe.time << ":\n" << fixed 
                    << "pseudorange on L1 (m): " << setprecision(4) << prL1 
                    << endl << "pseudorange on L2 (m): " << setprecision(4) 
                    << prL2 << endl << "Iono Error (m) : " << setprecision(4) 
                    << ionoError << endl << "L1 Doppler (Hz): " 
                    << setprecision(4) << doppL1 << endl << "L2 Doppler (Hz): "
                    << setprecision(4) << doppL2 << endl << "Iono rate (m/s): " 
                    << setprecision(4) << ionoErrorRate << endl;

            for (i = moe.obs.begin(); i != moe.obs.end(); i++)
            {
               const MDPObsEpoch::Observation& obs=i->second;
               SatID satID(moe.prn,SatID::systemGPS);

               // There will (usually) be 3 MBEN messages for each SV for 
               // the C/A, L1 P/Z/Y and L2 P/Z/Y obs measurements. Let's just 
               // output data when we see the C/A measurement

               if (obs.carrier == ccL1 && obs.range == rcCA)
               {
						try
						{
                     DayTime tempTime = moe.time + offsetSec;
                     Xvt xvt = gpsEphStore.getXvt(satID,tempTime);
                     EngEphemeris tempEph = gpsEphStore.findEphemeris(satID,
                                                                     moe.time);
						
                     cout << moe.time.printf("%4Y/%03j/%02H:%02M:%04.1f")
                          << fixed << right
                          << ", " << setw(8) << offsetSec
                          << ", " << setw(2) << (int) moe.prn
                          << ", " << setprecision(4) << setw(10) << ionoError
                          << ", " << setprecision(4) << setw(10)  
                          << ionoErrorRate
                          << ", " << setprecision(2) << setw(12) << xvt.x[0]
                          << ", " << setprecision(2) << setw(12) << xvt.x[1]
                          << ", " << setprecision(2) << setw(12) << xvt.x[2];
						 if (debugLevel)
						    cout << ", " << setprecision(2) << setw(7) 
                                   << moe.elevation << ", " 
                                   << setprecision(2) << setw(7) 
                                   << moe.azimuth;
                   cout << endl;
                  }
                  catch (...)
                  {
                     if (debugLevel > 1)
                        cout << "---\nCould not output data for PRN " 
                             << moe.prn << " at " << moe.time << endl;
                  }
               } // if (obs.carrier == ccL1 && obs.range == rcYcode)
            }    // for (i = moe.obs.begin(); i != moe.obs.end(); i++)
		 	}       // else if (mben.checkId(hdr.id) && (input >> mben) && mben)
         else if (epb.checkId(hdr.id) && (input >> epb) && epb)
			{
				// sort of a lazy hack here - using functionality from MDP classes
            if (debugLevel)
               epb.dump(cout);
            MDPNavSubframe nav;

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
				}
				
				if (firstEph && debugLevel)
					cout << "---\nGot first nav SF" << endl;
				
				MDPNavSubframe tmp = nav;		
				
				// First try the data assuming it is already upright
      		tmp.cooked = true;
      		bool parityGood = tmp.checkParity();
      		if (!parityGood)
      		{
         		if (debugLevel && firstEph)
            		cout << "Raw subframe" << endl;
         		nav.cooked = false;
         		nav.cookSubframe();
         		parityGood = nav.checkParity();
      		}
      		else
      		{
         		if (debugLevel && firstEph)
            		cout << "Cooked subframe" << endl;
      		}

      		firstEph=false;

      		if (!parityGood)
      		{
         		if (debugLevel)
            		cout << "Parity error" << endl;
         		return;
      		}

      		short sfid = nav.getSFID();
      		if (sfid > 3)
         		return;

      		short week = nav.time.GPSfullweek();
      		long sow = nav.getHOWTime();
      		if (sow > DayTime::FULLWEEK)
      		{
        	 		if (debugLevel)
            		cout << "Bad week" << endl;
         		return;
      		}

      		if (debugLevel>1)
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
            else if (debugLevel>2)
	           cout << "---\nmakeEngEphemeris failed for PRN " << epb.prn
                   << " at HOW time " << nav.time << endl;      		

            if (debugLevel > 3)
            {
               cout << "---\nengEph dump:\n";
               engEph.dump(cout);	
            }
      	} // else if (epb.checkId(hdr.id) && (input >> epb) && epb)
		}    // while (input >> hdr)
	}       // virtual void process()

   virtual void shutDown()
   {}

   GPSWeekSecond time;

private:
   AshtechStream input;
   int offsetSec;
   ifstream inputDev;
   ofstream output;
   CommandOptionWithAnyArg inputOpt, outputOpt, codeOpt;
   CommandOptionWithNumberArg weekOpt, offsetOpt;
   GPSEphemerisStore gpsEphStore;
   typedef pair<RangeCode, CarrierCode> RangeCarrierPair;
   typedef pair<RangeCarrierPair, short> NavIndex;
   typedef map<NavIndex, MDPNavSubframe> NavMap;
   typedef std::map<short, MDPNavSubframe> EphemerisPages;
   map<NavIndex, EphemerisPages> ephPageStore;
   bool firstEph;
   NavMap ephData;
   RangeCode rangeCode;
};


int main(int argc, char *argv[])
{
   Ashtech2XYZ crap(argv[0]);
   
   if (!crap.initialize(argc, argv))
      exit(0);
   
   crap.run();
}
