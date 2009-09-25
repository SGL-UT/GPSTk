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

/** @file Various utility functions on MDP streams/files */

#include "StringUtils.hpp"
#include "LoopedFramework.hpp"
#include "CommandOptionWithTimeArg.hpp"

#include "MDPProcessors.hpp"
#include "SummaryProc.hpp"
#include "TrackProc.hpp"
#include "NavProc.hpp"
#include "SubframeProc.hpp"

using namespace std;
using namespace gpstk;

class MDPTool : public gpstk::BasicFramework
{
public:
   MDPTool(const std::string& applName)
      throw()
      : timeFormat("%4Y %3j %02H:%02M:%04.1f"),
        BasicFramework(
         applName,
         "Perform various functions on a stream of MDP "
         "data. In the summary mode, the default is to only "
         "summarize the obs data above 10 degrees. Increasing "
         "the verbosity level will also summarize the data below "
         "10 degrees."),
        mdpInputOpt(
           'i', "input", 
           "Where to get the MDP data from. The default is to use stdin."),
        followOpt(
           'f', "follow", 
           "Follow the input file as it grows."),
        outputOpt(
           '\0', "output",
           "Where to send the output. The default is stdout."),
        styleOpt(
           's', "output-style",
           "What type of output to produce from the "
           "MDP stream. Valid styles are: brief, verbose, table, track, "
           "null, mdp, nav, subframe, and summary. The default is summary. Some "
           "modes aren't quite complete. Sorry."),
        pvtOpt('p', "pvt",  "Enable pvt output"),
        navOpt('n', "nav",  "Enable nav output"),
        tstOpt('t', "test", "Enable selftest output"),
        obsOpt('o', "obs",  "Enable obs output"),
        hexOpt('x', "hex",  "Dump all messages in hex"),
        badOpt('b', "bad",  "Try to process bad messages also."),
        bugMaskOpt('m', "bug-mask", "What RX bugs to be quite about. "
                   "1 SV count, 2 nav parity/fmt, 4 HOW/hdr time equal."),
        almOpt(
           'a', "almanac",
           "Build and process almanacs. Only applies to the nav style"),
        ephOpt(
           'e', "ephemeris",
           "Build and process engineering ephemerides. Only applies to the "
           "nav style"),
        minimalAlmOpt(
           '\0', "min-alm",
           "This allows a complete almanac to be constructed from fewer than "
           "50 pages. It is required for receivers the Ashtech Z(Y)12. The "
           "default is to require all 50 pages."),
        startTimeOpt(
           '\0', "start-time", "%4Y/%03j/%02H:%02M:%05.2f",
           "Ignore data before this time. (%4Y/%03j/%02H:%02M:%05.2f)"),
        stopTimeOpt(
           '\0',  "stop-time", "%4Y/%03j/%02H:%02M:%05.2f",
           "Ignore any data after this time"),
        extraOpt(
           "File to process."),
        timeSpanOpt('l', "time-span", "How much data to process, in seconds")
   {
      pvtOpt.setMaxCount(1);
      navOpt.setMaxCount(1);
      obsOpt.setMaxCount(1);
      tstOpt.setMaxCount(1);
      styleOpt.setMaxCount(1);
   }

   bool initialize(int argc, char *argv[]) throw()
   {
      CommandOptionWithAnyArg timeFormatOpt(
         '\0', "time-format", "Daytime format specifier used for times in the "
         "output. The default is \""+timeFormat + "\".");

      using std::basic_ios;
      if (!BasicFramework::initialize(argc,argv)) return false;

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
         fn = "<stdin>";
         mdpInput.basic_ios<char>::rdbuf(cin.rdbuf());
      }
      else
      {
         inputDev.open(fn.c_str(), ios::in);
         mdpInput.basic_ios<char>::rdbuf(inputDev.basic_ios<char>::rdbuf());
      }
      mdpInput.filename = fn;

      if (debugLevel)
         cout << "Taking input from " << fn << endl;

      if (outputOpt.getCount())
      {
         output.open(outputOpt.getValue()[0].c_str(), std::ios::out);
         if (debugLevel)
            cout << "Sending output to" 
                 << outputOpt.getValue()[0]
                 << endl;
      }
      else
      {
         if (debugLevel)
            cout << "Sending output to stdout" << endl;
         output.copyfmt(std::cout);
         output.clear(std::cout.rdstate());
         output.basic_ios<char>::rdbuf(std::cout.rdbuf());
      }


