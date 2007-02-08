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


#include <MSCData.hpp>
#include <MSCStream.hpp>

#include "OrdApp.hpp"
#include "OrdEngine.hpp"
#include "ObsReader.hpp"
#include "EphReader.hpp"
#include "MetReader.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

class OrdGen : public OrdApp
{
public:
   OrdGen() throw();
   
   bool initialize(int argc, char *argv[]) throw();

protected:
   virtual void spinUp();
   virtual void process();

private:
   string ordMode;
   Triple antennaPos;
   unsigned msid;

   CommandOptionWithAnyArg obsFileOption, ephFileOption, metFileOption;
};

//-----------------------------------------------------------------------------
// The constructor basically just sets up all the defaults
//-----------------------------------------------------------------------------
OrdGen::OrdGen() throw()
   : OrdApp("ordGen", "Generates observed range deviations."),
     ordMode("smart"), 
     msid(0),
     obsFileOption('o', "obs", "Where to get the obs data.", true),
   
     ephFileOption('e', "eph",  "Where to get the ephemeris data. Can be "
                   " rinex, fic, or sp3", true),

     metFileOption('w', "weather", "Weather data file name (RINEX met "
                    "format only).")
{}


//-----------------------------------------------------------------------------
// Here the command line options are set up, parsed, and used to configure
// the program.
//-----------------------------------------------------------------------------
bool OrdGen::initialize(int argc, char *argv[]) throw()
{
   CommandOptionWithAnyArg
      mscFileOption('c', "msc", "Station coordinate file."),
   
   
      ordModeOption('\0', "omode", "Specifies what observations are used to "
                    "compute the ORDs. Valid values are:"
                    "p1p2, c1p2, y1y2, c1, p1, c2, p2, smo, and smart. "
                    "The default is " + ordMode),
   
      antennaPosOption('p', "pos", "Location of the antenna in meters ECEF.");
   
   CommandOptionWithNumberArg 
      msidOption('m', "msid", "Station to process data for. Used to "
                 "select a station position from the msc file or data "
                 "from a SMODF file.");

   if (!OrdApp::initialize(argc,argv)) return false;

   if (ordModeOption.getCount())
      ordMode = lowerCase(ordModeOption.getValue()[0]);

   if (msidOption.getCount())
      msid = asUnsigned(msidOption.getValue().front());

   // Get the station position
   if (antennaPosOption.getCount())
   {
      string aps = antennaPosOption.getValue()[0];
      if (numWords(aps) != 3)
      {
         cerr << "Please specify three coordinates in the antenna postion." << endl;
         return false;
      }
      else
         for (int i=0; i<3; i++)
            antennaPos[i] = asDouble(word(aps, i));
   }
   else if (msid && mscFileOption.getCount() > 0)
   {
      string mscfn = (mscFileOption.getValue())[0];
      MSCStream msc(mscfn.c_str(), ios::in);
      MSCData mscd;
      while (msc >> mscd && mscd.station != msid)
         ;
      if (mscd.station == msid)
         antennaPos = mscd.coordinates;
   }
   else
   {
      string fn = (obsFileOption.getValue())[0];
      ObsReader obsReader(fn, verboseLevel);
      if (obsReader.inputType == FFIdentifier::tRinexObs)
         antennaPos = obsReader.roh.antennaPosition;
   }

   if (RSS(antennaPos[0], antennaPos[1], antennaPos[2]) < 1)
   {
      cerr << "Warning! The antenna appears to be within one meter of the" << endl
           << "center of the geoid. This program is not capable of" << endl
           << "accurately estimating the propigation of GNSS signals" << endl
           << "through solids such as a planetary crust or magma. Also," << endl
           << "if this location is correct, your antenna is probally" << endl
           << "no longer in the best of operating condition." << endl;
      return false;
   }

   return true;
}


//-----------------------------------------------------------------------------
// General program setup
//-----------------------------------------------------------------------------
void OrdGen::spinUp()
{
   if (verboseLevel)
   {
      if (msid)
         cout << "# msid: " << msid << endl;
      cout << "# Antenna Position: " << setprecision(8) << antennaPos << endl;
   }
}


//-----------------------------------------------------------------------------
void OrdGen::process()
{
   // This is only needed to help debug the FFIdentifer class
   FFIdentifier::debugLevel = debugLevel;

   // Get the ephemeris data
   EphReader ephReader;
   ephReader.verboseLevel = verboseLevel;
   for (int i=0; i<ephFileOption.getCount(); i++)
      ephReader.read(ephFileOption.getValue()[i]);
   gpstk::EphemerisStore& eph = *ephReader.eph;

   // Get the weather data...
   MetReader metReader;
   metReader.verboseLevel = verboseLevel;
   for (int i=0; i<metFileOption.getCount(); i++)
      metReader.read(metFileOption.getValue()[i]);
   WxObsData& wod = metReader.wx;

   // Use a New Brunswick trop model.
   NBTropModel tm;

   // Now set up the function object that is used to compute the ords.
   OrdEngine ordEngine(eph, wod, antennaPos, ordMode, tm);
   ordEngine.verboseLevel = verboseLevel;
   ordEngine.debugLevel = debugLevel;
   ORDEpochMap ordEpochMap;

   // Walk through each obs file, reading and computing ords along the way.
   for (int i=0; i<obsFileOption.getCount(); i++)
   {
      string fn = (obsFileOption.getValue())[i];
      ObsReader obsReader(fn, verboseLevel);
      obsReader.msid = msid;

      while (obsReader())
      {
         ObsEpoch obs(obsReader.getObsEpoch());

         if (!obsReader())
            break;

         ORDEpoch oe = ordEngine(obs);

         write(output, oe);
      }
   }
}


//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      OrdGen crap;
      if (!crap.initialize(argc, argv))
         exit(0);
      crap.run();
   }
   catch (gpstk::Exception &exc)
   { cerr << exc << endl; }
   catch (std::exception &exc)
   { cerr << "Caught std::exception " << exc.what() << endl; }
   catch (...)
   { cerr << "Caught unknown exception" << endl; }
}
