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


const string OrdApp::defaultTimeFormat("%4Y %3j %02H:%02M:%04.1f");

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
{}


//-----------------------------------------------------------------------------
// Here the command line options parsed and used to configure the program
//-----------------------------------------------------------------------------
bool OrdApp::initialize(int argc, char *argv[]) throw()
{
   if (!BasicFramework::initialize(argc,argv))
      return false;

   if (debugLevel)
      cout << "# program:" << argv0 << endl
           << "# debugLevel: " << debugLevel << endl
           << "# verboseLevel: " << verboseLevel << endl;

   if (outputOpt.getCount())
   {
      const string fn=outputOpt.getValue()[0];
      output.open(fn.c_str(), ios::out);
      if (debugLevel)
         cout << "# Sending output to" << fn << endl;
   }
   else
   {
      if (debugLevel)
         cout << "# Sending output to stdout" << endl;
      output.copyfmt(cout);
      output.clear(cout.rdstate());
      output.basic_ios<char>::rdbuf(cout.rdbuf());
   }

   if (inputOpt.getCount())
   {
      const string fn = inputOpt.getValue()[0];
      input.open(fn.c_str(), ios::in);
      if (debugLevel)
         cout << "# Reading ords from" << fn << endl;
   }
   else
   {
      if (debugLevel)
         cout << "# Reading ords from stdin" << endl;
      input.copyfmt(cin);
      input.clear(cin.rdstate());
      input.basic_ios<char>::rdbuf(cin.rdbuf());
   }

   if (timeFormatOpt.getCount())
      timeFormat = timeFormatOpt.getValue()[0];

   return true;
}


void OrdApp::write(ofstream& s, const ORDEpoch& ordEpoch) throw()
{
   if (!headerWritten)
   {
      s << "# Time              Type PRN  Elev         ORD(m) wonky" << endl;
      headerWritten=true;
   }

   s.setf(ios::fixed, ios::floatfield);
   s << setfill(' ') << right;
   
   string time = ordEpoch.time.printf(timeFormat);
   ORDEpoch::ORDMap::const_iterator pi;
   for (pi = ordEpoch.ords.begin(); pi != ordEpoch.ords.end(); pi++)
   {
      const SatID& svid = pi->first;
      const ObsRngDev& ord = pi->second;

      int type = 0;
      s << time << " " << setw(4) << type
        << " " << setw(3) << svid.id
        << " " << setprecision(1) << setw(5)  << ord.getElevation()
        << " " << setprecision(5) << setw(14) << ord.getORD()
        << " " << hex << setw(5) << ord.wonky << dec
        << endl;
   }

   if (ordEpoch.clockResidual.is_valid())
   {
      int type = 1;
      s << time << " " << setw(4) << type
        << " " << setprecision(5) << setw(24) << ordEpoch.clockResidual
        << endl;
   }

   if (ordEpoch.clockOffset.is_valid())
   {
      int type = 50;
      if (ordEpoch.wonky)
         type = 70;
      s << time << " " << setw(4) << type
        << " " << setprecision(5) << setw(24) << ordEpoch.clockOffset
        << endl;
   }
}

ORDEpoch OrdApp::read(std::ifstream& s) throw()
{
   ORDEpoch ordEpoch;
   ordEpoch.time = DayTime(DayTime::BEGINNING_OF_TIME);
   using namespace StringUtils;
   while (s)
   {      
      try
      {
         if (readBuffer.size() == 0)
         {
            getline(s, readBuffer);
            strip(readBuffer);
         }
         
         if ((readBuffer.size() < 24) || 
             (readBuffer=="# Time              Type PRN  Elev         ORD(m) wonky"))
         {
            readBuffer.erase(0, string::npos);
            continue;
         }
         else if ((readBuffer[0] == '#') ||(readBuffer[0] == '>')) 
         {
            output << readBuffer << endl;
            readBuffer.erase(0, string::npos);
            continue;
         }         

         DayTime time;
         time.setToString(readBuffer.substr(0,19), timeFormat);

         // This means that we have a complete epoch. Note that the most
         // recently read line is left in readBuffer
         if (ordEpoch.time != time && ordEpoch.ords.size() > 0)
            break;

         ordEpoch.time = time;

         istringstream iss(readBuffer.substr(20, string::npos));
         int type;
         iss >> type;

         if (type == 0)
         {
            if (readBuffer.size() < 46)
            {
               cout << "# Line too short" << endl;
               continue;
            }

            ObsRngDev ord;
            ord.obstime = time;

            int prn;
            double elev, res;
            unsigned wonky;

            iss >> prn >> elev >> res >> hex >> wonky >> dec;

            SatID svid(prn, SatID::systemGPS);
            ord.svid = svid;
            ord.elevation = elev;
            ord.ord = res;
            ord.wonky = wonky;
            
            ordEpoch.ords[svid] = ord;
         }
         else if (type == 1)
         {
            double c;
            iss >> c;
            ordEpoch.clockResidual = c;
         }
         else if (type == 50 || type == 70)
         {
            double c;
            iss >> c;
            ordEpoch.clockOffset = c;
            if (type == 70)
               ordEpoch.wonky = true;
         }

         readBuffer.erase(0, string::npos);
      }
      catch (Exception& e)
      {
         cout << "# Error reading ord file " << e << endl;
      }
   }

   return ordEpoch;
}
