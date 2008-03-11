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

#include "StringUtils.hpp"
#include "LoopedFramework.hpp"
#include "CommandOption.hpp"

#include "MDPProcessors.hpp"
#include "ScreenProc.hpp"

using namespace std;
using namespace gpstk;

class MDPTool : public BasicFramework
{
public:
   MDPTool(const std::string& applName)
      throw()
      : BasicFramework(applName, "A curses based near-real-time display of an "
                       " MDP stream."),
        extraOpt(
           "File to process."),
        mdpInputOpt('i', "input", "Where to get the MDP data from. The default "
                    "is to use stdin. Use the rfw program to get input from "
                    "a device")
   {}

   bool initialize(int argc, char *argv[]) throw()
   {
      if (!BasicFramework::initialize(argc,argv)) 
         return false;

      if (debugLevel)
         cout << "debugLevel: " << debugLevel << endl
              << "verboseLevel: " << verboseLevel << endl;

      string fn;
      if (mdpInputOpt.getCount())
         fn =  mdpInputOpt.getValue()[0];
      else if (extraOpt.getCount())
         fn = extraOpt.getValue()[0];

      if (fn == "")
      {
         mdpInput.basic_ios<char>::rdbuf(cin.rdbuf());
         mdpInput.filename = "<stdin>";
      }
      else
      {
         ifstream *inputDev = new ifstream(fn.c_str(), ios::in);
         mdpInput.std::basic_ios<char>::rdbuf(inputDev->std::basic_ios<char>::rdbuf());
         mdpInput.filename = fn;
      }
      
      processor = new MDPScreenProcessor(mdpInput, output);

      processor->debugLevel = debugLevel;
      processor->verboseLevel = verboseLevel;

      MDPHeader::debugLevel = debugLevel;

      return true;
   }
   
protected:
   virtual void spinUp()
   {
      if (!processor)
         cout << "No processor assigned." << endl, exit(-1);
   }

   virtual void process()
   {      
      try
      {
         processor->process();
      }
      catch (gpstk::Exception &e) 
      {
         cout << e << endl;
      }
      catch (std::exception &e)
      {
         cout << e.what() << endl;
      }
   }
   
   virtual void shutDown()
   {
      delete processor;
   }

private:
   MDPStream mdpInput;
   ofstream output;
   CommandOptionWithAnyArg mdpInputOpt;

   CommandOptionRest extraOpt;

   MDPProcessor* processor;
};


int main(int argc, char *argv[])
{
try
   {
      MDPTool crap(argv[0]);

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