      style = "summary";
      if (styleOpt.getCount())
         style = styleOpt.getValue()[0];

      if (style == "brief")
         processor = new MDPBriefProcessor(mdpInput, output);
      else if (style == "table")
         processor = new MDPTableProcessor(mdpInput, output);
      else if (style == "verbose")
         processor = new MDPVerboseProcessor(mdpInput, output);
      else if (style == "summary")
         processor = new MDPSummaryProcessor(mdpInput, output);
      else if (style == "null")
         processor = new MDPNullProcessor(mdpInput, output);
      else if (style == "track")
         processor = new MDPTrackProcessor(mdpInput, output);
      else if (style == "nav")
         processor = new MDPNavProcessor(mdpInput, output);
      else if (style == "subframe")
         processor = new MDPSubframeProcessor(mdpInput, output);
      else
      {
         cout << "Style " << style << " is not a valid style. (it may just not be implimented yet.)" << endl;
         return false;
      }
      
      if (debugLevel)
         cout << "Using style: " << style << endl;

      processor->pvtOut |= pvtOpt;
      processor->obsOut |= obsOpt;
      processor->navOut |= navOpt;
      processor->tstOut |= tstOpt;
      processor->processBad |= badOpt;
      if (timeFormatOpt.getCount())
      {
         timeFormat = timeFormatOpt.getValue()[0];
         processor->timeFormat = timeFormat;
      }
      
      // Some nav specific options
      if (style == "nav")
      {
         processor->navOut = true;
         processor->obsOut = true; // needed to know elevation/SNR of SVs
         MDPNavProcessor& np=dynamic_cast<MDPNavProcessor&>(*processor);
         np.almOut = almOpt;
         np.ephOut = ephOpt;
         np.minimalAlm = minimalAlmOpt;
      }
      else if (style == "subframe")
      {
         processor->navOut = true;
         processor->obsOut = true; // needed to know elevation/SNR of SVs
         
      }

      // If no outputs are specified, then at least set the obs output
      if (!processor->pvtOut && !processor->obsOut
          && !processor->navOut  && !processor->tstOut)
      {
         if (style == "summary")
            processor->obsOut = processor->pvtOut = processor->navOut = processor->tstOut = true;
         else
            processor->obsOut = true;
      }

      for (int i=0; i<bugMaskOpt.getCount(); i++)
         processor->bugMask |= StringUtils::asUnsigned(bugMaskOpt.getValue()[i]);
      
      if (followOpt.getCount())
         processor->followEOF = true;

      if (debugLevel)
      {
         string msgList;
         if (processor->pvtOut) msgList += "pvt ";
         if (processor->obsOut) msgList += "obs ";
         if (processor->navOut) msgList += "nav ";
         if (processor->tstOut) msgList += "tst ";
         if (msgList.size()==0)
            msgList = "no ";
         cout  << "Processing " << msgList << "messages." << endl;
         if (processor->followEOF)
            cout << "Following input as it grows" << endl;
      }

      processor->debugLevel = debugLevel;
      processor->verboseLevel = verboseLevel;

      MDPHeader::hexDump = hexOpt;
      MDPHeader::debugLevel = debugLevel;

      if (startTimeOpt.getCount())
         processor->startTime = startTimeOpt.getTime()[0];
      if (stopTimeOpt.getCount())
         processor->stopTime = stopTimeOpt.getTime()[0];
      if (timeSpanOpt.getCount())
         processor->timeSpan = StringUtils::asDouble(timeSpanOpt.getValue()[0]);

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
   ifstream inputDev;
   MDPStream mdpInput;
   ofstream output;
   gpstk::CommandOptionWithAnyArg mdpInputOpt, outputOpt;

   gpstk::CommandOptionNoArg pvtOpt, obsOpt, navOpt, tstOpt, hexOpt, badOpt;
   gpstk::CommandOptionNoArg almOpt, ephOpt, minimalAlmOpt;
   gpstk::CommandOptionNoArg followOpt;
   gpstk::CommandOptionWithAnyArg styleOpt;
   gpstk::CommandOptionWithNumberArg timeSpanOpt, bugMaskOpt;
   gpstk::CommandOptionWithTimeArg startTimeOpt, stopTimeOpt;

   gpstk::CommandOptionRest extraOpt;

   string style, timeFormat;

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
