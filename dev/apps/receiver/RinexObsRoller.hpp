#pragma ident "$Id$"


/**
 * @file RinexObsRoller.hpp
 * Distributes RINEX observations to a set of files.
 * Class declarations.
 */

#ifndef GPSTK_RINEX_OBS_ROLLER_HPP
#define GPSTK_RINEX_OBS_ROLLER_HPP

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

#include "RinexObsData.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsStream.hpp"
#include "Exception.hpp"
#include "DayTime.hpp"

namespace gpstk
{
   /** @addtogroup AshtechReceiver */
   //@{

   class RinexObsRoller
   {
   public:

      /// Constructor
      RinexObsRoller(const std::string& ifilespec, const RinexObsHeader& ihdrTemplate );
      
      /// Destructor
      virtual ~RinexObsRoller(void);

      bool write(const RinexObsData& rod, const DayTime& epoch=DayTime());

      std::string getCurrentFilename(void) const 
         { return currentFilename; }

      RinexObsStream& getStream(void) throw (Exception);

   protected:

   private:
      
      /// Pattern on which to create new files
      std::string filespec;
      
      /// Name of the current output file.
      std::string currentFilename;
      
      /// Stream to the current output file
      RinexObsStream *ofstrPtr;
             
         /// Tempate for creating new obs files
      RinexObsHeader headerTemplate;      
   }; // end class RinexObsRoller

   //@}

}  // end namespace gpstk

#endif // GPSTK_RINEX_OBS_ROLLER_HPP
