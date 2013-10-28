#pragma ident "$Id: $"

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
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

#include "BasicFramework.hpp"
#include "CommandOptionWithPositionArg.hpp"
#include "FICData.hpp"
#include "FICHeader.hpp"
#include "FICStream.hpp"
#include "GPSEphemerisStore.hpp"
#include "RinexNavData.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavStream.hpp"
#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "StringUtils.hpp"

using namespace std;
using namespace gpstk;

   /**
    * This class uses RINEX nav data to process RINEX obs data and strips
    * any observations that are below a set elevation angle. It requires
    * an antenna position to work, if one is specified on the command line
    * it will use that, if not, it will look for one in the RINEX obs
    * header.
    */
class Stripper : public BasicFramework
{
public:
   Stripper(const string& app) throw();
   virtual ~Stripper() throw() {}
   virtual bool initialize(int argc, char *argv[], bool pretty=true) throw();

protected:
   virtual void additionalSetup();
   virtual void process();

private:
   static const double DEFAULT_ELEVATION_MASK;
   CommandOptionWithAnyArg inputArg;
   CommandOptionWithAnyArg outputArg;
   CommandOptionWithAnyArg navArg;
   CommandOptionWithPositionArg posArg;
   CommandOptionWithAnyArg elvArg;
   GPSEphemerisStore ephs;
   Position pos;
   double elvmask;
};

const double Stripper::DEFAULT_ELEVATION_MASK = 10.0;

Stripper::Stripper(const string& app) throw()
      : BasicFramework(app, "Strip low elevation data from observations."),
        inputArg('i', "input", "Input RINEX obs file", true),
        outputArg('o', "output", "Output RINEX obs file", true),
        navArg('n', "nav", "Input RINEX or FIC nav file(s)", true),
        posArg('p', "position", "%x %y %z", "position (XYZ ECEF in meters)"),
        elvArg('e', "elevation", "elevation strip angle (default = 10 degrees)")
{
   inputArg.setMaxCount(1);
   outputArg.setMaxCount(1);
   posArg.setMaxCount(1);
   elvArg.setMaxCount(1);
}

bool Stripper::initialize(int argc, char *argv[], bool pretty) throw()
{
   if (!BasicFramework::initialize(argc, argv, pretty))
      return false;
   
   if (elvArg.getCount())
      elvmask = StringUtils::asDouble(elvArg.getValue()[0]);
   else
      elvmask = DEFAULT_ELEVATION_MASK;

   return true;
}

void Stripper::additionalSetup()
{
   for (size_t i = 0; i < navArg.getCount(); i++)
   {
      bool done = false;
      
      RinexNavStream rns(navArg.getValue()[i].c_str());
      RinexNavHeader rnh;
      rns >> rnh;
      RinexNavData rnd;
      rns >> rnd;
      if (rns)
      {
         done = true;
         if (verboseLevel)
            cout << "Loading RINEX nav data from "
                 << navArg.getValue()[i] << endl;
         ephs.addEphemeris(rnd);
         while (rns >> rnd)
            ephs.addEphemeris(rnd);
      }
      
      if (done)
         continue;
      
      FICStream fics(navArg.getValue()[i].c_str());
      FICHeader fich;
      fics >> fich;
      FICData ficd;
      fics >> ficd;
      if (fics)
      {
         if (verboseLevel)
            cout << "Loading FIC nav data from "
                 << navArg.getValue()[i] << endl;
         try
         {
            ephs.addEphemeris(RinexNavData(static_cast<EngEphemeris>(ficd)));
         }
         catch (WrongBlockNumber& e) {}
         while (fics >> ficd)
         {
            try
            {
               ephs.addEphemeris(RinexNavData(static_cast<EngEphemeris>(ficd)));
            }
            catch (WrongBlockNumber& e) {}
         }
      }
   }
}

void Stripper::process()
{
   RinexObsStream oros(outputArg.getValue()[0].c_str(), ios::out);
   RinexObsStream iros(inputArg.getValue()[0].c_str());
   RinexObsHeader roh;
   iros >> roh;
   oros << roh;

      // was the position specified on the command line?
   if (posArg.getCount())
   {
      pos = posArg.getPosition()[0];
   }
      // is there a valid position in the header?
   else if (roh.valid & RinexObsHeader::antennaPositionValid)
      pos = roh.antennaPosition;
   else
   {
         // we have no position, terminate with error
      cerr << "No valid position specified or in header." << endl;
      return;
   }
   
   RinexObsData irod;
   while (iros >> irod)
   {
      RinexObsData orod;
      orod.time = irod.time;
      orod.epochFlag = irod.epochFlag;
      orod.clockOffset = irod.clockOffset;
      orod.auxHeader = irod.auxHeader;
      for (RinexObsData::RinexSatMap::iterator i = irod.obs.begin();
           i != irod.obs.end(); i++)
      {
         try
         {
            double elv = pos.elevation(ephs.getXvt(i->first, irod.time).getPos());
            if (elv >= elvmask)
               orod.obs[i->first] = i->second;
            else if (verboseLevel)
               cout << "Stripped PRN " << i->first.id << " (elv = "
                    << setprecision(2) << elv << ") at "
                    << CivilTime(irod.time).printf("%02m/%02d/%02Y %02H:%02M:%03.1f")
                    << endl;
         }
         catch(InvalidRequest& e)
         {
            if (verboseLevel)
               cerr << e.getText() << endl;
         }
      }
      orod.numSvs = orod.obs.size();
      oros << orod;
   }
   oros.close();
   iros.close();
}

int main(int argc, char *argv[])
{
  try
  {
     Stripper proc(argv[0]);
     if (!proc.initialize(argc, argv))
        return 1;
     if (!proc.run())
        return 1;
     return 0;
  }
  catch(gpstk::Exception& e)
  {
     cerr << e << endl;
  }
  catch(std::exception& e)
  {
     cerr << e.what() << endl;
  }
  catch(...)
  {
     cerr << "unknown exception" << endl;
  }
  return 1;
}
