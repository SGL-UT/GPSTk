#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/MDPtools/mdpscreen/mdpscreen.cpp#3 $"

//lgpl-license START
//lgpl-license END

#include "StringUtils.hpp"
#include "LoopedFramework.hpp"
#include "CommandOption.hpp"

#include "TCPStream.hpp"

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
        mdpInputOpt('i', "input", "Where to get the MDP data from. The default "
                    "is to use stdin. If the file name begins with \"tcp:\" "
                    "the remainder is assumed to be a hostname[:port] and the "
                    "source is taken from a tcp socket at this address. If the "
                    "port number is not specified a default of 8910 is used.")
   {}

   bool initialize(int argc, char *argv[]) throw()
   {
      if (!BasicFramework::initialize(argc,argv)) 
         return false;

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
   TCPbuf rdbuf;
   CommandOptionWithAnyArg mdpInputOpt;

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
