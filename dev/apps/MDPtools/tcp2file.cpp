#pragma ident "$Id$"

/** @file reads a mdp stream and writes it to file(s)
 */

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

#include <StringUtils.hpp>
#include <LoopedFramework.hpp>
#include <CommandOption.hpp>
#include <TimeNamedFileStream.hpp>

#include "TCPStream.hpp"

using namespace std;
using namespace gpstk;



class MDP2File : public gpstk::BasicFramework
{
public:
   MDP2File(const std::string& applName) throw()
      : BasicFramework(applName,
                       "Reads data from a tcp socket and writes the data out to a TimeNamedFileStream."),
        output("tmp%03j_%04Y.raw", std::ios::app|std::ios::out)
   {}


   bool initialize(int argc, char *argv[]) throw()
   {
      CommandOptionWithAnyArg inputOpt(
         'i', "input", 
         "The hostname[:port] to connect to. If the "
         "port number is not specified a default of 8910 is used.");

      CommandOptionWithAnyArg outputSpecOpt(
         'o', "output",
         "The file spec for writing the files..");
      outputSpecOpt.setMaxCount(1);
      inputOpt.setMaxCount(1);

      if (!BasicFramework::initialize(argc,argv)) return false;

      if (debugLevel)
         cout << "debugLevel: " << debugLevel << endl
              << "verboseLevel: " << verboseLevel << endl;

      if (inputOpt.getCount())
      {
         int port=8910;
         string ifn(inputOpt.getValue()[0]);
         string::size_type i = ifn.find(":");
         if (i<ifn.size())
         {
            port = StringUtils::asInt(ifn.substr(i+1));
            ifn.erase(i);
         }
         if (debugLevel)
            cout << "Taking input from TCP socket at " << ifn
                 << ":" << port << endl;
         
         SocketAddr client(ifn, port);
         if (rdbuf.connect(client))
         {
            if (debugLevel)
               cout << "Conected to " << ifn << endl;
         }   
         else
         {
            cout << "Could not connect to " << ifn << endl;
            return false;
         }
         input.std::basic_ios<char>::rdbuf(&rdbuf);
      }
      else
      {
         cout << "Please specify source using the -i option." << endl;
         return false;
      }

      string spec = outputSpecOpt.getValue()[0];
      if (debugLevel)
         cout << "Using " << spec << " for output files" << endl;

      output.debugLevel = debugLevel;
      output.setFilespec(spec);

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
   ifstream input;
   TCPbuf rdbuf;

   // Use a ofstreams here since we don't want to use the MDP encode/decode
   // routines in saving the data. Yea it makes for more work but it makes
   // for a clean data path
   TimeNamedFileStream<ofstream> output;
};


int main(int argc, char *argv[])
{
   try
   {
      MDP2File crap(argv[0]);
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
