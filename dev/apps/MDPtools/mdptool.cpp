#pragma ident "$Id$"


/** @file Various utility functions on MDP streams/files */

//lgpl-license START
//lgpl-license END

#include "StringUtils.hpp"
#include "LoopedFramework.hpp"
#include "CommandOptionWithTimeArg.hpp"

#include "TCPStream.hpp"

#include "MDPProcessors.hpp"
#include "SummaryProc.hpp"
#include "TrackProc.hpp"
#include "NavProc.hpp"

using namespace std;
using namespace gpstk;

class MDPTool : public gpstk::BasicFramework
{
public:
   MDPTool(const std::string& applName)
      throw()
      : BasicFramework(
         applName,
         "Perform various functions on a stream of MDP "
         "data. In the summary mode, the default is to only "
         "summarize the obs data above 10 degrees. Increasing "
         "the verbosity level will also summarize the data below "
         "10 degrees."),
        mdpInputOpt(
           'i', "input", 
           "Where to get the MDP data from. The default "
           "is to use stdin. If the file name begins with \"tcp:\" "
           "the remainder is assumed to be a hostname[:port] and the "
           "source is taken from a tcp socket at this address. If the "
           "port number is not specified a default of 8910 is used."),
        outputOpt(
           '\0', "output",
           "Where to send the output. The default is stdout."),
        styleOpt(
           's', "output-style",
           "What type of output to produce from the "
           "MDP stream. Valid styles are: brief, verbose, table, track, "
           "null, mdp, nav, and summary. The default is summary. Some "
           "modes aren't quite complete. Sorry."),
        pvtOpt('p', "pvt",  "Enable pvt output"),
        navOpt('n', "nav",  "Enable nav output"),
        tstOpt('t', "test", "Enable selftest output"),
        obsOpt('o', "obs",  "Enable obs output"),
        hexOpt('x', "hex",  "Dump all messages in hex"),
        badOpt('b', "bad",  "Try to process bad messages also."),
        bugMaskOpt('m', "bug-mask", "What RX bugs to be quite about."
                   "1 SV count, 2 nav parity/fmt, 4 HOW/hdr time equal."),
        almOpt(
           'a', "almanac",
           "Build and process almanacs. Only applies to the nav style"),
        ephOpt(
           'e', "ephemeris",
           "Build and process engineering ephemerides. Only applies to the "
           "nav style"),
        startTimeOpt(
           '\0', "start-time", "%4Y/%03j/%02H:%02M:%05.2f",
           "Ignore data before this time. (%4Y/%03j/%02H:%02M:%05.2f)"),
        stopTimeOpt(
           '\0',  "stop-time", "%4Y/%03j/%02H:%02M:%05.2f",
           "Ignore any data after this time"),
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
      if (!BasicFramework::initialize(argc,argv)) return false;

      if (debugLevel)
         cout << "debugLevel: " << debugLevel << endl
              << "verboseLevel: " << verboseLevel << endl;

      if (mdpInputOpt.getCount())
      {
         string ifn(mdpInputOpt.getValue()[0]);
         if (ifn.substr(0, 4) == "tcp:")
         {
            int port = 8910;
            ifn.erase(0,4);
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
               exit(-1);
            }
            mdpInput.std::basic_ios<char>::rdbuf(&rdbuf);
            mdpInput.filename = ifn;
         }
         else
         {
            mdpInput.open(mdpInputOpt.getValue()[0].c_str());
            if (debugLevel)
               cout << "Taking input from the file " << mdpInput.filename << endl;
         }
      }
      else
      {
         if (debugLevel)
            cout << "Taking input from stdin" << endl;
         mdpInput.copyfmt(std::cin);
         mdpInput.clear(std::cin.rdstate());
         mdpInput.std::basic_ios<char>::rdbuf(std::cin.rdbuf());
         mdpInput.filename = "<stdin>";
      }

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
         output.std::basic_ios<char>::rdbuf(std::cout.rdbuf());
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
      
      // Some nav specific options
      if (style == "nav")
      {
         processor->navOut = true;
         MDPNavProcessor& np=dynamic_cast<MDPNavProcessor&>(*processor);
         np.almOut = almOpt;
         np.ephOut = ephOpt;
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
   MDPStream mdpInput;
   ofstream output;
   TCPbuf rdbuf;
   gpstk::CommandOptionWithAnyArg mdpInputOpt, outputOpt;

   gpstk::CommandOptionNoArg pvtOpt, obsOpt, navOpt, tstOpt, hexOpt, badOpt;
   gpstk::CommandOptionNoArg almOpt, ephOpt;
   gpstk::CommandOptionWithAnyArg styleOpt;
   gpstk::CommandOptionWithNumberArg timeSpanOpt, bugMaskOpt;
   gpstk::CommandOptionWithTimeArg startTimeOpt, stopTimeOpt;

   string style;

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
