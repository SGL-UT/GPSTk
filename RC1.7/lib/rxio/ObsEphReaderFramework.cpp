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

/**
 *  @file ObsEphReaderFramework.cpp
 */

#include "ObsEphReaderFramework.hpp"
#include <MSCData.hpp>
#include <MSCStream.hpp>
#include <GPSEphemerisStore.hpp>

using namespace std;

namespace gpstk
{
   using namespace StringUtils;

   template<class T> ostream& operator<<(ostream& ostr, const vector<T>& x)
   {
      using namespace std;
      for (typename vector<T>::const_iterator i = x.begin(); i!=x.end(); i++)
         ostr << *i << " ";
      return ostr;
   }
   
   bool ObsEphReaderFramework::initialize(int argc, char *argv[]) throw()
   {
      CommandOptionNoArg debugOption('d', "debug", "Increase debug level");
      CommandOptionNoArg verboseOption('v', "verbose", "Increase verbosity");
      CommandOptionNoArg helpOption('h', "help", "Print help usage");

      CommandOptionWithAnyArg obsFileOption(
         'o', "obs","Where to get the observation data. "
         "Supported formats are " + 
         ObsReader::formatsUnderstood() + ".", true);

      CommandOptionWithAnyArg ephFileOption(
         'e', "eph", "Where to get the Ephemeris data. Supported formats are " +
         EphReader::formatsUnderstood() + ".", true);

      CommandOptionWithAnyArg outputFileOption(
         '\0', "output", "A file to write the output to. The default is stdout.");
      outputFileOption.setMaxCount(1);

      CommandOptionWithAnyArg rxPosOption(
         'p', "position", "Receiver antenna position in ECEF "
         "(x,y,z) coordinates.  Format as a string: \"X Y Z\". "
         "Defaluts to estimating the position from the obs data "
         "unless a position can be read from the input.");
      rxPosOption.setMaxCount(1);

      CommandOptionWithAnyArg mscFileOption(
         'c', "msc", "Station coordinate file.");
      mscFileOption.setMaxCount(1);         

      CommandOptionWithAnyArg  msidOption(
         'm', "msid", "Station to process data for. Used to select "
         "a station position from the msc file.");
      msidOption.setMaxCount(1);

      CommandOptionNoArg searchNearOption('\0', "search-near", "Specify search near option when determining SV position with a broadcast ephemers. Usefull when ephemers data starts soon *after* the start of the observation data.");

      CommandOptionParser cop(appDesc);

      cop.parseOptions(argc, argv);

      if (helpOption.getCount())
      {
         cop.displayUsage(cout);
         exit(0);
      }
         
      if (cop.hasErrors())
      {
         cop.dumpErrors(cout);
         cop.displayUsage(cout);
         exit(0);
      }
      
      debugLevel = debugOption.getCount();
      verboseLevel = verboseOption.getCount();

      if (outputFileOption.getCount())
         outputFn = outputFileOption.getValue()[0];

      if (outputFn=="-" || outputFn=="")
      {
         output.copyfmt(cout);
         output.clear(cout.rdstate());
         output.ios::rdbuf(cout.rdbuf());
         outputFn = "<stdout>";
      }
      else
         output.open(outputFn.c_str(), ios::out);
      
      if (!output)
      {
         cout << "Could not open: " << outputFn << ". Terminating." << endl;
         exit(0);
      }

      obsFiles = obsFileOption.getValue();
      ephFiles = ephFileOption.getValue();

      if (verboseLevel)
         output << "Reading Observation data from: " << obsFiles << endl
                << "Reading Ephemeris data from: " << ephFiles << endl
                << "Writing output to " << outputFn << endl;


      // Read in all the ephemeris data
      ephReader.verboseLevel = verboseLevel;
      FFIdentifier::debugLevel = debugLevel;
      for (int i=0; i<ephFiles.size(); i++)
         ephReader.read(ephFiles[i]);
      gpstk::XvtStore<SatID>& eph = *ephReader.eph;

      bool searchNear = searchNearOption.getCount() > 0;
      if (searchNear && typeid(eph) == typeid(GPSEphemerisStore))
      {
         GPSEphemerisStore& bce = dynamic_cast<GPSEphemerisStore&>(eph);
         bce.SearchNear();
         if (verboseLevel)
            output << "Using SearchNear() for ephemers" << endl;
      }

      if (msidOption.getCount())
      {
         msid = msidOption.getValue()[0];
         if (verboseLevel)
            output << "Monitor station ID:" << msid << endl;
      }

      // Get the receiver position, trying the following sources, in order:
      // 1 command line position
      // 2 station ID + station coordinate file
      // 3 header information from first obs file
      // 4 computed from the first obs data file
      string rxPosSource;
      if (rxPosOption.getCount())
      {
         string aps = rxPosOption.getValue()[0];
         change(aps, ",", " ");
         if (numWords(aps) != 3)
         {
            output << "Please specify three coordinates in the antenna postion." << endl;
            return false;
         }
         else
         {
            rxPos = Position(asDouble(word(aps, 0)),
                             asDouble(word(aps, 1)),
                             asDouble(word(aps, 2)));
            rxPosSource = "command line";
         }
      }
      else if (msidOption.getCount() && mscFileOption.getCount())
      {
         string mscfn = mscFileOption.getValue()[0];
         MSCStream msc(mscfn.c_str(), ios::in);
         MSCData mscd;
         unsigned id = StringUtils::asUnsigned(msid);
         while (msc >> mscd && mscd.station != id)
            ;
         if (mscd.station == id)
         {
            rxPos = mscd.coordinates;
            rxPosSource = "msc file";
         }
      }
      else
      {
         string fn = (obsFileOption.getValue())[0];
         ObsReader obsReader(fn, debugLevel);
         if (obsReader.inputType == FFIdentifier::tRinexObs)
         {
            rxPos = obsReader.roh.antennaPosition;
            rxPosSource = "rinex obs header";
         }
         else
         {
            rxPosSource = "esitmated from obs and ephemers data";
            output << "code me " << endl;
            exit(0);
         }
      }

      if (verboseLevel)
         output << "Receier position : " << rxPos
                << " (from " << rxPosSource << ")" << endl;

      if (rxPos.radius() < 1)
         output << "Warning! The antenna appears to be within one meter of the" << endl
                << "center of the geoid.  If this location is correct, the " << endl
                << "antenna is probably no longer functional." << endl;

      return true;
   }

} // namespace gpstk
