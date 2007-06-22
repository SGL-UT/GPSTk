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

/** @file reads a stream and writes it to file(s) with names derived from
    system time.
 */

#include <fstream>

#include <unistd.h>
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#include <StringUtils.hpp>
#include <LoopedFramework.hpp>
#include <CommandOption.hpp>
#include <TimeNamedFileStream.hpp>

#include "DeviceStream.hpp"

using namespace std;
using namespace gpstk;
class RollingFileWriter : public gpstk::BasicFramework
{
public:
   RollingFileWriter(const std::string& applName) throw()
      : BasicFramework(applName,
                       "Reads data from a stream and writes the data out to a TimeNamedFileStream."),
        output("tmp%03j_%04Y.raw", std::ios::app|std::ios::out)
   {}


   bool initialize(int argc, char *argv[]) throw()
   {
      CommandOptionWithAnyArg inputOpt(
         'i', "input", 
         "Where to get the data from. Can be a regular file, a serial "
         "device (ser:/dev/ttyS0), a tcp port (tcp:hostname:port), or "
         "standard input. The default is just to take standard input.");

      CommandOptionWithAnyArg outputSpecOpt(
         'o', "output",
         "The file spec for writing the files..");

      outputSpecOpt.setMaxCount(1);
      inputOpt.setMaxCount(1);

      if (!BasicFramework::initialize(argc,argv)) return false;

      if (debugLevel)
         cout << "debugLevel: " << debugLevel << endl
              << "verboseLevel: " << verboseLevel << endl;

      string fn;
      if (inputOpt.getCount())
         fn = inputOpt.getValue()[0];
      input.open(fn, ios::in);

      if (debugLevel)
         cout << "Taking input from " << input.getTarget() << endl;


      if (outputSpecOpt.getCount())
      {
         string spec = outputSpecOpt.getValue()[0];
         output.setFilespec(spec);
      }

      output.debugLevel = debugLevel;

      if (debugLevel)
         cout << "Using " << output.getFilespec() 
              << " for output files" << endl;

      return true;
   }
   
protected:
   virtual void spinUp()
   {}

   virtual void process()
   {
      const size_t max_len=512;
      char data[max_len];
      while (input)
      {
         input.read(data, max_len);
         output.updateFileName();
         output.write(data, input.gcount());
         output.flush();
      }
   }

   virtual void shutDown()
   {}

private:
   DeviceStream<std::ifstream> input;

   TimeNamedFileStream<ofstream> output;
};


int main(int argc, char *argv[])
{
   try
   {
      RollingFileWriter crap(argv[0]);
      if (!crap.initialize(argc, argv))
         exit(0);
      crap.run();
   }
   catch (gpstk::Exception &exc)
   { cout << exc << endl; }
   catch (std::exception &exc)
   { cout << "Caught std::exception " << exc.what() << endl; }
   catch (...)
   { cout << "Caught unknown exception" << endl; }
}
