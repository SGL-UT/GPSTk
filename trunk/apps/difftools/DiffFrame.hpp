#pragma ident "$Id $"

#ifndef DIFFFRAME_HPP
#define DIFFFRAME_HPP

#include "CommandOptionWithTimeArg.hpp"
#include "BasicFramework.hpp"

class DiffFrame : public gpstk::BasicFramework
{
public:
      /**
       * arg0 is the name of the executable from argv[0].
       * type is a string with the type of file (i.e. "RINEX Obs").
       * message is an extra message that gets passed to the
       * program description.
       */
   DiffFrame(char* arg0, 
             const std::string& type, 
             const std::string& message = std::string())
         : gpstk::BasicFramework(arg0,
                                 "Diffs input " + type + " files. " + message),
           timeOption('t', "time", "Start of time range to compare (default ="
                      " \"beginning of time\")"),
           eTimeOption('e', "end-time", "End of time range to compare"
                       " (default = \"end of time\")"),
           inputFileOption("An input " + type + " file.", true),
           startTime(gpstk::DayTime::BEGINNING_OF_TIME),
           endTime(gpstk::DayTime::END_OF_TIME)
   {
      inputFileOption.setMaxCount(2);
      timeOption.setMaxCount(1);
      eTimeOption.setMaxCount(1);
      timeOptions.addOption(&timeOption);
      timeOptions.addOption(&eTimeOption);
   }

   virtual bool initialize(int argc, char* argv[]) throw()
   {
      if (!gpstk::BasicFramework::initialize(argc, argv))
      {
         return false;
      }

      if (inputFileOption.getCount() != 2)
      {
         std::cerr << "This program requires two input files." << std::endl;
         return false;
      }

      if (timeOption.getCount())
      {
         startTime = timeOption.getTime()[0];
      }

      if (eTimeOption.getCount())
      {
         endTime = eTimeOption.getTime()[0];
      }

      if (startTime > endTime)
      {
            std::cerr << "End time can't precede start time." << std::endl;
            return false;
      }
      return true;
   }

protected:
   virtual void process() = 0;
   
      /// start time for file record differencing
   gpstk::CommandOptionWithSimpleTimeArg timeOption;
      /// end time for file record differencing
   gpstk::CommandOptionWithSimpleTimeArg eTimeOption;
      /// if either of the time options are set
   gpstk::CommandOptionGroupOr timeOptions;
   gpstk::CommandOptionRest inputFileOption;

   gpstk::DayTime startTime, endTime;
};


#endif
