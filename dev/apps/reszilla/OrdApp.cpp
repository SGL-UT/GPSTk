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

#include <iostream>

#include "OrdApp.hpp"

using namespace std;
using namespace gpstk;


const std::string OrdApp::defaultTimeFormat("%Y %j %02H:%02M:%04.1f");

//-----------------------------------------------------------------------------
// The constructor basically just sets up all the command line options
//-----------------------------------------------------------------------------
OrdApp::OrdApp(
   const string& applName,
   const string& appDesc)
   throw()
   : BasicFramework(applName, appDesc),
     timeFormat(defaultTimeFormat),
     headerWritten(false),
     inputOpt('i', "input", 
              "Where to get the data to analyze. The default is stdin."),
     outputOpt('r', "output",
               "Where to send the output. The default is stdout."),
     timeFormatOpt('t', "time-format", "Daytime format specifier used for "
                   "times in the output. The default is \"" 
                   + defaultTimeFormat + "\".")
{
}


//-----------------------------------------------------------------------------
// Here the command line options parsed and used to configure the program
//-----------------------------------------------------------------------------
bool OrdApp::initialize(int argc, char *argv[]) throw()
{
   if (!BasicFramework::initialize(argc,argv)) return false;

   if (debugLevel)
      cout << "debugLevel: " << debugLevel << endl
           << "verboseLevel: " << verboseLevel << endl;

   if (outputOpt.getCount())
   {
      const string fn=outputOpt.getValue()[0];
      output.open(fn.c_str(), std::ios::out);
      if (debugLevel)
         cout << "Sending output to" << fn << endl;
   }
   else
   {
      if (debugLevel)
         cout << "Sending output to stdout" << endl;
      output.copyfmt(std::cout);
      output.clear(std::cout.rdstate());
      output.std::basic_ios<char>::rdbuf(std::cout.rdbuf());
   }

   if (inputOpt.getCount())
   {
      const string fn = inputOpt.getValue()[0];
      input.open(fn.c_str(), std::ios::in);
      if (debugLevel)
         cout << "Reading ords from" << fn << endl;
   }
   else
   {
      if (debugLevel)
         cout << "Reading ords from stdin" << endl;
      input.copyfmt(std::cin);
      input.clear(std::cin.rdstate());
      input.std::basic_ios<char>::rdbuf(std::cin.rdbuf());
   }

   if (timeFormatOpt.getCount())
      timeFormat = timeFormatOpt.getValue()[0];

   return true;
}


void OrdApp::write(std::ofstream& s, const ORDEpoch& ordEpoch) throw()
{
   if (!headerWritten)
   {
      s << "# time              type PRN  elev     ord/clk(m)  iodc  health"
        << endl;
      headerWritten=true;
   }

   s.setf(ios::fixed, ios::floatfield);
   s << setfill(' ');
   
   string time = ordEpoch.time.printf(timeFormat);
   ORDEpoch::ORDMap::const_iterator pi;
   for (pi = ordEpoch.ords.begin(); pi != ordEpoch.ords.end(); pi++)
   {
      const SatID& svid = pi->first;
      const ObsRngDev& ord = pi->second;
         
      s << left << setw(20) << time << right
        << " " << setw(4) << 0 // type
        << " " << setw(2) << svid.id
        << " " << setprecision(1) << setw(5)  << ord.getElevation()
        << " " << setprecision(5) << setw(14) << ord.getORD()
        << hex
        << " " << setw(5) << ord.getIODC()
        << " " << setw(5) << ord.getHealth()
        << dec
        << endl;
   }

   if (ordEpoch.clockOffset.is_valid())
      s << left << setw(20) << time << right
        << " " << setw(4) << 50 //type
        << " " << setprecision(3) << setw(14)  << ordEpoch.clockOffset
        << endl;
}

gpstk::ORDEpoch OrdApp::read(std::ifstream& s) throw()
{
}
