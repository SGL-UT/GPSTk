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
//  Copyright 2007, The University of Texas at Austin
//
//============================================================================

#ifndef CHECKFRAME_HPP
#define CHECKFRAME_HPP
#include <iostream>
#include <fstream>

#include "CommandOptionWithTimeArg.hpp"
#include "FileFilterFrame.hpp"
#include "BasicFramework.hpp"

template <class FileData>
struct NullTimeFilter : public std::unary_function<FileData, bool>
{
public:
   NullTimeFilter(const gpstk::CommonTime& startTime,
                  const gpstk::CommonTime& endTime)
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
         firstErrorOption('1', "quit-on-first-error", "Quit on the first"
                          " error encountered (default = no)."),
         timeOption('t', "time", "Time of first record to count (default ="
                    " \"beginning of time\")"),
         eTimeOption('e', "end-time", "End of time range to compare (default"
                     " = \"end of time\")"),
         inputFileOption("Each input file is checked for errors.", true),
         quitOnFirstError(false),
         startTime(gpstk::CommonTime::BEGINNING_OF_TIME),
         endTime(gpstk::CommonTime::END_OF_TIME)
   {
      timeOption.setMaxCount(1);
      eTimeOption.setMaxCount(1);
      timeOptions.addOption(&timeOption);
      timeOptions.addOption(&eTimeOption);
   }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
   virtual bool initialize(int argc, char* argv[]) throw()
   {
      if (!gpstk::BasicFramework::initialize(argc, argv))
         return false;
      if (firstErrorOption.getCount())
         quitOnFirstError = true;
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
#pragma clang diagnostic pop
protected:
   virtual void process()
   {
      unsigned errors = 0;
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
            ++errors;
            if (quitOnFirstError)
               GPSTK_RETHROW(e);
         }
         catch (std::exception& e)
         {
            std::cout << e.what() << std::endl;
            ++errors;
            if (quitOnFirstError)
               throw e;
         }
         catch (...)
         {
            std::cout << "unknown exception caught" << std::endl;
            ++errors;
            if (quitOnFirstError)
               throw;
         }
         
         itr++;
      }

      if (errors > 0)
      {
            // Throw an exception so the app returns 1 on any errors.
         gpstk::Exception exc("Encountered " + 
                              gpstk::StringUtils::asString(errors) +
                              " error(s).");
         GPSTK_THROW(exc);
      }
   }
   
      /// Quit on first error.
   gpstk::CommandOptionNoArg firstErrorOption;
      /// start time for record counting
   gpstk::CommandOptionWithSimpleTimeArg timeOption;
      /// end time for record counting
   gpstk::CommandOptionWithSimpleTimeArg eTimeOption;
      /// if either of the time options are set
   gpstk::CommandOptionGroupOr timeOptions;
   gpstk::CommandOptionRest inputFileOption;
   
   bool quitOnFirstError;
   gpstk::CommonTime startTime, endTime;
   
};

#endif
