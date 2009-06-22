//$Id$

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
#include <sstream>

#include <list>
#include <map>
#include <set>

#include "DayTime.hpp"
#include "GPSWeekSecond.hpp"
#include "TimeConstants.hpp"
#include "Exception.hpp"
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"

#include "StringUtils.hpp"
#include "InOutFramework.hpp"
#include "ObsUtils.hpp"

#include "MDPStream.hpp"
#include "MDPNavSubframe.hpp"
#include "MDPObsEpoch.hpp"
#include "MDPSelftestStatus.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


class MDPEdit : public InOutFramework<MDPStream, MDPStream>
{
public:
   MDPEdit(const string& applName)
      throw()
      : InOutFramework<MDPStream, MDPStream>(
         applName, "Edits a MDP file based on various criteria.")
   {}

   bool initialize(int argc, char *argv[]) throw()
   {
      // set up options. note that nothing is required
      CommandOptionWithAnyArg startOpt('s',"start","Throw out data before"
         " this time. Format as string: \"yyyy ddd HH:MM:SS\" ");
      CommandOptionWithAnyArg endOpt('e',"end","Throw out data after this"
         " time. Format as string: \"yyyy ddd HH:MM:SS\" ");
      CommandOptionWithAnyArg minSNROpt('\0',"snr","Throw out data with an SNR lower"
         " than this value. (dBHz)");
      CommandOptionWithNumberArg prnOpt('p',"PRN","Throw out obs data from"
         " this PRN. Repeat option for mutiple SVs.");
      CommandOptionWithNumberArg recordStartOpt('\0',"record-start","Throw"
         " out data before this record number.");
      CommandOptionWithNumberArg recordEndOpt('\0',"record-end","Throw out"
         " data after this record number.");
      CommandOptionNoArg obsOpt('O',"no-obs","Remove all obs messages.");
      CommandOptionNoArg navOpt('N',"no-nav","Remove all nav messages.");
      CommandOptionNoArg pvtOpt('P',"no-pvt","Remove all pvt messages.");
      CommandOptionNoArg stsOpt('S',"no-sts","Remove all self test "
                                    "status messages.");

      if (!InOutFramework<MDPStream, MDPStream>::initialize(argc,argv))
         return false;
         
      // get any PRNs to toss
      for (int index = 0; index < prnOpt.getCount(); index++ )
      {
         int prn = asInt(prnOpt.getValue()[index]);

         if ((prn < 1)||(prn > gpstk::MAX_PRN))
         {
            cout << "\n You entered an invalid PRN."
                 << "\n Exiting.\n\n";
            return false;
         }
         else 
         {
            prnSetToToss.insert(prn);
            if (debugLevel || verboseLevel)
               cout << "Throwing out data from PRN " << prn << endl;
         }
      }

      // get any time limits
      if (startOpt.getCount())
      {
         tStart.setToString(startOpt.getValue().front().c_str(),
                            "%Y %j %H:%M:%S");
         if (debugLevel)
            cout << "Throwing out data before " << tStart << endl;
      }
      else
      {
         tStart = DayTime::BEGINNING_OF_TIME;
         if (debugLevel || verboseLevel)
            cout << "No start time given.\n";
      }

      if (endOpt.getCount())
      {
         tEnd.setToString(endOpt.getValue().front().c_str(),"%Y %j %H:%M:%S");
         if (debugLevel || verboseLevel)
            cout << "Throwing out data after  " << tEnd << endl;
      }
      else
      {
         tEnd = DayTime::END_OF_TIME;
         if (debugLevel || verboseLevel)
            cout << "No end time given.\n";
      }

      recordStart = recordEnd = 0;
      if (recordStartOpt.getCount())
      {
         recordStart = asInt(recordStartOpt.getValue().front());
         if (debugLevel || verboseLevel)
            cout << "Throwing out data before record number "
                 << recordStart << endl;
      }
      if (recordEndOpt.getCount())
      {
         recordEnd = asInt(recordEndOpt.getValue().front());
         if (debugLevel || verboseLevel)
            cout << "Throwing out data after record number "
                 << recordEnd << endl;
      }

      minSNR = 0.0;
      if (minSNROpt.getCount())
      {
         minSNR = asDouble(minSNROpt.getValue().front());
         if (debugLevel || verboseLevel)
            cout << "Throwing out data with SNR < " << minSNR << endl;
      }

      // see if any message types should be removed
      noObs = noNav = noPvt = noSts = false;
      if (obsOpt.getCount())
      {
         noObs = true;
         if (debugLevel || verboseLevel)
            cout << "Removing obs messages.\n";
      }
      if (navOpt.getCount())
      {
         noNav = true;
         if (debugLevel || verboseLevel)
            cout << "Removing nav messages.\n";
      }
      if (pvtOpt.getCount())
      {
         noPvt = true;
         if (debugLevel || verboseLevel)
            cout << "Removing pvt messages.\n";
      }
      if (stsOpt.getCount())
      {
         noSts = true;
         if (debugLevel || verboseLevel)
            cout << "Removing self test status messages.\n";
      } 

      return true;
   }

protected:
   virtual void spinUp()
   {}

