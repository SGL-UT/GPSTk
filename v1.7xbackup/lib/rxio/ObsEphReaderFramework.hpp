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

#ifndef GPSTK_OBSEPHREADERFRAMEWORK_HPP
#define GPSTK_OBSEPHREADERFRAMEWORK_HPP

/**
 *  @file ObsEphReaderFramework.hpp
 */

#include <fstream>
#include <string>

#include "CommandOptionParser.hpp"
#include "ObsReader.hpp"
#include "EphReader.hpp"
#include "Position.hpp"

namespace gpstk
{
   // This object is intened to be created in place of a CommandOptionParser
   class ObsEphReaderFramework
   {
   public:
      
      ObsEphReaderFramework(const std::string& applDesc)
         throw()
         : debugLevel(0), verboseLevel(0), appDesc(applDesc)
      {}

      bool initialize(int argc, char *argv[]) throw();

      int debugLevel;          ///< Debug level for this run of the program.
      int verboseLevel;        ///< Verbose level for this run of the program.
      std::string appDesc;     ///< Description of program's function.
      std::string outputFn;    ///< Name of the output stream
      std::ofstream output;    ///< Use this for all output

      Position rxPos;
      std::string msid;

      std::vector<std::string> obsFiles;
      std::vector<std::string> ephFiles;

      EphReader ephReader;
      
   private:
      // Do not allow the use of the default constructor.
      ObsEphReaderFramework();
   };

} // namespace gpstk

#endif
