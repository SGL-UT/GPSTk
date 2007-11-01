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






#include <math.h>
#include <iostream>
#include "RinexObsData.hpp"
#include "RinexObsStream.hpp"
#include "CommandOption.hpp"
#include "CommandOptionParser.hpp"
#include "StringUtils.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])

{

   const double ttolerance=1e-3;
   
   CommandOptionWithArg fileOption(CommandOption::stdType, 'f',"filename","RINEX obs file to be thinned.",true);
   CommandOptionWithNumberArg dashs('s',"Seconds", "The desired data rate.",true);
   CommandOptionWithArg fileOutput(CommandOption::stdType,'o',"filename", "RINEX obs file with thinned obs.", true);
   CommandOptionParser m("This program thins or 'decimates' an input RINEX observation file.");

   dashs.setMaxCount(1);
   fileOption.setMaxCount(1);
   m.parseOptions(argc,argv);
   if (m.hasErrors())
   {
      m.dumpErrors(cout);
      m.displayUsage(cout);
   }
   int i;
  
   for (i=0;(i < fileOption.getCount());i++)
   {
      RinexObsStream ros(fileOption.getValue()[i].c_str());
      DayTime nextTime;
      bool firstRecord=true;

      RinexObsData data;
      RinexObsStream out(fileOutput.getValue()[i].c_str(), ios::out|ios::trunc);
      
      int drate= StringUtils::asInt(dashs.getValue()[0]);
      dashs.setMaxCount(1);
      RinexObsHeader h;


      long wcount=0, rcount=0;
      
      ros >> h;

      while (ros >> data)
      {
         rcount++;

	 if ( fmod(data.time.secOfDay(),drate) <= ttolerance )
         {
 
          // First obs? Then update and output the header.
         if (wcount==0)
         {
            if (h.valid & RinexObsHeader::intervalValid)
               h.interval = drate;
         
            if (h.valid & RinexObsHeader::firstTimeValid)
               h.firstObs = data.time;
      
            out << h;
         }

            out<<data;
            wcount++;
         }
      }

      cout << "Obs read:    " << rcount << endl;
      cout << "Obs written: " << wcount << endl;

   }          		   

   return 0;
}

