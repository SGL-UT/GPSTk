#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/receiver/RollingFile.hpp#1 $"

/**
 * @file RollingFile.hpp
 * Writes data to a file whose name is derived from a pattern and a nominal epoch. 
 * Class declarations.
 */

#ifndef GPSTK_ROLLING_FILE_HPP
#define GPSTK_ROLLING_FILE_HPP

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

#include <ostream>
#include <fstream>
#include <string>

#include "Exception.hpp"
#include "DayTime.hpp"

namespace gpstk
{
   /** @addtogroup AshtechReceiver */
   //@{

   class RollingFile
   {
   public:

      /// Constructor
      RollingFile(const std::string ifilespec);
      
      /// Destructor
      virtual ~RollingFile(void);

      bool write(const std::string& msg, const DayTime& epoch=DayTime());

      std::string getCurrentFilename(void) const 
         { return currentFilename; }

      std::ofstream& getStream(void) throw (Exception);

   protected:

   private:
      
      /// Pattern on which to create new files
      std::string filespec;
      
      /// Name of the current output file.
      std::string currentFilename;
      
      /// Stream to the current output file
      std::ofstream *ofstrPtr;
             
   }; // end class RollingFile

   //@}

}  // end namespace gpstk

#endif // GPSTK_ROLLING_FILE_HPP