   virtual void process()
   {
      msgCount=0;

      die = false;

      DayTime currEpoch;
      MDPEpoch oe;
      uint16_t fc=0; 
      
      while (!input.eof())     
      {
         input >> header;

         if (!input)
            break;
         else if (header.time > tEnd)
            continue;
         else if (recordEnd && (input.recordNumber > recordEnd))
	    return;
	 else if (header.time < tStart)
	    continue;
         else if (recordStart && (input.recordNumber < recordStart))
            continue;
	 
         msgCount++;

         if (msgCount == 1)
         {
            fc = header.freshnessCount;
            if (debugLevel)
               cout << "First message at " << header.time << endl;
         }

         if (verboseLevel > 4 || debugLevel > 3)
            cout << "Record: "    << input.recordNumber
                 << ", message: " << msgCount << ":" << endl;
              
         switch (input.header.id)
         {
            case gpstk::MDPObsEpoch::myId:
               if (!noObs)
               {
                  MDPObsEpoch obs;
                  input >> obs;
                  if (obs)
                  {
                     MDPObsEpoch::ObsMap::iterator i;
                     for (i = obs.obs.begin(); i != obs.obs.end(); )
                     {
                        MDPObsEpoch::Observation& o = i->second;
                        if (o.snr < minSNR)
                        {
                           obs.obs.erase(i++);
                           if (debugLevel > 2)
                           cout << "Dropping " << obs.prn
                                << " " << o.carrier << "," << o.range
                                << ", snr=" << o.snr << endl;
                        }
                        else
                           i++;
                     }
                     if (prnSetToToss.size())
                     {
                        // If we have moved on to a new time, output the previous epoch
                        if (currEpoch != obs.time)
                        {
                           if (!oe.empty())
                           {
                              MDPEpoch::iterator i;
                              for (i=oe.begin(); i != oe.end(); i++)
                              {
                                 i->second.numSVs = oe.size();
                                 i->second.freshnessCount = fc++;
                                 output << i->second;
                              }
                           }
                           currEpoch = obs.time;
                           oe.clear();
                        }
                        if (!prnSetToToss.count(obs.prn))
                        {
                           oe.insert(pair<const int, MDPObsEpoch>(obs.prn, obs));
                           if (debugLevel > 2)
                           {
                              cout << "  Writing obs message:\n";
                              obs.dump(cout);
                           }
                        }
                        else
                        {
                           if (debugLevel > 2)
                              cout << "  Not writing obs message for PRN "
                                   << obs.prn << endl;
                        }
                     }
                     else
                     {
                        if (debugLevel > 2)
                        {
                           cout << "  Writing obs message:\n";
                           obs.dump(cout);
                        }
                        obs.freshnessCount = fc++;
                        output << obs;    
                     }             
                  }
                  else
                  {
                     if (debugLevel > 2)
                     {
                        cout << "  Tossing obs message due to a bad read:\n";
                        obs.dump(cout);
                     }
                  }
               }
               else
               {
                  if (debugLevel > 3)
                  {
                     cout << "  Ignoring obs message from record "
                          << input.recordNumber << endl;
                  }
               }
               break;

            case gpstk::MDPPVTSolution::myId:
               if (!noPvt)
               {
                  MDPPVTSolution pvt;
                  input  >> pvt;
                  if (pvt)
                  {
                     pvt.freshnessCount = fc++;
                     output << pvt;
                     if (debugLevel > 2)
                     {
                        cout << "  Writing pvt message:\n";
                        pvt.dump(cout);
                     }
                  }
                  else if (debugLevel > 2)
                  {
                     cout << "  Tossing pvt message:\n";
                     pvt.dump(cout);
                  }                   
               }
               else if (debugLevel > 3)
               {
                  cout << "  Ignoring pvt message from record "
                       << input.recordNumber << endl;
               }               
               break;

            case gpstk::MDPNavSubframe::myId:
               if (!noNav)
               {
                  MDPNavSubframe nav;
                  input  >> nav;
                  if (nav)
                  {
                     nav.freshnessCount = fc++;
                     output << nav;
                     if (debugLevel > 2)
                     {
                        cout << "  Writing nav message:\n";
                        nav.dump(cout);
                     }
                  }
                  else if (debugLevel > 2)
                  {
                     cout << "  Tossing nav message:\n";
                     nav.dump(cout);
                  }                   
               }
               else if (debugLevel > 3)
               {
                  cout << "  Ignoring nav message from record "
                       << input.recordNumber << endl;                     
               }
               break;

            case gpstk::MDPSelftestStatus::myId:
               if (!noSts) 
               {
                  gpstk::MDPSelftestStatus sts;
                  input  >> sts;
                  if (sts)
                  {
                     sts.freshnessCount = fc++;
                     output << sts;
                     if (debugLevel > 2)
                     {
                        cout << "  Writing self test status message:\n";
                        sts.dump(cout);
                     }
                  }
               }
               else if (debugLevel > 3)
               {
                  cout << "  Ignoring status message from record "
                       << input.recordNumber << endl;                     
               }               
               break;

         } // switch (input.header.id)
      }    // while (!input.eof())  
      
      timeToDie = true;
      
   }       // virtual void process()

   virtual void shutDown()
   {
      if (verboseLevel)
         cout << "Doneskies.\n";
   }
     
private:
   bool noObs, noNav, noPvt, noSts, die; 
   DayTime tStart, tEnd;
   set<int> prnSetToToss;
   unsigned int recordStart, recordEnd;
   unsigned long msgCount, fcErrorCount;
   double minSNR;
   unsigned short firstFC, lastFC;
   MDPHeader header;
   
};


int main(int argc, char *argv[])
{
   MDPEdit crap(argv[0]);
   
   if (!crap.initialize(argc, argv))
      exit(0);
   
   crap.run();
}
