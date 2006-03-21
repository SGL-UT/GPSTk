#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/checktools/CheckFrame.hpp#3 $"

#ifndef CHECKFRAME_HPP
#define CHECKFRAME_HPP

#include "CommandOptionWithTimeArg.hpp"
#include "FileFilterFrame.hpp"
#include "BasicFramework.hpp"

template <class FileData>
struct NullTimeFilter : public std::unary_function<FileData, bool>
{
public:
   NullTimeFilter(const gpstk::DayTime& startTime,
                  const gpstk::DayTime& endTime)
   {}

   bool operator() (const FileData& l) const
   {
      return false;
   }
};
 

template <class FileStream, class FileData, class FilterTimeOperator = NullTimeFilter<FileData> >
class CheckFrame : public gpstk::BasicFramework
{
public:
   CheckFrame(char* arg0, std::string fileType) :
         gpstk::BasicFramework(arg0,
                               "Reads given input " + fileType + 
                               " files and check for errors. This will only"
                               " report the first error found in each file. "
                               " The entire file is always checked, regardless"
                               " of time options."),
         timeOption('t', "time", "Time of first record to count (default ="
                    " \"beginning of time\")"),
         eTimeOption('e', "end-time", "End of time range to compare (default"
                     " = \"end of time\")"),
         inputFileOption("Each input file is checked for errors.", true),
         startTime(gpstk::DayTime::BEGINNING_OF_TIME),
         endTime(gpstk::DayTime::END_OF_TIME)
   {
      timeOption.setMaxCount(1);
      eTimeOption.setMaxCount(1);
      timeOptions.addOption(&timeOption);
      timeOptions.addOption(&eTimeOption);
   }
   
   virtual bool initialize(int argc, char* argv[]) throw()
   {
      if (!gpstk::BasicFramework::initialize(argc, argv))
         return false;
      if (timeOption.getCount())
         startTime = timeOption.getTime()[0];
      if (eTimeOption.getCount())
         endTime = eTimeOption.getTime()[0];
      if (startTime > endTime)
      {
         std::cerr << "End time can't precede start time." << std::endl;
         return false;
      }
      return true;
   }
   
protected:
   virtual void process()
   {
      std::vector<std::string> inputFiles = inputFileOption.getValue();
      std::vector<std::string>::iterator itr = inputFiles.begin();
      FilterTimeOperator timeFilt(startTime, endTime);
      while (itr != inputFiles.end())
      {
         std::cout << "Checking " << *itr << std::endl;
         unsigned long recCount = 0;
         try
         {
            FileStream f((*itr).c_str());
            f.exceptions(std::ios::failbit);
            
            FileData temp;
            while (f >> temp)
            {
               if (!timeFilt(temp))
                  recCount++;
            }
            
            std::cout << "Read " << recCount << " records." 
                      << std::endl << std::endl;
         }
         catch (gpstk::Exception& e)
         {
            std::cout << e << std::endl << std::endl;
         }
         catch (std::exception& e)
         {
            std::cout << e.what() << std::endl;
         }
         catch (...)
         {
            std::cout << "unknown exception caught" << std::endl;
         }
         
         itr++;
      }
   }
   
      /// start time for record counting
   gpstk::CommandOptionWithSimpleTimeArg timeOption;
      /// end time for record counting
   gpstk::CommandOptionWithSimpleTimeArg eTimeOption;
      /// if either of the time options are set
   gpstk::CommandOptionGroupOr timeOptions;
   gpstk::CommandOptionRest inputFileOption;
   
   gpstk::DayTime startTime, endTime;
   
};

#endif
