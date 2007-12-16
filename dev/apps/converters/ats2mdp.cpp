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

#include <string>
#include <sstream>

#include <list>
#include <map>

#include "DayTime.hpp"
#include "GPSWeekSecond.hpp"
#include "TimeConstants.hpp"
#include "Exception.hpp"
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"

#include "StringUtils.hpp"
#include "InOutFramework.hpp"

#include "MDPStream.hpp"
#include "MDPNavSubframe.hpp"
#include "MDPObsEpoch.hpp"

#include "ATSStream.hpp"
#include "ATSData.hpp"

#include "ObsUtils.hpp"

using namespace std;
using namespace gpstk;


class ATS2MDP : public InOutFramework<ATSStream, MDPStream>
{
public:
   ATS2MDP(const string& applName)
      throw()
      : InOutFramework<ATSStream,MDPStream>(
         applName, "Converts ATS binary format data to "
         "MDP format.")
   {}

   bool initialize(int argc, char *argv[]) throw()
    {

      if (!InOutFramework<ATSStream, MDPStream>::initialize(argc,argv))
         return false;

      ATSData::debugLevel = debugLevel;
      if (debugLevel>3)
         ATSData::hexDump = true;

      if (debugLevel>4)
         MDPHeader::hexDump = true;

      return true;
   }
   
protected:
   virtual void spinUp()
   {}

   virtual void process()
   {
      input.rangeBias.resize(12);
      for (int i=0; i<12; i++)
         input.rangeBias[i] = i<6 ? 0 : 14.656;

      unsigned short fc=0;
      ATSData ats_record;
      MDPEpoch hint;
      try
      {
         if (debugLevel>1)
            input.exceptions(ifstream::eofbit|ifstream::failbit|ifstream::badbit);
         while (input >> ats_record)
         {
            if (debugLevel>2)
            {
               cout << "Record Number:" << input.recordNumber << endl;
               ats_record.dump(cout);
            }
            MDPEpoch me = makeMDPEpoch(ats_record, hint);

            MDPEpoch::iterator i;
            for (i=me.begin(); i != me.end(); i++)
               i->second.freshnessCount = fc++;

            hint = me;
            if (debugLevel>1)
               dump(cout, me);
            else
               output << me;
         }
      }
      catch (Exception& e)
      {
         cout << "Caught exception: " << e << endl;
      }

      timeToDie = true;
   }

   virtual void shutDown()
   {}

};


int main(int argc, char *argv[])
{
   ATS2MDP crap(argv[0]);
   
   if (!crap.initialize(argc, argv))
      exit(0);
   
   crap.run();
}
