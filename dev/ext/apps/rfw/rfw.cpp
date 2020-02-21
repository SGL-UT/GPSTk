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
                       "Reads data from a stream and writes the data out to a"
                       "TimeNamedFileStream."),
        output("tmp%03j_%04Y.raw", std::ios::app|std::ios::out)
   {}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
   bool initialize(int argc, char *argv[]) throw()
   {
      CommandOptionWithAnyArg inputOpt(
         'i', "input", 
         "Where to get the data from. Can be a regular file, a serial "
         "device (ser:/dev/ttyS0), a tcp port (tcp:hostname:port), or "
         "standard input. The default is just to take standard input.");

      CommandOptionWithAnyArg passwordOpt(
         '\0', "password", 
         "Provide a login password to tcp device.");

      CommandOptionWithAnyArg usernameOpt(
         '\0', "username", 
         "Provide a login username to tcp device.");

      CommandOptionWithAnyArg sendStringOpt(
         's', "send-string",
         "A string to send to the device being recorded. For example to querry an "
         "Ashtech Z(Y)-12 for as-broadcast ephemeris use the following string: "
         "'$PASHQ,EPB'$'\\r\\n'");

      CommandOptionWithAnyArg sendPeriodOpt(
         'p', "send-period",
         "The time (in seconds) to pause between sending of the send-strings. "
         "If strings are specified, the default period is 60 seconds.");

      CommandOptionWithAnyArg outputSpecOpt(
         'o', "output",
         "The file spec for writing the files. To have the output "
         "go to stdout, specify - as the output file. The default file spec "
         "is tmp%03j_%04Y.raw");

      CommandOptionRest extraOpt("File to process.");

      outputSpecOpt.setMaxCount(1);
      inputOpt.setMaxCount(1);

      if (!BasicFramework::initialize(argc,argv)) return false;

      if (debugLevel)
         cout << "debugLevel: " << debugLevel << endl
              << "verboseLevel: " << verboseLevel << endl;

      string fn;
      if (inputOpt.getCount())
         fn = inputOpt.getValue()[0];
      else if (extraOpt.getCount())
         fn = extraOpt.getValue()[0];
      input.open(fn, ios::in);

      if (debugLevel)
         cout << "Taking input from " << input.getTarget() << endl;

      if (outputSpecOpt.getCount())
      {
         string spec = outputSpecOpt.getValue()[0];
         output.setFilespec(spec);
      }

      if (output.getFilespec() == "-")
         output.setFilespec("<stdout>");
      
      for (int i=0; i<sendStringOpt.getCount(); i++)
         sendString.push_back(sendStringOpt.getValue()[i]);
      
      for (int i=0; i<sendPeriodOpt.getCount(); i++)
         sendPeriod.push_back(StringUtils::asInt(sendPeriodOpt.getValue()[i]));

      for (int i=sendPeriod.size(); i< sendString.size(); i++)
         sendPeriod.push_back(60);

      output.debugLevel = debugLevel;

      if (passwordOpt.getCount())
         password = passwordOpt.getValue()[0];

      if (usernameOpt.getCount())
         username = usernameOpt.getValue()[0];

      if (debugLevel)
      {
         cout << "Using " << output.getFilespec() 
              << " for output files" << endl;
         if (username != "" || password != "")
            cout << "Sending username:" << username
                 << ", password:" << password
                 << " for login." << endl;
         for (int i=0; i<sendString.size(); i++)
         {
            cout << "Send period:" << sendPeriod[i] << endl;
            StringUtils::hexDumpData(cout, sendString[i]);
         }
      }

      return true;
   }
#pragma clang diagnostic pop
protected:
   virtual void spinUp()
   {}

   virtual void process()
   {
      const int sendSize=sendString.size();
      vector<CommonTime> lastSendTime(sendSize);

      bool use_stdout = output.getFilespec() == "<stdout>";
      const size_t max_len=512;
      char data[max_len];

      if (username != "" || password != "")
      {
         string str;
         while (input)
         {
            input.read(data, 1);
            str += data[0];
            if (str.find("login: ") != string::npos)
            {
               if (debugLevel)
                  cout << "got login prompt" << endl;
               input << username << endl;
               str = "";
            }
            if (str.find("Password: ") != string::npos)
            {
               if (debugLevel)
                  cout << "got password prompt" << endl;
               input << password << endl;
               break;
            }
         }
      }

      while (input)
      {
         input.read(data, max_len);
         if (debugLevel)
            cout << "process read: " << input.gcount() << " bytes read." << endl;
         if (use_stdout)
         {
            cout.write(data, input.gcount());
            cout.flush();
         }
         else
         {
            output.updateFileName();
            output.write(data, input.gcount());
            output.flush();
         }

         CommonTime now = SystemTime();
         for (int i=0; i<sendSize; i++)
         {
            if (now - lastSendTime[i] > sendPeriod[i])
            {
               if (debugLevel)
                  cout << "Sending: " << sendString[i] << endl;
               input.write(sendString[i].c_str(), sendString[i].size());
               lastSendTime[i] = now;
            }
         }
      }
   }

   virtual void shutDown()
   {}

private:
   DeviceStream<std::fstream> input;

   TimeNamedFileStream<ofstream> output;

   string username, password;

   vector<string> sendString;
   vector<int> sendPeriod;
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
