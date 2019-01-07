//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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

// Standard library includes
#include <fstream>

// GPSTk main library includes
#include "BasicFramework.hpp"
#include "WGS84Geoid.hpp"
#include "GNSSconstants.hpp"
#include "IonoModel.hpp"
#include "SystemTime.hpp"
#include "TimeString.hpp"

// From GPSTk's rxio
#include "EphReader.hpp"

// From GPSTk's geomatics
#include "random.hpp"

using namespace std;
using namespace gpstk;


class MPSim : public BasicFramework
{
public:

   // Linear relation between iono delay on L1 and L2.
   static const double gamma;

   MPSim (char *arg0);
   ~MPSim();

   virtual bool initialize(int argc, char *argv[]) throw();

protected:
   virtual void process();

   string logFileName;
   string epochFormat;

   CommonTime startTime;

   CommandOptionWithAnyArg navOption;
   CommandOptionWithAnyArg logfileOption;
   CommandOptionWithAnyArg rateOption;

   ofstream logStream;

   double outputRate;

};

MPSim::MPSim(char *arg0)
  : BasicFramework(arg0, "GPSTk ground multipath simulation."),
    navOption('n', "nav-file", "Input file containing almanac or ephemeris, defining the orbit and clocks. Formats understood: "+EphReader::formatsUnderstood()+". Repeat for multiple files, but files must be of the same type." , true),
    logfileOption('g',"logfile","Write logfile to this file." ),
    rateOption('r',"rate","Observation interval (default=30s)",false)
  {
    rateOption.setMaxCount(1);
    logfileOption.setMaxCount(1);

    startTime = SystemTime();

    logFileName = "logfile";
    epochFormat="%02m/%02d/%04Y %02H:%02M:%03.1f";

    outputRate = 30.;
  }

MPSim::~MPSim()
{
   CommonTime endTime = SystemTime();
   logStream << "Execution end at: " << endTime.printf(epochFormat) << endl;
   logStream << "Total execution time: " << endTime - startTime << " seconds"
	     << endl;
}

bool MPSim::initialize(int argc, char *argv[])
    throw()
  {
    if(!BasicFramework::initialize(argc, argv))
      return false;

    if (logfileOption.getCount()>0)
    {
       logFileName = StringUtils::asString(logfileOption.getValue().front());
    }

    logStream.open( logFileName.c_str() );

    logStream << "mpsim log file" << endl;
    logStream << "Execution started at: " << printString(startTime, epochFormat) << endl;

    return true;
  }


void MPSim::process()
{
    IonoModel spsIonoCorr;

    // Read nav file and store unique list of ephemeredes
    if (navOption.getCount()>0)
    {
       EphReader ephs;
       for (size_t i=0; i<navOption.getCount(); i++)
       {
	 ephs.read(navOption.getValue()[i].c_str());
       }

       logStream << "Read these input files for ephemeris or almanac: " << endl;
       for (size_t i=0; i<ephs.filesRead.size(); i++)
	 logStream << "  " << ephs.filesRead[i] << endl;

    }


    if (rateOption.getCount()>0)
    {
       outputRate = StringUtils::asFloat(rateOption.getValue().front());
    }

    logStream << "Observation interval set to " << outputRate << " s" << endl;
}

const double MPSim::gamma = (L1_FREQ / L2_FREQ)*(L1_FREQ / L2_FREQ);


int main(int argc, char *argv[])
{
  try
    {
      MPSim mpsim(argv[0]);

      if(!mpsim.initialize(argc, argv))
	return 0;

      if(!mpsim.run())
	return 1;

      return 0;
    }
  catch(Exception& exc)
    {
      cout << exc << endl;
    }
  catch(...)
    {
      cout << "Caught an unknown exception." << endl;
    }
  return 1;
}
