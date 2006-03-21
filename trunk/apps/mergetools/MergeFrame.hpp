#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/mergetools/MergeFrame.hpp#3 $"

#ifndef MERGEFRAME_HPP
#define MERGEFRAME_HPP

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






#include "BasicFramework.hpp"

/// Base class for writing utilities that merge files

class MergeFrame : public gpstk::BasicFramework
{
public:
      /**
       * arg0 is the name of the executable from argv[0].
       * type is a string with the type of file (i.e. "RINEX Obs").
       * message is an extra message that gets passed to the
       * program description.
       */
   MergeFrame(char* arg0, 
              const std::string& type, 
              const std::string& message = std::string())
         : gpstk::BasicFramework(arg0, 
                                 "Sorts and merges input " + type +
                                 " files into a single file. " + message),
         inputFileOption('i',
                         "input",
                         "An input " + type +
                         " file, can be repeated as many times as needed.",
                         true),
         outputFileOption('o',
                          "output",
                          "Name for the merged output " + type + " file. Any "
                          "existing file with that name will be overwritten.", 
                          true)
      {
         outputFileOption.setMaxCount(1);
      }

   virtual bool initialize(int argc, char* argv[]) throw()
      {
         return gpstk::BasicFramework::initialize(argc, argv);
      }

protected:
   virtual void process() = 0;

   gpstk::CommandOptionWithAnyArg inputFileOption;
   gpstk::CommandOptionWithAnyArg outputFileOption;
};


#endif
